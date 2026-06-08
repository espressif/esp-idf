# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import sys
from collections.abc import Callable
from pathlib import Path
from typing import TYPE_CHECKING

import pytest

if TYPE_CHECKING:
    from eth_test_runner import EthTestRunner


def _setup_eth_test_runner_sys_path() -> Path | None:
    root = Path(__file__).resolve().parent
    for name in ('espressif__eth_test_app', 'eth_test_app'):
        candidate = root / 'managed_components' / name
        if (candidate / 'eth_test_runner.py').exists():
            candidate_str = str(candidate)
            if candidate_str not in sys.path:
                sys.path.insert(0, candidate_str)
            return candidate

    return None


# Support local runs when managed_components is already present after build.
_setup_eth_test_runner_sys_path()


@pytest.fixture
def eth_test_runner(
    app_path: str,
    download_app_extra: Callable[[str], None],
) -> 'EthTestRunner':
    runner_dir = _setup_eth_test_runner_sys_path()
    if runner_dir is None:
        download_app_extra(app_path)
        runner_dir = _setup_eth_test_runner_sys_path()

    if runner_dir is None:
        raise RuntimeError('eth_test_runner.py not found. Build the test app locally or download CI artifacts first.')

    from eth_test_runner import EthTestRunner

    return EthTestRunner()
