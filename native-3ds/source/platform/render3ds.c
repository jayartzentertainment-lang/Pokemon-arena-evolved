/* Native Citro2D presentation. No Nitro/DS VRAM/OAM calls are reused. */
#include "render3ds.h"
#include "generated/effects.h"
#include "arena/vendor/arena_moves.h"
#include "arena/vendor/arena_navigation.h"
#include "arena/vendor/arena_physics.h"
#include "arena/vendor/constants/moves.h"
#include <citro2d.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.14159265358979323846f
static const float direction[8][2]={{0,1},{.7071f,.7071f},{1,0},{.7071f,-.7071f},
    {0,-1},{-.7071f,-.7071f},{-1,0},{-.7071f,.7071f}};
struct ArenaView {
    C3D_RenderTarget *top,*bottom;
    C2D_SpriteSheet pokemon[4],effects,arena,font,props,ui;
    float cameraX,cameraY,zoom,textDepth;
    unsigned loads,quality;
};
static float Limit(float x,float a,float b){return x<a?a:x>b?b:x;}
static u32 RGB(unsigned r,unsigned g,unsigned b){return C2D_Color32(r,g,b,255);}
static void Line(float x0,float y0,float x1,float y1,float thickness,float depth,u32 color)
{ C2D_DrawLine(x0,y0,color,x1,y1,color,thickness,depth); }
static float X(const ArenaView *v,float x){return (x-v->cameraX)*v->zoom;}
static float Y(const ArenaView *v,float y){return 30+(y-v->cameraY)*v->zoom;}
static float ActorX(const CombatFighter *f,float a){return (f->previousX+(f->x-f->previousX)*a)/256.0f;}
static float ActorY(const CombatFighter *f,float a){return (f->previousY+(f->y-f->previousY)*a)/256.0f;}

static C2D_SpriteSheet Load(ArenaView *v,const char *path)
{
    C2D_SpriteSheet sheet=C2D_SpriteSheetLoad(path);size_t i;
    if(!sheet)return 0;
    v->loads++;
    for(i=0;i<C2D_SpriteSheetCount(sheet);i++){
        C2D_Image image=C2D_SpriteSheetGetImage(sheet,i);
        C3D_TexSetFilter(image.tex,GPU_NEAREST,GPU_NEAREST);
    }
    return sheet;
}
static void Text(ArenaView *v,float x,float y,u32 color,const char *format,...)
{
    char buffer[160];va_list args;unsigned i;
    C2D_ImageTint tint;
    va_start(args,format);vsnprintf(buffer,sizeof(buffer),format,args);va_end(args);
    C2D_PlainImageTint(&tint,color,1);
    for(i=0;buffer[i];i++){
        unsigned ch=(unsigned char)toupper((unsigned char)buffer[i]);
        if(ch=='\n'){y+=12;x=8;continue;}
        if(ch<32||ch>127)ch='?';
        if(ch!=' ')C2D_DrawImageAt(C2D_SpriteSheetGetImage(v->font,ch-32),x,y,v->textDepth,&tint,1,1);
        x+=8;
    }
}
static void Sprite(C2D_SpriteSheet sheet,unsigned image,float x,float y,float sx,float sy,
                   float angle,float cx,float cy,float alpha,float flash)
{
    C2D_Sprite sprite;C2D_ImageTint tint;
    if(!sheet||image>=C2D_SpriteSheetCount(sheet))return;
    C2D_SpriteFromSheet(&sprite,sheet,image);
    C2D_SpriteSetDepth(&sprite,.4f);
    C2D_SpriteSetCenter(&sprite,cx,cy);C2D_SpriteSetPos(&sprite,x,y);
    C2D_SpriteSetScale(&sprite,sx,sy);C2D_SpriteSetRotation(&sprite,angle);
    C2D_PlainImageTint(&tint,C2D_Color32(255,255,255,(u8)(Limit(alpha,0,1)*255)),flash);
    C2D_DrawImage(sprite.image,&sprite.params,&tint);
}
static void Effect(ArenaView *v,unsigned image,float x,float y,float width,float height,float angle,float alpha)
{
    C2D_Image item=C2D_SpriteSheetGetImage(v->effects,image);
    Sprite(v->effects,image,x,y,width/item.subtex->width,height/item.subtex->height,
           angle,.5f,.5f,alpha,0);
}
static unsigned AnimFrame(const ArenaSpriteAnimation *a,unsigned tick)
{
    unsigned frame=0;if(!a->count||!a->totalTicks)return 0;tick%=a->totalTicks;
    while(frame+1<a->count&&tick>=a->durations[frame]){tick-=a->durations[frame];frame++;}
    return frame;
}
static const ArenaSpriteFrame *Pose(const CombatFighter *f,const ArenaSpecies *s)
{
    unsigned slot=f->animation<5?f->animation:0;
    const ArenaSpriteAnimation *a;
    unsigned frame;
    if(f->hurtTimer)slot=COMBAT_IDLE;
    if(f->guardTimer)slot=COMBAT_ATTACK;
    a=&s->animation[slot];
    frame=AnimFrame(a,Combat_AnimationTick(f,a->totalTicks,a->hitTick));
    if(f->guardTimer)frame=AnimFrame(a,a->hitTick?a->hitTick/2:0);
    return &a->frames[(f->direction&7)*a->count+frame];
}
static C2D_SpriteSheet Pokemon(ArenaView *v,unsigned dex)
{
    unsigned i;
    for(i=0;i<4;i++)if(gSpecies[i].dex==dex){
        if(!v->pokemon[i])v->pokemon[i]=Load(v,gSpecies[i].texturePath);
        return v->pokemon[i];
    }
    return 0;
}

