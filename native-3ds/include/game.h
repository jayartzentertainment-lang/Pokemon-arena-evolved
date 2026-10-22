#ifndef ARENA_NATIVE_GAME_H
#define ARENA_NATIVE_GAME_H
#include "arena/combat.h"
#include "assets3ds.h"

enum GameButton {
    GAME_UP=1u<<0,GAME_DOWN=1u<<1,GAME_LEFT=1u<<2,GAME_RIGHT=1u<<3,
    GAME_Y=1u<<4,GAME_A=1u<<5,GAME_X=1u<<6,GAME_B=1u<<7,
    GAME_L=1u<<8,GAME_R=1u<<9,GAME_SELECT=1u<<10
};
typedef struct {
    uint16_t held,pressed;
    int16_t axisX,axisY; /* Circle Pad, screen-space y; zero means D-pad. */
    int8_t touchMove;
    uint8_t switchPressed;
} GameInput;
enum GameAimMode { GAME_AIM_LOCK, GAME_AIM_FREE };
typedef struct {
    int32_t x,y; /* World pixels, independent of the camera. */
    int16_t freeX,freeY;
    uint8_t mode,target,valid,blocked;
    uint16_t range;
} GameAim;
typedef struct {
    uint16_t hp,cooldown[4],naturalCooldown;
    uint8_t pp[4],stamina;
    uint32_t status,storedAt;
} GamePartySlot;
typedef struct {
    Combat combat;
    GamePartySlot party[2][6];
    CombatFighterConfig configs[2][6];
    uint8_t partyCount[2],levels[2][6];
    uint8_t binding[4],doubles,winner,rAge,rUsed,participated;
    uint8_t faintTicks[4],deployTicks[4];
    uint32_t ticks,switches;
    GameAim aim;
} ArenaGame;

void Game_Start(ArenaGame *game,int doubles);
void Game_StartParty(ArenaGame *game,const CombatFighterConfig configs[2][6],const uint8_t counts[2],const GamePartySlot state[2][6],int doubles);
void Game_Tick(ArenaGame *game,GameInput input);
int Game_Switch(ArenaGame *game,unsigned actor);
unsigned Game_Remaining(const ArenaGame *game,unsigned team);
const ArenaSpecies *Game_ActorSpecies(const ArenaGame *game,unsigned actor);
int Game_TouchMove(unsigned x,unsigned y);
int Game_TouchCommand(unsigned x,unsigned y);
void Game_MoveRect(unsigned slot,int *x,int *y,int *width,int *height);
/* 0 switch, 1 ball/lab mode, 2 pause, 3 next target, 4 lock/free toggle. */
void Game_CommandRect(unsigned command,int *x,int *y,int *width,int *height);
void Game_AimTick(ArenaGame *game,GameInput input);
void Game_CycleTarget(ArenaGame *game);
#endif
