#include "game.h"
#include "arena/vendor/arena_navigation.h"
#include "arena/vendor/constants/moves.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static ArenaGame game;
static GameInput None(void){GameInput in={0};in.touchMove=-1;return in;}
static void Setup(unsigned one,unsigned two,unsigned move)
{
    CombatFighterConfig configs[2][6]={0};GamePartySlot state[2][6]={0};uint8_t counts[2]={1,1};
    unsigned t,m;CombatShape a,b;
    configs[0][0]=ArenaSpecies_Find(one)->config;configs[1][0]=ArenaSpecies_Find(two)->config;
    if(move)configs[0][0].moves[0]=(uint16_t)move;
    for(t=0;t<2;t++){
        state[t][0].hp=configs[t][0].hp;state[t][0].stamina=100;
        for(m=0;m<4;m++)state[t][0].pp[m]=Combat_GetMoveInfo(configs[t][0].moves[m])->pp;
    }
    Game_StartParty(&game,configs,counts,state,0);game.combat.aiEnabled=0;game.combat.random=0;
    for(t=0;t<ARENA_OBSTACLES;t++)ArenaNav_SetObstacle((uint8_t)t,0);
    game.combat.fighters[0].x=100*256;game.combat.fighters[0].y=160*256;
    game.combat.fighters[1].x=340*256;game.combat.fighters[1].y=160*256;
    Combat_Hurtbox(&game.combat.fighters[0],&a);Combat_Hurtbox(&game.combat.fighters[1],&b);
    game.combat.fighters[1].y+=a.y0-b.y0;
}
static void Start(void){GameInput in=None();in.touchMove=0;Game_Tick(&game,in);}
static void Gaits(void)
{
    static const unsigned species[]={25,448,9,94,493};unsigned k,fast=0,heavy=0;
    for(k=0;k<5;k++){
        CombatFighter *f;GameInput in=None();unsigned i,cycles=0,lastPhase=0;int32_t start,cycleStart;
        Setup(species[k],9,0);f=&game.combat.fighters[0];start=cycleStart=f->x;
        assert(f->motion.cycle==ArenaSpecies_Find(species[k])->animation[COMBAT_WALK].totalTicks);
        in.held=GAME_RIGHT;
        for(i=0;i<100;i++){
            Game_Tick(&game,in);
            if(f->gaitPhase<lastPhase){
                int32_t travelled=f->x-cycleStart;
                assert(abs(travelled-f->motion.stride)<f->moveSpeed*2);
                cycleStart=f->x;cycles++;
            }
            lastPhase=f->gaitPhase;
        }
        assert(cycles>=2&&f->x>start&&f->direction==COMBAT_E);
        assert(!f->motion.hover||Combat_HoverHeight(f)>=3*256);
        if(species[k]==25)fast=f->moveSpeed;
        if(species[k]==9)heavy=f->moveSpeed;
        in=None();for(i=0;i<8;i++)Game_Tick(&game,in);
        assert(!f->vx&&!f->vy&&f->animation==COMBAT_IDLE);
        lastPhase=f->gaitPhase;start=f->x;
        for(i=0;i<15;i++)Game_Tick(&game,in);
        assert(f->x==start&&f->gaitPhase==lastPhase);
    }
    assert(fast>heavy&&heavy>=266*105/100&&heavy<=266*115/100);
    /* Lock controls aim, not a backwards-facing walk animation. */
    Setup(25,9,0);game.combat.fighters[1].x=30*256;
    {GameInput in=None();in.held=GAME_RIGHT;for(unsigned i=0;i<12;i++)Game_Tick(&game,in);}
    assert(game.aim.mode==GAME_AIM_LOCK&&game.combat.fighters[0].direction==COMBAT_E);
    assert(game.combat.aimX<0);
}
static void Contact(void)
{
    static const unsigned moves[]={MOVE_SCRATCH,MOVE_BITE,MOVE_TACKLE,MOVE_IRON_TAIL};
    for(unsigned n=0;n<4;n++){
        CombatFighter *f,*v;int32_t x;unsigned hit=0;
        Setup(25,9,moves[n]);f=&game.combat.fighters[0];v=&game.combat.fighters[1];
        v->x=f->x+(moves[n]==MOVE_TACKLE?50:moves[n]==MOVE_BITE?26:30)*256;x=f->x;
        Start();
        for(unsigned i=0;i<200;i++){
            if(game.combat.hits){
                const ArenaSpecies *s=ArenaSpecies_Find(25);
                const ArenaSpriteAnimation *a=&s->animation[f->animation];
                assert(f->phase==COMBAT_ACTIVE);
                assert(Combat_AnimationTick(f,a->totalTicks,a->hitTick)>=a->hitTick);
                assert(v->hurtTimer==v->reactionDuration&&v->reactionDuration>=v->motion.hurtTicks);
                assert(game.combat.contactSerial&&v->bufferTimer==0);hit=1;break;
            }
            Game_Tick(&game,None());
        }
        if(!hit)fprintf(stderr,"No contact: move=%u phase=%u x=%d enemy=%d y=%d enemyY=%d pp=%u serial=%u\n",moves[n],f->phase,f->x/256,v->x/256,f->y/256,v->y/256,f->pp[0],f->actionSerial);
        assert(hit);
        if(moves[n]==MOVE_TACKLE)assert(f->x>x);
        else assert(f->x==x);
        Game_Tick(&game,None());
        assert(f->previousX==f->x&&v->previousX==v->x); /* interpolation freezes with hit pause */
    }
    /* Swept, anisotropic registered body, not identical nine-pixel circles. */
    {CombatShape ray={0},large={0},small={0};ray.active=large.active=small.active=1;
     ray.kind=COMBAT_SHAPE_CAPSULE;ray.x1=100*256;ray.y0=ray.y1=13*256;ray.radiusX=ray.radiusY=2;
     large.x0=small.x0=50*256;large.radiusX=small.radiusX=7;large.radiusY=14;small.radiusY=7;
     assert(Combat_ShapeHits(&ray,&large));assert(!Combat_ShapeHits(&ray,&small));}
}
static void Commitment(void)
{
    CombatFighter *f;GameInput in=None();int32_t x;unsigned serial;
    Setup(448,9,MOVE_SWORDS_DANCE);f=&game.combat.fighters[0];x=f->x;Start();serial=f->actionSerial;
    in.held=GAME_RIGHT|GAME_A;
    while(f->phase!=COMBAT_READY){assert(f->x==x);Game_Tick(&game,in);}
    for(unsigned i=0;i<180;i++)Game_Tick(&game,in);
    assert(f->actionSerial==serial&&f->x>x); /* held attack is not another press */
    Setup(25,9,MOVE_SCRATCH);f=&game.combat.fighters[0];Start();
    assert(!Combat_QueueMove(&game.combat,0,0));
    in=None();in.pressed=GAME_A;Game_Tick(&game,in);in=None();
    for(unsigned i=0;i<200;i++)Game_Tick(&game,in);
    assert(f->actionSerial==1); /* early taps cannot reappear after recovery */
}
static void Dash(void)
{
    static const uint16_t dirs[]={GAME_DOWN,GAME_DOWN|GAME_RIGHT,GAME_RIGHT,GAME_UP|GAME_RIGHT,
        GAME_UP,GAME_UP|GAME_LEFT,GAME_LEFT,GAME_DOWN|GAME_LEFT};
    for(unsigned d=0;d<8;d++){
        GameInput in=None();unsigned trails=0;
        Setup(25,9,0);in.held=dirs[d]|GAME_B;in.pressed=GAME_B;Game_Tick(&game,in);in.pressed=0;
        assert(game.combat.fighters[0].dashDirection==d);
        for(unsigned tick=0;tick<20;tick++){
            for(unsigned i=0;i<COMBAT_EFFECTS;i++)if(game.combat.effects[i].life&&game.combat.effects[i].kind==COMBAT_DASH_TRAIL){
                assert(game.combat.effects[i].owner==0&&game.combat.fighters[0].dashTimer);trails++;
            }
            Game_Tick(&game,in);
        }
        assert(trails);
    }
}
int main(void)
{
    Gaits();Contact();Commitment();Dash();
    puts("PASS motion/contact: five authored gait cycles, lock-independent facing, weighted speeds/braking, registered swept bodies, strike/charge contact and Hurt, frozen hit interpolation, cast commitment, no ghost buffers, eight-direction dash lifecycle");
    return 0;
}
