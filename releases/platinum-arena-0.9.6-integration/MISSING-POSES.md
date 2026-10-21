# Pokémon poses and smoother animation

The first priority is the Platinum combat/UI integration. Keep the approved flame/VFX style and existing sound mappings. A broader sprite-animation pass follows that work; the user handles gameplay testing.

## What is actually missing

The current sprite loader requests five slots: `Idle`, `Walk`, `Shoot`, `Attack` and `SpAttack`. The source artwork often contains additional animations that the game does not load yet.

Read-only metadata inspection found `Hurt` and `Charge` declarations in all **306 locally retained extension XML files**. None declares an animation literally named `Guard`. This does not establish that every declaration is a distinct animation, or that every referenced PNG is already downloaded. The accepted 187 original sets were outside this local XML count.

The first four Pokémon to address are:

| Pokémon | Source folder | Verified extra animation metadata |
| --- | --- | --- |
| Charizard | [0006](https://github.com/PMDCollab/SpriteCollab/tree/d25607ff4746957df10bdb78db090887cd94f1f8/sprite/0006) | Separate 2-frame `Hurt` and 10-frame `Charge` |
| Blastoise | [0009](https://github.com/PMDCollab/SpriteCollab/tree/d25607ff4746957df10bdb78db090887cd94f1f8/sprite/0009) | Separate 2-frame `Hurt` and 10-frame `Charge`; also declares `Withdraw` |
| Gyarados | [0130](https://github.com/PMDCollab/SpriteCollab/tree/d25607ff4746957df10bdb78db090887cd94f1f8/sprite/0130) | Separate 2-frame `Hurt` and 10-frame `Charge`; large authored canvases need appropriate sizing |
| Lapras | [0131](https://github.com/PMDCollab/SpriteCollab/tree/d25607ff4746957df10bdb78db090887cd94f1f8/sprite/0131) | Separate 2-frame `Hurt` and 10-frame `Charge` |

Those four `AnimData.xml` files were fetched successfully from the already pinned SpriteCollab revision. Their extra poses are not aliases in the checked XML. This is source inspection, not proof that the poses have been integrated or tested in the ROM. Details are in [pose-metadata-audit.json](pose-metadata-audit.json).

`Charge` is not automatically a suitable defensive pose. A calm, consistent idle/brace pose is preferable to showing a shooting animation while guarding. A custom brace or recoil animation may need drawing when existing artwork does not fit.

The 64 alternate forms still use native front/back still-image fallbacks. They need matching form-specific directional artwork before they can receive the same animation coverage as canonical species.

## Where to get files

- **[SpriteCollab browser](https://sprites.pmdcollab.org/):** search by Pokémon name and inspect available artwork. Coverage varies by species, form and animation.
- **[SpriteCollab source](https://github.com/PMDCollab/SpriteCollab):** the source folders contain animation sheets, timing metadata, offsets, shadows and artist credits. The four links above use the exact revision already referenced by this project, so later upstream changes do not silently replace the accepted style.
- **[SkyTemple](https://skytemple.org/) and [its documentation](https://wiki.skytemple.org/):** Pokémon Mystery Dungeon sprite-format and editing resources. These are useful for preparing artwork, not a drop-in Platinum combat engine.

The user does not need to find every pose manually. When a matching upstream source is already available, the integration work can use it directly. A repository link or a small source folder is sufficient; another ROM upload is unnecessary.

For a directly downloadable example, Charizard's [animation metadata](https://raw.githubusercontent.com/PMDCollab/SpriteCollab/d25607ff4746957df10bdb78db090887cd94f1f8/sprite/0006/AnimData.xml), [hurt sheet](https://raw.githubusercontent.com/PMDCollab/SpriteCollab/d25607ff4746957df10bdb78db090887cd94f1f8/sprite/0006/Hurt-Anim.png) and [charge sheet](https://raw.githubusercontent.com/PMDCollab/SpriteCollab/d25607ff4746957df10bdb78db090887cd94f1f8/sprite/0006/Charge-Anim.png) returned HTTP 200 during this review. The corresponding offsets, shadows and credits also returned HTTP 200. The browser/editor sites listed here are recommended sources; this environment's network proxy blocked opening those sites, so their current pages were not inspected.

## What to send

Send one small ZIP per Pokémon or a source-repository link. For example:

```text
0006/
  AnimData.xml
  Hurt-Anim.png
  Hurt-Offsets.png
  Hurt-Shadow.png
  Charge-Anim.png
  Charge-Offsets.png
  Charge-Shadow.png
  credits.txt
LICENSE.md
```

Include any animation referenced by `CopyOf`; do not rename an alias without its referenced files. Preserve `FrameWidth`, `FrameHeight`, `Durations`, `RushFrame`, `HitFrame`, `ReturnFrame` and `ShadowSize` where supplied. The offset/shadow images are metadata and should not be flattened into the visible artwork.

For new artwork, keep the eight direction rows in the expected order: **S, SE, E, NE, N, NW, W, SW**. Keep a stable body/ground anchor and a common canvas across a sequence. Do not crop every frame independently. Preserve the original pixels, palette and transparency; provide mouth/hand/emitter locations when known. Gyarados should not be forced into Lapras's body dimensions or vice versa.

Keep uploads below the environment's 32 MiB attachment transfer limit, or share the source repository instead.

## Ways to make motion smoother

1. Match the visible release/contact pose to the attack's actual release/contact timing.
2. Play anticipation and recovery once; sustain a suitable open-mouth or firing pose while a beam is active.
3. Give hurt, recoil and guard their correct visual roles instead of reusing a special-attack loop.
4. Keep feet, shadows and effect origins registered while turning, attacking and dashing. Correct anchors before adding frames.
5. Coordinate movement acceleration/deceleration with the walking or dash motion. Extra frames cannot repair a sliding world position.
6. Add selected in-between poses where motion still jumps after timing and anchors are correct. More evenly spaced frames are not automatically better than authored holds.

For custom drawing, use [LibreSprite](https://libresprite.github.io/) or [Piskel](https://www.piskelapp.com/), or [Aseprite](https://www.aseprite.org/). Work at the original pixel resolution with nearest-neighbour previews; keep editable source files plus exported PNG/XML data and credits.

`pmd-red` is a useful GBA source reference for animation timing and registration. It is not a direct replacement for the Platinum renderer or battle system.
