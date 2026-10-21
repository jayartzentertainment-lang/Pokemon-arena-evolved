#ifndef ARENA_PLATINUM_MOVE_DATA_H
#define ARENA_PLATINUM_MOVE_DATA_H

#include "arena/combat.h"
#include "arena/vendor/arena_moves.h"

/* Canonical Platinum IDs: MOVE_NONE = 0 through MOVE_SHADOW_FORCE = 467. */
enum { ARENA_PLATINUM_MOVE_COUNT = 468 };

/* Native status effects are adapted to brief real-time effects. Complex move
 * scripts are not reproduced: unclassified user/side moves guard, while
 * unclassified opposing/field moves slow the target. Splash remains a no-op. */
enum ArenaPlatinumStatusEffect {
    ARENA_STATUS_NONE = 0,
    ARENA_STATUS_GUARD = 1,
    ARENA_STATUS_HEAL = 2,
    ARENA_STATUS_ATTACK_UP = 3,
    ARENA_STATUS_DEFENSE_UP = 4,
    ARENA_STATUS_SPEED_UP = 5,
    ARENA_STATUS_ATTACK_DOWN = 6,
    ARENA_STATUS_DEFENSE_DOWN = 7,
    ARENA_STATUS_SLOW = 8
};

/* Empty and invalid move IDs return NULL. Category is Gen IV, not inferred
 * from type: physical = 0, special = 1, status = 2. */
const CombatMoveInfo *ArenaPlatinum_GetMoveInfo(uint16_t move);
uint8_t ArenaPlatinum_GetStatusEffect(uint16_t move);

/* Generic real-time actions for moves without a hand-authored arena profile.
 * These describe action geometry and timing, not the full original effect. */
const struct ArenaMoveProfile *ArenaPlatinum_GetFallbackProfile(uint16_t move);

#endif
