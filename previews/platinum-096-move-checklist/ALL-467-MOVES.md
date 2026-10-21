# All 467 Platinum moves — sprite checklist

This is the current 0.9.6 visual inventory, not a build or a claim of finished
animations. The lists distinguish connected sprite families, body/contact
motion, conditional feedback and generic fallbacks. A shared effect can be
connected while still needing a move-specific replacement.

[Download spreadsheet](./ALL-467-MOVES.csv) · [Detailed data](./ALL-467-MOVES.json) ·
[Download all lists](./Move-Sprite-Checklist.zip)

## Current runtime coverage

| Current visual assignment | Moves |
|---|---:|
| Swift's move-specific star | 1 |
| Shared elemental or matching beam families | 100 |
| Body-motion/contact attacks | 151 |
| Conditional self/status feedback | 42 |
| Generic impact cones | 68 |
| Shared Swift-star assignments to review | 32 |
| Neutral water-shaped beam fallbacks | 9 |
| Generic projectile orbs | 35 |
| Self animations without a direct status adapter | 29 |
| **Total** | **467** |

The last 29 include Splash, which is intentionally harmless. Body-contact
attacks do not automatically need standalone projectile art. The 32 star
assignments need review; a star-like move can reasonably retain star artwork.
The 9 neutral beams and 35 orbs are clear visual replacement/adaptation priorities.

## What is already in your uploads

The 95-sheet Diamond/Pearl pack contains named sources for 79 moves: 70 have
usable-looking sprite/component sheets, 8 have only named background sheets,
and Sacred Fire's named sheet has an unresolved visual mismatch. These counts
describe source availability, not 70 completed live-combat animations.

Other supplied Ranger/PMD/KOF effects are also candidates. The 388 moves with
no direct named D/P match are not declared asset-missing: those larger packs
and the original Platinum graphics remain available for adaptation/extraction.
The original move engine's animation scripts are not automatically compatible
with the live top-down controller. Some named sheets are already partly used
as a shared family; their complete named sequence is not necessarily connected.

Known quality requests: Flamethrower and Hydro Pump already have beams, and
the user wants better presentation. Water Pulse has a supplied ring sheet.
Thunderbolt has an unused supplied lightning candidate despite its current
Swift-star assignment. Mechanics/status-script coverage is a separate column.

## Every move, alphabetically


### A

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Absorb (71) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Acid (51) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Acid Armor (151) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Acupressure (367) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | Accupressure.png |
| Aerial Ace (332) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Aerial-Ace.png |
| Aeroblast (177) | Neutral water-shaped beam: replace | Replace/remap with the correct themed beam or projectile. | Aero-Blast.png |
| Agility (97) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Air Cutter (314) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Air Slash (403) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Amnesia (133) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | Amnesia.png |
| Ancient Power (246) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | Ancient-Power.png |
| Aqua Jet (453) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Aqua Ring (392) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Aqua Tail (401) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Arm Thrust (292) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Aromatherapy (312) | Conditional status/buff feedback; ATLAS_LEAF | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Assist (274) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Assurance (372) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Astonish (310) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Attack Order (454) | Generic orb: replace/adapt; ATLAS_LEAF | Replace/remap with a suitable move/type sprite. | Attack-and-Heal-Order.png |
| Attract (213) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Aura Sphere (396) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Aurora Beam (62) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Avalanche (419) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### B

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Barrage (140) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Barrier (112) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Baton Pass (226) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | Baton-Pass.png |
| Beat Up (251) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Belly Drum (187) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Bide (117) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Bind (20) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Bite (44) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Blast Burn (307) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Blast-Burn.png |
| Blaze Kick (299) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Blizzard (59) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Unused Ranger 0/94 ice encasement/shatter (freeze/contact component, not a complete beam). |
| Block (335) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Body Slam (34) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Bone Club (125) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Bone Rush (198) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Bonemerang (155) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Bounce (340) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Brave Bird (413) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Brick Break (280) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Brine (362) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Bubble (145) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | PMD move_VFX/0073/000/001..011 water/bubble spray (shared component). |
| Bubble Beam (61) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | PMD move_VFX/0073/000/001..011 water/bubble spray (shared component). |
| Bug Bite (450) | Body motion and contact effect; ATLAS_LEAF | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Bug Buzz (405) | Generic orb: replace/adapt; ATLAS_LEAF | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Bulk Up (339) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Bullet Punch (418) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Bullet Seed (331) | Shared elemental/beam sprite connected; ATLAS_LEAF | Connected shared art; tailor to the move where needed. | Bullet-Seed.png |

