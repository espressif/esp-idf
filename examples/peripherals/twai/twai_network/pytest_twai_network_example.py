# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import os.path
from threading import Thread
from typing import Tuple

import pytest
from pytest_embedded import Dut

# Define tuple of strings to expect for each DUT.
master_expect = ('TWAI Master: Driver installed', 'TWAI Master: Driver uninstalled')
slave_expect = ('TWAI Slave: Driver installed', 'TWAI Slave: Driver uninstalled')
listen_only_expect = (
    'TWAI Listen Only: Driver installed',
    'TWAI Listen Only: Driver uninstalled',
)


def dut_thread_callback(**kwargs) -> None:  # type: ignore
    # Parse keyword arguments
    dut = kwargs['dut']  # Get DUT from kwargs
    expected = kwargs['expected']
    result = kwargs[
        'result'
    ]  # Get result[out] from kwargs. MUST be of mutable type e.g. list

    # Must reset again as flashing during start_app will reset multiple times, causing unexpected results
    dut.reset()

    for string in expected:
        dut.expect(string, 20)

    # Mark thread has run to completion without any exceptions
    result[0] = True


@pytest.mark.esp32
@pytest.mark.skip(reason="there's not a good approach to sync multiple DUTs")
@pytest.mark.twai_network
@pytest.mark.parametrize(
    'count, app_path',
    [
        (
            3,
            f'{os.path.join(os.path.dirname(__file__), "twai_network_master")}|'
            f'{os.path.join(os.path.dirname(__file__), "twai_network_slave")}|'
            f'{os.path.join(os.path.dirname(__file__), "twai_network_listen_only")}',
        ),
    ],
    indirect=True,
)
def test_twai_network_example(dut: Tuple[Dut, Dut, Dut]) -> None:
    dut_master = dut[0]
    dut_slave = dut[1]
    dut_listen_only = dut[2]

    # Create dict of keyword arguments for each dut
    results = [[False], [False], [False]]
    master_kwargs = {'dut': dut_master, 'result': results[0], 'expected': master_expect}
    slave_kwargs = {'dut': dut_slave, 'result': results[1], 'expected': slave_expect}
    listen_only_kwargs = {
        'dut': dut_listen_only,
        'result': results[2],
        'expected': listen_only_expect,
    }

    # Create thread for each dut
    dut_master_thread = Thread(
        target=dut_thread_callback, name='Master Thread', kwargs=master_kwargs
    )
    dut_slave_thread = Thread(
        target=dut_thread_callback, name='Slave Thread', kwargs=slave_kwargs
    )
    dut_listen_only_thread = Thread(
        target=dut_thread_callback, name='Listen Only Thread', kwargs=listen_only_kwargs
    )

    # Start each thread
    dut_listen_only_thread.start()
    dut_master_thread.start()
    dut_slave_thread.start()

    # Wait for threads to complete
    dut_listen_only_thread.join()
    dut_master_thread.join()
    dut_slave_thread.join()

    # check each thread ran to completion
    for result in results:
        if result[0] is not True:
            raise Exception('One or more threads did not run successfully')
