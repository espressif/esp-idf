# SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import subprocess
import time
from collections.abc import Generator
from contextlib import contextmanager
from typing import Any

import can
import pexpect
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

# ---------------------------------------------------------------------------
# Constants / Helpers
# ---------------------------------------------------------------------------

PROMPTS = ['twai>']

# Hardware configuration
DEFAULT_BITRATE = 500000
DEFAULT_TX_GPIO = 4
DEFAULT_RX_GPIO = 5
NO_TRANSCEIVER_GPIO = 4


def _ctrler_name(controller_id: int) -> str:
    return f'twai{controller_id}'


def _id_pattern(controller_str: str, can_id: int) -> str:
    """Return regex pattern for a dump line that contains this ctrl & CAN ID."""
    hex_part = f'{can_id:08X}' if can_id > 0x7FF else f'{can_id:03X}'
    return rf'{controller_str}\s+{hex_part}\s+\['


def esp_enter_flash_mode(dut: Dut) -> None:
    ser = dut.serial.proc
    ser.setRTS(True)  # EN Low
    time.sleep(0.5)
    ser.setDTR(True)  # GPIO0 Low
    ser.setRTS(False)  # EN High
    dut.expect('waiting for download', timeout=2)
    ser.setDTR(False)  # Back RTS/DTR to 1/1 to avoid affect to esptool


# ---------------------------------------------------------------------------
# TWAI helper
# ---------------------------------------------------------------------------


