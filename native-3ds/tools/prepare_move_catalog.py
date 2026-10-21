#!/usr/bin/env python3
"""Recover the supplied0.9.6 VFX family cels and genuine DP Swift stars for3DS."""
import argparse,hashlib,json,re
from pathlib import Path
from PIL import Image
from prepare_assets import palette,decode_tiles,opaque_sheet_key
R=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--ds-source',type=Path,required=True);p.add_argument('--dp-effects',type=Path,required=True);a=p.parse_args()
raw=(a.ds_source/'src/arena/v096_effect_art.inc').read_text();tile_text=raw.split('arena_v096_tiles[56][512]={',1)[1].split('};',1)[0];tiles=bytes(int(v,16) for v in re.findall(r'0x([0-9a-fA-F]{2})',tile_text));assert len(tiles)==56*512
pal=[int(v) for v in raw.split('arena_v096_palette[16]={',1)[1].split('}',1)[0].split(',')];dst=R/'gfx/private/move_catalog';dst.mkdir(exist_ok=True);images=[decode_tiles(tiles[n*512:(n+1)*512],32,32,palette(pal)) for n in range(56)]
swift=a.dp_effects/'Swift.png';source=Image.open(swift).convert('RGBA')
for box in [(194,131,218,155),(195,360,219,384)]:images.append(opaque_sheet_key(source.crop(box)))
for n,im in enumerate(images):im.save(dst/f'{n:03d}.png')
(R/'gfx/move_catalog.t3s').write_text('--atlas -f rgba5551 -z auto\n'+'\n'.join(f'private/move_catalog/{n:03d}.png' for n in range(len(images)))+'\n')
(R/'assets/move-catalog.json').write_text(json.dumps({'source':'Supplied DS0.9.6 converted VFX family artwork; renderer chooses by move/type. Shared families, not467 individually authored effects.','families':json.loads((a.ds_source/'res/arena/v096-effects/manifest.json').read_text())['family_map'],'frames':len(images),'swift_sha256':hashlib.sha256(swift.read_bytes()).hexdigest(),'swift_images':[56,57]},indent=2)+'\n');print('58 move-family cels prepared; Swift uses actual star art.')
