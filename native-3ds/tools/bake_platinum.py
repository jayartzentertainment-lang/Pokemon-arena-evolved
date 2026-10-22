#!/usr/bin/env python3
"""Convert original Nitro cells/props without drawing replacement maps.
Requires apicula at its recorded revision and Blender. Original coordinates,
model scale, model geometry and texture material colours remain the inputs.
"""
from pathlib import Path
import argparse,json,struct,subprocess,hashlib
ROOT=Path(__file__).resolve().parents[1]
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--source',type=Path,required=True);p.add_argument('--apicula',type=Path,required=True);p.add_argument('--only',type=int);a=p.parse_args();S=a.source
 models=json.loads((ROOT/'assets/private/platinum/models.json').read_text());base=ROOT/'validation/platinum-bake';base.mkdir(exist_ok=True);scenes=[]
 order=(S/'res/field/props/models/map_prop_models.order').read_text().splitlines();cache={}
 def convert(path,textures,key):
  out=base/key
  if key not in cache:
   out.mkdir(exist_ok=True)
   with (out/'conversion.log').open('w') as log:subprocess.run([str(a.apicula),'convert','-f=glb',str(path),*map(str,textures),'-o',str(out),'--overwrite'],check=True,stdout=log,stderr=subprocess.STDOUT)
   glbs=list(out.glob('*.glb'));assert len(glbs)==1,(key,len(glbs));cache[key]=str(glbs[0])
  return cache[key]
 for info in models:
  if a.only is not None and info['header']!=a.only:continue
  raw=(S/'res/field/maps/data'/('map_data_%03d.bin'%info['land'])).read_bytes();terrain,props,model,height=struct.unpack_from('<4I',raw)
  tmp=base/('land_%03d.nsbmd'%info['land']);tmp.write_bytes(raw[16+terrain+props:16+terrain+props+model]);tex=[S/'res/field/maps/texture_sets'/(info['texture']+'.nsbtx')]
  floor=convert(tmp,tex,'land_%03d'%info['land']);items=[{'model':floor,'position':[0,0,0],'rotation':[0,0,0],'scale':[1,1,1]}]
  # Props contain their own textures in Platinum, with the map textures as fallback.
  for at in range(16+terrain,16+terrain+props,48):
   v=struct.unpack_from('<12i',raw,at);path=S/'res/field/props/models'/order[v[0]]
   glb=convert(path,tex,'prop_%03d_tex%s'%(v[0],info['texture'][-3:]))
   items.append({'model':glb,'position':[k/4096 for k in v[1:4]],'rotation':list(v[4:7]),'scale':[k/4096 for k in v[7:10]]})
  x,z=info['cell'];out=ROOT/'gfx/private/platinum'/('h%03d_%02d_%02d.png'%(info['header'],x,z));out.parent.mkdir(exist_ok=True)
  scenes.append({'items':items,'output':str(out),'header':info['header'],'cell':[x,z]})
 scene=base/'scenes.json';scene.write_text(json.dumps(scenes,indent=2))
 with (base/'blender.log').open('w') as log:subprocess.run(['blender','-b','-t','2','--python',str(ROOT/'tools/bake_platinum_blender.py'),'--',str(scene)],stdout=log,stderr=subprocess.STDOUT,check=True)
 for row in scenes:
  out=Path(row['output']);assert out.is_file();key=out.stem
  (ROOT/'gfx'/(key+'.t3s')).write_text('--atlas -f rgba5551 -z auto\nprivate/platinum/'+out.name+'\n')
 report={'projection':'orthographic top-down conversion of original Nitro geometry; not original animated perspective renderer','cells':scenes,'outputs_sha256':{Path(r['output']).name:hashlib.sha256(Path(r['output']).read_bytes()).hexdigest() for r in scenes}}
 (ROOT/'provenance/platinum-bake.json').write_text(json.dumps(report,indent=2)+'\n');print('Converted original cells:',len(scenes),flush=True)
if __name__=='__main__':main()
