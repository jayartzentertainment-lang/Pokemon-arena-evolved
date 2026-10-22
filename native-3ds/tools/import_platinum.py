#!/usr/bin/env python3
"""Import supplied original Platinum data. No ROM download or rewritten content.

The audit copies lossless original data for every area. Native C tables compile
only the initial-area scripts; unsupported commands remain explicit VM faults.
The map baker consumes the original extracted Nitro models separately.
"""
from pathlib import Path
import argparse, ast, collections, hashlib, json, re, shutil, struct, unicodedata
ROOT=Path(__file__).resolve().parents[1]
START=['twinleaf_town_player_house_2f','twinleaf_town_player_house_1f','twinleaf_town','twinleaf_town_rival_house_1f','twinleaf_town_rival_house_2f','twinleaf_town_northeast_house','twinleaf_town_southwest_house','route_201','verity_lakefront','lake_verity','sandgem_town','sandgem_town_pokemon_research_lab','sandgem_town_counterpart_house_1f','sandgem_town_counterpart_house_2f','sandgem_town_pokecenter_1f','sandgem_town_mart','sandgem_town_house','lake_verity_low_water']
def sha(raw):return hashlib.sha256(raw).hexdigest()
def cstr(s):return json.dumps(s,ensure_ascii=True)
def textvalue(x):return ''.join(x) if isinstance(x,list) else x
def parse_script(path):
 code=[];labels={};entries=[]
 raw=re.sub(r'/\*.*?\*/',lambda m:'\n'*m.group(0).count('\n'),path.read_text(),flags=re.S)
 for line_no,line in enumerate(raw.splitlines(),1):
  line=line.split('//')[0].strip()
  if not line or line.startswith(('#','.')):continue
  if line.endswith(':'):labels[line[:-1]]=len(code);continue
  a=line.split(None,1);op=a[0];args=[s.strip() for s in a[1].split(',')] if len(a)>1 else []
  if op=='ScriptEntry':entries+=args;continue
  if op=='ScriptEntryEnd':continue
  code.append({'op':op,'args':args,'line':line_no})
 return {'file':path.name,'labels':labels,'entries':entries,'instructions':code}
def enum(path,known):
 n=0
 for line in path.read_text().splitlines():
  line=line.split('#')[0].split('//')[0].strip()
  if not line:continue
  if '=' in line:
   key,expr=map(str.strip,line.split('=',1))
   try:n=integer(expr,known)
   except ValueError:continue
  else:key=line
  if re.fullmatch(r'\w+',key):known[key]=n;n+=1

def integer(s,known):
 s=s.replace('TRUE','1').replace('FALSE','0')
 if s in known:return known[s]
 try:
  a=ast.parse(s,mode='eval')
  def visit(v):
   if isinstance(v,ast.Expression):return visit(v.body)
   if isinstance(v,ast.Constant) and isinstance(v.value,int):return v.value
   if isinstance(v,ast.Name) and v.id in known:return known[v.id]
   if isinstance(v,ast.UnaryOp) and isinstance(v.op,ast.USub):return -visit(v.operand)
   if isinstance(v,ast.BinOp):
    l,r=visit(v.left),visit(v.right)
    if isinstance(v.op,ast.Add):return l+r
    if isinstance(v.op,ast.Sub):return l-r
    if isinstance(v.op,ast.BitOr):return l|r
    if isinstance(v.op,ast.LShift):return l<<r
   raise ValueError(s)
  return visit(a)
 except (SyntaxError,ValueError):raise ValueError(s)

