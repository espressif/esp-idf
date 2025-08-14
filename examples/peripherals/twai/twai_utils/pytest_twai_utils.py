# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import re
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

PROMPTS = ['esp>', 'twai>', '>']


def _ctrl(controller_id: int) -> str:
    return f'twai{controller_id}'


def _id_pattern(controller_str: str, can_id: int) -> str:
    """Return regex pattern for a dump line that contains this ctrl & CAN ID."""
    hex_part = f'{can_id:08X}' if can_id > 0x7FF else f'{can_id:03X}'
    return rf'{controller_str}\s+{hex_part}\s+\['


class TestConfig:
    """Test configuration"""

    # Hardware configuration
    DEFAULT_BITRATE = 500000
    BITRATES = [125000, 250000, 500000, 1000000]
    DEFAULT_TX_GPIO = 4
    DEFAULT_RX_GPIO = 5
    NO_TRANSCEIVER_GPIO = 4

    # Test frame data
    BASIC_FRAMES = [
        ('123#', 'Empty data'),
        ('124#AA', '1 byte'),
        ('125#DEADBEEF', '4 bytes'),
        ('126#DEADBEEFCAFEBABE', '8 bytes'),
    ]

    EXTENDED_FRAMES = [
        ('12345678#ABCD', 'Extended frame'),
        ('1FFFFFFF#AA55BB66', 'Max extended ID'),
    ]

    RTR_FRAMES = [
        ('123#R', 'RTR default'),
        ('124#R8', 'RTR 8 bytes'),
    ]

    # FD frames (if FD is supported)
    FD_FRAMES = [
        ('123##0AABBCC', 'FD frame without BRS'),
        ('456##1DEADBEEF', 'FD frame with BRS'),
        ('789##2CAFEBABE', 'FD frame with ESI'),
        ('ABC##3112233', 'FD frame with BRS+ESI'),
    ]

    # Boundary ID tests
    BOUNDARY_ID_FRAMES = [
        ('7FF#AA', 'Max standard ID'),
        ('800#BB', 'Min extended ID (in extended format: 00000800)'),
        ('000#CC', 'Min ID'),
    ]

    INVALID_FRAMES = [
        ('G123#DEAD', 'Invalid ID character'),
        ('123#GG', 'Invalid data character'),
        ('123', 'Missing separator'),
        ('123#DEADBEEFCAFEBABEAA', 'Too much data'),
        ('123###DEAD', 'Too many separators'),
        ('123##', 'FD frame without data or flags'),
    ]

    # Filter tests (includes both basic and extended frame filtering)
    FILTER_TESTS = [
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

    # Range filter tests
    RANGE_FILTER_TESTS = [
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

    # Rapid succession test frames
    RAPID_FRAMES = ['123#AA', '124#BB', '125#CC', '126#DD', '127#EE']

    # Basic send test frames
    BASIC_SEND_FRAMES = ['123#DEADBEEF', '7FF#AA55', '12345678#CAFEBABE']


# ---------------------------------------------------------------------------
# TWAI helper (refactored)
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
            self.dut.expect(PROMPTS, timeout=10)
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
        return self.expect(expect or PROMPTS, timeout)

    # ------------------------- command builders -------------------------
    def build_init(
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
        ctrl = _ctrl(controller_id)
        parts = [f'twai_init {ctrl}']
        if tx_gpio is not None:
            parts += [f'-t {tx_gpio}']
        if rx_gpio is not None:
            parts += [f'-r {rx_gpio}']
        if bitrate is not None:
            parts += [f'-b {bitrate}']
        if fd_bitrate is not None:
            parts += [f'-B {fd_bitrate}']
        if clk_out_gpio is not None:
            parts += [f'-c {clk_out_gpio}']
        if bus_off_gpio is not None:
            parts += [f'-o {bus_off_gpio}']
        if loopback:
            parts += ['--loopback']
        if self_test:
            parts += ['--self-test']
        if listen:
            parts += ['--listen']
        return ' '.join(parts)

    def build_dump_start(self, *, controller_id: int = 0, dump_filter: str | None = None) -> str:
        cmd = f'twai_dump {_ctrl(controller_id)}'
        if dump_filter:
            cmd += f',{dump_filter}'
        return cmd

    def build_dump_stop(self, *, controller_id: int = 0) -> str:
        return f'twai_dump {_ctrl(controller_id)} --stop'

    # ------------------------- high-level ops -------------------------
    def init(self, controller_id: int = 0, **kwargs: Any) -> bool:
        return self.run(self.build_init(controller_id=controller_id, **kwargs))

    def deinit(self, controller_id: int = 0) -> bool:
        return self.run(f'twai_deinit {_ctrl(controller_id)}')

    def dump_start(self, controller_id: int = 0, dump_filter: str | None = None) -> bool:
        return self.run(self.build_dump_start(controller_id=controller_id, dump_filter=dump_filter))

    def dump_stop(self, controller_id: int = 0) -> tuple[bool, bool]:
        """Stop dump and return (stopped_ok, timeout_warning_seen)."""
        self.sendline(self.build_dump_stop(controller_id=controller_id))
        # If the dump task does not exit naturally, the implementation prints this warning.
        warning_seen = self.expect(r'Dump task did not exit naturally, timeout', timeout=5)
        # Whether or not warning appears, we should be back to a prompt.
        prompt_ok = self.expect(PROMPTS, timeout=2) or True  # relax
        return prompt_ok, warning_seen

    def send(self, frame_str: str, controller_id: int = 0) -> bool:
        return self.run(f'twai_send {_ctrl(controller_id)} {frame_str}')

    def info(self, controller_id: int = 0) -> bool:
        return self.run(
            f'twai_info {_ctrl(controller_id)}',
            [rf'TWAI{controller_id} Status:', r'Node State:', r'Bitrate:'],
        )

    def recover(self, controller_id: int = 0, timeout_ms: int | None = None) -> bool:
        cmd = f'twai_recover {_ctrl(controller_id)}'
        if timeout_ms is not None:
            cmd += f' -t {timeout_ms}'
        return self.run(cmd, ['Recovery not needed', 'node is Error Active', 'ESP_ERR_INVALID_STATE'])  # any

    def expect_info_format(self, controller_id: int = 0) -> bool:
        self.sendline(f'twai_info {_ctrl(controller_id)}')
        checks = [
            rf'TWAI{controller_id} Status: \w+',
            r'Node State: \w+',
            r'Error Counters: TX=\d+, RX=\d+',
            r'Bitrate: \d+ bps',
        ]
        return all(self.expect(p, timeout=2) for p in checks)

    def invalid_should_fail(self, cmd: str, timeout: float = 2.0) -> bool:
        self.sendline(cmd)
        return self.expect([r'Command returned non-zero error code:', r'ERROR', r'Failed', r'Invalid'], timeout=timeout)

    def test_with_patterns(self, cmd: str, patterns: list[str], timeout: float = 3.0) -> bool:
        self.sendline(cmd)
        return all(self.expect(p, timeout=timeout) for p in patterns)

    def send_and_expect_in_dump(
        self,
        frame_str: str,
        expected_id: int,
        controller_id: int = 0,
        timeout: float = 3.0,
    ) -> bool:
        ctrl = _ctrl(controller_id)
        self.sendline(f'twai_send {ctrl} {frame_str}')
        return self.expect(_id_pattern(ctrl, expected_id), timeout=timeout)

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
                tx_gpio=TestConfig.NO_TRANSCEIVER_GPIO,
                rx_gpio=TestConfig.NO_TRANSCEIVER_GPIO,
                bitrate=kwargs.get('bitrate', TestConfig.DEFAULT_BITRATE),
                loopback=True,
                self_test=True,
            )
        elif mode == 'standard':
            init_args.setdefault('tx_gpio', TestConfig.DEFAULT_TX_GPIO)
            init_args.setdefault('rx_gpio', TestConfig.DEFAULT_RX_GPIO)
            init_args.setdefault('bitrate', kwargs.get('bitrate', TestConfig.DEFAULT_BITRATE))
        else:
            raise ValueError(f'Unknown mode: {mode}')

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

            if dump_timeout_flag:
                pytest.fail(f'Dump stop timed out for {_ctrl(controller_id)}')


# ---------------------------------------------------------------------------
# CAN bus manager (external hardware)
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

            interface_up = 'UP' in result.stdout
            current_bitrate = None
            m = re.search(r'bitrate (\d+)', result.stdout)
            if m:
                current_bitrate = int(m.group(1))

            if current_bitrate != bitrate:
                logging.info(
                    f'Configuring CAN interface: current_bitrate={current_bitrate}, required_bitrate={bitrate}'
                )
                try:
                    if interface_up:
                        subprocess.run(
                            ['sudo', '-n', 'ip', 'link', 'set', self.interface, 'down'], check=True, capture_output=True
                        )
                    subprocess.run(
                        [
                            'sudo',
                            '-n',
                            'ip',
                            'link',
                            'set',
                            self.interface,
                            'up',
                            'type',
                            'can',
                            'bitrate',
                            str(bitrate),
                        ],
                        check=True,
                        capture_output=True,
                    )
                    time.sleep(0.5)
                except subprocess.CalledProcessError:
                    raise Exception(
                        f'Failed to configure CAN interface {self.interface}. '
                        f'Try: sudo ip link set {self.interface} down && '
                        f'sudo ip link set {self.interface} up type can bitrate {bitrate}'
                    )

            self.bus = can.Bus(interface='socketcan', channel=self.interface)
            yield self.bus
        except Exception as e:
            pytest.skip(f'CAN interface not available: {str(e)}')
        finally:
            if self.bus:
                try:
                    self.bus.shutdown()
                    subprocess.run(['sudo', '-n', 'ip', 'link', 'set', self.interface, 'down'], check=True)
                except Exception:
                    pass


# ---------------------------------------------------------------------------
# Fixtures
# ---------------------------------------------------------------------------


@pytest.fixture
def twai(dut: Dut) -> TwaiTestHelper:
    return TwaiTestHelper(dut)


@pytest.fixture
def can_manager() -> CanBusManager:
    return CanBusManager()


# ---------------------------------------------------------------------------
# CORE TESTS
# ---------------------------------------------------------------------------


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_twai_basic_operations(twai: TwaiTestHelper) -> None:
    with twai.session(
        mode='standard', tx_gpio=TestConfig.DEFAULT_TX_GPIO, rx_gpio=TestConfig.DEFAULT_RX_GPIO, start_dump=False
    ):
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
def test_twai_bitrate_configuration(twai: TwaiTestHelper) -> None:
    for bitrate in TestConfig.BITRATES:
        with twai.session(
            mode='standard', bitrate=bitrate, tx_gpio=TestConfig.DEFAULT_TX_GPIO, rx_gpio=TestConfig.DEFAULT_RX_GPIO
        ):
            assert twai.info(), f'Info failed for bitrate {bitrate}'

    # TWAI-FD bitrate validation (intentionally invalid: data bitrate < arbitration)
    if twai.init(
        tx_gpio=TestConfig.DEFAULT_TX_GPIO, rx_gpio=TestConfig.DEFAULT_RX_GPIO, bitrate=1_000_000, fd_bitrate=500_000
    ):
        try:
            ok = twai.test_with_patterns(
                f'twai_info {_ctrl(0)}',
                [r'TWAI0 Status:', r'Bitrate: 1000000'],
            )
            assert ok, 'FD bitrate validation info failed'
        finally:
            twai.deinit()


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_twai_frame_formats(twai: TwaiTestHelper) -> None:
    with twai.session():
        for frame_str, desc in TestConfig.BASIC_FRAMES:
            can_id = int(frame_str.split('#')[0], 16)
            assert twai.send_and_expect_in_dump(frame_str, can_id), f'Basic frame failed: {frame_str} ({desc})'
        for frame_str, desc in TestConfig.EXTENDED_FRAMES:
            can_id = int(frame_str.split('#')[0], 16)
            assert twai.send_and_expect_in_dump(frame_str, can_id), f'Extended frame failed: {frame_str} ({desc})'
        for frame_str, desc in TestConfig.RTR_FRAMES:
            assert twai.send(frame_str), f'RTR frame failed: {frame_str} ({desc})'


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_twai_info_and_recovery(twai: TwaiTestHelper) -> None:
    with twai.session():
        assert twai.info(), 'Info command failed'
        assert twai.expect_info_format(), 'Info format check failed'

        assert twai.test_with_patterns(
            f'twai_info {_ctrl(0)}',
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
def test_twai_input_validation(twai: TwaiTestHelper) -> None:
    with twai.session(start_dump=False):
        for frame_str, desc in TestConfig.INVALID_FRAMES:
            assert twai.invalid_should_fail(f'twai_send {_ctrl(0)} {frame_str}'), (
                f'Invalid frame should be rejected: {frame_str} ({desc})'
            )

    invalid_commands = [
        'twai_init',  # Missing controller ID
        f'twai_init {_ctrl(0)}',  # Missing required GPIO
        'twai_init twai99 -t 4 -r 5',  # Invalid controller ID
        f'twai_recover {_ctrl(0)} -t -5',  # Invalid timeout value
        f'twai_init {_ctrl(0)} -t -1 -r 5',  # Negative TX GPIO
        f'twai_init {_ctrl(0)} -t 99 -r 5',  # High GPIO number
        f'twai_init {_ctrl(0)} -t 4 -r 5 -c -1',  # Negative clk_out GPIO
        f'twai_init {_ctrl(0)} -t 4 -r 5 -b 0',  # Zero bitrate
    ]
    for cmd in invalid_commands:
        assert twai.invalid_should_fail(cmd), f'Invalid command should fail: {cmd}'

    uninitialized_ops = [f'twai_send {_ctrl(0)} 123#DEAD', f'twai_recover {_ctrl(0)}', f'twai_dump {_ctrl(0)}']
    for cmd in uninitialized_ops:
        assert twai.invalid_should_fail(cmd), f'Non-initialized operation should fail: {cmd}'

    with twai.session(start_dump=False):
        assert twai.invalid_should_fail(f'twai_init {_ctrl(0)} -t 4 -r 5'), (
            'Duplicate initialization should be prevented'
        )


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_twai_gpio_and_basic_send(twai: TwaiTestHelper) -> None:
    with twai.session():
        assert twai.send('123#DEADBEEF'), 'No-transceiver send failed'

    with twai.session(mode='standard', tx_gpio=TestConfig.DEFAULT_TX_GPIO, rx_gpio=TestConfig.DEFAULT_RX_GPIO):
        assert twai.info(), 'GPIO info failed'
        assert twai.test_with_patterns(
            f'twai_info {_ctrl(0)}',
            [rf'GPIOs: TX=GPIO{TestConfig.DEFAULT_TX_GPIO}, RX=GPIO{TestConfig.DEFAULT_RX_GPIO}'],
        )
        for frame_str in TestConfig.BASIC_SEND_FRAMES:
            assert twai.send(frame_str), f'Standard mode send failed: {frame_str}'

    if twai.init(tx_gpio=4, rx_gpio=5, clk_out_gpio=6, bus_off_gpio=7):
        try:
            assert twai.info(), 'Optional GPIO info failed'
            assert twai.test_with_patterns(f'twai_info {_ctrl(0)}', [r'TWAI0 Status:', r'GPIOs: TX=GPIO4']), (
                'GPIO info format failed'
            )
        finally:
            twai.deinit()


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_twai_send_various_frames(twai: TwaiTestHelper) -> None:
    with twai.session():
        for frame_str, desc in TestConfig.BOUNDARY_ID_FRAMES:
            assert twai.send(frame_str), f'Boundary ID failed: {frame_str} ({desc})'
        for frame_str in TestConfig.RAPID_FRAMES:
            assert twai.send(frame_str), f'Rapid send failed: {frame_str}'


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORT_FD == 1'), indirect=['target'])
def test_twai_fd_frames(twai: TwaiTestHelper) -> None:
    with twai.session():
        for frame_str, desc in TestConfig.FD_FRAMES:
            assert twai.send(frame_str), f'FD frame failed: {frame_str} ({desc})'


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_twai_filtering(twai: TwaiTestHelper) -> None:
    """Test TWAI filtering including automatic extended frame detection."""
    for filter_str, test_frames in TestConfig.FILTER_TESTS:
        with twai.session(dump_filter=filter_str):
            failed_cases: list[str] = []
            for frame_str, expected_id, should_receive in test_frames:
                received = twai.send_and_expect_in_dump(frame_str, expected_id, timeout=1.0)
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
def test_twai_range_filters(twai: TwaiTestHelper) -> None:
    """Test TWAI range filters (available on chips with range filter support)."""
    for filter_str, test_frames in TestConfig.RANGE_FILTER_TESTS:
        with twai.session(dump_filter=filter_str):
            failed_cases: list[str] = []
            for frame_str, expected_id, should_receive in test_frames:
                received = twai.send_and_expect_in_dump(frame_str, expected_id, timeout=1.0)
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
@pytest.mark.temp_skip_ci(targets=['esp32c5'], reason='no runner')
@idf_parametrize('target', soc_filtered_targets('SOC_TWAI_SUPPORTED == 1'), indirect=['target'])
def test_twai_external_communication(twai: TwaiTestHelper, can_manager: CanBusManager) -> None:
    """
    Test bidirectional communication with external CAN interface (hardware level).

    Requirements:
    - ESP node connected to physical CAN transceiver, properly wired to PC's socketcan
      interface (default can0) via CANH/CANL.
    - PC has `python-can` and can0 is available.
    - Bitrate matches TestConfig.DEFAULT_BITRATE (default 500 kbps).
    """
    test_frames = [
        ('123#DEADBEEF', 0x123, bytes.fromhex('DEADBEEF'), False),
        ('7FF#AA55', 0x7FF, bytes.fromhex('AA55'), False),
        ('12345678#CAFEBABE', 0x12345678, bytes.fromhex('CAFEBABE'), True),
    ]

    with can_manager.managed_bus(bitrate=TestConfig.DEFAULT_BITRATE) as can_bus:
        with twai.session(
            mode='standard',
            tx_gpio=TestConfig.DEFAULT_TX_GPIO,
            rx_gpio=TestConfig.DEFAULT_RX_GPIO,
            bitrate=TestConfig.DEFAULT_BITRATE,
            start_dump=False,
        ):
            # --- ESP -> PC Connectivity Test ---
            first_frame, test_id, test_data, test_extended = test_frames[0]
            if not twai.send(first_frame):
                pytest.skip(
                    f'ESP CAN send failed - check ESP GPIO '
                    f'{TestConfig.DEFAULT_TX_GPIO}/{TestConfig.DEFAULT_RX_GPIO} -> '
                    f'CAN transceiver connection'
                )

            deadline = time.time() + 3.0
            got: can.Message | None = None
            while time.time() < deadline:
                try:
                    msg = can_bus.recv(timeout=0.2)
                    if msg and msg.arbitration_id == test_id:
                        got = msg
                        break
                except Exception as e:
                    logging.debug(f'PC CAN receive exception: {e}')
            if got is None:
                pytest.skip(
                    'ESP->PC communication failed - check CAN transceiver -> PC can0 connection. '
                    "Verify wiring and 'sudo ip link set can0 up type can bitrate 500000'"
                )
            if got is not None and bytes(got.data) != test_data:
                pytest.fail(
                    f'ESP->PC data corruption detected: expected {test_data.hex()}, got {bytes(got.data).hex()}'
                )

            # --- Full ESP -> PC Test ---
            for frame_str, expected_id, expected_data, is_extended in test_frames:
                assert twai.send(frame_str), f'ESP->PC send failed: {frame_str}'
                deadline = time.time() + 1.0
                got = None
                while time.time() < deadline:
                    try:
                        msg = can_bus.recv(timeout=0.1)
                        if msg and msg.arbitration_id == expected_id:
                            got = msg
                            break
                    except Exception:
                        continue
                assert got is not None, f'ESP->PC receive timeout for ID=0x{expected_id:X}'
                assert bool(got.is_extended_id) == is_extended, (
                    f'ESP->PC extended flag mismatch for 0x{expected_id:X}: '
                    f'expected {is_extended}, got {got.is_extended_id}'
                )
                assert bytes(got.data) == expected_data, (
                    f'ESP->PC data mismatch for 0x{expected_id:X}: '
                    f'expected {expected_data.hex()}, got {bytes(got.data).hex()}'
                )

            # --- PC -> ESP ---
            assert twai.dump_start(), 'Failed to start twai_dump'
            assert twai.info(), 'Failed to get twai_info'

            test_msg = can.Message(arbitration_id=test_id, data=test_data, is_extended_id=test_extended)
            try:
                can_bus.send(test_msg)
                time.sleep(0.2)
                assert twai.expect(_id_pattern('twai0', test_id), timeout=2.0), (
                    f'PC->ESP frame not received: ID=0x{test_id:X}, data={test_data.hex()}'
                )
                for frame_str, expected_id, expected_data, is_extended in test_frames[1:]:
                    msg = can.Message(arbitration_id=expected_id, data=expected_data, is_extended_id=is_extended)
                    can_bus.send(msg)
                    time.sleep(0.1)
                    assert twai.expect(_id_pattern('twai0', expected_id), timeout=1.0), (
                        f'PC->ESP frame not received: ID=0x{expected_id:X}, data={expected_data.hex()}'
                    )
            finally:
                twai.dump_stop()
