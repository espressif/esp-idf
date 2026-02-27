#!/usr/bin/env python3
#
# Checks that every IDF example is referenced in the programming guide (docs/en/).
# Each example must be referenced exactly by its own path; parent-directory references do not count.
#
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import os
import re
import sys
from pathlib import Path

# Examples that are not yet referenced in the programming guide.
# These must be fixed: add a :example:`<path>` reference in docs/en/ and remove from here.
KNOWN_MISSING = {
    # TODO IDF-15374: add :example: references for ble_get_started examples
    'bluetooth/ble_get_started/bluedroid/Bluedroid_Beacon',
    'bluetooth/ble_get_started/bluedroid/Bluedroid_Connection',
    'bluetooth/ble_get_started/bluedroid/Bluedroid_GATT_Server',
    'bluetooth/ble_get_started/nimble/NimBLE_Security',
    # TODO IDF-15375: add :example: references for zigbee and ieee802154 examples
    'ieee802154/ieee802154_cli',
    'zigbee/esp_zigbee_gateway',
    'zigbee/light_sample/HA_on_off_light',
    'zigbee/light_sample/HA_on_off_switch',
    # TODO IDF-15376: add :example: reference for security_features_app
    'security/security_features_app',
    # TODO IDF-15380: add :example: references for bluetooth examples
    'bluetooth/bluedroid/ble/ble_acl_latency/cent',
    'bluetooth/bluedroid/ble/ble_acl_latency/periph',
    'bluetooth/bluedroid/ble/ble_ancs',
    'bluetooth/bluedroid/ble/ble_compatibility_test',
    'bluetooth/bluedroid/ble/ble_eddystone_receiver',
    'bluetooth/bluedroid/ble/ble_eddystone_sender',
    'bluetooth/bluedroid/ble/ble_enc_adv_data/enc_adv_data_cent',
    'bluetooth/bluedroid/ble/ble_enc_adv_data/enc_adv_data_prph',
    'bluetooth/bluedroid/ble/ble_hid_device_demo',
    'bluetooth/bluedroid/ble/ble_ibeacon',
    'bluetooth/bluedroid/ble/ble_multi_conn/ble_multi_conn_cent',
    'bluetooth/bluedroid/ble/ble_multi_conn/ble_multi_conn_prph',
    'bluetooth/bluedroid/ble/ble_spp_client',
    'bluetooth/bluedroid/ble/ble_spp_server',
    'bluetooth/bluedroid/ble/ble_throughput/throughput_client',
    'bluetooth/bluedroid/ble/ble_throughput/throughput_server',
    'bluetooth/bluedroid/ble_50/ble50_security_client',
    'bluetooth/bluedroid/ble_50/ble50_security_server',
    'bluetooth/bluedroid/ble_50/ble50_throughput/throughput_client',
    'bluetooth/bluedroid/ble_50/ble50_throughput/throughput_server',
    'bluetooth/bluedroid/ble_50/ble_conn_subrating_central',
    'bluetooth/bluedroid/ble_50/ble_conn_subrating_peripheral',
    'bluetooth/bluedroid/ble_50/ble_connection_central_with_cte',
    'bluetooth/bluedroid/ble_50/ble_connection_peripheral_with_cte',
    'bluetooth/bluedroid/ble_50/ble_pawr_advertiser',
    'bluetooth/bluedroid/ble_50/ble_pawr_advertiser_conn',
    'bluetooth/bluedroid/ble_50/ble_pawr_synchronizer',
    'bluetooth/bluedroid/ble_50/ble_periodic_adv_with_cte',
    'bluetooth/bluedroid/ble_50/ble_periodic_sync_with_cte',
    'bluetooth/bluedroid/ble_50/ble_power_control_central',
    'bluetooth/bluedroid/ble_50/ble_power_control_peripheral',
    'bluetooth/bluedroid/ble_50/multi-adv',
    'bluetooth/bluedroid/ble_50/periodic_adv',
    'bluetooth/bluedroid/ble_50/periodic_sync',
    'bluetooth/bluedroid/bluedroid_host_only/bluedroid_host_only_uart',
    'bluetooth/esp_ble_mesh/aligenie_demo',
    'bluetooth/esp_ble_mesh/directed_forwarding/df_client',
    'bluetooth/esp_ble_mesh/directed_forwarding/df_server',
    'bluetooth/nimble/ble_ancs',
    'bluetooth/nimble/ble_chan_sound_initiator',
    'bluetooth/nimble/ble_chan_sound_reflector',
    'bluetooth/nimble/ble_cte/ble_periodic_adv_with_cte',
    'bluetooth/nimble/ble_cte/ble_periodic_sync_with_cte',
    'bluetooth/nimble/ble_cts/cts_cent',
    'bluetooth/nimble/ble_cts/cts_prph',
    'bluetooth/nimble/ble_dynamic_service',
    'bluetooth/nimble/ble_enc_adv_data/enc_adv_data_cent',
    'bluetooth/nimble/ble_enc_adv_data/enc_adv_data_prph',
    'bluetooth/nimble/ble_gattc_gatts_coex',
    'bluetooth/nimble/ble_htp/htp_cent',
    'bluetooth/nimble/ble_htp/htp_prph',
    'bluetooth/nimble/ble_l2cap_coc/coc_blecent',
    'bluetooth/nimble/ble_l2cap_coc/coc_bleprph',
    'bluetooth/nimble/ble_multi_adv',
    'bluetooth/nimble/ble_multi_conn/ble_multi_conn_cent',
    'bluetooth/nimble/ble_multi_conn/ble_multi_conn_prph',
    'bluetooth/nimble/ble_pawr_adv/ble_pawr_adv',
    'bluetooth/nimble/ble_pawr_adv/ble_pawr_sync',
    'bluetooth/nimble/ble_pawr_adv_conn/ble_pawr_adv_conn',
    'bluetooth/nimble/ble_pawr_adv_conn/ble_pawr_sync_conn',
    'bluetooth/nimble/ble_periodic_adv',
    'bluetooth/nimble/ble_periodic_sync',
    'bluetooth/nimble/ble_phy/phy_cent',
    'bluetooth/nimble/ble_phy/phy_prph',
    'bluetooth/nimble/ble_proximity_sensor/proximity_sensor_cent',
    'bluetooth/nimble/ble_proximity_sensor/proximity_sensor_prph',
    'bluetooth/nimble/ble_spp/spp_client',
    'bluetooth/nimble/ble_spp/spp_server',
    'bluetooth/nimble/blecent',
    'bluetooth/nimble/blecsc',
    'bluetooth/nimble/blehr',
    'bluetooth/nimble/blemesh',
    'bluetooth/nimble/bleprph',
    'bluetooth/nimble/bleprph_host_only',
    'bluetooth/nimble/bleprph_wifi_coex',
    'bluetooth/nimble/hci',
    'bluetooth/nimble/throughput_app/blecent_throughput',
    'bluetooth/nimble/throughput_app/bleprph_throughput',
    # TODO IDF-15385: add :example: references for build_system examples
    'build_system/cmake/import_prebuilt/prebuilt',
    'build_system/cmakev2/features/component_manager',
    'build_system/cmakev2/features/conditional_component',
    'build_system/cmakev2/features/idf_as_lib',
    'build_system/cmakev2/features/import_lib',
    'build_system/cmakev2/features/import_lib_direct',
    'build_system/cmakev2/features/import_prebuilt',
    'build_system/cmakev2/features/import_prebuilt/prebuilt',
    'build_system/cmakev2/features/multi_binary',
    'build_system/cmakev2/features/multi_config',
    'build_system/cmakev2/features/plugins',
    'build_system/cmakev2/get-started/hello_world',
    # TODO IDF-15381: add :example: references for system and custom_bootloader examples
    'custom_bootloader/bootloader_multiboot',
    'system/gcov',
    'system/gdbstub',
    'system/rt_mqueue',
    'system/sysview_tracing',
    'system/sysview_tracing_heap_log',
    'system/ulp/lp_core/build_system',
    'system/ulp/lp_core/build_system/main/ulp',
    'system/ulp/lp_core/debugging/main/ulp',
    'system/ulp/lp_core/gpio_wakeup',
    'system/ulp/lp_core/inter_cpu_critical_section',
    'system/ulp/lp_core/lp_adc',
    'system/ulp/lp_core/lp_spi',
    'system/ulp/lp_core/lp_timer_interrupt',
    'system/ulp/lp_core/lp_touch',
    'system/ulp/ulp_riscv/gpio_pulse_counter',
    'system/unit_test/test',
    # TODO IDF-15382: add :example: references for peripherals examples
    'peripherals/camera/dvp_dsi',
    'peripherals/camera/dvp_spi_lcd',
    'peripherals/h264',
    'peripherals/i2s/i2s_advance/i2s_usb',
    'peripherals/spi_slave/receiver',
    'peripherals/spi_slave/sender',
    'peripherals/spi_slave_hd/append_mode/master',
    'peripherals/spi_slave_hd/append_mode/slave',
    'peripherals/spi_slave_hd/segment_mode/seg_master',
    'peripherals/spi_slave_hd/segment_mode/seg_slave',
    'peripherals/twai/twai_network/twai_listen_only',
    'peripherals/twai/twai_network/twai_sender',
    'peripherals/usb/device/cherryusb_serial_device',
    'peripherals/usb/host/cherryusb_host',
    # TODO IDF-15383: add :example: references for protocols examples
    'protocols/dns_over_https',
    'protocols/http_request',
    'protocols/icmp/pmtu_probe',
    'protocols/mqtt',
    'protocols/mqtt5',
    'protocols/smtp_client',
    'protocols/sockets/tcp_transport_client',
    # TODO IDF-15384: add :example: references for wifi examples
    'wifi/ftm',
    'wifi/wifi_nvs_config',
}

