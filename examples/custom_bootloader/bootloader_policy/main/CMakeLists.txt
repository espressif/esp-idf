# SPDX-FileCopyrightText: 2026 Matterize Labs (matterizelabs.com)
#
# SPDX-License-Identifier: Unlicense OR CC0-1.0

idf_component_register(SRCS "main.c" "../bootloader_components/boot_policy/policy_core.c"
                       INCLUDE_DIRS "." "../bootloader_components/boot_policy"
                       REQUIRES esp_partition esp_timer)