def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--source',type=Path,required=True);args=ap.parse_args();S=args.source.resolve()
 O=ROOT/'assets/private/platinum';O.mkdir(parents=True,exist_ok=True)
 # Original raw content is private and hash-addressed. Source manifests are public.
 manifest={};counts=collections.Counter();ops=collections.Counter();scripts={}
 folders=['res/field','res/text','res/trainers','res/items']
 for folder in folders:
  for p in sorted((S/folder).rglob('*')):
   if not p.is_file():continue
   rel=str(p.relative_to(S));raw=p.read_bytes();manifest[rel]=sha(raw);dest=O/'original'/rel;dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(raw)
   counts[folder]+=1
   if p.suffix=='.s' and '/scripts/' in rel:
    ir=parse_script(p);scripts[p.stem]=ir
    for ins in ir['instructions']:ops[ins['op']]+=1
 (O/'script-ir.json').write_text(json.dumps(scripts,separators=(',',':')))
 symbols={'LOCALID_PLAYER':255,'DIR_NORTH':0,'DIR_SOUTH':1,'DIR_WEST':2,'DIR_EAST':3}
 for path in sorted((S/'generated').glob('*.txt')):enum(path,symbols)
 for _ in range(3):
  for path in sorted((S/'include/constants').rglob('*.h')):
   for key,value in re.findall(r'^#define[ \t]+(\w+)[ \t]+([^\n/]+)',path.read_text(),re.M):
    try:symbols[key]=integer(value.strip(),symbols)
    except ValueError:pass
 # Public numeric IDs contain no proprietary artwork or dialogue.
 (ROOT/'include/platinum_ids.h').write_text('/* Original Platinum numeric IDs, generated. */\n'+''.join('#define PT_'+k+' '+str(v)+'\n' for k,v in sorted(symbols.items()) if k.startswith(('VAR_','FLAG_','MAP_HEADER_','MOVEMENT_TYPE_'))))
 headers={}
 for name,body in re.findall(r'\[(MAP_HEADER_\w+)\]\s*=\s*\{(.*?)\n\s*\}',(S/'include/data/map_headers.h').read_text(),re.S):
  headers[name]=dict(re.findall(r'\.(\w+)\s*=\s*([^,\n]+)',body))
 messages=[];msgids={}
 for p in sorted((S/'res/text').glob('*.json')):
  for m in json.loads(p.read_text()).get('messages',[]):
   msgids[m['id']]=len(messages);messages.append(textvalue(m.get('en_US','')))
 symbols.update(msgids)
 (O/'messages.json').write_text(json.dumps(messages,ensure_ascii=False))
 (O/'map-headers.json').write_text(json.dumps(headers,indent=2))
 generated=['/* Generated from original Platinum input; do not hand-edit. */','#include "platinum.h"']
 graphics=json.loads((ROOT/'provenance/platinum-graphics.json').read_text())['npc_graphics']
 generated.append('const uint16_t gPtCounterpartGfx[2]={%u,%u};'%(graphics['OBJ_EVENT_GFX_PLAYER_F']['index'],graphics['OBJ_EVENT_GFX_PLAYER_M']['index']))
 generated.append('const uint16_t gPtRivalGfx=%u;'%graphics['OBJ_EVENT_GFX_BARRY']['index'])
 # UTF8 is retained in private data; runtime transliterates only for atlas display.
 generated+=['const char *const gPtMessages[]={'+','.join(cstr(t) for t in messages)+'};','const unsigned gPtMessageCount=sizeof(gPtMessages)/sizeof(*gPtMessages);']
 unknown=collections.Counter();maprows=[];native_counts={};model_manifest=[]
 def emitcode(name,ir,known,external=False):
  rows=[]
  for ins in ir['instructions']:
   vals=[];unresolved=0
   for arg in ins['args']:
    try:vals.append(integer(arg,{**known,**ir['labels']}))
    except ValueError:vals.append(0);unresolved=1
   if len(vals)>6:unresolved=1
   if unresolved:unknown[ins['op']]+=1
   rows.append('{%s,{%s},%u,%u,%u}'%(cstr(ins['op']),','.join(map(str,vals[:6])) or '0',ins['line'],len(vals),unresolved))
  generated.append(('const ' if external else 'static const ')+'PtInstruction '+name+'[]={'+','.join(rows)+'};')
 for i,name in enumerate(START):
  h=headers['MAP_HEADER_'+name.upper()];hid=symbols['MAP_HEADER_'+name.upper()];matrix=json.loads((S/'res/field/matrices'/(h['mapMatrixID']+'.json')).read_text());cells=matrix['maps'];terrain=[]
  coords=[]
  for z,row in enumerate(cells):
   for x,land in enumerate(row):
    if not matrix['headers'] or matrix['headers'][z][x]=='MAP_HEADER_'+name.upper():coords.append((x,z,land))
  minx=min(c[0] for c in coords)*32;minz=min(c[1] for c in coords)*32
  width=(max(c[0] for c in coords)+1)*32-minx;height=(max(c[1] for c in coords)+1)*32-minz
  terrain=[65535]*(width*height)
  area=json.loads((S/'res/field/area_data'/(h['areaDataArchiveID']+'.json')).read_text())
  for x,z,land in coords:
   lid=int(land.split('_')[1]);p=S/'res/field/maps/data'/f'map_data_{lid:03d}.bin';raw=p.read_bytes();a,b,c,d=struct.unpack_from('<4I',raw);assert 16+a+b+c+d==len(raw) and a==2048
   attrs=struct.unpack_from('<1024H',raw,16)
   for iz in range(32):
    at=(z*32-minz+iz)*width+x*32-minx;terrain[at:at+32]=attrs[iz*32:iz*32+32]
   model_manifest.append({'header':hid,'name':name,'land':lid,'cell':[x,z],'origin':[minx,minz],'area':h['areaDataArchiveID'],'texture':area['mapTextureSet'],'props':area['mapPropSet']})
  known=dict(symbols);events=json.loads((S/'res/field/events'/(h['eventsArchiveID']+'.json')).read_text())
  objects=[]
  for n,obj in enumerate(events['object_events']):known[obj['id']]=n
  for n,obj in enumerate(events['object_events']):
   def val(v):
    try:return integer(str(v),known)
    except ValueError:return 0
   graphics=json.loads((ROOT/'provenance/platinum-graphics.json').read_text())['npc_graphics'].get(obj['graphics_id'],{})
   objects.append('{%u,%u,%u,%u,%u,%u,%u}'%(n,graphics.get('index',65535),val(obj['hidden_flag']),val(obj['script']),obj['x'],obj['z'],val(obj['initial_dir'])))
  ir=scripts[h['scriptsArchiveID']];emitcode('ptcode'+str(i),ir,known)
  entries=[ir['labels'][e] for e in ir['entries']]
  generated.append('static const uint32_t ptentries%d[]={%s};'%(i,','.join(map(str,entries))))
  native_counts[name]=len(ir['instructions'])
  generated.append('static const uint16_t ptterrain%d[]={%s};'%(i,','.join(map(str,terrain))))
  def array(typ,suffix,rows):generated.append('static const '+typ+' pt'+suffix+str(i)+'[]={'+(','.join(rows) if rows else '{0}')+'};')
  array('PtObject','obj',objects)
  warps=['{%u,%u,%u,%u}'%(w['x'],w['z'],symbols[w['dest_header_id']],w['dest_warp_id']) for w in events['warp_events']];array('PtWarp','warp',warps)
  triggers=['{%u,%u,%u,%u,%u,%u,%u}'%(w['script'],w['x'],w['z'],w['width'],w['length'],known[w['var']],w['value']) for w in events['coord_events']];array('PtTrigger','trigger',triggers)
  bgs=['{%u,%u,%u,%u}'%(w['script'],w['x'],w['z'],0) for w in events['bg_events']];array('PtBackground','bg',bgs)
  initpath=S/'res/field/scripts'/(h['initScriptsArchiveID']+'.s');init=initpath.read_text();transition=re.search(r'InitScriptEntry_OnTransition (\d+)',init);transition=int(transition[1]) if transition else 0
  onload=re.search(r'InitScriptEntry_OnLoad (\d+)',init);onload=int(onload[1]) if onload else 0
  frames=[]
  for var,value,entry in re.findall(r'InitScriptGoToIfEqual (\w+), (\d+), (\d+)',init):frames.append('{%u,%u,%u}'%(known[var],int(value),int(entry)))
  array('PtFrame','frame',frames)
  maprows.append('{%u,%u,%u,%u,%u,%u,%u,%s,%s,ptcode%u,%u,ptentries%u,%u,%u,%u,ptobj%u,%u,ptwarp%u,%u,pttrigger%u,%u,ptbg%u,%u,ptframe%u,%u,ptterrain%u}'%(hid,int(h['mapMatrixID'].split('_')[-1]),int(h['areaDataArchiveID'].split('_')[-1]),minx,minz,width,height,cstr(name),cstr(ir['file']),i,len(ir['instructions']),i,len(entries),transition,onload,i,len(objects),i,len(warps),i,len(triggers),i,len(bgs),i,len(frames),i))
 teams=[];trainers=[]
 for path in sorted((S/'res/trainers/data').glob('*.json')):
  data=json.loads(path.read_text());key='TRAINER_'+path.stem.upper()
  if key not in symbols:continue
  offset=len(teams)
  for mon in data.get('party',[]):
   moves=[symbols.get(x,0) for x in mon['moves']] if mon.get('moves') else [0]*4
   teams.append('{%u,%u,%u,%u,{%s}}'%(symbols[mon['species']],mon['level'],mon.get('iv_scale',0),mon.get('form',0),','.join(map(str,moves))))
  trainers.append('{%u,%u,%u,%u,%s}'%(symbols[key],offset,len(data.get('party',[])),int(data.get('double_battle',False)),cstr(data.get('name',path.stem))))
 growth=[0]*494
 for path in sorted((S/'res/pokemon').glob('*/data.json')):
  data=json.loads(path.read_text());dex=symbols.get('SPECIES_'+path.parent.name.upper(),0)
  if 0<dex<=493:growth[dex]=symbols[data['exp_rate']]
  raw=path.read_bytes();rel=str(path.relative_to(S));manifest[rel]=sha(raw);dest=O/'original'/rel;dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(raw)
 growpath=S/'res/prebuilt/poketool/personal/growtbl.narc';raw=growpath.read_bytes();manifest[str(growpath.relative_to(S))]=sha(raw);base=raw.index(b'GMIF')+8;tables=[]
 for index in range(8):
  start,end=struct.unpack_from('<II',raw,28+8*index);table=struct.unpack_from('<101I',raw,base+start);assert end-start==404;tables.append('{'+','.join(map(str,table))+'}')
 dest=O/'original'/growpath.relative_to(S);dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(raw)
 generated+=['const uint8_t gPtGrowthRates[494]={'+','.join(map(str,growth))+'};','const uint32_t gPtExperience[8][101]={'+','.join(tables)+'};']
 generated+=['const PtTrainerMon gPtTrainerMons[]={'+','.join(teams)+'};','const PtTrainer gPtTrainers[]={'+','.join(trainers)+'};','const unsigned gPtTrainerCount=sizeof(gPtTrainers)/sizeof(*gPtTrainers);']
 generated+=['const PtMap gPtMaps[]={'+','.join(maprows)+'};','const unsigned gPtMapCount=sizeof(gPtMaps)/sizeof(*gPtMaps);']
 new=scripts['scripts_init_new_game'];emitcode('gPtNewGame',new,symbols,True);generated.append('const unsigned gPtNewGameCount=sizeof(gPtNewGame)/sizeof(*gPtNewGame);')
 intro=['HelloThere','MyNameRowan','WidelyInhabited','HavePokeBall','LiveAlongsidePokemon','AboutYourself','GenderDialogue','NameDialogue','SoYoure','RivalNameDialogue','ConfirmRivalName','EndDialogue']
 helpids=['ControlInfo0','ControlInfo1','ControlInfo2','ControlInfo3','ControlInfoUnderstood','ControlInfoUseTouchscreen','InfoAnythingElse','AdventureInfo0','AdventureInfo1','AdventureInfo2','AdventureInfo3','AdventureInfo4','AdventureInfo5']
 generated.append('const uint16_t gPtIntroHelp[]={'+','.join(str(msgids['RowanIntro_Text_'+n]) for n in helpids)+'};')
 generated+=['const uint16_t gPtIntroMain[]={'+','.join(str(msgids['RowanIntro_Text_'+n]) for n in intro)+'};','const unsigned gPtIntroCount=sizeof(gPtIntroMain)/sizeof(*gPtIntroMain);']
 (ROOT/'source/imported/private/platinum_data.c').write_text('\n'.join(generated)+'\n')
 (O/'models.json').write_text(json.dumps(model_manifest,indent=2))
 report={'source_root':str(S),'original_input_sha256':manifest,'raw_file_counts':dict(counts),'script_files_losslessly_ingested':len(scripts),'script_command_frequency':dict(ops),'native_initial_map_headers':len(maprows),'native_compiled_script_instructions':native_counts,'unresolved_native_instruction_operands':dict(unknown),'full_original_campaign_playable':False,'content_reauthored':False,'notes':['Lossless ingestion is not VM execution support.','All unsupported commands stop rather than silently completing events.','Original models, props, terrain attributes and height collision are retained; runtime geometry support is reported separately.']}
 (ROOT/'provenance/platinum-import.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k not in ['original_input_sha256','script_command_frequency']},indent=2))
if __name__=='__main__':main()
