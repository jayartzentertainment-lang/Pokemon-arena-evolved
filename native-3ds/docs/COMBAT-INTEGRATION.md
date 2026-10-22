# Native integration development: movement and contact

This is phase 1 of the master directive, on the separate
`codex/native-3ds-full-integration` branch. It is not the finished campaign.
The existing engine, creator, 493 base species, move inventory and original
content importer are retained. No Android work or replacement story is added.

## Simulation changes

Movement uses fixed-point acceleration, braking and incremental body turning.
Lock-on controls attack aim while walking faces the direction actually travelled.
Authored directional frame durations remain intact; travelled world distance
advances a stride-calibrated gait phase. Collision-blocked motion does not cycle
feet. Grounded actors no longer receive an additional artificial renderer bob.
Gengar uses directional hovering poses rather than a forced leg cycle.

Pikachu, Lucario, Blastoise, Gengar and Arceus have explicit motion calibrations.
Their baseline walking speeds increase by 12%, 9%, 6%, 7% and 7%, respectively.
Acceleration and braking vary by species; Blastoise retains slower acceleration.
Other species use the existing speed profile with an 8% increase and a
body-dimension-derived stride. This fallback is not a hand-tuned 493-species audit.
The simulation remains 60 fixed steps per second. Agile startup/recovery is
adjusted selectively; PP, stamina, cooldowns and committed buff casts remain.

Native short-range attacks stay planted. Actual charge moves sweep the root
path. Bite has a narrower capsule; supported tail strikes sweep three sectors.
Projectiles sweep registered body ellipses, including relative target movement;
beams use registered body origins and actual growing reach. Rendered targeting
and AI use these coordinates and ranges. Damage begins at the authored hit cel,
with authored follow-through mapped to recovery.

Hurt duration follows the supplied Hurt animation, with a small extension for
strong hits. Recoil resistance is separate from HP damage. Confirmed contact
uses a body-surface point for particles. Hit pause freezes interpolation history
to avoid replaying the previous displacement. Stale early attack taps are
discarded; native attacks require a fresh press, with a short late-recovery buffer.

Hurtboxes are direction-specific stable ellipses registered to neutral source
body/foot anchors. They are not per-pixel masks for every deforming attack pose.
Three-sector tail geometry still shares the existing species attack artwork;
this does not add bespoke tail, punch or bite poses for every species/move.

## Dash assets

`tools/prepare_dash.py` converts supplied PMD wind and dust cels into a small atlas
with eight nearest-neighbour directional variants. Integer reduction preserves
pixel edges and source proportions. The dash renderer no longer uses the old
curved impact bowl. One restrained sprite afterimage remains. Dust is grounded;
wind follows the owner and fades within the actual dash lifetime.

The new source cels are protected inputs in `assets/private/dash-source`.
After restoring them, run `python3 tools/prepare_dash.py`. The earlier private
asset package alone does not include these new inputs. Exact hashes and
transformations are in `provenance/dash-2d.json`. Uploading artwork does not
establish redistribution rights; this artwork stays outside public source.
Existing Flamethrower, water, elemental and impact collections remain available.

## Developer contact inspection

Normal play has no debug overlay. For a development SD card only, create
`sdmc:/3ds/platinum-arena-preview/developer-mode`. In a paused battle, Select
toggles the overlay; Start resumes. It shows simulation hurtboxes, attack zones,
projectile sweeps, foot anchors, direction, resolved displacement and frame IDs.

Optional file contents select real Battle Lab actors/moves for reproducible
inspection, without modifying campaign teams, story flags or battle outcomes:

```text
actors 25 9
moves0 10 44 33 231
moves1 56 182 55 110
ai 1
```

IDs are original species/move IDs. The file is read on each fresh Lab battle.
`ai 0` isolates motion/contact inspection; it is not a competitive gameplay test.
Do not ship this marker on a normal player's SD card.

## Verification boundaries

`test_motion_contact` checks five gait calibrations, independent walking/aiming,
braking, registered swept geometry, contact timing, Hurt/recoil, hit-pause
interpolation, committed casts, rejected stale buffers and eight dash directions.
Host assertions do not establish visual polish. Native compilation, emulator
observations and physical-hardware profiling are separate results in the release
report. No physical hardware is available in this workspace.

The original story remains at its previously documented opening compatibility
coverage. This phase does not implement Sandgem services, the remaining maps,
Gyms, Galactic, Distortion World, League or postgame. See `IMPORT-COMPLETION.md`.
