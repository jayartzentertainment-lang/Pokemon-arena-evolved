# Platinum Arena Evolved — native 3DS original integration (unreleased work)

**Playable release withheld; source work available for review:** the user rejected the earlier custom field/story presentation. See `docs/DEVELOPMENT-CORRECTION.md`. This is unfinished source work, not the completed requested game.

This separate work begins importing **original Pokémon Platinum content** into the native Arena engine. The complete original campaign is unfinished. It does not present the old 34 condensed locations and 41 rewritten objectives as an authentic Platinum campaign. The approved v0.3.0 release remains unchanged.

The opening uses original supplied maps, original event instructions and dialogue, original NPC placements, original trainer teams and original new-game flags. The converter retains raw source data and produces native tables; a bounded compatibility interpreter executes supported commands. Unsupported events stop visibly instead of inventing dialogue or awarding progression.

## What changed

- Custom starts with the supplied Gen IV-proportioned male/female blank body from `pokemon png.rar` and no optional layers selected. Red and Brendan are fixed sprite bases with colour-only edits, matching the DS approach. See `docs/CHARACTER-CREATOR.md`.
- Original Rowan introductory text/background, original proportional message font and message-box tiles, original pre-Pokétch lower-screen tilemap, stationary modular character creator at the original selection point, and an intro-only Start+Select shortcut directly to the creator. The same chosen name and appearance enter the original upstairs bedroom.
- 18 original map headers and 32 converted geometry cells: Twinleaf and interiors, Route 201, Verity Lakefront, both relevant Lake Verity variants, Sandgem and initial interiors. These are imported layouts. Original Nitro geometry and textures are baked into a top-down view; this is not Platinum's original animated perspective renderer.
- Original transition/load/frame/coordinate scripts, walking sequences, camera pans, house warps, new-game flags, Rowan/Barry starter sequence and original first rival team, connected to Arena combat. All trainer teams and original growth tables are retained. Imported data coverage exceeds current runtime coverage; see the import report.
- Separate original-story save alongside appearance/party data, CRC validation and backup recovery. Existing v0.3.0 saves remain usable in their legacy campaign; they are not mapped to invented original-story flags.
- Sticky opponent lock. Select unlocks to physical Circle Pad free aim. The lower-screen aiming pad is removed. Target selection remains available in doubles.
- Larger readable move cards showing assigned buttons, PP and cooldowns, stamina and party indicators, using the current Arena HUD assets. The faithful original field font/windows are now imported separately; matching the whole original interface remains unfinished.
- Slower stat-based movement, especially Blastoise; species-based stamina/defensive resistance, learned and natural attack cooldowns, stationary short strikes and explicit travel only for charge moves.
- Dedicated directional Hurt poses for all 493 base species, loaded only for active fighters, plus corrected full-size Arceus animation sheets. The pinned SpriteCollab source and per-species credits accompany the protected editable assets. Original source hit-frame timing and white shadow anchors are preserved.
- Existing supplied Emerald Flamethrower/beams and other converted move VFX remain. Protect uses supplied animated cels at the defending side.

## Play

Open `Platinum-Arena-3DS-Prototype.3dsx` in Azahar, or copy `3ds/platinum-arena/` to a homebrew 3DS SD card and use Homebrew Launcher. This is native 3DS homebrew, not a Nintendo DS `.nds` ROM or Android APK.

Choose **ORIGINAL PLATINUM** for the new import. Choose **BATTLE LAB** to try any of 493 base species in 1v1/2v2. The fourth title item describes the import status. Continue prefers the separate original-story save when present; otherwise a valid legacy v0.3.0 save remains playable.

- Original field: Circle Pad/Dpad moves; A talks/advances dialogue; hold B to run after the original Running Shoes event. Start opens the Arena overlay menu for saving, appearance and party.
- Intro: Start+Select immediately opens the creator. Elsewhere Select+Start exits the application.
- Creator: BASE selects Custom, Red or Brendan. Custom offers matching Gen IV body/hair/clothes/hat layers; regional bases offer colours only. Up/Down selects a row; Left/Right or A changes it; L/R rotates a stationary preview; Y edits the name; Start confirms. Touch rows also work.
- Battle: Y natural strike; A selected learned move; X selected special; moving B dodges, standing B guards. Select toggles LOCK/FREE. R+Y or TARGET cycles opponents. Touch move cards cast moves, without touch aiming.
- L+A / L+X cycles the corresponding learned slot; L+Y cycles natural attacks. R tap/release switches party; R+X changes the AI ally tactic. Start pauses. During original trainer battles B from pause resumes; trainer encounters cannot be escaped through that menu.
- Lab: Left/Right selects a Dex entry; L/R skips ten; Up/Down selects the team being edited; Y toggles clash practice; A starts 1v1; X starts 2v2; B returns to title.

Azahar defaults: arrows = Circle Pad; T/G/F/H = Dpad; X/A/Z/S = 3DS Y/A/X/B; Q/W = L/R; M/N = Start/Select.

## Reproduce the native build

For the source-progress snapshot, extract WIP-SOURCE.zip and WIP-PRIVATE-ASSETS.zip into the **same** directory; both create `native-3ds/`. The encrypted private-assets package contains the original-content generated private C tables, raw imported source data, converted PNGs and Hurt source/credits required to reproduce this build. The public source ZIP deliberately excludes private artwork/dialogue tables.

```sh
python3 tools/test_host.py
bash tools/build_native.sh
```

The build uses the pinned official image `devkitpro/devkitarm@sha256:116afba8df8453961de2936ffab20dd441edf4d682856c1ec8b0e53d7ed0bbf5`. See `docs/IMPORT-COMPLETION.md` for original-data conversion commands and exact limits. No commercial ROM download is performed.

## Current limits

The full original adventure is **not yet playable**. Supported original commands, services, map coverage, source-based event checks and actual emulator observations are reported separately. Unsupported events stop; original shared-script services, bag/item/wild encounter systems, full BDHC height/directional collision, NPC autonomous movement, naming/nickname services, original intro animation, Pokédex services and later campaign progression still need porting. Do not assume that every imported Sandgem event executes to completion.

Trainer teams retain species/levels/moves/IV scales, but complete abilities, held items, natures/EVs, alternate forms and all exact move mechanics are unfinished. Original battle-to-field transitions and trainer poses are not fully reproduced. Original field audio commands currently have no commercial sound-engine backend; existing optional synthesized Arena combat cues remain. Physical 3DS performance and a full human playthrough are unverified.

Read `docs/VALIDATION.md`, `docs/IMPORT-COMPLETION.md` and the included machine-readable reports. Legacy condensed-campaign files are retained solely for v0.3.0 save compatibility and regression coverage.