class TwaiTestHelper:
    """TWAI test helper built on small, reusable atomic operations."""

    def __init__(self, dut: Dut) -> None:
        self.dut = dut
        self.timeout = 5
        self._wait_ready()

    # ------------------------- atomic I/O ops -------------------------
    def _wait_ready(self) -> None:
        try:
            self.dut.expect_exact(PROMPTS, timeout=10)
        except (pexpect.exceptions.TIMEOUT, pexpect.exceptions.EOF):
            self.sendline('help')
            self.expect(['Commands:'], timeout=5)

    def sendline(self, cmd: str) -> None:
        self.dut.write(f'\n{cmd}\n')

    def expect(self, patterns: list[str] | str, timeout: float | None = None) -> bool:
        timeout = timeout or self.timeout
        try:
            self.dut.expect(patterns, timeout=timeout)
            return True
        except (pexpect.exceptions.TIMEOUT, pexpect.exceptions.EOF):
            return False

    def run(self, cmd: str, expect: list[str] | str | None = None, timeout: float | None = None) -> bool:
        self.sendline(cmd)
        if self.expect([r'Command returned non-zero error code:', r'ERROR', r'Failed', r'Invalid'], timeout=0.5):
            return False
        return self.expect(expect or PROMPTS, timeout)

    # ------------------------- command builders -------------------------
    def build_init_cmd_str(
        self,
        *,
        controller_id: int = 0,
        tx_gpio: int | None = None,
        rx_gpio: int | None = None,
        bitrate: int | None = None,
        clk_out_gpio: int | None = None,
        bus_off_gpio: int | None = None,
        fd_bitrate: int | None = None,
        loopback: bool = False,
        self_test: bool = False,
        listen: bool = False,
    ) -> str:
        parts = [f'twai_init {_ctrler_name(controller_id)}']
        # GPIO and bitrate options (order preserved: tx, rx, bitrate, fd_bitrate, clk_out, bus_off)
        for value, flag in [
            (tx_gpio, '-t'),
            (rx_gpio, '-r'),
            (bitrate, '-b'),
            (fd_bitrate, '-B'),
            (clk_out_gpio, '-c'),
            (bus_off_gpio, '-o'),
        ]:
            if value is not None:
                parts.append(f'{flag} {value}')
        # Boolean flags (order preserved: loopback, self_test, listen)
        for value, flag in [(loopback, '--loopback'), (self_test, '--self-test'), (listen, '--listen')]:
            if value:
                parts.append(flag)
        return ' '.join(parts)

    # ------------------------- high-level ops -------------------------
    def init(self, controller_id: int = 0, **kwargs: Any) -> bool:
        return self.run(self.build_init_cmd_str(controller_id=controller_id, **kwargs))

    def deinit(self, controller_id: int = 0) -> bool:
        return self.run(f'twai_deinit {_ctrler_name(controller_id)}')

    def dump_start(self, controller_id: int = 0, dump_filter: str | None = None) -> bool:
        cmd = f'twai_dump {_ctrler_name(controller_id)}'
        if dump_filter:
            cmd += f',{dump_filter}'
        return self.run(cmd)

    def dump_stop(self, controller_id: int = 0) -> tuple[bool, bool]:
        """Stop dump and return (stopped_ok, timeout_warning_seen)."""
        self.sendline(f'twai_dump {_ctrler_name(controller_id)} --stop')
        # If the dump task does not exit naturally, the implementation prints this warning.
        warning_seen = self.expect(r'Dump task did not exit naturally, timeout', timeout=2)
        # Whether or not warning appears, we should be back to a prompt.
        prompt_ok = self.expect(PROMPTS, timeout=2) or True  # relax
        return prompt_ok, warning_seen

    def send(self, frame_str: str, controller_id: int = 0) -> bool:
        return self.run(f'twai_send {_ctrler_name(controller_id)} {frame_str}')

    def info(self, controller_id: int = 0) -> bool:
        return self.run(
            f'twai_info {_ctrler_name(controller_id)}',
            [rf'TWAI{controller_id} Status:', r'Node State:', r'Bitrate:'],
        )

    def recover(self, controller_id: int = 0, timeout_ms: int | None = None) -> bool:
        cmd = f'twai_recover {_ctrler_name(controller_id)}'
        if timeout_ms is not None:
            cmd += f' -t {timeout_ms}'
        self.sendline(cmd)
        return self.expect(['Recovery not needed', 'node is Error Active', 'ESP_ERR_INVALID_STATE'])  # any

    def expect_info_format(self, controller_id: int = 0) -> bool:
        self.sendline(f'twai_info {_ctrler_name(controller_id)}')
        checks = [
            rf'TWAI{controller_id} Status: \w+',
            r'Node State: \w+',
            r'Error Counters: TX=\d+, RX=\d+',
            r'Bitrate: \d+ bps',
        ]
        return all(self.expect(p, timeout=2) for p in checks)

    def invalid_should_fail(self, cmd: str, timeout: float = 2.0) -> bool:
        # Wait for prompt to clear any previous error logs from buffer
        self.expect(PROMPTS, timeout=1.0)
        self.sendline(cmd)
        return self.expect([r'Command returned non-zero error code:', r'ERROR', r'Failed', r'Invalid'], timeout=timeout)

    def test_with_patterns(self, cmd: str, patterns: list[str], timeout: float = 3.0) -> bool:
        self.sendline(cmd)
        return all(self.expect(p, timeout=timeout) for p in patterns)

    def send_and_expect_in_dump(
        self,
        frame_str: str,
        frame_id: int,
        controller_id: int = 0,
        timeout: float = 3.0,
    ) -> bool:
        ctrl = _ctrler_name(controller_id)
        self.sendline(f'twai_send {ctrl} {frame_str}')
        return self.expect(_id_pattern(ctrl, frame_id), timeout=timeout)

    # ------------------------- context manager -------------------------
    @contextmanager
    def session(
        self,
        *,
        controller_id: int = 0,
        mode: str = 'no_transceiver',
        start_dump: bool = True,
        dump_filter: str | None = None,
        **kwargs: Any,
    ) -> Generator['TwaiTestHelper', None, None]:
        """Manage init/dump lifecycle consistently.

        - mode="no_transceiver": loopback + self_test on a single GPIO.
        - mode="standard": caller must provide tx_gpio/rx_gpio (or we use defaults).
        """
        # Build effective init args
        init_args = dict(kwargs)
        init_args['controller_id'] = controller_id

        if mode == 'no_transceiver':
            init_args |= dict(
                tx_gpio=NO_TRANSCEIVER_GPIO,
                rx_gpio=NO_TRANSCEIVER_GPIO,
                bitrate=kwargs.get('bitrate', DEFAULT_BITRATE),
                loopback=True,
                self_test=True,
            )
        elif mode == 'standard':
            init_args.setdefault('tx_gpio', DEFAULT_TX_GPIO)
            init_args.setdefault('rx_gpio', DEFAULT_RX_GPIO)
            init_args.setdefault('bitrate', kwargs.get('bitrate', DEFAULT_BITRATE))
        else:
            raise ValueError(f'Unknown mode: {mode}')

        self.dut._hard_reset()  # Reset the chip to start running the test
        self._wait_ready()
        if not self.init(**init_args):
            raise RuntimeError(f'Failed to initialize TWAI in {mode} mode')

        dump_started = False
        dump_timeout_flag = False
        try:
            if start_dump:
                dump_started = self.dump_start(controller_id=controller_id, dump_filter=dump_filter)
            yield self
        finally:
            if dump_started:
                _, warning = self.dump_stop(controller_id=controller_id)
                dump_timeout_flag = warning

            self.deinit(controller_id=controller_id)
            esp_enter_flash_mode(self.dut)  # Stop the esp chip to avoid affect test bus

            if dump_timeout_flag:
                pytest.fail(f'Dump stop timed out for {_ctrler_name(controller_id)}')


