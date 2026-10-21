# Platinum DS overhaul — first source pass

**Source update and asset previews. No new ROM has been built.**

[Download editable source update](./DS-Visual-Overhaul-SOURCE-UPDATE.zip) ·
[View all effect cels](./effect-sprites.png) · [Animated assets](./effect-loop.gif) ·
[View beam cels](./beam-sprites.png) · [467-move table](./move-visuals.csv)

The direction is an original DS-style Platinum arena: retain the existing
story and responsive combat, then improve sprites, effects and presentation
within DS limits. Matching the reference videos exactly is no longer required.

This source pass connects 16 additional shared effect families (64 converted
cels), maps all 467 moves explicitly, removes the dispatch paths for 35 generic
orb projectiles and 32 unrelated Swift-star assignments, and replaces nine
neutral water-shaped beams with themed component chains. Swift keeps its
dedicated star. Not every move has unique art, and existing body-motion attacks
retain their authored Pokémon poses.

Flamethrower uses the fuller supplied Ranger107 strip and Hydro Pump uses
Ranger105. Ground/wind/lightning projectiles face their travel direction;
projectile/contact heights agree. Windup events no longer masquerade as hits,
and actual heavy/basic phase clocks synchronize charge and follow-through.

The reserved main sprite VRAM remains 65,344 of 65,536 bytes. Sixteen previously
unused image slots and an unused effect palette bank are reused. Four actor
and beam/shield slots remain independently available for 2v2. Final linked RAM
use and frame rate are not established by a source allocation check.

Six affected native translation units compile independently. The small
presentation checks and existing combat-polish checks pass with memory/undefined
behavior sanitizers; retained source regeneration produces identical tile bytes.
There has been **no native ROM link, emulator run or hardware playtest**.

## Using the source

This is an incremental update for the previously delivered **0.9.6 source**,
not a patch to apply directly to an NDS file. The ZIP contains complete changed
files under `pokeplatinum/`, a review patch, checksums and per-file baseline
hashes. Preserve local edits and review those baseline hashes before copying
the included files over the matching paths in that source tree.

Edit `tools/arena/generate_presentation.py` to change move-family assignments;
the CSV is a readable report. Selected original art and crop/anchor metadata
are in `res/arena/ds-overhaul/`. Regeneration and limitations are documented in
that folder's README. The package contains no ROM, proprietary SDK or compiler.

The previews decode converted native tiles. They are **asset previews, not
screenshots of this update running in the game**. Some longer source sequences
are deliberately adapted to four cels, and the palette is reduced for DS use.

## Remaining work

This pass does not fix the Lucas/custom-creator entry issue, add trainer parts,
redesign the bottom-screen UI, replace the 64 alternate-form portraits or add
unsupported retail move mechanics. The known capture-recovery input limitation
is also unchanged. These remain separate work for a later explicit prompt.
