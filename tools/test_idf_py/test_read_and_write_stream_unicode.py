#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
"""Regression for UTF-8 build lines on narrow Windows stdio (pipe + cp1252)."""

from __future__ import annotations

import asyncio
import io
import os
import sys
import tempfile
import unittest

try:
    os.environ['IDF_PATH']  # noqa: B018
except KeyError:
    print('This test needs IDF_PATH set (export.bat / export.sh).', file=sys.stderr)
    sys.exit(1)

CWD = os.path.dirname(__file__)
try:
    from idf_py_actions.tools import RunTool
except ImportError:
    sys.path.append(os.path.join(CWD, '..'))
    from idf_py_actions.tools import RunTool


class TestReadAndWriteStreamUnicode(unittest.TestCase):
    def test_utf8_line_survives_cp1252_text_wrapper(self) -> None:
        """Subprocess UTF-8 must reach the binary layer without TextIO cp1252 re-encode (esp-hosted-mcu#176)."""
        # U+1F600 is not representable in cp1252; \u2014 (em dash) is 0x97 in cp1252 and would not fail strict encode.
        exotic = '\U0001f600'
        line_bytes = f'ninja: unicode {exotic} done\n'.encode()

        narrow = io.TextIOWrapper(io.BytesIO(), encoding='cp1252', errors='strict')
        with self.assertRaises(UnicodeEncodeError):
            narrow.write(exotic)

        raw_out = io.BytesIO()
        text_out = io.TextIOWrapper(
            raw_out,
            encoding='cp1252',
            errors='strict',
            newline='',
            line_buffering=True,
        )
        self.assertFalse(text_out.isatty())

        captured: dict[str, bytes] = {}

        async def run() -> None:
            reader = asyncio.StreamReader()
            reader.feed_data(line_bytes)
            reader.feed_eof()
            with tempfile.TemporaryDirectory() as tmp:
                log_path = os.path.join(tmp, 'cap.log')
                tool = RunTool(
                    'unittest_build',
                    ['ninja', 'all'],
                    tmp,
                    build_dir=tmp,
                    hints=False,
                    interactive=False,
                    convert_output=False,
                )
                await tool.read_and_write_stream(reader, log_path, text_out)
                with open(log_path, 'rb') as f:
                    captured['log'] = f.read()

        asyncio.run(run())

        text_out.flush()
        emitted = raw_out.getvalue()
        self.assertIn(exotic.encode('utf-8'), emitted)
        self.assertIn(exotic.encode('utf-8'), captured['log'])
        self.assertTrue(captured['log'].startswith(b'Command:'))
