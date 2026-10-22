#include "game.h"
#include <string.h>
static unsigned LimitMotion(unsigned n,unsigned lo,unsigned hi){return n<lo?lo:n>hi?hi:n;}
void Game_ConfigureMotion(ArenaGame *g,unsigned actor)
{
    CombatFighter *f=&g->combat.fighters[actor];
    const ArenaSpecies *s=Game_ActorSpecies(g,actor);
    const ArenaSpriteAnimation *walk,*hurt;
    CombatMotionProfile p;unsigned d,i,scale,width=0,boost=8,accel=5,brake=4,stride=0;
    if(!s)return;
    walk=&s->animation[COMBAT_WALK];hurt=ArenaSpecies_Hurt(s->dex);
    if(!walk->count||!walk->totalTicks)return;
    memset(&p,0,sizeof(p));scale=(unsigned)(s->scale*256+.5f);
    p.cycle=walk->totalTicks;p.turnTicks=2;p.hover=s->style==2?1:0;
    /* World stride is an authored calibration, not a fixed distance per
     * metadata tick. Original cel durations remain the relative gait timing. */
    switch(s->dex){
    case 25:boost=12;accel=4;brake=3;stride=24;p.turnTicks=1;break;
    case 448:boost=9;accel=5;brake=4;stride=30;break;
    case 9:boost=6;accel=9;brake=6;stride=28;p.turnTicks=3;break;
    case 94:boost=7;accel=6;brake=5;stride=28;p.hover=2;break;
    case 493:boost=7;accel=7;brake=5;stride=44;p.turnTicks=2;break;
    default:break;
    }
    f->moveSpeed=(uint16_t)(f->moveSpeed*(100+boost)/100);
    p.acceleration=(uint16_t)((f->moveSpeed+accel-1)/accel);
    p.braking=(uint16_t)((f->moveSpeed+brake-1)/brake);
    p.hurtTicks=(uint8_t)LimitMotion(hurt?hurt->totalTicks:10,6,18);
    for(d=0;d<8;d++){
        /* Register each direction from the source neutral walk body. Crop
         * canvas changes and Hurt cels never change this collision envelope. */
        const ArenaSpriteFrame *frame=&walk->frames[d*walk->count];
        p.centerX[d]=(int16_t)(((int)frame->width/2-frame->originX)*(int)scale);
        p.centerY[d]=(int16_t)(((int)frame->height*3/5-frame->originY)*(int)scale);
        p.radiusX[d]=(uint8_t)LimitMotion(frame->width*scale/768,7,27);
        p.radiusY[d]=(uint8_t)LimitMotion(frame->height*scale/896,7,24);
        width+=frame->width*scale/256;
    }
    width/=8;
    p.groundRadius=(uint8_t)LimitMotion(width/3,6,18);
    p.stride=(uint16_t)((stride?stride:LimitMotion(width*9/10,16,50))*256);
    for(i=0;i<5;i++){
        const ArenaSpriteAnimation *a=&s->animation[i];unsigned tick=0,frame=0;
        p.action[i].total=a->totalTicks;p.action[i].hit=a->hitTick;
        while(frame+1<a->count&&tick+a->durations[frame]<=a->hitTick)tick+=a->durations[frame++];
        p.action[i].contact=a->count?a->durations[frame]:1;
    }
    Combat_ConfigureAnimation(&g->combat,actor,&p);
}