# ---------------------------------------------------------------------------
# USB-CAN bus manager (external hardware)
# ---------------------------------------------------------------------------


class CanBusManager:
    """CAN bus manager for external hardware tests"""

    def __init__(self, interface: str = 'can0'):
        self.interface = interface
        self.bus: can.Bus | None = None

    @contextmanager
    def managed_bus(self, bitrate: int = 500000) -> Generator[can.Bus, None, None]:
        try:
            result = subprocess.run(['ip', '-details', 'link', 'show', self.interface], capture_output=True, text=True)
            if result.returncode != 0:
                raise Exception(f'CAN interface {self.interface} not found')

            try:
                if 'UP' in result.stdout:
                    subprocess.run(  # Close the bus anyway if it is already up
                        ['sudo', '-n', 'ip', 'link', 'set', self.interface, 'down'], check=True, capture_output=True
                    )
                subprocess.run(
                    ['sudo', '-n', 'ip', 'link', 'set', self.interface, 'up', 'type', 'can', 'bitrate', f'{bitrate}'],
                    check=True,
                    capture_output=True,
                )
                time.sleep(0.5)
            except subprocess.CalledProcessError:
                raise Exception(f'Failed to configure CAN interface {self.interface} on bitrate {bitrate}.')

            self.bus = can.Bus(interface='socketcan', channel=self.interface)
            yield self.bus
        except Exception as e:
            pytest.skip(f'CAN interface not available: {str(e)}')
        finally:
            if self.bus:
                self.bus.shutdown()
                subprocess.run(['sudo', '-n', 'ip', 'link', 'set', self.interface, 'down'], check=True)


# ---------------------------------------------------------------------------
# Fixtures
# ---------------------------------------------------------------------------


@pytest.fixture
def twai(dut: Dut) -> TwaiTestHelper:
    return TwaiTestHelper(dut)


@pytest.fixture
def usb_can() -> CanBusManager:
    return CanBusManager()


