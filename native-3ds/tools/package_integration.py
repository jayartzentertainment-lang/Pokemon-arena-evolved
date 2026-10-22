#!/usr/bin/env python3
"""Package a verified development build and editable inputs for its owner.

Does not publish, include credentials, or describe the original campaign as done.
Reuses the authenticated earlier private input archive plus a checked input delta.
"""
from pathlib import Path
import argparse, hashlib, io, json, os, zipfile
from package_progress import source_files, inputs
R=Path(__file__).resolve().parents[1]
BASE_SHA='a305752b6d33f89537ca167ae32234c4cc8ef054b96b04f5b5220f72f0a19002'
def digest(data):return hashlib.sha256(data).hexdigest()
def plain_zip(entries):
    out=io.BytesIO()
    with zipfile.ZipFile(out,'w',zipfile.ZIP_DEFLATED) as z:
        for name,data in entries.items():z.writestr(name,data)
    return out.getvalue()
def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--baseline',type=Path,required=True)
    p.add_argument('--password-file',type=Path,required=True)
    p.add_argument('--output',type=Path,required=True)
    a=p.parse_args()
    import pyzipper
    import pyzipper.zipfile_aes as aes
    aes.PBKDF2=lambda password,salt,dkLen,count:hashlib.pbkdf2_hmac('sha1',password,salt,count,dkLen)
    password=json.loads(a.password_file.read_text())['password'].encode()
    baseline=a.baseline.read_bytes();assert digest(baseline)==BASE_SHA
    with pyzipper.AESZipFile(io.BytesIO(baseline)) as z:
        z.setpassword(password)
        old=dict((name,h) for h,name in (line.split('  ',1) for line in z.read('SHA256SUMS').decode().splitlines()))
    build=json.loads((R/'validation/build.json').read_text())
    report=json.loads((R/'docs/reports/combat-integration.json').read_text())
    assert report['candidate_sha256']==build['sha256']
    assert report['native_1v1_observed'] and report['native_2v2_observed']
    assert report['full_original_campaign_playable'] is False
    for path,h in {**build['inputs_sha256'],**build['private_generated_inputs_sha256']}.items():
        assert digest((R/path).read_bytes())==h,path
    app=R/'Platinum-Arena-3DS-Prototype.3dsx'
    assert app.read_bytes()[:4]==b'3DSX' and digest(app.read_bytes())==build['sha256']
    source={'native-3ds/'+str(p.relative_to(R)):p.read_bytes() for p in source_files()}
    token=os.environ.get('GH_TOKEN','').encode()
    for name,data in source.items():
        assert password not in data and (not token or token not in data),name
    private=sorted(set(inputs())|set(p for p in (R/'assets/private/dash-source').rglob('*') if p.is_file()))
    delta={}
    for path in private:
        name='native-3ds/'+str(path.relative_to(R));data=path.read_bytes()
        if old.get(name)!=digest(data):delta[name]=data
    delta['SHA256SUMS']=''.join(digest(data)+'  '+name+'\n' for name,data in delta.items()).encode()
    files={
        '3ds/platinum-arena-preview/'+app.name:app.read_bytes(),
        '3ds/platinum-arena-preview/'+app.with_suffix('.smdh').name:app.with_suffix('.smdh').read_bytes(),
        'READ-ME-FIRST.txt':(R/'docs/INTEGRATION-DOWNLOAD.md').read_bytes(),
        'EDITABLE-SOURCE.zip':plain_zip(source),
        'BASE-PRIVATE-INPUTS.zip':baseline,
        'NEW-PRIVATE-INPUTS.zip':plain_zip(delta),
        'COMBAT-INTEGRATION.json':json.dumps(report,indent=2).encode(),
    }
    for name in report.get('private_capture_files',[]):
        capture=R/'validation'/name
        assert capture.is_file() and capture.resolve().is_relative_to((R/'validation').resolve())
        files['verification/'+name]=capture.read_bytes()
    files['SHA256SUMS']=''.join(digest(data)+'  '+name+'\n' for name,data in files.items()).encode()
    a.output.mkdir(parents=True,exist_ok=True)
    target=a.output/'Platinum-Arena-3DS-INTEGRATION-DEVELOPMENT.zip'
    with pyzipper.AESZipFile(target,'w',compression=zipfile.ZIP_STORED,encryption=pyzipper.WZ_AES) as z:
        z.setpassword(password);z.setencryption(pyzipper.WZ_AES,nbits=256)
        for name,data in files.items():z.writestr(name,data)
    with pyzipper.AESZipFile(target) as z:
        z.setpassword(password)
        for name,data in files.items():assert z.read(name)==data,name
    assert target.stat().st_size<100*1024*1024
    result={'status':'Development integration; full original campaign incomplete',
            'candidate_sha256':build['sha256'],'package':target.name,
            'bytes':target.stat().st_size,'sha256':digest(target.read_bytes()),
            'source_files':len(source),'private_delta_files':len(delta)-1,
            'authenticated_archive_verified':True,'full_original_campaign_playable':False}
    (a.output/'integration-package.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result,indent=2))
if __name__=='__main__':main()
