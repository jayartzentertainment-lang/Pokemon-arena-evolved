# GitHub source progress

This branch contains editable **unfinished native 3DS work**, not the completed
requested game. The original Platinum story must be playable with the chosen
trainer and original presentation. The full native port does not meet that
requirement yet; no new playable release is published with this source snapshot.

The current source includes original-content import/compatibility work,
combat/HUD changes, original field text/graphics intake, supplied Gen IV blank Custom
characters and matching layered options and fixed Red/Brendan sprite bases with colour-only edits. Consult
CHARACTER-CREATOR.md and IMPORT-COMPLETION.md for exact implementation and gaps.

The main branch and approved native/DS releases are preserved. All current code
is under `native-3ds/` in this separate WIP branch. The `progress/` folder holds:

- `Platinum-Arena-3DS-WIP-SOURCE.zip`: public source, conversion tools, tests and reports.
- `Platinum-Arena-3DS-WIP-PRIVATE-ASSETS.zip`: encrypted converted artwork, retained original inputs, credits and generated private tables; use the privately supplied existing archive password.

Extract both archives into the same directory; both create `native-3ds/`.
The private companion is needed for rebuilding because it restores exact atlas
inputs and original-content tables deliberately excluded from public code.
Neither archive contains a game ROM or native executable. Do not treat these
source packages as an emulator-ready game download.

From the restored project directory:

```sh
python3 tools/test_host.py
bash tools/build_native.sh
```

The native build image is pinned in the build script. Original source/converted
asset hashes and actual checks are recorded in provenance/ and docs/reports/.
The earlier rejected custom field UI preview is not distributed as a finished
story integration. The remaining work is described in DEVELOPMENT-CORRECTION.md.

The later playable-preview branch adds an explicitly limited opening/Battle Lab download and isolated preview saves. Its current code is in native-3ds/; the earlier WIP-SOURCE.zip remains a historical source-only snapshot. See PLAY-PREVIEW.md. The full original campaign is still incomplete.
