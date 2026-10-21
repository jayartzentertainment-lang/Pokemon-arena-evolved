# Validation — native3DS integrated adventure0.3.0

This report separates portable state checks from observed native emulator play. It does not claim a full human campaign playthrough or physical hardware profiling.

## Host checks

11/11 suites passed with AddressSanitizer and UndefinedBehaviorSanitizer: the8 retained battle/physics/navigation/Platinum/2v2 suites, native party adapter, aiming/status/beam-quality suite, and new adventure suite. The selected preserved DS source hashes also match.

The adventure suite covers all493 base-species data/config lookup, mutable trainer choices, walking/follower trail, walkable links/story-marker/grass reachability on34 maps, six-slot switching, defeat/retry/retreat, capture, level evolution, all41 story flags,8 badges and Champion ending, bounded save encoding/CRC rejection and recovery from a corrupt primary using a known-good backup.

The scripted campaign walk constructs28 battle encounters, including4 actual2v2 setups, and **injects their outcomes** to verify event progression. This is not an end-to-end gameplay victory claim. The existing core suites exercise real battle resolution separately.

## Actual native execution

The exact final `.3dsx` identified in reports/build.json was opened in Azahar2126.2 with original3DS configuration, normal CPU setting, stereo disabled and software OpenGL. Input came through actual emulator keyboard/touch controls; game memory and save progress were not injected for this check.

Observed: stationary layered trainer preview, changing actual hair pieces, naming the trainer ARENA, that same trainer walking in Twinleaf/Route201, dialogue and story progression, authentic Sinnoh starter selection, Turtwig/Chimchar live battle handoff, return to the adventure, on-disk save and Continue, and a four-actor2v2 battle in the separate full-roster lab. Screenshots are actual emulator captures. A fresh-process Continue check is recorded in reports/emulator-results.json.

The lab and adventure save were checked separately: lab entry does not advance story objectives. The opening reaches the battle naturally via NPC interaction. Native UI uses the supplied Platinum system font and window-border pieces with corrected indexed transparency. A single menu press is consumed once rather than being replayed across render frames between simulation ticks.

The native FPS/CPU/GPU HUD uses emulated time; it is not proof of real original/New3DS performance. Source conversion and the binary build succeeded within original3DS-oriented texture limits; the hair atlas is split by body and Pokemon textures use a bounded8-sheet cache. Physical hardware, exhaustive493 visual review, complete move-mechanics/ability/item parity, and full human story balancing remain pending.

No audio listening test: NDSP was unavailable in this emulator and the game displayed SFX OFF. Audio remains optional original prototype cues. No visually confirmed beam-clash playthrough or full campaign completion is claimed for this release; inherited/native core checks cover clash formation and ownership.

Previous native0.1/0.2 source inputs/binaries and the DS0.9.6 integration binary were verified unchanged. Build, host, emulator and preservation details are included under reports/.
