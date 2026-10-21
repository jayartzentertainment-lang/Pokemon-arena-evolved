/* Shared Citro2D/font/panel presentation for the connected adventure. */
static void Panel(ArenaView *v,float x,float y,float w,float h,int selected)
{
    C2D_DrawRectSolid(x-2,y-2,.72f,w+4,h+4,selected?RGB(226,178,72):RGB(43,64,59));
    {
        unsigned row,col;
        for(row=0;row<3;row++)for(col=0;col<3;col++){
            float dx=col==0?0:col==1?4:w-4,dy=row==0?0:row==1?4:h-4;
            float ww=col==1?w-8:4,hh=row==1?h-8:4;
            if(row==1&&col==1)C2D_DrawRectSolid(x+dx,y+dy,.73f,ww,hh,RGB(232,236,213));
            else C2D_DrawImageAt(C2D_SpriteSheetGetImage(v->window,row*3+col),x+dx,y+dy,.73f,0,ww/8,hh/8);
        }
    }
}
static void Header(ArenaView *v,const char *title)
{
    C2D_DrawRectSolid(0,0,.80f,320,25,RGB(39,62,59));
    C2D_DrawRectSolid(0,24,.81f,320,2,RGB(225,179,79));
    Text(v,10,7,RGB(245,239,210),"%.37s",title);
}
static void Big(ArenaView *v,int x,int y,const char *text)
{
    unsigned i;C2D_ImageTint tint;C2D_PlainImageTint(&tint,RGB(244,211,115),1);
    for(i=0;text[i];i++){
        unsigned ch=(unsigned char)toupper((unsigned char)text[i]);
        if(ch>31&&ch<128)C2D_DrawImageAt(C2D_SpriteSheetGetImage(v->font,ch-32),x+i*16,y,.95f,&tint,2,2);
    }
}
static void Trainer(ArenaView *v,const AdventureAppearance *p,int direction,int frame,float x,float y,float scale)
{
    unsigned g;
    C2D_DrawEllipseSolid(x-10*scale,y-3*scale,.15f,20*scale,5*scale,C2D_Color32(18,43,34,95));
    for(g=0;g<6;g++){
        C2D_SpriteSheet sheet=g==3?v->hair[p->body]:v->trainer[g];
        unsigned idx=(g==3?p->option[g]:p->option[g]*2+p->body)*16+(direction&3)*4+(frame&3);
        if(sheet&&idx<C2D_SpriteSheetCount(sheet))C2D_DrawImageAt(C2D_SpriteSheetGetImage(sheet,idx),x-16*scale,y-44*scale,scale>1.3f?.90f:.4f,0,scale,scale);
    }
}
static void SmallMon(ArenaView *v,unsigned dex,unsigned dir,unsigned anim,unsigned tick,float x,float y,float scale)
{
    const ArenaSpecies *s=ArenaSpecies_Find(dex);const ArenaSpriteAnimation *a;const ArenaSpriteFrame *p;
    if(!s)return;
    a=&s->animation[anim];p=&a->frames[(dir&7)*a->count+AnimFrame(a,tick)];
    C2D_DrawEllipseSolid(x-11*scale,y-3*scale,.16f,22*scale,6*scale,C2D_Color32(14,32,30,80));
    Sprite(Pokemon(v,dex),p->image,x-p->originX*scale,y-p->originY*scale,scale,scale,0,0,0,1,0);
}
static void StoryMap(ArenaView *v,const Adventure *a,float alpha)
{
    const AdventureSave *s=&a->save;float px=(a->previousX+(s->x-a->previousX)*alpha)/256.0f;
    float py=(a->previousY+(s->y-a->previousY)*alpha)/256.0f;
    float cx=Limit(px-200,0,112),cy=Limit(py-118,0,144);unsigned i;
    int event=s->step<ADVENTURE_EVENTS&&gAdventureEvents[s->step].map==s->map;
    const AdventureMap *m=&gAdventureMaps[s->map];
    if(v->mapSheet)C2D_DrawImageAt(C2D_SpriteSheetGetImage(v->mapSheet,0),-cx,-cy,0,0,1,1);
    if(s->appearance.follower&&s->partyCount&&s->party[0].hp){
        static const uint8_t facing[4]={0,6,2,4};
        SmallMon(v,s->party[0].dex,facing[a->facing],a->moving?COMBAT_WALK:COMBAT_IDLE,
                 a->moving?a->ticks:0,a->followerX/256.0f-cx,a->followerY/256.0f-cy,.9f);
    }
    if(event){
        unsigned npc=gAdventureEvents[s->step].npc;
        Sprite(v->npcs,npc,256-cx-16,152-cy-29,1,1,0,0,0,1,0);
        Text(v,252-cx,106-cy+sin(a->ticks*.09f)*2,RGB(253,217,83),"!");
    }
    {
        unsigned n;for(n=0;n<gNpcCount;n++)if(!strcmp(gNpcNames[n],"nurse"))break;
        Sprite(v->npcs,n,368-cx-16,158-cy-29,1,1,0,0,0,1,0);
    }
    Trainer(v,&s->appearance,a->facing,a->walkFrame,px-cx,py-cy,1);
    for(i=0;i<m->linkCount;i++){
        static const int positions[4][2]={{252,22},{488,201},{252,362},{12,201}};
        static const char *arrows[4]={"^",">","V","<"};
        u32 col=m->links[i]<ADVENTURE_MAPS&&gAdventureMaps[m->links[i]].requires<=s->step?RGB(255,240,153):RGB(183,137,120);
        Text(v,positions[i][0]-cx,positions[i][1]-cy,col,"%s",arrows[i]);
    }
    C2D_DrawRectSolid(0,0,.8f,400,24,RGB(39,62,59));
    Text(v,10,7,RGB(246,242,216),"%s",m->name);
    if(a->mode==ADV_DIALOGUE){
        unsigned at=a->dialoguePage*180,k;char line[46];
        Panel(v,8,148,384,84,0);v->textDepth=.96f;
        for(k=0;k<4;k++){
            unsigned take=0;while(take<45&&a->dialogue[at])line[take++]=a->dialogue[at++];line[take]=0;
            Text(v,18,156+k*16,RGB(36,52,46),"%s",line);
        }
        Text(v,359,218,RGB(68,86,57),"A>");
    }
    if(a->mode==ADV_ENCOUNTER){
        C2D_DrawRectSolid(0,72,.8f,400,96,C2D_Color32(26,42,41,240));
        Big(v,52,84,"BATTLE START");
        Text(v,38,117,RGB(246,237,204),"%s",a->battleKind==ADV_WILD_BATTLE?"A WILD POKEMON APPEARED!":gAdventureEvents[a->battleEvent].name);
        Text(v,38,140,RGB(246,237,204),"PREPARE YOUR PARTNER...");
    }
}
static void TrainerCard(ArenaView *v,const Adventure *a)
{
    const AdventureSave *s=&a->save;unsigned i,n=0;
    Panel(v,20,35,360,170,0);
    Trainer(v,&s->appearance,a->previewDirection,0,103,170,2.4f);
    Text(v,184,55,RGB(35,52,46),"%s",s->name[0]?s->name:"TRAINER");
    Text(v,184,78,RGB(35,52,46),"YOUR OWN JOURNEY");
    Text(v,184,103,RGB(35,52,46),"SEEN %u",Adventure_DexCount(s->seen));
    Text(v,184,122,RGB(35,52,46),"CAUGHT %u",Adventure_DexCount(s->caught));
    for(i=0;i<8;i++)if(s->badges&(1u<<i))n++;
    Text(v,184,146,RGB(35,52,46),"BADGES %u/8",n);
    Text(v,184,171,RGB(35,52,46),"$%u",s->money);
}
static void Creator(ArenaView *v,const Adventure *a)
{
    static const char *rows[]={"NAME","BODY","SKIN","HAIR","TOP","BOTTOM","HAT","ACCESSORY","FOLLOWER"};
    static const unsigned group[9]={0,0,0,3,2,1,4,5,0};
    unsigned i;char value[24];Header(v,a->returnMode==ADV_CAMP?"EDIT YOUR TRAINER":"CREATE YOUR TRAINER");
    for(i=0;i<9;i++){
        unsigned g=group[i];
        if(i==0)snprintf(value,sizeof(value),"%s",a->save.name);
        else if(i==1)snprintf(value,sizeof(value),"BODY %c",a->save.appearance.body?'B':'A');
        else if(i==8)snprintf(value,sizeof(value),"%s",a->save.appearance.follower?"ON":"OFF");
        else snprintf(value,sizeof(value),"%s",gAppearanceLabels[g][a->save.appearance.option[g]]);
        if(i==a->cursor)C2D_DrawRectSolid(8,31+i*18,.7f,304,18,RGB(223,194,111));
        Text(v,13,35+i*18,RGB(34,52,45),"%s",rows[i]);
        Text(v,137,35+i*18,RGB(34,52,45),"< %.18s >",value);
    }
    Panel(v,12,204,296,29,1);Text(v,46,214,RGB(34,52,45),"START: CONFIRM TRAINER");
}
static void BottomAdventure(ArenaView *v,const Adventure *a)
{
    const AdventureSave *s=&a->save;unsigned i;u32 ink=RGB(34,52,45);
    C2D_DrawRectSolid(0,0,0,320,240,RGB(192,208,189));
    if(a->mode==ADV_TITLE){
        static const char *items[]={"CONTINUE ADVENTURE","NEW ADVENTURE","BATTLE LAB /493 POKEMON","ABOUT THIS BUILD"};
        Header(v,"PLATINUM ARENA /0.3.0");
        for(i=0;i<4;i++){Panel(v,40,60+i*40,240,30,a->cursor==i);Text(v,50,70+i*40,ink,"%s",items[i]);}
        if(a->messageTicks)Text(v,7,30,ink,"%.38s",a->message);
        else if(a->loadError)Text(v,7,30,ink,"SAVE DAMAGED: BACKUP OR NEW GAME");
        return;
    }
    if(a->mode==ADV_CREATOR){Creator(v,a);return;}
    if(a->mode==ADV_NAME){static const char keys[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 -";char key[4];Header(v,"NAME / B:DELETE / START:DONE");Panel(v,20,34,280,23,0);Text(v,40,42,ink,"%s_",s->name);for(i=0;i<40;i++){if(i==a->keyboard)C2D_DrawRectSolid(10+i%10*30,66+i/10*36,.7f,29,30,RGB(232,190,91));snprintf(key,sizeof(key),"%c",i<38?keys[i]:' ');if(i>=38)snprintf(key,sizeof(key),"%s",i==38?"DEL":"OK");Text(v,12+i%10*30,77+i/10*36,ink,"%s",key);}return;}
    if(a->mode==ADV_STARTER){static const char *names[]={"TURTWIG","CHIMCHAR","PIPLUP"};Header(v,"PROFESSOR ROWAN / CHOOSE PARTNER");for(i=0;i<3;i++){Panel(v,5+i*106,60,100,118,a->cursor==i);Text(v,10+i*106,85,ink,"%s",names[i]);Text(v,10+i*106,119,ink,"LEVEL5");Text(v,10+i*106,148,ink,"A:CHOOSE");}Text(v,19,204,ink,"LEFT/RIGHT: CHOOSE  A:CONFIRM");return;}
    if(a->mode==ADV_LAB){const ArenaSpecies *left=ArenaSpecies_Find(a->labDex[0]),*right=ArenaSpecies_Find(a->labDex[1]);Header(v,"BATTLE LAB / ALL493 BASE SPECIES");Panel(v,12,64,296,42,!a->labSide);Panel(v,12,114,296,42,a->labSide);Text(v,23,79,ink,"PLAYER <%03u %.16s>",a->labDex[0],left->name);Text(v,23,129,ink,"ENEMY  <%03u %.16s>",a->labDex[1],right->name);Text(v,12,35,ink,"Y:CLASH %s  L/R:+/-10",a->labClash?"ON":"OFF");Panel(v,12,178,140,32,0);Panel(v,168,178,140,32,0);Text(v,28,190,ink,"A:1V1");Text(v,185,190,ink,"X:2V2");Text(v,12,221,ink,"UP/DOWN: SIDE  B:TITLE");return;}
    if(a->mode==ADV_CAMP){static const char *items[]={"SAVE AND RETURN","PARTY / LEAD / POTIONS","SINNOH MAP / TRAVEL","EDIT YOUR TRAINER","POKEMON CENTER / HEAL","BUY5 BALLS / $100","TRAIN / WILD BATTLE","POKEMON PC","SAVE AND TITLE"};Header(v,"TRAINER MENU / A:CHOOSE B:BACK");for(i=0;i<9;i++){if(i==a->cursor)Panel(v,9,29+i*21,302,20,1);Text(v,17,35+i*21,ink,"%s",items[i]);}if(a->messageTicks)Text(v,6,225,ink,"%.38s",a->message);return;}
    if(a->mode==ADV_PARTY){Header(v,"PARTY / A:LEAD X:POTION B:BACK");for(i=0;i<s->partyCount;i++){const AdventureMon *m=&s->party[i];Panel(v,10,32+i*27,300,23,a->cursor==i);Text(v,20,36+i*27,ink,"%.12s LV%u %u/%u",ArenaSpecies_Find(m->dex)->name,m->level,m->hp,Adventure_Config(m).hp);Bar(20,51+i*27,270,m->hp,Adventure_Config(m).hp,RGB(62,169,106));}Text(v,12,205,ink,"POTIONS:%u   FOLLOWER:%s",s->potions,s->appearance.follower?"ON":"OFF");Text(v,12,223,ink,"A CHANGES YOUR BATTLE LEAD");return;}
    if(a->mode==ADV_PC){Header(v,"PC / A:WITHDRAW OR SWAP LEAD");for(i=0;i<8;i++){unsigned slot=a->cursor/8*8+i;if(slot>=s->boxCount)break;if(slot==a->cursor)Panel(v,9,35+i*20,302,19,1);Text(v,18,40+i*20,ink,"%02u %.15s LV%u",slot+1,ArenaSpecies_Find(s->box[slot].dex)->name,s->box[slot].level);}Text(v,12,210,ink,"Y:DEPOSIT L+X:RELEASE PC:%u/60",s->boxCount);return;}
    if(a->mode==ADV_MAP){unsigned first=a->cursor/9*9;Header(v,"SINNOH / A:VISITED TOWN B:BACK");for(i=0;i<9&&first+i<ADVENTURE_MAPS;i++){unsigned at=first+i;int visited=!!(s->visited&((uint64_t)1<<at));if(a->cursor==at)Panel(v,9,31+i*20,302,19,1);Text(v,17,36+i*20,ink,"%c %.29s",visited?'*':'-',gAdventureMaps[at].name);}Text(v,12,221,ink,"L/R:PAGES  * ALREADY VISITED");return;}
    Header(v,s->name);
    if(s->step<ADVENTURE_EVENTS){const AdventureEvent *e=&gAdventureEvents[s->step];Panel(v,10,35,300,54,0);Text(v,20,43,ink,"STORY %u/41: %s",s->step+1,e->name);Text(v,20,65,ink,"%.34s",gAdventureMaps[e->map].name);}else{Big(v,60,45,"CHAMPION");}
    {
        const AdventureMap *m=&gAdventureMaps[s->map];int next=Adventure_NextRoad(a);
        static const char *dirs[]={"NORTH","EAST","SOUTH","WEST"};
        for(i=0;i<m->linkCount;i++)Text(v,12,102+i*18,ink,"%c %-5s %.26s",(int)i==next?'*':' ',dirs[i],gAdventureMaps[m->links[i]].name);
    }
    Text(v,12,178,ink,"BALLS %u   $%u   DEX%u",s->balls,s->money,Adventure_DexCount(s->caught));
    for(i=0;i<3;i++)Panel(v,7+i*105,202,98,30,0);
    Text(v,18,212,ink,"A TALK");Text(v,124,212,ink,"Y PARTY");Text(v,227,212,ink,"MENU");
    if(a->messageTicks){C2D_DrawRectSolid(0,27,.98f,320,20,RGB(236,219,154));v->textDepth=.99f;Text(v,6,31,ink,"%.38s",a->message);v->textDepth=.95f;}
}
void View_DrawApp(ArenaView *v,const Adventure *a,float alpha,int paused,float fps,float cpu,float gpu,int audio)
{
    static const char *groups[]={"body","bottoms","tops","hair","headgear","accessories"};unsigned i;
    v->app=a;
    if(a->mode==ADV_BATTLE){View_Draw(v,&a->battle,alpha,paused,fps,cpu,gpu,audio);return;}
    /* All disk loading/eviction is before C3D_FrameBegin. The cache has8 slots. */
    for(i=0;i<6;i++)if(i!=3&&!v->trainer[i]){char path[80];snprintf(path,sizeof(path),"romfs:/gfx/trainer_%s.t3x",groups[i]);v->trainer[i]=Load(v,path);}
    if(!v->npcs)v->npcs=Load(v,"romfs:/gfx/story_npcs.t3x");
    if(!v->mapSheet||v->mapId!=a->save.map){char path[80];if(v->mapSheet){C3D_FrameSync();C2D_SpriteSheetFree(v->mapSheet);}snprintf(path,sizeof(path),"romfs:/gfx/map_%02u.t3x",a->save.map);v->mapSheet=Load(v,path);v->mapId=a->save.map;}
    if(a->mode==ADV_STARTER){Pokemon(v,387);Pokemon(v,390);Pokemon(v,393);}
    else if(a->mode==ADV_LAB){Pokemon(v,a->labDex[0]);Pokemon(v,a->labDex[1]);}
    else if(a->save.partyCount)Pokemon(v,a->save.party[a->cursor<a->save.partyCount&&a->mode==ADV_PARTY?a->cursor:0].dex);
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);C2D_TargetClear(v->top,RGB(55,82,66));C2D_SceneBegin(v->top);
    if(a->mode==ADV_FIELD||a->mode==ADV_DIALOGUE||a->mode==ADV_ENCOUNTER)StoryMap(v,a,alpha);
    else if(a->mode==ADV_TITLE){
        if(v->mapSheet)C2D_DrawImageAt(C2D_SpriteSheetGetImage(v->mapSheet,0),-56,-70,0,0,1,1);
        C2D_DrawRectSolid(0,0,.8f,400,240,C2D_Color32(21,47,43,220));
        Big(v,54,39,"PLATINUM ARENA");Text(v,76,82,RGB(242,234,201),"EVOLVED / NATIVE NINTENDO3DS");
        Trainer(v,&a->save.appearance,0,0,200,181,2);
        Text(v,60,208,RGB(242,234,201),"A CONDENSED SINNOH ADVENTURE");
    }else if(a->mode==ADV_STARTER){
        static const unsigned dex[]={387,390,393};Big(v,40,25,"CHOOSE YOUR PARTNER");
        for(i=0;i<3;i++){C2D_DrawEllipseSolid(26+i*132,160,.1f,90,30,RGB(165,195,144));SmallMon(v,dex[i],0,0,a->ticks,70+i*132,177,1.7f);}
    }else if(a->mode==ADV_LAB){Big(v,80,20,"BATTLE LAB");SmallMon(v,a->labDex[0],2,0,a->ticks,105,179,1.8f);SmallMon(v,a->labDex[1],6,0,a->ticks,295,179,1.8f);Text(v,63,208,RGB(242,234,201),"AUTHENTIC DIRECTIONAL SPRITES");}
    else if(a->mode==ADV_PARTY&&a->save.partyCount){const AdventureMon *m=&a->save.party[a->cursor];SmallMon(v,m->dex,0,0,a->ticks,100,168,2);Text(v,182,50,RGB(244,235,205),"%s LV%u",ArenaSpecies_Find(m->dex)->name,m->level);for(i=0;i<4;i++)Text(v,182,85+i*25,RGB(244,235,205),"%.20s",m->moves[i]?Combat_GetMoveInfo(m->moves[i])->name:"EMPTY");}
    else TrainerCard(v,a);
    C2D_TargetClear(v->bottom,RGB(192,208,189));C2D_SceneBegin(v->bottom);BottomAdventure(v,a);C3D_FrameEnd(0);
}
