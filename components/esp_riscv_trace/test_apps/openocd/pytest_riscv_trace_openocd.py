# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import hashlib
import os
import struct
import time
import typing
from dataclasses import dataclass

import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

if typing.TYPE_CHECKING:
    from conftest import OpenOCD


FLASH_PORT = '/dev/serial_ports/ttyUSB-esp32'
TRACE_TARGETS = soc_filtered_targets('SOC_RISCV_TRACE_SUPPORTED == 1')
READY = 'riscv_trace_ocd: ready'
LOG_TIMEOUT_S = 10
WRAP_WAIT_S = 1
TIMED_CAPTURE_MS = 500
LONG_CAPTURE_MS = 60000

# .esptrace v1 layout, mirrors esp_riscv_trace_file.c in openocd-esp32
ESPT_MAGIC = b'ESPT'
ESPT_VERSION = 1
ESPT_HDR = struct.Struct('<4sHH')
ESPT_HDR_SIZE = 100
ESPT_OFF_SNAPSHOT = 8
RVTS_DESC_SIZE = 32
RVTS_PARAMS_SIZE = 28
ESPT_OFF_SHA256 = ESPT_OFF_SNAPSHOT + RVTS_DESC_SIZE + RVTS_PARAMS_SIZE
SHA256_SIZE = 32
REC_HDR = struct.Struct('<BBHI')
REC_CORE_DESC = 1
REC_DATA = 2
REC_END = 3
END_OK = 0

# RVTS root and core descriptor fields, mirrors esp_riscv_trace_snapshot.h
RVTS_OFF_CORE_COUNT = 28
RVTS_OFF_CAPTURE_REASON = 29
RVTS_CORE_DESC_SIZE = 36
RVTS_CORE_DESC = struct.Struct('<IIIIII' + 'B' * 12)
RVTS_STATE_STOPPED = 3
RVTS_STATE_FROZEN = 4
RVTS_MEMORY_MODE_LOOP = 2
RVTS_REASON_EXPLICIT_STOP = 1
RVTS_REASON_PANIC = 2


@dataclass
class TraceCore:
    core_id: int
    state: int
    memory_mode: int
    capacity: int
    head_offset: int
    head_valid: int
    fifo_empty: int
    fifo_overflow: int
    desc: bytes
    data: bytes = b''


@dataclass
class TraceFile:
    core_count: int
    capture_reason: int
    sha256: bytes
    cores: dict[int, TraceCore]
    end_status: int | None


def read_esptrace(path: str) -> TraceFile:
    with open(path, 'rb') as f:
        blob = f.read()
    magic, version, hdr_size = ESPT_HDR.unpack_from(blob, 0)
    assert magic == ESPT_MAGIC, f'{path}: bad magic {magic!r}'
    assert version == ESPT_VERSION, f'{path}: version {version}'
    assert hdr_size == ESPT_HDR_SIZE, f'{path}: header size {hdr_size}'
    root = blob[ESPT_OFF_SNAPSHOT : ESPT_OFF_SNAPSHOT + RVTS_DESC_SIZE]
    trace = TraceFile(
        core_count=root[RVTS_OFF_CORE_COUNT],
        capture_reason=root[RVTS_OFF_CAPTURE_REASON],
        sha256=blob[ESPT_OFF_SHA256 : ESPT_OFF_SHA256 + SHA256_SIZE],
        cores={},
        end_status=None,
    )

    pos = hdr_size
    while pos < len(blob):
        rec_type, core, _flags, length = REC_HDR.unpack_from(blob, pos)
        payload = blob[pos + REC_HDR.size : pos + REC_HDR.size + length]
        assert len(payload) == length, f'{path}: truncated record at {pos}'
        pos += REC_HDR.size + length
        if rec_type == REC_CORE_DESC:
            assert length == RVTS_CORE_DESC_SIZE, f'{path}: core descriptor size {length}'
            fields = RVTS_CORE_DESC.unpack(payload)
            buffer_addr, capacity, head_offset = fields[0:3]
            core_id, state, memory_mode = fields[6:9]
            head_valid, fifo_empty, _memory_full, fifo_overflow = fields[13:17]
            trace.cores[core] = TraceCore(
                core_id, state, memory_mode, capacity, head_offset, head_valid, fifo_empty, fifo_overflow, payload
            )
        elif rec_type == REC_DATA:
            trace.cores[core].data = payload
        elif rec_type == REC_END:
            trace.end_status = struct.unpack('<I', payload)[0]
    assert pos == len(blob), f'{path}: {len(blob) - pos} trailing bytes'
    return trace


