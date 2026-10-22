#!/usr/bin/env python3
"""Decode the retained original intro screens and NPC cels for native textures."""
from pathlib import Path
import argparse,json,struct,hashlib
from PIL import Image
ROOT=Path(__file__).resolve().parents[1]
def narc(raw):
 assert raw[:4]==b'NARC';n=struct.unpack_from('<H',raw,24)[0];base=raw.index(b'GMIF')+8
 return [raw[base+a:base+b] for a,b in (struct.unpack_from('<II',raw,28+8*i) for i in range(n))]
def layer(char,pal,screen,transparent=False):
 at=char.index(b'RAHC');size,offset=struct.unpack_from('<II',char,at+24);pixels=char[at+8+offset:at+8+offset+size]
 at=pal.index(b'TTLP');size,offset=struct.unpack_from('<II',pal,at+16);colors=struct.unpack_from('<%uH'%(size//2),pal,at+8+offset)
 at=screen.index(b'NRCS');w,h=struct.unpack_from('<HH',screen,at+8);tiles=struct.unpack_from('<%uH'%(w*h//64),screen,at+20);im=Image.new('RGBA',(w,h));data=im.load()
 for y in range(h):
  for x in range(w):
   cell=tiles[(y//8)*(w//8)+x//8];tx=x%8;ty=y%8
   if cell&1024:tx=7-tx
   if cell&2048:ty=7-ty
   i=(cell&1023)*32+ty*4+tx//2;v=(pixels[i]>>(4*(tx&1)))&15;bank=(cell>>12)*16
   c=colors[(bank+v)%len(colors)];data[x,y]=((c&31)*255//31,((c>>5)&31)*255//31,((c>>10)&31)*255//31,0 if transparent and v==0 else 255)
 return im

def main():
 p=argparse.ArgumentParser();p.add_argument('--source',type=Path,required=True);a=p.parse_args();S=a.source
 assets=ROOT/'gfx/private/platinum_intro';assets.mkdir(exist_ok=True)
 members=narc((S/'res/prebuilt/demo/intro/intro.narc').read_bytes());bg=layer(members[0],members[3],members[4]);rowan=layer(members[19],members[20],members[23],True);bg.alpha_composite(rowan);bg.save(assets/'rowan.png')
 (ROOT/'gfx/platinum_intro.t3s').write_text('--atlas -f rgba5551 -z auto\nprivate/platinum_intro/rowan.png\n')
 # Preserve all original initial-area graphics IDs, selecting original sheet files.
 headers=json.loads((ROOT/'assets/private/platinum/map-headers.json').read_text());names=json.loads((ROOT/'assets/private/platinum/models.json').read_text());maps=sorted(set(m['name'] for m in names));gfxids=set()
 for name in maps:
  events=json.loads((S/'res/field/events'/('events_'+name+'.json')).read_text());gfxids.update(o['graphics_id'] for o in events['object_events'])
 gfxids.update(['OBJ_EVENT_GFX_PLAYER_M','OBJ_EVENT_GFX_PLAYER_F'])
 gfxids.discard('OBJ_EVENT_GFX_VAR_0')
 aliases={'BARRY':'barry','PROF_ROWAN':'prof_rowan','PLAYER_HOUSE_MOM':'player_mother','RIVAL_MOM':'rival_mother','POKEMON_CENTER_NURSE':'pokecenter_nurse','SHOP_ATTENDANT':'shop_attendant','POKEMON_BREEDER_F':'pokemon_breeder_f','POKEMON_BREEDER_M':'pokemon_breeder_m'}
 out=ROOT/'gfx/private/platinum_npcs';out.mkdir(exist_ok=True);records={};images=[]
 for key in sorted(gfxids):
  stem=key.removeprefix('OBJ_EVENT_GFX_');file=aliases.get(stem,stem.lower());paths=[S/'res/graphics/field_sprites'/folder/(file+'.png') for folder in ['npc','player','objects','pokemon']];path=next((v for v in paths if v.exists()),None)
  if path is None:records[key]={'supported':False,'reason':'Original sprite/model service pending'};continue
  im=Image.open(path).convert('RGBA')
  if im.size==(16,16):
   cell=Image.new('RGBA',(32,32));cell.alpha_composite(im,(8,16));im=cell
  assert im.width==32 and im.height%32==0,(key,im.size)
  records[key]={'supported':True,'index':len(images)//12,'source':str(path.relative_to(S)),'sha256':hashlib.sha256(path.read_bytes()).hexdigest()}
  for i in range(12):
   dest=out/('%04d.png'%len(images));im.crop((0,(i%(im.height//32))*32,32,(i%(im.height//32))*32+32)).save(dest);images.append('private/platinum_npcs/'+dest.name)
 records['OBJ_EVENT_GFX_VAR_0']={'supported':True,'index':65534,'reason':'Original opposite-gender counterpart, resolved at runtime'}
 (ROOT/'gfx/platinum_npcs.t3s').write_text('--atlas -f rgba5551 -z auto\n'+'\n'.join(images)+'\n')
 (ROOT/'provenance/platinum-graphics.json').write_text(json.dumps({'original_intro':True,'npc_graphics':records},indent=2)+'\n');print('Converted original intro and NPC sheets:',len(images)//12)
if __name__=='__main__':main()
