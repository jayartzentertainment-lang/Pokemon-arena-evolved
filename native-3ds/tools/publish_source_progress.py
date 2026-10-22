#!/usr/bin/env python3
"""Publish an explicitly unfinished source snapshot on its own GitHub branch.
Uses a temporary Git index, preserves main and all prior release branches, and
never uploads a playable executable. GH_TOKEN is read from the environment.
"""
from pathlib import Path
import argparse,hashlib,json,os,subprocess,tempfile,urllib.request,time,sys
R=Path(__file__).resolve().parents[1]
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--packages',type=Path,required=True);p.add_argument('--git-dir',type=Path,required=True);p.add_argument('--branch',default='codex/platinum-original-integration-wip');p.add_argument('--parent',default='72940b400db5fe577f5566fa8d70eec6f87a4fa4');a=p.parse_args()
 if not os.environ.get('GH_TOKEN'):sys.exit('GH_TOKEN is required to publish this requested source snapshot.')
 receiptfile=a.packages/'github-source-receipt.json'
 if receiptfile.exists():sys.exit('A publication receipt exists; verify it before attempting another push.')
 pack=json.loads((a.packages/'package-integrity.json').read_text());repo='jayartzentertainment-lang/Pokemon-arena-evolved';prefix='progress/native-3ds-original-integration/';files=pack['source_files'];names=list(pack['packages'])
 assert len(names)==2 and all(n.endswith('.zip') for n in names)
 assert set(names)==set(pack['verified_packages']),'Both packages must be verified before publication'
 with tempfile.TemporaryDirectory(prefix='platinum-source-progress-',dir='/tmp') as temp:
  t=Path(temp);ask=t/'askpass.py';ask.write_text('#!/usr/bin/env python3\nimport os,sys\nprint("x-access-token" if "username" in sys.argv[1].lower() else os.environ.get("GH_TOKEN",""))\n');ask.chmod(0o700)
  (t/'worktree').mkdir()
  env=os.environ.copy();env.update(GIT_WORK_TREE=str(t/'worktree'),GIT_ASKPASS=str(ask),GIT_TERMINAL_PROMPT='0',GIT_INDEX_FILE=str(t/'index'),GIT_AUTHOR_NAME='Codex',GIT_AUTHOR_EMAIL='codex@openai.com',GIT_COMMITTER_NAME='Codex',GIT_COMMITTER_EMAIL='codex@openai.com')
  def git(*args,data=None):
   q=subprocess.run(['git','--git-dir='+str(a.git_dir),'-c','credential.helper=',*args],input=data,env=env,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
   if q.returncode:raise RuntimeError(q.stderr.decode(errors='replace')[:1500])
   return q.stdout
  refs_before=git('ls-remote','--heads','--tags','https://github.com/'+repo+'.git')
  existing=git('ls-remote','https://github.com/'+repo+'.git','refs/heads/'+a.branch)
  if existing:sys.exit('Requested WIP branch already exists; do not overwrite another publication.')
  git('read-tree',a.parent)
  old=git('ls-files','-z','native-3ds/')
  if old:git('update-index','--force-remove','-z','--stdin',data=old)
  for name in files:
   assert 'private' not in Path(name).parts
   blob=git('hash-object','-w',str(R/name)).decode().strip();git('update-index','--add','--cacheinfo','100644,'+blob+',native-3ds/'+name)
  for name in names:
   path=a.packages/name;assert hashlib.sha256(path.read_bytes()).hexdigest()==pack['packages'][name]['sha256']
   blob=git('hash-object','-w',str(path)).decode().strip();git('update-index','--add','--cacheinfo','100644,'+blob+','+prefix+name)
  tree=git('write-tree').decode().strip();message=b'Publish native Platinum integration source progress and corrected trainer bases [skip ci]\n\nWork in progress, not a playable full-story release. Blank supplied Gen IV Custom base; fixed Red/Brendan silhouettes with palette-only controls; v1 save migration. Original field UI intake and combat/import work remain available for review. Private editable inputs are encrypted; no ROM or executable is added. Preserve main and approved releases.\n'
  commit=git('commit-tree',tree,'-p',a.parent,data=message).decode().strip();git('update-ref','refs/heads/'+a.branch,commit,'0'*40)
  git('push','https://github.com/'+repo+'.git','refs/heads/'+a.branch+':refs/heads/'+a.branch)
  refs_after=git('ls-remote','--heads','--tags','https://github.com/'+repo+'.git')
  unchanged_before=set(refs_before.decode().splitlines());unchanged_after={line for line in refs_after.decode().splitlines() if not line.endswith('refs/heads/'+a.branch)}
  receipt={'status':'PUBLISHED UNFINISHED SOURCE PROGRESS; no ROM/executable release','commit':commit,'branch':a.branch,'parent':a.parent,'source_url':'https://github.com/'+repo+'/tree/'+a.branch+'/native-3ds','main_and_prior_release_refs_unchanged':unchanged_before==unchanged_after,'verified':[]}
  receiptfile.write_text(json.dumps(receipt,indent=2)+'\n')
  assert receipt['main_and_prior_release_refs_unchanged'],'Another remote ref changed during publication; inspect before reporting preservation'
  # Persist the receipt immediately after push, before potentially slow HTTP checks.
  checks=[(R/'README.md','native-3ds/README.md'),*[(a.packages/n,prefix+n) for n in names]]
  for local,remote in checks:
   url='https://raw.githubusercontent.com/'+repo+'/'+commit+'/'+remote
   for attempt in range(5):
    try:
     with urllib.request.urlopen(url,timeout=45) as response:raw=response.read();status=response.status
     break
    except Exception:
     if attempt==4:raise
     time.sleep(attempt+1)
   digest=hashlib.sha256(raw).hexdigest();assert status==200 and digest==hashlib.sha256(local.read_bytes()).hexdigest()
   receipt['verified'].append({'url':url,'http_status':status,'bytes':len(raw),'sha256':digest});receiptfile.write_text(json.dumps(receipt,indent=2)+'\n')
  receipt['anonymous_source_and_package_downloads_verified']=True;receiptfile.write_text(json.dumps(receipt,indent=2)+'\n');print(json.dumps(receipt,indent=2))
if __name__=='__main__':main()
