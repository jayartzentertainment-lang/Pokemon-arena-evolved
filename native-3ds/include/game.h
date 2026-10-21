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
typedef struct {
    uint16_t hp,cooldown[4];
    uint8_t pp[4],stamina;
    uint32_t status,storedAt;
} GamePartySlot;
typedef struct {
    Combat combat;
    GamePartySlot party[2][4];
    uint8_t binding[4],doubles,winner,rAge,rUsed;
    uint8_t faintTicks[4],deployTicks[4];
    uint32_t ticks,switches;
} ArenaGame;

void Game_Start(ArenaGame *game,int doubles);
void Game_Tick(ArenaGame *game,GameInput input);
int Game_Switch(ArenaGame *game,unsigned actor);
unsigned Game_Remaining(const ArenaGame *game,unsigned team);
const ArenaSpecies *Game_ActorSpecies(const ArenaGame *game,unsigned actor);
int Game_TouchMove(unsigned x,unsigned y);
int Game_TouchCommand(unsigned x,unsigned y);
#endif