### C

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Calm Mind (347) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Camouflage (293) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Captivate (445) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Charge (268) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Charge Beam (451) | Neutral water-shaped beam: replace | Replace/remap with the correct themed beam or projectile. | Unused Ranger 0/111 bold lightning, 0/98 thin lightning, 0/45 branching discharge; adapt to this move. |
| Charm (204) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Chatter (448) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Clamp (128) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Close Combat (370) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Close-Combat.png |
| Comet Punch (4) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Confuse Ray (109) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Confusion (93) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Constrict (132) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Conversion (160) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Conversion 2 (176) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Copycat (383) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Cosmic Power (322) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Cotton Spore (178) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Counter (68) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Covet (343) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Crabhammer (152) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Cross Chop (238) | Body motion and contact effect; ATLAS_SLASH | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Cross Poison (440) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Crunch (242) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Crunch.png |
| Crush Claw (306) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Crush Grip (462) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Curse (174) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Cut (15) | Body motion and contact effect; ATLAS_SLASH | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Cut.png |

### D

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Dark Pulse (399) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Dark Void (464) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Defend Order (455) | Conditional status/buff feedback; ATLAS_LEAF | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Defense Curl (111) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Defog (432) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Destiny Bond (194) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Detect (197) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Dig (91) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Disable (50) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Discharge (435) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Unused Ranger 0/111 bold lightning, 0/98 thin lightning, 0/45 branching discharge; adapt to this move. |
| Dive (291) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Dizzy Punch (146) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Doom Desire (353) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Double Edge (38) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Double Hit (458) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Double Kick (24) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Double Slap (3) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Double Team (104) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Draco Meteor (434) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Dragon Breath (225) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Dragon Claw (337) | Body motion and contact effect; ATLAS_SLASH | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Dragon-Claw.png |
| Dragon Dance (349) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Dragon Pulse (406) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Dragon Rage (82) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Dragon Rush (407) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Drain Punch (409) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Dream Eater (138) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Drill Peck (65) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Dynamic Punch (223) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Dynamic-Punch.png — Named background component only; no actor/projectile sprite confirmed there |

### E

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Earth Power (414) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Earthquake (89) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Egg Bomb (121) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Embargo (373) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Ember (52) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Unused Ranger 0/126 orange flame phases. |
| Encore (227) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Endeavor (283) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Endure (203) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Energy Ball (412) | Shared elemental/beam sprite connected; ATLAS_LEAF | Connected shared art; tailor to the move where needed. | Energy-Ball.png |
| Eruption (284) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Eruption.png |
| Explosion (153) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Extrasensory (326) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Extreme Speed (245) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### F

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Facade (263) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Faint Attack (185) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Fake Out (252) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Fake Tears (313) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| False Swipe (206) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Feather Dance (297) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Feint (364) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Fire Blast (126) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Fire-Blast.png |
| Fire Fang (424) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Fire Punch (7) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Fire-Punch.png; Unused Ranger 0/126 flame accent + KOF heavy-hit frames22..35. |
| Fire Spin (83) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Fire-Spin.png |
| Fissure (90) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | Fissure.png |
| Flail (175) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Flame Wheel (172) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Flamethrower (53) | Shared elemental/beam sprite connected | Current beam is connected; visual quality upgrade requested by the user. | Ranger 0/126 flame particles + KOF fire particles; assemble a continuous stream. |
| Flare Blitz (394) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Flare-Blitz.png — Named background component only; no actor/projectile sprite confirmed there |
| Flash (148) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Flash Cannon (430) | Neutral water-shaped beam: replace | Replace/remap with the correct themed beam or projectile. | Flash-Cannon.png |
| Flatter (260) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Fling (374) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Fly (19) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Fly.png |
| Focus Blast (411) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | Focus-Blast.png |
| Focus Energy (116) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Focus Punch (264) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Follow Me (266) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Force Palm (395) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Foresight (193) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Frenzy Plant (338) | Shared elemental/beam sprite connected; ATLAS_LEAF | Connected shared art; tailor to the move where needed. | Frenzy-Plant.png |
| Frustration (218) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Fury Attack (31) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Fury Cutter (210) | Body motion and contact effect; ATLAS_LEAF | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Fury Swipes (154) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Future Sight (248) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### G

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Gastro Acid (380) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Giga Drain (202) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Giga Impact (416) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Giga-Impact.png — Named background component only; no actor/projectile sprite confirmed there |
| Glare (137) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Grass Knot (447) | Shared elemental/beam sprite connected; ATLAS_LEAF | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Grass Whistle (320) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Gravity (356) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Growl (45) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | Growl.png |
| Growth (74) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Grudge (288) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | Grudge.png |
| Guard Swap (385) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Guillotine (12) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Gunk Shot (441) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Gust (16) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Gyro Ball (360) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### H

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Hail (258) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Hammer Arm (359) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Hammer-Arm.png |
| Harden (106) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Haze (114) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Head Smash (457) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Headbutt (29) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Heal Bell (215) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Heal Block (377) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Heal Order (456) | Conditional status/buff feedback; ATLAS_LEAF | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | Attack-and-Heal-Order.png |
| Healing Wish (361) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Heart Swap (391) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Heat Wave (257) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Heat-Wave.png — Named background component only; no actor/projectile sprite confirmed there |
| Helping Hand (270) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | Helping-Hand.png |
| Hi Jump Kick (136) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Hidden Power (237) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Horn Attack (30) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Horn Drill (32) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Howl (336) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Hydro Cannon (308) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Hydro-Cannon-Back.png — Named background component only; no actor/projectile sprite confirmed there |
| Hydro Pump (56) | Shared elemental/beam sprite connected | Current beam is connected; visual quality upgrade requested by the user. | Hydro-Pump.png |
| Hyper Beam (63) | Neutral water-shaped beam: replace | Replace/remap with the correct themed beam or projectile. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Hyper Fang (158) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Hyper Voice (304) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Hypnosis (95) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Hypnosis.png |

