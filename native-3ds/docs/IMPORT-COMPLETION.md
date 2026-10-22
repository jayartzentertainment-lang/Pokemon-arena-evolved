# v0.4.0 original Platinum import — development completion report

**The full original campaign is incomplete.** This build delivers a reusable importer, a native compatibility interpreter and a checked opening slice. It does not substitute the old condensed campaign for original Platinum.

## Imported data versus executable coverage

The supplied unmodified `pokeplatinum-main` source is the input. The importer retains 3,822 field files, 686 text files, 2,898 trainer files, 1,160 item files, original per-species data and the original eight growth tables. It losslessly ingests 1,124 script files into symbolic intermediate representation. These counts describe retained data, not completed engine support.

18 map headers are compiled for native use. They cover the player's and rival's Twinleaf houses, two other Twinleaf interiors, Twinleaf Town, Route 201, Verity Lakefront, both Lake Verity variants, Sandgem Town, Rowan's lab, the counterpart house, Center, Mart and house. 32 original map cells are converted from Nitro geometry, placements, textures and prop transforms. Original grid coordinates, dimensions, terrain bits and warp/event definitions are retained. The native view is an orthographic top-down bake, not the original DS animated perspective/height renderer.

The compatibility layer handles the opening's variable/flag conditions, bounded calls/jumps, messages and substitutions, acknowledgement/yes-no/starter prompts, object movement and visibility, camera paths, map load/transition/frame/coordinate triggers, warp overrides, rival following, starter grant, original first trainer battle and post-battle continuation. It preserves original teams/growth inputs. It stops visibly on unsupported commands or missing destinations. Source ingestion is not an implementation of every original command.

## Source-based checks

`tests/test_platinum_import.c` checks original new-game initialization and upstairs television/rival sequence; actual movement through original terrain and house warps; Mom's exit warning; Twinleaf's rival collision and rival-house sequence; Route 201's Rowan/counterpart/Barry cutscene; original starter grant and actual level-five Turtwig/Chimchar configurations; first-battle continuation into the original Mom/Running Shoes event; Barry following into Verity Lakefront; original warp selection; and the original Lake Verity Cyrus camera/movement/message sequence and its flags.

The host test **injects a first-battle victory solely to test the original continuation**. It is not a claim of a gameplay victory or a complete original-ROM comparison. Source-based assertions and actual native emulator observations are recorded separately in `VALIDATION.md` and `reports/`.

## Integration and preservation

The native creator opens at Rowan's original selection text. Start+Select is an intro-only shortcut; confirming it starts the authentic upstairs bedroom with original new-game flags. The supplied layered trainer body/clothes/hair/hat/accessories remain editable and are used in exploration. This does not claim newly authored Generation IV trainer artwork or every original trainer animation.

Original-story saves use `sdmc:/3ds/platinum-arena-preview/platinum-v1.sav`, storing original flags/variables/position/NPC anchors/RNG/starter/Running Shoes alongside the Arena appearance/party profile. They are CRC checked with staged writes and a known-good backup. A battle/cutscene is not serialized mid-execution. Original map initialization is rerun on load. The separate `adventure-v1.sav` remains the v0.3.0 legacy campaign; incompatible story flags are not fabricated during migration.

The v0.3.0 project and release are unchanged, as are the earlier native and DS integration releases. The new native source/build live separately.

## Still required

- Port remaining original script commands, shared/common-script banks and services before later events can execute faithfully. Sandgem graphics/scripts are compiled, but the lab's player-state/naming/Pokédex and later services are not yet complete; do not assume its whole sequence works.
- Implement original bag/items, full wild encounters/catching progression, Pokédex, shops/Center services, original naming/nickname/record systems, original intro animations and rival-name selection.
- Full BDHC height, directional ledges/water/HM movement, animated models/doors/weather, autonomous NPC movement and all remaining maps/connections need native support. Current collision uses original terrain blocking bits and script-adjusted warp positions.
- Original shared scripts, original audio/music backend, complete battle presentation, trainer rewards/held items/abilities/natures/EVs, alternate forms, original level-up choice/evolution services and exact move parity remain unfinished. Missing services stop instead of inventing outcomes.
- Gyms/Galactic/Distortion World/legendaries/League/postgame exist in retained original inputs, but are **not claimed playable** in this preview.
- Original source behaviour is the reference; a side-by-side full campaign comparison and physical hardware profiling are pending.

## Reproduce conversion

The protected editable package restores the converted graphics and generated private C tables required by `bash tools/build_native.sh`. To regenerate original content, point the following tools to the supplied unmodified decomp source; no commercial game is downloaded:

```sh
python3 tools/import_platinum.py --source /path/to/original/pokeplatinum-main
python3 tools/prepare_platinum_graphics.py --source /path/to/original/pokeplatinum-main
python3 tools/import_platinum.py --source /path/to/original/pokeplatinum-main
python3 tools/bake_platinum.py --source /path/to/original/pokeplatinum-main --apicula /path/to/apicula
python3 tools/prepare_hurt_sprites.py
python3 tools/test_host.py
bash tools/build_native.sh
```

The first importer run discovers the map set; graphics conversion assigns source NPC indices; the second importer compiles those indices. Blender 4.3.2 and apicula revision `3d4e91e14045392a49c89e86dab8cb936225588c` produced the map conversions. Hurt/Arceus use SpriteCollab revision `4702f10d888d19e804c5986a91170dfc92ae01b8`, original durations/hit frames and white shadow anchors. Exact source-file hashes, credits and supported counts are in `provenance/`.
