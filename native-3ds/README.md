# Platinum Arena Evolved — native3DS0.3.0

A connected **condensed Sinnoh adventure** around the existing native3DS action-battle engine. The approved0.1.0 and combat-quality0.2.0 projects/releases remain intact.

This is a new native2D adaptation, not the original Platinum DS executable or an exact map/script conversion. It uses newly authored small layouts built with supplied Emerald tiles, authentic Platinum NPC sprites/system font, shortened rewritten story dialogue, and Platinum trainer parties. It is an alpha for playtesting.

Included:

- 34 connected, scrolling Sinnoh locations;41 ordered story objectives from Twinleaf/starter/rival through Team Galactic,8 gyms, Giratina, Elite Four and Cynthia/Hall of Fame. Gold markers and route hints guide the current objective. Story battles return to the same adventure state; defeat and retreat do not advance objectives.
- Plain editable trainer with2 bodies,4 skin options,12 bottom options,14 tops,28 hair options,9 hats and3 accessories, plus no-item choices. The actual supplied32x48 layers share identical anchors. The preview stands still. The same appearance/name is used in exploration and saved; editing is available later.
- All493 **base-species** authentic directional sprite sets, source frame timing and per-direction foot anchors. Battle Lab can select any species and launch1v1/2v2. This does not claim all alternate forms or467 mechanically complete moves.
- Six-slot persistent parties, native level-scaled stats, HP/PP/status restoration into battle, ordinary level evolutions, catching,60-slot PC, potions, shops/centers, follower toggle and smooth trail movement. Catching attempts cost balls and depend on target HP/status/catch rate. Capturing trainer Pokemon is blocked.
- Versioned CRC-protected save with temporary-file validation and previous-save backup. Progress autosaves between scenes; an in-progress battle is not a resumable checkpoint. Combat-only0.1/0.2 versions had no compatible adventure saves to migrate.
- Existing60Hz combat, aiming/lock-on/touch aim, dodge/guard, committed buffs,1v1/2v2 and party switching. Exact Emerald Flamethrower and Hydro VFX remain; an additional58-cel supplied family atlas and actual Swift stars provide typed effects for the expanded roster. Shared VFX families are explicitly reused.

## Play

Open `Platinum-Arena-3DS-Prototype.3dsx` in Azahar, or copy `3ds/platinum-arena/` to the SD card of a homebrew-enabled3DS and use Homebrew Launcher. Original3DS single Circle Pad is supported; a C-stick is optional. No APK, `.nds`, `.cia` or commercial ROM is included in this release.

The delivered download ZIP is AES256 encrypted, as with the working previous download. Use7-Zip on PC or ZArchiver on Android with the privately supplied password. The protected package includes the game, source, converted editable PNG assets, checks and actual emulator screenshots. The publicly readable source ZIP excludes private graphics.

### Adventure controls

- Circle Pad/Dpad: move; B: run. A: talk/confirm. Y: party. X: map. Start: trainer menu.
- Creator: Up/Down selects a row, Left/Right changes its actual sprite layer; L/R rotates the stationary preview; Y edits name; Start confirms. Touch left/right of a row changes its choice. The name keyboard supports touch/Dpad/A, B deletes and Start finishes.
- Party: Up/Down selects; A makes that Pokemon the lead/follower; X uses a40HP potion; B returns.
- PC: Up/Down selects; A withdraws or swaps with the lead when the party is full; Y deposits the lead while leaving at least one party member; L+X releases a boxed Pokemon.
- Map: Up/Down selects, L/R changes page, A travels to an already visited unlocked location.
- Gold ! marker: current story character. Lower screen shows the target location and a starred road toward it. Walk to the arrow at the corresponding map edge. Open MENU to heal/buy balls in a town, save, change appearance or enter a training encounter.

### Battles

- Y: PP-free natural strike. A: selected learned move. X: special/status. B while moving: dodge/dash; standing B: guard.
- Select: lock/facing. R+Y or TARGET>: change target. Right touch area: persistent free aim; left move cards: use move. Optional C-stick aims.
- L+A: cycle learned move; L+Y: cycle natural. R tap/release: switch party. R+X: AI ally tactic.
- L+R or BALL: attempt capture in a wild/Giratina encounter. Story BALL replaces the combat-demo reset button.
- Start: pause. X while paused: high/low secondary VFX. B while paused: retreat without completing the story objective. After victory/defeat, A/B/Start returns to the adventure.
- Lab: Left/Right chooses a Dex entry, L/R skips10, Up/Down switches edited team, Y toggles clash practice, A starts1v1 and X starts2v2. B returns to title. Lab battles never alter your adventure save.

Azahar defaults: arrows=Circle Pad; T/G/F/H=Dpad; X/A/Z/S=Y/A/X/B; Q/W=L/R; M/N=Start/Select.

## Source and build

Pinned official toolchain: `devkitpro/devkitarm@sha256:116afba8df8453961de2936ffab20dd441edf4d682856c1ec8b0e53d7ed0bbf5`.

With the protected editable assets extracted into `gfx/private/`:

```sh
python3 tools/test_host.py
bash tools/build_native.sh
```

To regenerate the converted artwork from the retained supplied inputs:

```sh
python3 tools/prepare_assets.py --all-species --ds-source /path/to/pokeplatinum --extra-effects /path/to/extra-effects --dp-effects /path/to/diamond-pearl-attack-graphics/source --emerald-source /path/to/arena-workspace
python3 tools/prepare_adventure.py --ds-source /path/to/pokeplatinum --creator /path/to/customization-tools --world-source /path/to/accepted-emerald-arena/ds
python3 tools/prepare_move_catalog.py --ds-source /path/to/pokeplatinum --dp-effects /path/to/diamond-pearl-attack-graphics/source
```

`content/campaign.json` contains editable native story data. `adventure_data.c` and `species_data.c` are generated tables. The converter verifies original input hashes, retains layer coordinates and does not download ROMs. Hair atlases are split by body to stay within Citro2D texture limits. A bounded8-sheet Pokemon cache prevents retaining all493 textures in memory.

## Current limits

The campaign is condensed: original Platinum map geometry, interiors, puzzles, script VM, cutscenes, HM traversal, contests, trading/link play, quests and postgame areas are not fully reproduced. Ordinary level evolutions work; item/trade/friendship/other special evolutions need their own systems. XP uses a simplified cubic Arena growth curve, accelerated story catch-up and auto-selected level-up moves. Full ability/held-item behaviour, natures/EVs/breeding and exact behaviour for all467 moves remain unfinished. Encounter tables are small native selections; after Hall of Fame, training encounters open the National Dex pool. The lab exposes the full roster at any time. This is not a claim of full original-game parity.

Audio remains the previous optional synthesized prototype cues, not Platinum's commercial audio engine; it may show SFX OFF if NDSP is unavailable. Physical original/New3DS profiling and full human campaign playtesting remain pending. See `docs/VALIDATION.md` for checks actually performed.
