/* Platinum action battle controller, adapted from accepted Arena DS 0.9.1.
 * Portable helper provenance is recorded in src/arena/vendor/PROVENANCE.md. */
#ifndef PLATINUM_ARENA_COMBAT_H
#define PLATINUM_ARENA_COMBAT_H
#include <stdint.h>
#define COMBAT_FIGHTERS 4
#define COMBAT_PROJECTILES 12
#define COMBAT_EFFECTS 32
#define COMBAT_STAMINA_MAX 100
#define COMBAT_BEAM_CLASHES 2
#define COMBAT_CLASH_PRESSURE_TICKS 180
#define COMBAT_CLASH_BURST_TICKS 18
#define COMBAT_CLASH_PRESSURE_LIMIT 96
/* PMD sprite-sheet order, as in the supplied Arena source assets. */
enum CombatDirection { COMBAT_S, COMBAT_SE, COMBAT_E, COMBAT_NE,
    COMBAT_N, COMBAT_NW, COMBAT_W, COMBAT_SW };
/* Simulation commands: L=PP-free natural; A=selected learned move;
 * ALT=special/status selection; X=legacy direct slot 0; Y=aim toggle.
 * Hardware assignments and shoulder chords live in controls.c. */
enum CombatButton { COMBAT_UP=1u<<0, COMBAT_DOWN=1u<<1,
    COMBAT_LEFT=1u<<2, COMBAT_RIGHT=1u<<3, COMBAT_A=1u<<4,
    COMBAT_B=1u<<5, COMBAT_X=1u<<6, COMBAT_Y=1u<<7,
    COMBAT_L=1u<<8, COMBAT_R=1u<<9, COMBAT_ALT=1u<<10,
    COMBAT_CYCLE_LIGHT=1u<<11,COMBAT_CYCLE_HEAVY=1u<<12,
    COMBAT_COMMAND=1u<<13 };
enum CombatStat { COMBAT_STAT_ATTACK,COMBAT_STAT_DEFENSE,COMBAT_STAT_SP_ATTACK,
    COMBAT_STAT_SP_DEFENSE,COMBAT_STAT_SPEED,COMBAT_STAT_ACCURACY,
    COMBAT_STAT_EVASION,COMBAT_STAT_COUNT };
enum CombatStatus { COMBAT_STATUS_NONE,COMBAT_STATUS_BURN,COMBAT_STATUS_POISON,
    COMBAT_STATUS_BAD_POISON,COMBAT_STATUS_PARALYSIS,COMBAT_STATUS_SLEEP,
    COMBAT_STATUS_FREEZE };
enum CombatTactic { COMBAT_TACTIC_ASSIST,COMBAT_TACTIC_DEFEND,COMBAT_TACTIC_FOCUS };
enum CombatAnimation { COMBAT_IDLE,COMBAT_WALK,COMBAT_SHOOT,
    COMBAT_ATTACK,COMBAT_SPECIAL };
enum CombatEffectKind { COMBAT_SPARK,COMBAT_RING,COMBAT_TRAIL,
    COMBAT_BLOCK,COMBAT_IMMUNE,COMBAT_CLASH,COMBAT_DASH_TRAIL,COMBAT_CONTACT };
enum CombatImpactKind { COMBAT_IMPACT_NONE,COMBAT_IMPACT_BLOCK,
    COMBAT_IMPACT_CRITICAL,COMBAT_IMPACT_CLASH };
