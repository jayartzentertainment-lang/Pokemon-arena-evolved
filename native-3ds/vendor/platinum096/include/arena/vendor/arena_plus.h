// Pokemon Emerald Arena Plus v0.0.1 — GPL-compatible gameplay helpers.
#ifndef GUARD_ARENA_PLUS_H
#define GUARD_ARENA_PLUS_H
#include "arena/vendor/portable_types.h"
struct ArenaPlusMotion {s32 x,y;};
// Fixed point Q8 movement: deliberately bounded inertia and an immediate dash.
static inline s32 ArenaPlus_Approach(s32 current,s32 target)
{
    s32 step=(target-current)/3;
    if(!step&&target!=current)step=target>current?1:-1;
    return current+step;
}
static inline void ArenaPlus_MoveStep(struct ArenaPlusMotion *motion,s32 dx,s32 dy,s32 speed)
{
    s32 tx=dx*speed,ty=dy*speed;
    if(dx&&dy){tx=tx*181/256;ty=ty*181/256;}
    motion->x=ArenaPlus_Approach(motion->x,tx);
    motion->y=ArenaPlus_Approach(motion->y,ty);
    if(!dx&&motion->x>-8&&motion->x<8)motion->x=0;
    if(!dy&&motion->y>-8&&motion->y<8)motion->y=0;
}
// Same-frame projectile path contact. All coordinates are Q8 pixels.
// Compute closest relative approach, avoiding tunnelling between frames.
static inline u8 ArenaPlus_SweptContact(s32 ax0,s32 ay0,s32 ax1,s32 ay1,
                                         s32 bx0,s32 by0,s32 bx1,s32 by1,
                                         s32 radiusPixels)
{
    s32 dx=(ax0-bx0)/256,dy=(ay0-by0)/256;
    s32 vx=((ax1-ax0)-(bx1-bx0))/256;
    s32 vy=((ay1-ay0)-(by1-by0))/256;
    s32 denom=vx*vx+vy*vy,t=0;
    if (denom)
    {
        t=-(dx*vx+dy*vy)*256/denom;
        if(t<0)t=0;
        if(t>256)t=256;
    }
    dx+=vx*t/256;
    dy+=vy*t/256;
    return dx*dx+dy*dy<=radiusPixels*radiusPixels;
}
// Returns 0 classic, 1 action; unsupported battles never force action.
static inline u8 ArenaPlus_UseAction(u8 compatible,u8 forceClassic)
{return compatible&&!forceClassic;}
#endif
