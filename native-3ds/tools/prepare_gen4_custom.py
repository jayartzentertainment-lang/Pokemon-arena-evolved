#!/usr/bin/env python3
"""Port the already-cleaned DS Custom bank from the supplied pokemon png.rar.
Keep native32x32 cells, shared anchors and original DS semantic palette.
No Lucas/Dawn or named trainer pixels are used as Custom body artwork.
"""
from pathlib import Path
from PIL import Image
import argparse,struct,json,hashlib,shutil
R=Path(__file__).resolve().parents[1]
SKIN=[[(31,26,23),(27,20,15)],[(31,27,25),(29,22,19)],[(25,18,12),(20,13,8)],[(17,11,8),(12,7,5)]]
HAIR=[[(7,8,10),(7,12,17)],[(4,4,5),(8,8,10)],[(9,5,3),(15,9,5)],[(16,11,4),(28,22,10)],[(13,4,3),(23,10,5)],[(13,14,17),(24,25,27)]]
TOP=[[(23,7,9),(27,13,15)],[(5,11,23),(12,19,30)],[(5,16,9),(13,24,14)],[(14,7,22),(22,14,29)],[(25,11,3),(31,20,8)],[(3,16,17),(9,25,24)],[(6,7,9),(14,15,18)],[(22,19,13),(30,28,22)]]
LEGS=[[(7,11,18),(14,19,27)],[(5,6,8),(12,13,16)],[(15,12,7),(25,22,15)],[(8,15,10),(17,24,17)]]
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--ds-source',type=Path);p.add_argument('--bank-directory',type=Path,default=R/'assets/private/trainer-bases');a=p.parse_args();S=a.ds_source/'res/arena/avatar' if a.ds_source else a.bank_directory;raw=(S/'custom.bin').read_bytes();prov=json.loads((S/'custom-provenance.json').read_text());assert raw[:4]==b'AVC1' and hashlib.sha256(raw).hexdigest()==prov['sha256'];assert struct.unpack_from('<3I',raw,4)==(1,416,0)
 retained=R/'assets/private/trainer-bases';retained.mkdir(exist_ok=True)
 for name in ['custom.bin','custom-provenance.json','EDITABLE-LAYERS.md']:
  if (S/name).resolve()!=(retained/name).resolve():shutil.copyfile(S/name,retained/name)
 counts=[4,13,25,25,17,1];groups=['body','bottoms','tops','hair','headgear','accessories']
 def frame(body,layer,direction,step):
  offset,size,stride,palsize=struct.unpack_from('<4I',raw,16+(body*16*13+layer)*16);assert stride==512 and not palsize and size>=16*512
  i=[1,2,3,0][direction]*4+step;data=raw[offset+i*512:offset+(i+1)*512];return [n for b in data for n in [b&15,b>>4]]
 def palette(skin=0,hair=0,top=0,leg=0):
  skinpair=SKIN[skin];hairpair=HAIR[hair];toppair=TOP[top];legpair=LEGS[leg]
  dark=lambda c:tuple(x//2 for x in c)
  # Bare base feet keep the supplied skin until an independent shoe option is ported.
  cols=[(0,0,0),(3,3,4),skinpair[0],skinpair[1],dark(skinpair[1]),hairpair[1],hairpair[0],dark(hairpair[0]),toppair[1],toppair[0],dark(toppair[0]),legpair[1],legpair[0],skinpair[0],skinpair[1],(29,29,28)]
  return [tuple(v*255//31 for v in c)+(0 if i==0 else 255,) for i,c in enumerate(cols)]
 for g,name in enumerate(groups):
  out=R/'gfx/private'/('trainer_gen4_'+name);out.mkdir(exist_ok=True);images=[]
  for option in range(counts[g]):
   for body in range(2):
    layer=0;pal=palette();empty=g!=0 and option==0
    if g==0:pal=palette(skin=option)
    elif g==1 and option:layer=1+(option-1)//4;pal=palette(leg=(option-1)%4)
    elif g==2 and option:layer=4+(option-1)//8;pal=palette(top=(option-1)%8)
    elif g==3 and option:layer=7+(option-1)//6;pal=palette(hair=(option-1)%6)
    elif g==4 and option:layer=11+(option-1)//8;pal=palette(top=(option-1)%8)
    for direction in range(4):
     for step in range(4):
      idx=[0]*1024 if empty else frame(body,layer,direction,step);im=Image.new('RGBA',(32,32));im.putdata([pal[n] for n in idx]);dest=out/('%04d.png'%len(images));im.save(dest);images.append('private/'+out.name+'/'+dest.name)
  (R/'gfx'/('trainer_gen4_'+name+'.t3s')).write_text('--atlas -f rgba5551 -z auto\n'+'\n'.join(images)+'\n')
 (R/'source/game/trainer_gen4_data.c').write_text('/* Gen IV-proportioned supplied Custom bank, ported from retained DS artwork. */\n#include "adventure.h"\nconst uint8_t gGen4AppearanceCounts[6]={'+','.join(map(str,counts))+'};\n')
 report={'source_archive':prov['source_archive'],'retained_ds_bank_sha256':prov['sha256'],'native_cell':[32,32],'native_direction_order':['south','west','east','north'],'default':'Blank male/female body only; all optional layers None','counts_by_layer':dict(zip(groups,counts)),'default_native_custom_base':True,'named_trainer_pixels_used_in_custom':False,'selected_art':'Retained DS clean body,4 hairstyles,3 tops,3 legwear,2 hats; supplied matching dyes','limits':'Walking layers converted for native field/creator; run/bike/front/back source remains retained, not yet claimed native cinematic support; carried accessories and independent shoes remain to be ported'}
 (R/'provenance/gen4-custom.json').write_text(json.dumps(report,indent=2)+'\n');print('Imported Gen IV-proportioned blank Custom and matching layered options from supplied pokemon png.rar / retained DS bank')
if __name__=='__main__':main()
