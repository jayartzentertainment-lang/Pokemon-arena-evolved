#!/usr/bin/env python3
"""Package and optionally publish the explicitly limited native 3DS preview.

Artwork stays in an AES protected playable download. Public source is updated
on a new branch only; main, the WIP snapshot and approved releases are preserved.
No archive password or GitHub credential is stored in the public source tree.
"""
from pathlib import Path
import argparse, hashlib, io, json, os, subprocess, tempfile, time, urllib.request
import sys, zipfile
from package_progress import source_files

ROOT=Path(__file__).resolve().parents[1]
REPO='jayartzentertainment-lang/Pokemon-arena-evolved'
PARENT='bbd1f5375c7b5e304534d32e2576e26ae66dc7ca'
FILENAME='Platinum-Arena-3DS-0.4.0-PREVIEW.zip'
REMOTE='previews/native-3ds-0.4.0/'+FILENAME

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--password-file',type=Path,required=True)
    parser.add_argument('--git-dir',type=Path)
    parser.add_argument('--publish',action='store_true')
    args=parser.parse_args()
    import pyzipper
    args.output.mkdir(parents=True,exist_ok=True)
    receiptpath=args.output/'delivery-receipt.json'
    if receiptpath.exists():
        sys.exit('A publication receipt exists; inspect it before another delivery.')
    build=json.loads((ROOT/'validation/build.json').read_text())
    password=json.loads(args.password_file.read_text())['password'].encode()
    app=ROOT/'Platinum-Arena-3DS-Prototype.3dsx'
    assert app.read_bytes()[:4]==b'3DSX'
    assert hashlib.sha256(app.read_bytes()).hexdigest()==build['sha256']
    for relative,digest in {**build['inputs_sha256'],**build['private_generated_inputs_sha256']}.items():
        assert hashlib.sha256((ROOT/relative).read_bytes()).hexdigest()==digest,relative
    observations=json.loads((ROOT/'docs/reports/play-preview.json').read_text())
    assert observations['candidate_sha256']==build['sha256']
    assert observations['native_1v1_observed'] and observations['native_2v2_observed']
    assert observations['full_original_campaign_playable'] is False
    files={'3ds/platinum-arena-preview/'+app.name:app.read_bytes(),
           '3ds/platinum-arena-preview/Platinum-Arena-3DS-Prototype.smdh':
               (ROOT/'Platinum-Arena-3DS-Prototype.smdh').read_bytes(),
           'READ-ME-FIRST.txt':(ROOT/'docs/PLAY-PREVIEW.md').read_bytes(),
           'reports/build.json':(ROOT/'validation/build.json').read_bytes(),
           'reports/play-preview.json':(ROOT/'docs/reports/play-preview.json').read_bytes(),
           'reports/import-completion.json':(ROOT/'docs/reports/import-completion.json').read_bytes()}
    sums='\n'.join(hashlib.sha256(data).hexdigest()+'  '+name for name,data in files.items())+'\n'
    files['SHA256SUMS.txt']=sums.encode()
    package=args.output/FILENAME
    with pyzipper.AESZipFile(package,'w',compression=pyzipper.ZIP_DEFLATED,
                            encryption=pyzipper.WZ_AES) as archive:
        archive.setpassword(password);archive.setencryption(pyzipper.WZ_AES,nbits=256)
        for name,data in files.items():archive.writestr(name,data)
    def verify(raw):
        with pyzipper.AESZipFile(io.BytesIO(raw)) as archive:
            archive.setpassword(password)
            assert set(archive.namelist())==set(files)
            for name,data in files.items():assert archive.read(name)==data,name
    verify(package.read_bytes())
    sources=source_files()
    token=os.environ.get('GH_TOKEN','').encode()
    for path in sources:
        data=path.read_bytes()
        assert password not in data and (not token or token not in data),path
    digest=hashlib.sha256(package.read_bytes()).hexdigest()
    metadata={'status':'Playable opening / Battle Lab preview; full campaign unfinished',
              'binary_sha256':build['sha256'],'package':FILENAME,
              'bytes':package.stat().st_size,'sha256':digest,
              'package_decrypted_and_verified':True,'full_original_campaign_playable':False}
    (args.output/'package-integrity.json').write_text(json.dumps(metadata,indent=2)+'\n')
    if not args.publish:
        print(json.dumps(metadata,indent=2));return
    assert token and args.git_dir,'Publishing needs GH_TOKEN and --git-dir'
    branch='codex/native-3ds-play-preview-'+build['sha256'][:12]
    with tempfile.TemporaryDirectory(prefix='platinum-play-preview-',dir='/tmp') as folder:
        temp=Path(folder);(temp/'tree').mkdir()
        ask=temp/'askpass.py'
        ask.write_text('#!/usr/bin/env python3\nimport os,sys\nprint("x-access-token" if "username" in sys.argv[1].lower() else os.environ["GH_TOKEN"])\n')
        ask.chmod(0o700)
        env=os.environ.copy();env.update(GIT_WORK_TREE=str(temp/'tree'),
            GIT_INDEX_FILE=str(temp/'index'),GIT_ASKPASS=str(ask),GIT_TERMINAL_PROMPT='0',
            GIT_AUTHOR_NAME='Codex',GIT_AUTHOR_EMAIL='codex@openai.com',
            GIT_COMMITTER_NAME='Codex',GIT_COMMITTER_EMAIL='codex@openai.com')
        def git(*command,data=None):
            result=subprocess.run(['git','--git-dir='+str(args.git_dir),'-c',
                'credential.helper=',*command],env=env,input=data,
                stdout=subprocess.PIPE,stderr=subprocess.PIPE)
            if result.returncode:raise RuntimeError(result.stderr.decode(errors='replace')[:1500])
            return result.stdout
        url='https://github.com/'+REPO+'.git'
        before=git('ls-remote','--heads','--tags',url)
        assert not git('ls-remote',url,'refs/heads/'+branch),'Preview branch already exists'
        git('read-tree',PARENT)
        old=git('ls-files','-z','native-3ds/')
        if old:git('update-index','--force-remove','-z','--stdin',data=old)
        for path in sources:
            name=str(path.relative_to(ROOT));assert 'private' not in path.relative_to(ROOT).parts
            blob=git('hash-object','-w',str(path)).decode().strip()
            git('update-index','--add','--cacheinfo','100644,'+blob+',native-3ds/'+name)
        blob=git('hash-object','-w',str(package.resolve())).decode().strip()
        git('update-index','--add','--cacheinfo','100644,'+blob+','+REMOTE)
        tree=git('write-tree').decode().strip()
        message=b'Publish native 3DS opening and Arena lab playable preview [skip ci]\n\nFull original Platinum campaign remains unfinished. Include Gen IV blank Custom and fixed Red/Brendan colour controls, Arena 1v1/2v2 and current opening import. Isolate preview saves from approved builds. Protect art-bearing download; expose reproducible source. Preserve main, earlier WIP snapshot and prior releases.\n'
        commit=git('commit-tree',tree,'-p',PARENT,data=message).decode().strip()
        git('update-ref','refs/heads/'+branch,commit,'0'*40)
        git('push',url,'refs/heads/'+branch+':refs/heads/'+branch)
        receipt={**metadata,'commit':commit,'branch':branch,'verified':[],
                 'source_url':'https://github.com/'+REPO+'/tree/'+branch+'/native-3ds',
                 'download_url':'https://raw.githubusercontent.com/'+REPO+'/'+commit+'/'+REMOTE}
        receiptpath.write_text(json.dumps(receipt,indent=2)+'\n')
        after=git('ls-remote','--heads','--tags',url)
        receipt['prior_remote_refs_preserved']=set(before.decode().splitlines())=={
            line for line in after.decode().splitlines() if not line.endswith('refs/heads/'+branch)}
        receiptpath.write_text(json.dumps(receipt,indent=2)+'\n')
        assert receipt['prior_remote_refs_preserved'],'Inspect remote refs before reporting preservation'
        for remote,expected in [(REMOTE,package.read_bytes()),('native-3ds/README.md',(ROOT/'README.md').read_bytes())]:
            rawurl='https://raw.githubusercontent.com/'+REPO+'/'+commit+'/'+remote
            for attempt in range(5):
                try:
                    with urllib.request.urlopen(rawurl,timeout=45) as response:
                        data=response.read();status=response.status
                    break
                except Exception:
                    if attempt==4:raise
                    time.sleep(attempt+1)
            assert status==200 and data==expected
            if remote==REMOTE:verify(data)
            receipt['verified'].append({'url':rawurl,'http_status':status,'bytes':len(data),
                                        'sha256':hashlib.sha256(data).hexdigest()})
            receiptpath.write_text(json.dumps(receipt,indent=2)+'\n')
        receipt['anonymous_download_and_decrypted_binary_verified']=True
        receiptpath.write_text(json.dumps(receipt,indent=2)+'\n')
        print(json.dumps(receipt,indent=2))

if __name__=='__main__':main()
