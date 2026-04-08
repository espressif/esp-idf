#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# BLE Log Console launcher.
# Works from any directory: ./run.sh, or /full/path/to/run.sh
# All arguments are forwarded to console.py.

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
IDF_PATH="$(cd "$SCRIPT_DIR/../../.." && pwd)"
export IDF_PATH

echo "Activating ESP-IDF environment ..."
# shellcheck source=/dev/null
. "$IDF_PATH/export.sh" > /dev/null 2>&1

echo "Installing extra dependencies ..."
python -m pip install --quiet textual textual-fspicker

exec python "$SCRIPT_DIR/console.py" "$@"
