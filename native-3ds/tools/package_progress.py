#!/usr/bin/env python3
"""Package reviewable WIP source and encrypted editable inputs, never a game ROM.
Requires pyzipper for AES. The password file is local and never archived.
"""
from pathlib import Path
import argparse,hashlib,json,sys,zipfile
R=Path(__file__).resolve().parents[1]
EXCLUDED={'USER-MASTER-DIRECTIVE.md','WORK-IN-PROGRESS.md','NEXT-MILESTONE-DIRECTIVE.md'}
BINARY={'.png','.gif','.t3x','.pyc','.nds','.gba','.3dsx','.smdh','.elf','.so','.zip','.7z','.rar'}
def source_files():
 result=[]
 for folder in ['source','include','vendor','tools','tests','setup','provenance','assets','gfx','docs','content']:
  for p in (R/folder).rglob('*'):
   rel=p.relative_to(R)
   if p.is_file() and not p.is_symlink() and 'private' not in rel.parts and '__pycache__' not in rel.parts and p.name not in EXCLUDED and p.suffix not in BINARY:result.append(p)
 return sorted(set(result+[R/n for n in ['README.md','ASSET-RIGHTS.md','Makefile','.gitignore']]))
def inputs():
 result=set()
 for recipe in (R/'gfx').glob('*.t3s'):
  for line in recipe.read_text().splitlines():
   if line.startswith('private/'):
    p=(R/'gfx'/line).resolve();assert p.is_relative_to(R/'gfx/private') and p.is_file();result.add(p)
 for folder in ['source/imported/private','assets/private/platinum','assets/private/spritecollab','assets/private/trainer-bases']:
  result.update(p for p in (R/folder).rglob('*') if p.is_file() and not p.is_symlink() and '__pycache__' not in p.parts)
 assert not any(p.suffix in {'.nds','.gba','.3dsx','.elf','.smdh'} for p in result)
 return sorted(result)
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,required=True);p.add_argument('--password-file',type=Path,required=True);p.add_argument('--only',choices=['all','source','private'],default='all',help='Package private inputs first, then finalize source/reports without recompressing unchanged inputs');a=p.parse_args()
 try:import pyzipper
 except ImportError:sys.exit('Install pyzipper to produce the encrypted private-assets companion.')
 # WinZip AES uses PBKDF2-HMAC-SHA1. Use the identical OpenSSL-backed
 # stdlib implementation to avoid Python HMAC overhead for many small cels.
 import pyzipper.zipfile_aes as aes
 original_kdf=aes.PBKDF2
 def aes_kdf(password,salt,dkLen,count):
  return hashlib.pbkdf2_hmac('sha1',password,salt,count,dkLen)
 assert aes_kdf(b'compatibility-check',b'0123456789abcdef',66,1000)==original_kdf(b'compatibility-check',b'0123456789abcdef',dkLen=66,count=1000)
 aes.PBKDF2=aes_kdf
 a.output.mkdir(parents=True,exist_ok=True);sources=source_files();private=inputs();password=json.loads(a.password_file.read_text())['password'];paths=[];verified=[]
 previous=json.loads((a.output/'package-integrity.json').read_text()) if (a.output/'package-integrity.json').exists() else {}
 for name,files,encrypted in [('Platinum-Arena-3DS-WIP-SOURCE.zip',sources,False),('Platinum-Arena-3DS-WIP-PRIVATE-ASSETS.zip',private,True)]:
  path=a.output/name;paths.append(path)
  if (a.only=='source' and encrypted) or (a.only=='private' and not encrypted):
   if path.exists() and name in previous.get('verified_packages',[]) and previous['packages'][name]['sha256']==hashlib.sha256(path.read_bytes()).hexdigest():verified.append(name)
   continue
  z=pyzipper.AESZipFile(path,'w',compression=zipfile.ZIP_DEFLATED,encryption=pyzipper.WZ_AES) if encrypted else zipfile.ZipFile(path,'w',compression=zipfile.ZIP_DEFLATED)
  if encrypted:z.setpassword(password.encode());z.setencryption(pyzipper.WZ_AES,nbits=256)
  sums=[]
  with z:
   for item in files:
    dest='native-3ds/'+str(item.relative_to(R));raw=item.read_bytes();z.writestr(dest,raw);sums.append(hashlib.sha256(raw).hexdigest()+'  '+dest)
   z.writestr('SHA256SUMS','\n'.join(sums)+'\n')
   z.writestr('READ-ME-FIRST.txt','UNFINISHED SOURCE WORK, NOT A PLAYABLE GAME RELEASE.\nExtract both WIP archives into the same directory. Both create native-3ds/.\nUse the README and docs for setup, actual progress and remaining original-story work.\nNo game ROM or executable is included.\n')
  reader=pyzipper.AESZipFile(path) if encrypted else zipfile.ZipFile(path)
  with reader as check:
   if encrypted:check.setpassword(password.encode())
   # Reading each complete entry verifies its CRC/AES authentication too.
   for line in check.read('SHA256SUMS').decode().splitlines():
    digest,entry_name=line.split('  ',1);assert hashlib.sha256(check.read(entry_name)).hexdigest()==digest
  assert path.stat().st_size<100*1024*1024
  verified.append(name)
 paths=[path for path in paths if path.exists()]
 receipt={'status':'UNFINISHED SOURCE PROGRESS; no ROM/executable release','source_files':[str(p.relative_to(R)) for p in sources],'private_files':len(private),'verified_packages':verified,'packages':{p.name:{'bytes':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} for p in paths}}
 (a.output/'package-integrity.json').write_text(json.dumps(receipt,indent=2)+'\n');print(json.dumps({'source_files':len(sources),'private_files':len(private),'verified_packages':verified,'packages':receipt['packages'],'no_game_executable':True},indent=2))
if __name__=='__main__':main()
