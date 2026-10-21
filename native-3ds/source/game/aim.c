/* Independent world-space targeting. No renderer, touch driver or second
 * analogue stick is required by the simulation. */
#include "game.h"
#include "arena/vendor/arena_moves.h"
#include "arena/vendor/arena_navigation.h"
#include <stdlib.h>

static const int16_t facing[8][2]={{0,256},{181,181},{256,0},{181,-181},
    {0,-256},{-181,-181},{-256,0},{-181,181}};
static int Opponent(const ArenaGame *g,unsigned actor)
{return actor<g->combat.fighterCount&&((actor^g->combat.controlActor)&1)&&g->combat.fighters[actor].hp;}
void Game_CycleTarget(ArenaGame *g)
{
    unsigned i,start=g->aim.target<4?g->aim.target:0;
    for(i=1;i<=4;i++)if(Opponent(g,(start+i)%4)){
        g->aim.target=(uint8_t)((start+i)%4);g->aim.mode=GAME_AIM_LOCK;return;
    }
    g->aim.target=255;
}
void Game_AimTick(ArenaGame *g,GameInput input)
{
    Combat *c=&g->combat;CombatFighter *f=&c->fighters[c->controlActor];
    GameAim *a=&g->aim;const struct ArenaMoveProfile *p;
    unsigned target,i;int dx,dy,ox,oy,override=0;
    uint16_t move=f->phase!=COMBAT_READY?f->actionMove:f->moves[f->selected&3];
    if(f->phase==COMBAT_READY&&!(input.held&(GAME_L|GAME_R))){
        if(input.held&GAME_Y)move=Combat_NaturalMove(f);
        else if(input.held&GAME_X)move=f->moves[f->specialSlot&3];
    }
    if(input.pressed&GAME_SELECT)a->mode=a->mode==GAME_AIM_LOCK?GAME_AIM_FACING:GAME_AIM_LOCK;
    if((input.held&GAME_R)&&(input.pressed&GAME_Y)){Game_CycleTarget(g);g->rUsed=1;}
    if(input.aimActive&&(abs(input.aimX)>5||abs(input.aimY)>5)){
        a->mode=GAME_AIM_FREE;a->freeX=input.aimX;a->freeY=input.aimY;
    }
    if(!Opponent(g,a->target)){
        c->lockTarget=255;target=Combat_TargetFighter(c,c->controlActor);a->target=(uint8_t)target;
    }
    target=a->target;c->lockAim=a->mode==GAME_AIM_LOCK;c->lockTarget=(uint8_t)target;
    p=ArenaMoves_Get(move);a->range=p&&p->range?p->range:64;
    a->assisted=0;ox=f->x/256;oy=f->y/256;
    dx=facing[f->direction&7][0];dy=facing[f->direction&7][1];
    if(a->mode==GAME_AIM_FREE){dx=a->freeX;dy=a->freeY;override=1;}
    else if(a->mode==GAME_AIM_LOCK&&Opponent(g,target)){
        dx=c->fighters[target].x/256-ox;dy=c->fighters[target].y/256-oy;override=1;
    }else if(a->assistEnabled&&f->phase==COMBAT_READY&&!(input.held&(GAME_L|GAME_R))&&
             ((input.held&(GAME_A|GAME_X|GAME_Y))||input.touchMove>=0)){
        int best=0x7fffffff;
        for(i=0;i<c->fighterCount;i++)if(Opponent(g,i)){
            int tx=c->fighters[i].x/256-ox,ty=c->fighters[i].y/256-oy;
            int dot=tx*dx+ty*dy,cross=abs(tx*dy-ty*dx),distance=tx*tx+ty*ty;
            if(dot>0&&dot>=cross*3&&distance<=(int)(a->range+8)*(a->range+8)&&
               ArenaNav_LineClear(ox,oy,ox+tx,oy+ty,1)&&distance<best){best=distance;target=i;}
        }
        if(best!=0x7fffffff){dx=c->fighters[target].x/256-ox;dy=c->fighters[target].y/256-oy;override=1;a->assisted=1;}
    }
    Combat_SetAim(c,dx,dy,override);
    if(override){dx=c->aimX;dy=c->aimY;}
    if(a->mode==GAME_AIM_LOCK&&Opponent(g,target)){
        a->x=c->fighters[target].x/256;a->y=c->fighters[target].y/256;
    }else {a->x=ox+dx*a->range/256;a->y=oy+dy*a->range/256;}
    dx=(int)a->x-ox;dy=(int)a->y-oy;
    a->blocked=(uint8_t)!ArenaNav_LineClear(ox,oy,(int)a->x,(int)a->y,1);
    a->valid=(uint8_t)(!a->blocked&&dx*dx+dy*dy<=(int)(a->range+1)*(a->range+1));
}
