# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import typing

import pytest


@pytest.fixture(autouse=True, scope='session')
def bsasm_terminal_env() -> typing.Generator[None, None, None]:
    """Set terminal env so bsasm subprocesses produce consistent output.

    Same pattern as tools/test_mkdfu/conftest.py and tools/test_idf_py/conftest.py:
    COLUMNS raises Rich's non-TTY default of 80, preventing most line wrapping.
    NO_COLOR=1 strips ANSI escape codes.

    Current tests mainly check return codes and binary content, but bsasm.py uses
    esp-pylib logging; keep this fixture so future string asserts stay stable.
    """
    keys = ('COLUMNS', 'LINES', 'NO_COLOR', 'FORCE_COLOR', 'PY_COLORS', 'TERM')
    saved = {k: os.environ.get(k) for k in keys}
    os.environ['COLUMNS'] = '1000'
    os.environ['LINES'] = '40'
    os.environ['NO_COLOR'] = '1'
    for k in ('FORCE_COLOR', 'PY_COLORS'):
        os.environ.pop(k, None)
    yield
    for k, v in saved.items():
        if v is None:
            os.environ.pop(k, None)
        else:
            os.environ[k] = v
