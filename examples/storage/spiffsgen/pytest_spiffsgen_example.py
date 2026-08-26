# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import hashlib
import os

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_spiffsgen_example(dut: Dut) -> None:
    # Test with default build configurations
    base_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'spiffs_image')

    # Expect hello.txt is read successfully
    with open(os.path.join(base_dir, 'hello.txt'), 'r', encoding='utf-8') as hello_txt:
        dut.expect('Read from hello.txt: ' + hello_txt.read().rstrip())

    # Expect alice.txt MD5 hash is computed accurately
    with open(os.path.join(base_dir, 'sub', 'alice.txt'), 'rb') as alice_txt:
        alice_md5 = hashlib.md5(alice_txt.read()).hexdigest()
        dut.expect('Computed MD5 hash of alice.txt: ' + alice_md5)
