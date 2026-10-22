/* Fixed-point locomotion and registered body geometry. Included by combat.c. */
void Combat_ConfigureAnimation(Combat *c,unsigned actor,const CombatMotionProfile *p)
{
    if(!c||actor>=c->fighterCount||!p||!p->cycle||!p->stride)return;
    c->fighters[actor].motion=*p;
    c->fighters[actor].gaitPhase=c->fighters[actor].gaitRemainder=0;
}
int32_t Combat_HoverHeight(const CombatFighter *f)
{
    unsigned phase=f->animTick%64;
    if(!f->motion.hover)return 0;
    /* A small deterministic hover; the renderer and body collider share it. */
    return 3*Q+(int32_t)(phase<32?phase:64-phase)*Q/32;
}
void Combat_Hurtbox(const CombatFighter *f,CombatShape *s)
{
    unsigned d=f->direction&7;memset(s,0,sizeof(*s));
    s->x0=s->x1=f->x;s->y0=s->y1=f->y;s->kind=COMBAT_SHAPE_ELLIPSE;s->active=!!f->hp;
    s->radiusX=s->radiusY=9;
    if(f->motion.cycle){
        s->x0=s->x1=f->x+f->motion.centerX[d];s->y0=s->y1=f->y+f->motion.centerY[d]-Combat_HoverHeight(f);
        s->radiusX=f->motion.radiusX[d];s->radiusY=f->motion.radiusY[d];
    }
}
void Combat_AttackOrigin(const CombatFighter *f,int32_t *x,int32_t *y)
{
    CombatShape body;Combat_Hurtbox(f,&body);
    if(x)*x=body.x0;
    if(y)*y=body.y0;
}
static int NativeTail(unsigned move)
{return move==MOVE_IRON_TAIL||move==MOVE_AQUA_TAIL||move==MOVE_POISON_TAIL;}
unsigned Combat_MoveRange(const CombatFighter *f,uint16_t move,const CombatFighter *target)
{
    const struct ArenaMoveProfile *p=ArenaMoves_Get(move);
    const CombatMoveInfo *m=Combat_GetMoveInfo(move);CombatShape a,b;
    if(!p)return 0;
    if(!f->motion.cycle||!m||m->category!=0||NativeCharge(move)||
       (p->kind!=ARENA_MOVE_MELEE&&p->kind!=ARENA_MOVE_RUSH))return p->range;
    Combat_Hurtbox(f,&a);Combat_Hurtbox(target?target:f,&b);
    {unsigned forward=a.radiusX>a.radiusY?a.radiusX:a.radiusY,other=b.radiusX>b.radiusY?b.radiusX:b.radiusY;
     if(NativeTail(move))return a.radiusX+20+other;
     return forward/2+(unsigned)Clamp(forward/2+10,12,23)+(p->visual==ARENA_VIS_BITE?4:5)+other;}
}
void Combat_AttackShape(const CombatFighter *f,CombatShape *s)
{
    const struct ArenaMoveProfile *p=ArenaMoves_Get(f->actionMove);
    const CombatMoveInfo *m=Combat_GetMoveInfo(f->actionMove);
    CombatShape body;int16_t ax,ay;unsigned reach,radius,dir;
    memset(s,0,sizeof(*s));
    if(!p||!m||!f->hp||f->phase!=COMBAT_ACTIVE||p->kind==ARENA_MOVE_PROJECTILE||
       (m->category==2&&NativeRule(f->actionMove)->selfTarget))return;
    Combat_Hurtbox(f,&body);Combat_AttackVector(f,&ax,&ay);
    s->kind=COMBAT_SHAPE_CAPSULE;s->active=1;s->x0=body.x0;s->y0=body.y0;
    radius=(unsigned)Clamp((int)p->radius/4,3,8);reach=p->range;
    if(p->kind==ARENA_MOVE_SELF&&m->category!=2){
        s->kind=COMBAT_SHAPE_ELLIPSE;s->x1=s->x0;s->y1=s->y0;
        s->radiusX=s->radiusY=p->range?p->range:p->radius;return;
    }
    if(ArenaMoves_Beam(f->actionMove)){
        reach=f->beamReach;radius=(unsigned)Clamp(p->radius/2,5,15);
        s->active=!!reach;
    }else if(NativeCharge(f->actionMove)){
        /* The charge's real swept path, not its authored maximum range. */
        s->x0=f->previousX+(body.x0-f->x);s->y0=f->previousY+(body.y0-f->y);
        s->x1=body.x0;s->y1=body.y0;s->radiusX=body.radiusX;s->radiusY=body.radiusY;return;
    }else if(NativeTail(f->actionMove)){
        unsigned active=Active(f,p),age=Combat_PhaseTick(f);
        /* Three readable sectors sweep across the facing side of the body. */
        dir=(f->attackDirection+7+(active?age*3/active:0))&7;
        ax=sDirections[dir][0];ay=sDirections[dir][1];reach=body.radiusX+15;
        s->kind=COMBAT_SHAPE_ARC;radius=5;
    }else if(m->category==0){
        unsigned forward=body.radiusX>body.radiusY?body.radiusX:body.radiusY;
        reach=(unsigned)Clamp(forward/2+10,12,23);
        s->x0+=ax*(int)forward/2;s->y0+=ay*(int)forward/2;
        radius=p->visual==ARENA_VIS_BITE?4:5;
    }
    s->x1=s->x0+ax*(int)reach;s->y1=s->y0+ay*(int)reach;
    s->radiusX=s->radiusY=(uint16_t)radius;
}
int Combat_ShapeHits(const CombatShape *a,const CombatShape *b)
{
    int64_t x,y,vx,vy,length,t=0;unsigned rx,ry;
    if(!a||!b||!a->active||!b->active)return 0;
    rx=a->radiusX+b->radiusX;ry=a->radiusY+b->radiusY;
    if(!rx||!ry)return 0;
    /* Normalize the Minkowski ellipse. Keep Q8 precision and 64-bit products
     * for a moving projectile or charge; no endpoint-only tunnelling. */
    x=(a->x0-b->x0)/(int)rx;y=(a->y0-b->y0)/(int)ry;
    vx=(a->x1-a->x0)/(int)rx;vy=(a->y1-a->y0)/(int)ry;
    length=vx*vx+vy*vy;
    if(length){t=-(x*vx+y*vy)*65536/length;if(t<0)t=0;if(t>65536)t=65536;}
    x+=vx*t/65536;y+=vy*t/65536;
    return x*x+y*y<=(int64_t)Q*Q;
}
static void TurnBody(CombatFighter *f,unsigned wanted)
{
    unsigned turn=(wanted+8-f->direction)&7;
    if(!turn||f->turnTimer)return;
    f->direction=(f->direction+(turn<=4?1:7))&7;
    f->turnTimer=f->motion.turnTicks;
}
static int32_t ApproachMotion(int32_t current,int32_t target,unsigned step)
{
    int32_t difference=target-current;
    return current+Clamp(difference,-(int32_t)step,(int32_t)step);
}
static void NativeMotion(CombatFighter *f,int dx,int dy,int speed)
{
    int tx=dx*speed,ty=dy*speed;
    unsigned step=f->motion.acceleration;
    if(dx&&dy){tx=tx*181/Q;ty=ty*181/Q;}
    if((!dx&&!dy)||(int64_t)f->vx*tx+(int64_t)f->vy*ty<0)step=f->motion.braking;
    f->vx=ApproachMotion(f->vx,tx,step);f->vy=ApproachMotion(f->vy,ty,step);
    if(!dx&&Abs(f->vx)<8)f->vx=0;
    if(!dy&&Abs(f->vy)<8)f->vy=0;
}
static void AdvanceGait(CombatFighter *f,int32_t x,int32_t y)
{
    int32_t dx=f->x-x,dy=f->y-y;
    unsigned distance=Root((unsigned)(dx*dx+dy*dy));
    uint64_t ticks;
    if(!f->motion.cycle||!f->motion.stride||!distance)return;
    ticks=(uint64_t)distance*f->motion.cycle*Q+f->gaitRemainder;
    f->gaitPhase=(f->gaitPhase+(uint32_t)(ticks/f->motion.stride))%(f->motion.cycle*Q);
    f->gaitRemainder=(uint32_t)(ticks%f->motion.stride);
}
