# Reference inspection for combat quality milestone 1

The supplied MP4s were available locally. Inspection used timestamped contact
sheets: eight representative frames from each of five clips, plus sixteen
frames at 8 fps across 15.0–16.875 seconds of `emerald-arena-cover-smoke.mp4`.
This is sampled visual analysis, not a claim that every frame was reviewed.
Names, durations and source SHA-256 hashes are in
`provenance/reference-videos.json`; reference images stay in private validation.

## What the footage shows

- **Cover/smoke clip, 15.5–16.875s:** Charizard commits to a directional pose;
  fire starts near its mouth, grows into a broad sustained plume, flickers with
  a bright white/yellow core and orange/red perimeter, then shrinks/disappears.
  Impacted cover produces small debris. It is visibly different from a narrow
  bolt or static orange rectangle.
- **17s combat clip:** directional repositioning, short physical contact
  feedback, recoil and briefly translucent hurt reactions. Readable Pokémon
  silhouettes and movement remain central to the presentation.
- **Combat-update clip:** short forward attacks, directional water/electric
  contact, recovery and clear health/move feedback. Anticipation, contact and
  damage are related events rather than unrelated timed effects.
- **Protect playtest:** the Pokémon remain readable around defense effects,
  switching and attack phases; menus use recognizable Pokémon framing.
- **Main 122s fighting reference:** more ambitious charge/fire spirals,
  elemental bursts, environmental damage and staged area effects. These
  illustrate the desired energy and timing; this milestone does not claim to
  reproduce every large environmental/area effect.

## Source recovery and adaptation

Recovered the exact supplied Emerald Arena
`graphics/arena/flame/sheet.png` (512x64, eight 64x64 cels) and inspected its
original `src/arena_move_fx.c:ArenaMoveFx_Flame` implementation. The original
cycles `(age/4)&7` and scales the plume with attack reach. The new converter
retains those eight cels and four-tick cadence. Hashes and emitter registration
are recorded in `provenance/emerald-flame.json`.

The new renderer draws the source plume with a broader body/core, moving
embers, mouth anchoring, short ignition/growth and fade phases. Hydro Pump
keeps the supplied D/P water sheet, directional cannon origins, core layering
and restrained spray. Ranger lightning remains distinct; actual D/P sword
cels accompany Swords Dance. Confirmed contacts use supplied elemental and KOF
impact art. No general preview bubbles stand in for these attacks.

Native beam collision grows with the first six active simulation ticks and
stops at cover. The visuals read that same reach, and opposed beams terminate
at the combat system's shared clash contact. Turning a sustained beam is
bounded to approximately three degrees per simulation tick. Visual potency
does not change damage, PP or the inherited heavy-commitment multiplier.

## Deliberate limits

The result follows the recovered source, color identity, origin and phase
structure. It is not a frame-for-frame copy of the reference footage. The 3DS
uses nearest-neighbor source textures, bounded sprite layers and particle
counts; expensive full-screen post-processing is not added. High/low VFX
budgets retain the same base attack and identical gameplay rules.

Eight-direction body art is preserved while aim trajectories are continuous.
Held contact-cel emitters are calibrated; all alternate animation poses,
future species and physical hardware still need individual review. New 3DS
performance and visual clash behavior should not be called verified merely
because the core/renderer compile or a host clash test passes.
