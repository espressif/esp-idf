#
# Common (non-language-specific) configuration for Sphinx
#
# This file is imported from a language-specific conf.py (ie en/conf.py or
# zh_CN/conf.py)
# type: ignore
# pylint: disable=wildcard-import
# pylint: disable=undefined-variable
import os.path
import re
import sys
from pathlib import Path

from esp_docs.conf_docs import *  # noqa: F403,F401

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'tools', 'cmakev2')))
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'tools', 'docs')))

if os.environ.get('IDF_PATH') is None:
    raise RuntimeError('IDF_PATH should be set, run export.sh before building docs')

BT_DOCS = [
    'api-guides/bt-architecture/index.rst',
    'api-guides/bt-architecture/overview.rst',
    'api-reference/bluetooth/esp_bt_defs.rst',
    'api-reference/bluetooth/esp_bt_device.rst',
    'api-reference/bluetooth/esp_bt_main.rst',
    'api-reference/bluetooth/bt_common.rst',
    'api-reference/bluetooth/bt_vhci.rst',
    'api-reference/bluetooth/controller_vhci.rst',
    'api-reference/bluetooth/index.rst',
    'migration-guides/release-5.x/5.4/bt_common.rst',
]

BLE_DOCS = [
    'api-guides/ble/index.rst',
    'api-guides/ble/overview.rst',
    'api-guides/ble/ble-feature-support-status.rst',
    'api-guides/ble/host-feature-support-status.rst',
    'api-guides/ble/ble-qualification.rst',
    'api-guides/ble/ble-multiconnection-guide.rst',
    'api-guides/ble/get-started/ble-introduction.rst',
    'api-guides/ble/get-started/ble-device-discovery.rst',
    'api-guides/ble/get-started/ble-connection.rst',
    'api-guides/ble/get-started/ble-data-exchange.rst',
    'api-guides/ble/smp.rst',
    'api-guides/low-power-mode/low-power-mode-ble.rst',
    'api-reference/bluetooth/bt_le.rst',
    'api-reference/bluetooth/esp_gap_ble.rst',
    'api-reference/bluetooth/esp_gatt_defs.rst',
    'api-reference/bluetooth/esp_gatts.rst',
    'api-reference/bluetooth/esp_gattc.rst',
    'api-reference/bluetooth/nimble/index.rst',
    'migration-guides/release-5.x/5.0/bluetooth-low-energy.rst',
]


BLE_MESH_DOCS = [
    'api-guides/esp-ble-mesh/ble-mesh-index.rst',
    'api-guides/esp-ble-mesh/ble-mesh-feature-list.rst',
    'api-guides/esp-ble-mesh/ble-mesh-terminology.rst',
    'api-guides/esp-ble-mesh/ble-mesh-architecture.rst',
    'api-guides/esp-ble-mesh/ble-mesh-faq.rst',
    'api-reference/bluetooth/esp-ble-mesh.rst',
]

CLASSIC_BT_DOCS = [
    'api-guides/classic-bt/index.rst',
    'api-guides/classic-bt/overview.rst',
    'api-guides/classic-bt/profiles-protocols.rst',
    'api-reference/bluetooth/classic_bt.rst',
    'api-reference/bluetooth/esp_a2dp.rst',
    'api-reference/bluetooth/esp_avrc.rst',
    'api-reference/bluetooth/esp_hidd.rst',
    'api-reference/bluetooth/esp_hidh.rst',
    'api-reference/bluetooth/esp_l2cap_bt.rst',
    'api-reference/bluetooth/esp_sdp.rst',
    'api-reference/bluetooth/esp_hf_defs.rst',
    'api-reference/bluetooth/esp_hf_client.rst',
    'api-reference/bluetooth/esp_hf_ag.rst',
    'api-reference/bluetooth/esp_spp.rst',
    'api-reference/bluetooth/esp_gap_bt.rst',
    'migration-guides/release-5.x/5.0/bluetooth-classic.rst',
    'migration-guides/release-5.x/5.2/bluetooth-classic.rst',
    'migration-guides/release-5.x/5.3/bluetooth-classic.rst',
    'migration-guides/release-5.x/5.4/bluetooth-classic.rst',
    'migration-guides/release-6.x/6.0/bluetooth-classic.rst',
]

