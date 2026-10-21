# Native prototype validation — 2026-10-21

Scope: native combat milestone 0.1.0, not the complete game.

## Build and source preservation

Genuine devkitARM/Citro2D build, native ARM11 ELF and `.3dsx` magic verified.
Executable size: **448,884 bytes**.
SHA-256: `504e1e4ac43dadd68de984c43a6d55658e289de900f016e5ea3fa271f9f0ad99`.
The exact immutable hash-named candidate was launched in Azahar; no file was
rebuilt under that candidate path.

ELF link sections: text 270,580 bytes; data 7,608; BSS 33,144. These are linker
figures, not total runtime RAM. Nine compressed texture sheets total 151,777
bytes; compressed file sizes do not equal GPU texture allocation.

The 0.9.6 source archive's 2,260 checksummed entries were verified. Recorded DS
combat/header/renderer/avatar/presentation/packed-asset hashes still match;
no DS release was rebuilt or overwritten for this native milestone.

## Host checks

**9/9 passed**, with AddressSanitizer and UndefinedBehaviorSanitizer:

- Movement, PP, timing, aim, damage/types, guard/dodge, swept hits and AI.
- Guard exhaustion/chip, projectile/beam clashes, critical events and bounds.
- Seven stat stages, statuses, cooldowns and stat-based 2v2 clashes.
- 19,602 move cone/segment cases.
- 13,068 navigation routes and all 128 destruction masks.
- Destructible material/physics behavior and bounded pools.
- Platinum metadata, category and stamina adaptations.
- Simultaneous 2v2, friendly-fire prevention and deterministic results.
- New native adapter: analog magnitude/D-pad, HP/PP/cooldown preservation,
  healthy unique party bindings, real Swords Dance +2 attack/PP, visible-faint
  replacement delay and whole-party victory logic.

These checks test actual reused/adapted C. Metadata/geometry coverage is not
proof that all 467 move-specific mechanics are complete.

## Observed emulator checks

Azahar **2126.2**, original-3DS configuration, Mesa llvmpipe software OpenGL.
Observed the native game itself, including dual-screen rendering, corrected
floor conversion and HUD/pause overlays, eight directional inputs, live 2v2,
touch commands, party switching and PP changes. Captured source-driven
Flamethrower, paired Hydro Pump cannon jets and Lucario's Swords Dance blades.
The final binary's 1v1 battle reached whole-party defeat after 3,712 simulation
ticks, with 51 registered hits and eight automatic/manual party switches.
Exit telemetry recorded nine cached texture-sheet loads. Victory logic also
passed the host adapter check; an emulator player-victory run is not claimed.

Session screenshots and telemetry are retained privately under `validation/`.
The factual final telemetry/report is copied under `docs/reports/`.

Native HUD FPS and Citro3D processing/drawing times use emulated time. They
cannot be used as physical 3DS measurements. Azahar host speed varied with
software rendering/cloud contention: observed roughly 31–61 host FPS across
captures, including paused captures and concurrent build work. Quieter 1v1
captures showed approximately 57–61 host FPS. This is a limited smoke test,
not a sustained original-hardware 60 FPS guarantee.

## Pending

Real original and New 3DS hardware profiling; sustained worst-case 2v2 frame
pacing; audio listening; exhaustive cannon/impact alignment across every pose;
long play sessions and balance; native campaign, creator and persistent saves.
Clash mechanics have host regression evidence; this session does not claim a
complete visual emulator clash playtest. Dedicated hurt/faint source poses
remain future artwork work.
