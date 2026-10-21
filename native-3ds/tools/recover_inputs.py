#!/usr/bin/env python3
"""Recover only the private inputs needed by the four-species atlas converter.

Reads the exact preserved 0.9.6 archive plus the canonical pret checkout.
Never extracts untrusted archive paths or changes either source project.
"""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import zipfile

ROOT=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--source-archive',type=Path,required=True)
parser.add_argument('--platinum-source',type=Path,required=True)
parser.add_argument('--out',type=Path,required=True)
args=parser.parse_args()
expected=json.loads((ROOT/'provenance/source-archive.json').read_text())
archive=args.source_archive.read_bytes()
assert hashlib.sha256(archive).hexdigest()==expected['sha256'],'Wrong 0.9.6 source archive'
manifest=json.loads((ROOT/'provenance/ds-096-source-manifest.json').read_text())
args.out.mkdir(parents=True,exist_ok=True)
with zipfile.ZipFile(args.source_archive) as source:
    for name in ['res/arena/arena.pack','res/arena/arena_bg0.bin',
                 'src/arena/effects_data.c','res/arena/battle-ui-preview.png']:
        member='added-files/'+name
        raw=source.read(member)
        assert hashlib.sha256(raw).hexdigest()==manifest['files_sha256'][member]
        target=args.out/name;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(raw)
for name in ['generated/moves.txt','generated/pokemon_types.txt']+[
        'res/pokemon/'+species+'/data.json' for species in ['pikachu','lucario','charizard','blastoise']]:
    source=args.platinum_source/name
    target=args.out/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(source,target)
print('Recovered verified private conversion inputs to '+str(args.out))
