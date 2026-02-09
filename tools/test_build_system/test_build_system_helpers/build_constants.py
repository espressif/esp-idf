# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
BOOTLOADER_BINS = ['build/bootloader/bootloader.elf', 'build/bootloader/bootloader.bin']
APP_BINS = ['build/build_test_app.elf', 'build/build_test_app.bin']
PARTITION_BIN = ['build/partition_table/partition-table.bin']
JSON_METADATA = ['build/project_description.json', 'build/flasher_args.json', 'build/config/sdkconfig.json']

# kconfig_menus.json is only generated during build in cmakev1.
KCONFIG_MENUS_JSON = ['build/config/kconfig_menus.json']

# Build system v1 artifacts
ALL_ARTIFACTS_BUILDV1 = [*BOOTLOADER_BINS, *APP_BINS, *PARTITION_BIN, *JSON_METADATA, *KCONFIG_MENUS_JSON]

# Build system v2 artifacts
ALL_ARTIFACTS_BUILDV2 = [*BOOTLOADER_BINS, *APP_BINS, *PARTITION_BIN, *JSON_METADATA]

# Default artifacts for tests that don't specify build version
ALL_ARTIFACTS = ALL_ARTIFACTS_BUILDV1
