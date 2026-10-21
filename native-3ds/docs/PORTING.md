# Port boundary

The exact 0.9.6 source archive was verified against all 2,260 recorded member
checksums before recovery. `vendor/platinum096/` preserves the selected original
portable code and regression sources. Native adaptations live outside it.

| System | Treatment |
| --- | --- |
| Combat, damage/types, PP, stamina, statuses, stages and deterministic clashes | Reused 0.9.6 portable simulation |
| Move geometry, navigation, destructible terrain | Reused portable helpers with preserved provenance |
| Circle Pad speed | Added optional Q8 control magnitude; digital behavior unchanged |
| Contact presentation | Added optional move ID on real contact events; phase-time accessors |
| Party and switching | New platform-neutral wrapper with four slots per team, preserved HP/PP/status/cooldowns, stable actor bindings |
| Faint/deploy transitions | Native wrapper timers; no fake dedicated source poses |
| Rendering | Rewritten with Citro2D/Citro3D frame submission; no Nitro hardware calls |
| Animation import | Decode verified packed 4bpp frames, crop/deduplicate, retain independent foot anchors/timing/directions |
| Floor import | Decode DS screen-block ordering and mirror the same world border |
| Input/UI | Native libctru controls and touchscreen; actual supplied panel artwork |
| Audio | Optional original synthesized prototype cues; original sound engine deferred |
| Overworld/creator/campaign/save | Not yet ported |

Simulation is 60 fixed ticks per second, with a bounded six-step catch-up and
interpolated drawing. Heavy commitment rules preserve the 0.9.6 extra 12.5%
startup/recovery; movement and the entire simulation are not slowed globally.
Under prolonged stalls over 100 ms, excess wall time is discarded to avoid an
unbounded catch-up loop. A hardware frame-pacing policy still needs profiling.

Species textures load only when an actor first binds that species. The current
four-species cache is bounded at four texture sheets; no per-frame file reads.
Future 493-species support needs an eviction policy as well as a data importer.

Engine state uses actor IDs and serials. Switching retains the tested outgoing
projectile/clash invalidation behavior. Both teams have four party slots in
this standalone arena, regardless of 1v1/2v2 active actor count.

Canonical four-species stats are level 50, neutral nature, 31 IVs, no EVs.
The arena is 448x320 world pixels and the camera fits active fighters; it is
not limited to showing one screen-sized rectangle of the world.

Toolchain: official devkitPro image
`devkitpro/devkitarm@sha256:116afba8df8453961de2936ffab20dd441edf4d682856c1ec8b0e53d7ed0bbf5`,
devkitARM GCC 16.1.0, tex3ds 2.3.0. Makefile based on the official current
`3ds-examples/graphics/gpu/gpusprites`, inspected at commit
`be2001fee08cf8ec7d3366e095e83f6f75da8942`.

The original 3DS remains the target: ARMv6k/mpcore, stereo disabled, optional
New 3DS CPU speedup disabled. An emulator configured as the original model is
useful evidence of loading/input/rendering, not evidence of physical hardware
performance or complete compatibility.
