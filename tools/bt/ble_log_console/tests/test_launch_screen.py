# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from pathlib import Path
from unittest.mock import MagicMock
from unittest.mock import patch

from src.backend.models import LaunchConfig
from src.frontend.launch_screen import BAUD_RATES
from src.frontend.launch_screen import DEFAULT_BAUD_RATE
from src.frontend.launch_screen import LaunchScreen

# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------


class TestBaudRateConstants:
    def test_baud_rates_is_list_of_ints(self) -> None:
        assert isinstance(BAUD_RATES, list)
        assert all(isinstance(b, int) for b in BAUD_RATES)

    def test_baud_rates_not_empty(self) -> None:
        assert len(BAUD_RATES) > 0

    def test_baud_rates_ascending(self) -> None:
        assert BAUD_RATES == sorted(BAUD_RATES)

    def test_default_baud_rate_in_list(self) -> None:
        assert DEFAULT_BAUD_RATE in BAUD_RATES

    def test_default_baud_rate_value(self) -> None:
        assert DEFAULT_BAUD_RATE == 3_000_000

    def test_common_rates_present(self) -> None:
        """Standard UART baud rates used by ESP-IDF should be available."""
        assert 115200 in BAUD_RATES
        assert 921600 in BAUD_RATES


# ---------------------------------------------------------------------------
# LaunchConfig dataclass
# ---------------------------------------------------------------------------


class TestLaunchConfig:
    def test_create_with_required_fields(self) -> None:
        cfg = LaunchConfig(port='/dev/ttyUSB0', baudrate=3000000, log_dir=Path('/tmp'))
        assert cfg.port == '/dev/ttyUSB0'
        assert cfg.baudrate == 3000000
        assert cfg.log_dir == Path('/tmp')

    def test_different_ports(self) -> None:
        for port in ['/dev/ttyUSB0', '/dev/ttyACM0', 'COM3', '/dev/tty.usbserial-1420']:
            cfg = LaunchConfig(port=port, baudrate=115200, log_dir=Path('.'))
            assert cfg.port == port

    def test_various_baud_rates(self) -> None:
        for baud in BAUD_RATES:
            cfg = LaunchConfig(port='/dev/ttyUSB0', baudrate=baud, log_dir=Path('.'))
            assert cfg.baudrate == baud

    def test_log_dir_is_path(self) -> None:
        cfg = LaunchConfig(port='COM1', baudrate=115200, log_dir=Path('/var/log'))
        assert isinstance(cfg.log_dir, Path)


# ---------------------------------------------------------------------------
# LaunchScreen instantiation
# ---------------------------------------------------------------------------


class TestLaunchScreenInit:
    def test_default_log_dir_is_cwd(self) -> None:
        screen = LaunchScreen()
        assert screen._default_log_dir == Path.cwd()

    def test_custom_log_dir(self) -> None:
        custom = Path('/tmp/my_logs')
        screen = LaunchScreen(default_log_dir=custom)
        assert screen._default_log_dir == custom

    def test_none_log_dir_falls_back_to_cwd(self) -> None:
        screen = LaunchScreen(default_log_dir=None)
        assert screen._default_log_dir == Path.cwd()

    def test_is_screen_subclass(self) -> None:
        from textual.screen import Screen

        assert issubclass(LaunchScreen, Screen)

    def test_bindings_include_quit(self) -> None:
        """LaunchScreen should have a quit binding on 'q'."""
        keys = [b.key for b in LaunchScreen.BINDINGS]
        assert 'q' in keys


# ---------------------------------------------------------------------------
# refresh_ports — unit-level (mocked widgets)
# ---------------------------------------------------------------------------


class TestRefreshPorts:
    @patch('src.frontend.launch_screen.list_serial_ports')
    def test_refresh_updates_select_with_ports(self, mock_lsp: MagicMock) -> None:
        """refresh_ports should scan ports and update the Select widget."""
        mock_lsp.return_value = ['/dev/ttyUSB0', '/dev/ttyUSB1']
        screen = LaunchScreen()

        mock_select = MagicMock()
        screen.query_one = MagicMock(return_value=mock_select)  # type: ignore[method-assign]

        screen.refresh_ports()

        mock_lsp.assert_called_once()
        mock_select.set_options.assert_called_once_with(
            [('/dev/ttyUSB0', '/dev/ttyUSB0'), ('/dev/ttyUSB1', '/dev/ttyUSB1')]
        )
        assert mock_select.value == '/dev/ttyUSB0'

    @patch('src.frontend.launch_screen.list_serial_ports')
    def test_refresh_empty_ports_no_value_set(self, mock_lsp: MagicMock) -> None:
        """When no ports found, set_options is called with empty list and value is not set."""
        mock_lsp.return_value = []
        screen = LaunchScreen()

        mock_select = MagicMock()
        screen.query_one = MagicMock(return_value=mock_select)  # type: ignore[method-assign]

        screen.refresh_ports()

        mock_select.set_options.assert_called_once_with([])
        # value should NOT have been reassigned when ports list is empty
        assert mock_select.value != '/dev/ttyUSB0'


