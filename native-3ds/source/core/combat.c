/* Native portable DS battle simulation, authored for this port.
 * Reuses the supplied Emerald Arena move geometry, navigation, physics and
 * Arena Plus momentum helpers; copied modules retain original provenance.
 * This action ruleset implements damage, drain and Protect. Native Gen IV move categories and PP are preserved; complex scripted effects
 * use bounded action adapters rather than the turn-based script interpreter.
 */
#include "arena/combat.h"
#include "arena/move_data.h"
#include "arena/vendor/arena_moves.h"
#include "arena/vendor/arena_navigation.h"
#include "arena/vendor/arena_physics.h"
#include "arena/vendor/arena_plus.h"
#include "arena/vendor/constants/moves.h"
#include <string.h>
#define Q 256
#define RGB(r,g,b) ((r)|((g)<<5)|((b)<<10))
typedef struct {
    uint8_t accuracy,status,chance,selfTarget,selfStage,heal,special;
    int8_t stages[COMBAT_STAT_COUNT];
} CombatNativeRule;
#include "combat_native_rules.inc"
static const int16_t sDirections[8][2] = {
    {0,256},{181,181},{256,0},{181,-181},
    {0,-256},{-181,-181},{-256,0},{-181,181}
};
static int32_t Abs(int32_t x) { return x<0?-x:x; }
static int32_t Clamp(int32_t x,int32_t lo,int32_t hi) { return x<lo?lo:x>hi?hi:x; }
static int Sign(int32_t x) { return x>0?1:x<0?-1:0; }
static uint16_t NextRandom(Combat *c) { c->random=c->random*1664525u+1013904223u; return c->random>>16; }
static const CombatNativeRule *NativeRule(uint16_t move)
{ return &sNativeRules[move<468?move:0]; }
static unsigned Staged(unsigned value,int stage,int accuracy)
{
    unsigned base=accuracy?3:2;
    stage=(int)Clamp(stage,-6,6);
    return stage>=0?value*(base+stage)/base:value*base/(base-stage);
}
int Combat_ChangeStage(Combat *c,unsigned side,unsigned stat,int delta)
{
    int old;
    if(!c||side>=c->fighterCount||stat>=COMBAT_STAT_COUNT)return 0;
    old=c->fighters[side].stages[stat];
    c->fighters[side].stages[stat]=(int8_t)Clamp(old+delta,-6,6);
    return c->fighters[side].stages[stat]-old;
}
uint16_t Combat_NaturalMove(const CombatFighter *f)
{ return f&&f->species<496?sNaturalMoves[f->species][f->lightChoice&1]:MOVE_STRUGGLE; }
void Combat_SetNativeStatus(Combat *c,unsigned side,uint32_t value)
{
    CombatFighter *f;unsigned status;
    if(!c||side>=c->fighterCount)return;
    f=&c->fighters[side];
    status=(value&7)?COMBAT_STATUS_SLEEP:(value&128)?COMBAT_STATUS_BAD_POISON:
        (value&8)?COMBAT_STATUS_POISON:(value&16)?COMBAT_STATUS_BURN:
        (value&32)?COMBAT_STATUS_FREEZE:(value&64)?COMBAT_STATUS_PARALYSIS:COMBAT_STATUS_NONE;
    if(f->status==status)return;
    f->status=(uint8_t)status;f->statusClock=0;f->toxicCounter=1;
    f->statusTicks=status==COMBAT_STATUS_SLEEP?(uint16_t)((value&7)*60):status==COMBAT_STATUS_FREEZE?480:0;
    f->statusLock=0;
    if(status==COMBAT_STATUS_SLEEP||status==COMBAT_STATUS_FREEZE) {
        f->phase=COMBAT_READY;f->actionTick=f->actionMove=0;
        f->dashTimer=f->guardTimer=f->guardHeld=0;f->vx=f->vy=0;f->castRoot=f->preciseAim=0;f->beamReach=0;
    }
}
uint32_t Combat_GetNativeStatus(const Combat *c,unsigned side)
{
    const CombatFighter *f;
    if(!c||side>=c->fighterCount)return 0;
    f=&c->fighters[side];
    switch(f->status) {
    case COMBAT_STATUS_SLEEP:return (uint32_t)Clamp((f->statusTicks+59)/60,1,7);
    case COMBAT_STATUS_POISON:return 8;
    case COMBAT_STATUS_BAD_POISON:return 128;
    case COMBAT_STATUS_BURN:return 16;
    case COMBAT_STATUS_FREEZE:return 32;
    case COMBAT_STATUS_PARALYSIS:return 64;
    default:return 0;
    }
}
static uint8_t Facing(int32_t dx,int32_t dy)
{
    if(Abs(dx)>Abs(dy)*2) return dx>0?COMBAT_E:COMBAT_W;
    if(Abs(dy)>Abs(dx)*2) return dy>0?COMBAT_S:COMBAT_N;
    if(!dx&&!dy) return COMBAT_S;
    return dx>0?(dy>0?COMBAT_SE:COMBAT_NE):(dy>0?COMBAT_SW:COMBAT_NW);
}
static int Enemies(unsigned one,unsigned two) { return (one&1)!=(two&1); }
unsigned Combat_TargetFighter(const Combat *c,unsigned side)
{
    unsigned target=COMBAT_FIGHTERS,i;
    int32_t best=0x7fffffff;
    if(!c||side>=c->fighterCount) return target;
    if(side==c->controlActor&&c->lockAim&&c->lockTarget<c->fighterCount&&
        Enemies(side,c->lockTarget)&&c->fighters[c->lockTarget].hp)return c->lockTarget;
    for(i=0;i<c->fighterCount;i++) if(Enemies(side,i)&&c->fighters[i].hp) {
        int32_t dx=(c->fighters[i].x-c->fighters[side].x)/Q;
        int32_t dy=(c->fighters[i].y-c->fighters[side].y)/Q;
        int32_t distance=dx*dx+dy*dy;
        if(distance<best) { best=distance;target=i; }
    }
    return target;
}
const CombatMoveInfo *Combat_GetMoveInfo(uint16_t move)
{ return ArenaPlatinum_GetMoveInfo(move); }
void Combat_SetMovePP(Combat *c,unsigned side,const uint8_t pp[4])
{
    unsigned i;
    if(!c||!pp||side>=c->fighterCount) return;
    for(i=0;i<4;i++) c->fighters[side].pp[i]=c->fighters[side].moves[i]?pp[i]:0;
}
uint16_t Combat_TypeColor(uint8_t type)
{
    static const uint16_t colors[18]={
        RGB(29,27,21),RGB(29,12,7),RGB(17,22,31),RGB(23,10,28),
        RGB(25,18,8),RGB(24,23,14),RGB(20,28,6),RGB(16,10,26),
        RGB(22,25,28),RGB(24,24,24),RGB(31,12,2),RGB(6,21,31),
        RGB(10,29,9),RGB(31,29,3),RGB(31,9,23),RGB(17,31,31),
        RGB(15,12,31),RGB(16,14,17)
    };
    return colors[type<18?type:0];
}
/* Gen IV retains these type IDs and Steel resistance to
 * Ghost/Dark. Immunities take precedence over dual-type resistance. */
