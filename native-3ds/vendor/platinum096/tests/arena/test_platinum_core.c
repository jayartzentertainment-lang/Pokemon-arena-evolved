/* Behavioral checks for native Gen IV moves and revised arena stamina. */
#include "arena/combat.h"
#include "arena/move_data.h"
#include "arena/vendor/arena_navigation.h"
#include "arena/vendor/constants/moves.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define Q 256
static Combat match;
static const CombatFighterConfig configs[4]={
    {390,800,100,100,70,{MOVE_FIRE_PUNCH,MOVE_THUNDERBOLT,MOVE_CLOSE_COMBAT,MOVE_LEER},{10,255},20,100},
    {393,800,100,100,60,{MOVE_WATER_GUN,MOVE_POUND,MOVE_PROTECT,MOVE_BUBBLE},{11,255},100,100},
    {387,800,100,100,60,{MOVE_RAZOR_LEAF,MOVE_TACKLE,MOVE_WITHDRAW,MOVE_ABSORB},{12,255},100,100},
    {403,800,100,100,60,{MOVE_SPARK,MOVE_BITE,MOVE_CHARGE,MOVE_THUNDER_SHOCK},{13,255},100,100}
};
static void tick(unsigned held,unsigned pressed)
{ Combat_Tick(&match,(CombatInput){(uint16_t)held,(uint16_t)pressed}); }
static void advance(unsigned n) {while(n--)tick(0,0);}
static void place(unsigned i,int x,int y)
{
    match.fighters[i].x=match.fighters[i].previousX=x*Q;
    match.fighters[i].y=match.fighters[i].previousY=y*Q;
}
static void reset(int teams)
{
    if(teams) Combat_InitTeams(&match,configs);else Combat_Init(&match,configs);
    match.aiEnabled=0;match.fighters[0].selected=0;
    for(unsigned i=0;i<ARENA_OBSTACLES;i++)ArenaNav_SetObstacle(i,0);
    place(0,70,100);place(1,100,100);
    if(teams){place(2,70,190);place(3,220,190);}
}
static void native_moves_and_categories(void)
{
    assert(!Combat_GetMoveInfo(0)&&!Combat_GetMoveInfo(468)&&!Combat_GetMoveInfo(65535));
    for(unsigned i=1;i<ARENA_PLATINUM_MOVE_COUNT;i++) {
        const CombatMoveInfo *m=Combat_GetMoveInfo(i);
        const struct ArenaMoveProfile *p=ArenaMoves_Get(i);
        assert(m&&m->move==i&&m->name&&m->pp&&m->category<=2);
        assert(p&&p->move==i&&p->kind!=ARENA_MOVE_NONE);
        if(m->category==2)assert(m->power==0);
    }
    assert(Combat_GetMoveInfo(MOVE_FIRE_PUNCH)->category==0);
    assert(Combat_GetMoveInfo(MOVE_SHADOW_BALL)->category==1);
    assert(Combat_GetMoveInfo(MOVE_CLOSE_COMBAT)->power==120);
    assert(Combat_GetMoveInfo(MOVE_LEAF_BLADE)->power==90);
    assert(Combat_GetMoveInfo(MOVE_SHADOW_FORCE)->pp==5);
    reset(0);tick(0,COMBAT_A);advance(35);
    unsigned low=800u-match.fighters[1].hp;
    reset(0);match.fighters[0].spAttack=2000;tick(0,COMBAT_A);advance(35);
    assert(800u-match.fighters[1].hp==low); /* Fire Punch uses physical attack. */
    reset(0);match.fighters[0].attack=300;tick(0,COMBAT_A);advance(35);
    assert(800u-match.fighters[1].hp>low);
    reset(0);match.fighters[0].moves[0]=MOVE_SHADOW_BALL;
    match.fighters[0].spAttack=20;tick(0,COMBAT_A);advance(55);
    low=800u-match.fighters[1].hp;
    reset(0);match.fighters[0].moves[0]=MOVE_SHADOW_BALL;
    match.fighters[0].spAttack=160;tick(0,COMBAT_A);advance(55);
    assert(800u-match.fighters[1].hp>low); /* Ghost special move uses Sp. Attack. */
}
static void stamina_is_for_mobility_and_defense(void)
{
    reset(0);match.fighters[0].stamina=0;
    uint8_t pp=match.fighters[0].pp[0];tick(0,COMBAT_A);
    assert(match.fighters[0].phase==COMBAT_WINDUP&&match.fighters[0].pp[0]==pp-1);
    reset(0);match.fighters[0].stamina=0;tick(0,COMBAT_L);
    assert(match.fighters[0].phase==COMBAT_WINDUP&&match.fighters[0].basicAttack);
    reset(0);tick(COMBAT_DOWN,COMBAT_B);
    assert(match.fighters[0].stamina==78&&match.fighters[0].dashTimer);
    advance(54);assert(match.fighters[0].stamina==100);
    reset(0);tick(0,COMBAT_B);assert(match.fighters[0].stamina==94);
    advance(24);assert(match.fighters[0].stamina==100);
}
static void status_uses_pp_without_damage(void)
{
    reset(0);match.fighters[0].selected=3;
    uint8_t pp=match.fighters[0].pp[3];tick(0,COMBAT_A);advance(25);
    assert(match.fighters[0].pp[3]==pp-1&&match.fighters[1].hp==800);
    assert(match.fighters[1].stages[COMBAT_STAT_DEFENSE]==-1);
    reset(0);match.fighters[0].moves[0]=MOVE_RECOVER;
    match.fighters[0].pp[0]=Combat_GetMoveInfo(MOVE_RECOVER)->pp;
    match.fighters[0].hp=200;tick(0,COMBAT_A);advance(25);
    assert(match.fighters[0].hp==600);
}
static void switching_control_preserves_actor_identity(void)
{
    reset(1);Combat before=match;
    assert(!Combat_SetControlledFighter(&match,1));
    assert(!Combat_SetControlledFighter(&match,4));
    assert(!Combat_SetControlledFighter(0,0));
    assert(!memcmp(&before,&match,sizeof(match)));
    match.fighters[2].pp[0]=3;match.projectiles[0].owner=2;
    assert(Combat_SetControlledFighter(&match,2)&&match.controlActor==2);
    int32_t x=match.fighters[2].x;
    tick(COMBAT_RIGHT,COMBAT_A);
    assert(match.fighters[2].pp[0]==2&&match.fighters[2].x>x);
    assert(match.fighters[0].pp[0]==Combat_GetMoveInfo(configs[0].moves[0])->pp);
    assert(match.projectiles[0].owner==2);
    reset(1);assert(Combat_SetControlledFighter(&match,2));
    match.hitstop=3;tick(COMBAT_DOWN,COMBAT_B);advance(3);
    assert(match.fighters[2].dashTimer&&!match.fighters[0].dashTimer);
    match.fighters[0].pp[0]=0;
    const uint8_t actual[4]={24,19,8,64};Combat_SetMovePP(&match,0,actual);
    assert(!memcmp(match.fighters[0].pp,actual,4)); /* PP Up values are retained. */
    match.fighters[0].hp=0;assert(!Combat_SetControlledFighter(&match,0));
}
int main(void)
{
    native_moves_and_categories();stamina_is_for_mobility_and_defense();
    status_uses_pp_without_damage();switching_control_preserves_actor_identity();
    puts("PASS: all Platinum move profiles, Gen IV categories, PP bonuses, action status, fluid stamina and stable ally control");
    return 0;
}
