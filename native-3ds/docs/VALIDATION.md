# Native 3DS 0.2.0 — milestone 1 validation

Date:2026-10-21. This report covers the combat quality update, not the full game.

## Build

Genuine native ARM11 .3dsx built with pinned official devkitPro GCC16.1.0,
libctru/Citro2D and tex3ds2.3.0. Binary: **461,728 bytes**.
SHA256: `7156caccf6214a24b51439eb10f503128038b24c84d478b6396916ae50836c19`.

The exact immutable hash-named candidate was launched and observed in
Azahar2126.2 with original3DS configuration, stereo/optionalCPU speedup disabled.
No executable was rebuilt underneath that candidate path.

## Host checks

**10/10 pass with AddressSanitizer and UndefinedBehaviorSanitizer.**
The retained eight combat/geometry/navigation/physics/2v2 suites and native
party adapter pass. Added quality checks cover:

- Movement while locked to a target; enemy cycling/invalidation without switching.
- Independent persistent free aim and continuous projectile launch vectors.
- Rooted Swords Dance, Agility, Calm Mind and Iron Defense with real stat changes.
- AI status casting uses the same movement lock.
- Interrupted casts do not grant unapplied buffs; Growl debuffs its target
  without applying a casting lock to that target.
- Beam reach grows with the effect, steering is bounded, simultaneous growing
  beams form a clash and switching clears its ownership/contact.
- Every one of the 76,800 touch-screen coordinates respects aim/move separation.

Legacy regressions retain the portable core's existing immediate beam reach;
the native wrapper explicitly enables synchronized growth. The added quality
suite tests that actual native path. Passing metadata/geometry coverage does
not imply all 467 move mechanics are implemented.

## Observed emulator checks

Observed the actual delivery binary's paused entry/high-low settings, reticle
tracking, movement while locked, touch free aim after finger release, manual
projectile PP, re-lock, guard/wind dash, 2v2 target cycling, status cast input,
new Emerald flame stream and Hydro Pump. Captures and native telemetry are
retained privately. `docs/reports/emulator-results.json` records final facts.

The casting telemetry records `root=1` at a fixed world position. Normal enemy
attacks can interrupt the cast: this is the designed interruption path, not
permission to slide while a cast remains committed. Host tests also verify
complete uninterrupted casts and the returned movement control.

The fire preview is captured from the running emulator, not a mocked renderer.
Core growing-clash geometry/ownership has automated evidence; a complete
visual emulator clash playtest is not claimed unless explicitly recorded.

## Performance and limits

Native HUD FPS/CPU/GPU times are measured in **emulated time**, not on a real
3DS. Software OpenGL/host contention affects wall-clock emulator speed. The
final metrics are in the JSON report; no sustained original-hardware60FPS
claim is made. Actual original/New3DS profiling and C-stick hardware testing
remain pending. NDSP was unavailable (SFX OFF); audio was not listening-tested.

Only four species and their equipped moves are in this native build. Complete
trainer creation, native saves, all493/all467mechanics and Sinnoh progression
remain future milestones. Held emitter cels were calibrated; exhaustive
all-pose/new-species alignment still requires review.

## Preservation

The approved0.1.0 source/binary and the separate Platinum0.9.6 DS release are
preserved. `provenance/approved-native-baseline.json` records original source
hashes; final preservation checks are included under `docs/reports/`.

Final exact-binary result: whole-party defeat after 4239 simulation ticks, 58 hits, 6 automatic/manual switches and nine texture-sheet loads. Native HUD median59.82FPS, processing0.981ms and drawing0.498ms are emulated-clock values only.
