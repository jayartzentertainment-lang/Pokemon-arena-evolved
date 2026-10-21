/* Player-facing 0.9.3 regression cases, using the production simulation. */
#include "arena/combat.h"
#include "arena/vendor/arena_moves.h"
#include "arena/vendor/arena_navigation.h"
#include "arena/vendor/constants/moves.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define Q 256
static Combat match;
static const CombatFighterConfig configs[4]={
    {390,800,100,100,70,{MOVE_FIRE_PUNCH,MOVE_FLAMETHROWER,MOVE_HYDRO_PUMP,MOVE_TACKLE},{10,255},100,100},
    {393,800,100,100,70,{MOVE_WATER_GUN,MOVE_FLAMETHROWER,MOVE_HYDRO_PUMP,MOVE_TACKLE},{11,255},100,100},
    {387,800,100,100,70,{MOVE_FLAMETHROWER,MOVE_TACKLE,0,0},{12,255},100,100},
    {403,800,100,100,70,{MOVE_HYDRO_PUMP,MOVE_TACKLE,0,0},{13,255},100,100}
};
static void tick(unsigned held,unsigned pressed)
{ Combat_Tick(&match,(CombatInput){(uint16_t)held,(uint16_t)pressed}); }
static void place(unsigned i,int x,int y)
{
    match.fighters[i].x=match.fighters[i].previousX=x*Q;
    match.fighters[i].y=match.fighters[i].previousY=y*Q;
}
static void reset(int doubles)
{
    if(doubles) Combat_InitTeams(&match,configs);else Combat_Init(&match,configs);
    match.aiEnabled=0;match.fighters[0].selected=0;
    for(unsigned i=0;i<ARENA_OBSTACLES;i++) ArenaNav_SetObstacle(i,0);
    place(0,70,100);place(1,200,100);
    if(doubles) { place(2,70,220);place(3,200,220); }
}
static void shot(unsigned slot,unsigned owner,int x,int y,int vx,uint16_t move)
{
    CombatProjectile *p=&match.projectiles[slot];
    memset(p,0,sizeof(*p));p->x=p->previousX=x*Q;p->y=p->previousY=y*Q;
    p->vx=vx*Q;p->life=20;p->radius=4;p->move=move;p->owner=owner;
}
static void held_shield_and_exhaustion(void)
{
    reset(0);tick(COMBAT_B,COMBAT_B);
    for(unsigned i=0;i<100;i++) tick(COMBAT_B,0);
    assert(match.fighters[0].guardTimer&&match.fighters[0].guardHeld);
    assert(match.fighters[0].stamina>0&&match.fighters[0].stamina<90);
    tick(COMBAT_B|COMBAT_UP,0); /* Moving direction while holding is no new dodge. */
    assert(match.fighters[0].guardTimer&&!match.fighters[0].dashTimer);
    tick(0,0);assert(!match.fighters[0].guardTimer&&!match.fighters[0].guardHeld);
    reset(0);tick(COMBAT_B,COMBAT_B);
    for(unsigned i=0;i<600;i++) tick(COMBAT_B,0);
    assert(!match.fighters[0].guardTimer&&match.fighters[0].guardExhausted);
    assert(match.fighters[0].stamina>0); /* It recovers without shield flicker. */
    tick(0,0);tick(COMBAT_B,COMBAT_B);assert(match.fighters[0].guardTimer);
    reset(0);match.fighters[0].stamina=0;tick(COMBAT_B,COMBAT_B);
    assert(!match.fighters[0].guardTimer);
}
static void guard_pressure_and_events(void)
{
    unsigned unguarded;
    reset(0);shot(0,1,90,100,-20,MOVE_WATER_GUN);tick(COMBAT_B,COMBAT_B);
    assert(match.fighters[0].hp==800&&match.blocked==1);
    assert(match.impactKind==COMBAT_IMPACT_BLOCK&&match.impactActor==0&&match.impactTimer);
    reset(0);shot(0,1,90,100,-20,MOVE_HYDRO_PUMP);tick(0,0);
    unguarded=800-match.fighters[0].hp;
    reset(0);shot(0,1,90,100,-20,MOVE_HYDRO_PUMP);tick(COMBAT_B,COMBAT_B);
    assert(match.fighters[0].hp<800&&800-match.fighters[0].hp<(int)unguarded);
    assert(match.blocked==1&&!match.hits&&!match.fighters[0].hurtTimer);
    reset(0);match.fighters[0].stamina=6;
    shot(0,1,90,100,-20,MOVE_WATER_GUN);tick(COMBAT_B,COMBAT_B);
    assert(!match.fighters[0].guardTimer&&match.fighters[0].hp<800);
}
static void beam(unsigned actor,uint16_t move,unsigned direction)
{
    CombatFighter *f=&match.fighters[actor];
    f->actionMove=move;f->phase=COMBAT_WINDUP;f->attackDirection=direction;
    f->actionTick=ArenaMoves_Get(move)->windup;
    if(Combat_GetMoveInfo(move)->power>=80)f->actionTick+=(f->actionTick+7)/8;
}
static void beam_clashes_and_friendly_fire(void)
{
    reset(0);place(0,70,100);place(1,200,100);
    beam(0,MOVE_FLAMETHROWER,COMBAT_E);beam(1,MOVE_HYDRO_PUMP,COMBAT_W);
    tick(0,0);
    assert(match.clashes==1&&match.impactKind==COMBAT_IMPACT_CLASH);
    assert(match.impactActor==255&&match.fighters[0].hp==800&&match.fighters[1].hp==800);
    assert(match.fighters[0].phase==COMBAT_ACTIVE&&match.fighters[1].phase==COMBAT_ACTIVE);
    assert(Combat_BeamContact(&match,0,0,0)&&Combat_BeamContact(&match,1,0,0));
    assert(match.impactX>70*Q&&match.impactX<200*Q);
    int32_t startX=match.fighters[0].x;
    for(unsigned i=0;i<40;i++)tick(COMBAT_RIGHT,0);
    assert(Combat_BeamContact(&match,0,0,0)&&match.fighters[0].x==startX);
    assert(match.fighters[0].hp==800&&match.fighters[1].hp==800&&match.clashes==1);
    for(unsigned i=0;i<100&&Combat_BeamContact(&match,0,0,0);i++)tick(0,0);
    assert(!Combat_BeamContact(&match,0,0,0)&&match.clashes==1);
    assert(match.beamClashes[0].phase==COMBAT_CLASH_BURST);
    reset(0);place(1,200,130); /* Parallel, offset beams do not falsely collide. */
    beam(0,MOVE_FLAMETHROWER,COMBAT_E);beam(1,MOVE_HYDRO_PUMP,COMBAT_W);tick(0,0);
    assert(!match.clashes);
    reset(1);place(2,200,100);
    beam(0,MOVE_FLAMETHROWER,COMBAT_E);beam(2,MOVE_HYDRO_PUMP,COMBAT_W);tick(0,0);
    assert(!match.clashes&&match.fighters[2].hp==800);
    /* Fast shots cross between endpoints; pool ordering never changes clash. */
    for(unsigned reverse=0;reverse<2;reverse++) {
        reset(0);place(0,40,200);place(1,240,200);
        shot(reverse,0,90,100,80,MOVE_WATER_GUN);
        shot(1-reverse,1,170,100,-80,MOVE_WATER_GUN);tick(0,0);
        assert(match.clashes==1&&!match.projectiles[0].life&&!match.projectiles[1].life);
    }
}
static void simultaneous_clashes_and_replacement(void)
{
    reset(1);
    beam(0,MOVE_FLAMETHROWER,COMBAT_E);beam(1,MOVE_HYDRO_PUMP,COMBAT_W);
    beam(2,MOVE_FLAMETHROWER,COMBAT_E);beam(3,MOVE_HYDRO_PUMP,COMBAT_W);
    tick(0,0);assert(match.clashes==2);
    for(unsigned i=0;i<4;i++)assert(Combat_BeamContact(&match,i,0,0));
    assert(Combat_ReplaceFighter(&match,0,&configs[0]));
    assert(!Combat_BeamContact(&match,0,0,0)&&!Combat_BeamContact(&match,1,0,0));
    assert(Combat_BeamContact(&match,2,0,0)&&Combat_BeamContact(&match,3,0,0));
    assert(Combat_SetControlledFighter(&match,2));
    tick(COMBAT_LEFT,0);assert(Combat_BeamContact(&match,2,0,0));
    assert(match.fighters[0].phase==COMBAT_READY&&!match.fighters[0].actionSerial);
}
static void release_events_and_pose_timing(void)
{
    const struct ArenaMoveProfile *p=ArenaMoves_Get(MOVE_FLAMETHROWER);
    reset(0);tick(0,COMBAT_A);
    assert(match.fighters[0].actionSerial==1&&!match.fighters[0].releaseSerial);
    for(unsigned i=0;i<40&&!match.fighters[0].releaseSerial;i++)tick(0,0);
    assert(match.fighters[0].releaseSerial==1&&match.fighters[0].releaseMove==MOVE_FIRE_PUNCH);
    for(unsigned i=0;i<100;i++)tick(0,0);
    assert(match.fighters[0].releaseSerial==1);
    CombatFighter *f=&match.fighters[0];
    f->actionMove=MOVE_FLAMETHROWER;f->phase=COMBAT_WINDUP;f->actionTick=p->windup;
    assert(Combat_AnimationTick(f,28,10)<10);
    f->phase=COMBAT_ACTIVE;
    for(unsigned i=0;i<60;i++) {
        f->actionTick=p->windup+i;assert(Combat_AnimationTick(f,28,10)==10);
    }
    f->phase=COMBAT_RECOVERY;f->actionTick=65535;
    assert(Combat_AnimationTick(f,28,10)==27);
    f->guardTimer=1;assert(Combat_AnimationTick(f,28,10)==0);
    assert(ArenaMoves_Beam(MOVE_ICE_BEAM)&&ArenaMoves_Get(MOVE_ICE_BEAM)->kind==ARENA_MOVE_CONE);
    assert(!ArenaMoves_Beam(MOVE_WATER_GUN)&&!ArenaMoves_Beam(MOVE_DRAGON_BREATH));
    reset(0);assert(!match.clashPractice);Combat_SetClashPractice(&match,1);
    match.aiEnabled=1;match.fighters[0].selected=1;tick(0,COMBAT_A);
    for(unsigned i=0;i<100&&!match.clashes;i++)tick(0,0);
    assert(match.clashes==1&&Combat_BeamContact(&match,0,0,0));
    reset(0);assert(!match.clashPractice);
}
static void critical_hits_have_real_damage(void)
{
    unsigned normal=0,critical=0;
    for(unsigned seed=0;seed<64;seed++) {
        reset(0);match.random=seed;
        shot(0,1,90,100,-20,MOVE_WATER_GUN);tick(0,0);
        if(match.criticals) {
            critical=800-match.fighters[0].hp;
            assert(match.impactKind==COMBAT_IMPACT_CRITICAL&&match.impactActor==0);
        } else normal=800-match.fighters[0].hp;
    }
    assert(normal&&critical==normal*2);
}
static void movement_stays_responsive(void)
{
    int32_t walking,attacking;
    reset(0);for(unsigned i=0;i<8;i++) tick(COMBAT_RIGHT,0);
    walking=match.fighters[0].x-70*Q;
    reset(0);tick(COMBAT_RIGHT,COMBAT_A);
    assert(match.fighters[0].actionSerial==1);
    for(unsigned i=1;i<8;i++) tick(COMBAT_RIGHT,0);
    attacking=match.fighters[0].x-70*Q;
    assert(attacking>walking/2&&attacking<=walking);
    reset(0);tick(COMBAT_RIGHT,COMBAT_B);
    for(unsigned i=0;i<11;i++) tick(COMBAT_RIGHT,0);
    assert(match.fighters[0].vx>0);
    int32_t x=match.fighters[0].x;tick(COMBAT_RIGHT,0);
    assert(match.fighters[0].x>x); /* No forced zero-velocity frame after dash. */
    reset(0);place(1,105,100);match.fighters[0].selected=3;tick(0,COMBAT_A);
    for(unsigned i=0;i<35;i++) tick(0,0);
    assert(match.hits&&match.fighters[0].x<match.fighters[1].x);
    assert(ARENA_WORLD_WIDTH==448&&ARENA_WORLD_HEIGHT==320);
}
int main(void)
{
    held_shield_and_exhaustion();guard_pressure_and_events();
    beam_clashes_and_friendly_fire();simultaneous_clashes_and_replacement();release_events_and_pose_timing();
    critical_hits_have_real_damage();movement_stays_responsive();
    puts("PASS: held guard, exhaustion latch, heavy chip, beam/projectile clashes, critical damage/events, fluent movement and larger bounds");
    return 0;
}
