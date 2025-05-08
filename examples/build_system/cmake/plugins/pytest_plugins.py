# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import textwrap

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_plugins(dut: Dut) -> None:
    log_text = textwrap.dedent(r"""
        Nihao plugin performing self-registration...
        Successfully registered plugin 'Nihao'
        Hello plugin performing self-registration...
        Successfully registered plugin 'Hello'
        main_task: Calling app_main()
        List of plugins:
        - Plugin 'Hello'
        - Plugin 'Nihao'
        Calling greet function of plugin 'Hello'...
        Hello, World!
        Done with greet function of plugin 'Hello'.
        Calling greet function of plugin 'Nihao'...
        你好 World!
        Done with greet function of plugin 'Nihao'.
    """).strip('\n')

    for line in log_text.split('\n'):
        dut.expect_exact(line.encode('utf-8'))
