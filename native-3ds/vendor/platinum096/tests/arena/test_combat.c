/* Integration checks exercise match behavior, not renderer implementation. */
#include "arena/combat.h"
#include "arena/vendor/arena_navigation.h"
#include "arena/vendor/arena_physics.h"
#include "arena/vendor/constants/moves.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define Q 256
static Combat match;
static CombatFighterConfig config[2]={
    {25,150,80,70,90,{MOVE_THUNDER_SHOCK,MOVE_QUICK_ATTACK,MOVE_POUND,MOVE_PROTECT},{13,255},0,0},
    {7,150,70,80,70,{MOVE_WATER_GUN,MOVE_TACKLE,MOVE_BUBBLE,MOVE_PROTECT},{11,255},0,0}
};
static void Reset(void)
{
    Combat_Init(&match,config);match.aiEnabled=0;match.fighters[0].selected=0;
    assert(ArenaNav_CanStand(match.fighters[0].x/Q,match.fighters[0].y/Q));
    assert(ArenaNav_CanStand(match.fighters[1].x/Q,match.fighters[1].y/Q));
}
static void Tick(unsigned held,unsigned pressed)
{ CombatInput input={(uint16_t)held,(uint16_t)pressed};Combat_Tick(&match,input); }
static void Advance(unsigned n)
{ while(n--) Tick(0,0); }
static void OpenArena(void)
{ unsigned i;for(i=0;i<ARENA_OBSTACLES;i++) ArenaNav_SetObstacle(i,0); }
static void Place(int ax,int ay,int bx,int by)
{
    match.fighters[0].x=ax*Q;match.fighters[0].y=ay*Q;
    match.fighters[1].x=bx*Q;match.fighters[1].y=by*Q;
    match.fighters[0].previousX=ax*Q;match.fighters[0].previousY=ay*Q;
    match.fighters[1].previousX=bx*Q;match.fighters[1].previousY=by*Q;
}
static void TestMetadataAndTypes(void)
{
    assert(Combat_GetMoveInfo(MOVE_THUNDER_SHOCK)->type==13);
    assert(Combat_GetMoveInfo(MOVE_WATER_GUN)->power==40);
    assert(Combat_GetMoveInfo(MOVE_TELEPORT)->category==2);
    assert(Combat_GetMoveInfo(MOVE_THUNDER)->power==120);
    assert(Combat_TypeMultiplier(13,11,255)==512);
    assert(Combat_TypeMultiplier(13,4,11)==0);
    assert(Combat_TypeMultiplier(10,6,12)==1024);
    assert(Combat_TypeMultiplier(12,2,3)==64);
    assert(Combat_TypeMultiplier(7,0,255)==0);
    assert(Combat_TypeMultiplier(10,12,12)==512);
}
static void TestMovementAndDash(void)
{
    int x,dx,i;
    Reset();OpenArena();Place(90,78,200,78);
    for(i=0;i<20;i++) Tick(COMBAT_RIGHT,0);
    x=match.fighters[0].x;Tick(0,0);
    assert(match.fighters[0].x>x); /* bounded inertia after releasing input */
    Reset();OpenArena();Place(90,78,200,78);
    Tick(COMBAT_RIGHT,COMBAT_B);
    assert(match.fighters[0].dashTimer==12);
    assert(match.fighters[0].stamina==78);
    dx=match.fighters[0].x-90*Q;assert(dx>2*Q);
    Advance(2);
    assert(match.fighters[0].dashCooldown>0);
    assert(match.fighters[0].direction==COMBAT_E);
    for(i=0;i<300;i++) Tick(COMBAT_LEFT|COMBAT_UP,0);
    assert(match.fighters[0].x>=ARENA_MIN_X*Q);
    assert(match.fighters[0].y>=ARENA_MIN_Y*Q);
}
static void TestProjectileAndPP(void)
{
    unsigned pp,hp;
    Reset();OpenArena();Place(60,78,125,78);
    pp=match.fighters[0].pp[0];hp=match.fighters[1].hp;
    Tick(0,COMBAT_A);assert(match.fighters[0].pp[0]==pp-1);
    Advance(9);assert(match.fighters[1].hp==hp); /* actual windup, no instant damage */
    Advance(55);assert(match.fighters[1].hp<hp);assert(match.hits==1);
    hp=match.fighters[1].hp;Advance(60);assert(match.fighters[1].hp==hp);
    match.fighters[0].pp[0]=0;Tick(0,COMBAT_A);
    assert(match.fighters[0].phase==COMBAT_READY);
}
static void TestAimAndBasic(void)
{
    unsigned hp;
    Reset();OpenArena();Place(80,78,108,78);
    hp=match.fighters[1].hp;Tick(0,COMBAT_L);Advance(20);
    assert(match.fighters[1].hp<hp);
    assert(match.fighters[0].pp[0]==Combat_GetMoveInfo(MOVE_THUNDER_SHOCK)->pp);
    Reset();OpenArena();Place(80,78,108,78);
    Tick(0,COMBAT_Y);assert(!match.lockAim);
    match.fighters[0].direction=COMBAT_W;
    Tick(0,COMBAT_L);Advance(30);
    assert(match.fighters[1].hp==match.fighters[1].maxHp);
}
static void InjectShot(int x,int y,int vx)
{
    CombatProjectile *p=&match.projectiles[0];
    memset(p,0,sizeof(*p));p->x=x*Q;p->y=y*Q;
    p->previousX=p->x;p->previousY=p->y;p->vx=vx;
    p->life=50;p->owner=1;p->move=MOVE_WATER_GUN;p->radius=4;
    p->type=11;p->color=Combat_TypeColor(11);
}
static void TestGuardDodgeAndFastContact(void)
{
    unsigned hp;
    Reset();OpenArena();Place(80,78,180,78);
    InjectShot(100,78,-12*Q);hp=match.fighters[0].hp;
    Tick(0,COMBAT_B);
    assert(match.fighters[0].hp==hp);assert(match.blocked==1);
    assert(!match.projectiles[0].life);
    Reset();OpenArena();Place(80,78,180,78);
    InjectShot(100,78,-12*Q);hp=match.fighters[0].hp;
    Tick(COMBAT_DOWN,COMBAT_B);assert(match.fighters[0].hp==hp);
    assert(match.dodged>0);
    Reset();OpenArena();Place(80,78,180,78);
    InjectShot(110,78,-60*Q);hp=match.fighters[0].hp;
    Tick(0,0);assert(match.fighters[0].hp<hp); /* no high-speed tunnelling */
}
static void TestTerrainAndDefenseBuffer(void)
{
    int i;
    Reset();Place(gArenaObstacles[0].left-20,
        (gArenaObstacles[0].top+gArenaObstacles[0].bottom)/2,254,136);
    match.lockAim=0;match.fighters[0].direction=COMBAT_E;
    match.fighters[0].selected=2;Tick(0,COMBAT_A);Advance(40);
    assert(gArenaPhysicsTelemetry.impacts>0);
    Reset();OpenArena();Place(80,78,180,78);
    Tick(0,COMBAT_L);Advance(8);
    Tick(COMBAT_DOWN,COMBAT_B);
    for(i=0;i<12&&!match.fighters[0].dashTimer;i++) Tick(0,0);
    assert(match.fighters[0].dashTimer); /* buffered dash cancels late recovery */
}
static void TestRushRequiresPhysicalContact(void)
{
    unsigned hp;
    Reset();OpenArena();Place(80,78,130,78);
    match.fighters[0].selected=1;hp=match.fighters[1].hp;
    Tick(0,COMBAT_A);Advance(4);
    assert(match.fighters[1].hp==hp); /* Quick Attack has begun, still out of contact. */
    Advance(12);assert(match.fighters[1].hp<hp);
}
static void TestInputDuringHitstop(void)
{
    Reset();OpenArena();Place(80,78,180,78);
    match.hitstop=3;
    Tick(COMBAT_DOWN,COMBAT_B);Advance(3);
    assert(match.fighters[0].dashTimer);
    assert(match.fighters[0].dashDirection==COMBAT_S);
}
static void TestAIMatchBoundsAndResult(void)
{
    int i;
    Reset();match.aiEnabled=1;
    for(i=0;i<10000&&!match.winner;i++) {
        Tick(0,(i%30)==0?COMBAT_A:0);
        assert(ArenaNav_CanStand(match.fighters[0].x/Q,match.fighters[0].y/Q));
        assert(ArenaNav_CanStand(match.fighters[1].x/Q,match.fighters[1].y/Q));
        assert(match.fighters[0].stamina<=100&&match.fighters[1].stamina<=100);
        assert(gArenaPhysicsTelemetry.live<=ARENA_FRAGMENTS);
    }
    assert(match.hits>0);
    assert(match.winner==1||match.winner==2);
    assert(match.fighters[match.winner==1?1:0].hp==0);
}
static void TestSpecialStatsAndHeldInput(void)
{
    unsigned low,high,pp;
    Reset();OpenArena();Place(60,78,125,78);match.fighters[0].spAttack=20;
    Tick(0,COMBAT_A);Advance(55);low=150-match.fighters[1].hp;
    Reset();OpenArena();Place(60,78,125,78);match.fighters[0].spAttack=180;
    Tick(0,COMBAT_A);Advance(55);high=150-match.fighters[1].hp;assert(high>low);
    Reset();OpenArena();Place(60,78,200,78);pp=match.fighters[0].pp[0];
    for(unsigned i=0;i<180;i++)Tick(COMBAT_A,0);
    assert(match.fighters[0].pp[0]+1u<pp); /* Held attacks repeat through genuine recovery. */
}
static void TestImmunityCannotDeadlockArena(void)
{
    Reset();OpenArena();Place(80,78,108,78);
    match.fighters[0].types[0]=0;match.fighters[0].types[1]=255;
    match.fighters[1].types[0]=7;match.fighters[1].types[1]=255;
    for(unsigned side=0;side<2;side++)for(unsigned i=0;i<4;i++)match.fighters[side].pp[i]=0;
    unsigned hp=match.fighters[1].hp;
    Tick(0,COMBAT_L);Advance(25);assert(match.fighters[1].hp<hp);
    match.aiEnabled=1;
    for(unsigned i=0;i<10000&&!match.winner;i++)Tick(COMBAT_L,0);
    assert(match.winner); /* PP exhaustion / Normal-Ghost cannot strand a round. */
}
static void TestExpandedWorldUsesWorldBounds(void)
{
    Reset();OpenArena();Place(290,210,355,210);
    unsigned hp=match.fighters[1].hp;
    Tick(0,COMBAT_A);Advance(55);
    assert(match.fighters[1].hp<hp); /* shots survive outside the former screen */
    Reset();OpenArena();Place(ARENA_MAX_X-15,ARENA_MAX_Y-15,180,100);
    for(unsigned i=0;i<160;i++)Tick(COMBAT_RIGHT|COMBAT_DOWN,0);
    assert(match.fighters[0].x==ARENA_MAX_X*Q);
    assert(match.fighters[0].y==ARENA_MAX_Y*Q);
    assert(ArenaNav_CanStand(match.fighters[0].x/Q,match.fighters[0].y/Q));
}
static void TestTeamReplacementPreservesArena(void)
{
    CombatFighter other;
    CombatEffect effects[COMBAT_EFFECTS];
    CombatProjectile other_shot;
    struct ArenaProp props[ARENA_OBSTACLES];
    struct ArenaFragment fragments[ARENA_FRAGMENTS];
    struct ArenaPhysicsTelemetry physics;
    CombatFighterConfig incoming=config[1];
    Reset();Place(180,110,250,110);
    /* Replacement must preserve actual damaged terrain/debris and the live
       opponent, rather than restarting a full match at each trainer wave. */
    ArenaPhysics_Hit(0,900,0,255);
    memcpy(props,gArenaProps,sizeof(props));
    memcpy(fragments,gArenaFragments,sizeof(fragments));
    physics=gArenaPhysicsTelemetry;
    match.fighters[0].hp=83;match.fighters[0].pp[0]=2;
    match.fighters[0].phase=COMBAT_RECOVERY;match.fighters[0].actionTick=15;
    other=match.fighters[0];
    match.fighters[1].hp=0;match.fighters[1].direction=COMBAT_NW;
    match.fighters[1].phase=COMBAT_ACTIVE;match.fighters[1].actionMove=MOVE_TACKLE;
    match.fighters[1].vx=500;match.fighters[1].buffered=COMBAT_A;
    match.fighters[1].bufferTimer=9;match.fighters[1].dashTimer=7;
    match.projectiles[0].owner=1;match.projectiles[0].life=12;
    match.projectiles[1].owner=0;match.projectiles[1].life=20;
    match.projectiles[1].move=MOVE_THUNDER_SHOCK;
    other_shot=match.projectiles[1];
    match.effects[0].life=9;match.effects[0].x=12345;
    memcpy(effects,match.effects,sizeof(effects));
    match.frame=567;match.random=0x12345678;match.winner=1;match.hitstop=8;
    incoming.species=130;incoming.hp=231;incoming.spAttack=99;
    incoming.moves[3]=MOVE_TELEPORT;
    assert(Combat_ReplaceFighter(&match,1,&incoming));
    assert(!memcmp(&other,&match.fighters[0],sizeof(other)));
    assert(!memcmp(&other_shot,&match.projectiles[1],sizeof(other_shot)));
    assert(!match.projectiles[0].life&&!match.projectiles[0].move);
    assert(!memcmp(effects,match.effects,sizeof(effects)));
    assert(!memcmp(props,gArenaProps,sizeof(props)));
    assert(!memcmp(fragments,gArenaFragments,sizeof(fragments)));
    assert(!memcmp(&physics,&gArenaPhysicsTelemetry,sizeof(physics)));
    assert(match.frame==567&&match.random==0x12345678&&!match.hitstop&&!match.winner);
    assert(match.fighters[1].species==130&&match.fighters[1].hp==231);
    assert(match.fighters[1].maxHp==231&&match.fighters[1].spAttack==99);
    assert(match.fighters[1].x==250*Q&&match.fighters[1].y==110*Q);
    assert(match.fighters[1].previousX==250*Q&&match.fighters[1].direction==COMBAT_NW);
    assert(match.fighters[1].stamina==100&&match.fighters[1].phase==COMBAT_READY);
    assert(!match.fighters[1].vx&&!match.fighters[1].buffered&&!match.fighters[1].dashTimer);
    assert(!match.fighters[1].actionMove&&!match.fighters[1].bufferTimer);
    assert(match.fighters[1].pp[0]==Combat_GetMoveInfo(incoming.moves[0])->pp);
    assert(match.fighters[1].moves[3]==MOVE_TELEPORT&&match.fighters[1].pp[3]==Combat_GetMoveInfo(MOVE_TELEPORT)->pp);
    Tick(0,0);assert(match.frame==568&&!match.winner);
    /* An invalid request is a complete no-op, including winner and terrain. */
    Combat unchanged=match;
    assert(!Combat_ReplaceFighter(&match,COMBAT_FIGHTERS,&incoming));
    assert(!Combat_ReplaceFighter(&match,0,0));
    assert(!Combat_ReplaceFighter(0,0,&incoming));
    assert(!memcmp(&unchanged,&match,sizeof(match)));
    /* Voluntary switching does not dismiss an existing opposing knockout. */
    match.winner=1;match.fighters[1].hp=0;
    assert(Combat_ReplaceFighter(&match,0,&config[0]));assert(match.winner==1);
}
static void TestEndgameStatsRetainLevelScaling(void)
{
    CombatFighterConfig high[2]={config[0],config[1]};
    /* Canonical level-100 HP can exceed 999 after the story arena's x8
       scaling (Blissey's maximum HP is 714). No stat truncates to 8 bits. */
    high[0].species=242;high[0].hp=714*8;high[0].attack=1600;
    high[0].defense=1400;high[0].spAttack=2016;high[0].spDefense=1800;
    Combat_Init(&match,high);
    assert(match.fighters[0].maxHp==5712&&match.fighters[0].hp==5712);
    assert(match.fighters[0].attack==1600&&match.fighters[0].defense==1400);
    assert(match.fighters[0].spAttack==2016&&match.fighters[0].spDefense==1800);
    high[0].hp=65535;high[0].attack=65535;high[0].defense=0;
    high[0].spAttack=0;high[0].spDefense=65535;high[0].speed=65535;
    high[0].moves[0]=MOVE_HYPER_BEAM;
    high[1].hp=8191;high[1].spDefense=1;high[1].defense=1;
    Combat_Init(&match,high);match.aiEnabled=0;OpenArena();Place(60,78,125,78);
    assert(match.fighters[0].maxHp==8191&&match.fighters[0].attack==2047);
    assert(match.fighters[0].defense==1&&match.fighters[0].spAttack==2047);
    assert(match.fighters[0].spDefense==2047&&match.fighters[0].speed==255);
    Tick(0,COMBAT_A);Advance(125);
    assert(match.fighters[1].hp<8191); /* Maximum attack cannot overflow damage. */
    high[0].hp=0;high[0].attack=0;high[0].spAttack=0;
    assert(Combat_ReplaceFighter(&match,0,&high[0]));
    assert(match.fighters[0].hp==1&&match.fighters[0].attack==1&&match.fighters[0].spAttack==1);
}
int main(void)
{
    TestMetadataAndTypes();TestMovementAndDash();TestProjectileAndPP();
    TestAimAndBasic();TestGuardDodgeAndFastContact();
    TestTerrainAndDefenseBuffer();TestRushRequiresPhysicalContact();TestInputDuringHitstop();TestAIMatchBoundsAndResult();TestSpecialStatsAndHeldInput();TestImmunityCannotDeadlockArena();TestExpandedWorldUsesWorldBounds();TestTeamReplacementPreservesArena();TestEndgameStatsRetainLevelScaling();
    puts("PASS: native combat movement, timing, PP, aim, types, guards, dodge, swept hits, terrain, input buffering and AI match");
    return 0;
}
