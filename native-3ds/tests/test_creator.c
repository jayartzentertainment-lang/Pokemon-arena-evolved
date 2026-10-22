#include "adventure.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static Adventure app;
static void Press(unsigned button){GameInput in={0};in.touchMove=-1;in.pressed=(uint16_t)button;Adventure_Input(&app,in,0,0,0,0);}
static void BlankAndRegional(void)
{
 unsigned i;AdventureAppearance custom;GameInput none={0};none.touchMove=-1;
 Adventure_Init(&app);Adventure_New(&app);
 assert(app.save.appearance.base==ADV_TRAINER_GEN4&&Adventure_CreatorRows(&app.save.appearance)==9);
 for(i=0;i<6;i++)assert(app.save.appearance.option[i]==0);
 /* Custom really changes the chosen supplied hair layer. */
 app.cursor=4;Press(GAME_RIGHT);assert(app.save.appearance.option[3]==1);custom=app.save.appearance;
 /* Selecting Red exposes colours, never a hairstyle or body override. */
 app.cursor=1;Press(GAME_RIGHT);assert(app.save.appearance.base==ADV_TRAINER_RED&&Adventure_CreatorRows(&app.save.appearance)==6);
 app.cursor=3;for(i=0;i<ADVENTURE_HAIR_COLOURS;i++)Press(GAME_RIGHT);
 assert(app.save.appearance.hairColour==0&&app.save.appearance.option[3]==custom.option[3]);
 Press(GAME_RIGHT);assert(app.save.appearance.hairColour==1&&app.save.appearance.body==0);
 app.cursor=4;Press(GAME_RIGHT);assert(app.save.appearance.outfitColour==1&&app.save.appearance.option[2]==custom.option[2]);
 app.cursor=1;Press(GAME_RIGHT);assert(app.save.appearance.base==ADV_TRAINER_BRENDAN);
 app.cursor=3;Press(GAME_LEFT);assert(app.save.appearance.hairColour==0&&app.save.appearance.option[3]==custom.option[3]);
 /* Custom preserves deliberate edits when returning, not a regional overlay. */
 app.cursor=1;Press(GAME_RIGHT);assert(app.save.appearance.base==ADV_TRAINER_GEN4&&app.save.appearance.option[3]==custom.option[3]);
 /* Gen IV-specific layer ranges and both bodies round trip through saves. */
 app.cursor=2;Press(GAME_RIGHT);assert(app.save.appearance.body==1);
 assert(Adventure_SaveValid(&app.save));
 app.cursor=7;for(i=0;i<gGen4AppearanceCounts[4];i++)Press(GAME_RIGHT);
 assert(app.save.appearance.option[4]==0);
 app.cursor=8;Press(GAME_RIGHT);assert(app.save.appearance.follower==0);
 /* Both pointer and physical paths select the same base. */
 Adventure_Input(&app,none,0,240,52,1);assert(app.save.appearance.base==ADV_TRAINER_RED);
 app.cursor=5;Press(GAME_DOWN);assert(app.cursor==0);Press(GAME_UP);assert(app.cursor==5);
}
static void SaveAndMigrate(void)
{
 AdventureSave loaded;uint8_t bytes[ADVENTURE_SAVE_BYTES];size_t n;unsigned base;
 assert(Adventure_SaveRead("tests/fixtures/arena-v1.sav",&loaded)==1);
 assert(!strcmp(loaded.name,"ARENA")&&loaded.appearance.base==ADV_TRAINER_CUSTOM&&!loaded.appearance.hairColour&&!loaded.appearance.outfitColour);
 assert(loaded.partyCount==1&&loaded.party[0].dex==387&&loaded.step==3);
 for(base=ADV_TRAINER_CUSTOM;base<ADV_TRAINER_BASE_COUNT;base++){
  app.save.appearance.base=(uint8_t)base;app.save.appearance.body=0;app.save.appearance.hairColour=5;app.save.appearance.outfitColour=7;
  assert(Adventure_SaveEncode(&app.save,bytes,sizeof(bytes),&n)&&bytes[4]==2);
  assert(Adventure_SaveDecode(&loaded,bytes,n));
  assert(!memcmp(&loaded.appearance,&app.save.appearance,sizeof(loaded.appearance)));
  bytes[n-1]^=1;assert(!Adventure_SaveDecode(&loaded,bytes,n));
 }
 app.save.appearance.base=ADV_TRAINER_GEN4;app.save.appearance.body=1;
 for(unsigned g=0;g<6;g++)app.save.appearance.option[g]=(uint8_t)(gGen4AppearanceCounts[g]-1);
 assert(Adventure_SaveEncode(&app.save,bytes,sizeof(bytes),&n));
 assert(Adventure_SaveDecode(&loaded,bytes,n)&&loaded.appearance.body==1&&loaded.appearance.base==ADV_TRAINER_GEN4);
 assert(!memcmp(&loaded.appearance,&app.save.appearance,sizeof(loaded.appearance)));
 app.save.appearance.option[5]=1;assert(!Adventure_SaveValid(&app.save));app.save.appearance.option[5]=0;
 app.save.appearance.base=ADV_TRAINER_BASE_COUNT;assert(!Adventure_SaveValid(&app.save));
 app.save.appearance.base=ADV_TRAINER_RED;app.save.appearance.hairColour=ADVENTURE_HAIR_COLOURS;assert(!Adventure_SaveValid(&app.save));
}
static void IntroIdentity(void)
{
 GameInput in={0};in.touchMove=-1;Adventure_Init(&app);
 /* Original new game -> shortcut -> blank Custom, not a recoloured Lucas. */
 app.cursor=1;Press(GAME_A);assert(app.mode==ADV_PT_INTRO);
 in.held=GAME_SELECT;Adventure_Input(&app,in,1,0,0,0);assert(app.mode==ADV_CREATOR&&app.platinum.introShortcut);
 assert(app.save.appearance.base==ADV_TRAINER_GEN4);
 for(unsigned i=0;i<6;i++)assert(app.save.appearance.option[i]==0);
 app.cursor=1;Press(GAME_LEFT);assert(app.save.appearance.base==ADV_TRAINER_BRENDAN);
 app.cursor=3;Press(GAME_RIGHT);snprintf(app.save.name,sizeof(app.save.name),"CREATOR");in.held=0;
 Adventure_Input(&app,in,1,0,0,0);assert(app.mode==ADV_PT_FIELD&&app.platinum.gender==0&&app.save.appearance.base==ADV_TRAINER_BRENDAN&&app.save.appearance.hairColour==1);
}
int main(void){BlankAndRegional();SaveAndMigrate();IntroIdentity();puts("PASS creator: supplied Gen IV blank Custom and matching layers, fixed Red/Brendan colour-only controls, pointer input, original intro identity, v2 save/CRC and actual v0.3 migration");return 0;}
