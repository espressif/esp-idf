# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: CC0-1.0

# BLE Log USB CDC smoke app for chips with a USB OTG peripheral
# (Full- or High-Speed), e.g. ESP32-S2/S3/S31/P4:
#   idf.py -C components/bt/common/ble_log/test_apps/ble_log_usb_test set-target esp32s31 build flash monitor

cmake_minimum_required(VERSION 3.22)

list(PREPEND SDKCONFIG_DEFAULTS
    "$ENV{IDF_PATH}/tools/test_apps/configs/sdkconfig.debug_helpers"
    "sdkconfig.defaults")
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ble_log_usb_test)
