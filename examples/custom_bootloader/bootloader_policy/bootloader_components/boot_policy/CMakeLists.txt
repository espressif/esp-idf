# SPDX-FileCopyrightText: 2026 Matterize Labs (matterizelabs.com)
#
# SPDX-License-Identifier: Unlicense OR CC0-1.0

idf_component_register(SRCS "boot_policy.c"
                       "policy_core.c"
                       INCLUDE_DIRS "."
                       REQUIRES bootloader_support spi_flash)