#define T(n) (1u<<(n))
static uint16_t TypeSingle(uint8_t attack,uint8_t defense)
{
    uint32_t weak=0,resist=0,immune=0;
    switch(attack) {
    case 0: resist=T(5)|T(8); immune=T(7); break;
    case 1: weak=T(0)|T(5)|T(8)|T(15)|T(17);resist=T(2)|T(3)|T(6)|T(14);immune=T(7);break;
    case 2: weak=T(1)|T(6)|T(12);resist=T(5)|T(8)|T(13);break;
    case 3: weak=T(12);resist=T(3)|T(4)|T(5)|T(7);immune=T(8);break;
    case 4: weak=T(3)|T(5)|T(8)|T(10)|T(13);resist=T(6)|T(12);immune=T(2);break;
    case 5: weak=T(2)|T(6)|T(10)|T(15);resist=T(1)|T(4)|T(8);break;
    case 6: weak=T(12)|T(14)|T(17);resist=T(1)|T(2)|T(3)|T(7)|T(8)|T(10);break;
    case 7: weak=T(7)|T(14);resist=T(8)|T(17);immune=T(0);break;
    case 8: weak=T(5)|T(15);resist=T(8)|T(10)|T(11)|T(13);break;
    case 10:weak=T(6)|T(8)|T(12)|T(15);resist=T(5)|T(10)|T(11)|T(16);break;
    case 11:weak=T(4)|T(5)|T(10);resist=T(11)|T(12)|T(16);break;
    case 12:weak=T(4)|T(5)|T(11);resist=T(2)|T(3)|T(6)|T(8)|T(10)|T(12)|T(16);break;
    case 13:weak=T(2)|T(11);resist=T(12)|T(13)|T(16);immune=T(4);break;
    case 14:weak=T(1)|T(3);resist=T(8)|T(14);immune=T(17);break;
    case 15:weak=T(2)|T(4)|T(12)|T(16);resist=T(8)|T(10)|T(11)|T(15);break;
    case 16:weak=T(16);resist=T(8);break;
    case 17:weak=T(7)|T(14);resist=T(1)|T(8)|T(17);break;
    default:break;
    }
    if(defense>=18) return Q;
    if(immune&T(defense)) return 0;
    return weak&T(defense)?512:resist&T(defense)?128:256;
}
#undef T
uint16_t Combat_TypeMultiplier(uint8_t attack,uint8_t one,uint8_t two)
{
    uint16_t result=TypeSingle(attack,one);
    return two==one?result:result*TypeSingle(attack,two)/Q;
}
static CombatEffect *Effect(Combat *c,int32_t x,int32_t y,uint16_t color,uint8_t kind,uint8_t radius)
{
    CombatEffect *e;
    unsigned i;
    for(i=0;i<COMBAT_EFFECTS;i++) if(!c->effects[i].life) break;
    if(i==COMBAT_EFFECTS) i=c->effectCursor++%COMBAT_EFFECTS;
    e=&c->effects[i];
    e->x=x;e->y=y;e->kind=kind;e->color=color;e->radius=radius;e->move=0;
    e->life=e->maxLife=kind==COMBAT_SPARK?14:kind==COMBAT_CLASH?24:kind==COMBAT_DASH_TRAIL?10:18;
    e->vx=kind==COMBAT_SPARK?(int16_t)((NextRandom(c)&511)-256):0;
    e->vy=kind==COMBAT_SPARK?(int16_t)(-100-(NextRandom(c)&255)):0;
    e->direction=COMBAT_S;
    return e;
}
static void Burst(Combat *c,int32_t x,int32_t y,uint16_t color)
{
    unsigned i;
    Effect(c,x,y,color,COMBAT_RING,3);
    for(i=0;i<5;i++) Effect(c,x,y,color,COMBAT_SPARK,1);
}
static void Impact(Combat *c,uint8_t kind,unsigned actor,int32_t x,int32_t y)
{
    c->impactSerial++;c->impactKind=kind;c->impactActor=(uint8_t)actor;
    c->impactX=x;c->impactY=y;c->impactTimer=kind==COMBAT_IMPACT_CRITICAL?22:18;
}
static void Clash(Combat *c,int32_t x,int32_t y)
{
    c->clashes++;c->hitstop=3;
    Burst(c,x,y,RGB(31,31,31));
    Effect(c,x,y,RGB(24,29,31),COMBAT_CLASH,20);
    Impact(c,COMBAT_IMPACT_CLASH,255,x,y);
}
static unsigned Root(unsigned value)
{
    unsigned result=0,bit=1u<<30;
    while(bit>value)bit>>=2;
    while(bit){if(value>=result+bit){value-=result+bit;result=(result>>1)+bit;}else result>>=1;bit>>=2;}
    return result;
}
static void Normalize(int x,int y,int16_t *ox,int16_t *oy)
{
    unsigned length;
    x=(int)Clamp(x,-32767,32767);y=(int)Clamp(y,-32767,32767);
    /* Retain fractional direction precision even for a nearby target. */
    while((Abs(x)>1024||Abs(y)>1024)){x/=2;y/=2;}
    x*=16;y*=16;length=Root((unsigned)(x*x+y*y));
    if(!length){*ox=0;*oy=256;return;}
    *ox=(int16_t)(x*256/(int)length);*oy=(int16_t)(y*256/(int)length);
}
void Combat_ConfigureMobility(Combat *c,unsigned actor,unsigned speed,unsigned hp,unsigned defense)
{
    CombatFighter *f;if(!c||actor>=c->fighterCount)return;
    f=&c->fighters[actor];c->nativeBalance=1;
    f->moveSpeed=(uint16_t)Clamp(110+(int)speed*2,150,510);
    f->staminaMax=(uint8_t)Clamp(75+(int)(hp+defense)/8,85,125);
    f->regenStep=(uint8_t)Clamp(1+(int)hp/90,1,3);
    f->dodgeScale=(uint8_t)Clamp((170+(int)speed)/3,65,110);
    f->resistance=(uint8_t)Clamp((int)defense,20,200);
    if(f->stamina==COMBAT_STAMINA_MAX||f->stamina>f->staminaMax)f->stamina=f->staminaMax;
}
static int NativeCharge(uint16_t move)
{
    switch(move){
    case MOVE_TACKLE:case MOVE_TAKE_DOWN:case MOVE_DOUBLE_EDGE:case MOVE_QUICK_ATTACK:
    case MOVE_EXTREME_SPEED:case MOVE_AQUA_JET:case MOVE_FLAME_WHEEL:case MOVE_FLARE_BLITZ:
    case MOVE_VOLT_TACKLE:case MOVE_HEADBUTT:case MOVE_ZEN_HEADBUTT:return 1;
    default:return 0;
    }
}
void Combat_SetAim(Combat *c,int x,int y,int enabled)
{
    if(!c)return;
    c->aimOverride=(uint8_t)(enabled&&(x||y));
    if(c->aimOverride)Normalize(x,y,&c->aimX,&c->aimY);
}
void Combat_AttackVector(const CombatFighter *f,int16_t *x,int16_t *y)
{
    if(!f)return;
    if(x)*x=f->preciseAim?f->attackAimX:sDirections[f->attackDirection&7][0];
    if(y)*y=f->preciseAim?f->attackAimY:sDirections[f->attackDirection&7][1];
}
int Combat_MoveTargetsSelf(uint16_t move){return !!NativeRule(move)->selfTarget;}
int Combat_IsCasting(const CombatFighter *f)
{return f&&f->castRoot&&f->phase!=COMBAT_READY&&f->hp&&!f->hurtTimer;}
static void SteerBeam(Combat *c,unsigned side)
{
    CombatFighter *f=&c->fighters[side];int cross,dot,sign,x,y;
    if(side!=c->controlActor||!c->aimOverride||!f->preciseAim||
       f->phase==COMBAT_READY||!ArenaMoves_Beam(f->actionMove)||Combat_BeamContact(c,side,0,0))return;
    cross=f->attackAimX*c->aimY-f->attackAimY*c->aimX;
    dot=f->attackAimX*c->aimX+f->attackAimY*c->aimY;
    if(dot>0&&Abs(cross)<=13*256){f->attackAimX=c->aimX;f->attackAimY=c->aimY;}
    else{
        sign=cross<0?-1:1;
        x=(f->attackAimX*256-f->attackAimY*13*sign)/256;
        y=(f->attackAimY*256+f->attackAimX*13*sign)/256;
        Normalize(x,y,&f->attackAimX,&f->attackAimY);
    }
    f->attackDirection=Facing(f->attackAimX,f->attackAimY);
}
static unsigned BeamRange(const Combat *c,const CombatFighter *f,const struct ArenaMoveProfile *p,unsigned growth)
{
    int16_t ax,ay;unsigned distance,maximum=p->range*(c->growingBeams?growth:6)/6;
    int px=f->x/Q,py=f->y/Q;
    Combat_AttackVector(f,&ax,&ay);
    for(distance=1;distance<=maximum;distance++){
        int x=px+ax*(int)distance/Q,y=py+ay*(int)distance/Q;
        if(!ArenaNav_LineClear(px,py,x,y,1))break;
    }
    return distance-1;
}
static uint8_t Windup(const CombatFighter *f,const struct ArenaMoveProfile *p)
{
    const CombatMoveInfo *m=Combat_GetMoveInfo(p->move);
    return f->basicAttack?5:(uint8_t)(p->windup+(m&&m->power>=80?(p->windup+7)/8:0));
}
static uint8_t Active(const CombatFighter *f,const struct ArenaMoveProfile *p)
{ return f->basicAttack?6:p->active; }
static uint8_t Recovery(const CombatFighter *f,const struct ArenaMoveProfile *p)
{
    const CombatMoveInfo *m=Combat_GetMoveInfo(p->move);
    unsigned recovery=(unsigned)Clamp(p->recovery*2/3,10,100);
    return f->basicAttack?12:(uint8_t)(recovery+(m&&m->power>=80?(recovery+7)/8:0));
}
void Combat_SetAnalogMagnitude(Combat *c,unsigned magnitude,int enabled)
{
    if(!c)return;
    c->controlMagnitude=(uint16_t)(magnitude>256?256:magnitude);
    c->analogInput=(uint8_t)!!enabled;
}
unsigned Combat_PhaseTick(const CombatFighter *f)
{
    const struct ArenaMoveProfile *p;
    unsigned elapsed,offset=0;
    if(!f||f->phase==COMBAT_READY||!(p=ArenaMoves_Get(f->actionMove)))return 0;
    elapsed=f->actionTick?f->actionTick-1:0;
    if(f->phase==COMBAT_ACTIVE||f->phase==COMBAT_RECOVERY)offset=Windup(f,p);
    if(f->phase==COMBAT_RECOVERY)offset+=Active(f,p);
    return elapsed>offset?elapsed-offset:0;
}
unsigned Combat_PhaseDuration(const CombatFighter *f)
{
    const struct ArenaMoveProfile *p;
    if(!f||!(p=ArenaMoves_Get(f->actionMove)))return 0;
    if(f->phase==COMBAT_WINDUP)return Windup(f,p);
    if(f->phase==COMBAT_ACTIVE)return Active(f,p);
    if(f->phase==COMBAT_RECOVERY)return Recovery(f,p);
    return 0;
}
unsigned Combat_AnimationTick(const CombatFighter *f,unsigned total,unsigned hit)
{
    const struct ArenaMoveProfile *p;
    unsigned time,windup,active,recovery,tail;
    if(!f||!total)return 0;
    if(f->hurtTimer||f->guardTimer)return 0;
    if(f->phase==COMBAT_READY||!(p=ArenaMoves_Get(f->actionMove)))
        return (f->dashTimer?f->animTick*2u:f->animTick)%total;
    if(hit>=total)hit=total-1;
    /* Missing marker: reserve an anticipation third, rather than starting a
     * whole attack loop on each render frame. */
    if(!hit&&total>2)hit=total/3;
    windup=Windup(f,p);active=Active(f,p);recovery=Recovery(f,p);
    time=f->actionTick?f->actionTick-1:0;
    if(f->phase==COMBAT_WINDUP) {
        unsigned before=hit?hit:1;
        return windup?(unsigned)Clamp((int)(time*before/windup),0,(int)before-1):0;
    }
    tail=total-1-hit;
    if(ArenaMoves_Beam(f->actionMove)) {
        if(f->phase==COMBAT_ACTIVE)return hit;
        time=time>windup+active?time-windup-active:0;
        return (unsigned)Clamp((int)(hit+(recovery?time*tail/recovery:tail)),0,(int)total-1);
    }
    time=time>windup?time-windup:0;
    return (unsigned)Clamp((int)(hit+time*tail/(active+recovery?active+recovery:1)),0,(int)total-1);
}
static void EndAction(CombatFighter *f)
{ f->phase=COMBAT_READY;f->actionTick=0;f->actionMove=0;f->basicAttack=0;f->castRoot=0;f->preciseAim=0;f->beamReach=0; }
static void ReleaseAction(CombatFighter *f)
{
    if(f->releasedActionSerial==f->actionSerial)return;
    f->releasedActionSerial=f->actionSerial;
    f->releaseMove=f->actionMove;f->releaseSerial++;
}
static int ClashOwnsAction(const Combat *c,const CombatBeamClash *pair,unsigned member)
{
    unsigned actor=pair->actor[member];
    const CombatFighter *f;
    if(actor>=c->fighterCount)return 0;
    f=&c->fighters[actor];
    return f->hp&&!f->hurtTimer&&!f->dashTimer&&f->phase!=COMBAT_READY&&
        f->actionMove==pair->move[member]&&f->actionSerial==pair->actionSerial[member];
}
int Combat_BeamContact(const Combat *c,unsigned side,int32_t *x,int32_t *y)
{
    unsigned i;
    if(!c||side>=c->fighterCount)return 0;
    for(i=0;i<COMBAT_BEAM_CLASHES;i++) {
        const CombatBeamClash *pair=&c->beamClashes[i];
        if(pair->phase==COMBAT_CLASH_PRESSURE&&
            (pair->actor[0]==side||pair->actor[1]==side)&&
            ClashOwnsAction(c,pair,0)&&ClashOwnsAction(c,pair,1)) {
            if(x)*x=pair->x;
            if(y)*y=pair->y;
            return 1;
        }
    }
    return 0;
}
static uint8_t Hit(Combat *c,unsigned owner,unsigned victim,uint16_t move,uint8_t basic,
                   int32_t sourceX,int32_t sourceY,int16_t aimX,int16_t aimY);