static void Camera(ArenaView *v,const ArenaGame *g)
{
    const Combat *c=&g->combat;unsigned i;float left=1000,right=0,top=1000,bottom=0;
    float zoom,worldWidth,worldHeight,cx,cy;
    for(i=0;i<c->fighterCount;i++)if(c->fighters[i].hp){
        float x=c->fighters[i].x/256.0f,y=c->fighters[i].y/256.0f;
        if(x<left)left=x;
        if(x>right)right=x;
        if(y<top)top=y;
        if(y>bottom)bottom=y;
    }
    if(left>right){left=100;right=340;top=80;bottom=240;}
    zoom=Limit(fminf(380/(right-left+100),188/(bottom-top+100)),.62f,g->doubles?.88f:1.08f);
    cx=(left+right)/2;cy=(top+bottom)/2-9;
    if(c->impactTimer&&c->impactActor<4){
        float amount=c->impactTimer/180.0f;
        zoom+=amount;cx+=(c->impactX/256.0f-cx)*amount;cy+=(c->impactY/256.0f-cy)*amount;
    }
    worldWidth=400/zoom;worldHeight=202/zoom;
    v->zoom=zoom;
    v->cameraX=worldWidth<448?Limit(cx-worldWidth/2,0,448-worldWidth):(448-worldWidth)/2;
    v->cameraY=worldHeight<320?Limit(cy-worldHeight/2,0,320-worldHeight):(320-worldHeight)/2;
}
static void Beam(ArenaView *v,const ArenaGame *g,unsigned actor,float alpha)
{
    const CombatFighter *f=&g->combat.fighters[actor];
    const ArenaSpecies *species=Game_ActorSpecies(g,actor);
    int16_t vx,vy;float dx,dy,wx,wy,endX,endY,reach,opacity=1;
    unsigned age=Combat_PhaseTick(f),image,k,emitters=1;
    int32_t contactX,contactY;
    int clash=Combat_BeamContact(&g->combat,actor,&contactX,&contactY);
    if(!species||!f->hp||f->hurtTimer||f->phase==COMBAT_READY)return;
    Combat_AttackVector(f,&vx,&vy);dx=vx/256.0f;dy=vy/256.0f;
    wx=ActorX(f,alpha);wy=ActorY(f,alpha);reach=f->beamReach;
    if(f->phase==COMBAT_WINDUP){
        unsigned duration=Combat_PhaseDuration(f);
        if(f->actionMove!=MOVE_FLAMETHROWER||age+6<duration)return;
        reach=10+(age+6-duration)*2;opacity=.55f;
    }else if(f->phase==COMBAT_RECOVERY){
        if(age>=6)return;
        opacity=1-age/6.0f;
    }else if(f->phase!=COMBAT_ACTIVE)return;
    if(!reach&&!clash)return;
    endX=wx+dx*reach;endY=wy+dy*reach-12;
    if(clash){
        endX=contactX/256.0f;endY=contactY/256.0f-12;
        reach=sqrtf((endX-wx)*(endX-wx)+(endY+12-wy)*(endY+12-wy));
    }
    image=FX_FLAME+(g->combat.frame/4)%8;
    if(f->actionMove==MOVE_HYDRO_PUMP){image=FX_HYDRO+(g->combat.frame/3)%4;emitters=2;}
    for(k=0;k<emitters;k++){
        unsigned dir=f->attackDirection&7;
        float startX=wx+dx*9*species->scale,startY=wy-15*species->scale+dy*6*species->scale;
        if(f->actionMove==MOVE_HYDRO_PUMP){
            static const uint8_t muzzle[8][2][2]={
                {{21,30},{43,30}},{{28,31},{42,25}},{{38,29},{38,29}},{{40,22},{28,17}},
                {{22,14},{42,14}},{{24,22},{36,17}},{{26,29},{26,29}},{{22,25},{36,31}}};
            static const uint8_t ground[8]={5,8,8,7,9,7,8,8};
            if(k&&(dir==2||dir==6))continue;
            startX=wx+((int)muzzle[dir][k][0]-32)*species->scale;
            startY=wy+((int)muzzle[dir][k][1]-32-ground[dir])*species->scale;
        }else if(f->species==6){
            /* Mouth coordinates on the supplied Charizard Shoot contact cels. */
            static const int8_t mouth[8][2]={{0,-10},{-6,-9},{19,-10},{12,-18},
                {0,-24},{-12,-18},{-19,-10},{6,-9}};
            startX=wx+mouth[dir][0]*species->scale;
            startY=wy+mouth[dir][1]*species->scale-3-sinf(g->combat.frame*.10f);
        }
        {
            float x=X(v,startX),y=Y(v,startY),ex=X(v,endX),ey=Y(v,endY);
            float sx=ex-x,sy=ey-y,length=sqrtf(sx*sx+sy*sy),theta=atan2f(sy,sx);
            float width=(emitters==2?30:Limit(reach*2/3.0f,12,80))*v->zoom;
            C2D_Image item=C2D_SpriteSheetGetImage(v->effects,image);
            if(length<1)continue;
            /* Original animated Emerald plume / supplied water texture: no
             * solid beam rectangles or generic projectile stand-ins. */
            Sprite(v->effects,image,x,y,length/item.subtex->width,width/item.subtex->height,theta,0,.5f,opacity,0);
            if(v->quality)Sprite(v->effects,image,x,y,length/item.subtex->width,width*.40f/item.subtex->height,theta,0,.5f,opacity*.38f,.35f);
            if(f->actionMove==MOVE_FLAMETHROWER&&f->phase==COMBAT_ACTIVE){
                unsigned n;
                for(n=0;n<(v->quality?3u:1u);n++){
                    float t=((g->combat.frame*3+n*31)%96)/96.0f;
                    Effect(v,FX_EMBER+(g->combat.frame/3+n)%4,x+sx*t-sinf(t*17)*3*v->zoom,
                           y+sy*t+cosf(t*13)*3*v->zoom,8*v->zoom,8*v->zoom,theta,opacity*(1-t)*.65f);
                }
            }else if(v->quality&&f->actionMove==MOVE_HYDRO_PUMP&&f->phase==COMBAT_ACTIVE){
                float shift=((g->combat.frame*7+k*9)%18-9.0f)*v->zoom;
                Effect(v,FX_HYDRO+(g->combat.frame/3)%4,ex-sinf(theta)*shift,ey+cosf(theta)*shift,
                       22*v->zoom,24*v->zoom,theta+1.2f,opacity*.55f);
            }
        }
    }
}
static void Actor(ArenaView *v,const ArenaGame *g,unsigned actor,float alpha)
{
    const CombatFighter *f=&g->combat.fighters[actor];
    const ArenaSpecies *s=Game_ActorSpecies(g,actor);
    const ArenaSpriteFrame *p;
    C2D_SpriteSheet sheet;
    float x,y,scale,lift=0,opacity=1;unsigned tick=g->combat.frame;
    if(!s||(!f->hp&&g->faintTicks[actor]>=20))return;
    sheet=Pokemon(v,s->dex);if(!sheet)return;
    p=Pose(f,s);scale=s->scale*v->zoom;
    x=X(v,ActorX(f,alpha));y=Y(v,ActorY(f,alpha));
    if(!f->hp){float fade=g->faintTicks[actor]/20.0f;opacity=1-fade;scale*=1-fade*.45f;y+=fade*9;}
    else if(g->deployTicks[actor])scale*=1-g->deployTicks[actor]/24.0f;
    if(s->style==2)lift=3+sinf(tick*.10f);
    else if(s->style==0&&f->animation==COMBAT_WALK)lift=fabsf(sinf(f->animTick*.7f));
    if(f->dashTimer){
        unsigned i;
        for(i=2;i>0;i--){
            float dx=direction[f->dashDirection&7][0],dy=direction[f->dashDirection&7][1];
            Sprite(sheet,p->image,x-p->originX*scale-dx*i*7*v->zoom,
                   y-p->originY*scale-dy*i*7*v->zoom-lift,scale,scale,0,0,0,.11f*(3-i),0);
        }
    }
    if(f->hp&&f->phase!=COMBAT_READY){
        const struct ArenaMoveProfile *profile=ArenaMoves_Get(f->actionMove);
        if(profile&&profile->kind==ARENA_MOVE_CONE&&ArenaMoves_Beam(f->actionMove))Beam(v,g,actor,alpha);
    }
    Sprite(sheet,p->image,x-p->originX*scale,y-p->originY*scale-lift,
           scale,scale,0,0,0,opacity,f->hurtTimer?.45f:0);
    if(actor==g->combat.controlActor)Text(v,x-3,y-p->height*scale-12,RGB(255,226,56),"V");
    if(f->guardTimer){
        unsigned cel=f->guardAge<8?f->guardAge/2:4;
        float centerY=y+(p->height*.5f-p->originY)*scale-lift;
        Effect(v,FX_GUARD+cel,x,centerY,p->width*scale+16,p->height*scale+12,0,.80f);
    }
    if(f->phase==COMBAT_ACTIVE){
        if(f->actionMove==MOVE_SWORDS_DANCE){
            unsigned k,cel=(tick/3)&3;
            for(k=0;k<3;k++){
                float phase=tick*.1f+k*2*PI/3;
                Effect(v,FX_SWORDS+cel,x+cosf(phase)*22*v->zoom,y-20+sin(phase)*8,
                       9*v->zoom,36*v->zoom,0,1);
            }
        }
        if(f->actionMove==MOVE_THUNDER_WAVE)
            Effect(v,FX_BOLT+(tick/3)%4,x+direction[f->attackDirection&7][0]*40*v->zoom,
                   y-14+direction[f->attackDirection&7][1]*40*v->zoom,65*v->zoom,12*v->zoom,
                   atan2f(direction[f->attackDirection&7][1],direction[f->attackDirection&7][0]),.85f);
        if(f->actionMove==MOVE_DOUBLE_TEAM){
            Sprite(sheet,p->image,x-p->originX*scale-12,y-p->originY*scale,scale,scale,0,0,0,.25f,0);
            Sprite(sheet,p->image,x-p->originX*scale+12,y-p->originY*scale,scale,scale,0,0,0,.25f,0);
        }
    }
}
static void Battle(ArenaView *v,const ArenaGame *g,float alpha)
{
    const Combat *c=&g->combat;unsigned i,j,order[4]={0,1,2,3};
    C2D_DrawImageAt(C2D_SpriteSheetGetImage(v->arena,0),X(v,0),Y(v,0),0,0,v->zoom,v->zoom);
    for(i=0;i<ARENA_OBSTACLES;i++){
        unsigned pose=gArenaProps[i].broken?2:gArenaProps[i].hp<gArenaProps[i].maxHp?1:0;
        unsigned image=pose*5+gArenaProps[i].kind;
        float x=(gArenaObstacles[i].left+gArenaObstacles[i].right)*.5f,y=gArenaObstacles[i].bottom;
        if(image>=15)image=0;
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(v->props,image),X(v,x-16),Y(v,y-30),.1f,0,v->zoom,v->zoom);
    }
    for(i=0;i<c->fighterCount;i++)if(c->fighters[i].hp){
        float x=X(v,ActorX(&c->fighters[i],alpha)),y=Y(v,ActorY(&c->fighters[i],alpha));
        C2D_DrawEllipseSolid(x-14*v->zoom,y-4*v->zoom,.15f,28*v->zoom,7*v->zoom,C2D_Color32(20,25,35,95));
    }
    for(i=0;i<c->fighterCount;i++)for(j=i+1;j<c->fighterCount;j++)
        if(c->fighters[order[i]].y>c->fighters[order[j]].y){unsigned swap=order[i];order[i]=order[j];order[j]=swap;}
    for(i=0;i<c->fighterCount;i++)Actor(v,g,order[i],alpha);
    for(i=0;i<COMBAT_PROJECTILES;i++)if(c->projectiles[i].life){
        const CombatProjectile *p=&c->projectiles[i];
        float x=X(v,(p->previousX+(p->x-p->previousX)*alpha)/256.0f);
        float y=Y(v,(p->previousY+(p->y-p->previousY)*alpha)/256.0f-12);
        unsigned cel=(c->frame/3)&3,image=FX_AURA+cel;
        float width=25,height=25,angle=0;
        if(p->move==MOVE_THUNDERBOLT){image=FX_BOLT+cel;width=48;height=13;angle=atan2f(p->vy,p->vx);}
        else if(p->move==MOVE_WATER_PULSE){image=FX_PULSE+(cel&1);width=height=28;}
        Effect(v,image,x,y,width*v->zoom,height*v->zoom,angle,1);
    }
    for(i=0;i<COMBAT_EFFECTS;i++)if(c->effects[i].life){
        const CombatEffect *e=&c->effects[i];unsigned age=e->maxLife-e->life;
        float x=X(v,e->x/256.0f),y=Y(v,e->y/256.0f-12);
        if(e->kind==COMBAT_DASH_TRAIL){
            float theta=atan2f(direction[e->direction&7][1],direction[e->direction&7][0]);
            Effect(v,FX_WIND+(age/3)%2,x,y+8,46*v->zoom,10*v->zoom,theta,e->life/(float)e->maxLife*.6f);
        }else if((e->kind==COMBAT_CONTACT||e->kind==COMBAT_BLOCK)&&age<10){
            Effect(v,FX_IMPACT+(age/3)%4,x,y,27*v->zoom,27*v->zoom,0,1-age/12.0f);
            if(e->move==MOVE_FLAMETHROWER)
                Effect(v,FX_EMBER+(age/3)%4,x,y-4,24*v->zoom,30*v->zoom,0,1-age/12.0f);
            else if(e->move==MOVE_HYDRO_PUMP)
                Effect(v,FX_HYDRO+(age/3)%4,x,y,32*v->zoom,28*v->zoom,-.8f,1-age/12.0f);
            else if(e->move==MOVE_THUNDERBOLT)
                Effect(v,FX_BOLT+(age/3)%4,x,y,36*v->zoom,12*v->zoom,(age&1)?-.8f:.8f,1-age/12.0f);
        }
        else if(e->kind==COMBAT_CLASH&&age<12)
            Effect(v,FX_IMPACT+(age/3)%4,x,y,38*v->zoom,38*v->zoom,0,1);
        /* Windup/release bookkeeping never pretends to be a hit. */
    }
    for(i=0;i<COMBAT_BEAM_CLASHES;i++)if(c->beamClashes[i].phase){
        const CombatBeamClash *pair=&c->beamClashes[i];
        float size=pair->phase==COMBAT_CLASH_PRESSURE?28:40;
        Effect(v,FX_IMPACT+(c->frame/3)%4,X(v,pair->x/256.0f),Y(v,pair->y/256.0f-12),size,size,0,1);
    }
}
static void Reticle(ArenaView *v,const ArenaGame *g,float alpha)
{
    const GameAim *a=&g->aim;const CombatFighter *f=&g->combat.fighters[g->combat.controlActor];
    float x,y,radius=12;unsigned k;u32 color;
    if(!f->hp||g->winner||(Combat_IsCasting(f)&&Combat_MoveTargetsSelf(f->actionMove)))return;
    x=X(v,(float)a->x);y=Y(v,(float)a->y-12);
    if(a->mode==GAME_AIM_LOCK&&a->target<g->combat.fighterCount){
        const CombatFighter *target=&g->combat.fighters[a->target];
        const ArenaSpecies *species=Game_ActorSpecies(g,a->target);
        if(!target->hp)return;
        x=X(v,ActorX(target,alpha));y=Y(v,ActorY(target,alpha)-12);
        radius=15*v->zoom;
        if(species){
            const ArenaSpriteFrame *pose=Pose(target,species);float scale=species->scale*v->zoom;
            x=X(v,ActorX(target,alpha))+(pose->width*.5f-pose->originX)*scale;
            y=Y(v,ActorY(target,alpha))+(pose->height*.5f-pose->originY)*scale;
            if(species->style==2)y-=(3+sinf(g->combat.frame*.1f))*v->zoom;
            radius=Limit(fmaxf(pose->width,pose->height)*scale*.55f,12,29);
        }
    }
    color=a->blocked?RGB(249,101,85):a->valid?RGB(168,243,139):RGB(255,198,87);
    for(k=0;k<4;k++){
        float sx=k&1?1:-1,sy=k&2?1:-1;
        Line(x+sx*radius,y+sy*radius,x+sx*(radius-6),y+sy*radius,4,.66f,RGB(30,44,45));
        Line(x+sx*radius,y+sy*radius,x+sx*radius,y+sy*(radius-6),4,.66f,RGB(30,44,45));
        Line(x+sx*radius,y+sy*radius,x+sx*(radius-6),y+sy*radius,2,.67f,color);
        Line(x+sx*radius,y+sy*radius,x+sx*radius,y+sy*(radius-6),2,.67f,color);
    }
    if(a->mode!=GAME_AIM_LOCK){
        Line(x-3,y,x+3,y,1,.67f,color);
        Line(x,y-3,x,y+3,1,.67f,color);
    }
}
static void Bar(float x,float y,float width,unsigned value,unsigned maximum,u32 color)
{
    C2D_DrawRectSolid(x-1,y-1,.85f,width+2,7,RGB(38,46,42));
    C2D_DrawRectSolid(x,y,.86f,width,5,RGB(79,91,80));
    if(maximum&&value)C2D_DrawRectSolid(x,y,.87f,width*value/maximum,5,color);
}
static void TopHud(ArenaView *v,const ArenaGame *g,float fps,float cpu,float gpu)
{
    const CombatFighter *player=&g->combat.fighters[g->combat.controlActor];
    unsigned target=Combat_TargetFighter(&g->combat,g->combat.controlActor);
    const ArenaSpecies *s=Game_ActorSpecies(g,g->combat.controlActor);
    unsigned i;
    C2D_DrawRectSolid(0,0,.8f,400,30,RGB(42,51,58));
    C2D_DrawRectSolid(0,29,.81f,400,2,RGB(152,170,148));
    Text(v,8,4,RGB(245,246,227),"%s",s?s->name:"PLAYER");
    Bar(8,19,100,player->hp,player->maxHp,RGB(88,213,112));
    Text(v,115,17,RGB(241,241,216),"%u/%u",player->hp,player->maxHp);
    if(target<4){
        const CombatFighter *enemy=&g->combat.fighters[target];
        const ArenaSpecies *op=Game_ActorSpecies(g,target);
        Text(v,224,4,RGB(245,246,227),"%s",op?op->name:"OPPONENT");
        Bar(224,19,112,enemy->hp,enemy->maxHp,RGB(229,167,76));
        for(i=0;i<Game_Remaining(g,1);i++)
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(v->props,15),340+i*12,13,.9f,0,1,1);
    }
    C2D_DrawRectSolid(0,228,.8f,400,12,RGB(42,51,58));
    Text(v,5,230,RGB(232,235,221),"%s  %.0fFPS CPU%.1f GPU%.1f",g->doubles?"2V2":"1V1",fps,cpu,gpu);
}
static const char *Status(const CombatFighter *f)
{
    static const char *labels[]={"OK","BRN","PSN","TOX","PAR","SLP","FRZ"};
    return f->status<7?labels[f->status]:"?";
}
static void Bottom(ArenaView *v,const ArenaGame *g,int paused,int audio)
{
    const CombatFighter *f=&g->combat.fighters[g->combat.controlActor];unsigned i;
    const ArenaSpecies *s=Game_ActorSpecies(g,g->combat.controlActor);
    u32 ink=RGB(38,48,43);
    C2D_DrawRectSolid(0,0,0,320,240,RGB(205,213,187));
    C2D_DrawRectSolid(0,0,.1f,320,23,RGB(58,73,68));
    Text(v,9,7,RGB(246,245,223),"PLATINUM ARENA / NATIVE 3DS");
    Text(v,9,31,ink,"%s  LV50  %s",s?s->name:"PLAYER",Status(f));
    Text(v,9,44,ink,"STAMINA");Bar(64,46,93,f->stamina,100,RGB(72,160,201));
    Text(v,175,43,ink,"PARTY %u/4",Game_Remaining(g,0));
    Text(v,9,54,ink,Combat_IsCasting(f)?"CASTING: MOVEMENT LOCKED":"Y STRIKE  B DEFEND  L+A CYCLE");
    for(i=0;i<4;i++){
        int x,y,width,height;const CombatMoveInfo *move=Combat_GetMoveInfo(f->moves[i]);
        Game_MoveRect(i,&x,&y,&width,&height);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(v->ui,0),x,y,.2f,0,1,1);
        if(i==f->selected)C2D_DrawRectSolid(x+7,y+3,.25f,width-14,1,RGB(207,115,36));
        Text(v,x+9,y+5,ink,"%s",move?move->name:"EMPTY");
        Text(v,x+9,y+16,ink,"PP%u/%u",f->pp[i],move?move->pp:0);
        if(f->cooldown[i])Text(v,x+91,y+16,ink,"CD%u",(f->cooldown[i]+59)/60);
        else if(i==f->selected)Text(v,x+138,y+16,ink,"A");
        else if(i==f->specialSlot)Text(v,x+138,y+16,ink,"X");
    }
    {
        const GameAim *aim=&g->aim;float dx,dy;int16_t ax,ay;
        u32 marker=aim->blocked?RGB(221,95,72):aim->valid?RGB(66,143,111):RGB(207,139,57);
        C2D_DrawRectSolid(182,65,.2f,132,116,RGB(60,83,77));
        C2D_DrawRectSolid(185,68,.21f,126,110,RGB(181,201,177));
        Text(v,193,72,ink,"TOUCH AIM");
        Line(198,126,298,126,1,.23f,RGB(130,156,132));
        Line(248,91,248,161,1,.23f,RGB(130,156,132));
        if(g->combat.aimOverride){ax=g->combat.aimX;ay=g->combat.aimY;}
        else {ax=(int16_t)(direction[f->direction&7][0]*256);ay=(int16_t)(direction[f->direction&7][1]*256);}
        dx=ax/256.0f;dy=ay/256.0f;
        Line(248,126,248+dx*36,126+dy*36,2,.3f,marker);
        C2D_DrawRectSolid(245+dx*36,123+dy*36,.31f,6,6,marker);
        Text(v,197,164,ink,"%s",aim->mode==GAME_AIM_LOCK?"LOCK":aim->mode==GAME_AIM_FREE?"FREE":"FACING");
        Text(v,185,185,ink,"TARGET >");
    }
    for(i=0;i<3;i++)C2D_DrawImageAt(C2D_SpriteSheetGetImage(v->ui,1),8+i*104,198,.2f,0,1,1);
    Text(v,17,211,ink,"R SWITCH");Text(v,122,211,ink,"NEW %s",g->doubles?"1V1":"2V2");Text(v,233,211,ink,"PAUSE");

    if(!audio)Text(v,240,32,ink,"SFX OFF");
    if(paused||g->winner){
        C2D_DrawRectSolid(12,62,.97f,296,136,RGB(52,66,62));
        C2D_DrawRectSolid(16,66,.98f,288,128,RGB(239,237,201));
        v->textDepth=.99f;
        Text(v,86,94,ink,"%s",g->winner?(g->winner==1?"VICTORY":"DEFEAT"):"PAUSED");
        Text(v,36,116,ink,"START: %s",g->winner?"NEW BATTLE":"RESUME");
        Text(v,36,137,ink,"SELECT: LOCK  R+Y: TARGET");
        Text(v,36,158,ink,"START+SELECT: EXIT");
        Text(v,36,174,ink,"X: VFX %s",v->quality?"HIGH":"LOW");
        v->textDepth=.95f;
    }
}
ArenaView *View_Create(void)
{
    ArenaView *v=calloc(1,sizeof(*v));if(!v)return 0;
    v->top=C2D_CreateScreenTarget(GFX_TOP,GFX_LEFT);v->bottom=C2D_CreateScreenTarget(GFX_BOTTOM,GFX_LEFT);
    v->effects=Load(v,"romfs:/gfx/effects.t3x");v->arena=Load(v,"romfs:/gfx/arena.t3x");
    v->font=Load(v,"romfs:/gfx/font.t3x");v->props=Load(v,"romfs:/gfx/props.t3x");v->ui=Load(v,"romfs:/gfx/ui.t3x");
    if(!v->top||!v->bottom||!v->effects||!v->arena||!v->font||!v->props||!v->ui){View_Destroy(v);return 0;}
    v->zoom=1;v->textDepth=.95f;v->quality=1;return v;
}
void View_Destroy(ArenaView *v)
{
    unsigned i;if(!v)return;
    for(i=0;i<4;i++)if(v->pokemon[i])C2D_SpriteSheetFree(v->pokemon[i]);
    if(v->effects)C2D_SpriteSheetFree(v->effects);
    if(v->arena)C2D_SpriteSheetFree(v->arena);
    if(v->font)C2D_SpriteSheetFree(v->font);
    if(v->props)C2D_SpriteSheetFree(v->props);
    if(v->ui)C2D_SpriteSheetFree(v->ui);
    free(v);
}
void View_Draw(ArenaView *v,const ArenaGame *g,float alpha,int paused,float fps,float cpu,float gpu,int audio)
{
    unsigned i;
    /* Lazy loads occur at a species binding change, outside the GPU frame. */
    for(i=0;i<g->combat.fighterCount;i++)Pokemon(v,g->combat.fighters[i].species);
    Camera(v,g);
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(v->top,RGB(58,83,62));C2D_SceneBegin(v->top);
    Battle(v,g,alpha);Reticle(v,g,alpha);TopHud(v,g,fps,cpu,gpu);
    C2D_TargetClear(v->bottom,RGB(205,213,187));C2D_SceneBegin(v->bottom);Bottom(v,g,paused,audio);
    C3D_FrameEnd(0);
}
unsigned View_TextureLoads(const ArenaView *v){return v?v->loads:0;}
void View_SetQuality(ArenaView *v,unsigned high){if(v)v->quality=!!high;}
