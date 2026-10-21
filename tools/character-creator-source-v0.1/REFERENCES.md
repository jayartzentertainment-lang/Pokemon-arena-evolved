# Layered character creator: verified scope and engine fit

21 October 2026. The user asks whether the uploaded layers suffice, endorses
an automatic image-layering script with aligned hats, and supplies two more
repositories for source intake. Existing no-ROM-build instruction remains.

## What the supplied artwork enables

The 158-PNG Character Customization archive supplies two body shapes with
four skin variants, and separate walking hair, tops, bottoms, headgear and
accessories. It is sufficient to implement an initial standing/walking
creator with actual shape changes. More source hunting is not needed merely
to start that work. It does not establish complete native cycling, surfing,
fishing, running, map-action and battle-back/throw animation coverage.

Use the real grid: four columns, four direction rows (down, left, right, up),
32x48 pixels per cell. Preserve the shared canvas and registration across
layers. Extra four transparent bottom rows in four sheets are padding. Do
not independently crop or centre heads, hats and bodies. Optional explicit
per-direction/per-body offsets support correction, but cannot guarantee every
hairstyle/headgear combination looks right. Some combinations require authored
occlusion masks or artwork corrections rather than translation.

## Suitable implementation for our game

- This is the native `pret/pokeplatinum` C engine. It does not execute Python,
  Pygame, C# or Pokemon Essentials scripts inside the ROM.
- A small editable Python/Pillow tool can select and compose the source PNG
  layers, preserve 32x48 registration, export sheets and save a versioned JSON
  selection. A native C implementation later consumes converted DS-ready
  layers/frames and caches the selected appearance when menu choices change.
  Parsing PNGs or repeatedly doing full compositing on every gameplay frame
  is unnecessary for the native game.
- Field texture names/order are north, south, west, east; the source direction
  rows need explicit remapping. Current field frames are 32x32, 4bpp; uploaded
  body pixels reach 36-38 high. Native allocation/rig adaptation or deliberate
  rescaling must be designed without clipping heads/feet. Any scaling must
  affect a whole assembled cell, keeping its parts registered.
- The combined character, not just each separate sheet, needs a palette
  budget. Two accessory sheets have a few partially transparent pixels.
- Existing saved customization uses four PlayerSave padding bytes: a 16-bit
  version tag plus 11 value bits and a five-bit checksum. It holds only the
  current base/colour selections. Expanded layer IDs need an explicit
  versioned persistence design with legacy fallback; do not append IDs into
  unrelated native save fields.
- Preserve editable PNGs and a human-readable catalogue of IDs, source paths,
  layer order and optional offsets. PokeMMO item IDs have no demonstrated
  mapping to these source filenames.

## Newly reviewed repositories

| Source | Pinned revision | Useful findings | Limits |
| --- | --- | --- | --- |
| [PokemonUnity/pklibrary](https://github.com/PokemonUnity/pklibrary) | `64a1c104a1551667acbd6b26348d04998e388b9c` | C# framework; `PlayerOutfit` stores outfit/shirt/pant/misc/hat IDs. | No working PNG-layer compositor or complete creator in inspected relevant classes. Equality operators are unfinished constant results; do not copy them. Not native DS C. |
| [MrArnaudMichel/Projet_Pokemon](https://github.com/MrArnaudMichel/Projet_Pokemon) | `f62dc04fcdadb5633fb74b3776a6f5e592a2010c` | Python/Pygame tutorial; direction mapping, bottom-centre positioning, menu/save concepts. | Uses precomposed trainer art and fixed 24x32 crops, incompatible with blindly importing our 32x48 layers. No layered hair/hat/clothing creator in inspected latest player/menu/save path. |

Detailed source snapshots, licences, hashes and reviews are retained in
`pklibrary/` and `Projet_Pokemon/`. No downloaded source was executed, no
external dependency was installed and no library was integrated or built.

## Editable tool work

The isolated Python compositor is provided in
`/workspace/platinum-port/customization-tools/`. It is an asset-authoring tool,
not a changed NDS ROM or a completed in-game creator. Keep any source-only
delivery and tool validation separate from game/compiler/runtime evidence.

It includes a command-line exporter, optional Tkinter selection menu, editable
catalogue and offsets, all 158 unchanged source PNGs, JSON selection profiles
and example outputs. Limited source-tool export/registration checks passed
for both body shapes, including a nonempty hat. The desktop GUI was not run.
The selected examples contain 13 and 17 visible RGB colours respectively;
the latter still needs native palette adaptation. See the tool's
`validation.json` for actual checks and limits.

No native game implementation or ROM build is authorized by these intake
notes themselves. The user remains responsible for gameplay testing.