BLUFI_DOCS = ['api-guides/ble/blufi.rst', 'api-reference/bluetooth/esp_blufi.rst']

WIFI_DOCS = [
    'api-guides/low-power-mode/low-power-mode-wifi.rst',
    'api-guides/wifi-driver/**',
    'api-guides/wifi-security.rst',
    'api-guides/wireshark-user-guide.rst',
    'api-reference/network/esp_now.rst',
    'api-reference/network/esp_smartconfig.rst',
    'api-reference/network/esp_wifi.rst',
    'api-reference/network/esp_dpp.rst',
    'migration-guides/release-5.x/5.2/wifi.rst',
    'migration-guides/release-5.x/5.4/wifi.rst',
    'migration-guides/release-5.x/5.5/wifi.rst',
    'migration-guides/release-6.x/6.0/wifi.rst',
]

IEEE802154_DOCS = ['migration-guides/release-5.x/5.1/ieee802154.rst', 'migration-guides/release-5.x/5.2/ieee802154.rst']

NAN_DOCS = ['api-reference/network/esp_nan.rst']

WIFI_MESH_DOCS = ['api-guides/esp-wifi-mesh.rst', 'api-reference/network/esp-wifi-mesh.rst']

COEXISTENCE_DOCS = ['api-guides/coexist.rst']

MM_SYNC_DOCS = ['api-reference/system/mm_sync.rst']

CAMERA_DOCS = ['api-reference/peripherals/camera_driver.rst']

BITSCRAMBLER_DOCS = ['api-reference/peripherals/bitscrambler.rst']

CLK_TREE_DOCS = ['api-reference/peripherals/clk_tree.rst']

UART_DOCS = ['api-reference/peripherals/uart.rst']

UHCI_DOCS = ['api-reference/peripherals/uhci.rst']

SDMMC_DOCS = ['api-reference/peripherals/sdmmc_host.rst']

SDIO_SLAVE_DOCS = ['api-reference/peripherals/sdio_slave.rst', 'api-reference/protocols/esp_sdio_slave_protocol.rst']

MCPWM_DOCS = ['api-reference/peripherals/mcpwm.rst']

DEDIC_GPIO_DOCS = ['api-reference/peripherals/dedic_gpio.rst']

PARLIO_DOCS = [
    'api-reference/peripherals/parlio/index.rst',
    'api-reference/peripherals/parlio/parlio_tx.rst',
    'api-reference/peripherals/parlio/parlio_rx.rst',
]

PCNT_DOCS = ['api-reference/peripherals/pcnt.rst']

RMT_DOCS = ['api-reference/peripherals/rmt.rst']

DAC_DOCS = ['api-reference/peripherals/dac.rst']

ETM_DOCS = ['api-reference/peripherals/etm.rst']

LDO_DOCS = ['api-reference/peripherals/ldo_regulator.rst']

TEMP_SENSOR_DOCS = ['api-reference/peripherals/temp_sensor.rst']

TOUCH_SENSOR_DOCS = ['api-reference/peripherals/cap_touch_sens.rst']

SPIRAM_DOCS = ['api-guides/external-ram.rst']

USB_DOCS = [
    'api-reference/peripherals/usb_device.rst',
    'api-reference/peripherals/usb_host.rst',
    'api-reference/peripherals/usb_host/usb_host_notes_arch.rst',
    'api-reference/peripherals/usb_host/usb_host_notes_design.rst',
    'api-reference/peripherals/usb_host/usb_host_notes_dwc_otg.rst',
    'api-reference/peripherals/usb_host/usb_host_notes_index.rst',
    'api-reference/peripherals/usb_host/usb_host_notes_usbh.rst',
    'api-reference/peripherals/usb_host/usb_host_notes_enum.rst',
    'api-reference/peripherals/usb_host/usb_host_notes_ext_hub.rst',
    'api-reference/peripherals/usb_host/usb_host_notes_ext_port.rst',
]

I80_LCD_DOCS = ['api-reference/peripherals/lcd/i80_lcd.rst']
RGB_LCD_DOCS = ['api-reference/peripherals/lcd/rgb_lcd.rst']
DSI_LCD_DOCS = ['api-reference/peripherals/lcd/dsi_lcd.rst']
PARLIO_LCD_DOCS = ['api-reference/peripherals/lcd/parl_lcd.rst']

USB_OTG_DFU_DOCS = ['api-guides/dfu.rst']