# Matches :example:`path` and :example:`label <path>`
_EXAMPLE_ROLE_RE = re.compile(r':example:`(?:[^<`]*<([^>]+)>|([^`]+))`')


def find_leaf_examples(examples_dir: Path) -> list[str]:
    """Return relative paths of all leaf examples (those containing a project() call)."""
    results = []
    for cmake_file in examples_dir.rglob('CMakeLists.txt'):
        # Skip managed_components and build dirs
        parts = cmake_file.parts
        if 'managed_components' in parts or 'build' in parts:
            continue
        if cmake_file.read_text().startswith('project(') or '\nproject(' in cmake_file.read_text():
            rel = cmake_file.parent.relative_to(examples_dir)
            results.append(str(rel))
    return sorted(results)


def collect_doc_example_refs(docs_dir: Path) -> set[str]:
    """Return the set of example paths referenced in .rst files under docs_dir."""
    refs: set[str] = set()
    for rst_file in docs_dir.rglob('*.rst'):
        text = rst_file.read_text(errors='replace')
        for m in _EXAMPLE_ROLE_RE.finditer(text):
            path = (m.group(1) or m.group(2)).strip()
            refs.add(path)
    return refs


def is_covered(example: str, refs: set[str]) -> bool:
    """An example is covered only if it is referenced exactly by its own path."""
    return example in refs


