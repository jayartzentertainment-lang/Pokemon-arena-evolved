#!/usr/bin/env python3
"""Run the reused battle regressions and the real native game's adapter tests."""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess

ROOT=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--ds-source',type=Path,help='Optionally verify the preserved DS project too')
args=parser.parse_args()
OUT=ROOT/'validation/host';OUT.mkdir(parents=True,exist_ok=True)
flags=['-std=c11','-O1','-g','-Wall','-Wextra','-Werror',
       '-fsanitize=address,undefined','-fno-omit-frame-pointer',
       '-Iinclude','-Ivendor/platinum096/include']
cc=os.environ.get('CC','cc')
core=['source/core/combat.c','source/core/move_data.c','source/vendor/arena_moves.c',
      'source/vendor/arena_navigation.c','source/vendor/arena_physics.c']
extra=['source/game/game.c','source/game/aim.c','source/game/species_data.c']
objects={}

def run(name,command):
    process=subprocess.run(command,cwd=ROOT,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    (OUT/(name+'.log')).write_text(process.stdout)
    if process.returncode:print(process.stdout)
    process.check_returncode()
    return process.stdout

for source in core+extra:
    obj=OUT/(Path(source).stem+'.o');run('compile-'+obj.stem,[cc,*flags,'-c',source,'-o',str(obj)])
    objects[source]=str(obj)
results=[]
tests=sorted((ROOT/'vendor/platinum096/tests/arena').glob('test_*.c'))+sorted((ROOT/'tests').glob('test_*.c'))
for test in tests:
    target=OUT/test.stem
    linked=core+(extra if test.parent==ROOT/'tests' else [])
    run('link-'+test.stem,[cc,*flags,str(test),*[objects[s] for s in linked],'-lm','-o',str(target)])
    output=run(test.stem,[str(target)])
    results.append({'test':test.stem,'passed':True,'output':output.strip()})
    print(test.stem+': passed',flush=True)
preserved=None
if args.ds_source:
    record=json.loads((ROOT/'provenance/ds-preservation.json').read_text())
    for relative,digest in record.items():
        assert hashlib.sha256((args.ds_source/relative).read_bytes()).hexdigest()==digest
    preserved=True
report={'tests':len(results),'passed':len(results),'sanitizers':['address','undefined'],
        'ds_project_preserved':preserved,'results':results}
(ROOT/'validation/host-results.json').write_text(json.dumps(report,indent=2)+'\n')
print('All '+str(len(results))+' host checks passed.'+
      (' Preserved DS files unchanged.' if preserved else ' DS preservation not checked in this run.'))
