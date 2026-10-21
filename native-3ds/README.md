# Platinum Arena — native Nintendo 3DS prototype 0.1.0

A separate native **`.3dsx` combat prototype**, built with libctru and Citro2D.
The existing Platinum 0.9.6 DS project and `.nds` release are preserved.
This first milestone contains an arena, not the Sinnoh campaign.

Playable roster: **Pikachu, Lucario, Charizard and Blastoise**, each with four
learned moves, directional artwork, a four-member party and automatic faint
replacement. Touch **NEW 2V2** to start two allied Pokémon against two enemies;
your partner is AI controlled. Touching this button resets the current battle.

## Play

On a homebrew-enabled Nintendo 3DS, put the `.3dsx` (and optional `.smdh`) in
`/3ds/platinum-arena/` on the SD card and open it with the Homebrew Launcher.
Azahar can open the `.3dsx` directly. It is not an `.nds` or a `.cia` installer.

| Input | Action |
| --- | --- |
| Circle Pad / D-pad | Eight-direction movement; Circle Pad magnitude controls speed |
| Y | Natural physical attack; no PP |
| A | Selected learned move |
| X | Assigned special/status move |
| Touch a move | Queue that specific equipped move |
| L + A | Cycle the selected learned move |
| L + Y | Cycle natural attack style |
| B while moving | Dodge / wind dash |
| Hold B while standing | Guard until release or exhaustion |
| Double tap D-pad direction | Directional dash |
| Tap R, then release | Switch to the next available party member |
| R + X | Cycle allied AI tactic in 2v2 |
| Select | Toggle aim lock; manual facing is the default |
| Start | Pause/resume; restart after a battle result |
| Start + Select | Exit |

Default Azahar keyboard: arrows = Circle Pad, T/G/F/H = D-pad,
X = Y, A = A, Z = X, S = B, Q = L, W = R, M = Start, N = Select.
Keyboard bindings depend on your emulator profile.

## Build from source

Linux prerequisites: working Docker, Python 3, a host C compiler and Python venv.
The setup uses the official devkitPro image pinned to an immutable digest.

```sh
bash setup/bootstrap_native.sh
source /workspace/native-3ds-setup/venv/bin/activate
```

The public source does **not** include the private Pokémon sprite/effect PNGs,
commercial ROMs, SDKs or generated texture atlases. Use your retained inputs:

1. The exact `Platinum-Arena-0.9.6-SOURCE.zip` recorded in
   `provenance/source-archive.json`.
2. A `pret/pokeplatinum` checkout at
   `1dc13dcea40c04ab53f1f607de46023dc409ef20` for canonical JSON/enumerations.
3. Your supplied Ranger, KOF and PMD effect directories and D/P attack sheets.

```sh
python tools/recover_inputs.py \
  --source-archive /path/to/Platinum-Arena-0.9.6-SOURCE.zip \
  --platinum-source /path/to/pokeplatinum \
  --out assets/private/recovered

python tools/prepare_assets.py \
  --ds-source assets/private/recovered \
  --extra-effects /path/to/extra-effects \
  --dp-effects /path/to/diamond-pearl-attack-graphics/source

bash tools/build_native.sh
python tools/test_host.py
```

`extra-effects` retains the supplied layout: `0/` Ranger images,
`1/Effects/` KOF frames and `2/move_VFX/` PMD folders. Consult
`assets/manifest.json` for exact input filenames and SHA-256 hashes.
`recover_inputs.py` verifies the archive and each copied archive member.
The converter verifies the packed 0.9.6 sprite source before decoding it.

Output: `Platinum-Arena-3DS-Prototype.3dsx`, `.smdh`, `.elf` and
`validation/build.json`. No base ROM is fetched by these tools.
`tools/test_host.py --ds-source /path/to/preserved/DS/checkout` additionally
checks the recorded DS preservation hashes without changing those files.

## Implemented and deferred

Implemented: native dual screens, fixed 60 Hz portable simulation, render
interpolation, analog magnitude, physical/special damage, types, PP/cooldowns,
startup/active/recovery, guard/dodge/stamina, knockback, hit reactions, visible
faint/deployment transitions, switching, AI and battle results. Swords Dance
raises Lucario's actual attack stage and spends PP. Existing real-time status
and clash rules are retained, including ownership invalidation on switching.

The four selected species use all eight directional sets and preserved frame
durations/contact markers. Dedicated hurt poses are not supplied for this
slice: damage reactions use a registered idle pose, flash and real knockback.
Fainting is a fade/shrink transition, not a new authored faint animation.

Ranger lightning/flame, true D/P Hydro Pump jets, KOF impacts/wind, PMD aura,
swords and embers, actual terrain and supplied Platinum UI panels are used.
Blastoise's emission points were measured on its directional Shoot cels; only
the visible cannon is drawn in strict side views. Effects remain prototype art
adaptations and need further directional/physical-hardware review.

The inherited database recognizes 467 move IDs/metadata and conservative
geometry. That does **not** mean every Generation IV move mechanic is complete.
Only four species are imported into this native executable. All 493 species,
abilities/items/weather, exhaustive move logic, save/load, trainer creation,
maps, NPCs, capture and the Sinnoh campaign remain later milestones.

Audio is an optional original synthesized placeholder, driven by real combat
events. The original game's sound engine/audio has **not** been ported. If NDSP
is unavailable the HUD says `SFX OFF` and combat continues. Audio was unavailable
in the headless emulator environment; no listening test is claimed.

See `docs/PORTING.md`, `docs/VALIDATION.md` and `ASSET-RIGHTS.md` for provenance,
observed checks and limits. Original/New 3DS hardware profiling remains pending.