USB_OTG_CONSOLE_DOCS = ['api-guides/usb-otg-console.rst']

FTDI_JTAG_DOCS = ['api-guides/jtag-debugging/configure-ft2232h-jtag.rst']

USB_SERIAL_JTAG_DOCS = [
    'api-guides/jtag-debugging/configure-builtin-jtag.rst',
    'api-guides/usb-serial-jtag-console.rst',
]

ULP_FSM_DOCS = [
    'api-reference/system/ulp-fsm.rst',
    'api-reference/system/ulp_macros.rst',
    'api-reference/system/ulp_instruction_set.rst',
]

RISCV_COPROC_DOCS = [
    'api-reference/system/ulp-risc-v.rst',
]

LP_CORE_DOCS = ['api-reference/system/ulp-lp-core.rst']

XTENSA_DOCS = ['api-guides/hlinterrupts.rst', 'api-reference/system/perfmon.rst']

RISCV_DOCS = []  # type: list[str]

TWAI_DOCS = ['api-reference/peripherals/twai.rst']

SDM_DOCS = ['api-reference/peripherals/sdm.rst']

I2C_DOCS = [
    'api-reference/peripherals/i2c.rst',
    'api-reference/peripherals/lcd/i2c_lcd.rst',
    'api-reference/peripherals/i2c_slave_v1.rst',
]

I3C_DOCS = [
    'api-reference/peripherals/i3c_master.rst',
]

SPI_DOCS = [
    'api-reference/peripherals/spi_master.rst',
    'api-reference/peripherals/spi_slave.rst',
    'api-reference/peripherals/sdspi_host.rst',
    'api-reference/peripherals/sdspi_share.rst',
    'api-reference/peripherals/lcd/spi_lcd.rst',
]

I2S_DOCS = ['api-reference/peripherals/i2s.rst']

VAD_DOCS = ['api-reference/peripherals/vad.rst']

LP_I2S_DOCS = ['api-reference/peripherals/lp_i2s.rst']

ISP_DOCS = ['api-reference/peripherals/isp.rst']

DSLP_STUB_DOCS = ['api-guides/deep-sleep-stub.rst']

ADC_DOCS = [
    'api-reference/peripherals/adc/index.rst',
    'api-reference/peripherals/adc/adc_oneshot.rst',
    'api-reference/peripherals/adc/adc_calibration.rst',
]

ADC_DMA_DOCS = ['api-reference/peripherals/adc/adc_continuous.rst']

ANA_CMPR_DOCS = ['api-reference/peripherals/ana_cmpr.rst']

SPI_SLAVE_HD_DOCS = ['api-reference/peripherals/spi_slave_hd.rst']

JPEG_DOCS = ['api-reference/peripherals/jpeg.rst']

PPA_DOCS = ['api-reference/peripherals/ppa.rst']

QEMU_DOCS = ['api-guides/tools/qemu.rst']

ESP_TEE_DOCS = [
    'security/tee/index.rst',
    'security/tee/tee.rst',
    'security/tee/tee-advanced.rst',
    'security/tee/tee-attestation.rst',
    'security/tee/tee-ota.rst',
    'security/tee/tee-sec-storage.rst',
]

ESP32_DOCS = (
    [
        'api-reference/system/himem.rst',
        'api-guides/romconsole.rst',
        'api-reference/system/ipc.rst',
        'security/secure-boot-v1.rst',
        'api-reference/peripherals/sd_pullup_requirements.rst',
        'hw-reference/esp32/**',
        'api-guides/RF_calibration.rst',
        'api-guides/phy.rst',
    ]
    + FTDI_JTAG_DOCS
    + QEMU_DOCS
)

ESP32S2_DOCS = (
    [
        'hw-reference/esp32s2/**',
        'api-guides/usb-console.rst',
        'api-reference/peripherals/ds.rst',
        'api-guides/RF_calibration.rst',
        'api-guides/phy.rst',
    ]
    + FTDI_JTAG_DOCS
    + USB_OTG_DFU_DOCS
    + USB_OTG_CONSOLE_DOCS
)

