#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# Build a single-file BLE Log Console executable via PyInstaller.
# The executable is placed in the caller's working directory.
# All intermediate build artifacts are cleaned up automatically.

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
IDF_PATH="$(cd "$SCRIPT_DIR/../../.." && pwd)"
CALLER_DIR="$(pwd)"
export IDF_PATH

echo "Activating ESP-IDF environment ..."
# shellcheck source=/dev/null
. "$IDF_PATH/export.sh" > /dev/null 2>&1

echo "Installing build dependencies ..."
python -m pip install --quiet textual textual-fspicker pyinstaller

echo "Building executable ..."
cd "$SCRIPT_DIR"
python build_exe.py

# Move executable to caller's directory and clean up
EXE_NAME="ble_log_console"
if [ -f "dist/$EXE_NAME" ]; then
    mv "dist/$EXE_NAME" "$CALLER_DIR/$EXE_NAME"
    echo ""
    echo "Executable ready: $CALLER_DIR/$EXE_NAME"
else
    echo "ERROR: Build produced no executable." >&2
    exit 1
fi

# Remove intermediate artifacts
rm -rf "$SCRIPT_DIR/build" "$SCRIPT_DIR/dist" "$SCRIPT_DIR"/*.spec

cd "$CALLER_DIR"
