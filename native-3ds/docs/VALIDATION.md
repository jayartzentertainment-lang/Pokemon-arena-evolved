# Validation — native 3DS v0.4.0 development preview

The original-content opening is tested separately from the retained v0.3.0 condensed campaign. No full original campaign, original-ROM behavioural parity or physical hardware performance is claimed.

## Host checks

14 suites pass with AddressSanitizer, UndefinedBehaviorSanitizer and warnings treated as errors. They cover the retained eight combat/navigation/physics/Platinum core suites, the legacy adventure suite, native party/input adapter, sticky lock and physical free-aim/HUD hitboxes, native species balancing and the original-content opening interpreter/checkpoint.

The new source-based opening test walks original tiles and warps, executes original rival/Mom/Rowan/starter instructions, configures the original first rival team, resumes the original Running Shoes sequence after a **host-injected victory**, follows Barry across the original map edge and completes the original Lake Verity Cyrus movement/camera/dialogue event. This is not a played battle victory or a full reference-ROM comparison.

Native balancing checks slower Blastoise speed, species stamina, natural-attack spam cooldown, stationary Scratch, genuine Tackle travel and high-power stamina/cooldown costs. Existing core checks cover beams, clashes and 2v2. The dedicated Hurt importer validates all 493 sheets and their original white shadow markers, preserving source durations and hit-frame timing. Exhaustive visual review of every species is pending.

## Native verification

Actual native emulator observations and the exact candidate hash are in `reports/emulator-results.json`. Source-based checks and emulator observations are distinguished. No audio listening or physical 3DS performance claim is made; the emulator may lack NDSP and display SFX OFF. Emulated FPS/CPU/GPU times are not measurements of real original/New 3DS hardware.

The pinned official devkitARM build produces an ARM11 ELF and native 3DSX. Original map textures use a four-cell streaming cache; ordinary Pokémon textures and their Hurt sheets share eight LRU slots and are loaded only for Pokémon in use. All retained sprites are present on RomFS without loading the entire roster into RAM.

Selected DS source hashes and approved native/DS release binaries are checked separately in `reports/preservation-final.json`. See `IMPORT-COMPLETION.md` for unported systems and honest import coverage.

The creator suite additionally verifies Gen IV blank new games, both body/layer ranges, fixed Red/Brendan colour-only rows, pointer input, original-intro identity handoff, version2 appearance persistence and an actual retained synthetic v0.3 format1 save fixture. Current creator observations are in reports/emulator-results.json; they do not verify a completed native original-story port.

The Gen IV update was observed in Azahar: blank defaults, real supplied layer changes, body/rotation previews, fixed regional colour controls, and the dressed Custom trainer in the imported bedroom TV event. The original full campaign and original perspective renderer remain unfinished. Screenshots are local/private; the public report identifies observations without redistributing the images.
