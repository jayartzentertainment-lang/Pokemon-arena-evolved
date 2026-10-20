# Platinum Arena 0.9.4 — integration playtest build

[Download the game ZIP](Platinum-Arena-0.9.4-INTEGRATION.zip?raw=true) · [Download the source changes](Platinum-Arena-0.9.4-SOURCE.zip?raw=true)

Extract the game ZIP and open its `.nds` file in your Nintendo DS emulator. This is the native Platinum-based game. The new build compiled and linked successfully; gameplay and visual checks remain for your playtesting.

This pass integrates continuous fire/water/ice beam artwork, sustained beam clashes, eased dodges and trails, body-fitted held shields, attack-release sound timing, native level/status healthboxes and full remaining-team counts. Other beam families use a neutral energy fallback. Platinum story/party/save interfaces remain in place.

Open Battle Lab by holding **L + R + Select** at the title screen. On the Mode row choose **Clash 1v1** or **Clash 2v2**, then press Start. With default controls, A fires the selected beam; the practice opponents answer it. Free modes remain available. The private practice teams do not change story saves.

Default combat controls: D-pad movement, Y strike, A/X special moves, L cycle, B shield/dodge, Select aim and Start menu. L+R retains catching in supported wild encounters. Control remapping resets on a ROM restart.

- [What changed and what remains](CHANGES.md)
- [Where to download missing poses](MISSING-POSES.md)
- [Current direction and testing agreement](USER-DIRECTION-094.md)
- [Build and pending runtime checks](RELEASE.json)
- [Checksums](SHA256SUMS)

The source ZIP contains modifications and assets to apply to `pret/pokeplatinum` revision `1dc13dcea40c04ab53f1f607de46023dc409ef20`, with instructions and credits. It excludes the ROM, saves and SDK/compiler tools. No additional base ROM upload is needed.

Compiled ROM SHA-256: `3eab0f6b819f3c1747f2fa5949558061d7d861b6d27e48f9e3413714afabbca5`.
