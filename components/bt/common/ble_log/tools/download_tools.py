# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Download and launch the stable BLE Log host tools from a configured ESP-IDF project."""

import argparse
import hashlib
import json
import os
import platform
import re
import subprocess
import sys
import tempfile
import time
from http.client import HTTPException
from pathlib import Path
from typing import IO
from urllib.error import HTTPError
from urllib.request import urlopen

REPOSITORY = 'espressif/esp-ble-tools'
ASSISTANT = 'ble_log_config_assistant'
CONSOLE = 'ble_log_console'


def host_suffix() -> tuple[str, str]:
    if platform.machine().lower() in ('amd64', 'x86_64'):
        if sys.platform == 'win32':
            return 'windows', '.exe'
        if sys.platform.startswith('linux'):
            return 'ubuntu', ''
    raise ValueError('BLE Log host binaries are available for Windows x64 and Ubuntu x86-64.')


def stable_asset(tool: str, system: str, suffix: str) -> tuple[str, str, int, str]:
    tag = tool + '_stable'
    name_pattern = re.compile(rf'{tool}_{system}_v[0-9]+(?:\.[0-9]+)+{re.escape(suffix)}')
    try:
        with urlopen(f'https://api.github.com/repos/{REPOSITORY}/releases/tags/{tag}', timeout=20) as response:
            assets = [(a['name'], a.get('digest') or '', a.get('size', 0)) for a in json.load(response)['assets']]
    except HTTPError as error:
        if error.code not in (403, 429):
            raise
        # Anonymous GitHub API requests share a rate limit behind a corporate proxy.
        with urlopen(f'https://github.com/{REPOSITORY}/releases/expanded_assets/{tag}', timeout=20) as response:
            html = response.read().decode()
        assets = []
        prefix = f'/{REPOSITORY}/releases/download/{tag}/'
        for row in html.split('</li>'):
            name_match = re.search(rf'href="{re.escape(prefix)}([^"/]+)"', row)
            digest_match = re.search(r'sha256:[0-9a-f]{64}', row)
            if name_match and digest_match:
                assets.append((name_match[1], digest_match[0], 0))
    matches = [asset for asset in assets if name_pattern.fullmatch(asset[0])]
    if len(matches) != 1 or not re.fullmatch(r'sha256:[0-9a-f]{64}', matches[0][1]):
        raise ValueError(
            f'Cannot identify a verified {system} binary at https://github.com/{REPOSITORY}/releases/tag/{tag}'
        )
    name, digest, size = matches[0]
    version = name.removeprefix(f'{tool}_{system}_').removesuffix(suffix)
    return f'https://github.com/{REPOSITORY}/releases/download/{tag}/{name}', digest, size, version


def digest(path: Path) -> str:
    checksum = hashlib.sha256()
    with path.open('rb') as stream:
        while block := stream.read(1024 * 1024):
            checksum.update(block)
    return 'sha256:' + checksum.hexdigest()


def copy_with_progress(response: IO[bytes], output: IO[bytes], size: int) -> None:
    received = 0
    # idf.py reads Ninja output line by line. Terminate each update with a
    # newline, moving back one line when idf.py is forwarding to a terminal.
    redraw = (sys.stdout.isatty() or os.environ.get('CLICOLOR_FORCE') == '1') and not os.environ.get('NO_COLOR')

    def show(prefix: str) -> None:
        amount = f'{received / 2**20:.1f} MiB'
        if size:
            percent = min(100, received * 100 // size)
            filled = percent // 5
            amount = f'[{"#" * filled}{"-" * (20 - filled)}] {percent:3d}%  {amount} / {size / 2**20:.1f} MiB'
        print(prefix + amount, flush=True)

    show('')
    prefix = '\033[1A\r\033[2K' if redraw else ''
    last_update = time.monotonic()
    while block := response.read(64 * 1024):
        output.write(block)
        received += len(block)
        now = time.monotonic()
        if now - last_update >= 0.5 and received != size:
            show(prefix)
            last_update = now
    show(prefix)


def installed_tool(tool: str) -> tuple[Path, str]:
    system, suffix = host_suffix()
    root = Path.cwd()
    executable = root / (tool + suffix)
    print(f'Checking stable release: {tool}', flush=True)
    url, expected, size, version = stable_asset(tool, system, suffix)
    if executable.is_file() and digest(executable) == expected:
        return executable, version
    print(f'Downloading {url.rsplit("/", 1)[-1]} to {root}', flush=True)
    output = tempfile.NamedTemporaryFile(dir=root, delete=False)
    temporary = Path(output.name)
    try:
        with output, urlopen(url, timeout=60) as response:
            copy_with_progress(response, output.file, size or int(response.headers.get('Content-Length') or 0))
        if digest(temporary) != expected:
            raise ValueError('Download failed SHA-256 verification. Run the command again to retry.')
        temporary.chmod(0o755)
        os.replace(temporary, executable)
    finally:
        temporary.unlink(missing_ok=True)
    return executable, version


def main(tool: str) -> int:
    path, version = installed_tool(tool)
    print(f'Starting {tool.replace("_", "-")} {version}: {path}', flush=True)
    command = [str(path)]
    if tool == ASSISTANT:
        command.append(str(Path.cwd()))
    # idf.py may pipe Ninja output for build hints. Give the TUI the terminal.
    if sys.stdin.isatty() and not sys.stdout.isatty():
        with open('CONOUT$' if os.name == 'nt' else '/dev/tty', 'wb', buffering=0) as terminal:
            return subprocess.call(command, stdout=terminal, stderr=terminal)
    return subprocess.call(command)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('tool', choices=['ble-log-config-assistant', 'ble-log-console'])
    args = parser.parse_args()
    try:
        sys.exit(main(args.tool.replace('-', '_')))
    except (OSError, HTTPException, ValueError, KeyError) as exc:
        sys.exit(f'{args.tool}: {exc}')
    except KeyboardInterrupt:
        sys.exit(130)
