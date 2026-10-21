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
| Lightning / defensive frames | Uploaded Pokémon Ranger Guardian Signs effects, numbered source files recorded in `assets/manifest.json` |
| Flamethrower | Exact supplied Emerald Arena `graphics/arena/flame/sheet.png`; original rendering/source hashes in `provenance/emerald-flame.json` |
| Swords Dance | Actual uploaded Diamond/Pearl `Swords-Dance.png` sword cels |
| Impacts / wind | Uploaded King of Fighters XI effects; filenames and hashes recorded in manifest |
| Aura / blades / embers | Uploaded PMD Explorers of Sky effect folders; exact source files recorded in manifest |
| Bitmap font | Daniel Hepper's `font8x8_basic.h`, public domain; original header/readme retained under `provenance/` |
| Prototype sound | New synthesized event cues; no commercial sound archive |

`assets/manifest.json` records every selected effects source hash, original
timings, directions, image bounds and per-frame foot origins. It reports this
four-species slice explicitly and makes no claim of complete animation coverage.


## Native adventure0.3.0

The extended493 base-species directional roster retains the supplied Platinum integration metadata and per-species credits in provenance/SPRITE-CREDITS-493.md. Original Nintendo/Chunsoft artwork rights are not reassigned by a community repository license. New adventure map layouts/dialogue are re-authored; their tiles are from the supplied Emerald/Arena world pack. Authentic Platinum field NPCs and system font are converted from the supplied decomp sources. Layered trainer artwork is from the supplied character-creator pack; provenance/TRAINER-LAYER-REFERENCES.md preserves the known source information. New manifest files record hashes and selections. Original source archives are unchanged.

Private converted PNGs, atlases, screenshots and executable are included only in the protected private download. Public plaintext source has code/manifests, not these graphical assets.
