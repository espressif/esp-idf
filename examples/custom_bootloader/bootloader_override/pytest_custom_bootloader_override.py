# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.app import IdfApp


@pytest.mark.supported_targets
@pytest.mark.generic
def test_custom_bootloader_impl_example(app: IdfApp, dut: Dut) -> None:
    # Expect to read a message from the custom bootloader
    # This message is defined in the Kconfig file, retrieve it while deleting
    # leading and trailing quotes (")
    welcome_message = app.sdkconfig['EXAMPLE_BOOTLOADER_WELCOME_MESSAGE']
    dut.expect_exact(welcome_message)

    # Expect to read a message from the user application
    dut.expect_exact('Application started!')