static uint16_t SlotButton(unsigned slot)
{ (void)slot;return COMBAT_A; }
static void MoveClashContact(const Combat *c,CombatBeamClash *pair)
{
    unsigned losing=pair->pressure<0?0:1;
    const CombatFighter *f=&c->fighters[pair->actor[losing]];
    int amount=Abs(pair->pressure);
    pair->x=pair->originX+(f->x-pair->originX)*amount/COMBAT_CLASH_PRESSURE_LIMIT;
    pair->y=pair->originY+(f->y-pair->originY)*amount/COMBAT_CLASH_PRESSURE_LIMIT;
}
/* Clash ownership consumes combat input; button presses cannot add power. */
static int ClashPushInput(Combat *c,unsigned side,CombatInput input)
{
    unsigned i,member;
    (void)input;
    for(i=0;i<COMBAT_BEAM_CLASHES;i++) {
        CombatBeamClash *pair=&c->beamClashes[i];
        if(pair->phase!=COMBAT_CLASH_PRESSURE||
            !ClashOwnsAction(c,pair,0)||!ClashOwnsAction(c,pair,1))continue;
        for(member=0;member<2;member++)if(pair->actor[member]==side) {
            return 1;
        }
    }
    return 0;
}
static void FinishBeamClash(Combat *c,CombatBeamClash *pair,int winningMember)
{
    unsigned member;
    int resolved=winningMember>=0&&ClashOwnsAction(c,pair,0)&&ClashOwnsAction(c,pair,1);
    pair->winner=resolved?pair->actor[winningMember]:COMBAT_FIGHTERS;
    for(member=0;member<2;member++)if(ClashOwnsAction(c,pair,member)) {
        CombatFighter *f=&c->fighters[pair->actor[member]];
        const struct ArenaMoveProfile *p=ArenaMoves_Get(f->actionMove);
        if(!p) { EndAction(f);continue; }
        f->phase=COMBAT_RECOVERY;f->actionTick=Windup(f,p)+Active(f,p);
        f->hitMask|=1u<<pair->actor[1-member];
        f->vx=f->vy=0;
    }
    pair->phase=COMBAT_CLASH_BURST;pair->age=0;
    /* Leave pressure before applying damage so the normal damage path may
     * interrupt the defeated action, and cannot resolve this pair twice. */
    if(resolved) {
        unsigned owner=pair->actor[winningMember],victim=pair->actor[1-winningMember];
        CombatFighter *a=&c->fighters[owner],*v=&c->fighters[victim];
        unsigned direction=Facing(v->x-a->x,v->y-a->y);
        pair->x=v->x;pair->y=v->y;
        Hit(c,owner,victim,pair->move[winningMember],0,a->x,a->y,
            sDirections[direction][0],sDirections[direction][1]);
    }
    Burst(c,pair->x,pair->y,RGB(31,29,21));
    Effect(c,pair->x,pair->y,RGB(24,29,31),COMBAT_CLASH,20);
    Impact(c,COMBAT_IMPACT_CLASH,255,pair->x,pair->y);
    if(c->hitstop<4)c->hitstop=4;
}
static void EndBeamClash(Combat *c,CombatBeamClash *pair)
{ FinishBeamClash(c,pair,-1); }
static void BeamClashStep(Combat *c)
{
    unsigned i;
    for(i=0;i<COMBAT_BEAM_CLASHES;i++) {
        CombatBeamClash *pair=&c->beamClashes[i];
        if(pair->phase==COMBAT_CLASH_PRESSURE) {
            pair->pressure=(int16_t)Clamp(pair->powerDelta*(int)(pair->age+1)/12,
                -COMBAT_CLASH_PRESSURE_LIMIT,COMBAT_CLASH_PRESSURE_LIMIT);
            MoveClashContact(c,pair);
            if(!ClashOwnsAction(c,pair,0)||!ClashOwnsAction(c,pair,1))EndBeamClash(c,pair);
            else if(pair->pressure>=COMBAT_CLASH_PRESSURE_LIMIT)FinishBeamClash(c,pair,0);
            else if(pair->pressure<=-COMBAT_CLASH_PRESSURE_LIMIT)FinishBeamClash(c,pair,1);
            else if(++pair->age>=COMBAT_CLASH_PRESSURE_TICKS)EndBeamClash(c,pair);
            else if(!(pair->age%12))
                Effect(c,pair->x,pair->y,RGB(31,31,24),COMBAT_CLASH,10);
        } else if(pair->phase==COMBAT_CLASH_BURST&&++pair->age>=COMBAT_CLASH_BURST_TICKS)
            memset(pair,0,sizeof(*pair));
    }
}
static void Buffer(CombatFighter *f,uint8_t action)
{ f->buffered=action;f->bufferTimer=10; }
int Combat_QueueMove(Combat *c,unsigned side,unsigned slot)
{
    CombatFighter *f;
    if(!c||side>=c->fighterCount||slot>=4)return 0;
    f=&c->fighters[side];
    if(!f->hp||!f->moves[slot]||!f->pp[slot]||f->cooldown[slot])return 0;
    f->bufferedSlot=(uint8_t)slot;Buffer(f,1);return 1;
}
static void MoveBody(CombatFighter *f,int32_t dx,int32_t dy)
{
    int32_t x=Clamp(f->x+dx,ARENA_MIN_X*Q,ARENA_MAX_X*Q);
    int32_t y=Clamp(f->y+dy,ARENA_MIN_Y*Q,ARENA_MAX_Y*Q);
    if(ArenaNav_CanStand(x/Q,f->y/Q)) f->x=x; else f->vx=0;
    if(ArenaNav_CanStand(f->x/Q,y/Q)) f->y=y; else f->vy=0;
}
static void StartAction(Combat *c,unsigned side,uint8_t basic,uint8_t slot)
{
    CombatFighter *f=&c->fighters[side];
    unsigned target=Combat_TargetFighter(c,side),i;
    uint16_t move=basic?Combat_NaturalMove(f):f->moves[slot&3];
    const CombatMoveInfo *m=Combat_GetMoveInfo(move);
    const struct ArenaMoveProfile *p;
    if(f->status==COMBAT_STATUS_SLEEP||f->status==COMBAT_STATUS_FREEZE||f->statusLock)return;
    if(!basic&&f->cooldown[slot&3])return;
    if(!basic&&(!m||!f->pp[slot&3])) {
        /* A fixed equipped-move layout still needs an escape when all PP is
         * exhausted. An empty single slot never steals another move's PP. */
        for(i=0;i<4;i++)if(f->pp[i]&&Combat_GetMoveInfo(f->moves[i]))break;
        if(i<4)return;
        basic=1;move=MOVE_STRUGGLE;m=Combat_GetMoveInfo(move);
    }
    if(!m||target==COMBAT_FIGHTERS)return;
    if(c->nativeBalance&&(f->stamina<(basic?4:2+m->power/40)||(basic&&f->naturalCooldown)))return;
    if(f->status==COMBAT_STATUS_PARALYSIS&&!(NextRandom(c)&3)) {f->statusLock=30;return;}
    if(f->confusionTicks&&(NextRandom(c)&1)) {
        unsigned damage=(40*Staged(f->attack,f->stages[COMBAT_STAT_ATTACK],0)/
            (Staged(f->defense,f->stages[COMBAT_STAT_DEFENSE],0)+1)+15)/5;
        f->hp=(uint16_t)(f->hp>(unsigned)damage?f->hp-damage:0);
        f->hurtTimer=15;f->statusLock=60;
        Effect(c,f->x,f->y,RGB(24,12,28),COMBAT_CONTACT,8);return;
    }
    if(side==c->controlActor&&c->aimOverride&&!NativeRule(move)->selfTarget)
        f->direction=Facing(c->aimX,c->aimY);
    else if(c->lockAim||side!=c->controlActor) {
        CombatFighter *other=&c->fighters[target];
        f->direction=Facing(other->x-f->x,other->y-f->y);
    }
    f->attackDirection=f->direction;
    f->preciseAim=(uint8_t)(side==c->controlActor&&c->aimOverride&&!NativeRule(move)->selfTarget);
    f->attackAimX=c->aimX;f->attackAimY=c->aimY;f->beamReach=0;
    f->castRoot=(uint8_t)(!basic&&m->category==2);
    if(f->castRoot)f->vx=f->vy=0;
    f->actionMove=move;f->actionTick=0;f->actionDistance=0;f->phase=COMBAT_WINDUP;
    f->actionSerial++;
    f->actionSlot=basic?4:slot&3;
    f->hitMask=0;f->terrainMask=0;f->basicAttack=basic;
    f->animTick=0;
    if(c->nativeBalance){
        unsigned cost=basic?4:2+m->power/40;
        f->stamina=(uint8_t)(f->stamina-cost);f->regenDelay=18;
        if(basic){p=ArenaMoves_Get(move);f->naturalCooldown=(uint16_t)(p?Windup(f,p)+Active(f,p)+Recovery(f,p)+18:48);}
    }
    if(!basic) {
        f->pp[slot&3]--;
        p=ArenaMoves_Get(move);
        if(p)f->cooldown[slot&3]=(uint16_t)(Windup(f,p)+Active(f,p)+Recovery(f,p)+
            (c->nativeBalance?(m->category==2?120:30+m->power):m->category==2?45:m->power>=80?30:0));
    }
}
static uint8_t CanCancel(const CombatFighter *f)
{
    const struct ArenaMoveProfile *p=ArenaMoves_Get(f->actionMove);
    if(f->castRoot&&f->phase!=COMBAT_READY)return 0;
    return f->phase==COMBAT_READY||(p&&f->phase==COMBAT_RECOVERY&&
        f->actionTick>=Windup(f,p)+Active(f,p))||
        (p&&ArenaMoves_Beam(f->actionMove)&&f->phase==COMBAT_ACTIVE);
}
static void Defend(Combat *c,unsigned side,int dx,int dy)
{
    CombatFighter *f=&c->fighters[side];
    if(f->status==COMBAT_STATUS_SLEEP||f->status==COMBAT_STATUS_FREEZE||
        !CanCancel(f)||f->hurtTimer||f->dashTimer||f->guardTimer) return;
    if(dx||dy) {
        if(f->dashCooldown||f->stamina<22) return;
        EndAction(f);f->stamina-=22;f->regenDelay=18;
        f->dashTimer=12;f->dashCooldown=30;
        f->dashDirection=Facing(dx,dy);f->direction=f->dashDirection;f->animTick=0;
    } else {
        if(f->stamina<=6||f->guardExhausted) return;
        EndAction(f);f->stamina-=6;f->regenDelay=12;f->guardTimer=16;
        f->guardHeld=side==c->controlActor;f->guardAge=0;
        Effect(c,f->x,f->y,RGB(12,25,31),COMBAT_BLOCK,12)->direction=f->direction;
    }
}
/* Canonical nonvolatile status admission, with Gen IV type immunities.
 * Electric types can be paralyzed in Gen IV; Ground immunity is handled by
 * the move's Electric type. Volatile confusion remains separate. */
