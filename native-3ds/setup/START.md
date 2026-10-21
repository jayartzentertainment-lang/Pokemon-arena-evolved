# Starting native 3DS combat quality0.2.0 in the retained cloud workspace

Active development: `/workspace/platinum-arena-3ds-next`, branch
`codex/native-3ds-quality-7156caccf621`. The user-approved0.1.0 source remains
under `/workspace/platinum-arena-3ds`; keep that baseline/release intact.
The latest source/build scope is in README.md and docs/VALIDATION.md.


Work in `/workspace/platinum-arena-3ds-next` (or the `native-3ds/` directory in the
published native development branch). Reuse the existing checkout; do not
create a Git worktree unless the user asks. The DS release is a separate project
under `/workspace/platinum-port/pokeplatinum`; do not clean/reset it or rebuild
it for a native-only task. Read `README.md`, `docs/PORTING.md`,
`docs/VALIDATION.md` and `ASSET-RIGHTS.md` before claiming coverage.

```sh
cd /workspace/platinum-arena-3ds-next
bash setup/bootstrap_native.sh
source /workspace/native-3ds-setup/venv/bin/activate
python tools/test_host.py --ds-source /workspace/platinum-port/pokeplatinum
bash tools/build_native.sh
```

The retained private atlas inputs must exist. On a fresh source checkout,
recover/convert the user-owned inputs using the README first. Do not download a
ROM or replace missing art with generic bubbles. Original 0.9.6 archive/pack
hashes are mandatory conversion checks. The devkitPro Docker cache may not
survive snapshots: bootstrap pulls the exact pinned digest if it is absent.
No credentials are needed for these public tool downloads.

## Emulator

Emulator/Xvfb processes do not survive snapshots. The official Azahar 2126.2
AppImage is checksum-verified and extracted under
`/workspace/native-3ds-setup/emulator/`. Use a hash-named immutable `.3dsx` copy
for a test session and never rebuild that file while it is open.

Set `System/is_new_3ds=false` in the isolated Azahar configuration to retain the
original-model compatibility baseline. The app also disables optional CPU
speedup and stereoscopic rendering. This does not substitute for testing real
original/New 3DS hardware.

With a desktop DISPLAY, launch:

```sh
DISPLAY=:93 \
XDG_CONFIG_HOME=/workspace/native-3ds-setup/emulator/config \
XDG_DATA_HOME=/workspace/native-3ds-setup/emulator/data \
XDG_CACHE_HOME=/workspace/native-3ds-setup/emulator/cache \
LIBGL_ALWAYS_SOFTWARE=1 SDL_AUDIODRIVER=dummy \
/workspace/native-3ds-setup/emulator/squashfs-root/AppRun -w \
  /absolute/path/to/immutable-candidate.3dsx
```

If no desktop exists, the retained Xvfb can be started on an unused display:

```sh
LD_LIBRARY_PATH=/workspace/.tools/emulator/root/usr/lib/x86_64-linux-gnu \
/workspace/.tools/emulator/root/usr/bin/Xvfb :93 -screen 0 1024x768x24 -nolisten tcp
```

Without a window manager, map/focus the new main window using the retained
xdotool binary and its matching library path. Read the new process/window IDs;
do not reuse IDs from old sessions or kill unrelated emulators. Hold simulated
buttons/touches at least 150 ms so libctru's scan sees the input. A black Xvfb
capture of an unmapped Qt window is not evidence of a broken application.

Native session telemetry:
`emulator/data/azahar-emu/sdmc/3ds/platinum-arena/session.log`.
Azahar log: `emulator/data/azahar-emu/log/azahar_log.txt`.
CPU/GPU times and FPS in the native HUD use emulated time; also inspect Azahar's
host speed/FPS. Software OpenGL and cloud contention can reduce host speed.
No audio listening test is possible with the current dummy audio sink.

The environment configuration saved through the onboarding tool is a draft for
review, not a published environment or proof of fresh-task restoration.