ESP32S3_DOCS = (
    [
        'hw-reference/esp32s3/**',
        'api-reference/system/ipc.rst',
        'api-guides/flash_psram_config.rst',
        'api-reference/peripherals/sd_pullup_requirements.rst',
        'api-guides/RF_calibration.rst',
        'api-guides/phy.rst',
    ]
    + USB_OTG_DFU_DOCS
    + USB_OTG_CONSOLE_DOCS
    + QEMU_DOCS
)

# No JTAG docs for this one as it gets gated on SOC_USB_SERIAL_JTAG_SUPPORTED down below.
ESP32C3_DOCS = ['hw-reference/esp32c3/**', 'api-guides/RF_calibration.rst', 'api-guides/phy.rst'] + QEMU_DOCS

ESP32C2_DOCS = ['api-guides/RF_calibration.rst', 'api-guides/phy.rst']

ESP32C5_DOCS = [
    'api-guides/phy.rst',
    'api-reference/peripherals/sd_pullup_requirements.rst',
    'api-guides/RF_calibration.rst',
] + ESP_TEE_DOCS

ESP32C61_DOCS = [
    'api-guides/phy.rst',
    'api-reference/peripherals/sd_pullup_requirements.rst',
    'api-guides/RF_calibration.rst',
] + ESP_TEE_DOCS

ESP32C6_DOCS = [
    'api-guides/RF_calibration.rst',
    'api-reference/peripherals/sd_pullup_requirements.rst',
    'api-guides/phy.rst',
] + ESP_TEE_DOCS

ESP32H2_DOCS = ['api-guides/RF_calibration.rst', 'api-guides/phy.rst'] + ESP_TEE_DOCS

ESP32H4_DOCS = [
    'api-reference/system/ipc.rst',
    'api-guides/RF_calibration.rst',
    'api-guides/phy.rst',
]

ESP32P4_DOCS = [
    'api-reference/system/ipc.rst',
    'api-reference/peripherals/cap_touch_sens.rst',
    'api-reference/peripherals/sd_pullup_requirements.rst',
] + USB_OTG_DFU_DOCS

