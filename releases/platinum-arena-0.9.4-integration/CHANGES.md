# Platinum Arena 0.9.4 — integration pass

This pass brings the approved beam presentation into the native Platinum combat overlay. It preserves the original move-sound lookup, Platinum encounter/story handoff, party data interfaces, save layout and configurable controls. Gameplay and visual testing are assigned to the user; this document describes implemented source changes, not a completed playtest.

## Combat and effects

- Registered beam attacks use continuous sprite strips instead of a chain of small orb/puff graphics. Fire uses the approved Ranger 104/106 source sheets. Water and ice use selected Ranger 102/108 strips. Other beam families currently use a neutral gray/white energy fallback pending their own artwork.
- The renderer derives the emitter from the current sprite's body bounds, facing and display size. Both participants in a clash end at the same visual contact point. These are body-bound estimates; exact authored mouth/hand anchors remain a later improvement.
- Held shields fit the visible body and hold an opened pose. Incoming guard contact produces a directional hit flash rather than repeatedly restarting the whole shield expansion.
- Dodges have an eased 12-tick speed curve, limited steering near their end, and directional streaks. Blocked axes keep their collision velocity response, and a stationary blocked dash does not emit movement trails.
- Authored attack timing follows combat wind-up, release/contact and recovery. A sustained beam holds its release pose while its flame animation continues. Until dedicated reaction art is imported, guard and hurt use a neutral idle fallback rather than a shooting pose.
- Opposing beams can sustain a shared pressure phase for 72 simulation ticks, followed by an 18-tick burst phase. Both attacks then recover after mutual deflection. Two independent pairs can coexist in 2v2, and replacement/interruption clears the affected pair safely.
- Ice Beam, Psybeam, Signal Beam, Charge Beam and Flash Cannon now have explicit beam profiles. Water Gun and Dragon Breath retain their projectile behavior.

## Platinum UI and session integration

- Native Pokémon levels and status labels feed the live-combat healthboxes. Move cards shade unavailable or exhausted moves. Selected-action text distinguishes preparation, attacking, recovery, guarding, dodging and clash contact.
- Team headers display remaining/total counts up to 12 combined Pokémon, with a six-ball visual overview. This preserves the numeric total in encounters with two owners per team.
- Battle Lab offers Free 1v1, Free 2v2, Clash 1v1 and Clash 2v2. Clash mode equips opposing beam moves and makes practice AI answer the player's beam, using the ordinary combat implementation. The private practice flag resets with each new match and is not a save setting.
- Open Battle Lab by holding L + R + Select at the title screen. Change the Mode row with Left/Right, select Clash 1v1 or Clash 2v2, and press Start. With default controls, A releases the selected beam; pause instructions follow remapped controls.
- Item failures, empty quantities, prohibited running and unavailable Pokémon show a notice until dismissed. Result text includes a draw state.
- Existing native move cues are unchanged and now trigger once at attack release. Hit and block events can both produce their existing cues in the same update.
- Battle heap budgeting retains a 16 KiB application reserve, checks the largest contiguous free allocation, and caps the child heap at 0xF0000. The existing initialization-failure cleanup/native fallback remains available.

## Resource budget

- Existing main OBJ bank/mapping: 64 KiB.
- Allocated OBJ art including live effects: 64,512 of 65,536 bytes, guarded by a compile-time bound.
- Additional live-effect RAM: 8 KiB across four actors, multiplexed between each actor's beam and held shield.
- Additional active-effect transfer: at most 8 KiB per VBlank. No per-frame file reads were added.
- New constant source beam data: 32,768 tile bytes and one 32-byte palette. The 13 original fire/water/ice colors remain intact, with two unused slots supplying the neutral fallback.

## Remaining animation work

See [MISSING-POSES.md](MISSING-POSES.md) for actual downloadable sources and the required PNG/XML/offset/shadow/credit files. Source metadata already defines Hurt and Charge for the four priority Pokémon and all 306 locally retained extension XML sets. Those declarations do not mean the extra sheets are loaded in this ROM. Dedicated guard/brace poses need suitable artwork; Charge is not automatically a guard pose. The 64 alternate forms still use native still-image fallbacks.

## Validation boundary

The release receipt records compiler/linker results and exact file hashes. No emulator session, gameplay test, visual capture or full regression sweep was run for this pass, at the user's request. Added combat regression source remains available for later use and is not reported as passing.

The user should assess diagonal beams, firing origins, shield fit, dash feel, sound timing, 1v1/2v2 behavior, story entry/return and save continuation. Complex native ability, weather, item and move-script interactions remain incomplete; showing a status label does not establish complete native status mechanics. Control remapping remains session-only.
