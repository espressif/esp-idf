# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut


@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.generic
def test_creating_task(
    dut: IdfDut
) -> None:
    dut.expect(r'esp32(?:[a-zA-Z]\d)?>')
    # test creating_task
    dut.write('create_task')
    expected_patterns = [
        'create task example: task#0 is running on core#0',
        'create task example: task#1 is running on core#0',
        r'create task example: task#2 is running on core#\d',
        r'create task example: task#3 is running on core#\d',
    ]
    dut.expect(expected_patterns, expect_all=True)


@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.generic
def test_queue(
    dut: IdfDut
) -> None:
    dut.expect(r'esp32(?:[a-zA-Z]\d)?>')
    # test queue tasks
    verify_amount = 5
    dut.write('queue')
    dut.expect('queue example: sent data')
    dut.expect('queue example: received data')

    for _ in range(verify_amount):
        data = eval(dut.expect(r'queue example: sent data = (\d+)').group(1))
        dut.expect('queue example: received data = ' + str(data))


@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.generic
def test_locks(
    dut: IdfDut
) -> None:
    dut.expect(r'esp32(?:[a-zA-Z]\d)?>')
    # test locks
    dut.write('lock')
    expected_patterns = [
        r'lock example: mutex task took \d+ us on core\d',
        r'lock example: spinlock task took \d+ us on core\d',
        r'lock example: atomic task took \d+ us on core\d',
        r'task\d read value = \d on core #\d',
        r'task\d set value = \d',
    ]
    dut.expect(expected_patterns, expect_all=True)


@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.generic
def test_task_notification(
    dut: IdfDut
) -> None:
    dut.expect(r'esp32(?:[a-zA-Z]\d)?>')
    # test task notification
    dut.write('task_notification')
    dut.expect('task notify example: send_task sends a notification')
    dut.expect('task notify example: 1 tasks pending')
    dut.expect('task notify example: rcv_task is processing this task notification')


@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.generic
def test_batch_proc_example(
    dut: IdfDut
) -> None:
    dut.expect(r'esp32(?:[a-zA-Z]\d)?>')
    # test batch processing example
    dut.write('batch_processing')

    batch_size = 5
    data_buf = [None] * batch_size

    for i in range(batch_size):
        res = dut.expect(r'batch processing example: enqueue data = (\d+)')
        data_buf[i] = eval(res.group(1)) if res else None
    for i in range(batch_size):
        expected_string = 'batch processing example: dequeue data = ' + str(data_buf[i])
        dut.expect(expected_string)
    dut.expect(r'batch processing example: decrease s_rcv_item_num to \d')
