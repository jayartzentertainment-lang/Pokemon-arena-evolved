#include "game.h"
#include "arena/vendor/arena_navigation.h"
#include "arena/vendor/arena_moves.h"
#include "arena/vendor/constants/moves.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static ArenaGame game;
static GameInput None(void){GameInput in={0};in.touchMove=-1;return in;}
static void Setup(int doubles)
{
    unsigned i;Game_Start(&game,doubles);game.combat.aiEnabled=0;
    for(i=0;i<ARENA_OBSTACLES;i++)ArenaNav_SetObstacle(i,0);
    game.combat.fighters[0].x=100*256;game.combat.fighters[0].y=150*256;
    game.combat.fighters[1].x=280*256;game.combat.fighters[1].y=150*256;
    game.combat.fighters[3].x=240*256;game.combat.fighters[3].y=230*256;
}
static void StrafeAndTargets(void)
{
    GameInput in=None();unsigned i;int32_t y;uint32_t serial;
    Setup(1);y=game.combat.fighters[0].y;in.axisY=-156;
    for(i=0;i<15;i++)Game_Tick(&game,in);
    assert(game.combat.fighters[0].y<y&&game.combat.aimX>200&&game.combat.aimY>0);
    assert(Combat_TargetFighter(&game.combat,0)==1);
    serial=game.combat.fighters[0].actionSerial;in=None();in.held=GAME_R|GAME_Y;in.pressed=GAME_Y;
    Game_Tick(&game,in);Game_Tick(&game,None());
    assert(game.aim.target==3&&Combat_TargetFighter(&game.combat,0)==3);
    assert(game.switches==0&&game.combat.fighters[0].actionSerial==serial);
    game.combat.fighters[3].hp=0;Game_AimTick(&game,None());assert(game.aim.target==1);
}
static void FreeAimAndProjectile(void)
{
    GameInput in=None();unsigned i;CombatProjectile *p=0;
    Setup(0);in.pressed=GAME_SELECT;in.axisX=41;in.axisY=-17;Game_Tick(&game,in);
    assert(game.aim.mode==GAME_AIM_FREE&&game.combat.aimX>200&&game.combat.aimY<0);
    in=None();Game_Tick(&game,in);
    assert(game.aim.mode==GAME_AIM_FREE&&game.combat.aimY<0);
    in=None();in.touchMove=0;Game_Tick(&game,in);in=None();
    for(i=0;i<100;i++){
        unsigned j;Game_Tick(&game,in);
        for(j=0;j<COMBAT_PROJECTILES;j++)if(game.combat.projectiles[j].life){p=&game.combat.projectiles[j];break;}
        if(p)break;
    }
    assert(p&&p->vx>0&&p->vy<0);
    assert(abs(p->vx*17+p->vy*41)<100); /* Continuous, not a snapped diagonal. */
    in.pressed=GAME_SELECT;Game_Tick(&game,in);assert(game.aim.mode==GAME_AIM_LOCK);
}
static void StatusRoots(void)
{
    static const unsigned moves[]={MOVE_SWORDS_DANCE,MOVE_AGILITY,MOVE_CALM_MIND,MOVE_IRON_DEFENSE};
    static const unsigned stats[]={COMBAT_STAT_ATTACK,COMBAT_STAT_SPEED,COMBAT_STAT_SP_ATTACK,COMBAT_STAT_DEFENSE};
    unsigned n;
    for(n=0;n<4;n++){
        GameInput in=None();CombatFighter *f;unsigned i;int32_t x,y;int applied=0;
        Setup(0);assert(Game_Switch(&game,0));f=&game.combat.fighters[0];
        f->moves[0]=(uint16_t)moves[n];f->pp[0]=Combat_GetMoveInfo((uint16_t)moves[n])->pp;
        x=f->x;y=f->y;in.axisX=156;in.touchMove=0;Game_Tick(&game,in);in.touchMove=-1;
        assert(Combat_IsCasting(f));
        for(i=0;i<300&&f->phase!=COMBAT_READY;i++){
            assert(f->x==x&&f->y==y&&f->vx==0&&f->vy==0);
            if(f->releaseSerial)applied=1;
            if(i==2){in.held=GAME_B|GAME_RIGHT;in.pressed=GAME_B;}
            else {in.held=GAME_RIGHT;in.pressed=0;}
            Game_Tick(&game,in);
        }
        assert(applied&&f->phase==COMBAT_READY&&!f->castRoot&&f->stages[stats[n]]>0);
        for(i=0;i<8;i++)Game_Tick(&game,in);
        assert(f->x>x);
        if(n==2)assert(f->stages[COMBAT_STAT_SP_DEFENSE]==1);
    }
}
static void InterruptionAndDebuff(void)
{
    GameInput in=None();CombatFighter *f;unsigned i;
    Setup(0);assert(Game_Switch(&game,0));f=&game.combat.fighters[0];
    in.touchMove=3;Game_Tick(&game,in);assert(Combat_IsCasting(f));
    Combat_SetNativeStatus(&game.combat,0,3);assert(!f->castRoot);
    for(i=0;i<80;i++)Game_Tick(&game,None());
    assert(f->stages[COMBAT_STAT_ATTACK]==0); /* Interrupted before release. */
    Setup(0);assert(Game_Switch(&game,0));f=&game.combat.fighters[0];
    f->moves[0]=MOVE_GROWL;f->pp[0]=40;game.combat.fighters[1].x=138*256;
    in=None();in.touchMove=0;Game_Tick(&game,in);
    assert(Combat_IsCasting(f));
    for(i=0;i<120;i++)Game_Tick(&game,None());
    assert(game.combat.fighters[1].stages[COMBAT_STAT_ATTACK]==-1);
    assert(!game.combat.fighters[1].castRoot);
}
static void BeamReachAndSteer(void)
{
    GameInput in=None();CombatFighter *f;unsigned i,first=0;
    Setup(0);assert(Game_Switch(&game,0));assert(Game_Switch(&game,0));
    f=&game.combat.fighters[0];f->x=100*256;f->y=200*256;
    game.combat.fighters[1].x=100*256;game.combat.fighters[1].y=110*256;
    in.pressed=GAME_SELECT;in.axisX=0;in.axisY=-100;in.touchMove=0;Game_Tick(&game,in);in.touchMove=-1;in.pressed=0;
    for(i=0;i<100&&f->phase!=COMBAT_ACTIVE;i++)Game_Tick(&game,in);
    assert(f->phase==COMBAT_ACTIVE);first=f->beamReach;
    assert(first>0&&first<=21&&game.combat.hits==0);
    in.axisX=100;in.axisY=0;Game_Tick(&game,in);
    assert(f->attackAimX>0&&f->attackAimX<25&&f->attackAimY< -240);
    assert(f->beamReach>=first);
    /* Switching interrupts ownership and discards the old emitter/action. */
    assert(Game_Switch(&game,0));assert(!game.combat.fighters[0].beamReach);
    for(i=0;i<COMBAT_BEAM_CLASHES;i++)assert(!game.combat.beamClashes[i].phase);
}
static void TouchSeparation(void)
{
    unsigned x,y,i;
    for(y=0;y<240;y++)for(x=0;x<320;x++)
        assert(!(Game_TouchMove(x,y)>=0&&Game_TouchCommand(x,y)>=0));
    for(i=0;i<4;i++){
        int x0,y0,w,h;Game_MoveRect(i,&x0,&y0,&w,&h);
        assert(w>=140&&h>=40&&x0>=0&&x0+w<=320&&y0+h<=240);
        assert(Game_TouchMove(x0+w/2,y0+h/2)==(int)i);
    }
    for(i=0;i<5;i++){
        int x0,y0,w,h;Game_CommandRect(i,&x0,&y0,&w,&h);
        assert(w>=96&&h>=30&&x0>=0&&x0+w<=320&&y0+h<=240);
        assert(Game_TouchCommand(x0+w/2,y0+h/2)==(int)i);
    }
    assert(Game_TouchMove(160,120)<0&&Game_TouchCommand(160,120)<0);
}
static void NativeClash(void)
{
    GameInput in=None();unsigned i;int32_t x,y;
    Setup(0);assert(Game_Switch(&game,0));assert(Game_Switch(&game,0));
    assert(Game_Switch(&game,1));assert(Game_Switch(&game,1));assert(Game_Switch(&game,1));
    assert(game.combat.fighters[0].species==6&&game.combat.fighters[1].species==6);
    game.combat.fighters[0].x=100*256;game.combat.fighters[1].x=280*256;
    game.combat.fighters[0].y=game.combat.fighters[1].y=150*256;
    assert(Combat_QueueMove(&game.combat,1,0));in.touchMove=0;
    Game_Tick(&game,in);in.touchMove=-1;
    for(i=0;i<90&&!game.combat.clashes;i++)Game_Tick(&game,in);
    assert(game.combat.clashes==1&&game.combat.hits==0);
    assert(Combat_BeamContact(&game.combat,0,&x,&y));
    {int32_t ox,oy;int16_t ax,ay;Combat_AttackOrigin(&game.combat.fighters[0],&ox,&oy);Combat_AttackVector(&game.combat.fighters[0],&ax,&ay);
     assert(x>100*256&&x<280*256);
     /* Contact follows the registered body-height beam, not the foot plane. */
     assert(llabs((int64_t)(x-ox)*ay-(int64_t)(y-oy)*ax)<=2LL*256*256);}
    assert(Game_Switch(&game,0));assert(!Combat_BeamContact(&game.combat,0,0,0));
    assert(!Combat_BeamContact(&game.combat,1,0,0));
}
static void AICastRoot(void)
{
    GameInput in=None();CombatFighter *enemy;unsigned i;int32_t x,y;
    Setup(0);assert(Game_Switch(&game,1));assert(Game_Switch(&game,1));
    enemy=&game.combat.fighters[1];enemy->moves[0]=MOVE_SWORDS_DANCE;enemy->pp[0]=30;
    enemy->aiCooldown=255;game.combat.aiEnabled=1;x=enemy->x;y=enemy->y;
    assert(Combat_QueueMove(&game.combat,1,0));Game_Tick(&game,in);
    assert(Combat_IsCasting(enemy));
    for(i=0;i<200&&enemy->phase!=COMBAT_READY;i++){
        assert(enemy->x==x&&enemy->y==y);Game_Tick(&game,in);
    }
    assert(enemy->stages[COMBAT_STAT_ATTACK]==2&&!enemy->castRoot);
}
static void ExplicitAimOnly(void)
{
    GameInput in=None();int16_t x,y;unsigned i;
    Setup(1);in.axisX=-156;Game_AimTick(&game,in);
    assert(game.aim.mode==GAME_AIM_LOCK&&game.combat.aimX>0);
    game.combat.fighters[1].y=240*256;Game_AimTick(&game,in);
    assert(game.aim.target==1&&game.combat.aimX>0&&game.combat.aimY>0);
    in=None();in.pressed=GAME_SELECT;Game_AimTick(&game,in);
    assert(game.aim.mode==GAME_AIM_FREE);x=game.combat.aimX;y=game.combat.aimY;
    game.combat.fighters[1].x=20*256;game.combat.fighters[1].y=20*256;
    for(i=0;i<30;i++){in=None();in.axisX=3;in.axisY=-3;Game_AimTick(&game,in);}
    assert(game.combat.aimX==x&&game.combat.aimY==y); /* Neutral drift never retargets. */
    in=None();in.held=GAME_DOWN;Game_AimTick(&game,in);
    assert(game.combat.aimX==0&&game.combat.aimY==256);
    game.combat.fighters[1].hp=0;Game_AimTick(&game,None());
    assert(game.aim.mode==GAME_AIM_FREE&&game.combat.aimY==256);
    in=None();in.pressed=GAME_SELECT;Game_AimTick(&game,in);
    assert(game.aim.mode==GAME_AIM_LOCK&&game.aim.target==3);
    game.combat.fighters[3].hp=0;Game_AimTick(&game,None());
    assert(game.aim.target==COMBAT_FIGHTERS);
}
int main(void)
{
    StrafeAndTargets();FreeAimAndProjectile();StatusRoots();InterruptionAndDebuff();
    BeamReachAndSteer();TouchSeparation();NativeClash();AICastRoot();ExplicitAimOnly();
    puts("PASS quality: strafing, target cycling/invalidation, sticky lock and single-pad free aim, projectile vectors, four rooted buffs, interruption, Growl, growing beams, bounded steering, ownership cleanup and 76800 touch coordinates");
    return 0;
}
