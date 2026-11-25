# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Exercise first install, cache reuse and recovery without a network or a DUT."""

import hashlib
import io
import json
from email.message import Message
from pathlib import Path
from urllib.error import HTTPError
from urllib.response import addinfourl

import download_tools
import pytest


@pytest.mark.parametrize('tool', [download_tools.ASSISTANT, download_tools.CONSOLE])
@pytest.mark.parametrize(('system', 'suffix'), [('ubuntu', ''), ('windows', '.exe')])
def test_install_once_from_matching_stable(
    tool: str, system: str, suffix: str, tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    binary = b'fixture binary'
    digest = 'sha256:' + hashlib.sha256(binary).hexdigest()
    name = f'{tool}_{system}_v1.2.3{suffix}'
    assets = [{'name': n, 'digest': digest, 'size': len(binary)} for n in [name + '.pdf', name, 'another_tool']]
    tag = f'{tool}_stable'
    urls = []

    def open_url(url: str, timeout: int) -> io.BytesIO:
        urls.append(url)
        if '/releases/tags/' in url:
            assert url.endswith('/releases/tags/' + tag)
            return io.BytesIO(json.dumps({'assets': assets}).encode())
        assert url.endswith(f'/releases/download/{tag}/{name}')
        return io.BytesIO(binary)

    monkeypatch.setattr(download_tools, 'host_suffix', lambda: (system, suffix))
    monkeypatch.setattr(download_tools, 'urlopen', open_url)
    monkeypatch.chdir(tmp_path)
    path, version = download_tools.installed_tool(tool)
    assert version == 'v1.2.3'
    assert path == tmp_path / (tool + suffix)
    assert path.read_bytes() == binary
    assert download_tools.installed_tool(tool) == (path, version)
    assert len(urls) == 3
    path.write_bytes(b'outdated')
    assert download_tools.installed_tool(tool) == (path, version)
    assert path.read_bytes() == binary
    assert len(urls) == 5


def test_rate_limit_fallback_rejects_corrupt_download(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    tag = download_tools.CONSOLE + '_stable'
    href = f'/{download_tools.REPOSITORY}/releases/download/{tag}/ble_log_console_windows_v1.2.3.exe'
    html = f'<li><a href="{href}">Windows</a> sha256:{"0" * 64}</li>'

    def open_url(url: str, timeout: int) -> io.BytesIO | addinfourl:
        if 'api.github.com' in url:
            raise HTTPError(url, 403, 'rate limit exceeded', Message(), None)
        if 'expanded_assets' in url:
            return io.BytesIO(html.encode())
        assert url == 'https://github.com' + href
        return addinfourl(io.BytesIO(b'incomplete'), Message(), url)

    monkeypatch.setattr(download_tools, 'urlopen', open_url)
    monkeypatch.setattr(download_tools, 'host_suffix', lambda: ('windows', '.exe'))
    monkeypatch.chdir(tmp_path)
    with pytest.raises(ValueError, match='SHA-256'):
        download_tools.installed_tool(download_tools.CONSOLE)
    assert list(tmp_path.iterdir()) == []


@pytest.mark.parametrize('tool', [download_tools.ASSISTANT, download_tools.CONSOLE])
def test_command_launches_only_selected_tool(
    tool: str, tmp_path: Path, monkeypatch: pytest.MonkeyPatch, capsys: pytest.CaptureFixture[str]
) -> None:
    calls = []

    def install(selected: str) -> tuple[Path, str]:
        assert selected == tool
        return tmp_path / tool, 'v1.2.3'

    def launch(command: list[str]) -> int:
        calls.append(command)
        return 7

    monkeypatch.chdir(tmp_path)
    monkeypatch.setattr(download_tools, 'installed_tool', install)
    monkeypatch.setattr(download_tools.sys.stdin, 'isatty', lambda: False)
    monkeypatch.setattr(download_tools.subprocess, 'call', launch)
    assert download_tools.main(tool) == 7
    expected = [str(tmp_path / tool)]
    if tool == download_tools.ASSISTANT:
        expected.append(str(tmp_path))
    assert calls == [expected]
    assert f'v1.2.3: {tmp_path / tool}' in capsys.readouterr().out


@pytest.mark.parametrize('known_size', [True, False])
def test_progress_is_visible_before_completion(
    known_size: bool, capsys: pytest.CaptureFixture[str], monkeypatch: pytest.MonkeyPatch
) -> None:
    payload = b'a' * (2 * 1024 * 1024)
    times = iter(range(100))
    monkeypatch.setattr(download_tools.time, 'monotonic', lambda: next(times))
    output = io.BytesIO()
    download_tools.copy_with_progress(io.BytesIO(payload), output, len(payload) if known_size else 0)
    lines = capsys.readouterr().out.splitlines()
    assert any('1.0 MiB' in line for line in lines[:-1])
    assert '2.0 MiB' in lines[-1]
    assert ('100%' in lines[-1]) == known_size
    assert output.getvalue() == payload