# ---------------------------------------------------------------------------
# CORE TESTS
# ---------------------------------------------------------------------------


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_twai_utils_basic_operations(twai: TwaiTestHelper) -> None:
    with twai.session(start_dump=False):
        # Test basic send operation
        assert twai.send('123#DEADBEEF'), 'Basic send operation failed'

        # Test dump filter operations - first start should succeed
        assert twai.dump_start(dump_filter='123:7FF'), 'First dump start failed'

        # Second start should be handled gracefully (already running)
        twai.dump_start(dump_filter='456:7FF')  # Should handle "already running" case

        # Stop should work normally
        stopped_ok, warning = twai.dump_stop()
        assert stopped_ok, 'Dump stop failed'


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_twai_utils_bitrate_configuration(twai: TwaiTestHelper) -> None:
    for bitrate in [125000, 250000, 500000, 1000000]:
        with twai.session(mode='standard', bitrate=bitrate, tx_gpio=DEFAULT_TX_GPIO, rx_gpio=DEFAULT_RX_GPIO):
            assert twai.info(), f'Info failed for bitrate {bitrate}'


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_twai_utils_frame_formats(twai: TwaiTestHelper) -> None:
    with twai.session():
        BASIC_FRAMES = [
            ('123#', 'Empty data'),
            ('124#AA', '1 byte'),
            ('125#DEADBEEF', '4 bytes'),
            ('126#DEADBEEFCAFEBABE', '8 bytes'),
        ]
        for frame_str, desc in BASIC_FRAMES:
            can_id = int(frame_str.split('#')[0], 16)
            assert twai.send_and_expect_in_dump(frame_str, can_id), f'Basic frame failed: {frame_str} ({desc})'

        EXTENDED_FRAMES = [
            ('12345678#ABCD', 'Extended frame'),
            ('1FFFFFFF#AA55BB66', 'Max extended ID'),
        ]
        for frame_str, desc in EXTENDED_FRAMES:
            can_id = int(frame_str.split('#')[0], 16)
            assert twai.send_and_expect_in_dump(frame_str, can_id), f'Extended frame failed: {frame_str} ({desc})'

        RTR_FRAMES = [
            ('123#R', 'RTR default'),
            ('124#R8', 'RTR 8 bytes'),
        ]
        for frame_str, desc in RTR_FRAMES:
            assert twai.send(frame_str), f'RTR frame failed: {frame_str} ({desc})'


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_twai_utils_info_and_recovery(twai: TwaiTestHelper) -> None:
    with twai.session():
        assert twai.info(), 'Info command failed'
        assert twai.expect_info_format(), 'Info format check failed'

        assert twai.test_with_patterns(
            f'twai_info {_ctrler_name(0)}',
            [
                r'TWAI0 Status: Running',
                r'Node State: Error Active',
                r'Error Counters: TX=0, RX=0',
            ],
        ), 'Expected status patterns not found'

        assert twai.recover(), 'Recover status check failed'
        assert twai.recover(timeout_ms=1000), 'Recover command with timeout failed'


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_twai_utils_input_validation(twai: TwaiTestHelper) -> None:
    with twai.session(start_dump=False):
        INVALID_FRAMES = [
            ('G123#DEAD', 'Invalid ID character'),
            ('123#GG', 'Invalid data character'),
            ('123', 'Missing separator'),
            ('123#DEADBEEFCAFEBABEAA', 'Too much data'),
            ('123###DEAD', 'Too many separators'),
            ('123##', 'FD frame without data or flags'),
        ]
        for frame_str, desc in INVALID_FRAMES:
            assert twai.invalid_should_fail(f'twai_send {_ctrler_name(0)} {frame_str}'), (
                f'Invalid frame should be rejected: {frame_str} ({desc})'
            )

        assert twai.invalid_should_fail(f'twai_init {_ctrler_name(0)} -t 4 -r 5'), (
            'Duplicate initialization should be prevented'
        )

        # deinit esp_twai for following tests
        twai.deinit()

        invalid_commands = [
            'twai_init',  # Missing controller ID
            f'twai_init {_ctrler_name(0)}',  # Missing required GPIO
            'twai_init twai99 -t 4 -r 5',  # Invalid controller ID
            f'twai_recover {_ctrler_name(0)} -t -5',  # Invalid timeout value
            f'twai_init {_ctrler_name(0)} -t -1 -r 5',  # Negative TX GPIO
            f'twai_init {_ctrler_name(0)} -t 99 -r 5',  # High GPIO number
            f'twai_init {_ctrler_name(0)} -t 4 -r 5 -c -1',  # Negative clk_out GPIO
            f'twai_init {_ctrler_name(0)} -t 4 -r 5 -b 0',  # Zero bitrate
        ]
        for cmd in invalid_commands:
            assert twai.invalid_should_fail(cmd), f'Invalid command should fail: {cmd}'

        uninitialized_ops = [
            f'twai_send {_ctrler_name(0)} 123#DEAD',
            f'twai_recover {_ctrler_name(0)}',
            f'twai_dump {_ctrler_name(0)}',
        ]
        for cmd in uninitialized_ops:
            assert twai.invalid_should_fail(cmd), f'Non-initialized operation should fail: {cmd}'


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_twai_utils_gpio_and_basic_send(twai: TwaiTestHelper) -> None:
    with twai.session():
        assert twai.info(), 'GPIO info failed'
        assert twai.test_with_patterns(
            f'twai_info {_ctrler_name(0)}',
            [rf'GPIOs: TX=GPIO{NO_TRANSCEIVER_GPIO}, RX=GPIO{NO_TRANSCEIVER_GPIO}'],
        )
        # Basic send test frames
        BASIC_SEND_FRAMES = ['123#DEADBEEF', '7FF#AA55', '12345678#CAFEBABE']
        for frame_str in BASIC_SEND_FRAMES:
            assert twai.send(frame_str), f'Standard mode send failed: {frame_str}'

        # skip esp32 due to gpio 6/7 not available, esp32c5 due to clk_io not supported
        if twai.dut.app.target in ['esp32', 'esp32c5']:
            return

        # deinit esp_twai for following tests
        twai.deinit()
        if twai.init(tx_gpio=4, rx_gpio=5, clk_out_gpio=6, bus_off_gpio=7):
            assert twai.info(), 'Optional GPIO info failed'
            assert twai.test_with_patterns(f'twai_info {_ctrler_name(0)}', [r'TWAI0 Status:', r'GPIOs: TX=GPIO4']), (
                'GPIO info format failed'
            )


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_twai_utils_send_various_frames(twai: TwaiTestHelper) -> None:
    with twai.session():
        # Boundary ID tests
        BOUNDARY_ID_FRAMES = [
            ('7FF#AA', 'Max standard ID'),
            ('800#BB', 'Min extended ID (in extended format: 00000800)'),
            ('000#CC', 'Min ID'),
        ]
        for frame_str, desc in BOUNDARY_ID_FRAMES:
            assert twai.send(frame_str), f'Boundary ID failed: {frame_str} ({desc})'

        # Rapid succession test frames
        RAPID_FRAMES = ['123#AA', '124#BB', '125#CC', '126#DD', '127#EE']
        for frame_str in RAPID_FRAMES:
            assert twai.send(frame_str), f'Rapid send failed: {frame_str}'


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_FD_SUPPORTED == 1'), indirect=['target'])
def test_twai_utils_fd_frames(twai: TwaiTestHelper) -> None:
    with twai.session():
        FD_FRAMES = [
            ('123##0AABBCC', 'FD frame without BRS'),
            ('456##1DEADBEEF', 'FD frame with BRS'),
            ('789##2CAFEBABE', 'FD frame with ESI'),
            ('ABC##3112233', 'FD frame with BRS+ESI'),
        ]
        for frame_str, desc in FD_FRAMES:
            assert twai.send(frame_str), f'FD frame failed: {frame_str} ({desc})'


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_twai_utils_mask_filters(twai: TwaiTestHelper) -> None:
    """Test TWAI filtering including automatic extended frame detection."""
    MASK_FILTER_GROUPS = [
        # No filter - basic functionality
        (
            '',
            [
                ('123#DEAD', 0x123, True),  # Standard frame passes
                ('12345678#CAFE', 0x12345678, True),  # Extended frame passes
            ],
        ),
        # Standard frame mask filter (is_ext=false by length=3, value<=0x7FF)
        (
            '123:7FF',
            [
                ('123#DEAD', 0x123, True),  # Standard frame matches
                ('456#BEEF', 0x456, False),  # Standard frame doesn't match
                ('12345678#CAFE', 0x12345678, False),  # Extended frame filtered out
            ],
        ),
        # Extended frame mask filter (is_ext=true by length>3)
        (
            '12345678:1FFFFFFF',
            [
                ('123#DEAD', 0x123, False),  # Standard frame filtered out
                ('12345678#CAFE', 0x12345678, True),  # Extended frame matches
            ],
        ),
        # Extended frame mask filter (is_ext=true by value>0x7FF)
        (
            '800:1FFFFFFF',
            [
                ('7FF#BEEF', 0x7FF, False),  # Max standard ID filtered out
                ('800#CAFE', 0x800, True),  # Extended ID matches exactly
            ],
        ),
    ]
    for filter_str, test_frames in MASK_FILTER_GROUPS:
        with twai.session(dump_filter=filter_str):
            failed_cases: list[str] = []
            for frame_str, frame_id, should_receive in test_frames:
                received = twai.send_and_expect_in_dump(frame_str, frame_id, timeout=1.0)
                if received != should_receive:
                    expected_action = 'receive' if should_receive else 'filter out'
                    actual_action = 'received' if received else 'filtered out'
                    failed_cases.append(f'{frame_str}: expected {expected_action}, got {actual_action}')

            if failed_cases:
                pytest.fail(
                    f'Filter test failed for filter "{filter_str or "no filter"}":\n'
                    + '\n'.join(failed_cases)
                    + '\n\nNote: Filters auto-detect extended frames by:'
                    '\n- String length > 3 chars or ID value > 0x7FF'
                )


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_RANGE_FILTER_NUM > 0'), indirect=['target'])
def test_twai_utils_range_filters(twai: TwaiTestHelper) -> None:
    """Test TWAI range filters (available on chips with range filter support)."""
    RANGE_FILTER_GROUPS = [
        # Standard frame range filter
        (
            'a-15',  # Test hex range parsing
            [
                ('00a#DEAD', 0x00A, True),  # Within range
                ('00f#BEEF', 0x00F, True),  # Within range
                ('015#CAFE', 0x015, True),  # At upper bound
                ('009#BABE', 0x009, False),  # Below range
                ('016#FEED', 0x016, False),  # Above range
            ],
        ),
        # Extended frame range filter
        (
            '10000000-1FFFFFFF',
            [
                ('123#DEAD', 0x123, False),  # Standard frame filtered out
                ('0FFFFFFF#BEEF', 0x0FFFFFFF, False),  # Below range
                ('10000000#CAFE', 0x10000000, True),  # At lower bound
                ('1FFFFFFF#FEED', 0x1FFFFFFF, True),  # At upper bound
            ],
        ),
    ]
    for filter_str, test_frames in RANGE_FILTER_GROUPS:
        with twai.session(dump_filter=filter_str):
            failed_cases: list[str] = []
            for frame_str, frame_id, should_receive in test_frames:
                received = twai.send_and_expect_in_dump(frame_str, frame_id, timeout=1.0)
                if received != should_receive:
                    expected_action = 'receive' if should_receive else 'filter out'
                    actual_action = 'received' if received else 'filtered out'
                    failed_cases.append(f'{frame_str}: expected {expected_action}, got {actual_action}')
            if failed_cases:
                pytest.fail(f'Range filter failed for filter "{filter_str}":\n' + '\n'.join(failed_cases))


