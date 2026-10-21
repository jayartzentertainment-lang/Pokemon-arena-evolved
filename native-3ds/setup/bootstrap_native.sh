#!/usr/bin/env bash
set -euo pipefail
task_native_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
task_native_cache="${ARENA_3DS_SETUP_ROOT:-/workspace/native-3ds-setup}"
task_native_image='devkitpro/devkitarm@sha256:116afba8df8453961de2936ffab20dd441edf4d682856c1ec8b0e53d7ed0bbf5'
for task_native_command in docker python3 cc; do
    command -v "$task_native_command" >/dev/null || {
        echo "Required command missing: $task_native_command" >&2; exit 1;
    }
done
docker info >/dev/null
if ! docker image inspect "$task_native_image" >/dev/null 2>&1; then
    docker pull "$task_native_image"
fi
mkdir -p "$task_native_cache"
if ! test -x "$task_native_cache/venv/bin/python"; then
    python3 -m venv --system-site-packages "$task_native_cache/venv"
fi
"$task_native_cache/venv/bin/python" -m pip install --disable-pip-version-check 'Pillow==12.3.0'
"$task_native_cache/venv/bin/python" "$task_native_root/tools/setup_emulator.py" --root "$task_native_cache/emulator"
echo "Native 3DS toolchain and emulator ready; activate: source $task_native_cache/venv/bin/activate"