def main() -> int:
    parser = argparse.ArgumentParser(description='Check that all IDF examples are referenced in the programming guide.')
    parser.add_argument(
        '--idf-path',
        default=os.environ.get('IDF_PATH', '.'),
        help='Path to the IDF repository root (default: $IDF_PATH or .)',
    )
    args = parser.parse_args()

    idf_path = Path(args.idf_path)
    examples_dir = idf_path / 'examples'
    docs_dir = idf_path / 'docs' / 'en'

    if not examples_dir.is_dir():
        print(f'ERROR: examples directory not found: {examples_dir}', file=sys.stderr)
        return 1
    if not docs_dir.is_dir():
        print(f'ERROR: docs/en directory not found: {docs_dir}', file=sys.stderr)
        return 1

    examples = find_leaf_examples(examples_dir)
    refs = collect_doc_example_refs(docs_dir)

    newly_missing = []
    fixed = []

    for ex in examples:
        covered = is_covered(ex, refs)
        if not covered and ex not in KNOWN_MISSING:
            newly_missing.append(ex)
        elif covered and ex in KNOWN_MISSING:
            fixed.append(ex)

    if fixed:
        print('The following examples were in KNOWN_MISSING but are now referenced in docs.')
        print('Please remove them from KNOWN_MISSING in tools/ci/check_examples_documented.py:')
        for ex in fixed:
            print(f'  {ex}')
        print()

    if newly_missing:
        print('ERROR: The following examples are not referenced anywhere in the programming guide (docs/en/).')
        print('Add a :example:`<path>` role to an appropriate .rst file in docs/en/, then run this check again.')
        print()
        for ex in newly_missing:
            print(f'  {ex}')
        return 1

    if not fixed:
        print(f'OK: all {len(examples)} examples are referenced in the programming guide.')
    return 1 if fixed else 0


if __name__ == '__main__':
    sys.exit(main())
