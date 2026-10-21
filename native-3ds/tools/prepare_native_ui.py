#!/usr/bin/env python3
"""Use supplied authentic Platinum window borders and system font at native pixels."""
from pathlib import Path
import argparse,json,hashlib
from PIL import Image
R=Path(__file__).resolve().parents[1]
def convert(ds):
 framepath=ds/'res/graphics/windows/standard_system.png';indexed=Image.open(framepath);frame=indexed.convert('RGBA');alpha=Image.new('L',indexed.size);alpha.putdata([0 if v==0 else 255 for v in indexed.getdata()]);frame.putalpha(alpha);assert set(alpha.getdata())=={0,255}
 dst=R/'gfx/private/window_frame';dst.mkdir(exist_ok=True)
 for y in range(3):
  for x in range(3):frame.crop((x*8,y*8,x*8+8,y*8+8)).save(dst/f'{y*3+x}.png')
 (R/'gfx/window_frame.t3s').write_text('--atlas -f rgba5551 -z auto\n'+'\n'.join(f'private/window_frame/{i}.png' for i in range(9))+'\n')
 def panel(w,h):
  out=Image.new('RGBA',(w,h))
  for row in range(3):
   for col in range(3):
    x0=[0,4,w-4][col];y0=[0,4,h-4][row];ww=[4,w-8,4][col];hh=[4,h-8,4][row];part=frame.crop((col*8,row*8,col*8+8,row*8+8)).resize((ww,hh),Image.Resampling.NEAREST)
    if col==1 and row==1:part=Image.new('RGBA',part.size,(232,236,213,255))
    out.alpha_composite(part,(x0,y0))
  return out
 u=R/'gfx/private/ui';u.mkdir(exist_ok=True);panel(164,28).save(u/'move.png');panel(96,34).save(u/'footer.png')
 (R/'gfx/ui.t3s').write_text('--atlas -f rgba5551 -z auto\nprivate/ui/move.png\nprivate/ui/footer.png\n')
 fp=ds/'res/fonts/font_system.png';font=Image.open(fp);mapping={}
 cp=ds/'tools/msgenc/charmap.txt'
 for line in cp.read_text().splitlines():
  if '=' in line:
   code,ch=line.split('=',1)
   if len(ch)==1:
    try:mapping[ch]=int(code,16)-1
    except ValueError:pass
 for ascii_ch,glyph in [('<','←'),('>','→'),('^','↑')]:mapping[ascii_ch]=mapping[glyph]
 f=R/'gfx/private/font';f.mkdir(exist_ok=True)
 for char in range(32,128):
  ch=chr(char);n=mapping.get(ch,mapping['?']);source=font.crop(((n%16)*16,(n//16)*16,(n%16+1)*16,(n//16+1)*16));im=Image.new('RGBA',(8,10))
  if ch!=' ':
   for yy in range(10):
    for xx in range(8):
     v=source.getpixel((xx,yy+3))
     if v in (1,2):im.putpixel((xx,yy),(255,255,255,255 if v==1 else 100))
  im.save(f/f'{char:04d}.png')
 (R/'gfx/font.t3s').write_text('--atlas -f rgba5551 -z auto\n'+'\n'.join(f'private/font/{c:04d}.png' for c in range(32,128))+'\n')
 (R/'assets/native-ui.json').write_text(json.dumps({'window':'Platinum standard_system, actual8px corner/edge pieces; paper-colored center','font':'Platinum font_system, original glyph pixels, ASCII arrows mapped to actual arrow glyphs','sources':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [framepath,fp,cp]}},indent=2)+'\n')
 manifest=R/'assets/manifest.json'
 if manifest.exists():
  data=json.loads(manifest.read_text());data['font']='Actual Platinum font_system glyphs; see native-ui.json';manifest.write_text(json.dumps(data,indent=2)+'\n')
 print('Actual Platinum9-slice window borders and system glyphs prepared.')
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--ds-source',type=Path,required=True);a=p.parse_args();convert(a.ds_source)
