#ifndef ARENA_NATIVE_ADVENTURE_H
#define ARENA_NATIVE_ADVENTURE_H
#include "game.h"
#include "platinum.h"
#include <stddef.h>
#define ADVENTURE_MAPS 34
#define ADVENTURE_EVENTS 41
#define ADVENTURE_PARTY 6
#define ADVENTURE_BOX 60
#define ADVENTURE_SAVE_BYTES 4096
/* Explicit native2D story state; DS overlays and scripts are not executed. */
enum AdventureMode { ADV_TITLE,ADV_CREATOR,ADV_NAME,ADV_FIELD,ADV_DIALOGUE,ADV_STARTER,ADV_ENCOUNTER,ADV_BATTLE,ADV_CAMP,ADV_PARTY,ADV_MAP,ADV_LAB,ADV_PC,ADV_PT_INTRO,ADV_PT_FIELD,ADV_PT_MENU };
enum AdventureBattle { ADV_STORY_BATTLE,ADV_WILD_BATTLE,ADV_LAB_BATTLE,ADV_PT_BATTLE };
/* Keep legacy indices for saved appearances; new Custom uses the Gen IV bank. */
enum AdventureTrainerBase { ADV_TRAINER_CUSTOM,ADV_TRAINER_RED,ADV_TRAINER_BRENDAN,ADV_TRAINER_GEN4,ADV_TRAINER_BASE_COUNT };
#define ADVENTURE_HAIR_COLOURS 6
#define ADVENTURE_OUTFIT_COLOURS 8
typedef struct { uint8_t body,option[6],follower,base,hairColour,outfitColour; } AdventureAppearance;
typedef struct { uint16_t dex,hp,moves[4];uint8_t pp[4],level;uint32_t xp,status; } AdventureMon;
typedef struct {
    char name[11];AdventureAppearance appearance;
    AdventureMon party[6],box[ADVENTURE_BOX];
    uint8_t partyCount,boxCount,map,step,starter,badges,champion;
    uint16_t balls,potions,money;int32_t x,y;
    uint32_t playTicks,random;uint64_t visited;uint8_t seen[62],caught[62];
} AdventureSave;
typedef struct {
    const char *name;uint8_t requires,linkCount,links[4];uint16_t wild[4];
    uint8_t blocked[768],grass[768];
} AdventureMap;
typedef struct {uint8_t level;uint16_t move;} AdventureLearn;
typedef struct {uint8_t level;uint16_t dex;} AdventureEvolution;
typedef struct {uint8_t stats[6],types[2],catchRate;uint16_t expReward,learnStart,learnCount,evoStart,evoCount;} AdventureDex;
typedef struct {uint16_t dex;uint8_t level;uint16_t moves[4];} AdventureTrainerMon;
typedef struct {
    const char *name,*text;uint8_t map,npc,kind;uint16_t teamStart;
    uint8_t teamCount;int8_t badge;uint8_t level,doubles;
} AdventureEvent;
typedef struct {
    AdventureSave save,checkpoint;ArenaGame battle;Platinum platinum,platinumCheckpoint;uint8_t hasPlatinumSave;
    uint8_t mode,returnMode,hasSave,loadError,cursor,previewDirection;
    uint8_t dialoguePage,dialogueAction,battleKind,battleEvent,caughtBattle,needsSave;
    uint8_t keyboard,selectedParty,selectedBox,labSide,labClash,starterChoice;
    uint16_t labDex[2],transition,encounterCooldown,messageTicks;
    uint8_t facing,moving,walkFrame,newGameConfirm,menuPage;
    uint32_t ticks,walkDistance;int32_t previousX,previousY,followerX,followerY;
    int32_t trailX[32],trailY[32];uint8_t trailIndex;
    char message[120],dialogue[720];
} Adventure;
extern const AdventureMap gAdventureMaps[ADVENTURE_MAPS];
extern const AdventureEvent gAdventureEvents[ADVENTURE_EVENTS];
extern const AdventureTrainerMon gStoryTeams[];
extern const AdventureDex gAdventureDex[494];
extern const AdventureLearn gAdventureLearn[];
extern const AdventureEvolution gAdventureEvolution[];
extern const uint8_t gAppearanceCounts[6];
extern const uint8_t gGen4AppearanceCounts[6];
extern const char *const *gAppearanceLabels[6];
extern const char *gNpcNames[];
extern const unsigned gNpcCount;
void Adventure_Init(Adventure *app);
void Adventure_New(Adventure *app);
unsigned Adventure_CreatorRows(const AdventureAppearance *appearance);
unsigned Adventure_AppearanceCount(const AdventureAppearance *appearance,unsigned group);
void Adventure_Input(Adventure *app,GameInput input,int start,int touchX,int touchY,int touchDown);
void Adventure_Tick(Adventure *app,GameInput input);
void Adventure_FinishBattle(Adventure *app,int fled);
void Adventure_Catch(Adventure *app);
void Adventure_StartLab(Adventure *app,int doubles);
int Adventure_Travel(Adventure *app,unsigned map,int discoveredOnly);
int Adventure_NextRoad(const Adventure *app);
void Adventure_MakeMon(AdventureMon *mon,unsigned dex,unsigned level);
CombatFighterConfig Adventure_Config(const AdventureMon *mon);
void Adventure_Heal(Adventure *app);
void Adventure_Interact(Adventure *app);
unsigned Adventure_Healthy(const Adventure *app);
unsigned Adventure_DexCount(const uint8_t bits[62]);
const char *Adventure_Objective(const Adventure *app);
int Adventure_SaveEncode(const AdventureSave *save,uint8_t *out,size_t capacity,size_t *written);
int Adventure_SaveDecode(AdventureSave *save,const uint8_t *bytes,size_t size);
int Adventure_SaveWrite(const char *path,const AdventureSave *save);
int Adventure_SaveRead(const char *path,AdventureSave *save);
int Adventure_SaveValid(const AdventureSave *save);
#endif
