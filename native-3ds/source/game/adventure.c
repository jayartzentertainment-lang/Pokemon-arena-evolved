#include "adventure.h"
#include "arena/vendor/constants/moves.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
static unsigned Min(unsigned a,unsigned b){return a<b?a:b;}
static void Say(Adventure *a,const char *text,unsigned action)
{
    size_t at=0;
    memset(a->dialogue,0,sizeof(a->dialogue));
    while(*text&&at+46<sizeof(a->dialogue)){
        size_t n=strlen(text),take=n>45?45:n;
        if(n>45){size_t split=take;while(split&&text[split]!=' ')split--;if(split)take=split;}
        memcpy(a->dialogue+at,text,take);at+=take;text+=take;while(*text==' ')text++;
        if(*text)while(at%45)a->dialogue[at++]=' ';
    }
    a->dialoguePage=0;a->dialogueAction=(uint8_t)action;a->mode=ADV_DIALOGUE;
}
static void Notice(Adventure *a,const char *s){snprintf(a->message,sizeof(a->message),"%s",s);a->messageTicks=240;}
static unsigned Random(Adventure *a){a->save.random^=a->save.random<<13;a->save.random^=a->save.random>>17;a->save.random^=a->save.random<<5;return a->save.random;}
static void Mark(uint8_t bits[62],unsigned dex){if(dex&&dex<=493)bits[dex/8]|=(uint8_t)(1u<<(dex%8));}
unsigned Adventure_DexCount(const uint8_t bits[62]){unsigned d,n=0;for(d=1;d<=493;d++)if(bits[d/8]&(1u<<(d%8)))n++;return n;}
CombatFighterConfig Adventure_Config(const AdventureMon *m){CombatFighterConfig c={0};const AdventureDex *d;unsigned v[6],i;if(!m||!m->dex||m->dex>493)return c;d=&gAdventureDex[m->dex];for(i=0;i<6;i++)v[i]=(2*d->stats[i]+31)*m->level/100+(i?5:m->level+10);c.species=m->dex;c.hp=m->dex==292?1:v[0];c.attack=v[1];c.defense=v[2];c.speed=v[3];c.spAttack=v[4];c.spDefense=v[5];memcpy(c.moves,m->moves,sizeof(c.moves));memcpy(c.types,d->types,sizeof(c.types));return c;}
static void Learn(AdventureMon *m){const AdventureDex *d=&gAdventureDex[m->dex];unsigned i,count=0;uint16_t moves[4]={0};for(i=0;i<d->learnCount;i++){const AdventureLearn *l=&gAdventureLearn[d->learnStart+i];if(l->level<=m->level&&l->move&&l->move<=467){unsigned j,duplicate=0;for(j=0;j<count;j++)if(moves[j]==l->move)duplicate=1;if(duplicate)continue;if(count<4)moves[count++]=l->move;else{memmove(moves,moves+1,3*sizeof(*moves));moves[3]=l->move;}}}if(!count)moves[0]=MOVE_TACKLE;memcpy(m->moves,moves,sizeof(m->moves));for(i=0;i<4;i++)m->pp[i]=moves[i]?Combat_GetMoveInfo(moves[i])->pp:0;}
void Adventure_MakeMon(AdventureMon *m,unsigned dex,unsigned level){memset(m,0,sizeof(*m));if(dex<1||dex>493)return;m->dex=(uint16_t)dex;m->level=(uint8_t)(level<1?1:level>100?100:level);m->xp=(uint32_t)m->level*m->level*m->level;Learn(m);m->hp=Adventure_Config(m).hp;}
void Adventure_Heal(Adventure *a){unsigned i,j;for(i=0;i<a->save.partyCount;i++){AdventureMon *m=&a->save.party[i];m->hp=Adventure_Config(m).hp;m->status=0;for(j=0;j<4;j++)m->pp[j]=m->moves[j]?Combat_GetMoveInfo(m->moves[j])->pp:0;}a->needsSave=1;}
unsigned Adventure_Healthy(const Adventure *a){unsigned i,n=0;for(i=0;i<a->save.partyCount;i++)if(a->save.party[i].hp)n++;return n;}
static void ResetTrail(Adventure *a){unsigned i;a->previousX=a->save.x;a->previousY=a->save.y;a->followerX=a->save.x;a->followerY=a->save.y+18*256;for(i=0;i<32;i++){a->trailX[i]=a->save.x;a->trailY[i]=a->save.y;}a->trailIndex=0;}
void Adventure_Init(Adventure *a){memset(a,0,sizeof(*a));Adventure_New(a);a->mode=ADV_TITLE;a->labDex[0]=6;a->labDex[1]=9;}
void Adventure_New(Adventure *a){memset(&a->save,0,sizeof(a->save));snprintf(a->save.name,sizeof(a->save.name),"TRAINER");/* Custom starts at the supplied blank body; every optional layer is None. */a->save.appearance.base=ADV_TRAINER_GEN4;a->save.appearance.follower=1;a->save.x=256*256;a->save.y=256*256;a->save.visited=1;a->save.random=0x54a7d19;a->save.money=3000;a->save.potions=5;a->save.balls=10;a->battleKind=ADV_STORY_BATTLE;a->needsSave=0;a->transition=0;a->messageTicks=0;a->mode=ADV_CREATOR;a->cursor=0;a->previewDirection=0;a->newGameConfirm=0;ResetTrail(a);}
const char *Adventure_Objective(const Adventure *a){return a->save.step<ADVENTURE_EVENTS?gAdventureEvents[a->save.step].name:"CHAMPION / EXPLORE SINNOH";}
int Adventure_Travel(Adventure *a,unsigned map,int discovered){if(map>=ADVENTURE_MAPS)return 0;if(gAdventureMaps[map].requires>a->save.step){Notice(a,"Finish your current story objective first.");return 0;}if(discovered&&!(a->save.visited&((uint64_t)1<<map))){Notice(a,"Visit this place on foot before fast travel.");return 0;}a->save.map=(uint8_t)map;a->save.visited|=(uint64_t)1<<map;a->save.x=256*256;a->save.y=260*256;a->encounterCooldown=120;a->walkDistance=0;a->mode=ADV_FIELD;ResetTrail(a);a->needsSave=1;return 1;}
int Adventure_NextRoad(const Adventure *a){unsigned queue[ADVENTURE_MAPS],parent[ADVENTURE_MAPS],head=0,tail=0,i,dest,at;if(a->save.step>=ADVENTURE_EVENTS)return -1;dest=gAdventureEvents[a->save.step].map;if(dest==a->save.map)return -1;for(i=0;i<ADVENTURE_MAPS;i++)parent[i]=255;queue[tail++]=a->save.map;parent[a->save.map]=a->save.map;while(head<tail){at=queue[head++];for(i=0;i<gAdventureMaps[at].linkCount;i++){unsigned n=gAdventureMaps[at].links[i];if(parent[n]==255&&gAdventureMaps[n].requires<=a->save.step){parent[n]=at;queue[tail++]=n;}}}if(parent[dest]==255)return -1;while(parent[dest]!=a->save.map){dest=parent[dest];if(dest>=ADVENTURE_MAPS)return -1;}for(i=0;i<gAdventureMaps[a->save.map].linkCount;i++)if(gAdventureMaps[a->save.map].links[i]==dest)return (int)i;return -1;}
static void Complete(Adventure *a){if(a->save.step<ADVENTURE_EVENTS){const AdventureEvent *e=&gAdventureEvents[a->save.step];if(e->badge>=0)a->save.badges|=(uint8_t)(1u<<e->badge);a->save.step++;if(a->save.step==ADVENTURE_EVENTS)a->save.champion=1;}a->needsSave=1;}
static void BeginBattle(Adventure *a,int wild,unsigned dex,unsigned level){CombatFighterConfig configs[2][6]={{{0}}};GamePartySlot states[2][6]={{{0}}};uint8_t counts[2]={0,0};AdventureMon enemy[6];const AdventureEvent *e=a->save.step<ADVENTURE_EVENTS?&gAdventureEvents[a->save.step]:0;unsigned i,j,doubles=0;memset(enemy,0,sizeof(enemy));if(!Adventure_Healthy(a)){Notice(a,"Your team needs healing. Visit a town or use potions.");a->mode=ADV_FIELD;return;}a->battleKind=wild?ADV_WILD_BATTLE:ADV_STORY_BATTLE;a->battleEvent=a->save.step;a->caughtBattle=0;counts[0]=a->save.partyCount;
 if(wild){Adventure_MakeMon(&enemy[0],dex,level);counts[1]=1;}else if(e){
  if(e->kind==2){unsigned rival[3]={390,393,387};unsigned r=rival[a->save.starter];if(e->level>=16)r++;Adventure_MakeMon(&enemy[0],r,e->level);counts[1]=1;if(e->level>5){Adventure_MakeMon(&enemy[1],396,e->level-1);counts[1]=2;}}
  else if(e->kind==4){static const uint16_t duo[][2]={{41,434},{265,267},{453,434},{432,435}};unsigned row=a->save.step==12?0:a->save.step==14?1:a->save.step==19?2:3;for(i=0;i<2;i++)Adventure_MakeMon(&enemy[i],duo[row][i],e->level);counts[1]=2;doubles=1;}
  else if(e->teamCount){counts[1]=e->teamCount;for(i=0;i<counts[1];i++){const AdventureTrainerMon *t=&gStoryTeams[e->teamStart+i];Adventure_MakeMon(&enemy[i],t->dex,t->level);memcpy(enemy[i].moves,t->moves,sizeof(t->moves));for(j=0;j<4;j++)enemy[i].pp[j]=t->moves[j]?Combat_GetMoveInfo(t->moves[j])->pp:0;}}
 }
 for(i=0;i<counts[0];i++){const AdventureMon *m=&a->save.party[i];configs[0][i]=Adventure_Config(m);states[0][i].hp=m->hp;states[0][i].status=m->status;states[0][i].stamina=100;memcpy(states[0][i].pp,m->pp,4);Mark(a->save.seen,m->dex);}
 /* A genuine partner actor joins story doubles if the owned team has <2 healthy Pokemon. */
 if(doubles&&Adventure_Healthy(a)<2){AdventureMon ally;unsigned slot=counts[0];if(slot==6){for(slot=0;slot<6&&states[0][slot].hp;slot++);if(slot==6)slot=5;}Adventure_MakeMon(&ally,a->save.step==14?113:395,e->level);configs[0][slot]=Adventure_Config(&ally);states[0][slot].hp=ally.hp;states[0][slot].stamina=100;memcpy(states[0][slot].pp,ally.pp,4);if(slot==counts[0])counts[0]++;}
 for(i=0;i<counts[1];i++){configs[1][i]=Adventure_Config(&enemy[i]);states[1][i].hp=enemy[i].hp;states[1][i].stamina=100;memcpy(states[1][i].pp,enemy[i].pp,4);Mark(a->save.seen,enemy[i].dex);}
 if(!counts[1]){Notice(a,"No valid opponent configured.");a->mode=ADV_FIELD;return;}
 Game_StartParty(&a->battle,configs,counts,states,doubles);
 for(i=0;i<counts[0];i++)a->battle.levels[0][i]=i<a->save.partyCount?a->save.party[i].level:e->level;
 for(i=0;i<counts[1];i++)a->battle.levels[1][i]=enemy[i].level;
 a->mode=ADV_ENCOUNTER;a->transition=60;
}
static int OriginalBattle(Adventure *a)
{
 CombatFighterConfig configs[2][6]={{{0}}};GamePartySlot states[2][6]={{{0}}};uint8_t counts[2]={a->save.partyCount,0};unsigned i,j;const PtTrainer *trainer=NULL;
 for(i=0;i<gPtTrainerCount;i++)if(gPtTrainers[i].id==a->platinum.trainer){trainer=&gPtTrainers[i];break;}
 if(!trainer||!counts[0]||!trainer->count||trainer->count>6)return 0;
 for(i=0;i<counts[0];i++){const AdventureMon *m=&a->save.party[i];configs[0][i]=Adventure_Config(m);states[0][i].hp=m->hp;states[0][i].stamina=100;states[0][i].status=m->status;memcpy(states[0][i].pp,m->pp,4);}
 counts[1]=trainer->count;
 for(i=0;i<counts[1];i++){
  const PtTrainerMon *source=&gPtTrainerMons[trainer->offset+i];AdventureMon mon;unsigned iv=source->iv*31/255;const AdventureDex *d=&gAdventureDex[source->dex];
  if(source->form||source->iv>255)return 0;
  Adventure_MakeMon(&mon,source->dex,source->level);
  if(source->moves[0]){memcpy(mon.moves,source->moves,sizeof(mon.moves));for(j=0;j<4;j++)mon.pp[j]=mon.moves[j]?Combat_GetMoveInfo(mon.moves[j])->pp:0;}
  configs[1][i]=Adventure_Config(&mon);
  configs[1][i].hp=(2*d->stats[0]+iv)*source->level/100+source->level+10;
  configs[1][i].attack=(2*d->stats[1]+iv)*source->level/100+5;configs[1][i].defense=(2*d->stats[2]+iv)*source->level/100+5;
  configs[1][i].speed=(2*d->stats[3]+iv)*source->level/100+5;configs[1][i].spAttack=(2*d->stats[4]+iv)*source->level/100+5;configs[1][i].spDefense=(2*d->stats[5]+iv)*source->level/100+5;
  states[1][i].hp=configs[1][i].hp;states[1][i].stamina=100;memcpy(states[1][i].pp,mon.pp,4);
 }
 Game_StartParty(&a->battle,configs,counts,states,trainer->doubles);
 for(i=0;i<counts[0];i++)a->battle.levels[0][i]=a->save.party[i].level;
 for(i=0;i<counts[1];i++)a->battle.levels[1][i]=gPtTrainerMons[trainer->offset+i].level;
 a->battleKind=ADV_PT_BATTLE;a->mode=ADV_BATTLE;return 1;
}
static void OriginalService(Adventure *a)
{
 Platinum *p=&a->platinum;
 if(p->service==3){
  if(a->save.partyCount>=6){Platinum_Set(p,p->waitVar,0);}else{AdventureMon *m=&a->save.party[a->save.partyCount++];Adventure_MakeMon(m,p->serviceArg[0],p->serviceArg[1]);m->xp=gPtExperience[gPtGrowthRates[m->dex]][m->level];Platinum_Set(p,p->waitVar,1);Mark(a->save.caught,m->dex);Mark(a->save.seen,m->dex);}
  p->waiting=p->service=0;
 }else if(p->service==4){if(!OriginalBattle(a)){p->blocked=1;p->fault="Original trainer team / form not supported";}}
 else if(p->service==5){Adventure_Heal(a);p->waiting=p->service=0;}
}
static void Wild(Adventure *a){const AdventureMap *m=&gAdventureMaps[a->save.map];unsigned count=0,level=3,i;while(count<4&&m->wild[count])count++;for(i=0;i<8;i++)if(a->save.badges&(1u<<i))level+=5;if(a->save.step<ADVENTURE_EVENTS){unsigned at;for(at=a->save.step;at<ADVENTURE_EVENTS;at++)if(gAdventureEvents[at].level){unsigned goal=gAdventureEvents[at].level; if(goal>level+2)level=goal-2;break;}}level+=Random(a)%3;if(!count||!a->save.partyCount)return;BeginBattle(a,1,a->save.champion?(1+Random(a)%493):m->wild[Random(a)%count],level);}
void Adventure_Interact(Adventure *a){const AdventureEvent *e=a->save.step<ADVENTURE_EVENTS?&gAdventureEvents[a->save.step]:0;int x=a->save.x/256,y=a->save.y/256;if(e&&e->map==a->save.map&&abs(x-256)<=40&&abs(y-152)<=40){Say(a,e->text,1);return;}if(abs(x-368)<=40&&abs(y-158)<=40){Adventure_Heal(a);Say(a,"Your Pokemon are fully healed, including PP and status. Come back any time!",0);return;}Notice(a,"Follow the gold marker. START opens your trainer menu.");}
static void ResolveDialogue(Adventure *a){const AdventureEvent *e=a->save.step<ADVENTURE_EVENTS?&gAdventureEvents[a->save.step]:0;a->mode=ADV_FIELD;if(a->dialogueAction!=1||!e)return;if(e->kind==1){a->mode=ADV_STARTER;a->cursor=0;return;}if(e->kind==2||e->kind==3||e->kind==4){BeginBattle(a,0,0,0);return;}if(e->kind==5){BeginBattle(a,1,487,e->level);a->battleKind=ADV_STORY_BATTLE;return;}if(a->save.step==0||a->save.step==4||a->save.step==14)Adventure_Heal(a);if(a->save.step==7){a->save.balls=Min(a->save.balls+20,999);a->save.potions=Min(a->save.potions+5,999);}Complete(a);}
static void Award(Adventure *a){unsigned reward=0,t,i,j,minLevel=0;for(i=0;i<a->battle.partyCount[1];i++)reward+=gAdventureDex[a->battle.configs[1][i].species].expReward*a->battle.levels[1][i];if(a->battleKind==ADV_STORY_BATTLE)minLevel=gAdventureEvents[a->battleEvent].level;for(i=0;i<a->save.partyCount;i++){AdventureMon *m=&a->save.party[i];unsigned oldLevel=m->level,oldDex=m->dex,oldMax=Adventure_Config(m).hp;m->xp+=reward/(a->save.partyCount>2?2:1);if(m->xp<minLevel*minLevel*minLevel)m->xp=minLevel*minLevel*minLevel;while(m->level<100&&m->xp>=(unsigned)(m->level+1)*(m->level+1)*(m->level+1))m->level++;for(t=0;t<2;t++){const AdventureDex *d=&gAdventureDex[m->dex];unsigned found=0;for(j=0;j<d->evoCount;j++){const AdventureEvolution *e=&gAdventureEvolution[d->evoStart+j];if(m->level>=e->level){m->dex=e->dex;found=1;break;}}if(!found)break;}if(m->level!=oldLevel||m->dex!=oldDex){Learn(m);if(m->hp){int hp=(int)m->hp+(int)Adventure_Config(m).hp-(int)oldMax;m->hp=hp<1?1:Min((unsigned)hp,Adventure_Config(m).hp);}}Mark(a->save.caught,m->dex);Mark(a->save.seen,m->dex);}a->save.money=Min(a->save.money+reward/2+150,60000);}
static void OriginalExperience(Adventure *a)
{
 unsigned i,j,participants=0;
 for(i=0;i<a->save.partyCount;i++)if((a->battle.participated&(1u<<i))&&a->save.party[i].hp)participants++;
 if(!participants)return;
 for(i=0;i<a->save.partyCount;i++)if((a->battle.participated&(1u<<i))&&a->save.party[i].hp){
  AdventureMon *m=&a->save.party[i];unsigned oldHp=Adventure_Config(m).hp,oldLevel=m->level;
  for(j=0;j<a->battle.partyCount[1];j++)if(!a->battle.party[1][j].hp){unsigned dex=a->battle.configs[1][j].species;m->xp+=(gAdventureDex[dex].expReward*a->battle.levels[1][j]/(7*participants))*3/2;}
  while(m->level<100&&m->xp>=gPtExperience[gPtGrowthRates[m->dex]][m->level+1])m->level++;
  if(m->level!=oldLevel){m->hp=(uint16_t)(m->hp+Adventure_Config(m).hp-oldHp);}
 }
}
void Adventure_FinishBattle(Adventure *a,int fled){unsigned i;int win=a->battle.winner==1||a->caughtBattle;if(a->battleKind==ADV_LAB_BATTLE){a->mode=ADV_LAB;return;}for(i=0;i<a->save.partyCount;i++){if(a->battle.configs[0][i].species==a->save.party[i].dex){a->save.party[i].hp=a->battle.party[0][i].hp;a->save.party[i].status=a->battle.party[0][i].status;memcpy(a->save.party[i].pp,a->battle.party[0][i].pp,4);}}if(a->battleKind==ADV_PT_BATTLE){OriginalExperience(a);a->platinum.lastBattleWon=(uint8_t)win;a->platinum.waiting=a->platinum.service=0;a->mode=ADV_PT_FIELD;a->needsSave=1;return;}a->mode=ADV_FIELD;a->encounterCooldown=240;ResetTrail(a);if(!fled&&win){Award(a);if(a->battleKind==ADV_STORY_BATTLE&&a->save.step==a->battleEvent)Complete(a);if(a->battleEvent==14)Adventure_Heal(a);Say(a,a->caughtBattle?"Caught successfully! Your partner has joined the party or PC. Progress has been saved.":"Victory! Your partners gained experience. Open PARTY to see levels, moves and evolutions. Your adventure continues.",0);}else if(!fled){Adventure_Heal(a);a->save.money=a->save.money>300?a->save.money-300:0;Say(a,"Your team needs a rest. You have been healed. Your story objective is still waiting, so you can prepare and try again.",0);}else Notice(a,"Returned from battle. Story progress has not advanced.");a->needsSave=1;}
void Adventure_Catch(Adventure *a){CombatFighter *enemy;AdventureMon m;unsigned chance;if(a->mode!=ADV_BATTLE||a->battleKind==ADV_LAB_BATTLE)return;if(a->battleKind!=ADV_WILD_BATTLE&&!(a->battleKind==ADV_STORY_BATTLE&&gAdventureEvents[a->battleEvent].kind==5)){Notice(a,"You cannot catch a trainer's Pokemon.");return;}if(!a->save.balls){Notice(a,"No Pokeballs left. Buy more in a town.");return;}if(a->save.partyCount==6&&a->save.boxCount==ADVENTURE_BOX){Notice(a,"Party and PC are full. Release a boxed Pokemon first.");return;}enemy=&a->battle.combat.fighters[1];if(!enemy->hp||a->battle.winner)return;a->save.balls--;chance=15+gAdventureDex[enemy->species].catchRate/3+(enemy->maxHp-enemy->hp)*55/enemy->maxHp;if(enemy->status)chance+=20;if(chance>95)chance=95;if(Random(a)%100>=chance){Notice(a,"The Pokemon broke free! Weaken it or inflict a status.");return;}Adventure_MakeMon(&m,enemy->species,a->battle.levels[1][0]);m.hp=enemy->hp;m.status=Combat_GetNativeStatus(&a->battle.combat,1);memcpy(m.moves,enemy->moves,sizeof(m.moves));memcpy(m.pp,enemy->pp,4);if(a->save.partyCount<6)a->save.party[a->save.partyCount++]=m;else a->save.box[a->save.boxCount++]=m;Mark(a->save.caught,m.dex);Mark(a->save.seen,m.dex);a->caughtBattle=1;a->battle.winner=1;}
void Adventure_StartLab(Adventure *a,int doubles){CombatFighterConfig configs[2][6]={{{0}}};GamePartySlot states[2][6]={{{0}}};uint8_t counts[2]={4,4};unsigned t,i,j;for(t=0;t<2;t++)for(i=0;i<4;i++){const ArenaSpecies *s=ArenaSpecies_Find(i?gSpecies[i].dex:a->labDex[t]);configs[t][i]=s->config;states[t][i].hp=s->config.hp;states[t][i].stamina=100;for(j=0;j<4;j++)states[t][i].pp[j]=Combat_GetMoveInfo(s->config.moves[j])->pp;}Game_StartParty(&a->battle,configs,counts,states,doubles);for(t=0;t<2;t++)for(i=0;i<4;i++)a->battle.levels[t][i]=50;if(a->labClash)Combat_SetClashPractice(&a->battle.combat,1);a->battleKind=ADV_LAB_BATTLE;a->mode=ADV_BATTLE;}
static int Free(const Adventure *a,int x,int y){const AdventureMap *m=&gAdventureMaps[a->save.map];int i;if(x<6||y<6||x>=506||y>=378)return 0;i=(y/16)*32+x/16;return !m->blocked[i];}
static void Move(Adventure *a,GameInput in){int dx=0,dy=0,speed=(in.held&GAME_B)?640:448;unsigned h=in.held;int ax=in.axisX,ay=in.axisY;if(abs(ax)>18||abs(ay)>18){int mag=(int)sqrt((double)ax*ax+(double)ay*ay);if(mag>156){ax=ax*156/mag;ay=ay*156/mag;}dx=(int)(ax*speed/156.0);dy=(int)(ay*speed/156.0);if(abs(ax)>abs(ay))a->facing=ax>0?2:1;else a->facing=ay>0?0:3;}else{if(h&GAME_LEFT)dx=-speed;if(h&GAME_RIGHT)dx=speed;if(h&GAME_UP)dy=-speed;if(h&GAME_DOWN)dy=speed;if(dx&&dy){dx=dx*181/256;dy=dy*181/256;}if(dx)a->facing=dx>0?2:1;else if(dy)a->facing=dy>0?0:3;}
 a->previousX=a->save.x;a->previousY=a->save.y;a->moving=0;
 if(dx&&Free(a,(a->save.x+dx)/256-5,a->save.y/256-3)&&Free(a,(a->save.x+dx)/256+5,a->save.y/256+2)){a->save.x+=dx;a->moving=1;}
 if(dy&&Free(a,a->save.x/256-5,(a->save.y+dy)/256-3)&&Free(a,a->save.x/256+5,(a->save.y+dy)/256+2)){a->save.y+=dy;a->moving=1;}
 if(a->moving){a->walkDistance+=abs(a->save.x-a->previousX)+abs(a->save.y-a->previousY);a->walkFrame=(a->walkDistance/2200)%4;a->trailX[a->trailIndex]=a->save.x;a->trailY[a->trailIndex]=a->save.y;a->trailIndex=(a->trailIndex+1)%32;a->followerX=a->trailX[a->trailIndex];a->followerY=a->trailY[a->trailIndex];}else a->walkFrame=0;
 {const AdventureMap *m=&gAdventureMaps[a->save.map];int x=a->save.x/256,y=a->save.y/256;int road=y<20?0:x>490?1:y>364?2:x<20?3:-1;if(road>=0&&road<m->linkCount&&!a->transition){if(!Adventure_Travel(a,m->links[road],0)){a->save.x=256*256;a->save.y=260*256;ResetTrail(a);}a->transition=30;}if(!a->encounterCooldown&&a->save.partyCount&&m->grass[(y/16)*32+x/16]&&a->walkDistance>16*256){a->walkDistance=0;if(Random(a)%6==0)Wild(a);}}
}
void Adventure_Tick(Adventure *a,GameInput in){a->ticks++;if(a->messageTicks)a->messageTicks--;if(a->transition)a->transition--;if(a->encounterCooldown)a->encounterCooldown--;if(a->mode!=ADV_TITLE&&a->mode!=ADV_LAB&&a->battleKind!=ADV_LAB_BATTLE)a->save.playTicks++;if(a->mode==ADV_PT_FIELD){
 Platinum *p=&a->platinum;p->runHeld=!!(in.held&GAME_B);int x=in.axisX,y=in.axisY;unsigned direction=4;
 if(abs(x)>18||abs(y)>18)direction=abs(x)>abs(y)?(x<0?2:3):(y<0?0:1);
 else if(in.held&GAME_UP)direction=0;else if(in.held&GAME_DOWN)direction=1;else if(in.held&GAME_LEFT)direction=2;else if(in.held&GAME_RIGHT)direction=3;
 if(direction<4){Platinum_Move(p,direction);}
 Platinum_Tick(p);OriginalService(a);
 }else if(a->mode==ADV_FIELD)Move(a,in);else if(a->mode==ADV_ENCOUNTER&&!a->transition)a->mode=ADV_BATTLE;}
