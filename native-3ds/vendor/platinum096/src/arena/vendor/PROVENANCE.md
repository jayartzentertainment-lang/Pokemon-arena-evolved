These portable geometry, navigation, terrain and momentum helpers come from
the accepted Arena Plus DS v0.9.1 source at
`/workspace/Pokemon-arena-evolved/ds/vendor/arena`.

The Platinum adaptation isolates includes beneath `arena/vendor`, shares fixed
width portable types, and uses native Platinum move IDs. The existing authored
move profiles and collision/physics calculations remain intact. Moves without
an authored profile use `ArenaPlatinum_GetFallbackProfile`, generated from
Platinum's local `res/moves` JSON; these actions implement geometry and timing,
with bounded real-time adaptations for status effects.

`src/arena/combat.c` starts from the accepted v0.9.1 combat simulation. Its
Platinum changes are per-move Gen IV physical/special categories, complete
move metadata, current PP restoration, revised stamina, temporary action
status effects, and control transfer between stable allied actor IDs. Basic
strikes use no PP or stamina. Moves use their actual PP, while dash and guard
spend stamina, which regenerates after a short delay.

The accepted source's historical upstream provenance is retained at
`/workspace/Pokemon-arena-evolved/ds/vendor/arena/PROVENANCE.md`.

Original provenance record:

# Imported Arena helpers

Copied from user-supplied `workspace.rar`, reassembled from six uploaded archive
parts. The original is preserved at `/workspace/source-import/source/workspace`.

- `arena_moves.c/.h`: move profiles and cone/segment collision.
- `arena_navigation.c/.h`: shared obstacle geometry and visibility routing.
- `arena_physics.c/.h`: bounded debris, obstacle destruction and chains.
- `arena_plus.h`: Arena Plus inertia and swept-contact helpers.
- `constants/moves.h`: Gen III move identities.

Existing comments/provenance are retained. Host-mode definitions select portable
types and remove GBA hardware attributes. The native controller/render backend
is separate under `source/`.

The supplied Polish Patch 01 physics radius fix changes `<46*46` to `<47*47`.
Navigation/physics tests use Patch 01 fixtures for the relocated obstacle.
The GBA floating-number safety patch is inapplicable to this renderer, which
has no GBA OAM allocations or that sprite-pool access path.

No original GBA code, ROM, or save was overwritten.
