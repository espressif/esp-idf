# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
Pytest Related Constants. Don't import third-party packages here.
"""
import os
import typing as t
from dataclasses import dataclass

from _pytest.python import Function
from pytest_embedded.utils import to_list

SUPPORTED_TARGETS = ['esp32', 'esp32s2', 'esp32c3', 'esp32s3', 'esp32c2', 'esp32c6', 'esp32h2']
PREVIEW_TARGETS: t.List[str] = []  # this PREVIEW_TARGETS excludes 'linux' target
DEFAULT_SDKCONFIG = 'default'

TARGET_MARKERS = {
    'esp32': 'support esp32 target',
    'esp32s2': 'support esp32s2 target',
    'esp32s3': 'support esp32s3 target',
    'esp32c3': 'support esp32c3 target',
    'esp32c2': 'support esp32c2 target',
    'esp32c6': 'support esp32c6 target',
    'esp32h2': 'support esp32h2 target',
    'esp32p4': 'support esp32p4 target',
    'linux': 'support linux target',
}

SPECIAL_MARKERS = {
    'supported_targets': "support all officially announced supported targets ('esp32', 'esp32s2', 'esp32c3', 'esp32s3', 'esp32c2', 'esp32c6')",
    'preview_targets': "support all preview targets ('none')",
    'all_targets': 'support all targets, including supported ones and preview ones',
    'temp_skip_ci': 'temp skip tests for specified targets only in ci',
    'temp_skip': 'temp skip tests for specified targets both in ci and locally',
    'nightly_run': 'tests should be executed as part of the nightly trigger pipeline',
    'host_test': 'tests which should not be built at the build stage, and instead built in host_test stage',
    'qemu': 'build and test using qemu-system-xtensa, not real target',
}

ENV_MARKERS = {
    # single-dut markers
    'generic': 'tests should be run on generic runners',
    'flash_suspend': 'support flash suspend feature',
    'eth_ip101': 'connected via wired 10/100M ethernet',
    'eth_lan8720': 'connected via LAN8720 ethernet transceiver',
    'eth_rtl8201': 'connected via RTL8201 ethernet transceiver',
    'eth_ksz8041': 'connected via KSZ8041 ethernet transceiver',
    'eth_dp83848': 'connected via DP83848 ethernet transceiver',
    'eth_w5500': 'SPI Ethernet module with two W5500',
    'eth_ksz8851snl': 'SPI Ethernet module with two KSZ8851SNL',
    'eth_dm9051': 'SPI Ethernet module with two DM9051',
    'quad_psram': 'runners with quad psram',
    'octal_psram': 'runners with octal psram',
    'usb_host': 'usb host runners',
    'usb_host_flash_disk': 'usb host runners with USB flash disk attached',
    'usb_device': 'usb device runners',
    'ethernet_ota': 'ethernet OTA runners',
    'flash_encryption': 'Flash Encryption runners',
    'flash_encryption_f4r8': 'Flash Encryption runners with 4-line flash and 8-line psram',
    'flash_encryption_f8r8': 'Flash Encryption runners with 8-line flash and 8-line psram',
    'flash_multi': 'Multiple flash chips tests',
    'psram': 'Chip has 4-line psram',
    'ir_transceiver': 'runners with a pair of IR transmitter and receiver',
    'twai_transceiver': 'runners with a TWAI PHY transceiver',
    'flash_encryption_wifi_high_traffic': 'Flash Encryption runners with wifi high traffic support',
    'ethernet': 'ethernet runner',
    'ethernet_flash_8m': 'ethernet runner with 8mb flash',
    'ethernet_router': 'both the runner and dut connect to the same router through ethernet NIC',
    'ethernet_vlan': 'ethernet runner GARM-32-SH-1-R16S5N3',
    'wifi_ap': 'a wifi AP in the environment',
    'wifi_router': 'both the runner and dut connect to the same wifi router',
    'wifi_high_traffic': 'wifi high traffic runners',
    'wifi_wlan': 'wifi runner with a wireless NIC',
    'Example_ShieldBox_Basic': 'basic configuration of the AP and ESP DUT placed in shielded box',
    'Example_ShieldBox': 'multiple shielded APs connected to shielded ESP DUT via RF cable with programmable attenuator',
    'xtal_26mhz': 'runner with 26MHz xtal on board',
    'xtal_40mhz': 'runner with 40MHz xtal on board',
    'external_flash': 'external flash memory connected via VSPI (FSPI)',
    'sdcard_sdmode': 'sdcard running in SD mode',
    'sdcard_spimode': 'sdcard running in SPI mode',
    'emmc': 'eMMC card',
    'MSPI_F8R8': 'runner with Octal Flash and Octal PSRAM',
    'MSPI_F4R8': 'runner with Quad Flash and Octal PSRAM',
    'MSPI_F4R4': 'runner with Quad Flash and Quad PSRAM',
    'jtag': 'runner where the chip is accessible through JTAG as well',
    'usb_serial_jtag': 'runner where the chip is accessible through builtin JTAG as well',
    'adc': 'ADC related tests should run on adc runners',
    'xtal32k': 'Runner with external 32k crystal connected',
    'no32kXtal': 'Runner with no external 32k crystal connected',
    'multi_dut_modbus_rs485': 'a pair of runners connected by RS485 bus',
    'psramv0': 'Runner with PSRAM version 0',
    'esp32eco3': 'Runner with esp32 eco3 connected',
    'ecdsa_efuse': 'Runner with test ECDSA private keys programmed in efuse',
    'ccs811': 'Runner with CCS811 connected',
    'nvs_encr_hmac': 'Runner with test HMAC key programmed in efuse',
    'i2c_oled': 'Runner with ssd1306 I2C oled connected',
    'httpbin': 'runner for tests that need to access the httpbin service',
    # multi-dut markers
    'ieee802154': 'ieee802154 related tests should run on ieee802154 runners.',
    'openthread_br': 'tests should be used for openthread border router.',
    'openthread_bbr': 'tests should be used for openthread border router linked to Internet.',
    'openthread_sleep': 'tests should be used for openthread sleepy device.',
    'zigbee_multi_dut': 'zigbee runner which have multiple duts.',
    'wifi_two_dut': 'tests should be run on runners which has two wifi duts connected.',
    'generic_multi_device': 'generic multiple devices whose corresponding gpio pins are connected to each other.',
    'twai_network': 'multiple runners form a TWAI network.',
    'sdio_master_slave': 'Test sdio multi board.',
    'usj_device': 'Test usb_serial_jtag and usb_serial_jtag is used as serial only (not console)'
}


@dataclass
class PytestApp:
    path: str
    target: str
    config: str

    def __hash__(self) -> int:
        return hash((self.path, self.target, self.config))


@dataclass
class PytestCase:
    path: str
    name: str

    apps: t.Set[PytestApp]
    target: str

    item: Function

    def __hash__(self) -> int:
        return hash((self.path, self.name, self.apps, self.all_markers))

    @property
    def all_markers(self) -> t.Set[str]:
        return {marker.name for marker in self.item.iter_markers()}

    @property
    def is_nightly_run(self) -> bool:
        return 'nightly_run' in self.all_markers

    @property
    def target_markers(self) -> t.Set[str]:
        return {marker for marker in self.all_markers if marker in TARGET_MARKERS}

    @property
    def env_markers(self) -> t.Set[str]:
        return {marker for marker in self.all_markers if marker in ENV_MARKERS}

    @property
    def skipped_targets(self) -> t.Set[str]:
        def _get_temp_markers_disabled_targets(marker_name: str) -> t.Set[str]:
            temp_marker = self.item.get_closest_marker(marker_name)

            if not temp_marker:
                return set()

            # temp markers should always use keyword arguments `targets` and `reason`
            if not temp_marker.kwargs.get('targets') or not temp_marker.kwargs.get('reason'):
                raise ValueError(
                    f'`{marker_name}` should always use keyword arguments `targets` and `reason`. '
                    f'For example: '
                    f'`@pytest.mark.{marker_name}(targets=["esp32"], reason="IDF-xxxx, will fix it ASAP")`'
                )

            return set(to_list(temp_marker.kwargs['targets']))  # type: ignore

        temp_skip_ci_targets = _get_temp_markers_disabled_targets('temp_skip_ci')
        temp_skip_targets = _get_temp_markers_disabled_targets('temp_skip')

        # in CI we skip the union of `temp_skip` and `temp_skip_ci`
        if os.getenv('CI_JOB_ID'):
            skip_targets = temp_skip_ci_targets.union(temp_skip_targets)
        else:  # we use `temp_skip` locally
            skip_targets = temp_skip_targets

        return skip_targets
