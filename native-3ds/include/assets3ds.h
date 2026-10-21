#ifndef ARENA_ASSETS_3DS_H
#define ARENA_ASSETS_3DS_H
#include <stdint.h>
#include "arena/combat.h"

typedef struct {
    uint16_t image;
    int16_t originX,originY;
    uint16_t width,height;
} ArenaSpriteFrame;
typedef struct {
    uint16_t count,totalTicks,hitTick;
    const uint8_t *durations;
    const ArenaSpriteFrame *frames; /* direction-major, eight directions */
} ArenaSpriteAnimation;
typedef struct {
    unsigned dex;
    const char *name,*texturePath;
    float scale;
    unsigned style; /* 0 agile,1 grounded,2 flying,3 heavy */
    CombatFighterConfig config;
    ArenaSpriteAnimation animation[5];
} ArenaSpecies;
extern const ArenaSpecies gSpecies[];
extern const unsigned gSpeciesCount;
const ArenaSpecies *ArenaSpecies_Find(unsigned dex);
#endif
