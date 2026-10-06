# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('config', ['default'], indirect=['config'])
@idf_parametrize('target', ['esp32', 'esp32s2', 'esp32s3'], indirect=['target'])
def test_task_self_delete(dut: Dut) -> None:
    dut.expect_exact('task_self_delete: PASS')
    # PASS is printed before main_task self-deletes. The heartbeat only runs
    # if that delete (and the child delete) did not panic.
    dut.expect_exact('task_self_delete: still running')