def traced_cores(dut: IdfDut) -> list[int]:
    mask = int(dut.app.sdkconfig.get('ESP_RISCV_TRACE_CORE_MASK'))
    return [core for core in range(mask.bit_length()) if mask & (1 << core)]


def core_mask(cores: list[int]) -> int:
    return sum(1 << core for core in cores)


def check_trace_file(dut: IdfDut, path: str, state: int, reason: int, cores: list[int]) -> TraceFile:
    trace = read_esptrace(path)
    with open(dut.app.elf_file, 'rb') as elf:
        assert trace.sha256 == hashlib.sha256(elf.read()).digest(), f'{path}: app ELF SHA-256 mismatch'
    assert trace.capture_reason == reason, f'{path}: capture reason {trace.capture_reason}'
    assert trace.end_status == END_OK, f'{path}: END status {trace.end_status}'
    assert sorted(trace.cores) == cores, f'{path}: cores {sorted(trace.cores)}'
    for core in cores:
        c = trace.cores[core]
        assert c.core_id == core
        assert c.state == state, f'{path} core {core}: state {c.state}'
        assert c.memory_mode == RVTS_MEMORY_MODE_LOOP, f'{path} core {core}: memory mode {c.memory_mode}'
        assert c.head_valid and c.fifo_empty, f'{path} core {core}: head_valid {c.head_valid} fifo_empty {c.fifo_empty}'
        assert len(c.data) == c.capacity, f'{path} core {core}: {len(c.data)} data bytes, capacity {c.capacity}'
        # Start clears the buffer, so data after the head means the loop buffer wrapped
        wrapped = c.head_offset == c.capacity or any(c.data[c.head_offset :])
        assert wrapped, f'{path} core {core}: buffer did not wrap'
    return trace


def ocd_log_mark(ocd: 'OpenOCD') -> int:
    return os.path.getsize(ocd.log_file)


def ocd_log_wait(ocd: 'OpenOCD', text: str, since: int, timeout: float = LOG_TIMEOUT_S) -> str:
    """Waits for text in the OpenOCD log written after the since offset and returns that part of the log."""
    end = time.time() + timeout
    while True:
        with open(ocd.log_file, encoding='utf-8', errors='ignore') as f:
            f.seek(since)
            log = f.read()
        if text in log:
            return log
        assert time.time() < end, f'"{text}" not in {ocd.log_file} within {timeout} s:\n{log}'
        ocd.consume_output(0.2)


def ocd_wait_reply(ocd: 'OpenOCD', cmd: str, text: str, timeout: float = LOG_TIMEOUT_S) -> str:
    """Repeats cmd until its reply contains text and returns that reply."""
    end = time.time() + timeout
    while True:
        out: str = ocd.write_cmd(cmd)
        if text in out:
            return out
        assert time.time() < end, f'"{text}" not in reply to "{cmd}" within {timeout} s:\n{out}'
        time.sleep(0.2)


def trigger_panic(dut: IdfDut) -> None:
    dut.write('panic')
    dut.expect_exact('Setting breakpoint at 0x', timeout=LOG_TIMEOUT_S)


def start_openocd_session(ocd: 'OpenOCD', dut: IdfDut) -> None:
    ocd.write_cmd('reset run')
    dut.expect_exact(READY, timeout=LOG_TIMEOUT_S)