# format: {tag needed to include: documents to included}, tags are parsed from sdkconfig and peripheral_caps.h headers
conditional_include_dict = {
    'SOC_BT_SUPPORTED': BT_DOCS,
    'SOC_BLE_SUPPORTED': BLE_DOCS,
    'SOC_BLE_MESH_SUPPORTED': BLE_MESH_DOCS,
    'SOC_BLUFI_SUPPORTED': BLUFI_DOCS,
    'SOC_WIFI_SUPPORTED': WIFI_DOCS,
    'SOC_BT_CLASSIC_SUPPORTED': CLASSIC_BT_DOCS,
    'SOC_IEEE802154_SUPPORTED': IEEE802154_DOCS,
    'SOC_SUPPORT_COEXISTENCE': COEXISTENCE_DOCS,
    'SOC_PSRAM_DMA_CAPABLE': MM_SYNC_DOCS,
    'SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE': MM_SYNC_DOCS,
    'SOC_CLK_TREE_SUPPORTED': CLK_TREE_DOCS,
    'SOC_UART_SUPPORTED': UART_DOCS,
    'SOC_UHCI_SUPPORTED': UHCI_DOCS,
    'SOC_SDMMC_HOST_SUPPORTED': SDMMC_DOCS,
    'SOC_SDIO_SLAVE_SUPPORTED': SDIO_SLAVE_DOCS,
    'SOC_MCPWM_SUPPORTED': MCPWM_DOCS,
    'SOC_USB_OTG_SUPPORTED': USB_DOCS,
    'SOC_USB_SERIAL_JTAG_SUPPORTED': USB_SERIAL_JTAG_DOCS,
    'SOC_DEDICATED_GPIO_SUPPORTED': DEDIC_GPIO_DOCS,
    'SOC_LCD_I80_SUPPORTED': I80_LCD_DOCS,
    'SOC_LCD_RGB_SUPPORTED': RGB_LCD_DOCS,
    'SOC_PARLIO_LCD_SUPPORTED': PARLIO_LCD_DOCS,
    'SOC_MIPI_DSI_SUPPORTED': DSI_LCD_DOCS,
    'SOC_SPIRAM_SUPPORTED': SPIRAM_DOCS,
    'SOC_PARLIO_SUPPORTED': PARLIO_DOCS,
    'SOC_PCNT_SUPPORTED': PCNT_DOCS,
    'SOC_RMT_SUPPORTED': RMT_DOCS,
    'SOC_DAC_SUPPORTED': DAC_DOCS,
    'SOC_ETM_SUPPORTED': ETM_DOCS,
    'SOC_ULP_FSM_SUPPORTED': ULP_FSM_DOCS,
    'SOC_RISCV_COPROC_SUPPORTED': RISCV_COPROC_DOCS,
    'SOC_LP_CORE_SUPPORTED': LP_CORE_DOCS,
    'SOC_DIG_SIGN_SUPPORTED': ['api-reference/peripherals/ds.rst'],
    'SOC_ECDSA_SUPPORTED': ['api-reference/peripherals/ecdsa.rst'],
    'SOC_HMAC_SUPPORTED': ['api-reference/peripherals/hmac.rst'],
    'SOC_ASYNC_MEMCPY_SUPPORTED': ['api-reference/system/async_memcpy.rst'],
    'CONFIG_IDF_TARGET_ARCH_XTENSA': XTENSA_DOCS,
    'CONFIG_IDF_TARGET_ARCH_RISCV': RISCV_DOCS,
    'SOC_TEMP_SENSOR_SUPPORTED': TEMP_SENSOR_DOCS,
    'SOC_TOUCH_SENSOR_SUPPORTED': TOUCH_SENSOR_DOCS,
    'SOC_TWAI_SUPPORTED': TWAI_DOCS,
    'SOC_I2C_SUPPORTED': I2C_DOCS,
    'SOC_I3C_MASTER_SUPPORTED': I3C_DOCS,
    'SOC_GPSPI_SUPPORTED': SPI_DOCS,
    'SOC_I2S_SUPPORTED': I2S_DOCS,
    'SOC_LP_I2S_SUPPORTED': LP_I2S_DOCS,
    'SOC_LP_VAD_SUPPORTED': VAD_DOCS,
    'SOC_ISP_SUPPORTED': ISP_DOCS,
    'ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB': DSLP_STUB_DOCS,
    'SOC_ADC_SUPPORTED': ADC_DOCS,
    'SOC_ADC_DMA_SUPPORTED': ADC_DMA_DOCS,
    'SOC_ANA_CMPR_SUPPORTED': ANA_CMPR_DOCS,
    'SOC_SDM_SUPPORTED': SDM_DOCS,
    'SOC_WIFI_MESH_SUPPORT': WIFI_MESH_DOCS,
    'SOC_MIPI_CSI_SUPPORTED': CAMERA_DOCS,
    'SOC_BITSCRAMBLER_SUPPORTED': BITSCRAMBLER_DOCS,
    'SOC_SPI_SUPPORT_SLAVE_HD_VER2': SPI_SLAVE_HD_DOCS,
    'SOC_WIFI_NAN_SUPPORT': NAN_DOCS,
    'SOC_JPEG_CODEC_SUPPORTED': JPEG_DOCS,
    'SOC_PPA_SUPPORTED': PPA_DOCS,
    'SOC_GP_LDO_SUPPORTED': LDO_DOCS,
    'esp32': ESP32_DOCS,
    'esp32s2': ESP32S2_DOCS,
    'esp32s3': ESP32S3_DOCS,
    'esp32c2': ESP32C2_DOCS,
    'esp32c3': ESP32C3_DOCS,
    'esp32c5': ESP32C5_DOCS,
    'esp32c6': ESP32C6_DOCS,
    'esp32c61': ESP32C61_DOCS,
    'esp32h2': ESP32H2_DOCS,
    'esp32h4': ESP32H4_DOCS,
    'esp32p4': ESP32P4_DOCS,
}

extensions += [  # noqa: F405
    'sphinx_copybutton',
    'sphinxcontrib.wavedrom',
    # Note: order is important here, events must
    # be registered by one extension before they can be
    # connected to another extension
    'esp_docs.idf_extensions.build_system',
    'esp_docs.idf_extensions.esp_err_definitions',
    'esp_docs.idf_extensions.gen_defines',
    'esp_docs.idf_extensions.kconfig_reference',
    'esp_docs.idf_extensions.gen_idf_tools_links',
    'esp_docs.esp_extensions.run_doxygen',
    'esp_docs.esp_extensions.add_html_zip',
    'linuxdoc.rstFlatTable',  # https://return42.github.io/linuxdoc/linuxdoc-howto/table-markup.html#flat-table
    'esp_docs_cmakev2_extension',
    'gen_version_specific_includes',
]

