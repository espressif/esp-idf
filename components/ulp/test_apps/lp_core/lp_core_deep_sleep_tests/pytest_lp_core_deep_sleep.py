# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from lp_unity import TwoBoardSleep
from pytest_embedded_idf import CaseTester
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

# HP prints SLEEPER_SLEEP, then spends ~50 ms entering deep sleep.
SLEEP_ENTRY_DELAY_S = 0.3
# Match the LP GPIO wakeup test: the edge is sent only after sleep has settled.
GPIO_EDGE_DELAY_S = 2.0
# LP timer is 500 ms. Wait until main() is running before the peer toggles.
LP_RUNNING_DELAY_S = 0.8


@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize('config', ['defaults'], indirect=True)
@idf_parametrize(
    'target',
    soc_filtered_targets(
        '(SOC_LP_CORE_SUPPORTED == 1) and (SOC_LP_I2C_SUPPORTED == 1) and '
        '(SOC_DEEP_SLEEP_SUPPORTED == 1) and (SOC_RTC_TIMER_SUPPORTED == 1)'
    ),
    indirect=['target'],
)
def test_lp_i2c_during_deep_sleep(case_tester: CaseTester) -> None:
    TwoBoardSleep(case_tester, 'lp_i2c_sleep').run_retained('I2C_PEER_ARMED', 'I2C_PEER_OK')


@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize('config', ['defaults'], indirect=True)
@idf_parametrize(
    'target',
    soc_filtered_targets(
        '(SOC_LP_CORE_SUPPORTED == 1) and (SOC_ULP_LP_UART_SUPPORTED == 1) and '
        '(SOC_DEEP_SLEEP_SUPPORTED == 1) and (SOC_RTC_TIMER_SUPPORTED == 1)'
    ),
    indirect=['target'],
)
def test_lp_uart_during_deep_sleep(case_tester: CaseTester) -> None:
    TwoBoardSleep(case_tester, 'lp_uart_sleep').run_retained('UART_PEER_ARMED', 'UART_PEER_OK')


@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize('config', ['defaults'], indirect=True)
@idf_parametrize(
    'target',
    soc_filtered_targets(
        '(SOC_LP_CORE_SUPPORTED == 1) and (SOC_ULP_LP_UART_SUPPORTED == 1) and (SOC_DEEP_SLEEP_SUPPORTED == 1)'
    ),
    indirect=['target'],
)
def test_lp_uart_wakeup_from_deep_sleep(case_tester: CaseTester) -> None:
    TwoBoardSleep(case_tester, 'lp_uart_wkup_sleep').run_wakeup('UART_WKUP_PEER_ARMED', 'send', SLEEP_ENTRY_DELAY_S)


@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize('config', ['defaults'], indirect=True)
@idf_parametrize(
    'target',
    soc_filtered_targets(
        '(SOC_LP_CORE_SUPPORTED == 1) and (SOC_RTCIO_PIN_COUNT > 0) and '
        '(SOC_DEEP_SLEEP_SUPPORTED == 1) and (SOC_RTC_TIMER_SUPPORTED == 1)'
    ),
    indirect=['target'],
)
def test_lp_gpio_io_during_deep_sleep(case_tester: CaseTester) -> None:
    TwoBoardSleep(case_tester, 'lp_gpio_io_sleep').run_retained(
        'GPIO_IO_PEER_ARMED',
        'GPIO_IO_PEER_OK',
        after_sleep_signal='drive',
        delay_s=LP_RUNNING_DELAY_S,
    )


@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize('config', ['defaults'], indirect=True)
@idf_parametrize(
    'target',
    soc_filtered_targets(
        '(SOC_LP_CORE_SUPPORTED == 1) and (SOC_RTCIO_PIN_COUNT > 0) and '
        '(SOC_DEEP_SLEEP_SUPPORTED == 1) and (SOC_RTC_TIMER_SUPPORTED == 1)'
    ),
    indirect=['target'],
)
def test_lp_gpio_interrupt_during_deep_sleep(case_tester: CaseTester) -> None:
    TwoBoardSleep(case_tester, 'lp_gpio_isr_sleep').run_wakeup('GPIO_ISR_PEER_ARMED', 'edge', LP_RUNNING_DELAY_S)


@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [2], indirect=True)
@pytest.mark.parametrize('config', ['defaults'], indirect=True)
@idf_parametrize(
    'target',
    soc_filtered_targets(
        '(SOC_LP_CORE_SUPPORTED == 1) and (SOC_RTCIO_PIN_COUNT > 0) and '
        '(SOC_DEEP_SLEEP_SUPPORTED == 1) and (SOC_RTC_GPIO_EDGE_WAKEUP_SUPPORTED == 1)'
    ),
    indirect=['target'],
)
def test_lp_gpio_edge_wakeup_during_deep_sleep(case_tester: CaseTester) -> None:
    TwoBoardSleep(case_tester, 'lp_gpio_wkup_sleep').run_wakeup('GPIO_WKUP_PEER_ARMED', 'edge', GPIO_EDGE_DELAY_S)
