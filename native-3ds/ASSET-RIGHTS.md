# Asset provenance and distribution boundary

The public source release contains code, conversion tools, timing/registration
data and attribution. Private source PNGs, original ROMs and converted texture
atlases are excluded. The locally generated `.3dsx` embeds the user's supplied
art in RomFS and is a private testing build; it is not included in public Git.

This project does not grant a new redistribution license to Nintendo, Pokémon,
CHUNSOFT, SNK, extracted commercial artwork or unidentified uploaded assets.
Attribution is not a replacement for permission. Resolve those assets' rights
before publishing an art-bearing executable or source-art pack.

| Material | Preserved source / credit |
| --- | --- |
| Four directional Pokémon | PMDCollab/SpriteCollab `d25607ff4746957df10bdb78db090887cd94f1f8`; exact source credits in `provenance/four-species-credits.txt` and `lucario-source.json` |
| SpriteCollab terms | `provenance/SpriteCollab-LICENSE.md`; contributor terms do not license the original official Pokémon sprites |
| Forest floor / destructible props | User-supplied Arena Plus workspace, recovered through verified 0.9.6 sources |
| Move panels | Supplied Platinum battle UI conversion; source NARC provenance copied to `provenance/battle-ui-provenance.json` |
| Hydro Pump / Water Pulse | Uploaded Diamond/Pearl Attack Graphics sheet; Hydro-Pump sheet credits KLNOTHINCOMIN |
| Lightning / flame / defensive frames | Uploaded Pokémon Ranger Guardian Signs effects, numbered source files recorded in `assets/manifest.json` |
| Impacts / wind | Uploaded King of Fighters XI effects; filenames and hashes recorded in manifest |
| Aura / blades / embers | Uploaded PMD Explorers of Sky effect folders; exact source files recorded in manifest |
| Bitmap font | Daniel Hepper's `font8x8_basic.h`, public domain; original header/readme retained under `provenance/` |
| Prototype sound | New synthesized event cues; no commercial sound archive |

`assets/manifest.json` records every selected effects source hash, original
timings, directions, image bounds and per-frame foot origins. It reports this
four-species slice explicitly and makes no claim of complete animation coverage.