# Use wavedrompy as backend, instead of wavedrom-cli
render_using_wavedrompy = True

smartquotes = False

# link roles config
github_repo = 'espressif/esp-idf'

# context used by sphinx_idf_theme
html_context['github_user'] = 'espressif'  # noqa: F405
html_context['github_repo'] = 'esp-idf'  # noqa: F405


# Extra options required by sphinx_idf_theme
project_slug = 'esp-idf'
versions_url = 'https://dl.espressif.com/dl/esp-idf/idf_versions.js'

idf_targets = ['esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c2', 'esp32c5', 'esp32c6', 'esp32p4']
languages = ['en', 'zh_CN']

google_analytics_id = os.environ.get('CI_GOOGLE_ANALYTICS_ID', None)

project_homepage = 'https://github.com/espressif/esp-idf'

linkcheck_anchors = False

linkcheck_exclude_documents = [
    'index',  # several false positives due to the way we link to different sections
    'api-reference/protocols/esp_local_ctrl',  # Fails due to `https://<mdns-hostname>.local`
]


# URLs to ignore during linkcheck
linkcheck_ignore = [
    # Certificate error
    'https://webhome.phy.duke.edu/~rgb/General/dieharder.php',
    # Not published docs
    'https://docs.espressif.com/projects/esptool/en/latest/esp32c2/espefuse/index.html',
    'https://docs.espressif.com/projects/esptool/en/latest/esp32c2/espsecure/index.html#remote-signing-using-an-external-hs',
    'https://docs.espressif.com/projects/esptool/en/latest/esp32c6/espefuse/index.html',
    'https://docs.espressif.com/projects/esptool/en/latest/esp32c6/espsecure/index.html#remote-signing-using-an-external-hs',
    'https://docs.espressif.com/projects/esptool/en/latest/esp32h2/espefuse/index.html',
    'https://docs.espressif.com/projects/esptool/en/latest/esp32h2/espsecure/index.html#remote-signing-using-an-external-hs',
    # Rejects user-agent
    'https://www.cadence.com/content/dam/cadence-www/global/en_US/documents/tools/ip/tensilica-ip/isa-summary.pdf',
]

# Custom added feature to allow redirecting old URLs
with open('../page_redirects.txt') as f:
    lines = [
        re.sub(' +', ' ', line.strip()) for line in f.readlines() if line.strip() != '' and not line.startswith('#')
    ]
    for line in lines:  # check for well-formed entries
        if len(line.split(' ')) != 2:
            raise RuntimeError(f'Invalid line in page_redirects.txt: {line}')
html_redirect_pages = [tuple(line.split(' ')) for line in lines]

html_static_path = ['../_static']
html_css_files = ['js/chatbot_widget.css']

idf_build_system = {
    'doxygen_component_info': True,
    'component_info_ignore_file': Path(os.environ['IDF_PATH']) / 'docs' / 'component_info_ignore_file.txt',
}

# Please update following list to enable Qemu doc guide (and cross references) for a new target
QEMU_TARGETS = ['esp32', 'esp32c3', 'esp32s3']

# Please update following list to enable ESP-TEE doc guide (and cross references) for a new target
ESP_TEE_TARGETS = ['esp32c6', 'esp32h2', 'esp32c5', 'esp32c61']


# Callback function for user setup that needs be done after `config-init`-event
# config.idf_target is not available at the initial config stage
def conf_setup(app, config):
    config.add_warnings_content = (
        f'This document is not updated for {config.idf_target.upper()} yet, so some of the content may not be correct.'
    )

    add_warnings_file = f'{app.confdir}/../docs_not_updated/{config.idf_target}.txt'

    if config.idf_target in QEMU_TARGETS:
        app.tags.add('TARGET_SUPPORT_QEMU')

    if config.idf_target in ESP_TEE_TARGETS:
        app.tags.add('TARGET_SUPPORT_ESP_TEE')

    try:
        with open(add_warnings_file) as warning_file:
            config.add_warnings_pages = warning_file.read().splitlines()
    except FileNotFoundError:
        # Not for all target
        pass

    config.html_baseurl = f'https://docs.espressif.com/projects/esp-idf/{config.language}/stable/{config.idf_target}'


user_setup_callback = conf_setup
