#ifndef ARENA_NATIVE_RENDER_3DS_H
#define ARENA_NATIVE_RENDER_3DS_H
#include "game.h"
#include "adventure.h"
typedef struct ArenaView ArenaView;
ArenaView *View_Create(void);
void View_Destroy(ArenaView *view);
void View_Draw(ArenaView *view,const ArenaGame *game,float alpha,int paused,
               float fps,float cpuMs,float gpuMs,int audioReady);
void View_DrawApp(ArenaView *view,const Adventure *app,float alpha,int paused,float fps,float cpuMs,float gpuMs,int audioReady);
unsigned View_TextureLoads(const ArenaView *view);
void View_SetQuality(ArenaView *view,unsigned high);
#endif
