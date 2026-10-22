#!/usr/bin/env python3
"""Import dedicated Hurt poses and corrected Arceus from pinned SpriteCollab.
No generated/repainted artwork. Crops retain each frame's original white shadow marker.
Missing sheets/markers remain unsupported and are listed, never synthesized.
"""
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor
import hashlib,io,json,urllib.request,time,xml.etree.ElementTree as ET
from PIL import Image
ROOT=Path(__file__).resolve().parents[1]
REV='4702f10d888d19e804c5986a91170dfc92ae01b8'
BASE='https://raw.githubusercontent.com/PMDCollab/SpriteCollab/'+REV+'/'
RAW=ROOT/'assets/private/spritecollab'/REV;RAW.mkdir(parents=True,exist_ok=True)
def fetch(path):
 file=RAW/path
 if file.exists():return file.read_bytes()
 for attempt in range(3):
  try:
   raw=urllib.request.urlopen(BASE+path,timeout=20).read();file.parent.mkdir(parents=True,exist_ok=True);file.write_bytes(raw);return raw
  except urllib.error.HTTPError as e:
   if e.code==404:return None
  except Exception:
   pass
  time.sleep(.25*(attempt+1))
 return None

def load(dex):
 folder='sprite/%04d/'%dex
 raw=fetch(folder+'AnimData.xml');credits=fetch(folder+'credits.txt')
 if raw is None:return dex,None,'No original animation descriptor'
 try:
  tree=ET.fromstring(raw);anims={a.findtext('Name'):a for a in tree.findall('.//Anim')}
  targets=['Hurt']+(['Idle','Walk','Shoot','Attack','Charge'] if dex==493 else [])
  records={}
  for name in targets:
   actual=name
   if actual not in anims and dex==493:actual='SpAttack' if name=='Charge' and 'SpAttack' in anims else 'Attack'
   for _ in range(16):
    copy=anims[actual].findtext('CopyOf')
    if not copy:break
    actual=copy
   a=anims[actual];w=int(a.findtext('FrameWidth'));h=int(a.findtext('FrameHeight'));dur=[int(t.text) for t in a.findall('./Durations/Duration')]
   picture=fetch(folder+actual+'-Anim.png');offsets=fetch(folder+actual+'-Offsets.png');shadow=fetch(folder+actual+'-Shadow.png')
   if picture is None or offsets is None or shadow is None:raise ValueError('Missing '+actual+' cel/offset sheet')
   im=Image.open(io.BytesIO(picture)).convert('RGBA');off=Image.open(io.BytesIO(shadow)).convert('RGBA')
   if im.size!=(len(dur)*w,8*h) or off.size!=im.size:raise ValueError('Unexpected animation dimensions '+actual)
   hit=int(a.findtext('HitFrame') or 0)
   records[name]=(w,h,dur,im,off,actual,sum(dur[:hit]))
  return dex,records,None
 except Exception as e:return dex,None,str(e)

def main():
 files=[];data=['/* Pinned SpriteCollab import; matching credits and licence bundled. */','#include "assets3ds.h"'];animations={};unsupported={};source=[];arceus=[]
 for path in ['credit_names.txt','LICENSE','LICENSE.md','README.md']:fetch(path)
 def compile_frames(dex,name,record,images,lookup,out):
  w,h,durations,im,offsets,actual,hit_tick=record;frames=[]
  for direction in range(8):
   for frame in range(len(durations)):
    box=(frame*w,direction*h,(frame+1)*w,(direction+1)*h);cell=im.crop(box);bounds=cell.getbbox()
    if not bounds:raise ValueError('Empty '+name+' cel')
    markers=offsets.crop(box);feet=[(x,y) for y in range(h) for x in range(w) if markers.getpixel((x,y))==(255,255,255,255)]
    if len(feet)!=1:raise ValueError('Expected one original white shadow marker')
    foot=feet[0];crop=cell.crop(bounds);key=(crop.size,hashlib.sha256(crop.tobytes()).hexdigest())
    if key not in lookup:
     lookup[key]=len(images);path=out/('%04d.png'%len(images));crop.save(path);images.append('private/'+out.name+'/'+path.name)
    frames.append((lookup[key],foot[0]-bounds[0],foot[1]-bounds[1],*crop.size))
  prefix='hurt%03d'%dex if name=='Hurt' else 'arceus_'+name.lower()
  data.append('static const uint8_t '+prefix+'_ticks[]={'+','.join(map(str,durations))+'};')
  data.append('static const ArenaSpriteFrame '+prefix+'_frames[]={'+','.join('{'+','.join(map(str,f))+'}' for f in frames)+'};')
  return '{%u,%u,%u,%s_ticks,%s_frames}'%(len(durations),sum(durations),hit_tick,prefix,prefix)
 with ThreadPoolExecutor(max_workers=8) as pool:
  for dex,records,error in pool.map(load,range(1,494)):
   if error:unsupported[dex]=error;continue
   out=ROOT/'gfx/private'/('hurt_%04d'%dex);out.mkdir(exist_ok=True);images=[];lookup={}
   try:animations[dex]=compile_frames(dex,'Hurt',records['Hurt'],images,lookup,out)
   except Exception as e:unsupported[dex]=str(e);continue
   (ROOT/'gfx'/('hurt_%04d.t3s'%dex)).write_text('--atlas -f rgba5551 -z auto\n'+'\n'.join(images)+'\n')
   if dex==493:
    out=ROOT/'gfx/private/arceus_corrected';out.mkdir(exist_ok=True);images=[];lookup={}
    for name in ['Idle','Walk','Shoot','Attack','Charge']:arceus.append(compile_frames(dex,name,records[name],images,lookup,out))
    (ROOT/'gfx/pokemon_0493.t3s').write_text('--atlas -f rgba5551 -z auto\n'+'\n'.join(images)+'\n')
   if dex%50==0:print('Imported dedicated Hurt:',dex,flush=True)
 data.append('static const ArenaSpriteAnimation hurtData[494]={'+','.join('[%u]=%s'%(k,v) for k,v in animations.items())+'};')
 data.append('const ArenaSpriteAnimation *ArenaSpecies_Hurt(unsigned dex){return dex<494&&hurtData[dex].count?&hurtData[dex]:0;}')
 if arceus:
  data.append('const ArenaSpecies gNativeArceus={493,"ARCEUS","romfs:/gfx/pokemon_0493.t3x",1.0f,0,{493,195,140,140,140,{414,304,245,287},{0,0},140,140},{'+','.join(arceus)+'}};')
 (ROOT/'source/imported/private/hurt_data.c').write_text('\n'.join(data)+'\n')
 for path in RAW.rglob('*'):
  if path.is_file():source.append({'path':str(path.relative_to(RAW)),'sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'bytes':path.stat().st_size})
 report={'repository':'https://github.com/PMDCollab/SpriteCollab','revision':REV,'source_files':source,'native_dedicated_hurt_species':len(animations),'unsupported':unsupported,'arceus_replaced_from_original_directional_sheets':bool(arceus),'modifications':'Crop/deduplicate only; white source shadow marker retained as foot anchor. Native Hurt playback follows the hit reaction duration.','credit_names':'assets/private/spritecollab/'+REV+'/credit_names.txt','license':'CC-BY-NC4.0; supplied classic assets retain original authorship'}
 (ROOT/'provenance/hurt-sprites.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='source_files'},indent=2),flush=True)
if __name__=='__main__':main()
