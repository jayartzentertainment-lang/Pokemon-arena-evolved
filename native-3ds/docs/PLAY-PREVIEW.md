# Native 3DS 0.4.0 playable preview

This is an opening-and-combat preview. It does **not** contain a completed,
playable full Platinum campaign. Later required event services and original
renderer/menu systems are unfinished. Unsupported events stop visibly; they do
not award invented progression. Sandgem and all later campaign content are not
promised playable. See IMPORT-COMPLETION.md.

## Install

Extract the protected download with 7-Zip or ZArchiver using the password sent
privately with the download. It contains everything the app needs at runtime;
no source conversion, compilation or additional ROM is needed to try it.

- On a homebrew-enabled 3DS, copy the enclosed `3ds/` folder to the SD card root,
  merge folders if asked, then open **Platinum Arena 3DS Preview** in Homebrew
  Launcher. This is a `.3dsx` homebrew app, not an installable CIA.
- In Azahar, extract first, then open
  `3ds/platinum-arena-preview/Platinum-Arena-3DS-Prototype.3dsx`.
  It requires a 3DS emulator, not a Nintendo DS emulator.

Runtime saves and session logs live in `sdmc:/3ds/platinum-arena-preview/`.
Older `sdmc:/3ds/platinum-arena/` files are never written by this preview.
No save files are bundled. To test an older compatible save, make a backup
and copy it into the preview folder; legacy condensed-story flags are not
converted into fabricated original-story progress.

## Try the creator and opening

Choose **PLATINUM OPENING / WIP**. At the intro, Start+Select opens the creator
immediately. Custom starts from the supplied Gen IV-style blank male/female
body. Add the supplied hair, top, legwear and hat layers. Red and Brendan use
fixed hairstyles with colour-only edits. L/R rotates a stationary preview;
Y edits the name and Start confirms. Your appearance is used in the imported
original bedroom and supported opening events. The native field renderer
still uses orthographic conversions, not Platinum's original perspective.

## Try 1v1 and 2v2

Choose **BATTLE LAB /493 POKEMON** on the title screen.
Left/Right chooses a species; L/R skips ten entries; Up/Down chooses which side
to edit. A starts 1v1; X starts 2v2. Y toggles beam-clash practice.

- Circle Pad/Dpad: movement. Select: toggle opponent lock / physical free aim.
- Y: natural strike. A: selected learned move. X: selected special move.
- B while moving: dodge. B while standing: guard; stamina limits defence.
- L+A or L+X: cycle learned slots; L+Y: cycle natural strikes.
- R tap/release: switch party. R+Y or TARGET: switch opponent target.
- Start: pause. B from the paused lab battle returns to Lab.
- Start+Select: exit the app outside the intro.

Azahar's test mapping: arrows = Circle Pad; T/G/F/H = Dpad; X/A/Z/S =
3DS Y/A/X/B; Q/W = L/R; M/N = Start/Select. Your own mapping may differ.
The lower-screen touch aiming pad is removed; move cards remain touchable.

The app includes 493 base species, supplied directional animations and move
VFX, species balancing, cooldown/PP/stamina UI and the current Arena combat.
It does not claim every original move, held item, ability or animation is exact.
Physical 3DS performance and a complete human campaign playthrough are unverified.
See the exact build and emulator observation reports included with the download.

## Rebuild

Public source is in `native-3ds/` on the matching preview branch. The protected
editable-input archive in the earlier source-progress branch restores its
private data/artwork; those inputs are unchanged for this preview. Restore
both into the same native project directory, then run:

```sh
python3 tools/test_host.py
bash tools/build_native.sh
```

The official devkitARM Docker image is pinned. The earlier approved releases
and source-progress snapshot remain preserved.