### I

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Ice Ball (301) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Unused Ranger 0/94 ice encasement/shatter (freeze/contact component, not a complete beam). |
| Ice Beam (58) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Unused Ranger 0/94 ice encasement/shatter (freeze/contact component, not a complete beam). |
| Ice Fang (423) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Ice Punch (8) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Ice Shard (420) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Ice-Shards.png; Unused Ranger 0/94 ice encasement/shatter (freeze/contact component, not a complete beam). |
| Icicle Spear (333) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Icy Wind (196) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Unused Ranger 0/94 ice encasement/shatter (freeze/contact component, not a complete beam). |
| Imprison (286) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Ingrain (275) | Self animation; effect needs review; ATLAS_LEAF | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Iron Defense (334) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Iron Head (442) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Iron-Head.png |
| Iron Tail (231) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### J

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Judgment (449) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Jump Kick (26) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### K

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Karate Chop (2) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Kinesis (134) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Knock Off (282) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### L

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Last Resort (387) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Last-Resort.png |
| Lava Plume (436) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Lava-Plume.png |
| Leaf Blade (348) | Body motion and contact effect; ATLAS_LEAF | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Leaf Storm (437) | Shared elemental/beam sprite connected; ATLAS_LEAF | Connected shared art; tailor to the move where needed. | Leaf-Storm.png |
| Leech Life (141) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Leech Seed (73) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Leech-Seed.png |
| Leer (43) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Lick (122) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Light Screen (113) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Lock On (199) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | Lock-On.png |
| Lovely Kiss (142) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Low Kick (67) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Lucky Chant (381) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Lunar Dance (461) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Luster Purge (295) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### M

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Mach Punch (183) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Magic Coat (277) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Magical Leaf (345) | Shared elemental/beam sprite connected; ATLAS_LEAF | Connected shared art; tailor to the move where needed. | Magical-Leaf.png |
| Magma Storm (463) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Magnet Bomb (443) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Magnet Rise (393) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Magnitude (222) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Me First (382) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Mean Look (212) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | Mean-Look.png |
| Meditate (96) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Mega Drain (72) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Mega Kick (25) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Mega Punch (5) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Megahorn (224) | Body motion and contact effect; ATLAS_SLASH | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Memento (262) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Metal Burst (368) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Metal Claw (232) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Metal Sound (319) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Meteor Mash (309) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Metronome (118) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Milk Drink (208) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Mimic (102) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Mind Reader (170) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Minimize (107) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Miracle Eye (357) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Mirror Coat (243) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Mirror Move (119) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Mirror Shot (429) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Mist (54) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Mist Ball (296) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Moonlight (236) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Morning Sun (234) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Mud Bomb (426) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | Mud-Bomb.png |
| Mud Shot (341) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | Mud-Shot.png |
| Mud Slap (189) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Mud Sport (300) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | Mud-Sport.png |
| Muddy Water (330) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### N

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Nasty Plot (417) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Natural Gift (363) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Nature Power (267) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Needle Arm (302) | Body motion and contact effect; ATLAS_LEAF | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Needle-Arn.png |
| Night Shade (101) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Night Slash (400) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Nightmare (171) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Nightmare.png — Named background component only; no actor/projectile sprite confirmed there |

