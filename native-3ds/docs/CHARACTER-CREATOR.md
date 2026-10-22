# Trainer bases and the blank Custom character

New Custom characters use the other supplied **Gen IV-proportioned male/female
blank set** from `pokemon png.rar`. The retained DS converter had already cleaned
its editor guides and aligned the layers. This native conversion preserves those
32x32 cells and common anchors; it does not draw Lucas, Red or Brendan underneath.
All optional layers start at None. Skin comes from the selected body palette.

The current creator offers two bodies, four skin palettes, four authored
hairstyles with six dyes, three tops with eight dyes, three legwear styles with
four dyes, and two hats with eight dyes, plus None for each optional layer.
Style and dye combinations are currently selected by the same row. There is no
invented accessory option when this set has no matching accessory art. The preview
stays still; L/R changes its facing. Chosen layers remain selected after a
temporary switch to a named trainer.

Red and Brendan are separate fixed bases reused from the retained DS build.
Their creator offers skin, hair colour and outfit colour, plus name/follower
settings. It does not offer a hairstyle, hat or clothing-shape replacement.
Colour masks match the retained DS palette groups and never change silhouettes.

The older 32x48 modular Custom pack is retained as Custom Classic for existing
v0.3 saves. Its selections are not reinterpreted as indices into the Gen IV pack.
Choosing another base from Classic explicitly starts the new Custom bank blank.
Native profile format 2 appends the base and regional colours to the old payload.
Format 1 still decodes to its previous Classic appearance, name, party and legacy
progress. Original-story flags remain separate and are not fabricated during
legacy-save migration.

All choices use the same drawing function for the creator, trainer card and
implemented original/legacy field scenes. Only the selected modular texture bank
is loaded; switching bases releases the other bank before loading replacements.
Current native integration uses walking poses. The supplied running, cycling,
front and throwing sources are retained for later integration; this is not a
claim that all original cutscene, surf, bike or trainer battle poses run natively.

`provenance/gen4-custom.json` records the exact cleaned DS bank and original
archive. The protected inputs retain the bank, original path/hash manifest and
cleanup notes. Credits/licence are not established by the upload itself.
`provenance/trainer-bases.json` separately records fixed regional art and the
Classic bodies retained only for save compatibility.

`tests/test_creator.c` checks actual inputs, blank defaults, both Gen IV bodies,
layer bounds, fixed regional colour rules, touch controls, original intro identity,
v2 appearance persistence, CRC rejection and a synthetic v0.3 format-1 save.

To regenerate the Gen IV PNG layers after restoring protected inputs:

```sh
python3 tools/prepare_gen4_custom.py
```

Or pass `--ds-source /path/to/retained/DS-source`. To regenerate the fixed-base
atlas use `tools/prepare_trainer_bases.py` with the original DS/customization
inputs. These tools only write this native project; approved old releases and
the retained DS source remain unchanged.
