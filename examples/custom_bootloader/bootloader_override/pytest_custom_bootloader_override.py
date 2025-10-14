# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.app import IdfApp
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_custom_bootloader_impl_example(app: IdfApp, dut: Dut) -> None:
    # Expect to read a message from the custom bootloader
    # This message is defined in the Kconfig file, retrieve it while deleting
    # leading and trailing quotes (")
    welcome_message = app.sdkconfig['EXAMPLE_BOOTLOADER_WELCOME_MESSAGE']
    dut.expect_exact(welcome_message)

    # Expect to read a message from the user application
    dut.expect_exact('Application started!')
