# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
BOOTLOADER_BINS = ['build/bootloader/bootloader.elf', 'build/bootloader/bootloader.bin']
APP_BINS = ['build/build_test_app.elf', 'build/build_test_app.bin']
PARTITION_BIN = ['build/partition_table/partition-table.bin']
JSON_METADATA = ['build/project_description.json', 'build/flasher_args.json', 'build/config/kconfig_menus.json', 'build/config/sdkconfig.json']
ALL_ARTIFACTS = [
    *BOOTLOADER_BINS,
    *APP_BINS,
    *PARTITION_BIN,
    *JSON_METADATA
]