enum CombatPhase { COMBAT_READY,COMBAT_WINDUP,COMBAT_ACTIVE,COMBAT_RECOVERY };
enum CombatClashPhase { COMBAT_CLASH_NONE,COMBAT_CLASH_PRESSURE,COMBAT_CLASH_BURST };
typedef struct { uint16_t held,pressed; } CombatInput;
typedef struct {
    uint16_t species,hp,attack,defense,speed,moves[4];
    uint8_t types[2]; /* Gen IV type IDs (Normal=0, Fire=10); 255=no second type. */
    uint16_t spAttack,spDefense;
} CombatFighterConfig;
typedef struct {
    uint16_t move;
    uint8_t type,power,pp,category; /* Native Gen IV physical=0, special=1, status=2. */
    const char *name;
} CombatMoveInfo;
typedef struct {
    int32_t x,y,previousX,previousY,vx,vy; /* positions and velocities in Q8 */
    uint32_t actionSerial; /* Successful starts only; includes PP-free fallback. */
    uint32_t releaseSerial,releasedActionSerial,practiceActionSerial;
    uint16_t species,hp,maxHp,attack,defense,speed,moves[4];
    uint16_t animTick,actionMove,actionTick,actionDistance,releaseMove,walkDistance;
    uint8_t types[2],pp[4],selected,direction,animation,stamina;
    uint8_t guardTimer,dashTimer,hurtTimer,dashCooldown,regenDelay;
    uint8_t guardHeld,guardAge,guardExhausted;
    uint8_t phase,hitMask,terrainMask,basicAttack,buffered,bufferTimer,bufferedSlot;
    uint8_t dashDirection,attackDirection,aiTimer,aiMove,aiCooldown,actionSlot;
    uint8_t tapTimer[4],tapReleased,lastDirections;
    int16_t aiGoalX,aiGoalY;
    uint16_t spAttack,spDefense;
    uint16_t attackBoost,defenseBoost,speedBoost,attackDown,defenseDown,slowTimer;
    int8_t stages[COMBAT_STAT_COUNT]; /* Persistent until replacement; -6..6. */
    uint16_t cooldown[4],statusTicks,statusClock,confusionTicks,statusLock;
    uint16_t lightScreenTicks;
    uint8_t status,toxicCounter,lightChoice,specialSlot;
} CombatFighter;
typedef struct {
    int32_t x,y,previousX,previousY;
    int16_t vx,vy;
    uint16_t move,color;
    uint8_t owner,life,radius,type,visual,basic;
} CombatProjectile;
typedef struct {
    int32_t x,y;
    int16_t vx,vy;
    uint16_t color,move; /* Native presentation: optional confirmed-hit move. */
    uint8_t life,maxLife,radius,kind,direction;
} CombatEffect;
typedef struct {
    int32_t x,y,originX,originY; /* Shared contact/origin in world Q8. */
    int16_t pressure; /* -LIMIT..LIMIT; positive pushes toward actor[1]. */
    uint32_t actionSerial[2];
    uint16_t move[2];
    uint8_t actor[2],age,phase,tapCooldown[2],winner;
    int16_t powerDelta; /* Frozen stat/power/type/angle/timing contest. */
} CombatBeamClash;
typedef struct {
    CombatFighter fighters[COMBAT_FIGHTERS];
    CombatProjectile projectiles[COMBAT_PROJECTILES];
    CombatEffect effects[COMBAT_EFFECTS];
    CombatBeamClash beamClashes[COMBAT_BEAM_CLASHES];
    uint32_t frame,hits,blocked,dodged,blastSerial,random,clashes,criticals;
    /* Presentation events never change collision or consume render timing.
     * Actor is the defender (255 for a clash); coordinates are world Q8. */
    uint32_t impactSerial;
    int32_t impactX,impactY;
    uint8_t impactActor,impactKind,impactTimer;
    uint8_t hitstop,winner,lockAim,aiEnabled,effectCursor,fighterCount,controlActor,clashPractice,tactic;
    uint16_t controlMagnitude; /* Q8 Circle Pad magnitude; digital fallback unchanged. */
    uint8_t analogInput;
} Combat;
/* One live match owns the imported navigation/physics module globals. */
void Combat_Init(Combat *combat,const CombatFighterConfig configs[2]);
/* Four simultaneous actors: player 0 + AI ally 2 versus AI opponents 1 + 3.
 * Combat_Init remains a two-actor match. Winner 1/2 denotes the surviving team. */
void Combat_InitTeams(Combat *combat,const CombatFighterConfig configs[COMBAT_FIGHTERS]);
/* Transfer player input to a living allied actor without changing actor IDs,
 * positions, HP/PP, projectiles or party bindings. Other actors retain AI. */
int Combat_SetControlledFighter(Combat *combat,unsigned side);
/* Nearest living opposing actor, or COMBAT_FIGHTERS when none remain. */
unsigned Combat_TargetFighter(const Combat *combat,unsigned side);
/* Replace one team member without restarting the arena or its opponent.
 * Position/facing persist; the incoming member starts ready with fresh stats,
 * stamina and configured PP. Its outgoing projectiles and hitstop are cleared.
 * Caller may restore the incoming member's saved HP/PP after this operation. */
int Combat_ReplaceFighter(Combat *combat,unsigned side,const CombatFighterConfig *config);
void Combat_Tick(Combat *combat,CombatInput input);
/* Lab-only synchronization, off after every match initialization. Opposing AI
 * and an AI ally hold position and answer the player's beam with their own. */
void Combat_SetClashPractice(Combat *combat,int enabled);
/* Authored sprite timing follows combat phases; held beams do not restart
 * their windup. Returns a tick in [0,totalTicks), never an asset pointer. */
unsigned Combat_AnimationTick(const CombatFighter *fighter,unsigned totalTicks,unsigned hitTick);
unsigned Combat_PhaseTick(const CombatFighter *fighter);
unsigned Combat_PhaseDuration(const CombatFighter *fighter);
void Combat_SetAnalogMagnitude(Combat *combat,unsigned magnitude,int enabled);
/* During pressure, both source beams end at the same moving contact point.
 * Returns zero for an interrupted or finished pair. Coordinates may be NULL. */
int Combat_BeamContact(const Combat *combat,unsigned side,int32_t *x,int32_t *y);
/* Restore native current PP, including PP Up bonuses. Invalid move slots stay 0. */
void Combat_SetMovePP(Combat *combat,unsigned side,const uint8_t pp[4]);
/* Queue one of all four learned slots; PP is spent only at a legal start. */
int Combat_QueueMove(Combat *combat,unsigned side,unsigned slot);
int Combat_ChangeStage(Combat *combat,unsigned side,unsigned stat,int delta);
/* Gen IV nonvolatile bitmask: sleep 0..2, poison3, burn4, freeze5,
 * paralysis6, toxic7. Confusion and stat stages are volatile. */
void Combat_SetNativeStatus(Combat *combat,unsigned side,uint32_t status);
uint32_t Combat_GetNativeStatus(const Combat *combat,unsigned side);
uint16_t Combat_NaturalMove(const CombatFighter *fighter);
const CombatMoveInfo *Combat_GetMoveInfo(uint16_t move);
uint16_t Combat_TypeColor(uint8_t type);
/* Gen IV chart, Q8 multiplier (0,64,128,256,512,1024). */
uint16_t Combat_TypeMultiplier(uint8_t attackType,uint8_t defense1,uint8_t defense2);
#endif
