# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Build a single-file executable for BLE Log Console using PyInstaller.

Usage:
    pip install pyinstaller
    python build_exe.py
"""
import subprocess
import sys


def main() -> None:
    cmd = [
        sys.executable,
        '-m',
        'PyInstaller',
        'console.py',
        '--onefile',
        '--name',
        'ble_log_console',
        '--hidden-import',
        'textual',
        '--hidden-import',
        'textual.drivers',
        '--hidden-import',
        'textual.css',
        '--hidden-import',
        'textual_fspicker',
        '--hidden-import',
        'serial',
        '--hidden-import',
        'serial.tools',
        '--hidden-import',
        'serial.tools.list_ports',
        '--hidden-import',
        'serial.tools.list_ports_common',
        '--hidden-import',
        'serial.tools.list_ports_linux',
        '--hidden-import',
        'serial.tools.list_ports_windows',
        '--hidden-import',
        'serial.tools.list_ports_osx',
        '--collect-data',
        'textual',
        '--collect-data',
        'textual_fspicker',
        '--noconfirm',
    ]

    print(f'Running: {" ".join(cmd)}')
    result = subprocess.run(cmd, check=False)
    if result.returncode != 0:
        print(f'\nBuild failed (exit code {result.returncode}).', file=sys.stderr)
        print('If you see hidden import errors, add the missing module to the cmd list above.', file=sys.stderr)
        sys.exit(result.returncode)

    print('\nBuild complete. Executable: dist/ble_log_console')


if __name__ == '__main__':
    main()
