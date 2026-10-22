#!/usr/bin/env python3
"""Convert original Platinum field/text assets without replacing their artwork.
Uses the supplied original source. Glyphs retain original pixels, case, widths
and shadow colours; the field subscreen is decoded from the original tilemap.
"""
from pathlib import Path
import argparse,json,hashlib,re,struct
from PIL import Image
R=Path(__file__).resolve().parents[1]
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--source',type=Path,required=True);a=p.parse_args();S=a.source;files={}
 def original(name):
  q=S/name;raw=q.read_bytes();files[name]=hashlib.sha256(raw).hexdigest();dst=R/'assets/private/platinum/original'/name;dst.parent.mkdir(parents=True,exist_ok=True);dst.write_bytes(raw);return q
 def atlas(name,cells):
  d=R/'gfx/private'/name;d.mkdir(exist_ok=True);lines=[]
  for i,im in enumerate(cells):
   q=d/('%04d.png'%i);im.save(q);lines.append('private/'+name+'/'+q.name)
  (R/'gfx'/(name+'.t3s')).write_text('--atlas -f rgba5551 -z auto\n'+'\n'.join(lines)+'\n')
 font=Image.open(original('res/fonts/font_message.png'));meta=json.loads(original('res/fonts/font_message.json').read_text());widths=meta['glyphWidths'];cells=[]
 for i in range(len(widths)):
  box=((i%16)*16,(i//16)*16,(i%16+1)*16,(i//16+1)*16);idx=font.crop(box);im=idx.convert('RGBA');im.putalpha(Image.frombytes('L',idx.size,bytes(0 if n==0 else 255 for n in idx.getdata())));cells.append(im)
 atlas('platinum_message_font',cells)
 mapping={}
 for line in original('tools/msgenc/charmap.txt').read_text().splitlines():
  m=re.match(r'^([0-9A-F]{4})=(.)$',line)
  if m and 0<int(m[1],16)<=len(widths):mapping[ord(m[2])]=int(m[1],16)-1
 fallback=mapping[ord('?')];rows=sorted(mapping.items())
 data=['/* Original Platinum glyph indices/widths. See original-ui.json. */','#include "platinum.h"','const uint8_t gPtFontWidths[]={'+','.join(map(str,widths))+'};','static const uint32_t unicode[]={'+','.join(str(u) for u,i in rows)+'};','static const uint16_t glyph[]={'+','.join(str(i) for u,i in rows)+'};', 'unsigned Platinum_Glyph(unsigned cp){unsigned lo=0,hi=sizeof(unicode)/sizeof(*unicode);while(lo<hi){unsigned mid=(lo+hi)/2;if(unicode[mid]<cp)lo=mid+1;else hi=mid;}return lo<sizeof(unicode)/sizeof(*unicode)&&unicode[lo]==cp?glyph[lo]:%u;}'%fallback]
 (R/'source/imported/private/platinum_ui_data.c').write_text('\n'.join(data)+'\n')
 cells=[]
 for name in ['message_box_00','standard_field']:
  src=Image.open(original('res/graphics/windows/'+name+'.png'));im=src.convert('RGBA');im.putalpha(Image.frombytes('L',src.size,bytes(0 if n==0 else 255 for n in src.getdata())))
  for y in range(0,im.height,8):
   for x in range(0,im.width,8):cells.append(im.crop((x,y,x+8,y+8)))
 atlas('platinum_windows',cells)
 tiles=Image.open(original('res/graphics/poketch/unavailable_bg_tiles.png')).convert('RGBA');raw=original('res/graphics/poketch/unavailable.NSCR').read_bytes();at=raw.index(b'NRCS');w,h=struct.unpack_from('<HH',raw,at+8);screen=Image.new('RGBA',(w,h));ncols=tiles.width//8
 for z in range(h//8):
  for x in range(w//8):
   v=struct.unpack_from('<H',raw,at+20+(z*w//8+x)*2)[0];i=v&1023;cell=tiles.crop((i%ncols*8,i//ncols*8,i%ncols*8+8,i//ncols*8+8))
   if v&1024:cell=cell.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
   if v&2048:cell=cell.transpose(Image.Transpose.FLIP_TOP_BOTTOM)
   screen.alpha_composite(cell,(x*8,z*8))
 atlas('platinum_field_bottom',[screen.crop((0,0,256,192))])
 (R/'provenance/original-ui.json').write_text(json.dumps({'input_sha256':files,'glyphs':len(widths),'unicode_mappings':len(rows),'font':'Original FONT_MESSAGE, unscaled source pixels, original proportional widths/case/shadows','windows':'Original message_box_00 and standard_field','field_subscreen':'Original PoketchUnavailableScreen assets, used only before original Poketch acquisition'},indent=2)+'\n')
 print('Converted original Platinum font, message/system borders and pre-Poketch field screen')
if __name__=='__main__':main()
