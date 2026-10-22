/* Opt-in developer view. Every shape comes from the simulation's own query. */
static void DebugEllipse(ArenaView *v,float x,float y,float rx,float ry,u32 color)
{
    for(unsigned i=0;i<20;i++){
        float a=i*2*PI/20,b=(i+1)*2*PI/20;
        Line(x+cosf(a)*rx,y+sinf(a)*ry,x+cosf(b)*rx,y+sinf(b)*ry,1,.82f,color);
    }
}
static void DebugShape(ArenaView *v,const CombatShape *s,u32 color)
{
    float x0=X(v,s->x0/256.0f),y0=Y(v,s->y0/256.0f);
    float x1=X(v,s->x1/256.0f),y1=Y(v,s->y1/256.0f);
    if(!s->active)return;
    DebugEllipse(v,x0,y0,s->radiusX*v->zoom,s->radiusY*v->zoom,color);
    if(s->kind!=COMBAT_SHAPE_ELLIPSE){
        DebugEllipse(v,x1,y1,s->radiusX*v->zoom,s->radiusY*v->zoom,color);
        Line(x0,y0,x1,y1,1,.82f,color);
    }
}
static void CombatDebug(ArenaView *v,const ArenaGame *g)
{
    const Combat *c=&g->combat;
    if(!v->debug)return;
    for(unsigned i=0;i<c->fighterCount;i++){
        const CombatFighter *f=&c->fighters[i];const ArenaSpecies *species=Game_ActorSpecies(g,i);
        CombatShape body,attack;unsigned slot=f->animation<5?f->animation:0;
        float x=X(v,f->x/256.0f),y=Y(v,f->y/256.0f);
        if(!species)continue;
        Combat_Hurtbox(f,&body);Combat_AttackShape(f,&attack);
        DebugShape(v,&body,RGB(75,240,140));DebugShape(v,&attack,RGB(255,96,72));
        /* Foot registration, direction, resolved root displacement and velocity. */
        Line(x-3,y,x+3,y,1,.83f,RGB(255,230,70));Line(x,y-3,x,y+3,1,.83f,RGB(255,230,70));
        Line(X(v,f->previousX/256.0f),Y(v,f->previousY/256.0f),x,y,2,.83f,RGB(255,230,70));
        Line(x,y,x+direction[f->direction&7][0]*16*v->zoom,y+direction[f->direction&7][1]*16*v->zoom,1,.83f,RGB(115,180,255));
        if(f->motion.hover==2&&slot==COMBAT_WALK)slot=COMBAT_IDLE;
        {const ArenaSpriteAnimation *a=&species->animation[slot];unsigned tick;
         if(f->hurtTimer&&ArenaSpecies_Hurt(species->dex)){
             unsigned duration=f->reactionDuration?f->reactionDuration:16;
             a=ArenaSpecies_Hurt(species->dex);slot=5;
             tick=(f->hurtTimer<duration?duration-f->hurtTimer:0)*a->totalTicks/duration;
             if(tick>=a->totalTicks)tick=a->totalTicks-1;
         }else tick=Combat_AnimationTick(f,a->totalTicks,a->hitTick);
         Text(v,x-25,y+7,RGB(255,245,210),"%u D%u A%u F%u",f->species,f->direction,slot,AnimFrame(a,tick));}
    }
    for(unsigned i=0;i<COMBAT_PROJECTILES;i++)if(c->projectiles[i].life){
        const CombatProjectile *p=&c->projectiles[i];CombatShape trace={0};
        trace.x0=p->previousX;trace.y0=p->previousY;trace.x1=p->x;trace.y1=p->y;
        trace.radiusX=trace.radiusY=p->radius;trace.active=1;trace.kind=COMBAT_SHAPE_CAPSULE;
        DebugShape(v,&trace,RGB(255,115,235));
    }
    for(unsigned i=0;i<COMBAT_EFFECTS;i++)if(c->effects[i].life&&c->effects[i].bodySpace){
        const CombatEffect *e=&c->effects[i];float x=X(v,e->x/256.0f),y=Y(v,e->y/256.0f);
        Line(x-5,y,x+5,y,2,.84f,RGB(255,255,255));Line(x,y-5,x,y+5,2,.84f,RGB(255,255,255));
    }
    Text(v,6,222,RGB(255,230,80),"DEV: BODY GREEN / HIT RED / SHOT PINK");
}
