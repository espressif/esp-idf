# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os.path
import re
import time

import pytest
import serial
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

# Matches lines emitted by encode() in trace_FreeRTOS.c, e.g.
#   [+  12345 us] TASK_IN      producer
#   [+     12 us] Q_SEND       q=0x3fc8a210
TRACE_LINE_RE = re.compile(r'^\[\+\s*\d+ us\] ([A-Z_]+)\s*(.*)$')

# Minimum number of occurrences each event type must reach within the capture window.
EXPECTED_MIN_COUNTS = {
    'TASK_CREATE': 2,  # producer + consumer
    'Q_CREATE': 1,  # xQueueCreate in app_main
    'TASK_IN': 5,  # producer/consumer + idle task
    'Q_SEND': 5,  # producer sends every 50 ms
    'Q_RECEIVE': 5,  # consumer receives every send
    'ISR_IN': 5,  # FreeRTOS systick
}

ISR_EXIT_MIN = 5

# Fraction of malformed lines we tolerate before failing.
MAX_MALFORMED_RATIO = 0.05


def _validate_trace_data(trace_log_path: str) -> None:
    """Validate the human-readable trace log produced by ext_trace_lib."""
    with open(trace_log_path, encoding='utf-8', errors='replace') as f:
        lines = [line.rstrip() for line in f if line.strip()]

    assert lines, f'No trace data captured in {trace_log_path}'

    counts: dict[str, int] = {}
    create_names: set[str] = set()
    malformed = 0

    for line in lines:
        m = TRACE_LINE_RE.match(line)
        if not m:
            malformed += 1
            continue
        evt, detail = m.group(1), m.group(2).strip()
        counts[evt] = counts.get(evt, 0) + 1
        if evt == 'TASK_CREATE':
            create_names.add(detail)

    ratio = malformed / len(lines)
    assert ratio <= MAX_MALFORMED_RATIO, (
        f'Too many malformed lines: {malformed}/{len(lines)} '
        f'({ratio:.1%} > {MAX_MALFORMED_RATIO:.0%}). Likely USJ TX ring overflow; '
        f'bump CONFIG_ESP_TRACE_USJ_TX_BUFFER_SIZE or slow down the producer.'
    )

    for evt, minimum in EXPECTED_MIN_COUNTS.items():
        seen = counts.get(evt, 0)
        assert seen >= minimum, f'Expected at least {minimum} {evt} events, got {seen}'

    isr_exits = counts.get('ISR_OUT', 0) + counts.get('ISR_YIELD', 0)
    assert isr_exits >= ISR_EXIT_MIN, (
        f'Expected at least {ISR_EXIT_MIN} ISR_OUT+ISR_YIELD events, '
        f'got {isr_exits} (ISR_OUT={counts.get("ISR_OUT", 0)}, '
        f'ISR_YIELD={counts.get("ISR_YIELD", 0)})'
    )

    assert 'producer' in create_names, f'producer task not seen in TASK_CREATE: {create_names}'
    assert 'consumer' in create_names, f'consumer task not seen in TASK_CREATE: {create_names}'


def _capture_trace(ser: serial.Serial, trace_log_path: str, capture_s: float = 5.0) -> None:
    """Capture trace output from the USB-Serial-JTAG endpoint."""
    ser.reset_input_buffer()
    with open(trace_log_path, 'w+b') as f:
        end_time = time.time() + capture_s
        while time.time() < end_time:
            try:
                if ser.in_waiting:
                    f.write(ser.read(ser.in_waiting))
            except serial.SerialTimeoutException:
                assert False, 'Timeout reached while reading from serial port, exiting...'

        # Drain anything still in flight after the capture window.
        time.sleep(0.2)
        end_time = time.time() + 1.0
        last_data_time = time.time()
        while time.time() < end_time and (time.time() - last_data_time) <= 0.3:
            try:
                if ser.in_waiting:
                    f.write(ser.read(ser.in_waiting))
                    last_data_time = time.time()
            except serial.SerialTimeoutException:
                assert False, 'Timeout reached while reading from serial port, exiting...'


@pytest.mark.usb_serial_jtag
@idf_parametrize('target', soc_filtered_targets('SOC_USB_SERIAL_JTAG_SUPPORTED == 1'), indirect=['target'])
@pytest.mark.parametrize('config', [pytest.param('default')], indirect=True)
@pytest.mark.temp_skip_ci(targets=['esp32h4', 'esp32s31'], reason='lack of runner # TODO: IDFCI-10703')
def test_esp_trace_ext_lib_usj(dut: IdfDut) -> None:
    dut.expect('Start of trace session', timeout=5)

    time.sleep(1)  # wait for USJ port to be ready
    usj_port = '/dev/serial_ports/ttyACM-esp32'
    ser = serial.Serial(usj_port, baudrate=1000000, timeout=10)
    trace_log_path = os.path.join(dut.logdir, 'ext_trace.log')

    _capture_trace(ser, trace_log_path)
    _validate_trace_data(trace_log_path)
