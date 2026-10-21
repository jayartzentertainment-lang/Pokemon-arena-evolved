#include <3ds.h>
#include <citro2d.h>
#include "game.h"
#include "render3ds.h"
#include "audio3ds.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static ArenaGame game;
static uint16_t Buttons(u32 keys)
{
    uint16_t out=0;
    static const u32 native[]={KEY_DUP,KEY_DDOWN,KEY_DLEFT,KEY_DRIGHT,
        KEY_Y,KEY_A,KEY_X,KEY_B,KEY_L,KEY_R,KEY_SELECT};
    unsigned i;
    for(i=0;i<11;i++)if(keys&native[i])out|=(uint16_t)(1u<<i);
    return out;
}
static int Failure(const char *message)
{
    consoleInit(GFX_BOTTOM,0);printf("%s\n\nSTART exits.\n",message);
    while(aptMainLoop()){
        hidScanInput();if(hidKeysDown()&KEY_START)break;
        gspWaitForVBlank();
    }
    return 1;
}
int main(int argc,char **argv)
{
    ArenaView *view;
    GameInput pending={0,0,0,0,-1,0};
    u64 previous,now,statsAt;
    unsigned accumulator=0,frames=0,paused=0,steps;
    float fps=0,cpu=0,gpu=0;
    int audio;
    FILE *log=0;
    (void)argc;(void)argv;
    gfxInitDefault();gfxSet3D(false);osSetSpeedupEnable(false);
    if(R_FAILED(romfsInit())){Failure("RomFS could not be opened.");gfxExit();return 1;}
    if(!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE)||!C2D_Init(2048)){
        Failure("Native graphics initialization failed.");romfsExit();gfxExit();return 1;
    }
    C2D_Prepare();view=View_Create();
    if(!view){Failure("Missing converted atlases. Run prepare_assets.py then make.");
        C2D_Fini();C3D_Fini();romfsExit();gfxExit();return 1;}
    mkdir("sdmc:/3ds",0777);mkdir("sdmc:/3ds/platinum-arena",0777);
    log=fopen("sdmc:/3ds/platinum-arena/session.log","w");
    audio=Audio_Init();Game_Start(&game,0);
    if(log){fprintf(log,"Native3DS prototype0.1.0; baseline original3DS; audio=%d\n",audio);fflush(log);}
    previous=statsAt=osGetTime();
    while(aptMainLoop()){
        u32 down,held;circlePosition circle;
        hidScanInput();down=hidKeysDown();held=hidKeysHeld();hidCircleRead(&circle);
        if((down&KEY_START)&&(held&KEY_SELECT))break;
        if(down&KEY_START){
            if(game.winner){Game_Start(&game,game.doubles);paused=0;}
            else paused=!paused;
            accumulator=0;pending.pressed=0;
        }
        pending.held=Buttons(held);pending.pressed|=Buttons(down);
        pending.axisX=circle.dx;pending.axisY=-circle.dy;
        if(down&KEY_TOUCH){
            touchPosition touch;int command;
            hidTouchRead(&touch);pending.touchMove=(int8_t)Game_TouchMove(touch.px,touch.py);
            command=Game_TouchCommand(touch.px,touch.py);
            if(command==0)pending.switchPressed=1;
            if(command==1){Game_Start(&game,!game.doubles);paused=0;accumulator=0;}
            if(command==2){paused=!paused;accumulator=0;}
        }
        now=osGetTime();
        if(!paused&&!game.winner){
            u64 delta=now-previous;if(delta>100)delta=100;
            accumulator+=(unsigned)delta*60;
            for(steps=0;accumulator>=1000&&steps<6;steps++){
                Game_Tick(&game,pending);Audio_Observe(&game);accumulator-=1000;
                pending.pressed=0;pending.touchMove=-1;pending.switchPressed=0;
            }
        }else {accumulator=0;pending.pressed=0;pending.touchMove=-1;pending.switchPressed=0;}
        previous=now;
        View_Draw(view,&game,accumulator/1000.0f,paused,fps,cpu,gpu,audio);
        cpu=C3D_GetProcessingTime();gpu=C3D_GetDrawingTime();frames++;
        if(now-statsAt>=1000){
            fps=frames*1000.0f/(now-statsAt);frames=0;statsAt=now;
            if(log){
                const CombatFighter *f=&game.combat.fighters[game.combat.controlActor];
                fprintf(log,"sample ticks=%lu mode=%u actor=%u dir=%u x=%ld y=%ld hp=%u pp=%u,%u,%u,%u phase=%u hits=%lu switches=%lu winner=%u fps=%.2f cpu_ms=%.3f gpu_ms=%.3f\n",
                    (unsigned long)game.ticks,game.doubles,f->species,f->direction,
                    (long)(f->x/256),(long)(f->y/256),f->hp,
                    f->pp[0],f->pp[1],f->pp[2],f->pp[3],f->phase,
                    (unsigned long)game.combat.hits,(unsigned long)game.switches,
                    game.winner,fps,cpu,gpu);
                fflush(log);
            }
        }
    }
    if(log){fprintf(log,"ticks=%lu hits=%lu switches=%lu winner=%u texture_loads=%u fps=%.2f cpu_ms=%.3f gpu_ms=%.3f\n",
        (unsigned long)game.ticks,(unsigned long)game.combat.hits,(unsigned long)game.switches,
        game.winner,View_TextureLoads(view),fps,cpu,gpu);fclose(log);}
    if(audio)Audio_Stop();
    View_Destroy(view);C2D_Fini();C3D_Fini();romfsExit();gfxExit();
    return 0;
}
