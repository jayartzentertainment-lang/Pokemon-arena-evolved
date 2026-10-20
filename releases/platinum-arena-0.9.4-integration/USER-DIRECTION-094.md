# Current direction for the Platinum integration pass

Recorded from the user's latest request on 20 October 2026.

## Agreed order

1. Keep the approved Flamethrower/VFX appearance and existing sound mappings. Add other effect artwork only where necessary.
2. Improve dashing, attack presentation and VFX integration in the current Platinum source.
3. Adapt the live-combat UI and system boundaries to native Platinum: retain its story, menus, encounter presentation, party/state and save flow.
4. Follow with a broader animation-fluidity pass, using proper extra poses and consistent proportions/anchors.

The existing goal remains simultaneous 1v1/2v2 live action, with held defence/stamina, readable beam clashes, opponent reserve indicators and configurable controls. Preserve Y strike, A/X moves, L cycling, R free by default, B guard/dodge, Select aim, Start pause and L+R catching unless the user changes the mapping.

## Testing and delivery

The user explicitly takes responsibility for gameplay/emulator playtesting. Do not spend this pass replaying story scenes or conducting exhaustive visual tests. Source checks and compilation may still be used to produce a concrete test build. Clearly distinguish those checks from gameplay verification.

The user will provide sources rather than more ROMs. Ask for specific missing source artwork only when it is not already available. [MISSING-POSES.md](MISSING-POSES.md) explains usable packs and confirmed existing pose metadata.

The approved Flamethrower GIF was a standalone visual mock-up, not proof of ROM integration. The prior 0.9.3 ROM and its recorded validation remain a separate baseline. Do not describe every native ability, item, weather or move-script interaction as complete without implementation evidence.

Use the already established GitHub delivery route for final downloads, then verify the published bytes. Earlier workspace and `sandbox:/workspace` links did not download successfully for this user.

## Preview reference

[Approved Flamethrower GIF](https://github.com/jayartzentertainment-lang/Pokemon-arena-evolved/raw/89c02cf604a96f6d7b68773b0f14b708e27e698f/previews/flamethrower/flamethrower-preview.gif)

This document records direction and scope. It does not assert that the requested changes are complete.
