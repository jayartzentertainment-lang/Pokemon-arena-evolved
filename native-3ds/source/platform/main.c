#include <3ds.h>
#include <citro2d.h>
#include "adventure.h"
#include "render3ds.h"
#include "audio3ds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
static Adventure app;
static const char *savePath="sdmc:/3ds/platinum-arena/adventure-v1.sav";
static uint16_t Buttons(u32 keys){uint16_t out=0;static const u32 native[]={KEY_DUP,KEY_DDOWN,KEY_DLEFT,KEY_DRIGHT,KEY_Y,KEY_A,KEY_X,KEY_B,KEY_L,KEY_R,KEY_SELECT};unsigned i;for(i=0;i<11;i++)if(keys&native[i])out|=(uint16_t)(1u<<i);return out;}
static int Failure(const char *message){consoleInit(GFX_BOTTOM,0);printf("%s\n\nSTART exits.\n",message);while(aptMainLoop()){hidScanInput();if(hidKeysDown()&KEY_START)break;gspWaitForVBlank();}return 1;}
static void SavePending(FILE *log){if(!app.needsSave||app.mode==ADV_BATTLE||app.mode==ADV_ENCOUNTER)return;app.needsSave=0;if(Adventure_SaveWrite(savePath,&app.save)){app.hasSave=1;app.checkpoint=app.save;if(strstr(app.message,"Saving")){snprintf(app.message,sizeof(app.message),"Adventure saved.");app.messageTicks=180;}}else{snprintf(app.message,sizeof(app.message),"SAVE FAILED: check SD card free space.");app.messageTicks=360;if(log){fprintf(log,"save_failed step=%u map=%u\n",app.save.step,app.save.map);fflush(log);}}}
int main(int argc,char **argv){ArenaView *view;GameInput pending={0};u64 previous,now,statsAt;unsigned accumulator=0,frames=0,paused=0,steps,touchCapture=0,quality=1,lastMode=255,lastStep=255,captureChord=0;float fps=0,cpu=0,gpu=0;int audio,loaded;FILE *log=0;(void)argc;(void)argv;gfxInitDefault();gfxSet3D(false);osSetSpeedupEnable(false);if(R_FAILED(romfsInit())){Failure("RomFS could not be opened.");gfxExit();return 1;}if(!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE)||!C2D_Init(4096)){Failure("Native graphics initialization failed.");romfsExit();gfxExit();return 1;}C2D_Prepare();view=View_Create();if(!view){Failure("Missing converted atlases. Run both asset preparation tools.");C2D_Fini();C3D_Fini();romfsExit();gfxExit();return 1;}mkdir("sdmc:/3ds",0777);mkdir("sdmc:/3ds/platinum-arena",0777);log=fopen("sdmc:/3ds/platinum-arena/session.log","w");audio=Audio_Init();Adventure_Init(&app);loaded=Adventure_SaveRead(savePath,&app.save);app.hasSave=!!loaded;if(loaded)app.checkpoint=app.save;else{FILE *existing=fopen(savePath,"rb");app.loadError=existing!=0;if(existing)fclose(existing);}if(loaded==2){snprintf(app.message,sizeof(app.message),"Recovered your adventure from backup.");app.messageTicks=300;}pending.touchMove=-1;if(log){fprintf(log,"Native3DS0.3.0: condensed Sinnoh campaign,493 directional base species; audio=%d save=%d\n",audio,loaded);fflush(log);}previous=statsAt=osGetTime();while(aptMainLoop()){
 u32 down,held;circlePosition circle,aim;touchPosition touch={0};int oldMode=app.mode;
 hidScanInput();down=hidKeysDown();held=hidKeysHeld();hidCircleRead(&circle);hidCstickRead(&aim);if(held&KEY_TOUCH)hidTouchRead(&touch);if((down&KEY_START)&&(held&KEY_SELECT))break;
 pending.held=Buttons(held);pending.pressed|=Buttons(down);pending.axisX=circle.dx;pending.axisY=-circle.dy;pending.aimX=aim.dx;pending.aimY=-aim.dy;pending.aimActive=abs(aim.dx)>18||abs(aim.dy)>18;
 if(app.mode==ADV_BATTLE){
  if(paused&&(down&KEY_X)){quality=!quality;View_SetQuality(view,quality);}
  if(app.battle.winner&&(down&(KEY_START|KEY_A|KEY_B))){Adventure_FinishBattle(&app,0);paused=0;}
  else if(paused&&(down&KEY_B)){Adventure_FinishBattle(&app,1);paused=0;}
  else if(down&KEY_START){paused=!paused;accumulator=0;pending.pressed=0;}
  if((held&(KEY_L|KEY_R))==(KEY_L|KEY_R)&&(down&(KEY_L|KEY_R))){Adventure_Catch(&app);captureChord=1;}
  if(captureChord){app.battle.rAge=0;app.battle.rUsed=1;pending.held&=~(GAME_L|GAME_R);pending.pressed&=~(GAME_L|GAME_R);if(!(held&(KEY_L|KEY_R)))captureChord=0;}
  if(down&KEY_TOUCH){int command;touchCapture=Game_TouchAim(touch.px,touch.py)?1:2;pending.touchMove=(int8_t)Game_TouchMove(touch.px,touch.py);command=Game_TouchCommand(touch.px,touch.py);if(command==0)pending.switchPressed=1;if(command==1){if(app.battleKind==ADV_LAB_BATTLE){Adventure_StartLab(&app,!app.battle.doubles);paused=0;accumulator=0;}else Adventure_Catch(&app);}if(command==2){paused=!paused;accumulator=0;}if(command==3)Game_CycleTarget(&app.battle);}
  if((held&KEY_TOUCH)&&touchCapture==1){pending.aimX=(int)touch.px-248;pending.aimY=(int)touch.py-126;pending.aimActive=1;}
 }else {Adventure_Input(&app,pending,!!(down&KEY_START),touch.px,touch.py,!!(down&KEY_TOUCH));pending.pressed=0;}
 if(!(held&KEY_TOUCH))touchCapture=0;
 if(oldMode!=app.mode){captureChord=0;pending.pressed=0;pending.touchMove=-1;pending.switchPressed=0;accumulator=0;paused=0;}
 now=osGetTime();{u64 delta=now-previous;if(delta>100)delta=100;accumulator+=(unsigned)delta*60;}
 for(steps=0;accumulator>=1000&&steps<6;steps++){
  if(app.mode==ADV_BATTLE&&!paused&&!app.battle.winner){Game_Tick(&app.battle,pending);Audio_Observe(&app.battle);}
  Adventure_Tick(&app,pending);accumulator-=1000;pending.pressed=0;pending.touchMove=-1;pending.switchPressed=0;
 }
 previous=now;SavePending(log);View_DrawApp(view,&app,accumulator/1000.0f,paused,fps,cpu,gpu,audio);cpu=C3D_GetProcessingTime();gpu=C3D_GetDrawingTime();frames++;
 if(log&&(lastMode!=app.mode||lastStep!=app.save.step)){fprintf(log,"state mode=%u story=%u map=%u trainer=%s party=%u winner=%u\n",app.mode,app.save.step,app.save.map,app.save.name,app.save.partyCount,app.battle.winner);fflush(log);lastMode=app.mode;lastStep=app.save.step;}
 if(now-statsAt>=1000){fps=frames*1000.0f/(now-statsAt);frames=0;statsAt=now;if(log){fprintf(log,"sample mode=%u step=%u map=%u x=%ld y=%ld ticks=%lu battle=%lu hits=%lu winner=%u fps=%.2f cpu=%.3f gpu=%.3f\n",app.mode,app.save.step,app.save.map,(long)(app.save.x/256),(long)(app.save.y/256),(unsigned long)app.ticks,(unsigned long)app.battle.ticks,(unsigned long)app.battle.combat.hits,app.battle.winner,fps,cpu,gpu);fflush(log);}}
 }
 SavePending(log);if(log){fprintf(log,"exit step=%u map=%u party=%u texture_loads=%u\n",app.save.step,app.save.map,app.save.partyCount,View_TextureLoads(view));fclose(log);}if(audio)Audio_Stop();View_Destroy(view);C2D_Fini();C3D_Fini();romfsExit();gfxExit();return 0;
}
