# Platinum Arena 0.9.6 — supplied assets integrated

This update is built on the existing native Nintendo DS Platinum port. The
published 0.9.5 ROM and editable source remain preserved as its baseline.
The latest user-supplied directive replaces the older no-testing request with
focused verification. Compile, host checks, emulator observations and untested
campaign/hardware coverage are recorded separately in the delivery report.

## Character creator

- The supplied trainer PNG layers now compose actual different bodies, four
  hairstyles per body, three tops, three legwear styles and three hat choices.
  Skin, hair, top, legwear and shoe dyes are independent. These are visible
  shapes and colours, rather than the old four-row recolour-only interface.
- Identity, Appearance and Clothes pages use native text and touch/button
  navigation. A large full-body portrait accompanies a four-facing field
  preview. Standing is the default; walking can be enabled explicitly.
- Previous/next, facing, randomize, reset and confirmation update the actual
  composited art. Native Platinum naming follows confirmation.
- All 663,552 custom combinations have a bounded save representation with
  CRC protection in the same four previously audited padding bytes. PlayerSave
  remains 44 bytes; previous Sinnoh/Hoenn/Kanto avatar records remain readable.
- Field, card and battle hooks use the chosen custom identity. Walking,
  running and cycling have authored layers. Missing fishing/surf/scripted
  gestures use explicitly adapted same-character poses, not default Lucas/Dawn.
  The naming keyboard's tiny body icon remains the original gender icon.
- Reproducible source converters, full source hashes, layer bounds and visual
  proofs are retained under `res/arena/avatar/` and `tools/arena/`.

## Battle interface, creatures and effects

- Genuine Platinum command-key, healthbox and Poké Ball artwork is decoded
  from the supplied native source's `pl_batt_bg.narc`. Artwork and transparent
  native-font text use separate subscreen BG layers. Four learned moves are
  touchable, with PP and cooldown data; party, bag and controls have touch links.
- Creature motion includes species-aware floating/heavy/agile presentation,
  grounded shadows, directional turning, visible hurt recoil, guard poses,
  fainting and a short Poké Ball deployment presentation. Supplied Venusaur
  Hurt/Charge frames are cached; other species use documented compatible poses.
- The supplied Diamond/Pearl, Ranger, PMD and KOF sheets supply a 56-frame atlas
  with fire, water, electric, ice, contact, guard, clash, slash, leaf, psychic,
  shadow, buff and debuff families. Two thin layered wind frames follow dash
  direction. Swift retains its actual sourced stars, and approved continuous
  beams and existing move sound mappings remain.
- Thirty-three original effect files, crop/anchor/timing manifests and credits
  accompany the importer. Suitable selected effects are used; the complete
  source inventories are not claimed to be complete move-specific coverage.
- Main OBJ allocation is 65,344 of 65,536 bytes. New menu artwork uses BG
  memory, effects reuse the existing atlas, and no per-frame filesystem read
  or new particle heap allocation is introduced.

## Controls and combat

- Defaults follow the new directive: Y natural attack, A selected learned
  attack, X special/status, B guard or directional dodge, L+Y natural cycle,
  L+A learned-attack cycle, short R tap/release party switch, held R+X team
  command, Select aim, Start pause. All four equipped moves remain directly
  accessible on the touchscreen. L+R catching retains priority.
- Direction release followed by a second press within 12 ticks retains the
  requested double-tap dash, using the same stamina/cooldown rules as B-dodge.
- Natural/attack/special/aim bindings can be swapped among Y/A/X/Select;
  B/L/R remain reserved. Modifier chords follow the remapped action.
- Natural attacks use modest canonical species learnset choices with an
  explicitly bounded, PP-free arena damage adaptation. Learned moves consume
  their own PP and have individual cooldowns preserved across switching.
- Powerful learned moves receive 12.5% extra startup/recovery commitment;
  simulation speed, active durations, movement and quick attacks are retained.
- Seven independent stat stages clamp to -6..+6 and persist through actions.
  Damage, movement, accuracy and evasion use those stages. Replacement resets
  stages. Common boosts/debuffs and distinct timed screens have native data.
- Burn, poison/toxic, paralysis, sleep, freeze and confusion use central
  simulation clocks, mechanics and matching feedback. Nonvolatile statuses
  round-trip through actual native Pokémon data and respond to native medicine.
- Beam contests use move power, staged stats, type, angle and establishment
  timing. Pressing buttons adds no clash strength. Two independently owned
  pairs can resolve one normal hit or a timed draw; replacements cannot inherit
  stale clash damage. Projectile crossings still mutually cancel.
- AI uses observed positions/phases/resources/types, avoiding input reading.
  Assist/Defend/Focus commands direct allied behaviour. Player fainting opens
  an actual reserve chooser, with native party ownership maintained.

## Remaining scope

This is a substantial integrated development build, not completion of every
item in the master vision. New elemental hazard pools, global followers,
complete additional reaction-pose coverage, all alternate forms and full retail
ability/weather/item/move-script semantics remain unfinished. The generated
coverage report lists unsupported status scripts instead of inventing effects.
Natural attack damage is an arena adaptation rather than retail free attacks.
Controls are session settings. Long campaigns and real DS hardware require
additional coverage; compiler success alone does not validate them.

See the accompanying final report for exact tests, emulator results, build
hashes, downloads and remaining issues of the delivered candidate.