# ---------------------------------------------------------------------------
# connect — unit-level (mocked widgets)
# ---------------------------------------------------------------------------


class TestConnect:
    def _make_screen_with_mocks(
        self,
        port_value: object,
        baud_value: int = 3000000,
        dir_value: str = '/tmp/logs',
    ) -> tuple[LaunchScreen, MagicMock, MagicMock, MagicMock]:
        """Helper: create a LaunchScreen with mocked query_one results."""
        screen = LaunchScreen()

        mock_port_select = MagicMock()
        mock_port_select.value = port_value

        mock_baud_select = MagicMock()
        mock_baud_select.value = baud_value

        mock_dir_input = MagicMock()
        mock_dir_input.value = dir_value

        def fake_query_one(selector: str, widget_type: type = object) -> MagicMock:
            if selector == '#port-select':
                return mock_port_select
            if selector == '#baud-select':
                return mock_baud_select
            if selector == '#dir-input':
                return mock_dir_input
            raise ValueError(f'Unexpected selector: {selector}')

        screen.query_one = fake_query_one  # type: ignore[assignment]
        screen.dismiss = MagicMock()  # type: ignore[method-assign]
        screen.notify = MagicMock()  # type: ignore[method-assign]

        return screen, mock_port_select, mock_baud_select, mock_dir_input

    def test_connect_with_valid_port(self) -> None:
        """connect() should dismiss with LaunchConfig when port is selected."""
        screen, _, _, _ = self._make_screen_with_mocks(
            port_value='/dev/ttyUSB0',
            baud_value=921600,
            dir_value='/tmp/logs',
        )

        screen.connect()

        screen.dismiss.assert_called_once()
        config = screen.dismiss.call_args[0][0]
        assert isinstance(config, LaunchConfig)
        assert config.port == '/dev/ttyUSB0'
        assert config.baudrate == 921600
        assert config.log_dir == Path('/tmp/logs')

    def test_connect_with_blank_port_shows_error(self) -> None:
        """connect() should notify error and NOT dismiss when port is BLANK."""
        from textual.widgets import Select

        screen, _, _, _ = self._make_screen_with_mocks(port_value=Select.BLANK)

        screen.connect()

        screen.notify.assert_called_once_with('Please select a serial port', severity='error')
        screen.dismiss.assert_not_called()

    def test_connect_log_dir_is_path_object(self) -> None:
        """The log_dir in LaunchConfig should be a Path, not a string."""
        screen, _, _, _ = self._make_screen_with_mocks(port_value='COM3', dir_value='/home/user/logs')

        screen.connect()

        config = screen.dismiss.call_args[0][0]
        assert isinstance(config.log_dir, Path)
        assert str(config.log_dir) == '/home/user/logs'


# ---------------------------------------------------------------------------
# action_quit
# ---------------------------------------------------------------------------


class TestActionQuit:
    def test_action_quit_dismisses_with_none(self) -> None:
        screen = LaunchScreen()
        screen.dismiss = MagicMock()  # type: ignore[method-assign]

        screen.action_quit()

        screen.dismiss.assert_called_once_with(None)


# ---------------------------------------------------------------------------
# compose — structural checks (no App context required)
# ---------------------------------------------------------------------------


class TestComposeMethod:
    def test_compose_is_defined(self) -> None:
        """LaunchScreen.compose should be a callable method."""
        assert callable(getattr(LaunchScreen, 'compose', None))

    def test_default_css_contains_expected_ids(self) -> None:
        """DEFAULT_CSS should reference the widget IDs used in compose."""
        css = LaunchScreen.DEFAULT_CSS
        for widget_id in [
            'launch-container',
            'launch-title',
            'port-select',
            'refresh-btn',
            'dir-input',
            'browse-btn',
            'connect-btn',
            'no-ports-label',
        ]:
            assert widget_id in css, f'Missing CSS rule for #{widget_id}'

    def test_baud_options_built_correctly(self) -> None:
        """Verify the baud option tuples match the expected (label, value) shape."""
        baud_options = [(str(b), b) for b in BAUD_RATES]
        assert all(isinstance(label, str) and isinstance(val, int) for label, val in baud_options)
        assert len(baud_options) == len(BAUD_RATES)
