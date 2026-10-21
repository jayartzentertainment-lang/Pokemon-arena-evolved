/* 0.9.6 behavioural checks: real simulation clocks, native statuses, learned
 * move resources, natural attacks, stages and deterministic beam ownership. */
#include "arena/combat.h"
#include "arena/vendor/arena_navigation.h"
#include "arena/vendor/arena_moves.h"
#include "arena/vendor/constants/moves.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define Q 256
static Combat c;
static const CombatFighterConfig config[4]={
    {25,800,100,100,90,{MOVE_POUND,MOVE_THUNDER_SHOCK,MOVE_SWORDS_DANCE,MOVE_PROTECT},{13,255},100,100},
    {7,800,100,100,70,{MOVE_POUND,MOVE_WATER_GUN,MOVE_GROWL,MOVE_PROTECT},{11,255},100,100},
    {448,800,100,100,90,{MOVE_POUND,MOVE_THUNDER_SHOCK,MOVE_SWORDS_DANCE,MOVE_PROTECT},{1,8},100,100},
    {94,800,100,100,70,{MOVE_POUND,MOVE_WATER_GUN,MOVE_GROWL,MOVE_PROTECT},{7,3},100,100}
};
static void tick(unsigned held,unsigned pressed)
{ Combat_Tick(&c,(CombatInput){(uint16_t)held,(uint16_t)pressed}); }
static void advance(unsigned ticks){while(ticks--)tick(0,0);}
static void place(unsigned actor,int x,int y)
{ c.fighters[actor].x=c.fighters[actor].previousX=x*Q;c.fighters[actor].y=c.fighters[actor].previousY=y*Q; }
static void reset(int teams)
{
    if(teams)Combat_InitTeams(&c,config);else Combat_Init(&c,config);
    c.aiEnabled=0;
    for(unsigned i=0;i<ARENA_OBSTACLES;i++)ArenaNav_SetObstacle(i,0);
    place(0,70,100);place(1,100,100);
    if(teams){place(2,70,220);place(3,200,220);}
}
static void move(unsigned actor,unsigned slot,uint16_t id)
{ c.fighters[actor].moves[slot]=id;c.fighters[actor].pp[slot]=Combat_GetMoveInfo(id)->pp; }
static void shot(uint16_t id)
{
    CombatProjectile *p=&c.projectiles[0];memset(p,0,sizeof(*p));
    p->x=p->previousX=90*Q;p->y=p->previousY=100*Q;p->vx=20*Q;
    p->life=10;p->radius=4;p->move=id;p->owner=0;
}
static unsigned damage(uint16_t id,unsigned stat,int stage,unsigned nativeStatus)
{
    reset(0);c.random=7;Combat_ChangeStage(&c,0,stat,stage);
    Combat_SetNativeStatus(&c,0,nativeStatus);shot(id);tick(0,0);
    return 800-c.fighters[1].hp;
}
static void stages_persist_and_drive_damage(void)
{
    reset(0);
    for(unsigned stat=0;stat<COMBAT_STAT_COUNT;stat++) {
        assert(Combat_ChangeStage(&c,0,stat,99)==6);
        assert(!Combat_ChangeStage(&c,0,stat,1));
        assert(Combat_ChangeStage(&c,0,stat,-99)==-12);
        assert(c.fighters[0].stages[stat]==-6);
    }
    advance(900);
    for(unsigned stat=0;stat<COMBAT_STAT_COUNT;stat++)assert(c.fighters[0].stages[stat]==-6);
    assert(Combat_ReplaceFighter(&c,0,&config[0]));
    for(unsigned stat=0;stat<COMBAT_STAT_COUNT;stat++)assert(!c.fighters[0].stages[stat]);
    unsigned physical=damage(MOVE_POUND,COMBAT_STAT_ATTACK,0,0);
    assert(damage(MOVE_POUND,COMBAT_STAT_ATTACK,2,0)>physical);
    assert(damage(MOVE_POUND,COMBAT_STAT_SP_ATTACK,2,0)==physical);
    assert(damage(MOVE_POUND,COMBAT_STAT_ATTACK,0,16)<physical);
    unsigned special=damage(MOVE_WATER_GUN,COMBAT_STAT_SP_ATTACK,0,0);
    assert(damage(MOVE_WATER_GUN,COMBAT_STAT_SP_ATTACK,2,0)>special);
    assert(damage(MOVE_WATER_GUN,COMBAT_STAT_ATTACK,2,0)==special);
    assert(damage(MOVE_WATER_GUN,COMBAT_STAT_ATTACK,0,16)==special);
    reset(0);assert(Combat_QueueMove(&c,0,2));tick(0,0);advance(25);
    assert(c.fighters[0].stages[COMBAT_STAT_ATTACK]==2);
    advance(600);assert(c.fighters[0].stages[COMBAT_STAT_ATTACK]==2);
}
static void defense_and_speed_stages(void)
{
    unsigned plain=damage(MOVE_POUND,COMBAT_STAT_ATTACK,0,0);
    reset(0);c.random=7;Combat_ChangeStage(&c,1,COMBAT_STAT_DEFENSE,2);shot(MOVE_POUND);tick(0,0);
    assert(800u-c.fighters[1].hp<plain);
    plain=damage(MOVE_WATER_GUN,COMBAT_STAT_ATTACK,0,0);
    reset(0);c.random=7;Combat_ChangeStage(&c,1,COMBAT_STAT_SP_DEFENSE,2);shot(MOVE_WATER_GUN);tick(0,0);
    assert(800u-c.fighters[1].hp<plain);
    reset(0);place(1,300,100);for(unsigned i=0;i<20;i++)tick(COMBAT_RIGHT,0);
    int32_t normal=c.fighters[0].x-70*Q;
    reset(0);place(1,300,100);Combat_ChangeStage(&c,0,COMBAT_STAT_SPEED,2);
    for(unsigned i=0;i<20;i++)tick(COMBAT_RIGHT,0);
    assert(c.fighters[0].x-70*Q>normal);
    reset(0);place(1,300,100);Combat_SetNativeStatus(&c,0,64);
    for(unsigned i=0;i<20;i++)tick(COMBAT_RIGHT,0);
    assert(c.fighters[0].x-70*Q<normal/2);
}
static void accuracy_and_evasion_are_opposed(void)
{
    unsigned normal=0,misses=0,matched=0;
    for(unsigned seed=0;seed<64;seed++) {
        reset(0);c.random=seed;shot(MOVE_POUND);tick(0,0);normal+=!!c.hits;
        reset(0);c.random=seed;Combat_ChangeStage(&c,1,COMBAT_STAT_EVASION,6);
        shot(MOVE_POUND);tick(0,0);misses+=!c.hits;
        reset(0);c.random=seed;Combat_ChangeStage(&c,1,COMBAT_STAT_EVASION,6);
        Combat_ChangeStage(&c,0,COMBAT_STAT_ACCURACY,6);shot(MOVE_POUND);tick(0,0);matched+=!!c.hits;
    }
    assert(normal==64&&matched==64&&misses>20);
}
static void statuses_use_central_clock(void)
{
    static const unsigned native[]={8,16,32,64,128,3};
    for(unsigned i=0;i<sizeof(native)/sizeof(native[0]);i++) {
        reset(0);Combat_SetNativeStatus(&c,0,native[i]);
        assert(Combat_GetNativeStatus(&c,0)==native[i]);
        advance(20);unsigned clock=c.fighters[0].statusClock;
        Combat_SetNativeStatus(&c,0,Combat_GetNativeStatus(&c,0));assert(c.fighters[0].statusClock==clock);
    }
    reset(0);Combat_SetNativeStatus(&c,0,8);c.hitstop=200;advance(179);
    assert(c.fighters[0].hp==800);tick(0,0);assert(c.fighters[0].hp==700);
    advance(180);assert(c.fighters[0].hp==600);
    reset(0);Combat_SetNativeStatus(&c,0,128);advance(180);assert(c.fighters[0].hp==750);
    advance(180);assert(c.fighters[0].hp==650&&c.fighters[0].toxicCounter==3);
    reset(0);Combat_SetNativeStatus(&c,0,3);unsigned pp=c.fighters[0].pp[0];
    tick(COMBAT_RIGHT,COMBAT_X);assert(c.fighters[0].pp[0]==pp&&c.fighters[0].x==70*Q);
    advance(179);assert(!c.fighters[0].status);
    tick(0,COMBAT_X);assert(c.fighters[0].pp[0]==pp-1);
    reset(0);Combat_SetNativeStatus(&c,0,32);tick(COMBAT_RIGHT,COMBAT_B);
    assert(!c.fighters[0].dashTimer&&c.fighters[0].stamina==100);advance(480);assert(!c.fighters[0].status);
    reset(0);c.fighters[0].confusionTicks=300;c.hitstop=20;advance(20);assert(c.fighters[0].confusionTicks==280);
}
static void native_status_moves_and_immunity(void)
{
    reset(0);move(0,0,MOVE_WILL_O_WISP);c.random=10;
    assert(Combat_QueueMove(&c,0,0));tick(0,0);advance(35);
    assert(c.fighters[1].status==COMBAT_STATUS_BURN);
    reset(0);move(0,0,MOVE_TOXIC);c.fighters[1].types[0]=8;c.random=10;
    Combat_QueueMove(&c,0,0);tick(0,0);advance(35);assert(!c.fighters[1].status);
    reset(0);move(0,0,MOVE_CONFUSE_RAY);Combat_QueueMove(&c,0,0);tick(0,0);advance(35);
    assert(c.fighters[1].confusionTicks>0&&!c.fighters[1].status);
    reset(0);move(0,0,MOVE_SAND_ATTACK);Combat_QueueMove(&c,0,0);tick(0,0);advance(35);
    assert(c.fighters[1].stages[COMBAT_STAT_ACCURACY]==-1);
    reset(0);move(0,0,MOVE_CALM_MIND);Combat_QueueMove(&c,0,0);tick(0,0);advance(35);
    assert(c.fighters[0].stages[COMBAT_STAT_SP_ATTACK]==1&&c.fighters[0].stages[COMBAT_STAT_SP_DEFENSE]==1);
}
static void four_moves_pp_cooldowns_and_natural(void)
{
    reset(0);assert(Combat_NaturalMove(&c.fighters[0])==MOVE_QUICK_ATTACK);
    uint8_t pp[4];memcpy(pp,c.fighters[0].pp,4);
    tick(0,COMBAT_L);assert(c.fighters[0].basicAttack&&c.fighters[0].actionMove==MOVE_QUICK_ATTACK);
    assert(!memcmp(pp,c.fighters[0].pp,4));advance(60);
    tick(0,COMBAT_CYCLE_LIGHT);assert(Combat_NaturalMove(&c.fighters[0])==MOVE_SLAM);
    for(unsigned slot=0;slot<4;slot++) {
        reset(0);memcpy(pp,c.fighters[0].pp,4);assert(Combat_QueueMove(&c,0,slot));tick(0,0);
        assert(c.fighters[0].actionSlot==slot&&c.fighters[0].pp[slot]==pp[slot]-1);
        for(unsigned other=0;other<4;other++)if(slot!=other)assert(c.fighters[0].pp[other]==pp[other]);
        assert(!Combat_QueueMove(&c,0,slot)&&c.fighters[0].cooldown[slot]);
    }
    reset(0);move(0,0,MOVE_HYPER_BEAM);Combat_QueueMove(&c,0,0);tick(0,0);
    unsigned cooldown=c.fighters[0].cooldown[0];c.hitstop=10;advance(10);
    assert(c.fighters[0].cooldown[0]==cooldown-10);
    assert(!Combat_QueueMove(&c,0,4));c.fighters[0].pp[1]=0;assert(!Combat_QueueMove(&c,0,1));
}
static void beam(unsigned actor,uint16_t id,unsigned direction)
{
    CombatFighter *f=&c.fighters[actor];unsigned w=ArenaMoves_Get(id)->windup;
    f->actionMove=id;f->phase=COMBAT_ACTIVE;f->attackDirection=direction;
    f->actionTick=w+(Combat_GetMoveInfo(id)->power>=80?(w+7)/8:0);f->actionSerial=1;
}
static void direction_double_tap_shares_dash_rules(void)
{
    reset(0);place(1,300,200);
    tick(COMBAT_RIGHT,COMBAT_RIGHT);
    for(unsigned i=0;i<30;i++) {
        tick(COMBAT_RIGHT,0);
        assert(!c.fighters[0].dashTimer&&c.fighters[0].stamina==100);
    }
    reset(0);place(1,300,200);
    tick(COMBAT_RIGHT,COMBAT_RIGHT);tick(0,0);
    tick(COMBAT_RIGHT,COMBAT_RIGHT);
    assert(c.fighters[0].dashTimer==12&&c.fighters[0].dashCooldown==30);
    assert(c.fighters[0].dashDirection==COMBAT_E&&c.fighters[0].stamina==78);
    /* A third/fourth edge during the same dash cannot charge a second cost
     * or bypass its cooldown. Holding afterwards never repeats the dash. */
    tick(0,0);tick(COMBAT_RIGHT,COMBAT_RIGHT);tick(0,0);
    tick(COMBAT_RIGHT,COMBAT_RIGHT);
    assert(c.fighters[0].dashTimer==8&&c.fighters[0].stamina==78);
    unsigned previous=c.fighters[0].dashTimer;
    for(unsigned i=0;i<45;i++) {
        tick(COMBAT_RIGHT,0);
        assert(c.fighters[0].dashTimer<=previous);
        previous=c.fighters[0].dashTimer;
    }
    reset(0);place(1,300,200);
    tick(COMBAT_UP|COMBAT_LEFT,COMBAT_UP|COMBAT_LEFT);tick(0,0);
    tick(COMBAT_UP|COMBAT_LEFT,COMBAT_UP|COMBAT_LEFT);
    assert(c.fighters[0].dashTimer==12&&c.fighters[0].dashDirection==COMBAT_NW);
    assert(c.fighters[0].stamina==78); /* Diagonal edges trigger one dash. */
    reset(0);place(1,300,200);c.fighters[0].stamina=21;c.fighters[0].regenDelay=20;
    tick(COMBAT_RIGHT,COMBAT_RIGHT);tick(0,0);tick(COMBAT_RIGHT,COMBAT_RIGHT);
    assert(!c.fighters[0].dashTimer&&c.fighters[0].stamina==21);
    reset(0);place(1,300,200);
    tick(COMBAT_RIGHT,COMBAT_RIGHT);tick(0,0);advance(12);
    tick(COMBAT_RIGHT,COMBAT_RIGHT);
    assert(!c.fighters[0].dashTimer&&c.fighters[0].stamina==100);
}
static void deterministic_clashes_ignore_button_mashing(void)
{
    Combat expected;
    for(unsigned mash=0;mash<2;mash++) {
        reset(0);place(1,200,100);c.fighters[0].spAttack=400;
        beam(0,MOVE_FLAMETHROWER,COMBAT_E);beam(1,MOVE_FLAMETHROWER,COMBAT_W);tick(0,0);
        assert(c.clashes==1&&Combat_BeamContact(&c,0,0,0));
        for(unsigned i=0;i<200&&Combat_BeamContact(&c,0,0,0);i++)tick(0,mash?COMBAT_A:0);
        assert(c.beamClashes[0].winner==0&&c.hits==1&&c.fighters[0].hp==800&&c.fighters[1].hp<800);
        if(!mash)expected=c;else {
            assert(c.frame==expected.frame&&c.hits==expected.hits);
            assert(c.fighters[1].hp==expected.fighters[1].hp);
            assert(!memcmp(c.beamClashes,expected.beamClashes,sizeof(c.beamClashes)));
        }
    }
    reset(1);place(1,200,100);c.fighters[0].types[0]=c.fighters[1].types[0]=0;
    c.fighters[2].types[0]=c.fighters[3].types[0]=0;c.fighters[2].types[1]=c.fighters[3].types[1]=255;
    for(unsigned actor=0;actor<4;actor++)beam(actor,MOVE_FLAMETHROWER,actor&1?COMBAT_W:COMBAT_E);
    tick(0,0);assert(c.clashes==2);advance(190);
    assert(!c.hits&&c.fighters[0].hp==800&&c.fighters[1].hp==800);
}
int main(void)
{
    stages_persist_and_drive_damage();defense_and_speed_stages();accuracy_and_evasion_are_opposed();
    statuses_use_central_clock();native_status_moves_and_immunity();four_moves_pp_cooldowns_and_natural();
    direction_double_tap_shares_dash_rules();
    deterministic_clashes_ignore_button_mashing();
    puts("PASS: 0.9.6 seven persistent stages, native status/ticks, natural PP, four slots, cooldowns and stat-based 2v2 clashes");
    return 0;
}
