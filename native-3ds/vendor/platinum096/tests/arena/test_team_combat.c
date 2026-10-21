/* Four live actors share one arena. These checks exercise team outcomes and
 * real attack geometry, rather than emulating doubles as successive duels. */
#include "arena/combat.h"
#include "arena/vendor/arena_navigation.h"
#include "arena/vendor/constants/moves.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define Q 256
static Combat match;
static const CombatFighterConfig configs[COMBAT_FIGHTERS]={
    {25,180,80,70,90,{MOVE_THUNDER_SHOCK,MOVE_QUICK_ATTACK,MOVE_POUND,MOVE_PROTECT},{13,255},90,70},
    {7,180,70,80,70,{MOVE_WATER_GUN,MOVE_TACKLE,MOVE_BUBBLE,MOVE_PROTECT},{11,255},70,80},
    {1,180,80,70,80,{MOVE_THUNDER_SHOCK,MOVE_POUND,MOVE_FLAMETHROWER,MOVE_PROTECT},{12,255},90,70},
    {4,180,70,80,70,{MOVE_EMBER,MOVE_SCRATCH,MOVE_FLAMETHROWER,MOVE_PROTECT},{10,255},70,80}
};
static void reset(void)
{
    Combat_InitTeams(&match,configs);match.aiEnabled=0;match.fighters[0].selected=0;
    for(unsigned i=0;i<ARENA_OBSTACLES;i++) ArenaNav_SetObstacle(i,0);
}
static void place(unsigned side,int x,int y)
{
    CombatFighter *f=&match.fighters[side];
    f->x=f->previousX=x*Q;f->y=f->previousY=y*Q;f->vx=f->vy=0;
}
static void tick(unsigned held,unsigned pressed)
{ Combat_Tick(&match,(CombatInput){(uint16_t)held,(uint16_t)pressed}); }
static void advance(unsigned frames)
{ while(frames--) tick(0,0); }
static void shot(unsigned index,unsigned owner,int x,int y,int vx,uint16_t move)
{
    CombatProjectile *p=&match.projectiles[index];
    memset(p,0,sizeof(*p));p->x=p->previousX=x*Q;p->y=p->previousY=y*Q;
    p->vx=(int16_t)(vx*Q);p->life=30;p->owner=(uint8_t)owner;p->move=move;p->radius=4;
}
static void initialization_and_targets(void)
{
    Combat_Init(&match,configs);
    assert(match.fighterCount==2&&!match.fighters[2].hp&&!match.fighters[3].hp);
    assert(!Combat_ReplaceFighter(&match,2,&configs[2]));
    assert(Combat_TargetFighter(&match,0)==1);
    Combat_InitTeams(&match,configs);
    assert(match.fighterCount==4);
    for(unsigned i=0;i<COMBAT_FIGHTERS;i++)
        assert(ArenaNav_CanStand(match.fighters[i].x/Q,match.fighters[i].y/Q));
    reset();place(0,80,100);place(1,190,100);place(2,81,120);place(3,120,100);
    assert(Combat_TargetFighter(&match,0)==3); /* the adjacent ally is ignored */
    match.fighters[3].hp=0;assert(Combat_TargetFighter(&match,0)==1);
    match.fighters[1].hp=0;assert(Combat_TargetFighter(&match,0)==COMBAT_FIGHTERS);
    assert(Combat_TargetFighter(&match,4)==COMBAT_FIGHTERS);
    assert(Combat_TargetFighter(0,0)==COMBAT_FIGHTERS);
}
static void whole_team_defeat(void)
{
    reset();match.fighters[1].hp=0;tick(0,0);assert(!match.winner);
    match.fighters[3].hp=0;tick(0,0);assert(match.winner==1);
    /* A surviving ally keeps the battle live even if the player has fainted. */
    reset();match.fighters[0].hp=0;tick(0,0);assert(!match.winner);
    int32_t x=match.fighters[0].x;
    tick(COMBAT_RIGHT|COMBAT_A,COMBAT_X);
    assert(!match.fighters[0].hp&&match.fighters[0].x==x);
    assert(match.fighters[0].phase==COMBAT_READY);
    match.fighters[2].hp=0;tick(0,0);assert(match.winner==2);
    assert(Combat_ReplaceFighter(&match,2,&configs[2]));
    assert(!match.winner&&match.fighters[2].hp==configs[2].hp);
    assert(Combat_ReplaceFighter(&match,3,&configs[3]));
    assert(!Combat_ReplaceFighter(&match,4,&configs[0]));
}
static void melee_and_cones_hit_both_enemies(void)
{
    reset();match.lockAim=0;match.fighters[0].direction=COMBAT_E;
    place(0,80,100);place(1,105,92);place(2,93,100);place(3,105,108);
    tick(0,COMBAT_L);advance(20);
    assert(match.fighters[1].hp<configs[1].hp&&match.fighters[3].hp<configs[3].hp);
    assert(match.fighters[2].hp==configs[2].hp&&match.hits==2);
    reset();match.lockAim=0;match.fighters[0].direction=COMBAT_E;
    match.fighters[0].moves[0]=MOVE_FLAMETHROWER;
    match.fighters[0].pp[0]=Combat_GetMoveInfo(MOVE_FLAMETHROWER)->pp;
    place(0,60,100);place(1,120,92);place(2,100,100);place(3,120,108);
    tick(0,COMBAT_A);advance(50);
    assert(match.fighters[1].hp<configs[1].hp&&match.fighters[3].hp<configs[3].hp);
    assert(match.fighters[2].hp==configs[2].hp&&match.hits==2);
}
static void projectiles_ignore_allies_and_choose_first_enemy(void)
{
    reset();place(0,60,100);place(1,140,100);place(2,85,100);place(3,105,100);
    shot(0,0,60,100,100,MOVE_THUNDER_SHOCK);tick(0,0);
    assert(match.fighters[2].hp==configs[2].hp);
    assert(match.fighters[3].hp<configs[3].hp);
    assert(match.fighters[1].hp==configs[1].hp&&!match.projectiles[0].life);
    /* Allied shots pass through each other; opposing shots still clash. */
    reset();place(0,40,180);place(1,250,180);place(2,40,220);place(3,250,220);
    shot(0,0,80,100,20,MOVE_WATER_GUN);shot(1,2,120,100,-20,MOVE_WATER_GUN);
    tick(0,0);assert(match.projectiles[0].life&&match.projectiles[1].life);
    reset();place(0,40,180);place(1,250,180);place(2,40,220);place(3,250,220);
    shot(0,0,80,100,20,MOVE_WATER_GUN);shot(1,3,120,100,-20,MOVE_WATER_GUN);
    tick(0,0);assert(!match.projectiles[0].life&&!match.projectiles[1].life);
}
static void fainted_drain_owner_cannot_revive(void)
{
    reset();place(0,40,180);place(1,120,100);place(2,60,100);place(3,250,180);
    match.fighters[2].hp=0;
    shot(0,2,95,100,30,MOVE_GIGA_DRAIN);tick(0,0);
    assert(match.fighters[1].hp<configs[1].hp&&!match.fighters[2].hp);
    advance(100);assert(!match.fighters[2].hp);
}
static void separate_all_living_bodies(void)
{
    reset();place(0,60,100);place(1,220,100);place(2,60,100);place(3,220,100);
    tick(0,0);
    assert(match.fighters[0].x<match.fighters[2].x);
    assert(match.fighters[1].x<match.fighters[3].x);
    reset();place(0,60,100);place(2,60,100);match.fighters[2].hp=0;
    tick(0,0);assert(match.fighters[0].x==60*Q); /* corpses do not obstruct */
}
static void allied_ai_fights_after_player_faints(void)
{
    reset();match.aiEnabled=1;
    place(0,40,180);place(1,145,100);place(2,80,100);place(3,240,180);
    match.fighters[0].hp=0;
    match.fighters[2].hp=match.fighters[2].maxHp=800;
    match.fighters[2].spAttack=180;match.fighters[2].attack=180;
    match.fighters[2].defense=match.fighters[2].spDefense=180;
    for(unsigned i=0;i<600&&!match.winner;i++) tick(0,0);
    assert(match.fighters[2].pp[0]<Combat_GetMoveInfo(configs[2].moves[0])->pp);
    assert(match.fighters[1].hp<configs[1].hp||match.fighters[3].hp<configs[3].hp);
    assert(!match.fighters[0].hp);
    for(unsigned i=0;i<10000&&!match.winner;i++) tick(0,0);
    assert(match.winner==1||match.winner==2);
    assert(!match.fighters[0].hp);
    if(match.winner==1)assert(match.fighters[2].hp&&!match.fighters[1].hp&&!match.fighters[3].hp);
    else assert(!match.fighters[2].hp&&(match.fighters[1].hp||match.fighters[3].hp));
}
static void deterministic_live_match(void)
{
    Combat expected;
    reset();match.aiEnabled=1;
    for(unsigned i=0;i<3000&&!match.winner;i++)
        tick((i/120)&1?COMBAT_A:COMBAT_ALT,(i%150)==0?COMBAT_X:0);
    assert(match.hits&&match.fighters[2].pp[0]<Combat_GetMoveInfo(configs[2].moves[0])->pp);
    expected=match;
    reset();match.aiEnabled=1;
    for(unsigned i=0;i<3000&&!match.winner;i++)
        tick((i/120)&1?COMBAT_A:COMBAT_ALT,(i%150)==0?COMBAT_X:0);
    assert(!memcmp(&match,&expected,sizeof(match)));
}
int main(void)
{
    initialization_and_targets();whole_team_defeat();melee_and_cones_hit_both_enemies();
    projectiles_ignore_allies_and_choose_first_enemy();fainted_drain_owner_cannot_revive();
    separate_all_living_bodies();allied_ai_fights_after_player_faints();deterministic_live_match();
    puts("PASS: simultaneous 2v2 teams, nearest targets, allied AI, friendly-fire prevention, whole-team defeat, faint safety and determinism");
    return 0;
}
