#!/usr/bin/env bash
set -euo pipefail
task_project_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$task_project_root"
task_build_image='devkitpro/devkitarm@sha256:116afba8df8453961de2936ffab20dd441edf4d682856c1ec8b0e53d7ed0bbf5'
docker run --rm -v "$task_project_root:/project" -w /project "$task_build_image" \
    bash -c 'set -e; make -j2; mkdir -p validation; /opt/devkitpro/devkitARM/bin/arm-none-eabi-size Platinum-Arena-3DS-Prototype.elf > validation/native-size.txt'
python3 tools/record_build.py
