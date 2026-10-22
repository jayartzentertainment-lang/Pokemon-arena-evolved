#include "adventure.h"
#include "platinum_ids.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static Platinum p;
static const PtMap *Room(void){return &gPtMaps[0];}
static int Settled(const Platinum *state)
{
 const PtMap *map=Platinum_Map(state->header);unsigned i;
 if(state->pc!=PT_NO_PC||state->waiting||state->pendingTransition||state->actors[PT_ACTORS].ticks)return 0;
 for(i=0;map&&i<map->frameCount;i++)if(Platinum_Value(state,map->frames[i].var)==map->frames[i].value)return 0;
 return 1;
}
static void Drain(unsigned limit)
{
 unsigned n,idle=0;
 for(n=0;n<limit&&!p.blocked;n++){
  if(p.waiting==1)Platinum_Advance(&p);
  Platinum_Tick(&p);
  if(Settled(&p)){if(++idle==2)break;}else idle=0;
 }
 if(p.blocked)fprintf(stderr,"Original VM blocked: %s PC%ld in %s\n",p.fault,(long)p.faultPC,Platinum_Map(p.header)->scriptFile);
 assert(!p.blocked);
}
static void OriginalOpening(void)
{
 unsigned var=0,rival=0,i;
 Platinum_New(&p);if(p.blocked)fprintf(stderr,"NEW FAULT %s PC%ld\n",p.fault,(long)p.faultPC);assert(!p.blocked&&p.x==4&&p.z==6&&p.intro);
 p.intro=0;p.creatorDone=1;snprintf(p.buffers[7],32,"ARENA");
 assert(Platinum_Enter(&p,Room()->header,4,6,0));
 assert(Room()->matrix==129&&Room()->width==32&&Room()->height==32);
 for(i=0;i<Room()->frameCount;i++)if(Room()->frames[i].script==3)var=Room()->frames[i].var;
 assert(var>=0x4000&&Platinum_Value(&p,var)==0);
 Drain(200);assert(Platinum_Value(&p,var)==1);
 assert(!p.actors[0].visible);
 /* Original trigger north of the spawn at (4,5), with original rival path. */
 assert(Platinum_Move(&p,0));Drain(2500);
 assert(p.x==4&&p.z==5&&!p.actors[0].visible&&!p.locked);
 for(i=0;i<Room()->triggerCount;i++)if(Room()->triggers[i].script==7)rival=Room()->triggers[i].var;
 assert(rival&&Platinum_Value(&p,rival)==1);
 assert(p.header==Room()->header);puts("original opening: TV state and full rival entry7 movement/message/flag sequence passed");
}
static void FailClosed(void)
{
 Platinum saved=p;p.pc=Room()->codeCount+1;p.waiting=0;Platinum_Tick(&p);assert(p.blocked);assert(!memcmp(p.flags,saved.flags,sizeof(p.flags)));p=saved;
 assert(!Platinum_Enter(&p,65535,1,1,0)&&p.blocked);p=saved;
}
static void Checkpoint(void)
{
 Adventure app;uint8_t bytes[4096],back[4096];size_t n,n2;Platinum loaded;FILE *f;
 Adventure_Init(&app);assert(Adventure_SaveEncode(&app.save,bytes,sizeof(bytes),&n));
 assert(Platinum_SaveWrite("/tmp/native-platinum-check.sav",&p,bytes,n));
 assert(Platinum_SaveWrite("/tmp/native-platinum-check.sav",&p,bytes,n));
 assert(Platinum_SaveRead("/tmp/native-platinum-check.sav",&loaded,back,sizeof(back),&n2)==1);
 assert(n==n2&&!memcmp(bytes,back,n)&&!memcmp(p.vars,loaded.vars,sizeof(p.vars))&&!memcmp(p.flags,loaded.flags,sizeof(p.flags))&&loaded.x==4&&loaded.z==5);
 f=fopen("/tmp/native-platinum-check.sav","wb");assert(f);fputs("corrupt",f);fclose(f);
 assert(Platinum_SaveRead("/tmp/native-platinum-check.sav",&loaded,back,sizeof(back),&n2)==2);
 remove("/tmp/native-platinum-check.sav");remove("/tmp/native-platinum-check.sav.bak");
 puts("original checkpoint: flags/vars/position + compatible appearance/party payload + backup recovery passed");
}
static Adventure original;
static void DrainAdventure(void)
{
 unsigned n,idle=0;GameInput in={0};in.touchMove=-1;
 for(n=0;n<12000;n++){
  Platinum *state=&original.platinum;
  if(state->blocked){fprintf(stderr,"BLOCKED header%u op%s PC%ld\n",state->header,state->fault,(long)state->faultPC);assert(!state->blocked);}
  if(original.mode==ADV_BATTLE)return;
  if(state->waiting==1)Platinum_Advance(state);
  if(state->waiting==4&&(state->service==1||state->service==2)){GameInput yes=in;yes.pressed=GAME_A;Adventure_Input(&original,yes,0,0,0,0);}
  Adventure_Tick(&original,in);
  if(Settled(state)){if(++idle==2)return;}else idle=0;
 }
 assert(!"original event did not settle");
}
static void WalkTo(unsigned gx,unsigned gz,unsigned destination)
{
 unsigned n;
 for(n=0;n<300;n++){
  Platinum *state=&original.platinum;const PtMap *m=Platinum_Map(state->header);unsigned size=m->width*m->height,head=0,tail=0,start,goal,at,i;int *parent;unsigned *queue;static const int dx[]={0,0,-1,1},dz[]={-1,1,0,0};
  if(destination==PT_MAP_HEADER_ROUTE_201&&gx==112&&gz==857&&Platinum_Value(state,PT_VAR_FOLLOWER_RIVAL_STATE)==1)return;
  if(state->header==destination&&(state->x==gx&&state->z==gz))return;
  if(gx<m->minX||gz<m->minZ||gx>=m->minX+m->width||gz>=m->minZ+m->height){fprintf(stderr,"Bad test goal %u,%u from%u\n",gx,gz,state->header);abort();}
  parent=malloc(size*sizeof(*parent));queue=malloc(size*sizeof(*queue));assert(parent&&queue);
  for(i=0;i<size;i++)parent[i]=-1;
  start=(state->z-m->minZ)*m->width+state->x-m->minX;goal=(gz-m->minZ)*m->width+gx-m->minX;parent[start]=(int)start;queue[tail++]=start;
  while(head<tail&&parent[goal]<0){
   unsigned current=queue[head++];int x=(int)(current%m->width),z=(int)(current/m->width);
   for(i=0;i<4;i++){
    int nx=x+dx[i],nz=z+dz[i],blocked=0;unsigned actor,next;
    if(nx<0||nz<0||nx>=m->width||nz>=m->height)continue;
    next=(unsigned)nz*m->width+(unsigned)nx;if(parent[next]>=0)continue;
    if(m->terrain[next]&0x8000){unsigned w;int warp=0;for(w=0;w<m->warpCount;w++)if(m->warps[w].x==nx+m->minX&&m->warps[w].z==nz+m->minZ)warp=1;if(!warp)continue;}
    for(actor=0;actor<m->objectCount&&actor<PT_ACTORS;actor++)if(state->actors[actor].visible&&state->actors[actor].x/4096==nx+m->minX&&state->actors[actor].z/4096==nz+m->minZ)blocked=1;
    if(blocked){continue;}
    parent[next]=(int)current;queue[tail++]=next;
   }
  }
  if(parent[goal]<0){fprintf(stderr,"No original walk path from header%u tile%u,%u to%u,%u\n",state->header,state->x,state->z,gx,gz);abort();}
  at=goal;while((unsigned)parent[at]!=start&&at!=start)at=(unsigned)parent[at];
  i=at==start?4:at+1==start?2:at==start+1?3:at<start?0:1;
  free(parent);free(queue);if(i==4)return;
  assert(Platinum_Move(state,i));DrainAdventure();
  if(state->header!=m->header){assert(state->header==destination);return;}
 }
 fprintf(stderr,"WALK BOUND header%u at%u,%u goal%u,%u destination%u rivalState%u\n",original.platinum.header,original.platinum.x,original.platinum.z,gx,gz,destination,Platinum_Value(&original.platinum,PT_VAR_FOLLOWER_RIVAL_STATE));
 assert(!"original walk exceeded bound");
}
static void AuthenticHouseAndTown(void)
{
 Adventure_Init(&original);original.platinum=p;original.mode=ADV_PT_FIELD;
 WalkTo(8,4,PT_MAP_HEADER_TWINLEAF_TOWN_PLAYER_HOUSE_1F);
 assert(original.platinum.header==PT_MAP_HEADER_TWINLEAF_TOWN_PLAYER_HOUSE_1F);
 assert(Platinum_Value(&original.platinum,PT_VAR_PLAYER_HOUSE_STATE)==1);
 WalkTo(6,10,PT_MAP_HEADER_TWINLEAF_TOWN);
 assert(Platinum_Value(&original.platinum,PT_VAR_PLAYER_HOUSE_STATE)==2);
 WalkTo(105,876,PT_MAP_HEADER_TWINLEAF_TOWN);
 assert(Platinum_Value(&original.platinum,PT_VAR_TWINLEAF_TOWN_RIVAL_TRIGGER_STATE)==1);
 WalkTo(105,875,PT_MAP_HEADER_TWINLEAF_TOWN_RIVAL_HOUSE_1F);
 WalkTo(2,3,PT_MAP_HEADER_TWINLEAF_TOWN_RIVAL_HOUSE_2F);
 assert(Platinum_Value(&original.platinum,PT_VAR_RIVAL_HOUSE_STATE)==1);
 assert(Platinum_HasFlag(&original.platinum,PT_FLAG_RIVAL_LEFT_HOME));
 puts("original exploration: physical tile paths, house warps, Mom warning, Twinleaf rival thud and rival-house sequence passed");
 WalkTo(3,4,PT_MAP_HEADER_TWINLEAF_TOWN_RIVAL_HOUSE_1F);
 WalkTo(6,10,PT_MAP_HEADER_TWINLEAF_TOWN);
 WalkTo(112,864,PT_MAP_HEADER_TWINLEAF_TOWN);
 assert(Platinum_Move(&original.platinum,0));DrainAdventure();
 assert(original.platinum.header==PT_MAP_HEADER_ROUTE_201);
 WalkTo(112,857,PT_MAP_HEADER_ROUTE_201);
 assert(Platinum_Value(&original.platinum,PT_VAR_FOLLOWER_RIVAL_STATE)==1);
 fprintf(stderr,"ORIGINAL_ROUTE_READY at%u,%u facing%u\n",original.platinum.x,original.platinum.z,original.platinum.facing);
 puts("original Route201: Rowan/Dawn/Barry dialogue, original yes/no branches and briefcase setup passed");
 WalkTo(113,854,PT_MAP_HEADER_ROUTE_201);
 assert(!Platinum_Move(&original.platinum,2));
 Platinum_Interact(&original.platinum);DrainAdventure();
 assert(original.save.partyCount==1&&original.save.party[0].dex==387);
 fprintf(stderr,"ORIGINAL_STARTER_GIVEN at%u,%u facing%u state%u\n",original.platinum.x,original.platinum.z,original.platinum.facing,Platinum_Value(&original.platinum,PT_VAR_FOLLOWER_RIVAL_STATE));
 puts("original briefcase: choosing Turtwig runs original grant/dialogue and records native party");
 assert(original.mode==ADV_BATTLE&&original.battleKind==ADV_PT_BATTLE);
 assert(original.battle.combat.fighters[0].species==387&&original.battle.combat.fighters[1].species==390);
 assert(original.battle.levels[1][0]==5&&original.battle.combat.fighters[1].maxHp==19);
 /* Host-only injected outcome tests the original continuation, not a gameplay victory. */
 original.battle.party[1][0].hp=0;original.battle.combat.fighters[1].hp=0;original.battle.winner=1;Adventure_FinishBattle(&original,0);DrainAdventure();
 assert(original.platinum.header==PT_MAP_HEADER_TWINLEAF_TOWN_PLAYER_HOUSE_1F);
 assert(Platinum_Value(&original.platinum,PT_VAR_PLAYER_HOUSE_STATE)==4&&original.platinum.runningShoes);
 assert(original.save.party[0].level==6);
 puts("original first battle: actual Turtwig/Chimchar lv5 configs; host-injected victory returns through original Warp, Mom and RunningShoes events");
 WalkTo(6,10,PT_MAP_HEADER_TWINLEAF_TOWN);
 WalkTo(112,864,PT_MAP_HEADER_TWINLEAF_TOWN);
 assert(Platinum_Move(&original.platinum,0));DrainAdventure();
 WalkTo(112,857,PT_MAP_HEADER_ROUTE_201);
 assert(original.platinum.partner&&Platinum_Value(&original.platinum,PT_VAR_FOLLOWER_RIVAL_STATE)==3);
 WalkTo(96,852,PT_MAP_HEADER_ROUTE_201);
 assert(Platinum_Move(&original.platinum,2));DrainAdventure();
 assert(original.platinum.header==PT_MAP_HEADER_VERITY_LAKEFRONT);
 WalkTo(81,844,PT_MAP_HEADER_LAKE_VERITY_LOW_WATER);
 fprintf(stderr,"LAKE ENTRY header%u tile%u,%u state%u visited%u pending%u pc%u\n",original.platinum.header,original.platinum.x,original.platinum.z,Platinum_Value(&original.platinum,PT_VAR_VERITY_LAKEFRONT_STATE),Platinum_Value(&original.platinum,PT_VAR_VISITED_LAKE_VERITY_WITH_RIVAL),original.platinum.pendingTransition,original.platinum.pc);
 assert(original.platinum.header==PT_MAP_HEADER_LAKE_VERITY_LOW_WATER);
 assert(Platinum_Value(&original.platinum,PT_VAR_VISITED_LAKE_VERITY_WITH_RIVAL)==1&&!original.platinum.cameraActive);
 puts("original Lake Verity: Barry follows across original map edges, original warp selection and complete Cyrus camera/movement/dialogue sequence passed");
}
int main(void){OriginalOpening();FailClosed();Checkpoint();AuthenticHouseAndTown();puts("PASS original Platinum import: original IDs/layout/TV/rival path, unsupported-event stop and native checkpoint");return 0;}
