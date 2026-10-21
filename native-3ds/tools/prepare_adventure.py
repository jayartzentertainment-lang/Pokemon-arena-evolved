#!/usr/bin/env python3
"""Convert retained trainer layers, Platinum NPCs/data and supplied Emerald map tiles.
No network, original files unchanged. Native layouts/dialogue live in content/.
"""
import argparse,hashlib,json,struct,subprocess,sys
from pathlib import Path
from PIL import Image,ImageDraw
R=Path(__file__).resolve().parents[1]
def main():
 p=argparse.ArgumentParser();p.add_argument('--ds-source',type=Path,required=True);p.add_argument('--creator',type=Path,required=True);p.add_argument('--world-source',type=Path,required=True);a=p.parse_args()
 sources={}
 def read(path):
  raw=path.read_bytes();sources[str(path)]={'sha256':hashlib.sha256(raw).hexdigest(),'bytes':len(raw)};return raw
 def sheet(name,images):
  dst=R/'gfx/private'/name;dst.mkdir(exist_ok=True);lines=[]
  for i,im in enumerate(images):
   im.save(dst/f'{i:04d}.png');lines.append(f'private/{name}/{i:04d}.png')
  (R/'gfx'/f'{name}.t3s').write_text('--atlas -f rgba5551 -z auto\n'+'\n'.join(lines)+'\n')
 cat=json.loads(read(a.creator/'catalog.json'));headers=['/* Generated from supplied art catalog; identical32x48 anchors. */','#include "adventure.h"'];counts=[];label_defs=[]
 for gi,group in enumerate(['body','bottoms','tops','hair','headgear','accessories']):
  images=[];labels=[]
  if group=='body':options=list(cat['bodies']['A']['skins']);counts.append(len(options))
  else:options=list(cat['layers'][group]);counts.append(len(options))
  for key in options:
   labels.append(('SKIN '+key) if group=='body' else ('NONE' if key=='0' else key.replace('/',' / ')))
   for body in ['A','B']:
    rec=cat['bodies'][body]['skins'][key] if group=='body' else cat['layers'][group][key]['files'][body]
    path=a.creator/rec['path'];raw=read(path);assert hashlib.sha256(raw).hexdigest()==rec['sha256']
    im=Image.open(path).convert('RGBA');assert im.size in [(128,192),(128,196)]
    if im.height==196:assert not im.getchannel('A').crop((0,192,128,196)).getbbox()
    for direction in range(4):
     for frame in range(4):images.append(im.crop((frame*32,direction*48,frame*32+32,direction*48+48)))
  if group=='hair':
   for body_index,body_name in enumerate(['A','B']):
    sheet('trainer_hair_'+body_name,[images[(option*2+body_index)*16+frame] for option in range(len(options)) for frame in range(16)])
   (R/'gfx/trainer_hair.t3s').unlink(missing_ok=True)
  else:sheet('trainer_'+group,images)
  label_defs.append('static const char *labels_%u[]={%s};'%(gi,','.join(json.dumps(v) for v in labels)))
 headers+=label_defs;headers.append('const uint8_t gAppearanceCounts[6]={'+','.join(map(str,counts))+'};');headers.append('const char *const *gAppearanceLabels[6]={'+','.join('labels_'+str(i) for i in range(6))+'};')
 campaign=json.loads(read(R/'content/campaign.json'))
 # Authentic Platinum NPC32x32 idle cels: source rows0..2 face north,3..5 south.
 names=sorted(set(e['npc'] for e in campaign['events'])|{'nurse','clerk'});npcimages=[]
 for name in names:
  alias={'dawn':'player/player_f','galactic_grunt_m':'npc/grunt_m','nurse':'npc/pokecenter_nurse','clerk':'npc/shop_attendant'}.get(name,'npc/'+name)
  path=a.ds_source/'res/graphics/field_sprites'/(alias+'.png')
  if not path.exists():
   alias={'clerk':'npc/cashier_m'}.get(name,alias);path=a.ds_source/'res/graphics/field_sprites'/(alias+'.png')
  read(path);im=Image.open(path).convert('RGBA');npcimages.append(im.crop((0,96,32,128)))
 sheet('story_npcs',npcimages);headers.append('const unsigned gNpcCount=%u;'%len(names));headers.append('const char *gNpcNames[]={'+','.join(json.dumps(v) for v in names)+'};')
 # Export world offsets through its portable metadata, never parse C initializers.
 helper=R/'validation/export_world.c';helper.write_text('#include "world_data.h"\n#include <stdio.h>\nint main(){const char *names[]={"LittlerootTown","OldaleTown","Route101"};for(unsigned i=0;i<3;i++){int n=world_map_find(names[i]);if(n<0)return 1;const WorldMap *m=&world_maps[n];printf("%u,%u,%u,%u,%u\\n",m->atlas_offset,m->cells_offset,m->width,m->height,m->tile_count);}return 0;}\n')
 exe=R/'validation/export_world';subprocess.run(['cc','-I'+str(a.world_source/'include'),str(helper),str(a.world_source/'source/world_data.c'),'-o',str(exe)],check=True)
 rows=[list(map(int,line.split(','))) for line in subprocess.check_output([str(exe)],text=True).splitlines()];world=read(a.world_source/'assets/world.pack')
 off,_,_,_,count=rows[0];tiles=[]
 for i in range(count):
  vals=struct.unpack_from('<256H',world,off+i*512);im=Image.new('RGBA',(16,16));im.putdata([((v&31)*8,((v>>5)&31)*8,((v>>10)&31)*8,255 if v&32768 else 0) for v in vals]);tiles.append(im)
 def original(row):
  off,cells,w,h,count=row;out=Image.new('RGBA',(w*16,h*16))
  for y in range(h):
   for x in range(w):
    b,f,_,_=struct.unpack_from('<4H',world,cells+(y*w+x)*8);out.alpha_composite(tiles[b],(x*16,y*16));out.alpha_composite(tiles[f],(x*16,y*16))
  return out
 town=original(rows[0]);oldale=original(rows[1]);tree=town.crop((0,0,32,32));house=town.crop((32,64,112,144));lab=town.crop((48,192,160,272));center=oldale.crop((80,208,144,272));mart=oldale.crop((208,48,272,112))
 map_defs=[]
 for m in campaign['maps']:
  theme=m['theme'];bg=Image.new('RGBA',(512,384));collision=bytearray(32*24);grass=bytearray(32*24)
  base=tiles[3]
  if theme in ('cave','ruins','distortion','factory','league'):base=tiles[237]
  if theme=='snow':
   base=base.copy();base.putdata([(int(r*.30+170),int(g*.25+180),int(b*.25+190),aa) for r,g,b,aa in base.getdata()])
  if theme=='distortion':
   base=base.copy();base.putdata([(int(b*.7+35),int(r*.35+15),int(g*.7+40),aa) for r,g,b,aa in base.getdata()])
  for y in range(24):
   for x in range(32):
    bg.alpha_composite(base,(x*16,y*16))
    if x in (0,31) or y in (0,23):collision[y*32+x]=1
  def block(rect):
   x0,y0,x1,y1=rect
   for y in range(y0//16,(y1+15)//16):
    for x in range(x0//16,(x1+15)//16):collision[y*32+x]=1
  for y in range(0,384,32):
   for x in (0,480):bg.alpha_composite(tree,(x,y));block((x,y,x+32,y+32))
  for x in range(32,480,32):
   for y in (0,352):bg.alpha_composite(tree,(x,y));block((x,y,x+32,y+32))
  # Connected cross-path: every exit and story actor is reachable.
  for y in range(24):
   for x in range(32):
    if x in (15,16) or y in (12,13):
     bg.alpha_composite(tiles[180],(x*16,y*16));collision[y*32+x]=0
  city=theme in ('town','city','marsh','league')
  if city:
   bg.alpha_composite(house,(80,64));block((80,64,160,144))
   bg.alpha_composite(center,(352,64));block((352,64,416,128))
   bg.alpha_composite(mart,(80,256));block((80,256,144,320))
   bg.alpha_composite(lab,(336,256));block((336,256,448,336))
  elif theme in ('cave','ruins','factory','distortion'):
   for x,y in [(96,80),(368,80),(96,288),(384,288)]:
    rock=tiles[240];bg.alpha_composite(rock,(x,y));block((x,y,x+16,y+16))
  # Actual tall-grass art; avoid buildings, service actors and cross-path.
  for y in range(16,20):
   for x in range(4,10):
    if not collision[y*32+x]:bg.alpha_composite(tiles[2],(x*16,y*16));grass[y*32+x]=1
  if theme=='lake':
   for y in range(4,10):
    for x in range(3,10):bg.alpha_composite(tiles[239],(x*16,y*16));collision[y*32+x]=1
  # Portal signage is rendered separately; only established roads leave maps.
  sheet('map_%02d'%m['id'],[bg]);map_defs.append('{%s,%u,%u,{%s},{%s},{%s},{%s}}'%(json.dumps(m['name']),m['requires'],len(m['links']),','.join(map(str,m['links'])) or '0',','.join(map(str,m['wild'])),','.join(map(str,collision)),','.join(map(str,grass))))
 headers.append('const AdventureMap gAdventureMaps[ADVENTURE_MAPS]={'+',\n'.join(map_defs)+'};')
 species=(a.ds_source/'generated/species.txt').read_text().splitlines();moves=(a.ds_source/'generated/moves.txt').read_text().splitlines();types=(a.ds_source/'generated/pokemon_types.txt').read_text().splitlines();dexid={s:i for i,s in enumerate(species)};moveid={s:i for i,s in enumerate(moves)}
 learn=[];dexdefs=['{.catchRate=0}'];evol=[]
 for dex in range(1,494):
  path=a.ds_source/'res/pokemon'/species[dex].removeprefix('SPECIES_').lower()/'data.json';d=json.loads(read(path));stats=d['base_stats'];start=len(learn)
  for lv,mv in d['learnset']['by_level']:learn.append('{%u,%u}'%(lv,moveid[mv]))
  elstart=len(evol)
  for entry in d.get('evolutions',[]):
   if len(entry)!=3:continue
   method,param,target=entry
   if method=='EVO_LEVEL':evol.append('{%u,%u}'%(param,dexid[target]))
  dexdefs.append('{{%s},{%s},%u,%u,%u,%u,%u,%u}'%(','.join(str(stats[k]) for k in ['hp','attack','defense','speed','special_attack','special_defense']),','.join(str(types.index(t)) for t in d['types']),d['catch_rate'],d['base_exp_reward'],start,len(learn)-start,elstart,len(evol)-elstart))
 headers.append('const AdventureLearn gAdventureLearn[]={'+','.join(learn)+'};');headers.append('const AdventureEvolution gAdventureEvolution[]={'+','.join(evol)+'};');headers.append('const AdventureDex gAdventureDex[494]={'+',\n'.join(dexdefs)+'};')
 trainer_defs=[];mon_defs=[]
 for i,event in enumerate(campaign['events']):
  trainer=event['trainer'];start=len(mon_defs)
  if trainer:
   path=a.ds_source/'res/trainers/data'/(trainer+'.json');d=json.loads(read(path))
   for mon in d['party']:
    lv=mon['level'];dex=dexid[mon['species']];mv=[moveid[m] for m in mon.get('moves',[])]
    if not mv:mv=[moveid[m] for level,m in json.loads((a.ds_source/'res/pokemon'/species[dex].removeprefix('SPECIES_').lower()/'data.json').read_text())['learnset']['by_level'] if level<=lv][-4:]
    mv=mv+([0]*(4-len(mv)));mon_defs.append('{%u,%u,{%s}}'%(dex,lv,','.join(map(str,mv))))
  trainer_defs.append('{%s,%s,%u,%u,%u,%u,%u,%d,%u,%u}'%(json.dumps(event['name']),json.dumps(event['text']),event['map'],names.index(event['npc']),['talk','starter','rival','trainer','duo','legend','ending'].index(event['kind']),start,len(mon_defs)-start,event['badge'],event['level'],int(event['doubles'])))
 headers.append('const AdventureTrainerMon gStoryTeams[]={'+','.join(mon_defs)+'};');headers.append('const AdventureEvent gAdventureEvents[ADVENTURE_EVENTS]={'+',\n'.join(trainer_defs)+'};')
 from prepare_native_ui import convert
 convert(a.ds_source)
 (R/'source/game/adventure_data.c').write_text('\n'.join(headers)+'\n');(R/'assets/adventure-manifest.json').write_text(json.dumps({'schema':1,'source_files':sources,'maps':len(map_defs),'story_events':len(trainer_defs),'appearance_options':counts,'authentic_NPCs':names,'party_data':'Original Platinum teams, levels and learned moves; AI/item behaviour remains Arena-specific','map_art':'User supplied Emerald16px tiles; new native2D layouts, not original Platinum layouts','scope':campaign['scope']},indent=2)+'\n');print('Prepared',len(map_defs),'maps,',len(trainer_defs),'events, trainer layers and493 species progression data.')
if __name__=='__main__':main()
