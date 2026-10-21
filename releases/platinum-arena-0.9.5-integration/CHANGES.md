# Platinum Arena 0.9.5 — combat upgrade for user playtesting

This is a focused update to the native Platinum live-combat overlay. The user
has requested delivery without assistant gameplay or emulator testing. Native
compiler/linker evidence and exact download hashes are recorded separately;
the changes below describe source implementation, not a completed playtest.

## Controls and movement

- Default Y uses equipped move 1; A uses move 2; X uses move 3 or 4, with L
  toggling X between those two slots. These are actual equipped moves with PP,
  rather than the former universal Y strike/selected-and-next move scheme.
- B is permanently reserved for guard while stationary and dodge with a
  direction. Other combat bindings remain configurable for the current ROM
  session. Start pause, Select aim and L+R catching retain their roles.
- A release followed by a second direction press inside the double-tap window
  can dash. It uses the same cancellation, stamina and cooldown checks as a
  B-direction dodge. Holding a direction is not a repeated double-tap.
- When all equipped moves are out of PP, the existing PP-free basic path is
  used as a Struggle fallback instead of leaving the fighter unable to act.
  This fallback does not reproduce retail Struggle recoil.

## Interactive beam pressure

- Two independent pairs remain available in simultaneous 2v2.
- During a clash, tap the button assigned to the move that fired the beam.
  Fresh presses move the shared contact point toward the opponent; holding
  the button does not repeatedly push, and taps spend no additional PP.
- Pressure has a per-actor cooldown and bounded AI pulses. Reaching a pressure
  endpoint resolves one normal damage attempt, then ends the participating
  beams. A clash that times out without a breakthrough dissipates mutually.
- The pressure window is 180 simulation ticks, with the existing short burst
  phase afterward. Replacement, interruption and action ownership remain
  explicit; a clash result must not damage a replacement fighter.
- The HUD shows pressure information. Battle Lab's Clash modes use the first
  equipped move, now Y by default; hold L+R+Select at title to open the Lab.

## Contact visuals, Swift and presentation

- Swift uses actual star artwork selected from the supplied Diamond/Pearl
  attack-graphics sheet. The source PNG, extraction/colour manifest and
  reproducible importer are retained under `res/arena/move-effects/` and
  `tools/arena/import_swift_art.py`.
- Melee swings no longer emit the old unconditional active-phase hit graphic.
  Actual damaging contact emits a separate contact effect. Attack motion and
  contact sparks have distinct roles; recovery still follows the contact.
- Contact feedback and directional spark/glint presentation replace generic
  orb-shaped impact feedback. Approved continuous beam artwork and the native
  move-sound lookup are retained.
- Small grounded motion envelopes refine walking and attack presentation
  using existing directional frames. The bottom HUD describes the equipped
  move slots, fixed defence and X-slot toggle consistently with the input code.
- Selected art reuses the existing OBJ mapping. Final OBJ allocation is
  64,832 of the chosen 65,536 bytes, including two 16x16 Swift frames and one
  small glint; no full new effect atlas or per-frame source-image read is added.

## Scope and limits

The Platinum story/encounter, party, save, reward, catching and existing 1v1/2v2
interfaces remain the foundation. This pass does not integrate the expanded
layered trainer creator, global followers, additional forms or a complete set
of dedicated hurt/brace animations. Those source assets remain preserved for
later work. Other generic move-art fallbacks and complex native ability,
weather, held-item and move-script interactions still need further coverage.

Gameplay feel, all controls, diagonal effects, 2v2 clashes, guard, story
entry/return and saving remain pending the user's playtesting. No automated
gameplay test, emulator session, screenshot run or host regression suite was
performed for this build. Compiler success is not gameplay verification.