static void InflictStatus(Combat *c,unsigned victim,unsigned status)
{
    CombatFighter *f=&c->fighters[victim];
    if(!f->hp)return;
    if(status==7) { if(!f->confusionTicks)f->confusionTicks=180+(NextRandom(c)%241);return; }
    if(f->status)return;
    if((status==COMBAT_STATUS_BURN&&(f->types[0]==10||f->types[1]==10))||
        (status==COMBAT_STATUS_FREEZE&&(f->types[0]==15||f->types[1]==15))||
        ((status==COMBAT_STATUS_POISON||status==COMBAT_STATUS_BAD_POISON)&&
         (f->types[0]==3||f->types[1]==3||f->types[0]==8||f->types[1]==8)))return;
    f->status=(uint8_t)status;f->statusClock=0;f->toxicCounter=1;
    f->statusTicks=status==COMBAT_STATUS_SLEEP?180+(NextRandom(c)%121):
        status==COMBAT_STATUS_FREEZE?480:0;
    if(status==COMBAT_STATUS_SLEEP||status==COMBAT_STATUS_FREEZE) {
        EndAction(f);f->dashTimer=f->guardTimer=f->guardHeld=0;f->vx=f->vy=0;
    }
}
static void ApplyStatus(Combat *c,unsigned owner,unsigned victim,uint16_t move)
{
    const CombatNativeRule *r=NativeRule(move);
    const CombatMoveInfo *m=Combat_GetMoveInfo(move);
    CombatFighter *f=&c->fighters[victim];
    CombatFighter *stageTarget=&c->fighters[r->selfStage?owner:victim];
    unsigned i,hasEffect=r->status||r->heal||r->special;
    if(!m)return;
    for(i=0;i<COMBAT_STAT_COUNT;i++)if(r->stages[i])hasEffect=1;
    if(!hasEffect)return;
    if(m->category!=2&&r->chance&&NextRandom(c)%100>=r->chance)return;
    for(i=0;i<COMBAT_STAT_COUNT;i++)if(r->stages[i])
        Combat_ChangeStage(c,r->selfStage?owner:victim,i,r->stages[i]);
    if(r->status)InflictStatus(c,victim,r->status);
    if(r->heal)f->hp=(uint16_t)Clamp(f->hp+f->maxHp/2,1,f->maxHp);
    switch(r->special) {
    case 1:for(i=0;i<c->fighterCount;i++)memset(c->fighters[i].stages,0,sizeof(c->fighters[i].stages));break;
    case 2:f->status=0;f->statusTicks=0;f->confusionTicks=0;break;
    case 3:f->hp=f->maxHp;f->status=0;InflictStatus(c,victim,COMBAT_STATUS_SLEEP);f->statusTicks=180;break;
    case 4:f->defenseBoost=600;break; /* Reflect is timed, not a stat stage. */
    case 5:f->lightScreenTicks=600;break;
    case 6:if(f->stamina){f->guardTimer=16;f->guardAge=0;f->regenDelay=12;}break;
    case 7:if(owner!=victim)memcpy(c->fighters[owner].stages,f->stages,sizeof(f->stages));break;
    case 8:Combat_ChangeStage(c,victim,COMBAT_STAT_ATTACK,2);InflictStatus(c,victim,7);break;
    case 9:Combat_ChangeStage(c,victim,COMBAT_STAT_SP_ATTACK,1);InflictStatus(c,victim,7);break;
    }
    /* Older HUD fields remain presentation compatibility flags only. Stages
     * drive damage/speed and have no expiry timer. */
    if(stageTarget->stages[COMBAT_STAT_ATTACK]>0)stageTarget->attackBoost=1;
    if(stageTarget->stages[COMBAT_STAT_ATTACK]<0)stageTarget->attackDown=1;
    if(stageTarget->stages[COMBAT_STAT_DEFENSE]<0)stageTarget->defenseDown=1;
    Effect(c,f->x,f->y,RGB(20,28,31),COMBAT_RING,12);
}
/* Returns 1 for contact, including blocks and immunity; 0 lets an evaded
 * projectile continue. No hit test uses sprite dimensions or render pixels. */
static uint8_t Hit(Combat *c,unsigned owner,unsigned victim,uint16_t move,uint8_t basic,
                   int32_t sourceX,int32_t sourceY,int16_t aimX,int16_t aimY)
{
    CombatFighter *a=&c->fighters[owner],*v=&c->fighters[victim];
    const CombatMoveInfo *m=Combat_GetMoveInfo(move);
    uint16_t mult;
    int damage,guardFacing,critical=0;
    if(!m||!v->hp||!Enemies(owner,victim)) return 0;
    if((v->dashTimer&&v->dashTimer>=5)||v->hurtTimer>10) {c->dodged++;return 0;}
    if(!basic) {
        unsigned accuracy=NativeRule(move)->accuracy;
        if(accuracy) {
            accuracy=Staged(accuracy,a->stages[COMBAT_STAT_ACCURACY]-v->stages[COMBAT_STAT_EVASION],1);
            if(accuracy<100&&NextRandom(c)%100>=accuracy) {
                Effect(c,v->x,v->y,RGB(20,20,20),COMBAT_IMMUNE,7);return 1;
            }
        }
    }
    if(!basic&&m->category==2) {
        if(v->guardTimer&&v->stamina) {
            c->blocked++;v->stamina--;v->regenDelay=20;
            Impact(c,COMBAT_IMPACT_BLOCK,victim,v->x,v->y);
            if(!v->stamina) { v->guardTimer=v->guardHeld=0;v->guardExhausted=1; }
            return 1;
        }
        if(m->type==13&&!Combat_TypeMultiplier(m->type,v->types[0],v->types[1]))return 1;
        ApplyStatus(c,owner,victim,move);return 1;
    }
    /* The emergency arena fallback is typeless so exhausted PP cannot leave
     * a pairing permanently unable to finish. Equipped moves keep their type. */
    mult=basic?Q:Combat_TypeMultiplier(m->type,v->types[0],v->types[1]);
    if(!mult) { Effect(c,v->x,v->y,RGB(20,20,20),COMBAT_IMMUNE,10);return 1; }
    unsigned special=m->category==1;
    unsigned attack=Staged(special?a->spAttack:a->attack,
        a->stages[special?COMBAT_STAT_SP_ATTACK:COMBAT_STAT_ATTACK],0);
    unsigned defense=Staged(special?v->spDefense:v->defense,
        v->stages[special?COMBAT_STAT_SP_DEFENSE:COMBAT_STAT_DEFENSE],0);
    if(!special&&a->status==COMBAT_STATUS_BURN)attack=attack/2?attack/2:1;
    damage=(((basic?25:m->power)*attack)/(defense?defense:1)+15)/5;
    if(!basic&&(a->types[0]==m->type||a->types[1]==m->type)) damage=damage*3/2;
    if((!special&&v->defenseBoost)||(special&&v->lightScreenTicks))damage=(damage+1)/2;
    damage=Clamp(damage*mult/Q,1,v->maxHp);
    guardFacing=(sourceX-v->x)*sDirections[v->direction][0]+(sourceY-v->y)*sDirections[v->direction][1]>=0;
    if(v->guardTimer&&v->stamina&&guardFacing) {
        int perfect=v->guardAge<5;
        int cost=perfect?4:10+damage/4;
        if(c->nativeBalance&&!perfect)cost=Clamp(cost*100/(75+v->resistance/2),5,80);
        if(v->stamina>=cost) {
            v->stamina-=cost;v->regenDelay=20;c->blocked++;
            /* Heavy native moves still apply reduced pressure through a
             * shield. Basic strikes and low-power moves are fully blocked. */
            if(!basic&&m->power>=80) {
                int chip=Clamp(damage/5,1,v->hp);
                if(move==MOVE_FALSE_SWIPE&&chip>=v->hp) chip=v->hp>1?v->hp-1:0;
                v->hp-=chip;
            }
            Effect(c,v->x,v->y,RGB(10,31,31),COMBAT_BLOCK,14)->direction=Facing(sourceX-v->x,sourceY-v->y);
            Impact(c,COMBAT_IMPACT_BLOCK,victim,v->x,v->y);
            c->hitstop=perfect?3:2;
            /* Contact rebounds visibly without interrupting unrelated ranged
             * actions; heavy pressure pushes both bodies apart. */
            if(basic||ArenaMoves_Get(move)->kind==ARENA_MOVE_MELEE||ArenaMoves_Get(move)->kind==ARENA_MOVE_RUSH) {
                a->vx=-aimX*(m->power>=80?2:1);a->vy=-aimY*(m->power>=80?2:1);
                if(!basic&&m->power>=80) {v->vx=aimX;v->vy=aimY;}
            }
            if(!v->stamina||!v->hp) { v->guardTimer=v->guardHeld=0;v->guardExhausted=1; }
            return 1;
        }
        v->stamina=0;v->guardTimer=v->guardHeld=0;v->guardExhausted=1;
        Burst(c,v->x,v->y,RGB(31,20,6));
    }
    /* Native Gen IV base chance, doubled for the supported high-crit moves.
     * A critical is an actual damage event, not a cosmetic random zoom. */
    if(!basic) {
        unsigned mask=15;
        switch(move) {
        case MOVE_KARATE_CHOP:case MOVE_RAZOR_LEAF:case MOVE_CRABHAMMER:
        case MOVE_SLASH:case MOVE_AEROBLAST:case MOVE_CROSS_CHOP:
        case MOVE_BLAZE_KICK:case MOVE_AIR_CUTTER:case MOVE_POISON_TAIL:
        case MOVE_LEAF_BLADE:case MOVE_NIGHT_SLASH:case MOVE_SHADOW_CLAW:
        case MOVE_PSYCHO_CUT:case MOVE_STONE_EDGE:case MOVE_ATTACK_ORDER:
            mask=7;break;
        }
        critical=!(NextRandom(c)&mask);
        if(critical) damage=Clamp(damage*2,1,v->maxHp);
    }
    if(move==MOVE_FALSE_SWIPE&&damage>=v->hp) damage=v->hp>1?v->hp-1:0;
    if(damage>v->hp) damage=v->hp;
    v->hp-=damage;v->hurtTimer=15;v->guardTimer=v->guardHeld=0;v->dashTimer=0;
    v->vx=aimX*3;v->vy=aimY*3;
    if(c->nativeBalance){v->vx=v->vx*100/(70+v->resistance/2);v->vy=v->vy*100/(70+v->resistance/2);}v->regenDelay=16;EndAction(v);
    if(a->hp&&(move==MOVE_ABSORB||move==MOVE_MEGA_DRAIN||move==MOVE_GIGA_DRAIN||move==MOVE_LEECH_LIFE))
        a->hp=Clamp(a->hp+(damage+1)/2,0,a->maxHp);
    if(v->status==COMBAT_STATUS_FREEZE&&m->type==10){v->status=0;v->statusTicks=0;}
    if(!basic&&v->hp)ApplyStatus(c,owner,victim,move);
    Burst(c,v->x,v->y,Combat_TypeColor(m->type));
    {
        CombatEffect *contact=Effect(c,v->x,v->y,Combat_TypeColor(m->type),COMBAT_CONTACT,12);
        contact->direction=Facing(aimX,aimY);contact->move=move;
    }
    c->hits++;c->hitstop=basic?2:critical?5:3;
    if(critical&&damage) { c->criticals++;Impact(c,COMBAT_IMPACT_CRITICAL,victim,v->x,v->y); }
    return 1;
}
static void SpawnProjectile(Combat *c,unsigned side,const struct ArenaMoveProfile *p)
{
    CombatFighter *f=&c->fighters[side];
    const CombatMoveInfo *m=Combat_GetMoveInfo(p->move);
    CombatProjectile *shot;
    unsigned i;
    for(i=0;i<COMBAT_PROJECTILES;i++) if(!c->projectiles[i].life) break;
    if(i==COMBAT_PROJECTILES||!m) return;
    shot=&c->projectiles[i];
    shot->owner=side;shot->move=p->move;shot->type=m->type;
    shot->basic=f->basicAttack;
    {int16_t x,y;Combat_AttackVector(f,&x,&y);shot->vx=x*p->speed/Q;shot->vy=y*p->speed/Q;}
    shot->x=f->x;shot->y=f->y;
    shot->previousX=shot->x;shot->previousY=shot->y;
    shot->life=(uint8_t)Clamp(p->range*Q/(p->speed?p->speed:Q),1,200);
    shot->radius=(uint8_t)Clamp(p->radius/3,3,7);
    shot->visual=p->visual;shot->color=Combat_TypeColor(m->type);
    Effect(c,f->x,f->y,shot->color,COMBAT_RING,4);
}
static void ActionStep(Combat *c,unsigned side)
{
    CombatFighter *f=&c->fighters[side];
    const struct ArenaMoveProfile *p=ArenaMoves_Get(f->actionMove);
    const CombatMoveInfo *m=Combat_GetMoveInfo(f->actionMove);
    unsigned w,a,i,reach;
    int16_t ax,ay,px,py,tx,ty;
    if(!f->hp||!p||!m||f->status==COMBAT_STATUS_SLEEP||f->status==COMBAT_STATUS_FREEZE||
        f->phase==COMBAT_READY||Combat_BeamContact(c,side,0,0)) return;
    w=Windup(f,p);a=Active(f,p);
    Combat_AttackVector(f,&ax,&ay);reach=p->range;
    if(c->nativeBalance&&m->category==0&&p->kind==ARENA_MOVE_RUSH&&!NativeCharge(f->actionMove))reach=28;
    f->animation=p->move==MOVE_PROTECT?COMBAT_IDLE:p->animation;f->direction=f->attackDirection;
    if(f->actionTick<w) {
        f->phase=COMBAT_WINDUP;
        if(!(f->actionTick%6)) Effect(c,f->x,f->y,Combat_TypeColor(m->type),COMBAT_RING,3);
    } else if(f->actionTick<w+a) {
        f->phase=COMBAT_ACTIVE;
        if(ArenaMoves_Beam(f->actionMove)){
            unsigned growth=(unsigned)Clamp(f->actionTick-w+1,1,6);
            reach=BeamRange(c,f,p,growth);f->beamReach=(uint16_t)reach;
        }
        if(f->actionTick==w)ReleaseAction(f);
        if(f->actionTick==w&&p->kind==ARENA_MOVE_PROJECTILE) SpawnProjectile(c,side,p);
        if(f->actionTick==w&&NativeRule(p->move)->selfTarget&&m->category==2&&p->move!=MOVE_PROTECT)
            ApplyStatus(c,side,side,p->move);
        if(p->move==MOVE_PROTECT) {
            if(f->stamina) { f->guardTimer=16;f->guardAge=0; }
            if(!(f->actionTick%12)) Effect(c,f->x,f->y,RGB(10,31,31),COMBAT_BLOCK,14);
        }
        if(p->kind==ARENA_MOVE_RUSH&&(!c->nativeBalance||NativeCharge(f->actionMove))&&f->actionDistance<p->range*Q) {
            unsigned step=c->nativeBalance?p->speed*3/4:p->speed,remaining=(unsigned)p->range*Q-f->actionDistance;
            if(step>remaining) step=remaining;
            MoveBody(f,ax*(int)step/Q,ay*(int)step/Q);f->actionDistance+=step;
        }
        px=f->x/Q;py=f->y/Q;
        if((m->category!=2||!NativeRule(p->move)->selfTarget)&&
            (p->kind==ARENA_MOVE_MELEE||p->kind==ARENA_MOVE_RUSH||p->kind==ARENA_MOVE_CONE||
             (p->kind==ARENA_MOVE_SELF&&m->category==2))) {
            for(i=0;i<c->fighterCount;i++) if(Enemies(side,i)&&c->fighters[i].hp&&!(f->hitMask&(1u<<i))) {
                CombatFighter *v=&c->fighters[i];
                int hit;
                tx=v->x/Q;ty=v->y/Q;
                hit=ArenaMoves_InCone(tx-px,ty-py,ax,ay,reach?reach:1,p->cone);
                if(p->kind==ARENA_MOVE_RUSH&&(!c->nativeBalance||NativeCharge(f->actionMove))) hit=ArenaMoves_SegmentHit(f->previousX/Q,f->previousY/Q,px,py,tx,ty,18);
                if(hit&&ArenaNav_LineClear(px,py,tx,ty,1)&&
                    Hit(c,side,i,f->actionMove,f->basicAttack,f->x,f->y,ax,ay)) {
                    f->hitMask|=1u<<i;
                    /* A connecting tackle stops at contact instead of
                     * carrying the attacker through and past its target. */
                    if(p->kind==ARENA_MOVE_RUSH) {
                        f->actionDistance=p->range*Q;f->vx=f->vy=0;
                    }
                }
            }
        }
        if(p->kind!=ARENA_MOVE_SELF&&p->kind!=ARENA_MOVE_PROJECTILE) {
            int16_t obstacle=ArenaNav_FirstObstacle(px,py,px+ax*(int)reach/Q,py+ay*(int)reach/Q,2);
            if(obstacle>=0&&!(f->terrainMask&(1u<<obstacle))) {
                f->terrainMask|=1u<<obstacle;
                ArenaPhysics_Hit(obstacle,ax*3,ay*3,m->power>=80?2:1);
            }
            if(!(f->actionTick%5)) Effect(c,f->x+ax*18,f->y+ay*18,Combat_TypeColor(m->type),COMBAT_RING,p->kind==ARENA_MOVE_CONE?9:5);
        }
    } else f->phase=COMBAT_RECOVERY;
    f->actionTick++;
    if(f->actionTick>=w+a+Recovery(f,p)) EndAction(f);
}
/* Resolve beams before either actor can damage a body. Two independently
 * owned slots permit both rows of a 2v2 fight to sustain contact. Neither
 * actor nor its action timer advances during the bounded pressure stage. */
