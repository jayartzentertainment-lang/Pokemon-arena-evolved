# Native3DS integrated adventure

Current implementation: `/workspace/platinum-arena-3ds-story` (public source prefix `native-3ds/`). Preserve `/workspace/platinum-arena-3ds`, `/workspace/platinum-arena-3ds-next` and the DS integration under `/workspace/platinum-port/pokeplatinum`.

Read README.md, docs/ADVENTURE-STATUS.md and docs/VALIDATION.md. Use tools/test_host.py and the pinned official Docker image via tools/build_native.sh. Graphics are intentionally excluded from public plaintext source. The privately delivered editable-asset ZIP restores gfx/private and allows rebuilding without downloading source ROMs.

New story runtime: adventure.c/adventure_save.c; content/campaign.json; prepare_adventure.py. Combat keeps the existing fixed60Hz engine and adapters; use Game_StartParty for six-slot story teams, while Game_Start preserves the former four-slot lab baseline. Saves live at sdmc:/3ds/platinum-arena/adventure-v1.sav with a .bak checkpoint. Do not modify user saves for a playtest; use isolated emulator data/config directories.
