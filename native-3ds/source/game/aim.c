/* Sticky opponent lock or single-Circle-Pad free aim. Touch only presses
 * commands and move cards; it never supplies an aiming vector. */
#include "game.h"
#include "arena/vendor/arena_moves.h"
#include "arena/vendor/arena_navigation.h"

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
    unsigned target;int dx,dy,ox=f->x/256,oy=f->y/256;
    uint16_t move=f->phase!=COMBAT_READY?f->actionMove:f->moves[f->selected&3];
    if(f->phase==COMBAT_READY&&!(input.held&(GAME_L|GAME_R))){
        if(input.held&GAME_Y)move=Combat_NaturalMove(f);
        else if(input.held&GAME_X)move=f->moves[f->specialSlot&3];
    }
    /* Removing the lock preserves its current direction until the player
     * moves the pad. Movement can never silently cancel a lock. */
    if(input.pressed&GAME_SELECT){
        if(a->mode==GAME_AIM_LOCK){
            a->freeX=c->aimX;a->freeY=c->aimY;
            if(Opponent(g,a->target)&&!f->motion.cycle){
                a->freeX=(int16_t)(c->fighters[a->target].x/256-ox);
                a->freeY=(int16_t)(c->fighters[a->target].y/256-oy);
            }
            a->mode=GAME_AIM_FREE;
        }else a->mode=GAME_AIM_LOCK;
    }
    if((input.held&GAME_R)&&(input.pressed&GAME_Y)){Game_CycleTarget(g);g->rUsed=1;}
    if(!Opponent(g,a->target)){
        c->lockTarget=255;a->target=(uint8_t)Combat_TargetFighter(c,c->controlActor);
    }
    target=a->target;c->lockAim=a->mode==GAME_AIM_LOCK;c->lockTarget=(uint8_t)target;
    if(f->motion.cycle){int32_t x,y;Combat_AttackOrigin(f,&x,&y);ox=x/256;oy=y/256;}
    p=ArenaMoves_Get(move);a->range=p&&p->range?p->range:64;
    if(f->motion.cycle)a->range=(uint16_t)Combat_MoveRange(f,move,target<c->fighterCount?&c->fighters[target]:0);
    dx=facing[f->direction&7][0];dy=facing[f->direction&7][1];
    if(a->mode==GAME_AIM_LOCK&&Opponent(g,target)){
        if(f->motion.cycle){CombatShape body;Combat_Hurtbox(&c->fighters[target],&body);dx=body.x0/256-ox;dy=body.y0/256-oy;}
        else {dx=c->fighters[target].x/256-ox;dy=c->fighters[target].y/256-oy;}
    }else if(a->mode==GAME_AIM_FREE){
        if((int)input.axisX*input.axisX+(int)input.axisY*input.axisY>18*18){
            a->freeX=input.axisX;a->freeY=input.axisY;
        }else{
            int x=!!(input.held&GAME_RIGHT)-!!(input.held&GAME_LEFT);
            int y=!!(input.held&GAME_DOWN)-!!(input.held&GAME_UP);
            if(x||y){a->freeX=(int16_t)(x*256);a->freeY=(int16_t)(y*256);}
        }
        if(a->freeX||a->freeY){dx=a->freeX;dy=a->freeY;}
    }
    Combat_SetAim(c,dx,dy,1);
    a->freeX=c->aimX;a->freeY=c->aimY;
    if(a->mode==GAME_AIM_LOCK&&Opponent(g,target)){
        if(f->motion.cycle){CombatShape body;Combat_Hurtbox(&c->fighters[target],&body);a->x=body.x0/256;a->y=body.y0/256;}
        else {a->x=c->fighters[target].x/256;a->y=c->fighters[target].y/256;}
    }else {a->x=ox+c->aimX*a->range/256;a->y=oy+c->aimY*a->range/256;}
    dx=(int)a->x-ox;dy=(int)a->y-oy;
    a->blocked=(uint8_t)!ArenaNav_LineClear(ox,oy,(int)a->x,(int)a->y,1);
    a->valid=(uint8_t)(!a->blocked&&dx*dx+dy*dy<=(int)(a->range+1)*(a->range+1));
}