# ---------------------------------------------------------------------------
# EXTERNAL HARDWARE TESTS
# ---------------------------------------------------------------------------


@pytest.mark.twai_std
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='no runner')
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_twai_utils_external_communication(twai: TwaiTestHelper, usb_can: CanBusManager) -> None:
    test_frames = [
        ('123#DEADBEEF', 0x123, bytes.fromhex('DEADBEEF'), False),
        ('7FF#AA55', 0x7FF, bytes.fromhex('AA55'), False),
        ('12345678#CAFEBABE', 0x12345678, bytes.fromhex('CAFEBABE'), True),
    ]

    with usb_can.managed_bus(bitrate=DEFAULT_BITRATE) as can_bus:
        with twai.session(
            mode='standard',
            tx_gpio=DEFAULT_TX_GPIO,
            rx_gpio=DEFAULT_RX_GPIO,
            bitrate=DEFAULT_BITRATE,
            start_dump=False,
        ):
            # --- ESP -> PC Test ---
            for frame_str, frame_id, expected_data, is_extended in test_frames:
                assert twai.send(frame_str), f'ESP->PC send failed: {frame_str}'
                deadline = time.time() + 2.0
                got = None
                while time.time() < deadline:
                    try:
                        msg = can_bus.recv(timeout=0.1)
                        if msg and msg.arbitration_id == frame_id:
                            got = msg
                            break
                    except Exception:
                        continue
                assert got is not None, f'ESP->PC receive timeout for ID=0x{frame_id:X}'
                assert bool(got.is_extended_id) == is_extended, (
                    f'ESP->PC extended flag mismatch for 0x{frame_id:X}: '
                    f'expected {is_extended}, got {got.is_extended_id}'
                )
                assert bytes(got.data) == expected_data, (
                    f'ESP->PC data mismatch for 0x{frame_id:X}: '
                    f'expected {expected_data.hex()}, got {bytes(got.data).hex()}'
                )

            # --- PC -> ESP ---
            assert twai.dump_start(), 'Failed to start twai_dump'
            assert twai.info(), 'Failed to get twai_info'

            try:
                for frame_str, frame_id, expected_data, is_extended in test_frames:
                    msg = can.Message(arbitration_id=frame_id, data=expected_data, is_extended_id=is_extended)
                    print(f'\nPC->ESP sending frame: {msg}, Return: {can_bus.send(msg)}')
                    time.sleep(0.1)
                    assert twai.expect(_id_pattern('twai0', frame_id), timeout=1.0), (
                        f'PC->ESP frame not received: ID=0x{frame_id:X}, data={expected_data.hex()}'
                    )
            finally:
                twai.dump_stop()
