#ifndef PLATINUM_ARENA_ASSETS_H
#define PLATINUM_ARENA_ASSETS_H
#include <stdint.h>

#define ASSET_ANIM_COUNT 5
#define ASSET_ANIM_IDLE 0
#define ASSET_ANIM_WALK 1
#define ASSET_ANIM_SHOOT 2
#define ASSET_ANIM_ATTACK 3
#define ASSET_ANIM_SPECIAL 4

typedef struct {
    uint32_t offset,size;
    uint16_t frames,total_ticks,hit_tick;
    const uint8_t *durations;
    int8_t ground[8];
} AssetAnimation;
typedef struct {
    const char *name;
    uint16_t dex,palette[16];
    uint8_t hp,attack,defense,speed,sp_attack,sp_defense,type1,type2;
    AssetAnimation animations[ASSET_ANIM_COUNT];
} AssetSpecies;
extern const AssetSpecies asset_species[];
extern const unsigned asset_species_count;
extern const uint32_t asset_pack_size;
extern const uint8_t asset_species_animation_style[];
const AssetSpecies *ArenaAssets_FindSpecies(unsigned dex,unsigned form);
/* Zero denotes an authentic directional sheet. Native stills use their
 * maximum opaque front/back extent, excluding transparent canvas padding. */
unsigned ArenaAssets_BodyExtent(unsigned dex,unsigned form);
unsigned ArenaAssets_DisplaySize(unsigned dex,unsigned form,unsigned nominal);
/* Native 32x32 4bpp source effects: props[0..14], flame[15..22],
 * guard[23..30], spark[31]. Sixteen colors per frame. */
extern const uint8_t arena_ui_healthbox[2][2400];
extern const uint8_t arena_effect_tiles[35][512];
extern const uint16_t arena_effect_palettes[35][16];
#endif
