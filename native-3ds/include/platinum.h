#ifndef ARENA_PLATINUM_H
#define ARENA_PLATINUM_H
#include <stdint.h>
#include <stddef.h>
/* Original source IDs are retained. Unknown commands stop the VM at their
 * original file/line; no invented story state or dialogue is substituted. */
#define PT_ACTORS 32
#define PT_STACK 32
#define PT_VARS 1024
#define PT_FLAG_BYTES 1024
#define PT_LOCAL_VARS 32
#define PT_NO_PC 0xffffffffu

typedef struct {const char *op;int32_t arg[6];uint16_t line;uint8_t argc,unresolved;} PtInstruction;
typedef struct {uint16_t id,gfx,flag,script,x,z;uint8_t dir;} PtObject;
typedef struct {uint16_t x,z,dest,warp;} PtWarp;
typedef struct {uint16_t script,x,z,width,length,var,value;} PtTrigger;
typedef struct {uint16_t script,x,z,dir;} PtBackground;
typedef struct {uint16_t var,value,script;} PtFrame;
typedef struct {uint16_t dex;uint8_t level;uint16_t iv;uint8_t form;uint16_t moves[4];} PtTrainerMon;
typedef struct {uint16_t id,offset;uint8_t count,doubles;const char *name;} PtTrainer;
typedef struct {
 uint16_t header,matrix,area,minX,minZ,width,height;
 const char *name,*scriptFile;const PtInstruction *code;uint32_t codeCount;
 const uint32_t *entries;uint16_t entryCount,transition,onload;
 const PtObject *objects;unsigned objectCount;const PtWarp *warps;unsigned warpCount;
 const PtTrigger *triggers;unsigned triggerCount;const PtBackground *backgrounds;unsigned backgroundCount;
 const PtFrame *frames;unsigned frameCount;const uint16_t *terrain;
} PtMap;
typedef struct {int32_t x,z,fromX,fromZ;uint32_t pc;uint16_t ticks,duration;uint8_t dir,visible,lockDirection;uint16_t movementType;} PtActor;
typedef struct {
 uint16_t vars[PT_VARS],locals[PT_LOCAL_VARS];uint8_t flags[PT_FLAG_BYTES];
 uint16_t header,x,z;uint8_t facing,active,intro,introPage,introChoice,creatorDone,introMenu,introHelp,introShortcut;
 uint32_t random,pc,stack[PT_STACK];uint8_t depth,locked,waiting,blocked,messagePage,waitTicks;
 uint16_t message,waitVar,starter,trainer,serviceArg[4],bank;uint8_t service,choice,lastBattleWon,gender,partner,pendingTransition,deferredWarp,runningShoes,runHeld;int32_t faultPC;const char *fault;
 PtActor actors[PT_ACTORS+1],camera;uint8_t cameraActive;PtWarp warps[32];
 char buffers[8][32];char text[4096];
} Platinum;
extern const uint8_t gPtGrowthRates[494];extern const uint32_t gPtExperience[8][101];
extern const PtTrainerMon gPtTrainerMons[];extern const PtTrainer gPtTrainers[];extern const unsigned gPtTrainerCount;
extern const PtMap gPtMaps[];extern const unsigned gPtMapCount;
extern const uint16_t gPtCounterpartGfx[2],gPtRivalGfx;
extern const uint8_t gPtFontWidths[];
unsigned Platinum_Glyph(unsigned codepoint);
extern const char *const gPtMessages[];extern const unsigned gPtMessageCount;
extern const uint16_t gPtIntroMain[],gPtIntroHelp[];extern const unsigned gPtIntroCount;
extern const PtInstruction gPtNewGame[];extern const unsigned gPtNewGameCount;
const PtMap *Platinum_Map(unsigned header);
unsigned Platinum_Value(const Platinum *p,unsigned value);
void Platinum_Set(Platinum *p,unsigned key,unsigned value);
void Platinum_Flag(Platinum *p,unsigned flag,int enabled);
int Platinum_HasFlag(const Platinum *p,unsigned flag);
void Platinum_New(Platinum *p);
int Platinum_Enter(Platinum *p,unsigned header,unsigned x,unsigned z,unsigned facing);
int Platinum_RunEntry(Platinum *p,unsigned entry);
void Platinum_Tick(Platinum *p);
void Platinum_Advance(Platinum *p);
int Platinum_Move(Platinum *p,unsigned direction);
void Platinum_Interact(Platinum *p);
void Platinum_Text(Platinum *p,unsigned message);
int Platinum_SaveWrite(const char *path,const Platinum *p,const uint8_t *profile,size_t size);
int Platinum_SaveRead(const char *path,Platinum *p,uint8_t *profile,size_t capacity,size_t *size);
#endif
