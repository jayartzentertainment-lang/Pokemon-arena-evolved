# Editable trainer source-art compositor

This Python tool combines the uploaded matching body, hair, clothing, headgear
and accessory sheets. It includes a simple local selection menu and a headless
command-line exporter. It does **not** modify, compile or run Pokémon Platinum.

## Run

Use Python 3.10 or newer and Pillow (`python -m pip install -r requirements.txt`).
For the optional menu, Python also needs Tkinter, normally included with Windows
and macOS Python installers (Linux commonly packages it as `python3-tk`).

```sh
python composer.py gui
python composer.py list
python composer.py list --layer hair
python composer.py render --body A --skin 0 --hair 1/0 --top 1/0 --bottom 1/0 --hat 0 --accessory 0 --output exports/trainer.png --preview exports/preview.png
python composer.py render --profile exports/trainer.profile.json --output exports/edited.png
```

The menu shows a stationary source-art frame. Change direction/frame to inspect
each view; it is not animated gameplay. Export writes the full walking sheet,
an editable `.profile.json` recording selections, and an `.export.json` receipt
with source hashes and final palette/alpha counts. The preview option exports
one frame enlarged with nearest-neighbour pixels. All original PNGs are retained
byte-for-byte in `sources/`; exports cannot overwrite that directory.

## Edit the catalogue and alignment

`catalog.json` is the editable catalogue. IDs such as `hair 1/0` preserve the
upload's folder numbering; these are not PokeMMO item IDs or invented official
hairstyle names. A/B are the source's two body shapes. Four skin/base colour
variants and each matching A/B clothing file are mapped explicitly.

The source grid is four columns by four rows of **32x48** cells. Rows face
down, left, right, up. Body and overlay pixels keep their original coordinates;
no layer is independently resized, trimmed around its contents, or recentered.
Four 128x196 input sheets have four empty bottom rows; only those transparent
rows are omitted from the composed 128x192 walking sheet. The 128x128 example
poses are retained as sources but are not used as walking layers.

Each option has an `offsets` object. Default `{}` means use the authored
coordinates. To move one hat one source pixel up for all bodies and another
pixel left only for body A facing up, edit that hat's entry:

```json
"offsets": {"all": [0, -1], "A.up": [-1, 0]}
```

Keys may be `all`, a body (`A`/`B`), a direction, or `body.direction`.
Applicable offsets add together. They apply identically to all four columns
for that direction, not to the sheet as one moving strip. The exporter rejects
offsets which would clip visible pixels outside their fixed animation cell.

`layer_order` begins with body and contains bottoms, tops, hair, headgear and
accessories exactly once; change their order when the artwork requires it.
Using a common canvas prevents accidental independent placement, but does not
prove that every hair/hat/clothing combination has correct occlusion. Some
combinations need bespoke masks, rear/front layers or adjusted source pixels.
This tool does not invent those missing drawings or guarantee every combination.

## Platinum integration remains separate

The ROM is native C; it will not execute this Python script. A later adapter can
use this catalogue and compositing rules to rebuild/cached-compose a selected
trainer when a menu choice changes, then feed its graphics to the native rig.
Source bodies are 36–38 pixels tall inside 48-pixel cells, whereas the current
Platinum rig has 32-pixel frames. That conversion needs a deliberate shared
scale/anchor decision. Shrinking hair and body independently is not appropriate.

Exports retain full source RGBA colours. The receipt reports whether the
composition exceeds 15 visible colours and whether it has partial alpha; native
RGB555/4bpp conversion and palette budgeting are **not** implemented here.
Running, bike, surf, fishing and complete battle/back/throw states still need
explicit artwork/adaptation. Saved selections here are desktop JSON only; the
ROM needs its own compatible versioned save encoding and menu integration.

## Source provenance

PNG source: user upload `Character Customization.zip`, 1,550,010 bytes,
SHA-256 `7c9fda08aa35b4177c79449bfeb3623b31320097e1c1c299391ec568929d3d12`.
The archive contains 158 PNGs, including duplicates, empty choices and example
poses. File counts do not equal the number of unique selectable styles.
No artist/license text was included; preserve the originals and add verified
credits/terms when available. Public catalogue data and atlas tools do not
establish a connection between these image filenames and PokeMMO item IDs.

`validation.json` records the limited tool-only checks actually performed.
No ROM build, emulator or game playtest is part of those checks.