static int ActiveBeam(const CombatFighter *f,const struct ArenaMoveProfile *p)
{
    return f->hp&&!f->hurtTimer&&!f->dashTimer&&p&&ArenaMoves_Beam(f->actionMove)&&
        f->phase!=COMBAT_READY&&f->actionTick>=Windup(f,p)&&
        f->actionTick<Windup(f,p)+Active(f,p);
}
static unsigned ClashPower(const CombatFighter *a,const CombatFighter *b)
{
    const CombatMoveInfo *m=Combat_GetMoveInfo(a->actionMove);
    const struct ArenaMoveProfile *p=ArenaMoves_Get(a->actionMove);
    unsigned special=m&&m->category==1;
    unsigned attack=Staged(special?a->spAttack:a->attack,
        a->stages[special?COMBAT_STAT_SP_ATTACK:COMBAT_STAT_ATTACK],0);
    unsigned defense=Staged(special?b->spDefense:b->defense,
        b->stages[special?COMBAT_STAT_SP_DEFENSE:COMBAT_STAT_DEFENSE],0);
    unsigned power;
    int dx=(b->x-a->x)/Q,dy=(b->y-a->y)/Q;
    int16_t aimX,aimY;
    int dot;
    Combat_AttackVector(a,&aimX,&aimY);dot=dx*aimX+dy*aimY;
    unsigned alignment=(unsigned)Clamp(dot/(Abs(dx)+Abs(dy)+1),0,256);
    if(!m||!p)return 0;
    power=(m->power*attack/(defense?defense:1)+1)*Combat_TypeMultiplier(m->type,b->types[0],b->types[1])/Q;
    if(a->types[0]==m->type||a->types[1]==m->type)power=power*3/2;
    if(!special&&a->status==COMBAT_STATUS_BURN)power/=2;
    power=power*(192+alignment/4)/256;
    /* A beam established a few frames earlier gains a small timing bonus. */
    return power*(100+(unsigned)Clamp(a->actionTick-Windup(a,p),0,12))/100;
}
static void BeamClashes(Combat *c)
{
    unsigned i,j,slot;
    for(i=0;i<c->fighterCount;i++) {
        CombatFighter *a=&c->fighters[i];
        const struct ArenaMoveProfile *pa=ArenaMoves_Get(a->actionMove);
        if(!ActiveBeam(a,pa)||Combat_BeamContact(c,i,0,0)) continue;
        for(j=i+1;j<c->fighterCount;j++) {
            CombatFighter *b=&c->fighters[j];
            const struct ArenaMoveProfile *pb=ArenaMoves_Get(b->actionMove);
            int ax,ay,bx,by,step,rangeA,rangeB,best=0x7fffffff,cx=0,cy=0;
            int aX=a->x/Q,aY=a->y/Q,bX=b->x/Q,bY=b->y/Q;
            CombatBeamClash *pair;
            if(!Enemies(i,j)||!ActiveBeam(b,pb)||Combat_BeamContact(c,j,0,0)) continue;
            for(slot=0;slot<COMBAT_BEAM_CLASHES;slot++)
                if(c->beamClashes[slot].phase==COMBAT_CLASH_NONE)break;
            if(slot==COMBAT_BEAM_CLASHES)return;
            {int16_t x,y;Combat_AttackVector(a,&x,&y);ax=x;ay=y;Combat_AttackVector(b,&x,&y);bx=x;by=y;}
            if(ax*bx+ay*by>0) continue; /* Parallel friendly-direction fire. */
            rangeA=(int)BeamRange(c,a,pa,(unsigned)Clamp(a->actionTick-Windup(a,pa)+1,1,6));
            rangeB=(int)BeamRange(c,b,pb,(unsigned)Clamp(b->actionTick-Windup(b,pb)+1,1,6));
            for(step=12;step<=rangeA;step+=4) {
                int x=aX+ax*step/Q,y=aY+ay*step/Q;
                int distance=Abs(x*2-aX-bX)+Abs(y*2-aY-bY);
                if(distance<best&&(x-bX)*bx+(y-bY)*by>=12*Q&&
                    ArenaMoves_SegmentHit(bX,bY,bX+bx*rangeB/Q,bY+by*rangeB/Q,x,y,10)&&
                    ArenaNav_LineClear(aX,aY,x,y,1)&&ArenaNav_LineClear(bX,bY,x,y,1)) {
                    best=distance;cx=x;cy=y;
                }
            }
            if(best!=0x7fffffff) {
                pair=&c->beamClashes[slot];memset(pair,0,sizeof(*pair));
                pair->x=pair->originX=cx*Q;pair->y=pair->originY=cy*Q;
                pair->actor[0]=(uint8_t)i;pair->actor[1]=(uint8_t)j;
                pair->winner=COMBAT_FIGHTERS;
                pair->move[0]=a->actionMove;pair->move[1]=b->actionMove;
                pair->actionSerial[0]=a->actionSerial;pair->actionSerial[1]=b->actionSerial;
                {
                    unsigned one=ClashPower(a,b),two=ClashPower(b,a);
                    pair->powerDelta=(int16_t)(((int)one-(int)two)*24/(int)(one+two+1));
                }
                pair->phase=COMBAT_CLASH_PRESSURE;
                a->phase=b->phase=COMBAT_ACTIVE;
                a->actionTick=Windup(a,pa);b->actionTick=Windup(b,pb);
                a->vx=a->vy=b->vx=b->vy=0;
                a->animation=pa->animation;b->animation=pb->animation;
                a->direction=a->attackDirection;b->direction=b->attackDirection;
                ReleaseAction(a);ReleaseAction(b);
                Clash(c,cx*Q,cy*Q);
                break;
            }
        }
    }
}
static void ProjectileStep(Combat *c)
{
    unsigned i,j;
    /* Stage every shot's travel first. Clash contact cannot depend on which
     * projectile happened to obtain a lower pool slot. */
    for(i=0;i<COMBAT_PROJECTILES;i++) {
        CombatProjectile *p=&c->projectiles[i];
        int16_t obstacle;
        if(!p->life) continue;
        if(p->owner>=c->fighterCount) { p->life=0;continue; }
        p->previousX=p->x;p->previousY=p->y;
        p->x+=p->vx;p->y+=p->vy;
        obstacle=ArenaNav_FirstObstacle(p->previousX/Q,p->previousY/Q,p->x/Q,p->y/Q,p->radius);
        if(obstacle>=0) {
            const CombatMoveInfo *m=Combat_GetMoveInfo(p->move);
            ArenaPhysics_Hit(obstacle,p->vx,p->vy,m&&m->power>=80?2:1);
            Burst(c,p->x,p->y,p->color);p->life=0;continue;
        }
    }
    for(i=0;i<COMBAT_PROJECTILES;i++) {
        CombatProjectile *p=&c->projectiles[i];
        if(!p->life) continue;
        for(j=i+1;j<COMBAT_PROJECTILES&&p->life;j++) {
            CombatProjectile *q=&c->projectiles[j];
            if(q->life&&Enemies(q->owner,p->owner)&&ArenaPlus_SweptContact(
                p->previousX,p->previousY,p->x,p->y,q->previousX,q->previousY,q->x,q->y,p->radius+q->radius)) {
                p->life=q->life=0;Clash(c,(p->x+q->x)/2,(p->y+q->y)/2);
            }
        }
    }
    for(i=0;i<COMBAT_PROJECTILES;i++) {
        CombatProjectile *p=&c->projectiles[i];
        if(!p->life) continue;
        /* Resolve the first swept opposing body along the shot's travel. An
         * allied body never absorbs a shot; an evaded enemy lets it continue. */
        unsigned considered=0;
        for(unsigned pass=0;pass<c->fighterCount&&p->life;pass++) {
            unsigned victim=COMBAT_FIGHTERS;
            int32_t closest=0x7fffffff;
            for(j=0;j<c->fighterCount;j++) if(!(considered&(1u<<j))&&
                Enemies(p->owner,j)&&c->fighters[j].hp) {
                CombatFighter *target=&c->fighters[j];
                if(ArenaPlus_SweptContact(p->previousX,p->previousY,p->x,p->y,
                    target->previousX,target->previousY,target->x,target->y,p->radius+9)) {
                    int32_t along=(target->previousX-p->previousX)/Q*p->vx/Q+
                        (target->previousY-p->previousY)/Q*p->vy/Q;
                    if(along<closest) { closest=along;victim=j; }
                }
            }
            if(victim==COMBAT_FIGHTERS) break;
            considered|=1u<<victim;
            if(Hit(c,p->owner,victim,p->move,p->basic,p->previousX,p->previousY,
                sDirections[Facing(p->vx,p->vy)][0],sDirections[Facing(p->vx,p->vy)][1])) {
                p->life=0;
            }
        }
        if(!p->life) continue;
        if(p->life) {
            if(p->x<4*Q||p->x>(ARENA_WORLD_WIDTH-4)*Q||
               p->y<24*Q||p->y>(ARENA_WORLD_HEIGHT-9)*Q) p->life=0;
            else p->life--;
            if(!(c->frame&3)) Effect(c,p->x,p->y,p->color,COMBAT_TRAIL,2);
        }
    }
}
static void EffectStep(Combat *c)
{
    unsigned i;
    for(i=0;i<COMBAT_EFFECTS;i++) {
        CombatEffect *e=&c->effects[i];
        if(!e->life) continue;
        e->life--;e->x+=e->vx;e->y+=e->vy;
        if(e->kind==COMBAT_SPARK) e->vy+=25;
    }
}
static CombatInput AIInput(Combat *c,unsigned side)
{
    CombatInput out={0,0};
    unsigned target=Combat_TargetFighter(c,side);
    CombatFighter *f=&c->fighters[side],*v;
    if(!f->hp||target==COMBAT_FIGHTERS) return out;
    if(!(side&1)&&side!=c->controlActor&&c->tactic==COMBAT_TACTIC_FOCUS) {
        unsigned focused=Combat_TargetFighter(c,c->controlActor);
        if(focused<c->fighterCount)target=focused;
    }
    v=&c->fighters[target];
    if(!(side&1)&&side!=c->controlActor&&c->tactic==COMBAT_TACTIC_DEFEND&&c->fighters[c->controlActor].hp) {
        CombatFighter *player=&c->fighters[c->controlActor];
        int rx=player->x-f->x,ry=player->y-f->y;
        if(Abs(rx)+Abs(ry)>65*Q) {
            struct ArenaPoint waypoint;
            if(ArenaNav_NextWaypoint(f->x/Q,f->y/Q,player->x/Q,player->y/Q,&waypoint)) {
                rx=waypoint.x*Q-f->x;ry=waypoint.y*Q-f->y;
            }
            if(Abs(rx)>Q)out.held|=rx>0?COMBAT_RIGHT:COMBAT_LEFT;
            if(Abs(ry)>Q)out.held|=ry>0?COMBAT_DOWN:COMBAT_UP;
            return out;
        }
    }
    if(c->clashPractice) {
        CombatFighter *player=&c->fighters[c->controlActor];
        uint32_t serial=player->actionSerial*4u+c->controlActor+1u;
        /* Practice changes only AI intention. The ordinary release, beam
         * collision, PP and paired-clash implementation still does the work. */
        if(player->phase!=COMBAT_READY&&ArenaMoves_Beam(player->actionMove)&&
            f->phase==COMBAT_READY&&f->practiceActionSerial!=serial) {
            for(unsigned slot=0;slot<4;slot++)if(f->pp[slot]&&ArenaMoves_Beam(f->moves[slot])) {
                f->selected=(uint8_t)slot;
                f->practiceActionSerial=serial;
                out.pressed=SlotButton(slot);break;
            }
        }
        return out;
    }
    int dx=v->x-f->x,dy=v->y-f->y,dist=ArenaNav_Distance(f->x/Q,f->y/Q,v->x/Q,v->y/Q);
    int desired=30,best=-100000,bestSlot=-1,i;
    uint8_t threat=0;
    for(i=0;i<4;i++) {
        const CombatMoveInfo *m=Combat_GetMoveInfo(f->moves[i]);
        const struct ArenaMoveProfile *p=ArenaMoves_Get(f->moves[i]);
        int score;
        const CombatNativeRule *r=NativeRule(f->moves[i]);
        if(!m||!p||!f->pp[i]||f->cooldown[i]) continue;
        if(m->category==2) {
            score=0;
            if(r->heal&&f->hp*5<f->maxHp*3)score=140;
            if(r->status&&!v->status&&!r->selfTarget)score=75;
            if((r->special==2||r->special==3)&&f->status)score=120;
            for(unsigned stat=0;stat<COMBAT_STAT_COUNT;stat++)
                if(r->stages[stat]>0&&r->selfTarget&&f->stages[stat]<2)score+=24;
                else if(r->stages[stat]<0&&!r->selfTarget&&v->stages[stat]>-2)score+=18;
            if(!score)continue;
        } else {
            score=m->power*Combat_TypeMultiplier(m->type,v->types[0],v->types[1])/Q;
            score=(int)((unsigned)score*Staged(m->category==1?f->spAttack:f->attack,
                f->stages[m->category==1?COMBAT_STAT_SP_ATTACK:COMBAT_STAT_ATTACK],0)/
                (Staged(m->category==1?v->spDefense:v->defense,
                v->stages[m->category==1?COMBAT_STAT_SP_DEFENSE:COMBAT_STAT_DEFENSE],0)+1));
            if(m->category==0&&f->status==COMBAT_STATUS_BURN)score/=2;
            if(v->guardTimer)score/=2;
        }
        if(!score) continue;
        if(!r->selfTarget)score-=Abs(dist-(p->kind==ARENA_MOVE_PROJECTILE||p->kind==ARENA_MOVE_CONE?80:28))/2;
        if(score>best) { best=score;bestSlot=i; }
    }
    if(bestSlot>=0) {
        const struct ArenaMoveProfile *p=ArenaMoves_Get(f->moves[bestSlot]);
        f->selected=(uint8_t)bestSlot;
        desired=(p->kind==ARENA_MOVE_PROJECTILE||p->kind==ARENA_MOVE_CONE)?80:27;
    }
    if(!f->aiTimer) {
        struct ArenaPoint waypoint;
        f->aiTimer=12;
        if(ArenaNav_NextWaypoint(f->x/Q,f->y/Q,v->x/Q,v->y/Q,&waypoint)) {
            f->aiGoalX=waypoint.x;f->aiGoalY=waypoint.y;
        } else { f->aiGoalX=v->x/Q;f->aiGoalY=v->y/Q; }
    }
    if(dist>desired+6||!ArenaNav_LineClear(f->x/Q,f->y/Q,v->x/Q,v->y/Q,2)) {
        dx=f->aiGoalX*Q-f->x;dy=f->aiGoalY*Q-f->y;
    } else if(dist<desired-16&&desired>40) { dx=-dx;dy=-dy; }
    else { /* Short lateral steps keep ranged opponents from remaining still. */
        int lateral=(c->frame/90)&1?1:-1;
        dx=Sign(-dy)*lateral;dy=Sign(v->x-f->x)*lateral;
        if(desired<40) dx=dy=0;
    }
    if(Abs(dx)>Q/2) out.held|=dx>0?COMBAT_RIGHT:COMBAT_LEFT;
    if(Abs(dy)>Q/2) out.held|=dy>0?COMBAT_DOWN:COMBAT_UP;
    /* The lateral branch uses unit intent rather than Q8 distances. */
    if(Abs(dx)==1) out.held|=dx>0?COMBAT_RIGHT:COMBAT_LEFT;
    if(Abs(dy)==1) out.held|=dy>0?COMBAT_DOWN:COMBAT_UP;
    if(v->phase==COMBAT_WINDUP&&dist<65) threat=1;
    for(i=0;i<COMBAT_PROJECTILES;i++) {
        CombatProjectile *p=&c->projectiles[i];
        if(p->life&&Enemies(p->owner,side)&&Abs(p->x-f->x)+Abs(p->y-f->y)<48*Q&&
            (p->x-f->x)*p->vx+(p->y-f->y)*p->vy<0) threat=1;
    }
    if(threat&&f->stamina>=32&&!f->aiCooldown&&CanCancel(f)) {
        f->direction=Facing(v->x-f->x,v->y-f->y);
        if((NextRandom(c)&3)==0) out.held=0; /* Guard sometimes; dodge mostly. */
        else {
            int sx=Sign(-(v->y-f->y)),sy=Sign(v->x-f->x);
            if((c->frame/120)&1) {sx=-sx;sy=-sy;}
            out.held=(sx>0?COMBAT_RIGHT:sx<0?COMBAT_LEFT:0)|
                (sy>0?COMBAT_DOWN:sy<0?COMBAT_UP:0);
        }
        out.pressed=COMBAT_B;f->aiCooldown=40;
    } else if(!f->aiCooldown&&f->phase==COMBAT_READY) {
        const struct ArenaMoveProfile *p=ArenaMoves_Get(bestSlot>=0?f->moves[bestSlot]:Combat_NaturalMove(f));
        int self=bestSlot>=0&&NativeRule(f->moves[bestSlot])->selfTarget;
        if(p&&(self||(dist<=p->range+8&&ArenaNav_LineClear(f->x/Q,f->y/Q,v->x/Q,v->y/Q,1)))) {
            out.pressed=bestSlot>=0?SlotButton(bestSlot):COMBAT_L;f->aiCooldown=10;
        }
    }
    return out;
}
/* A held direction is one tap. A real release followed by a second press
 * inside 12 simulation input ticks uses the same dash path as B+direction. */
