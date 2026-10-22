#!/usr/bin/env python3
"""Use supplied PMD wind and dust cels, with uniform integer reduction only."""
from pathlib import Path
import argparse,hashlib,json,shutil
from PIL import Image
R=Path(__file__).resolve().parents[1]
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--source',type=Path);a=p.parse_args()
 retained=R/'assets/private/dash-source';source=a.source or retained
 out=R/'gfx/private/dash_2d';out.mkdir(parents=True,exist_ok=True);images=[];records=[]
 for family,indices,reduce in [('0027',[0,4,8,12,18,24],2),('0097',[0,6,12,18,24,28],4)]:
  frames=[]
  for n in indices:
   rel=Path(family)/'000'/('%03d.png'%n);path=source/rel;raw=path.read_bytes()
   dest=retained/rel;dest.parent.mkdir(parents=True,exist_ok=True)
   if path.resolve()!=dest.resolve():shutil.copyfile(path,dest)
   im=Image.open(path).convert('RGBA');im=im.resize((im.width//reduce,im.height//reduce),Image.Resampling.NEAREST);frames.append(im)
   records.append({'source':str(rel),'sha256':hashlib.sha256(raw).hexdigest(),'uniform_integer_reduction':reduce})
  for direction in range(8) if family=='0027' else range(1):
   # Native direction S,SE,E,NE,N,NW,W,SW. No GPU perspective stretching.
   angle=[-90,-45,0,45,90,135,180,225][direction]
   for im in frames:
    cel=im.rotate(angle,Image.Resampling.NEAREST,expand=True) if family=='0027' else im
    filename='%03d.png'%len(images);cel.save(out/filename);images.append('private/dash_2d/'+filename)
 (R/'gfx/dash_2d.t3s').write_text('--atlas -f rgba5551 -z auto\n'+'\n'.join(images)+'\n')
 (R/'provenance/dash-2d.json').write_text(json.dumps({'source':'User-supplied PMD Explorers of Sky move_VFX','wind_images':'direction * 6 + cel; eight nearest-neighbour authored variants','dust_images':'48 + cel','sources':records,'transformations':'Uniform integer reduction and nearest-neighbour in-plane rotations; source colours/alpha retained','runtime':'Native aspect ratio, dash-owner lifecycle, no trail after dash completion, dust only on grounded actors'},indent=2)+'\n')
 print('Prepared 48 directional PMD wind cels and 6 dust cels.')
if __name__=='__main__':main()