### O

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Octazooka (190) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Odor Sleuth (316) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Ominous Wind (466) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Outrage (200) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Overheat (315) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### P

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Pain Split (220) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Pay Day (6) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Payback (371) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Peck (64) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Perish Song (195) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Petal Dance (80) | Shared elemental/beam sprite connected; ATLAS_LEAF | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Pin Missile (42) | Generic orb: replace/adapt; ATLAS_LEAF | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Pluck (365) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Poison Fang (305) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Poison Gas (139) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Poison Jab (398) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Poison-Jab.png |
| Poison Powder (77) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Poison Sting (40) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Poison Tail (342) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Pound (1) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Powder Snow (181) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Power Gem (408) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Power Swap (384) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Power Trick (379) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Power Whip (438) | Body motion and contact effect; ATLAS_LEAF | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Present (217) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Protect (182) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Psybeam (60) | Neutral water-shaped beam: replace | Replace/remap with the correct themed beam or projectile. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Psych Up (244) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Psychic (94) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Psychic.png — Named background component only; no actor/projectile sprite confirmed there |
| Psycho Boost (354) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Psycho Cut (427) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Psycho-Cut.png |
| Psycho Shift (375) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Psywave (149) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Punishment (386) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Pursuit (228) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### Q

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Quick Attack (98) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### R

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Rage (99) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Rain Dance (240) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Rapid Spin (229) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Razor Leaf (75) | Shared elemental/beam sprite connected; ATLAS_LEAF | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Razor Wind (13) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Recover (105) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Recycle (278) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Reflect (115) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Refresh (287) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Rest (156) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Return (216) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Revenge (279) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Reversal (179) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Roar (46) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Roar Of Time (459) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Rock Blast (350) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Rock Climb (431) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Rock Polish (397) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Rock Slide (157) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Rock Smash (249) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Rock-Smash.png |
| Rock Throw (88) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Rock Tomb (317) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | Rock-Tomb.png |
| Rock Wrecker (439) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Role Play (272) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Rolling Kick (27) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Rollout (205) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Roost (355) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### S

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Sacred Fire (221) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Sacred-Fire.png — Named file visually mismatched; identity must be checked |
| Safeguard (219) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Sand Attack (28) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Sand Tomb (328) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Sandstorm (201) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Scary Face (184) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | Scary-Face-Reg.png, Scary-Face-Transparent.png |
| Scratch (10) | Body motion and contact effect; ATLAS_SLASH | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Scratch-Shadow-Claw.png |
| Screech (103) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Secret Power (290) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Seed Bomb (402) | Shared elemental/beam sprite connected; ATLAS_LEAF | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Seed Flare (465) | Shared elemental/beam sprite connected; ATLAS_LEAF | Connected shared art; tailor to the move where needed. | Seed-Flare.png |
| Seismic Toss (69) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Siesmic-Toss.png |
| Selfdestruct (120) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Shadow Ball (247) | Neutral water-shaped beam: replace | Replace/remap with the correct themed beam or projectile. | Shadow-Ball.png |
| Shadow Claw (421) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Scratch-Shadow-Claw.png |
| Shadow Force (467) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Shadow-Force.png |
| Shadow Punch (325) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Shadow Sneak (425) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Sharpen (159) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Sheer Cold (329) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Shock Wave (351) | Neutral water-shaped beam: replace | Replace/remap with the correct themed beam or projectile. | Unused Ranger 0/111 bold lightning, 0/98 thin lightning, 0/45 branching discharge; adapt to this move. |
| Signal Beam (324) | Neutral water-shaped beam: replace; ATLAS_LEAF | Replace/remap with the correct themed beam or projectile. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Silver Wind (318) | Generic orb: replace/adapt; ATLAS_LEAF | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Sing (47) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Sketch (166) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Skill Swap (285) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Skull Bash (130) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Sky Attack (143) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Sky Uppercut (327) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Slack Off (303) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Slam (21) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Slash (163) | Body motion and contact effect; ATLAS_SLASH | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Sleep Powder (79) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Sleep Talk (214) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Sludge (124) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Sludge Bomb (188) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Smelling Salt (265) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Smog (123) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Smoke Screen (108) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Snatch (289) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Snore (173) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Soft-Boiled (135) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Solar Beam (76) | Neutral water-shaped beam: replace | Replace/remap with the correct themed beam or projectile. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Sonic Boom (49) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Spacial Rend (460) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | Spacial-Rend.png |
| Spark (209) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Spider Web (169) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Spike Cannon (131) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Spikes (191) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | Spikes.png |
| Spit Up (255) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Spite (180) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Splash (150) | Intentional non-damaging body animation | Splash is intentionally harmless; no damaging projectile or mandatory extra VFX is needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Spore (147) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Stealth Rock (446) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Steel Wing (211) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Stockpile (254) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Stomp (23) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Stone Edge (444) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | Srone-Edge.png |
| Strength (70) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| String Shot (81) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Struggle (165) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Stun Spore (78) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Submission (66) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Substitute (164) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | Substitute-Back.png, Substitute-Front.png |
| Sucker Punch (389) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Sucker-Punch.png |
| Sunny Day (241) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Super Fang (162) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Superpower (276) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Supersonic (48) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Surf (57) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Surf-Regular.png, Surf-Transparent.png |
| Swagger (207) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | Swagger.png |
| Swallow (256) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Sweet Kiss (186) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Sweet Scent (230) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Swift (129) | Move-specific sprite connected | Already connected; optional polish. | Swift.png |
| Switcheroo (415) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Swords Dance (14) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | Swords-Dance.png |
| Synthesis (235) | Conditional status/buff feedback; ATLAS_LEAF | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### T

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Tackle (33) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Tail Glow (294) | Conditional status/buff feedback; ATLAS_LEAF | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Tail Whip (39) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Tailwind (366) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Take Down (36) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Taunt (269) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Teeter Dance (298) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Teleport (100) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Thief (168) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Thrash (37) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Thunder (87) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | Unused Ranger 0/111 bold lightning, 0/98 thin lightning, 0/45 branching discharge; adapt to this move. |
| Thunder Fang (422) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Thunder Punch (9) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Thunder Shock (84) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | Unused Ranger 0/111 bold lightning, 0/98 thin lightning, 0/45 branching discharge; adapt to this move. |
| Thunder Wave (86) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Unused Ranger 0/111 bold lightning, 0/98 thin lightning, 0/45 branching discharge; adapt to this move. |
| Thunderbolt (85) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | Unused Ranger 0/111 bold lightning, 0/98 thin lightning, 0/45 branching discharge; adapt to this move. |
| Tickle (321) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Torment (259) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Toxic (92) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Toxic Spikes (390) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Transform (144) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Tri Attack (161) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Trick (271) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Trick Room (433) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Trick-Room.png — Named background component only; no actor/projectile sprite confirmed there |
| Triple Kick (167) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Trump Card (376) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Twineedle (41) | Generic orb: replace/adapt; ATLAS_LEAF | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Twister (239) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### U

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| U-turn (369) | Body motion and contact effect; ATLAS_LEAF | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Uproar (253) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### V

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Vacuum Wave (410) | Generic orb: replace/adapt | Replace/remap with a suitable move/type sprite. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Vice Grip (11) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Vine Whip (22) | Body motion and contact effect; ATLAS_LEAF | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Vital Throw (233) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Volt Tackle (344) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### W

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Wake Up Slap (358) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Water Gun (55) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | PMD move_VFX/0073/000/001..011 water/bubble spray (shared component). |
| Water Pulse (352) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Water-Pulse.png |
| Water Sport (346) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Water-Sport.png |
| Water Spout (323) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Waterfall (127) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Waterfall.png |
| Weather Ball (311) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Whirlpool (250) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Whirlpool.png |
| Whirlwind (18) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Will-O-Wisp (261) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Wing Attack (17) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Wish (273) | Self animation; effect needs review | Review intended action and supported mechanics before adding an effect. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Withdraw (110) | Conditional status/buff feedback | Shared effect appears when its rule triggers; dedicated icon/animation is optional. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Wood Hammer (452) | Body motion and contact effect; ATLAS_LEAF | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | Wood-Hammer.png |
| Worry Seed (388) | Shared elemental/beam sprite connected | Connected shared art; tailor to the move where needed. | Worry-Seed.png |
| Wrap (35) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Wring Out (378) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### X

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| X-Scissor (404) | Body motion and contact effect; ATLAS_LEAF | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | X-Scizzor.png |

### Y

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Yawn (281) | Generic impact-cone effect | Choose a move-appropriate effect or intentional motion; current impact puff is generic. | No direct named D/P match confirmed; other packs/native extraction remain options. |

### Z

| Move (ID) | Current art/status | Sprite work | Supplied candidate |
|---|---|---|---|
| Zap Cannon (192) | Shared Swift-star fallback: review | Check the move: replace/remap the Swift star where it does not fit. | No direct named D/P match confirmed; other packs/native extraction remain options. |
| Zen Headbutt (428) | Body motion and contact effect | Baseline motion/contact exists. Dedicated extra particles are optional; improve choreography as needed. | No direct named D/P match confirmed; other packs/native extraction remain options. |
