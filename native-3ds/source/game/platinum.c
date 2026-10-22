#include "platinum.h"
#include "platinum_ids.h"
#include "assets3ds.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int Is(const char *a,const char *b){return !strcmp(a,b);}
const PtMap *Platinum_Map(unsigned header){unsigned i;for(i=0;i<gPtMapCount;i++)if(gPtMaps[i].header==header)return &gPtMaps[i];return NULL;}
unsigned Platinum_Value(const Platinum *p,unsigned key){if(key>=0x4000&&key<0x4000+PT_VARS)return p->vars[key-0x4000];if(key>=0x8000&&key<0x8000+PT_LOCAL_VARS)return p->locals[key-0x8000];return key;}
void Platinum_Set(Platinum *p,unsigned key,unsigned value){if(key>=0x4000&&key<0x4000+PT_VARS)p->vars[key-0x4000]=(uint16_t)value;else if(key>=0x8000&&key<0x8000+PT_LOCAL_VARS)p->locals[key-0x8000]=(uint16_t)value;else {p->blocked=1;p->fault="Invalid original variable address";}}
void Platinum_Flag(Platinum *p,unsigned flag,int value){if(flag<PT_FLAG_BYTES*8){if(value)p->flags[flag/8]|=1u<<(flag%8);else p->flags[flag/8]&=~(1u<<(flag%8));}else {p->blocked=1;p->fault="Original flag exceeds supported bank";}}
int Platinum_HasFlag(const Platinum *p,unsigned flag){return flag<PT_FLAG_BYTES*8&&!!(p->flags[flag/8]&(1u<<(flag%8)));}
static unsigned NextRandom(Platinum *p){p->random=p->random*1103515245u+24691u;return p->random>>16;}
void Platinum_New(Platinum *p)
{
 unsigned i;memset(p,0,sizeof(*p));p->active=1;p->intro=1;p->random=0x54a7d19;p->pc=PT_NO_PC;p->header=gPtMaps[0].header;p->x=4;p->z=6;
 for(i=0;i<gPtNewGameCount;i++){
  const PtInstruction *s=&gPtNewGame[i];
  if(Is(s->op,"SetFlag"))Platinum_Flag(p,s->arg[0],1);
  else if(Is(s->op,"SetVar"))Platinum_Set(p,s->arg[0],s->arg[1]);
  else if(Is(s->op,"InitSizeContestRecord"))Platinum_Set(p,PT_VAR_SIZE_CONTEST_RECORD,33280);
  else if(Is(s->op,"RandomizeJubilifeLottery")){
   unsigned lo=NextRandom(p),hi=NextRandom(p);
   /* Preserve the original setter's low-half-twice behaviour, including its bug. */
   Platinum_Set(p,PT_VAR_LOTTERY_TRAINER_ID_LOW_HALF,lo);Platinum_Set(p,PT_VAR_LOTTERY_TRAINER_ID_LOW_HALF,hi);
  }else if(Is(s->op,"InitDailyRandomLevel"))Platinum_Set(p,PT_VAR_DAILY_RANDOM_LEVEL,NextRandom(p)%98+2);
  else if(!Is(s->op,"End")){p->blocked=1;p->fault=s->op;p->faultPC=(int32_t)i;break;}
 }
}
static int Actor(const Platinum *p,unsigned id){
 const PtMap *m=Platinum_Map(p->header);
 if(id==255)return PT_ACTORS;
 if(id==242)return PT_ACTORS-1;
 if(id<PT_ACTORS&&m&&id<m->objectCount&&m->objects[id].gfx==gPtRivalGfx&&p->actors[PT_ACTORS-1].visible)return PT_ACTORS-1;
 return id<PT_ACTORS?(int)id:-1;
}
static void Position(PtActor *a,unsigned x,unsigned z,unsigned dir){memset(a,0,sizeof(*a));a->x=a->fromX=(int32_t)(x*16+8)*256;a->z=a->fromZ=(int32_t)(z*16+8)*256;a->dir=(uint8_t)dir;a->visible=1;a->pc=PT_NO_PC;}
int Platinum_RunEntry(Platinum *p,unsigned entry)
{
 const PtMap *m=Platinum_Map(p->header);if(!m||!entry||p->blocked)return 0;
 if(entry>m->entryCount){p->blocked=1;p->fault="Original shared script bank not yet supported";return 0;}
 p->bank=p->header;
 p->pc=m->entries[entry-1];p->depth=p->waiting=0;return 1;
}
int Platinum_Enter(Platinum *p,unsigned header,unsigned x,unsigned z,unsigned dir)
{
 const PtMap *m=Platinum_Map(header);unsigned i;int follow=0;
 for(i=0;i<PT_ACTORS;i++)if(p->partner&&p->actors[i].visible&&p->actors[i].movementType==PT_MOVEMENT_TYPE_FOLLOW_PLAYER)follow=1;
 if(!m||x<m->minX||z<m->minZ||x>=m->minX+m->width||z>=m->minZ+m->height){p->blocked=1;p->fault="Original destination not yet supported";return 0;}
 p->header=p->bank=(uint16_t)header;p->x=(uint16_t)x;p->z=(uint16_t)z;p->facing=(uint8_t)dir;p->pc=PT_NO_PC;p->waiting=p->locked=p->depth=0;p->text[0]=0;
 if(m->warpCount>32){p->blocked=1;p->fault="Original warp table exceeds native capacity";return 0;}
 memcpy(p->warps,m->warps,m->warpCount*sizeof(*m->warps));p->cameraActive=0;p->camera.pc=PT_NO_PC;p->deferredWarp=0;
 memset(p->actors,0,sizeof(p->actors));for(i=0;i<=PT_ACTORS;i++)p->actors[i].pc=PT_NO_PC;
 for(i=0;i<m->objectCount&&i<PT_ACTORS;i++){
  const PtObject *o=&m->objects[i];Position(&p->actors[i],o->x,o->z,o->dir);
  p->actors[i].visible=!o->flag||!Platinum_HasFlag(p,o->flag);
 }
 Position(&p->actors[PT_ACTORS],x,z,dir);
 if(follow){static const int dx[]={0,0,-1,1},dz[]={-1,1,0,0};Position(&p->actors[PT_ACTORS-1],x-dx[dir],z-dz[dir],dir);p->actors[PT_ACTORS-1].movementType=PT_MOVEMENT_TYPE_FOLLOW_PLAYER;}
 p->pendingTransition=2;return 1;
}
static void Fault(Platinum *p,const PtInstruction *s){p->blocked=1;p->fault=s->op;p->faultPC=(int32_t)p->pc-1;}
void Platinum_Text(Platinum *p,unsigned id)
{
 const char *s;char raw[4096];size_t at=0,i=0;unsigned kind,index;
 if(id>=gPtMessageCount){p->blocked=1;p->fault="Invalid original message ID";return;}
 s=gPtMessages[id];p->message=(uint16_t)id;p->messagePage=0;
 while(*s&&at+2<sizeof(raw)){
  if(!strncmp(s,"{STRVAR_1 ",10)&&sscanf(s,"{STRVAR_1 %u, %u",&kind,&index)==2){
   const char *end=strchr(s,'}');if(!end){p->blocked=1;p->fault="Malformed original text control";return;}
   if(index<8){size_t n=strlen(p->buffers[index]);if(n>sizeof(raw)-at-1)n=sizeof(raw)-at-1;memcpy(raw+at,p->buffers[index],n);at+=n;}s=end+1;continue;
  }
  {char ch=*s++;raw[at++]=(ch=='\r'||ch=='\f')?'\n':ch;}
 }
 raw[at]=0;at=0;memset(p->text,0,sizeof(p->text));
 /* Keep the original case, Unicode glyphs, authored line/paragraph breaks.
  * The original message window shows two lines per acknowledgement. */
 while(raw[i]&&at+2<sizeof(p->text))p->text[at++]=raw[i++];
 p->waiting=1;
}
void Platinum_Advance(Platinum *p)
{
 unsigned lines=0;const char *s=p->text;unsigned skip=p->messagePage*2;
 while(*s&&skip){if(*s++=='\n')skip--;}
 while(*s&&lines<2)if(*s++=='\n')lines++;
 if(*s)p->messagePage++;else {p->waiting=0;p->text[0]=0;}
}
static int AudioOp(const char *s){return Is(s,"PlaySE")||Is(s,"WaitSE")||Is(s,"PlayFanfare")||Is(s,"WaitFanfare")||Is(s,"PlayDefaultMusic")||Is(s,"SetInitialVolumeForSequence")||Is(s,"Common_SetRivalBGM")||Is(s,"Common_FadeToDefaultMusic2")||Is(s,"Common_SetCounterpartBGM")||Is(s,"Common_FadeToDefaultMusic")||Is(s,"PlayMusic")||Is(s,"PlayCry")||Is(s,"WaitCry");}
static int Jump(Platinum *p,const PtMap *m,int target,int call){if(target<0||(unsigned)target>=m->codeCount)return 0;if(call){if(p->depth==PT_STACK)return 0;p->stack[p->depth++]=p->pc;}p->pc=(uint32_t)target;return 1;}
static void Run(Platinum *p)
{
 const PtMap *m=Platinum_Map(p->bank);unsigned budget=0;
 if(!m)return;
 while(p->pc!=PT_NO_PC&&!p->waiting&&!p->blocked&&budget++<96){
  const PtInstruction *s;const char *o;int a,b,c,actor;unsigned va,vb;int yes=0,call=0,conditional=0;
  if(p->pc>=m->codeCount){p->blocked=1;p->fault="Original script PC outside bank";break;}
  s=&m->code[p->pc++];o=s->op;a=s->arg[0];b=s->arg[1];c=s->arg[2];va=Platinum_Value(p,(unsigned)a);vb=Platinum_Value(p,(unsigned)b);
  if(AudioOp(o))continue; /* Sound service pending; these never change story flags. */
  if(s->unresolved){Fault(p,s);break;}
  if(Is(o,"End")){p->pc=PT_NO_PC;continue;}
  if(Is(o,"Return")){if(!p->depth){Fault(p,s);break;}p->pc=p->stack[--p->depth];continue;}
  if(Is(o,"LockAll")){p->locked=1;continue;}if(Is(o,"ReleaseAll")){p->locked=0;continue;}
  if(Is(o,"BufferRivalStarterSpeciesName")||Is(o,"BufferPlayerStarterSpeciesName")){
   unsigned dex=p->starter;const ArenaSpecies *species;
   if(Is(o,"BufferRivalStarterSpeciesName"))dex=dex==387?390:dex==390?393:387;
   species=ArenaSpecies_Find(dex);if(a<0||a>=8||!species){Fault(p,s);continue;}snprintf(p->buffers[a],32,"%s",species->name);continue;
  }
  if(Is(o,"BufferCounterpartName")){if(a>=0&&a<8)snprintf(p->buffers[a],32,"%s",p->gender?"LUCAS":"DAWN");continue;}
  if(Is(o,"GiveRunningShoes")){p->runningShoes=1;continue;}
  if(Is(o,"GiveBag")){Platinum_Flag(p,PT_FLAG_BAG_ACQUIRED,1);continue;}
  if(Is(o,"GiveJournal")){Platinum_Flag(p,PT_FLAG_JOURNAL_ACQUIRED,1);continue;}
  if(Is(o,"GetPlayerGender")){Platinum_Set(p,(unsigned)a,p->gender);continue;}
  if(Is(o,"GetPlayerStarterSpecies")){Platinum_Set(p,(unsigned)a,p->starter);continue;}
  if(Is(o,"ShowYesNoMenu")){p->waiting=4;p->service=1;p->waitVar=(uint16_t)a;p->choice=0;continue;}
  if(Is(o,"StartChooseStarterScene")){p->waiting=4;p->service=2;p->choice=0;continue;}
  if(Is(o,"SaveChosenStarter")||Is(o,"ReturnToField"))continue;
  if(Is(o,"GivePokemon")){p->waiting=4;p->service=3;p->waitVar=(uint16_t)s->arg[3];p->serviceArg[0]=(uint16_t)va;p->serviceArg[1]=(uint16_t)vb;continue;}
  if(Is(o,"StartFirstBattle")||Is(o,"StartTrainerBattle")){p->waiting=4;p->service=4;p->trainer=(uint16_t)va;continue;}
  if(Is(o,"CheckWonBattle")){Platinum_Set(p,(unsigned)a,p->lastBattleWon);continue;}
  if(Is(o,"HealParty")){p->waiting=4;p->service=5;continue;}
  if(Is(o,"SetHasPartner")||Is(o,"ClearHasPartner")){p->partner=(uint8_t)Is(o,"SetHasPartner");Platinum_Flag(p,PT_FLAG_HAS_PARTNER,p->partner);continue;}
  if(Is(o,"SetStepFlag")||Is(o,"ClearStepFlag")){Platinum_Flag(p,PT_FLAG_STEP,Is(o,"SetStepFlag"));continue;}
  if(Is(o,"SetObjectFlagIsPersistent")){actor=Actor(p,(unsigned)a);if(actor<0||!b){Fault(p,s);continue;}if(p->actors[actor].movementType!=PT_MOVEMENT_TYPE_FOLLOW_PLAYER){Fault(p,s);continue;}continue;}
  if(Is(o,"SetObjectEventMovementType")||Is(o,"SetMovementType")){
   actor=Actor(p,(unsigned)a);if(actor<0){Fault(p,s);continue;}p->actors[actor].movementType=(uint16_t)b;
   if(b>=PT_MOVEMENT_TYPE_LOOK_NORTH&&b<=PT_MOVEMENT_TYPE_LOOK_EAST)p->actors[actor].dir=(uint8_t)(b-PT_MOVEMENT_TYPE_LOOK_NORTH);
   continue;
  }
  if(Is(o,"AddFreeCamera")){Position(&p->camera,(unsigned)a,(unsigned)b,0);p->cameraActive=1;continue;}
  if(Is(o,"ApplyFreeCameraMovement")){if(a<0||(unsigned)a>=m->codeCount){Fault(p,s);continue;}p->camera.pc=(uint32_t)a;p->camera.ticks=0;continue;}
  if(Is(o,"RestoreCamera")){p->cameraActive=0;p->camera.pc=PT_NO_PC;continue;}
  if(Is(o,"SetWarpEventPos")){if(a<0||(unsigned)a>=m->warpCount){Fault(p,s);continue;}p->warps[a].x=(uint16_t)b;p->warps[a].z=(uint16_t)c;continue;}
  if(Is(o,"LockObject"))continue;
  if(Is(o,"LoadDoorAnimation")||Is(o,"PlayDoorOpenAnimation")||Is(o,"PlayDoorCloseAnimation")||Is(o,"UnloadAnimation"))continue;
  if(Is(o,"WaitForAnimation")||Is(o,"WaitFadeScreen")){p->waiting=3;p->waitTicks=12;p->waitVar=0x800c;continue;}
  if(Is(o,"FadeScreenOut")||Is(o,"FadeScreenIn"))continue;
  if(Is(o,"Warp")){
   uint32_t pc=p->pc,stack[PT_STACK];unsigned depth=p->depth,bank=p->bank;
   memcpy(stack,p->stack,sizeof(stack));
   if(!Platinum_Enter(p,(unsigned)a,vb,Platinum_Value(p,(unsigned)c),(unsigned)s->arg[3]))continue;
   p->pc=pc;p->bank=(uint16_t)bank;p->depth=(uint8_t)depth;memcpy(p->stack,stack,sizeof(stack));continue;
  }
  if(Is(o,"SetVar")){Platinum_Set(p,(unsigned)a,vb);continue;}
  if(Is(o,"AddVar")){Platinum_Set(p,(unsigned)a,va+vb);continue;}
  if(Is(o,"SetFlag")||Is(o,"ClearFlag")){Platinum_Flag(p,(unsigned)a,Is(o,"SetFlag"));continue;}
  if(Is(o,"GoToIfInRange")){if(va>=(unsigned)b&&va<=(unsigned)c&&!Jump(p,m,s->arg[3],0))Fault(p,s);continue;}
  if(Is(o,"GoTo")||Is(o,"Call")){if(!Jump(p,m,a,Is(o,"Call")))Fault(p,s);continue;}
  call=!strncmp(o,"CallIf",6);conditional=call||!strncmp(o,"GoToIf",6);
  if(conditional){
   const char *test=o+6;int target=c;
   if(Is(test,"Eq"))yes=va==vb;else if(Is(test,"Ne"))yes=va!=vb;else if(Is(test,"Ge"))yes=va>=vb;else if(Is(test,"Gt"))yes=va>vb;else if(Is(test,"Le"))yes=va<=vb;else if(Is(test,"Lt"))yes=va<vb;
   else if(Is(test,"Set")||Is(test,"Unset")){yes=Platinum_HasFlag(p,(unsigned)a)==Is(test,"Set");target=b;}
   else {Fault(p,s);continue;}
   if(yes&&!Jump(p,m,target,call)){Fault(p,s);}
   continue;
  }
  if(Is(o,"BufferPlayerName")||Is(o,"BufferRivalName")){/* Player buffer is set by the platform before calling Run. */
   if(a>=0&&a<8){snprintf(p->buffers[a],32,"%s",Is(o,"BufferPlayerName")?p->buffers[7]:"BARRY");}
   continue;
  }
  if(Is(o,"Message")||Is(o,"EventMessage")||Is(o,"NPCMessage")||Is(o,"ShowScrollingSign")||Is(o,"ShowArrowSign")){Platinum_Text(p,(unsigned)a);continue;}
  if(Is(o,"CloseMessage")||Is(o,"WaitButton"))continue; /* Message acknowledgement already yielded. */
  if(Is(o,"ApplyMovement")){
   actor=Actor(p,(unsigned)a);if(actor<0||b<0||(unsigned)b>=m->codeCount){Fault(p,s);continue;}
   p->actors[actor].pc=(uint32_t)b;p->actors[actor].ticks=0;continue;
  }
  if(Is(o,"WaitMovement")){p->waiting=2;continue;}
  if(Is(o,"WaitTime")){p->waitTicks=(uint8_t)(va>255?255:va);p->waitVar=(uint16_t)b;p->waiting=3;continue;}
  if(Is(o,"AddObject")||Is(o,"RemoveObject")){actor=Actor(p,(unsigned)a);if(actor<0){Fault(p,s);continue;}p->actors[actor].visible=Is(o,"AddObject");continue;}
  if(Is(o,"SetObjectEventPos")){actor=Actor(p,(unsigned)a);if(actor<0){Fault(p,s);continue;}Position(&p->actors[actor],(unsigned)b,(unsigned)c,p->actors[actor].dir);continue;}
  if(Is(o,"SetObjectEventDir")){actor=Actor(p,(unsigned)a);if(actor<0){Fault(p,s);continue;}p->actors[actor].dir=(uint8_t)b;continue;}
  if(Is(o,"FacePlayer"))continue; /* Presentation facing service is tracked separately. */
  if(Is(o,"GetPlayerMapPos")){Platinum_Set(p,(unsigned)a,p->x);Platinum_Set(p,(unsigned)b,p->z);continue;}
  if(Is(o,"GetPlayerDir")){Platinum_Set(p,(unsigned)a,p->facing);continue;}
  if(Is(o,"GetNationalDexEnabled")){Platinum_Set(p,(unsigned)a,0);continue;}
  Fault(p,s);
 }
 if(budget>96&&p->pc!=PT_NO_PC&&!p->waiting){p->blocked=1;p->fault="Original script exceeded instruction budget";}
}
static int Direction(const char *s){if(strstr(s,"North"))return 0;if(strstr(s,"South"))return 1;if(strstr(s,"West"))return 2;if(strstr(s,"East"))return 3;return -1;}
static void Motion(Platinum *p,PtActor *a)
{
 static const int dx[]={0,0,-1,1},dz[]={-1,1,0,0};const PtMap *m=Platinum_Map(p->bank);
 if(a->ticks){a->ticks--;if(!a->ticks){a->fromX=a->x;a->fromZ=a->z;}return;}
 if(a->pc!=PT_NO_PC&&m){
  const PtInstruction *s;if(a->pc>=m->codeCount){p->blocked=1;p->fault="Original movement PC out of range";return;}
  s=&m->code[a->pc++];
  if(Is(s->op,"LockDir")){a->lockDirection=1;return;}
  if(Is(s->op,"UnlockDir")){a->lockDirection=0;return;}
  if(Is(s->op,"SetInvisible")){a->visible=0;return;}
  if(Is(s->op,"SetVisible")){a->visible=1;return;}
  if(Is(s->op,"EndMovement")){a->pc=PT_NO_PC;return;}
  if(s->unresolved){Fault(p,s);return;}
  {int dir=Direction(s->op);unsigned times=s->argc?(unsigned)s->arg[0]:1;
   if(dir>=0){if(!a->lockDirection||!strncmp(s->op,"Face",4)||strstr(s->op,"OnSpot"))a->dir=(uint8_t)dir;
    if(!strncmp(s->op,"Walk",4)&&!strstr(s->op,"OnSpot")){
     a->fromX=a->x;a->fromZ=a->z;a->x+=dx[dir]*(int)times*16*256;a->z+=dz[dir]*(int)times*16*256;
    }
    a->duration=a->ticks=(uint16_t)(times*(strstr(s->op,"Fast")?8:12));
   }else if(!strncmp(s->op,"Delay",5)){a->duration=a->ticks=(uint16_t)(atoi(s->op+5)*times);}
   else if(Is(s->op,"EmoteExclamationMark")){a->duration=a->ticks=8;}
   else {Fault(p,s);return;}
  }
 }
}
static int WarpAtFeet(Platinum *p)
{
 const PtMap *m=Platinum_Map(p->header);unsigned i;if(!m)return 0;
 for(i=0;i<m->warpCount;i++)if(p->warps[i].x==p->x&&p->warps[i].z==p->z){
  const PtWarp *w=&p->warps[i];const PtMap *dest=Platinum_Map(w->dest);
  if(!dest||w->warp>=dest->warpCount){p->blocked=1;p->fault="Original warp destination not yet supported";return 1;}
  {const PtWarp *d=&dest->warps[w->warp];Platinum_Enter(p,w->dest,d->x,d->z,p->facing);}return 1;
 }
 return 0;
}
static void Trigger(Platinum *p)
{
 const PtMap *m=Platinum_Map(p->header);unsigned i;if(!m||p->pc!=PT_NO_PC||p->blocked)return;
 /* Coordinate cutscenes get their original opportunity before leaving a map,
  * including Mom's grass warning on the player's house exit tile. */
 for(i=0;i<m->triggerCount;i++){
  const PtTrigger *e=&m->triggers[i];if(p->x>=e->x&&p->x<e->x+e->width&&p->z>=e->z&&p->z<e->z+e->length&&Platinum_Value(p,e->var)==e->value){p->deferredWarp=1;Platinum_RunEntry(p,e->script);return;}
 }
 WarpAtFeet(p);
}
void Platinum_Tick(Platinum *p)
{
 const PtMap *m=Platinum_Map(p->header);unsigned i,moving=0;PtActor *player=&p->actors[PT_ACTORS];int completed=player->ticks==1&&player->pc==PT_NO_PC;
 if(!p->active||p->intro||p->blocked)return;
 for(i=0;i<=PT_ACTORS;i++){Motion(p,&p->actors[i]);if(p->actors[i].pc!=PT_NO_PC||p->actors[i].ticks)moving=1;}
 if(p->cameraActive){Motion(p,&p->camera);if(p->camera.pc!=PT_NO_PC||p->camera.ticks)moving=1;}
 p->x=(uint16_t)(player->x/4096);p->z=(uint16_t)(player->z/4096);p->facing=player->dir;
 if(p->waiting==2&&!moving)p->waiting=0;
 if(p->waiting==3&&p->waitTicks&&!--p->waitTicks){p->waiting=0;Platinum_Set(p,p->waitVar,0);}
 if(completed)Trigger(p);
 m=Platinum_Map(p->header);
 if(p->pc==PT_NO_PC&&p->pendingTransition){unsigned stage=p->pendingTransition--;p->bank=p->header;Platinum_RunEntry(p,m?(stage==2?m->onload:m->transition):0);}
 if(p->pc==PT_NO_PC&&!p->pendingTransition&&!p->waiting&&m)for(i=0;i<m->frameCount;i++)if(Platinum_Value(p,m->frames[i].var)==m->frames[i].value){Platinum_RunEntry(p,m->frames[i].script);break;}
 Run(p);
 if(p->pc==PT_NO_PC&&!p->waiting&&p->deferredWarp){p->deferredWarp=0;WarpAtFeet(p);}
}
int Platinum_Move(Platinum *p,unsigned dir)
{
 static const int dx[]={0,0,-1,1},dz[]={-1,1,0,0};const PtMap *m=Platinum_Map(p->header);PtActor *a=&p->actors[PT_ACTORS];int x,z;unsigned i;
 if(!m||dir>3||p->blocked||p->locked||p->waiting||p->pendingTransition||p->pc!=PT_NO_PC||a->ticks)return 0;
 a->dir=p->facing=(uint8_t)dir;x=p->x+dx[dir];z=p->z+dz[dir];
 if(x<m->minX||z<m->minZ||x>=m->minX+m->width||z>=m->minZ+m->height){
  for(i=0;i<gPtMapCount;i++){const PtMap *n=&gPtMaps[i];if(n->matrix==m->matrix&&x>=n->minX&&z>=n->minZ&&x<n->minX+n->width&&z<n->minZ+n->height&&!(n->terrain[(z-n->minZ)*n->width+x-n->minX]&0x8000))return Platinum_Enter(p,n->header,(unsigned)x,(unsigned)z,dir);}
  return 0;
 }
 if(m->terrain[(z-m->minZ)*m->width+x-m->minX]&0x8000){
  int warp=0;for(i=0;i<m->warpCount;i++)if(p->warps[i].x==x&&p->warps[i].z==z)warp=1;
  if(!warp)return 0;
 }
 for(i=0;i<m->objectCount&&i<PT_ACTORS;i++)if(p->actors[i].visible&&p->actors[i].x/4096==x&&p->actors[i].z/4096==z)return 0;
 if(p->partner)for(i=0;i<PT_ACTORS;i++)if(p->actors[i].visible&&p->actors[i].movementType==PT_MOVEMENT_TYPE_FOLLOW_PLAYER){PtActor *f=&p->actors[i];f->fromX=f->x;f->fromZ=f->z;f->x=a->x;f->z=a->z;f->dir=dir;f->duration=f->ticks=(uint16_t)(p->runningShoes&&p->runHeld?4:8);}
 a->fromX=a->x;a->fromZ=a->z;a->x=(x*16+8)*256;a->z=(z*16+8)*256;a->duration=a->ticks=(uint16_t)(p->runningShoes&&p->runHeld?4:8);return 1;
}
void Platinum_Interact(Platinum *p)
{
 static const int dx[]={0,0,-1,1},dz[]={-1,1,0,0};const PtMap *m=Platinum_Map(p->header);unsigned i;int x=p->x+dx[p->facing&3],z=p->z+dz[p->facing&3];
 if(!m||p->locked||p->waiting||p->pc!=PT_NO_PC||p->blocked)return;
 for(i=0;i<m->objectCount&&i<PT_ACTORS;i++)if(p->actors[i].visible&&p->actors[i].x/4096==x&&p->actors[i].z/4096==z){Platinum_RunEntry(p,m->objects[i].script);return;}
 for(i=0;i<m->backgroundCount;i++)if(m->backgrounds[i].x==x&&m->backgrounds[i].z==z){Platinum_RunEntry(p,m->backgrounds[i].script);return;}
}
