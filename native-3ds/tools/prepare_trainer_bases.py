#!/usr/bin/env python3
"""Reuse retained DS regional sprites as fixed silhouettes with palette masks.
No generated trainer artwork or interchangeable regional hairstyle layers.
Custom keeps the supplied modular base; all optional layers start at None.
"""
from pathlib import Path
from PIL import Image
import argparse,hashlib,json,shutil
R=Path(__file__).resolve().parents[1]
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--ds-source',type=Path,required=True);p.add_argument('--creator',type=Path,required=True);a=p.parse_args();source=a.ds_source/'res/arena/avatar';out=R/'gfx/private/trainer_fixed';out.mkdir(exist_ok=True);manifest=[];lines=[]
 retained=R/'assets/private/trainer-bases';retained.mkdir(parents=True,exist_ok=True)
 # Exactly the native DS recolour slots, split into two colour masks per channel.
 groups={ 'red': [[{2},{3}],[{4,8},{1}],[{12},{11}]],
          'brendan': [[{1},{2,3}],[{6,8},{5,7}],[{11,13},{10,12}]] }
 for who in ['red','brendan']:
  path=source/(who+'-state-00.png');raw=path.read_bytes();shutil.copyfile(path,retained/path.name);im=Image.open(path);assert im.mode=='P' and im.size==(256,128)
  # DS N,S,W,E -> supplied modular/renderer S,W,E,N. First sixteen cels are walk.
  for layer in range(7):
   for direction in [1,2,3,0]:
    for step in range(4):
     frame=direction*4+step;cell=im.crop(((frame%8)*32,(frame//8)*32,(frame%8+1)*32,(frame//8+1)*32));indices=list(cell.getdata());rgba=cell.convert('RGBA')
     if layer==0:rgba.putalpha(Image.frombytes('L',(32,32),bytes(255 if v else 0 for v in indices)))
     else:
      slots=groups[who][(layer-1)//2][(layer-1)%2];rgba=Image.new('RGBA',(32,32),(255,255,255,0));rgba.putalpha(Image.frombytes('L',(32,32),bytes(255 if v in slots else 0 for v in indices)))
     dest=out/('%04d.png'%len(lines));rgba.save(dest);lines.append('private/trainer_fixed/'+dest.name)
  manifest.append({'base':who,'path':'res/arena/avatar/'+path.name,'sha256':hashlib.sha256(raw).hexdigest(),'direction_order':['down','left','right','up'],'frames_per_direction':4,'regional_hairstyle_editable':False,'changes':'Source pixels/anchors retained; binary palette masks match retained DS recolour groups; no sprite geometry changes'})
 catalog=json.loads((a.creator/'catalog.json').read_text());blank=[]
 for body in ['A','B']:
  rec=catalog['bodies'][body]['skins']['0'];p=a.creator/rec['path'];assert hashlib.sha256(p.read_bytes()).hexdigest()==rec['sha256'];blank.append({'body':body,'source':rec['path'],'sha256':rec['sha256']})
 for group in ['bottoms','tops','hair','headgear','accessories']:
  for body in ['A','B']:
   rec=catalog['layers'][group]['0']['files'][body];p=a.creator/rec['path'];assert hashlib.sha256(p.read_bytes()).hexdigest()==rec['sha256'];assert not Image.open(p).convert('RGBA').getchannel('A').getbbox(),(group,body,'None must be truly empty')
 (R/'gfx/trainer_fixed.t3s').write_text('--atlas -f rgba5551 -z auto\n'+'\n'.join(lines)+'\n')
 report={'fixed_regional_bases':manifest,'custom_blank_bodies':blank,'custom_default_optional_layers':'None (all indices zero)','atlas_layout':'base[Red,Brendan] * 112 + layer[original,skin0,skin1,hair0,hair1,outfit0,outfit1] * 16 + direction[S,W,E,N]*4 + step','runtime_scope':'Creator, trainer card and native/legacy field use the same Trainer function; original cinematic/battle trainer poses still need integration'}
 (R/'provenance/trainer-bases.json').write_text(json.dumps(report,indent=2)+'\n');print('Imported fixed Red/Brendan and verified supplied blank Custom body + empty optional layers')
if __name__=='__main__':main()