unsigned Adventure_CreatorRows(const AdventureAppearance *p){return p->base==ADV_TRAINER_GEN4?9:p->base==ADV_TRAINER_CUSTOM?10:6;}
unsigned Adventure_AppearanceCount(const AdventureAppearance *p,unsigned g)
{
 if(g>=6)return 0;
 if(p->base==ADV_TRAINER_GEN4)return gGen4AppearanceCounts[g];
 if(p->base==ADV_TRAINER_CUSTOM)return gAppearanceCounts[g];
 /* Fixed bases retain unused Custom selections when switching temporarily. */
 return gGen4AppearanceCounts[g]>gAppearanceCounts[g]?gGen4AppearanceCounts[g]:gAppearanceCounts[g];
}
static void AppearanceCycle(Adventure *a,int delta)
{
 AdventureAppearance *p=&a->save.appearance;unsigned row=a->cursor;
 if(row==0){a->mode=ADV_NAME;a->keyboard=0;return;}
 if(row==1){
  static const uint8_t bases[]={ADV_TRAINER_GEN4,ADV_TRAINER_RED,ADV_TRAINER_BRENDAN};unsigned selected=0;
  if(p->base==ADV_TRAINER_CUSTOM){memset(p->option,0,sizeof(p->option));p->base=ADV_TRAINER_GEN4;}
  else {while(selected<3&&bases[selected]!=p->base)selected++;p->base=bases[(selected+3+delta)%3];}
  if(p->base==ADV_TRAINER_RED||p->base==ADV_TRAINER_BRENDAN)p->body=0;
 }
 else if(p->base==ADV_TRAINER_RED||p->base==ADV_TRAINER_BRENDAN){
  if(row==2)p->option[0]=(uint8_t)((p->option[0]+gAppearanceCounts[0]+delta)%gAppearanceCounts[0]);
  else if(row==3)p->hairColour=(uint8_t)((p->hairColour+ADVENTURE_HAIR_COLOURS+delta)%ADVENTURE_HAIR_COLOURS);
  else if(row==4)p->outfitColour=(uint8_t)((p->outfitColour+ADVENTURE_OUTFIT_COLOURS+delta)%ADVENTURE_OUTFIT_COLOURS);
  else if(row==5)p->follower^=1;
 }else {
  static const uint8_t group[10]={0,0,0,0,3,2,1,4,5,0};
  if(row==2)p->body^=1;
  else if(row==Adventure_CreatorRows(p)-1)p->follower^=1;
  else if(row<9){unsigned g=group[row],n=Adventure_AppearanceCount(p,g);p->option[g]=(uint8_t)((p->option[g]+n+delta)%n);}
 }
 if(a->cursor>=Adventure_CreatorRows(p))a->cursor=(uint8_t)(Adventure_CreatorRows(p)-1);
 if(a->returnMode==ADV_CAMP||a->returnMode==ADV_PT_MENU)a->needsSave=1;
}
static void NewConfirmed(Adventure *a){a->mode=ADV_FIELD;a->needsSave=1;ResetTrail(a);Say(a,"Welcome to Twinleaf Town! Your own trainer is ready. Speak to Mom at the gold marker, then meet Barry. Use the Circle Pad to walk and A to talk.",0);}
static void Camp(Adventure *a,unsigned c){unsigned i;const AdventureMap *m=&gAdventureMaps[a->save.map];int town=strstr(m->name,"Town")||strstr(m->name,"City")||a->save.map==33;switch(c){case 0:a->mode=ADV_FIELD;a->needsSave=1;Notice(a,"Saving adventure...");break;case 1:a->mode=ADV_PARTY;a->cursor=0;break;case 2:a->mode=ADV_MAP;a->cursor=a->save.map;break;case 3:a->mode=ADV_CREATOR;a->returnMode=ADV_CAMP;a->cursor=0;break;case 4:if(town){Adventure_Heal(a);Notice(a,"Pokemon Center: team fully healed.");}else Notice(a,"Visit a town Pokemon Center to heal.");break;case 5:if(town&&a->save.money>=100){a->save.money-=100;a->save.balls=Min(a->save.balls+5,999);a->needsSave=1;Notice(a,"Bought5 Pokeballs for $100.");}else Notice(a,"Buy balls in a town. You need $100.");break;case 6:if(a->save.partyCount){a->mode=ADV_FIELD;Wild(a);}break;case 7:a->mode=ADV_PC;a->cursor=0;break;case 8:a->mode=ADV_TITLE;a->cursor=0;a->needsSave=1;break;case 9:for(i=0;i<a->save.partyCount;i++)if(a->save.party[i].hp){AdventureMon temp=a->save.party[0];a->save.party[0]=a->save.party[i];a->save.party[i]=temp;break;}break;}}
void Adventure_Input(Adventure *a,GameInput in,int start,int tx,int ty,int touch){unsigned down=in.pressed;int delta=(down&GAME_RIGHT)?1:(down&GAME_LEFT)?-1:0;unsigned i;
 if(a->mode==ADV_PT_INTRO){
  Platinum *p=&a->platinum;snprintf(p->buffers[0],32,"%s",p->introPage>=9&&p->introPage<=10?"BARRY":a->save.name);
  if(start&&(in.held&GAME_SELECT)){p->waiting=0;p->text[0]=0;p->introMenu=p->introHelp=0;p->introShortcut=1;a->mode=ADV_CREATOR;a->returnMode=ADV_PT_INTRO;return;}
  if(p->introMenu){
   if(down&GAME_UP)p->introChoice=(p->introChoice+2)%3;
   if(down&GAME_DOWN)p->introChoice=(p->introChoice+1)%3;
   if(down&GAME_A){p->introMenu=0;p->introHelp=p->introChoice==0?1:p->introChoice==1?8:0;if(!p->introHelp)p->introPage=2;}
   return;
  }
  if(!p->waiting)Platinum_Text(p,p->introHelp?gPtIntroHelp[p->introHelp-1]:gPtIntroMain[p->introPage]);
  if(down&GAME_A){
   Platinum_Advance(p);
   if(!p->waiting){
    if(p->introHelp){if(++p->introHelp==8||p->introHelp==14){p->introHelp=0;p->introMenu=1;}}
    else if(p->introPage==1){p->introMenu=1;p->introChoice=2;}
    else if(p->introPage==6&&!p->creatorDone){a->mode=ADV_CREATOR;a->returnMode=ADV_PT_INTRO;}
    else if(++p->introPage>=gPtIntroCount){p->intro=0;p->waiting=0;Platinum_Enter(p,gPtMaps[0].header,4,6,0);a->mode=ADV_PT_FIELD;}
   }
  }
  return;
 }
 if(a->mode==ADV_PT_MENU){
  if(down&GAME_UP)a->cursor=(a->cursor+4)%5;
  if(down&GAME_DOWN)a->cursor=(a->cursor+1)%5;
  if((down&GAME_B)||start){a->mode=ADV_PT_FIELD;return;}
  if(down&GAME_A){
   if(a->cursor==0)a->mode=ADV_PT_FIELD;
   if(a->cursor==1){a->needsSave=1;a->mode=ADV_PT_FIELD;}
   if(a->cursor==2){a->returnMode=ADV_PT_MENU;a->mode=ADV_CREATOR;a->cursor=0;}
   if(a->cursor==3){a->returnMode=ADV_PT_MENU;a->mode=ADV_PARTY;a->cursor=0;}
   if(a->cursor==4){a->needsSave=1;a->mode=ADV_TITLE;a->cursor=0;}
  }
  return;
 }
 if(a->mode==ADV_PT_FIELD){
  Platinum *p=&a->platinum;snprintf(p->buffers[7],32,"%s",a->save.name);
  if(p->blocked){if(down&GAME_B){a->mode=ADV_TITLE;a->cursor=0;}return;}
  if(p->waiting==4&&p->service==1){
   if(down&(GAME_UP|GAME_DOWN))p->choice^=1;
   if(down&GAME_A){Platinum_Set(p,p->waitVar,p->choice);p->service=p->waiting=0;}return;
  }
  if(p->waiting==4&&p->service==2){
   if(down&GAME_LEFT){p->choice=(p->choice+2)%3;}
   if(down&GAME_RIGHT)p->choice=(p->choice+1)%3;
   if(down&GAME_A){static const unsigned dex[]={387,390,393};p->starter=(uint16_t)dex[p->choice];a->save.starter=p->choice;p->service=p->waiting=0;}return;
  }
  if(p->waiting==1&&(down&GAME_A))Platinum_Advance(p);
  else if(down&GAME_A)Platinum_Interact(p);
  if(start&&!p->locked&&p->pc==PT_NO_PC&&!p->waiting){a->mode=ADV_PT_MENU;a->cursor=0;}
  
  return;
 }
 if(a->mode==ADV_TITLE){if(down&GAME_UP)a->cursor=(a->cursor+3)%4;if(down&GAME_DOWN)a->cursor=(a->cursor+1)%4;if(touch&&tx>=40&&tx<280&&ty>=60&&ty<220){a->cursor=(ty-60)/40;down|=GAME_A;}if(start)down|=GAME_A;if(down&GAME_A){if(a->cursor==0){if(a->hasPlatinumSave){a->save=a->checkpoint;a->platinum=a->platinumCheckpoint;a->mode=ADV_PT_FIELD;a->platinum.active=1;}else if(a->hasSave){a->platinum.active=0;a->mode=ADV_FIELD;a->battleKind=ADV_STORY_BATTLE;ResetTrail(a);}else Notice(a,"No saved adventure yet. Choose NEW ADVENTURE.");}else if(a->cursor==1){if((a->hasSave||a->hasPlatinumSave)&&!a->newGameConfirm){a->newGameConfirm=1;Notice(a,"Press A again to replace your adventure. B cancels.");}else{Adventure_New(a);Platinum_New(&a->platinum);a->mode=ADV_PT_INTRO;a->returnMode=ADV_PT_INTRO;/* Preserve the blank Custom default through the original intro. */a->save.balls=0;a->save.potions=0;}}else if(a->cursor==2){a->mode=ADV_LAB;a->labSide=0;}else Notice(a,"Original Platinum import in development. See import report.");}if(down&GAME_B)a->newGameConfirm=0;return;}
 if(a->mode==ADV_LAB){if(down&(GAME_UP|GAME_DOWN|GAME_SELECT))a->labSide^=1;if(down&GAME_L)delta=-10;if(down&GAME_R)delta=10;if(delta)a->labDex[a->labSide]=(a->labDex[a->labSide]-1+493+delta)%493+1;if(down&GAME_Y)a->labClash^=1;if(touch){if(ty>=64&&ty<112){a->labSide=0;delta=tx<160?-1:1;}if(ty>=112&&ty<160){a->labSide=1;delta=tx<160?-1:1;}if(ty>=176&&ty<212){Adventure_StartLab(a,tx>160);return;}if(delta)a->labDex[a->labSide]=(a->labDex[a->labSide]-1+493+delta)%493+1;}if(down&GAME_A)Adventure_StartLab(a,0);if(down&GAME_X)Adventure_StartLab(a,1);if((down&GAME_B)||start){a->mode=ADV_TITLE;a->cursor=0;}return;}
 if(a->mode==ADV_CREATOR){unsigned rows=Adventure_CreatorRows(&a->save.appearance);if(down&GAME_UP)a->cursor=(a->cursor+rows-1)%rows;if(down&GAME_DOWN)a->cursor=(a->cursor+1)%rows;if(down&GAME_L)a->previewDirection=(a->previewDirection+3)%4;if(down&GAME_R)a->previewDirection=(a->previewDirection+1)%4;if(delta)AppearanceCycle(a,delta);if(down&GAME_A)AppearanceCycle(a,1);if(down&GAME_Y){a->mode=ADV_NAME;a->keyboard=0;}if(touch&&ty>=32&&ty<32+(int)rows*16){a->cursor=(ty-32)/16;AppearanceCycle(a,tx<160?-1:1);}if(touch&&ty>=210&&ty<234)start=1;if(start){if(a->returnMode==ADV_CAMP||a->returnMode==ADV_PT_MENU){a->mode=a->returnMode;a->needsSave=1;}else if(a->platinum.active){a->platinum.gender=a->save.appearance.body;a->platinum.creatorDone=1;a->platinum.introPage=7;a->platinum.waiting=0;if(a->platinum.introShortcut){a->platinum.intro=0;Platinum_Enter(&a->platinum,gPtMaps[0].header,4,6,0);a->mode=ADV_PT_FIELD;}else a->mode=ADV_PT_INTRO;}else NewConfirmed(a);}if(down&GAME_B){if(a->returnMode==ADV_CAMP||a->returnMode==ADV_PT_MENU)a->mode=a->returnMode;else {a->mode=ADV_TITLE;a->cursor=1;if(a->hasSave)a->save=a->checkpoint;}}return;}
 if(a->mode==ADV_NAME){static const char keys[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 -";unsigned len=strlen(a->save.name);if(down&GAME_LEFT)a->keyboard=(a->keyboard+39)%40;if(down&GAME_RIGHT)a->keyboard=(a->keyboard+1)%40;if(down&GAME_UP)a->keyboard=(a->keyboard+30)%40;if(down&GAME_DOWN)a->keyboard=(a->keyboard+10)%40;if(touch&&tx>=10&&tx<310&&ty>=66&&ty<210){a->keyboard=(ty-66)/36*10+(tx-10)/30;down|=GAME_A;}if(down&GAME_B){if(len)a->save.name[len-1]=0;}if(down&GAME_A){if(a->keyboard==38){if(len)a->save.name[len-1]=0;}else if(a->keyboard==39)start=1;else if(len<10){a->save.name[len]=keys[a->keyboard];a->save.name[len+1]=0;}}if(start&&a->save.name[0])a->mode=ADV_CREATOR;return;}
 if(a->mode==ADV_DIALOGUE){if(down&(GAME_A|GAME_B)||touch||start){if((unsigned)(a->dialoguePage+1)*180<strlen(a->dialogue))a->dialoguePage++;else ResolveDialogue(a);}return;}
 if(a->mode==ADV_STARTER){if(down&GAME_LEFT)a->cursor=(a->cursor+2)%3;if(down&GAME_RIGHT)a->cursor=(a->cursor+1)%3;if(touch&&ty>=60&&ty<180){a->cursor=Min(tx/107,2);down|=GAME_A;}if(down&GAME_A){static const unsigned dex[3]={387,390,393};a->save.partyCount=1;Adventure_MakeMon(&a->save.party[0],dex[a->cursor],5);a->save.starter=a->cursor;Mark(a->save.seen,dex[a->cursor]);Mark(a->save.caught,dex[a->cursor]);Complete(a);a->mode=ADV_FIELD;Say(a,"Your starter joined you! Barry is ready for your first live battle. Speak to him at the marker. B dodges while moving and guards while standing.",0);}return;}
 if(a->mode==ADV_FIELD){if(start){a->mode=ADV_CAMP;a->cursor=0;}if(down&GAME_A)Adventure_Interact(a);if(down&GAME_X){a->mode=ADV_MAP;a->cursor=a->save.map;}if(down&GAME_Y){a->mode=ADV_PARTY;a->cursor=0;}if(touch&&ty>=196){if(tx<105)Adventure_Interact(a);else if(tx<212){a->mode=ADV_PARTY;a->cursor=0;}else{a->mode=ADV_CAMP;a->cursor=0;}}return;}
 if(a->mode==ADV_CAMP){unsigned rows=Adventure_CreatorRows(&a->save.appearance);if(down&GAME_UP)a->cursor=(a->cursor+rows-1)%rows;if(down&GAME_DOWN)a->cursor=(a->cursor+1)%rows;if(touch&&ty>=30&&ty<219){a->cursor=(ty-30)/21;down|=GAME_A;}if(down&GAME_A)Camp(a,a->cursor);if(start||(down&GAME_B))a->mode=ADV_FIELD;return;}
 if(a->mode==ADV_PARTY){if(!a->save.partyCount){if(down&GAME_B||start||touch)a->mode=ADV_FIELD;return;}if(down&GAME_UP)a->cursor=(a->cursor+a->save.partyCount-1)%a->save.partyCount;if(down&GAME_DOWN)a->cursor=(a->cursor+1)%a->save.partyCount;if(touch&&ty>=32&&ty<194){i=(ty-32)/27;if(i<a->save.partyCount)a->cursor=i;}if(down&GAME_A){AdventureMon tmp=a->save.party[0];a->save.party[0]=a->save.party[a->cursor];a->save.party[a->cursor]=tmp;a->needsSave=1;Notice(a,"Lead Pokemon and follower updated.");}if(down&GAME_X){AdventureMon *m=&a->save.party[a->cursor];if(a->save.potions&&m->hp<Adventure_Config(m).hp){a->save.potions--;m->hp=Min(m->hp+40,Adventure_Config(m).hp);a->needsSave=1;Notice(a,"Potion restored40 HP.");}}if((down&GAME_B)||start)a->mode=ADV_FIELD;return;}
 if(a->mode==ADV_PC){if(a->save.boxCount){if(down&GAME_UP)a->cursor=(a->cursor+a->save.boxCount-1)%a->save.boxCount;if(down&GAME_DOWN)a->cursor=(a->cursor+1)%a->save.boxCount;if(down&GAME_A){if(a->save.partyCount<6){a->save.party[a->save.partyCount++]=a->save.box[a->cursor];memmove(&a->save.box[a->cursor],&a->save.box[a->cursor+1],(a->save.boxCount-a->cursor-1)*sizeof(AdventureMon));a->save.boxCount--;a->cursor=0;}else{AdventureMon tmp=a->save.party[0];a->save.party[0]=a->save.box[a->cursor];a->save.box[a->cursor]=tmp;}a->needsSave=1;Notice(a,"PC transfer complete.");}if((down&GAME_X)&&(in.held&GAME_L)){memmove(&a->save.box[a->cursor],&a->save.box[a->cursor+1],(a->save.boxCount-a->cursor-1)*sizeof(AdventureMon));a->save.boxCount--;a->cursor=0;a->needsSave=1;Notice(a,"Boxed Pokemon released.");}}if((down&GAME_Y)&&a->save.partyCount>1&&a->save.boxCount<ADVENTURE_BOX){a->save.box[a->save.boxCount++]=a->save.party[0];memmove(&a->save.party[0],&a->save.party[1],(a->save.partyCount-1)*sizeof(AdventureMon));a->save.partyCount--;a->needsSave=1;Notice(a,"Lead Pokemon deposited in PC.");}if((down&GAME_B)||start)a->mode=a->platinum.active?ADV_PT_MENU:ADV_CAMP;return;}
 if(a->mode==ADV_MAP){if(down&GAME_UP)a->cursor=(a->cursor+ADVENTURE_MAPS-1)%ADVENTURE_MAPS;if(down&GAME_DOWN)a->cursor=(a->cursor+1)%ADVENTURE_MAPS;if(touch&&ty>=32&&ty<212){unsigned first=(a->cursor/9)*9;unsigned idx=first+(ty-32)/20;if(idx<ADVENTURE_MAPS)a->cursor=idx;}if(down&GAME_L)a->cursor=(a->cursor+ADVENTURE_MAPS-9)%ADVENTURE_MAPS;if(down&GAME_R)a->cursor=(a->cursor+9)%ADVENTURE_MAPS;if(down&GAME_A)Adventure_Travel(a,a->cursor,1);if((down&GAME_B)||start)a->mode=ADV_FIELD;return;}
}
