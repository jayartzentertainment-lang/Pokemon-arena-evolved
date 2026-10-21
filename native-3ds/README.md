# Platinum Arena Evolved — native 3DS Combat Quality Update 0.2.0

Milestone 1 builds on the user-approved 0.1.0 prototype. This is a native
Nintendo 3DS `.3dsx`, using devkitARM, libctru and Citro2D. **No Android port is
included.** The accepted 0.1.0 native build and the separate Platinum DS game
are preserved.

Play as Pikachu, Lucario, Charizard or Blastoise, with four learned moves each,
four-member parties, 1v1 or simultaneous 2v2 with an AI partner.

## New in this milestone

- Default enemy lock-on with a visible body-registered target marker.
- Movement and aiming are independent: strafe around your selected opponent.
- Select toggles lock-on/facing; R+Y or TARGET > cycles living enemies in 2v2.
- The right-side touch pad sets a persistent free-aim direction. Move buttons
  occupy a separate column, with captured single-touch interactions.
- Optional New 3DS C-stick aiming; original 3DS does not require a second stick.
- Continuous projectile/beam vectors, bounded beam steering and cone/range/
  line-of-sight soft assistance in facing mode. Assistance does not home shots.
- Status casts explicitly stop walking and defensive cancels until completion
  or interruption. Effects resolve at the release event, separately from the
  effect lifetime. Swords Dance/Agility/Calm Mind/Iron Defense/Growl are checked.
- Exact supplied Emerald Arena eight-frame Flamethrower plume, replacing the
  thin Ranger flame. Mouth origins compensate the authored pose and flight bob.
- More substantial Hydro Pump cores/spray, flowing embers and elemental impacts.
- Beam collision reach grows with the visible startup and stops at solid cover.
- High/low VFX presentation budgets; gameplay rules remain identical.
- Game opens paused, so controls can be read before the opponent attacks.

## Play and controls

Extract the private testing ZIP. Open the `.3dsx` in Azahar, or put the included
`3ds/platinum-arena/` folder on the SD card of a homebrew-enabled 3DS and launch
through Homebrew Launcher. Press **Start** to begin.

| Input | Action |
| --- | --- |
| Circle Pad / D-pad | Eight-direction movement; analog speed on Circle Pad |
| Y | PP-free natural strike |
| A | Selected learned move |
| X | Special/status move |
| Four left-side touch cards | Select and queue that move |
| Right-side TOUCH AIM pad | Set free-aim direction; persists after release |
| Select | Toggle lock-on / facing; return from free aim to lock-on |
| R+Y or TARGET > | Cycle living opponents |
| New 3DS C-stick | Optional independent free aim |
| B moving / B standing | Dodge/wind dash / held guard |
| L+A / L+Y | Cycle learned selection / natural attack style |
| Tap R and release | Switch healthy party member |
| R+X | Cycle allied AI tactic |
| Start | Pause/resume; restart after a result |
| X while paused | Toggle high/low VFX |
| NEW 2V2 / NEW 1V1 | Reset into that battle mode |
| Start+Select | Exit |

Azahar defaults: arrow keys = Circle Pad; X/A/Z/S = Y/A/X/B; Q/W = L/R;
M/N = Start/Select. R+Y is W+X on that default keyboard profile.

Target brackets use green for a clear, in-range aim point, amber for out of
range and red for blocked aim. Self-buffs do not require manually targeting
your own Pokémon. Rooted casting shows **CASTING: MOVEMENT LOCKED**.

## Build and validate

Linux prerequisites: Docker, Python 3/venv and a host C compiler.

```sh
bash setup/bootstrap_native.sh
source /workspace/native-3ds-setup/venv/bin/activate
python tools/test_host.py
```

Public source omits private PNGs, texture atlases and commercial ROMs. Recover
the verified 0.9.6 inputs and use the retained user-supplied effects/workspace:

```sh
python tools/recover_inputs.py \
  --source-archive /path/to/Platinum-Arena-0.9.6-SOURCE.zip \
  --platinum-source /path/to/pokeplatinum \
  --out assets/private/recovered

python tools/prepare_assets.py \
  --ds-source assets/private/recovered \
  --extra-effects /path/to/extra-effects \
  --dp-effects /path/to/diamond-pearl-attack-graphics/source \
  --emerald-source /path/to/supplied/Arena/workspace

bash tools/build_native.sh
```

The canonical pret checkout is pinned at
`1dc13dcea40c04ab53f1f607de46023dc409ef20`. The archive and packed art hashes
are checked before conversion. Emerald source must contain
`graphics/arena/flame/sheet.png`; conversion fails rather than silently
substituting a different flame. Effects layout remains `0/` Ranger,
`1/Effects/` KOF and `2/move_VFX/` PMD. See `assets/manifest.json`.

Outputs: `Platinum-Arena-3DS-Prototype.3dsx`, `.smdh`, `.elf` and
`validation/build.json`. The private delivery filenames include 0.2.0.

Ten host checks use ASan/UBSan. Add
`--ds-source /workspace/platinum-port/pokeplatinum` to verify DS preservation.
The quality suite exercises real native input, casting, aim and growing-clash
paths; legacy regressions retain the portable core's default instant beam
reach. The native game explicitly enables collision-synchronized beam growth.

## Scope and limits

This milestone improves the approved combat demo. It does not contain the
Sinnoh campaign, trainer creator, save/load, all 493 native species or complete
467-move mechanics. Those remain the next milestones in the user's directive.
No alternate region/story has been substituted.

The source sprites keep eight directional poses and authored timing/anchors.
A continuous aim vector does not create new 360-degree body artwork. Dedicated
hurt/faint poses and exhaustive pose-by-pose emitter review remain future work.

Reference footage was inspected through timestamped samples and a focused
Flamethrower motion sequence. See `docs/REFERENCE-ANALYSIS.md` for specific
observations and adaptation limits. This is a hardware-aware approximation,
not a claim to reproduce every reference-video effect.

Audio remains the optional original synthesized prototype cues. Original DS
audio has not been ported. NDSP was unavailable in this emulator environment,
so the HUD shows SFX OFF; no audio listening verification is claimed.

Read `docs/VALIDATION.md` and `docs/reports/` for final build/run evidence.
Physical original/New 3DS profiling remains pending. Public code/source ZIPs
exclude the art-bearing testing executable; private downloads are delivered
separately, respecting `ASSET-RIGHTS.md`.
