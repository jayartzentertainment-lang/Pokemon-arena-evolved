# Platinum Arena 0.9.6 — current build delivery

The user requested the compiled build immediately, before further emulator QA.
This is the new 0.9.6 candidate, not the older 0.9.5 download.

## 1. Implemented features
Native Platinum story foundation, live1v1/2v2, a genuinely layered trainer
creator, textured Platinum battle-menu art, four learned touch moves,
resource/status data, sourced VFX and wind dash presentation.

## 2. VFX coverage
Thirty-three retained supplied files support13 families: fire, water,
electric, ice, contact, guard, clash, slash, leaf, psychic, shadow, buff and
debuff. Source Swift stars and continuous beams remain. Two sourced wind
frames follow dash direction. NativeGUI graphics come from the supplied
Platinum source's pl_batt_bg.narc. Original source/hash/crop/anchor/credits
manifests are retained. This is selected coverage, not every move's exact art.

## 3. Battle changes
Species-aware motion/recoil, authored Venusaur Hurt/Charge, entry/faint
presentation; heavy startup/recovery +12.5%, seven persistent stages, major
statuses/confusion with central clocks, learned PP/cooldowns, PP-free natural
attacks, stat/type/timing beam contests, observed-state AI and allied tactics.
Double-tap dash remains; individual cooldowns survive party switching.
The bottom screen has actual native panels rather than plain white buttons.

## 4. Character creator
Both bodies have4 hairstyles,3 tops,3 legwear choices and3 hat choices, plus
independent skin/hair/top/legwear/shoe dyes. Large full-body portrait and
directional preview, Identity/Appearance/Clothes tabs, touch/button controls,
randomize/reset/facing/optionalwalking, confirmation and native naming.
663,552 bounded custom identities use CRC5 in the existing four savepadding
bytes; old records remain compatible and PlayerSave remains44bytes.
Actual field/card/battle hooks use the appearance. Native save/reload on this
ROM has NOT yet been observed in an emulator; host record roundtrips passed.

## 5. Testing
PASS: native Metrowerks compile/link and stable compiled-source hashes.
PASS: eight core tests with ASan/UBSan, controls, party bridge with status and
manual replacement/cooldown preservation, rewards, legacy/custom save records,
layer bounds/unique silhouettes/palette isolation, deterministic conversion.
The updated capture regression check FAILS: holding the new natural button
can start an action during the short post-capture recovery, delaying the next
special. Capture transaction tests passed before this recovery assertion.
No claim of complete capture regression acceptance is made.
Emulator creator/name/field/save/reload, actual1v1/2v2 visuals and real hardware
remain PENDING. No older runtime result is reused as this ROM's proof.
OBJ budget65344/65536bytes; BG art uses a separate native memory bank.

## 6. Playable build
Extract Platinum-Arena-0.9.6-INTEGRATION.nds from the gameZIP and open in a DS
emulator. Exact ROM hash and size are in BUILD.json and RELEASE.json.
Editable source is a separate SOURCE.zip, based on the pinned upstream commit.

## 7. Remaining issues
The capture-recovery issue above, elemental hazard pools, global followers,
full reaction/form coverage and advanced retail move/ability/weather/item
scripts remain unfinished.81statusmoves have native adapters;89statusscripts
remain unsupported rather than receiving invented guard/slow behavior.
Missing trainer special gestures use documented same-character adaptations;
the naming keyboard's tinygender icon remains Lucas/Dawn. Remapping is
session-only. Full campaign, hardware performance and save continuation need
user playtesting. The master vision is not claimed complete.