static int DirectionDoubleTap(CombatFighter *f,CombatInput input,int enabled)
{
    static const uint16_t bits[4]={COMBAT_UP,COMBAT_DOWN,COMBAT_LEFT,COMBAT_RIGHT};
    uint16_t directions=input.held&15u;
    uint16_t edges=(input.pressed|(~f->lastDirections&directions))&15u;
    unsigned i;int dash=0;
    f->tapReleased|=f->lastDirections&~directions;
    for(i=0;i<4;i++) {
        if(f->tapTimer[i])f->tapTimer[i]--;
        if(edges&bits[i]) {
            if(enabled&&f->tapTimer[i]&&(f->tapReleased&bits[i])) {
                dash=1;f->tapTimer[i]=0;
            } else f->tapTimer[i]=12;
            f->tapReleased&=~bits[i];
        }
    }
    f->lastDirections=(uint8_t)directions;
    return dash;
}
static void QueueInput(CombatFighter *f,CombatInput input,int doubleTapEnabled)
{
    int dx=!!(input.held&COMBAT_RIGHT)-!!(input.held&COMBAT_LEFT);
    int dy=!!(input.held&COMBAT_DOWN)-!!(input.held&COMBAT_UP);
    int doubleTap=DirectionDoubleTap(f,input,doubleTapEnabled);
    unsigned i;
    if(input.pressed&COMBAT_CYCLE_LIGHT)f->lightChoice^=1;
    if(input.pressed&COMBAT_CYCLE_HEAVY) {
        for(i=1;i<=4;i++) {
            unsigned slot=(f->selected+i)&3;
            const CombatMoveInfo *m=Combat_GetMoveInfo(f->moves[slot]);
            if(m&&m->category!=2){f->selected=(uint8_t)slot;break;}
        }
    }
    if(input.pressed&COMBAT_R)f->specialSlot=(f->specialSlot+1)&3;
    if(input.pressed&COMBAT_B) {
        f->dashDirection=Facing(dx,dy);Buffer(f,dx||dy?3:4);
    } else if((input.held&COMBAT_B)&&!dx&&!dy&&!f->guardTimer&&!f->guardExhausted) {
        Buffer(f,4);
    } else if(doubleTap&&(dx||dy)) {
        f->dashDirection=Facing(dx,dy);Buffer(f,3);
    } else if(input.pressed&COMBAT_L) Buffer(f,2);
    else if(input.pressed&COMBAT_X) { Buffer(f,1);f->bufferedSlot=0; }
    else if(input.pressed&COMBAT_A) { Buffer(f,1);f->bufferedSlot=f->selected; }
    else if(input.pressed&COMBAT_ALT) { Buffer(f,1);f->bufferedSlot=f->specialSlot; }
    else if(!f->bufferTimer&&f->phase==COMBAT_READY&&(input.held&COMBAT_L)) Buffer(f,2);
    else if(!f->bufferTimer&&f->phase==COMBAT_READY&&(input.held&COMBAT_X)) { Buffer(f,1);f->bufferedSlot=0; }
    else if(!f->bufferTimer&&f->phase==COMBAT_READY&&(input.held&COMBAT_A)) { Buffer(f,1);f->bufferedSlot=f->selected; }
    else if(!f->bufferTimer&&f->phase==COMBAT_READY&&(input.held&COMBAT_ALT)) { Buffer(f,1);f->bufferedSlot=f->specialSlot; }
}
static void FighterStep(Combat *c,unsigned side,CombatInput input)
{
    CombatFighter *f=&c->fighters[side];
    unsigned target=Combat_TargetFighter(c,side);
    struct ArenaPlusMotion motion;
    int dx=!!(input.held&COMBAT_RIGHT)-!!(input.held&COMBAT_LEFT);
    int dy=!!(input.held&COMBAT_DOWN)-!!(input.held&COMBAT_UP);
    int holdingGuard=!!((input.held|input.pressed)&COMBAT_B);
    int speed;
    if(!f->hp) return;
    f->animTick++;
    if(!holdingGuard) f->guardExhausted=0;
    if(f->guardHeld&&(!holdingGuard||!f->stamina||f->hurtTimer)) {
        f->guardHeld=f->guardTimer=0;f->regenDelay=12;
        if(!f->stamina) f->guardExhausted=1;
    }
    if(f->guardTimer) {
        if(f->guardAge<255) f->guardAge++;
        if(f->guardHeld) {
            f->guardTimer=16;f->regenDelay=16;
            if(!(c->frame&3)&&f->stamina) f->stamina--;
            if(!f->stamina) {
                f->guardTimer=f->guardHeld=0;f->guardExhausted=1;f->regenDelay=24;
            }
        } else f->guardTimer--;
    }
    if(f->dashTimer) f->dashTimer--;
    if(f->hurtTimer) f->hurtTimer--;
    if(f->dashCooldown) f->dashCooldown--;
    if(f->aiTimer) f->aiTimer--;
    if(f->aiCooldown) f->aiCooldown--;
    if(f->regenDelay) f->regenDelay--;
    else if(!(c->frame%(c->nativeBalance?6:3))&&f->stamina<(c->nativeBalance?f->staminaMax:COMBAT_STAMINA_MAX))
        f->stamina=(uint8_t)Clamp(f->stamina+(c->nativeBalance?f->regenStep:2),0,c->nativeBalance?f->staminaMax:COMBAT_STAMINA_MAX);
    if(Combat_BeamContact(c,side,0,0)) {
        const struct ArenaMoveProfile *p=ArenaMoves_Get(f->actionMove);
        f->vx=f->vy=0;f->direction=f->attackDirection;
        f->animation=p?p->animation:COMBAT_IDLE;f->bufferTimer=0;
        ClashPushInput(c,side,input);
        memset(f->tapTimer,0,sizeof(f->tapTimer));f->tapReleased=0;
        f->lastDirections=(uint8_t)(input.held&15u);
        return;
    }
    QueueInput(f,input,side==c->controlActor);
    SteerBeam(c,side);
    if(f->status==COMBAT_STATUS_SLEEP||f->status==COMBAT_STATUS_FREEZE) {
        f->bufferTimer=0;f->vx=f->vy=0;f->animation=COMBAT_IDLE;return;
    }
    if(!f->hurtTimer&&!f->guardTimer&&!f->dashTimer&&f->bufferTimer) {
        if(f->buffered>=3) {
            if(CanCancel(f)) {
                uint8_t dir=f->dashDirection;
                Defend(c,side,f->buffered==3?sDirections[dir][0]:0,f->buffered==3?sDirections[dir][1]:0);
                f->bufferTimer=0;
            }
        } else if(f->phase==COMBAT_READY) {
            StartAction(c,side,f->buffered==2,f->bufferedSlot);f->bufferTimer=0;
        }
    }
    if(f->bufferTimer) f->bufferTimer--;
    if(f->hurtTimer) {
        f->vx=f->vx*205/Q;f->vy=f->vy*205/Q;
        MoveBody(f,f->vx,f->vy);f->animation=COMBAT_IDLE;return;
    }
    if(Combat_IsCasting(f)){
        const struct ArenaMoveProfile *p=ArenaMoves_Get(f->actionMove);
        f->vx=f->vy=0;f->direction=f->attackDirection;f->animation=p?p->animation:COMBAT_SPECIAL;return;
    }
    if(f->dashTimer) {
        static const uint16_t dashSpeedByTimer[13]={0,384,512,640,768,896,1024,1152,1280,1344,1280,1024,720};
        int dashSpeed=dashSpeedByTimer[f->dashTimer<=12?f->dashTimer:12];
        if(c->nativeBalance)dashSpeed=dashSpeed*f->dodgeScale/100;
        if(f->dashTimer<=4&&(dx||dy)) {
            unsigned requested=Facing(dx,dy);
            unsigned turn=(requested+8-f->dashDirection)&7;
            if(turn&&turn<=4)f->dashDirection=(f->dashDirection+1)&7;
            else if(turn)f->dashDirection=(f->dashDirection+7)&7;
        }
        f->direction=f->dashDirection;
        f->vx=sDirections[f->dashDirection][0]*dashSpeed/Q;
        f->vy=sDirections[f->dashDirection][1]*dashSpeed/Q;
        MoveBody(f,f->vx,f->vy);f->animation=COMBAT_WALK;
        if(!(c->frame&1)&&(f->x!=f->previousX||f->y!=f->previousY))
            Effect(c,f->x,f->y,RGB(14,23,31),COMBAT_DASH_TRAIL,7)->direction=f->dashDirection;
        return;
    }
    if(f->guardTimer) {
        if(side==c->controlActor&&c->aimOverride)f->direction=Facing(c->aimX,c->aimY);
        else if((c->lockAim||side!=c->controlActor)&&target!=COMBAT_FIGHTERS)
            f->direction=Facing(c->fighters[target].x-f->x,c->fighters[target].y-f->y);
        else if(dx||dy) f->direction=Facing(dx,dy);
        f->vx=f->vy=0;f->animation=COMBAT_IDLE;return;
    }
    if(side==c->controlActor&&c->aimOverride){
        if(f->phase==COMBAT_READY)f->direction=Facing(c->aimX,c->aimY);
    }else if((c->lockAim||side!=c->controlActor)&&target!=COMBAT_FIGHTERS) {
        CombatFighter *v=&c->fighters[target];
        if(f->phase==COMBAT_READY) f->direction=Facing(v->x-f->x,v->y-f->y);
    } else if(dx||dy) {
        if(f->phase==COMBAT_READY) f->direction=Facing(dx,dy);
    }
    speed=c->nativeBalance?f->moveSpeed:Clamp(270+f->speed*2,340,620);
    speed=(int)Staged((unsigned)speed,f->stages[COMBAT_STAT_SPEED],0);
    if(f->status==COMBAT_STATUS_PARALYSIS)speed/=4;
    speed=(int)Clamp(speed,70,1100);
    if(f->slowTimer) speed=speed*4/5;
    if(f->phase==COMBAT_WINDUP) speed=speed*3/4;
    if(f->phase==COMBAT_ACTIVE) speed=speed*2/3;
    if(f->phase==COMBAT_RECOVERY) speed=speed*9/10;
    if(side==c->controlActor&&c->analogInput)speed=speed*c->controlMagnitude/256;
    motion.x=f->vx;motion.y=f->vy;
    ArenaPlus_MoveStep(&motion,dx,dy,speed);
    f->vx=motion.x;f->vy=motion.y;MoveBody(f,f->vx,f->vy);
    {
        int mx=Abs(f->x-f->previousX),my=Abs(f->y-f->previousY);
        int travelled=mx>my?mx+my*3/8:my+mx*3/8;
        f->animation=travelled>8?COMBAT_WALK:COMBAT_IDLE;
        if(f->animation==COMBAT_WALK&&f->phase==COMBAT_READY) {
            /* Two travelled pixels advance one authored timing tick. Feet
             * stop stepping against walls and track real movement speed. */
            f->animTick--;f->walkDistance=(uint16_t)(f->walkDistance+travelled);
            f->animTick+=f->walkDistance/(2*Q);f->walkDistance%=2*Q;
        }
    }
}
static void SeparateBodies(Combat *c)
{
    unsigned i,j;
    for(i=0;i<c->fighterCount;i++) for(j=i+1;j<c->fighterCount;j++) {
        CombatFighter *a=&c->fighters[i],*b=&c->fighters[j];
        int dx=(b->x-a->x)/Q,dy=(b->y-a->y)/Q;
        if(a->hp&&b->hp&&dx*dx+dy*dy<16*16) {
            int sx=Sign(dx),sy=Sign(dy);
            if(!sx&&!sy) sx=1;
            if(!Combat_BeamContact(c,i,0,0)&&!Combat_IsCasting(a))MoveBody(a,-sx*Q/2,-sy*Q/2);
            if(!Combat_BeamContact(c,j,0,0)&&!Combat_IsCasting(b))MoveBody(b,sx*Q/2,sy*Q/2);
        }
    }
}
static void InitFighter(CombatFighter *f,const CombatFighterConfig *p)
{
    unsigned j;
    memset(f,0,sizeof(*f));
    f->species=p->species;f->hp=f->maxHp=Clamp(p->hp,1,8191);
    f->attack=Clamp(p->attack,1,2047);f->defense=Clamp(p->defense,1,2047);
    f->spAttack=Clamp(p->spAttack?p->spAttack:p->attack,1,2047);
    f->spDefense=Clamp(p->spDefense?p->spDefense:p->defense,1,2047);
    f->speed=Clamp(p->speed,1,255);f->types[0]=p->types[0];f->types[1]=p->types[1];
    f->stamina=COMBAT_STAMINA_MAX;
    f->selected=0;f->specialSlot=0;f->actionSlot=4;
    for(j=0;j<4;j++) {
        const CombatMoveInfo *m=Combat_GetMoveInfo(p->moves[j]);
        f->moves[j]=m?p->moves[j]:MOVE_NONE;f->pp[j]=m?m->pp:0;
    }
    for(j=0;j<4;j++) {
        const CombatMoveInfo *m=Combat_GetMoveInfo(f->moves[j]);
        const CombatMoveInfo *selected=Combat_GetMoveInfo(f->moves[f->selected]);
        if(m&&m->category!=2&&(!selected||m->power>selected->power))f->selected=(uint8_t)j;
    }
    for(j=0;j<4;j++) {
        const CombatMoveInfo *m=Combat_GetMoveInfo(f->moves[j]);
        if(m&&m->category==1)f->specialSlot=(uint8_t)j;
    }
    for(j=0;j<4;j++) {
        const CombatMoveInfo *m=Combat_GetMoveInfo(f->moves[j]);
        if(m&&m->category==2){f->specialSlot=(uint8_t)j;break;}
    }
}
static void InitMatch(Combat *c,const CombatFighterConfig *configs,unsigned count)
{
    unsigned i;
    memset(c,0,sizeof(*c));
    ArenaNav_Init();ArenaPhysics_Init();
    c->lockAim=1;c->lockTarget=255;c->aiEnabled=1;c->random=0xD5A4E2u;c->fighterCount=count;
    for(i=0;i<count;i++) {
        CombatFighter *f=&c->fighters[i];
        InitFighter(f,&configs[i]);
        f->x=(ARENA_WORLD_WIDTH/2+((i&1)?62:-62))*Q;
        f->y=(ARENA_WORLD_HEIGHT/2+8+(count>2?(i<2?-30:30):0))*Q;
        f->previousX=f->x;f->previousY=f->y;
        f->direction=(i&1)?COMBAT_W:COMBAT_E;
    }
}
void Combat_Init(Combat *c,const CombatFighterConfig configs[2])
{ InitMatch(c,configs,2); }
void Combat_InitTeams(Combat *c,const CombatFighterConfig configs[COMBAT_FIGHTERS])
{ InitMatch(c,configs,COMBAT_FIGHTERS); }
void Combat_SetClashPractice(Combat *c,int enabled)
{
    unsigned i;
    if(!c)return;
    c->clashPractice=enabled!=0;
    for(i=0;i<c->fighterCount;i++)c->fighters[i].practiceActionSerial=0;
}
int Combat_SetControlledFighter(Combat *c,unsigned side)
{
    if(!c||side>=c->fighterCount||(side&1)||!c->fighters[side].hp) return 0;
    c->controlActor=(uint8_t)side;
    memset(c->fighters[side].tapTimer,0,sizeof(c->fighters[side].tapTimer));
    c->fighters[side].tapReleased=c->fighters[side].lastDirections=0;
    return 1;
}
int Combat_ReplaceFighter(Combat *c,unsigned side,const CombatFighterConfig *config)
{
    int32_t x,y;
    uint8_t direction,was_fainted;
    CombatFighter *f;
    if(!c||!config||side>=c->fighterCount)return 0;
    for(unsigned i=0;i<COMBAT_BEAM_CLASHES;i++)if(c->beamClashes[i].phase==COMBAT_CLASH_PRESSURE&&
        (c->beamClashes[i].actor[0]==side||c->beamClashes[i].actor[1]==side))EndBeamClash(c,&c->beamClashes[i]);
    f=&c->fighters[side];x=f->x;y=f->y;direction=f->direction;
    was_fainted=!f->hp;
    InitFighter(f,config);
    f->x=f->previousX=x;f->y=f->previousY=y;f->direction=direction;
    for(unsigned i=0;i<COMBAT_PROJECTILES;i++)
        if(c->projectiles[i].owner==side)memset(&c->projectiles[i],0,sizeof(c->projectiles[i]));
    c->hitstop=0;
    if(was_fainted&&f->hp)c->winner=0;
    return 1;
}
/* One simulation frame is one clock tick, including impact hitstop. Status,
 * confusion and cooldown clocks therefore cannot be prolonged by animation. */