@pytest.mark.usb_serial_jtag
@pytest.mark.temp_skip_ci(targets=TRACE_TARGETS, reason='needs OpenOCD with esp riscv_trace')
@idf_parametrize('config', ['ocd'], indirect=['config'])
@idf_parametrize('target', TRACE_TARGETS, indirect=['target'])
@idf_parametrize('port', [FLASH_PORT], indirect=['port'])
def test_riscv_trace_openocd_start_stop(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    cores = traced_cores(dut)
    mask = core_mask(cores)
    stop_file, dump_file, timed_file, unused_file = (
        os.path.join(dut.logdir, name)
        for name in ('stop.esptrace', 'dump.esptrace', 'timed.esptrace', 'unused.esptrace')
    )

    time.sleep(1)  # Wait for the USJ port to be ready
    dut.expect_exact(READY, timeout=LOG_TIMEOUT_S)
    with openocd_dut.run() as ocd:
        start_openocd_session(ocd, dut)

        info = ocd.write_cmd('esp riscv_trace info', '(RVOC) ABI 1')
        assert '(RVTS) ABI 1.0' in info and 'start/stop available' in info, info
        ocd.write_cmd(f'esp riscv_trace start stream {unused_file}', 'Stream mode is not supported yet')
        untraced_mask = mask | (1 << (max(cores) + 1))
        ocd.write_cmd(f'esp riscv_trace start loop {unused_file} 0 0x{untraced_mask:x}', 'cores that are not traced')

        # Loop capture until the stop command
        ocd.write_cmd(f'esp riscv_trace start loop {stop_file}', f'loop capture started on cores 0x{mask:x}')
        info = ocd.write_cmd('esp riscv_trace info', 'OpenOCD capture: loop')
        assert 'state looping' in info, info

        ocd.write_cmd(f'esp riscv_trace start loop {unused_file}', 'already running')
        ocd.write_cmd(f'esp riscv_trace dump {unused_file}', 'Cannot dump: an OpenOCD capture to')
        dut.write('api_start')
        dut.expect_exact('api_start: ESP_ERR_INVALID_STATE', timeout=LOG_TIMEOUT_S)

        ocd.consume_output(WRAP_WAIT_S)
        mark = ocd_log_mark(ocd)
        ocd.write_cmd('esp riscv_trace stop')
        log = ocd_log_wait(ocd, f'Wrote cores 0x{mask:x}', mark)
        assert 'stopped (stop command)' in log and 'wrapped' in log, log
        stopped = check_trace_file(dut, stop_file, RVTS_STATE_STOPPED, RVTS_REASON_EXPLICIT_STOP, cores)

        # Dump after the stop reads the same capture
        mark = ocd_log_mark(ocd)
        ocd.write_cmd(f'esp riscv_trace dump {dump_file}')
        ocd_log_wait(ocd, f'Wrote cores 0x{mask:x}', mark)
        dumped = check_trace_file(dut, dump_file, RVTS_STATE_STOPPED, RVTS_REASON_EXPLICIT_STOP, cores)
        for core in cores:
            assert dumped.cores[core].desc == stopped.cores[core].desc, f'core {core}: descriptor differs'
            assert dumped.cores[core].data == stopped.cores[core].data, f'core {core}: data differs'

        # Timed capture stops by itself
        mark = ocd_log_mark(ocd)
        ocd.write_cmd(f'esp riscv_trace start loop {timed_file} {TIMED_CAPTURE_MS}')
        ocd_log_wait(ocd, f'Wrote cores 0x{mask:x}', mark)
        log = ocd_log_wait(ocd, 'stopped (duration elapsed)', mark)
        assert 'wrapped' in log, log
        check_trace_file(dut, timed_file, RVTS_STATE_STOPPED, RVTS_REASON_EXPLICIT_STOP, cores)
        info = ocd.write_cmd('esp riscv_trace info')
        assert 'OpenOCD capture' not in info, info

        ocd.write_cmd('esp riscv_trace stop', 'Nothing to stop')
        assert not os.path.exists(unused_file), f'{unused_file} was written'

        # The encoder stays stopped after an OpenOCD stop
        dut.write('status')
        for core in cores:
            dut.expect_exact(f'status: core {core} err ESP_OK work 0', timeout=LOG_TIMEOUT_S)

        # The app can start the encoder again
        dut.write('api_start')
        dut.expect_exact('api_start: ESP_OK', timeout=LOG_TIMEOUT_S)
        dut.write('status')
        dut.expect(r'status: core 0 err ESP_OK work [12]', timeout=LOG_TIMEOUT_S)


@pytest.mark.usb_serial_jtag
@pytest.mark.temp_skip_ci(targets=TRACE_TARGETS, reason='needs OpenOCD with esp riscv_trace')
@idf_parametrize('config', ['ocd'], indirect=['config'])
@idf_parametrize('target', TRACE_TARGETS, indirect=['target'])
@idf_parametrize('port', [FLASH_PORT], indirect=['port'])
def test_riscv_trace_openocd_panic(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    cores = traced_cores(dut)
    mask = core_mask(cores)
    stop_file, after_reset_file, timed_file = (
        os.path.join(dut.logdir, name)
        for name in ('panic_stop.esptrace', 'after_reset.esptrace', 'panic_timed.esptrace')
    )

    time.sleep(1)  # Wait for the USJ port to be ready
    dut.expect_exact(READY, timeout=LOG_TIMEOUT_S)
    with openocd_dut.run() as ocd:
        start_openocd_session(ocd, dut)

        # Panic during a capture that waits for the stop command
        ocd.write_cmd(f'esp riscv_trace start loop {stop_file}', f'loop capture started on cores 0x{mask:x}')
        ocd.consume_output(WRAP_WAIT_S)
        trigger_panic(dut)
        # The early-break handler finalizes the snapshot right before the debugger halt
        info = ocd_wait_reply(ocd, 'esp riscv_trace info', 'frozen by panic')
        assert info.count('frozen by panic') == len(cores), info
        assert 'capture reason panic' in info, info

        mark = ocd_log_mark(ocd)
        ocd.write_cmd('esp riscv_trace stop')
        log = ocd_log_wait(ocd, f'Wrote cores 0x{mask:x}', mark)
        assert f'loop capture on cores 0x{mask:x} was frozen by a panic (stop command)' in log, log
        assert ' s captured' not in log, log
        check_trace_file(dut, stop_file, RVTS_STATE_FROZEN, RVTS_REASON_PANIC, cores)
        # The stop after the panic released the session before any reset
        info = ocd.write_cmd('esp riscv_trace info')
        assert 'OpenOCD capture' not in info, info
        ocd.write_cmd('esp riscv_trace stop', 'Nothing to stop')

        # A new capture works after a reset
        start_openocd_session(ocd, dut)
        ocd.write_cmd(f'esp riscv_trace start loop {after_reset_file}', f'loop capture started on cores 0x{mask:x}')
        ocd.consume_output(WRAP_WAIT_S)
        mark = ocd_log_mark(ocd)
        ocd.write_cmd('esp riscv_trace stop')
        ocd_log_wait(ocd, f'Wrote cores 0x{mask:x}', mark)
        check_trace_file(dut, after_reset_file, RVTS_STATE_STOPPED, RVTS_REASON_EXPLICIT_STOP, cores)

        # Panic during a long timed capture. Stop writes it at once, without waiting for the timer.
        mark = ocd_log_mark(ocd)
        ocd.write_cmd(f'esp riscv_trace start loop {timed_file} {LONG_CAPTURE_MS}')
        ocd.consume_output(WRAP_WAIT_S)
        trigger_panic(dut)
        ocd_wait_reply(ocd, 'esp riscv_trace info', 'frozen by panic')
        ocd.write_cmd('esp riscv_trace stop')
        log = ocd_log_wait(ocd, f'Wrote cores 0x{mask:x}', mark)
        assert 'was frozen by a panic (stop command)' in log, log
        assert 'duration elapsed' not in log, log
        check_trace_file(dut, timed_file, RVTS_STATE_FROZEN, RVTS_REASON_PANIC, cores)
        info = ocd.write_cmd('esp riscv_trace info')
        assert 'OpenOCD capture' not in info, info


@pytest.mark.usb_serial_jtag
@pytest.mark.temp_skip_ci(targets=TRACE_TARGETS, reason='needs OpenOCD with esp riscv_trace')
@idf_parametrize('config', ['ocd_autostart'], indirect=['config'])
@idf_parametrize('target', TRACE_TARGETS, indirect=['target'])
@idf_parametrize('port', [FLASH_PORT], indirect=['port'])
def test_riscv_trace_openocd_autostart_panic(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    cores = traced_cores(dut)
    mask = core_mask(cores)
    dump_file, unused_file = (os.path.join(dut.logdir, name) for name in ('autostart.esptrace', 'unused.esptrace'))

    time.sleep(1)  # Wait for the USJ port to be ready
    dut.expect_exact(READY, timeout=LOG_TIMEOUT_S)
    with openocd_dut.run() as ocd:
        start_openocd_session(ocd, dut)

        # Autostart owns the encoder, so OpenOCD can only read
        info = ocd.write_cmd('esp riscv_trace info', 'start/stop disabled in firmware')
        assert info.count(', capturing,') == len(cores), info
        ocd.write_cmd(f'esp riscv_trace start loop {unused_file}', 'start/stop is disabled in firmware')
        mark = ocd_log_mark(ocd)
        ocd.write_cmd(f'esp riscv_trace dump {unused_file}')
        ocd_log_wait(ocd, 'is capturing. Stop the capture first.', mark)
        assert not os.path.exists(unused_file), f'{unused_file} was written'

        # A panic stops the autostart capture, and dump reads it without core dump
        ocd.consume_output(WRAP_WAIT_S)
        trigger_panic(dut)
        info = ocd_wait_reply(ocd, 'esp riscv_trace info', 'frozen by panic')
        assert info.count('frozen by panic') == len(cores), info
        assert 'capture reason panic' in info, info

        mark = ocd_log_mark(ocd)
        ocd.write_cmd(f'esp riscv_trace dump {dump_file}')
        ocd_log_wait(ocd, f'Wrote cores 0x{mask:x}', mark)
        check_trace_file(dut, dump_file, RVTS_STATE_FROZEN, RVTS_REASON_PANIC, cores)
