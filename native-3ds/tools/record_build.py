#!/usr/bin/env python3
from pathlib import Path
import hashlib
import json
import struct

ROOT=Path(__file__).resolve().parents[1]
name='Platinum-Arena-3DS-Prototype'
app=ROOT/(name+'.3dsx');elf=ROOT/(name+'.elf')
raw=app.read_bytes();machine=struct.unpack_from('<H',elf.read_bytes(),18)[0]
assert raw[:4]==b'3DSX' and len(raw)>64 and machine==40
inputs={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest()
        for folder in ['source','include','gfx'] for p in (ROOT/folder).rglob('*')
        if p.is_file() and 'private' not in p.parts}
inputs['Makefile']=hashlib.sha256((ROOT/'Makefile').read_bytes()).hexdigest()
textures={p.name:p.stat().st_size for p in (ROOT/'romfs/gfx').glob('*.t3x')}
size=(ROOT/'validation/native-size.txt').read_text()
report={'format':'Native3DS homebrew3DSX, ARM11 ELF machine40','bytes':len(raw),
        'version':'0.4.0-integration-development-motion-contact',
        'sha256':hashlib.sha256(raw).hexdigest(),'compiler':'devkitARM GCC16.1.0',
        'toolchain_image':'devkitpro/devkitarm@sha256:116afba8df8453961de2936ffab20dd441edf4d682856c1ec8b0e53d7ed0bbf5',
        'link_size':size.strip(),'textures':textures,'inputs_sha256':inputs,
        'private_generated_inputs_sha256':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in (ROOT/'source/imported/private').glob('*.c')},
        'texture_sha256':{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in (ROOT/'romfs/gfx').glob('*.t3x')},
        'emulator_verified':False,'physical_hardware_profiled':False}
(ROOT/'validation/build.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps({k:report[k] for k in ['format','bytes','sha256','link_size']},indent=2))