static void StatusStep(Combat *c)
{
    unsigned side,slot;
    for(side=0;side<c->fighterCount;side++) {
        CombatFighter *f=&c->fighters[side];
        if(f->naturalCooldown)f->naturalCooldown--;
        for(slot=0;slot<4;slot++)if(f->cooldown[slot])f->cooldown[slot]--;
        if(!f->hp)continue;
        if(f->defenseBoost)f->defenseBoost--;
        if(f->lightScreenTicks)f->lightScreenTicks--;
        if(f->slowTimer)f->slowTimer--;
        if(f->statusLock)f->statusLock--;
        if(f->confusionTicks)f->confusionTicks--;
        if(f->status)f->statusClock++;
        if(f->status==COMBAT_STATUS_SLEEP||f->status==COMBAT_STATUS_FREEZE) {
            if(f->statusTicks)f->statusTicks--;
            if(!f->statusTicks||(f->status==COMBAT_STATUS_FREEZE&&
                !(f->statusClock%60)&&NextRandom(c)%100<20)) {
                f->status=0;f->statusTicks=f->statusClock=0;
                Effect(c,f->x,f->y,RGB(20,28,31),COMBAT_RING,8);
            }
        } else if(f->statusClock>=180&&
            (f->status==COMBAT_STATUS_BURN||f->status==COMBAT_STATUS_POISON||f->status==COMBAT_STATUS_BAD_POISON)) {
            unsigned damage;
            f->statusClock=0;
            damage=f->status==COMBAT_STATUS_BAD_POISON?f->maxHp*f->toxicCounter/16:f->maxHp/8;
            if(!damage)damage=1;
            f->hp=(uint16_t)(f->hp>damage?f->hp-damage:0);
            if(f->status==COMBAT_STATUS_BAD_POISON&&f->toxicCounter<15)f->toxicCounter++;
            Effect(c,f->x,f->y,Combat_TypeColor(f->status==COMBAT_STATUS_BURN?10:3),COMBAT_CONTACT,8);
            if(!f->hp) {EndAction(f);f->dashTimer=f->guardTimer=f->guardHeld=0;f->bufferTimer=0;}
        }
    }
}
void Combat_Tick(Combat *c,CombatInput input)
{
    unsigned i;
    CombatInput controls[COMBAT_FIGHTERS]={{0,0}};
    uint8_t alive[2]={0,0};
    c->frame++;EffectStep(c);
    if(c->impactTimer) c->impactTimer--;
    if(c->winner) return;
    StatusStep(c);
    if(input.pressed&COMBAT_COMMAND)c->tactic=(c->tactic+1)%3;
    if(input.pressed&COMBAT_Y) c->lockAim=!c->lockAim;
    if(c->hitstop) {
        c->hitstop--;
        if(c->fighters[c->controlActor].hp&&!ClashPushInput(c,c->controlActor,input))
            QueueInput(&c->fighters[c->controlActor],input,1);
        return;
    }
    for(i=0;i<c->fighterCount;i++) {
        c->fighters[i].previousX=c->fighters[i].x;
        c->fighters[i].previousY=c->fighters[i].y;
    }
    BeamClashStep(c);
    controls[c->controlActor]=input;
    if(c->aiEnabled) for(i=0;i<c->fighterCount;i++) if(i!=c->controlActor) controls[i]=AIInput(c,i);
    for(i=0;i<c->fighterCount;i++) FighterStep(c,i,controls[i]);
    BeamClashes(c);
    for(i=0;i<c->fighterCount;i++) if(!c->fighters[i].hurtTimer&&!c->fighters[i].dashTimer) ActionStep(c,i);
    SeparateBodies(c);ProjectileStep(c);ArenaPhysics_Update();
    if(c->blastSerial!=gArenaPhysicsTelemetry.blastSerial) {
        c->blastSerial=gArenaPhysicsTelemetry.blastSerial;
        Burst(c,gArenaPhysicsTelemetry.blastX*Q,gArenaPhysicsTelemetry.blastY*Q,RGB(31,22,3));
        /* Terrain explosions are visual/physical only in this prototype. */
    }
    for(i=0;i<c->fighterCount;i++) if(c->fighters[i].hp) alive[i&1]=1;
    if(!alive[1]) c->winner=1;
    else if(!alive[0]) c->winner=2;
}
