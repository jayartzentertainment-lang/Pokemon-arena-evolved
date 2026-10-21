#!/usr/bin/env python3
"""Download and verify the official Azahar Linux AppImage, without root/FUSE."""
import argparse
import hashlib
from pathlib import Path
import subprocess
import urllib.request

VERSION='2126.2'
SHA256='af037f8b775198f636b14bcfe63fb401b02246ebfefe1025bb905ce042556960'
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--root',type=Path,required=True)
args=parser.parse_args();args.root=args.root.resolve();args.root.mkdir(parents=True,exist_ok=True)
app=args.root/('azahar-'+VERSION+'.AppImage')
if not app.exists():
    url='https://github.com/azahar-emu/azahar/releases/download/'+VERSION+'/azahar.AppImage'
    request=urllib.request.Request(url,headers={'User-Agent':'Platinum-Arena-native-setup'})
    temporary=app.with_suffix('.download')
    with urllib.request.urlopen(request,timeout=60) as source,temporary.open('wb') as out:
        while chunk:=source.read(1024*1024):out.write(chunk)
    temporary.replace(app)
assert hashlib.sha256(app.read_bytes()).hexdigest()==SHA256,'Azahar download checksum mismatch'
app.chmod(0o700)
if not (args.root/'squashfs-root/AppRun').is_file():
    subprocess.run([str(app),'--appimage-extract'],cwd=args.root,check=True,stdout=subprocess.DEVNULL)
print('Verified official Azahar '+VERSION+' SHA256 '+SHA256)
