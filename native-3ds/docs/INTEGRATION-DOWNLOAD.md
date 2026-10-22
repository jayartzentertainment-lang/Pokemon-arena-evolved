# Native 3DS integration development package

**The full original Platinum campaign is unfinished.** This package contains
the movement/contact/dash integration stage, the existing creator and Battle Lab,
and the previously supported imported opening. It is not the completed game.

Extract the outer AES ZIP with 7-Zip using the password supplied privately.
For play, copy its `3ds/` directory onto a homebrew-enabled Nintendo 3DS SD card
and launch the `.3dsx` through Homebrew Launcher. In Azahar, open that `.3dsx`.
The assets are embedded. No original commercial ROM is included.

Saves remain in `sdmc:/3ds/platinum-arena-preview/`, separate from the approved
older native releases. Back up that development folder before upgrading it.
Normal gameplay has no developer overlays or artificial battle outcomes.

Title: PLATINUM OPENING / WIP starts the imported original opening; BATTLE LAB
supports 1v1/2v2 and the existing species selections. The original campaign stops
where unsupported services/content remain; the condensed adventure is not used
to replace it. See `IMPORT-COMPLETION.md` for exact limits.

Controls retain the existing physical targeting, lock-on, move selection,
learned-move buttons, guard/dodge and switching. The bottom-screen move cards
are actionable. The unwanted touch aiming pad remains removed. Start pauses;
Start+Select exits except during the new-game introduction, where it opens the
creator. Controls and cooldowns are displayed by the existing battle HUD.

## Editable source and build inputs

Extract `EDITABLE-SOURCE.zip`, then `BASE-PRIVATE-INPUTS.zip`, then
`NEW-PRIVATE-INPUTS.zip` into the same working directory. The baseline input
archive uses the same private password; the source/delta archives inside this
protected package are ordinary ZIPs. All three create `native-3ds/`. Their
SHA256SUMS inventories distinguish source, baseline and updated inputs.
The baseline archive's older readme describes source-only work; this outer
package also includes the separate native executable.

From `native-3ds/`:

```sh
python3 tools/prepare_dash.py
python3 tools/test_host.py
bash tools/build_native.sh
```

The asset converter requires Pillow. Building requires Docker and the pinned
official devkitPro SDK image selected by `build_native.sh`. The first build
converts the PNG atlases and can take several minutes. Original Platinum asset
regeneration additionally requires the user's supplied decomp inputs and the
tools documented in `IMPORT-COMPLETION.md`. Those inputs are never downloaded
from a commercial-ROM site.

Private artwork stays in the protected testing package. It is not licensed for
public redistribution by inclusion here. Public editable code is on the separate
`codex/native-3ds-full-integration` branch; previous releases remain preserved.

`COMBAT-INTEGRATION.json` records the exact executable hash and distinguishes
host checks, observed emulator gameplay and unavailable physical hardware.
Phase 2–6 work remains: full appearance/pose integration, original field services,
remaining maps and event compatibility, move-mechanics completion, the original
story through League/postgame, and final balancing/performance work.
