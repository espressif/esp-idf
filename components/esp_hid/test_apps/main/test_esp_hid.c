/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include "unity.h"
#include "unity_test_runner.h"
#include "esp_log.h"
#include "esp_hid_common.h"
#include "hid_descriptor.h"

TEST_CASE("can parse hidReportMap", "[esp_hid]")
{
    esp_hid_report_map_t * report_map = esp_hid_parse_report_map(hidReportMap, sizeof(hidReportMap));
    TEST_ASSERT_NOT_NULL(report_map);
    TEST_ASSERT(report_map->usage == ESP_HID_USAGE_KEYBOARD);
    TEST_ASSERT(report_map->appearance == 0x03C1);
    TEST_ASSERT(report_map->reports_len == 8);
    TEST_ASSERT(report_map->reports[0].report_id == 1);
    TEST_ASSERT(report_map->reports[0].report_type == ESP_HID_REPORT_TYPE_INPUT);
    TEST_ASSERT(report_map->reports[0].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT);
    TEST_ASSERT(report_map->reports[0].usage == ESP_HID_USAGE_MOUSE);
    TEST_ASSERT(report_map->reports[0].value_len == 4);
    TEST_ASSERT(report_map->reports[1].report_id == 1);
    TEST_ASSERT(report_map->reports[1].report_type == ESP_HID_REPORT_TYPE_INPUT);
    TEST_ASSERT(report_map->reports[1].protocol_mode == ESP_HID_PROTOCOL_MODE_BOOT);
    TEST_ASSERT(report_map->reports[1].usage == ESP_HID_USAGE_MOUSE);
    TEST_ASSERT(report_map->reports[1].value_len == 3);
    TEST_ASSERT(report_map->reports[2].report_id == 2);
    TEST_ASSERT(report_map->reports[2].report_type == ESP_HID_REPORT_TYPE_INPUT);
    TEST_ASSERT(report_map->reports[2].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT);
    TEST_ASSERT(report_map->reports[2].usage == ESP_HID_USAGE_KEYBOARD);
    TEST_ASSERT(report_map->reports[2].value_len == 8);
    TEST_ASSERT(report_map->reports[3].report_id == 2);
    TEST_ASSERT(report_map->reports[3].report_type == ESP_HID_REPORT_TYPE_INPUT);
    TEST_ASSERT(report_map->reports[3].protocol_mode == ESP_HID_PROTOCOL_MODE_BOOT);
    TEST_ASSERT(report_map->reports[3].usage == ESP_HID_USAGE_KEYBOARD);
    TEST_ASSERT(report_map->reports[3].value_len == 8);
    TEST_ASSERT(report_map->reports[4].report_id == 2);
    TEST_ASSERT(report_map->reports[4].report_type == ESP_HID_REPORT_TYPE_OUTPUT);
    TEST_ASSERT(report_map->reports[4].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT);
    TEST_ASSERT(report_map->reports[4].usage == ESP_HID_USAGE_KEYBOARD);
    TEST_ASSERT(report_map->reports[4].value_len == 1);
    TEST_ASSERT(report_map->reports[5].report_id == 2);
    TEST_ASSERT(report_map->reports[5].report_type == ESP_HID_REPORT_TYPE_OUTPUT);
    TEST_ASSERT(report_map->reports[5].protocol_mode == ESP_HID_PROTOCOL_MODE_BOOT);
    TEST_ASSERT(report_map->reports[5].usage == ESP_HID_USAGE_KEYBOARD);
    TEST_ASSERT(report_map->reports[5].value_len == 1);
    TEST_ASSERT(report_map->reports[6].report_id == 3);
    TEST_ASSERT(report_map->reports[6].report_type == ESP_HID_REPORT_TYPE_INPUT);
    TEST_ASSERT(report_map->reports[6].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT);
    TEST_ASSERT(report_map->reports[6].usage == ESP_HID_USAGE_CCONTROL);
    TEST_ASSERT(report_map->reports[6].value_len == 2);
    TEST_ASSERT(report_map->reports[7].report_id == 4);
    TEST_ASSERT(report_map->reports[7].report_type == ESP_HID_REPORT_TYPE_OUTPUT);
    TEST_ASSERT(report_map->reports[7].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT);
    TEST_ASSERT(report_map->reports[7].usage == ESP_HID_USAGE_VENDOR);
    TEST_ASSERT(report_map->reports[7].value_len == 127);
    esp_hid_free_report_map(report_map);
}

TEST_CASE("can parse relMouseReportMap", "[esp_hid]")
{
    esp_hid_report_map_t * report_map = esp_hid_parse_report_map(relMouseReportMap, sizeof(relMouseReportMap));
    TEST_ASSERT_NOT_NULL(report_map);
    TEST_ASSERT(report_map->usage == ESP_HID_USAGE_MOUSE);
    TEST_ASSERT(report_map->appearance == 0x03C2);
    TEST_ASSERT(report_map->reports_len == 2);
    TEST_ASSERT(report_map->reports[0].report_id == 1);
    TEST_ASSERT(report_map->reports[0].report_type == ESP_HID_REPORT_TYPE_INPUT);
    TEST_ASSERT(report_map->reports[0].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT);
    TEST_ASSERT(report_map->reports[0].usage == ESP_HID_USAGE_MOUSE);
    TEST_ASSERT(report_map->reports[0].value_len == 4);
    TEST_ASSERT(report_map->reports[1].report_id == 1);
    TEST_ASSERT(report_map->reports[1].report_type == ESP_HID_REPORT_TYPE_INPUT);
    TEST_ASSERT(report_map->reports[1].protocol_mode == ESP_HID_PROTOCOL_MODE_BOOT);
    TEST_ASSERT(report_map->reports[1].usage == ESP_HID_USAGE_MOUSE);
    TEST_ASSERT(report_map->reports[1].value_len == 3);
    esp_hid_free_report_map(report_map);
}

TEST_CASE("can parse absMouseReportMap", "[esp_hid]")
{
    esp_hid_report_map_t * report_map = esp_hid_parse_report_map(absMouseReportMap, sizeof(absMouseReportMap));
    TEST_ASSERT_NOT_NULL(report_map);
    TEST_ASSERT(report_map->usage == ESP_HID_USAGE_MOUSE);
    TEST_ASSERT(report_map->appearance == 0x03C2);
    TEST_ASSERT(report_map->reports_len == 2);
    TEST_ASSERT(report_map->reports[0].report_id == 1);
    TEST_ASSERT(report_map->reports[0].report_type == ESP_HID_REPORT_TYPE_INPUT);
    TEST_ASSERT(report_map->reports[0].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT);
    TEST_ASSERT(report_map->reports[0].usage == ESP_HID_USAGE_MOUSE);
    TEST_ASSERT(report_map->reports[0].value_len == 6);
    TEST_ASSERT(report_map->reports[1].report_id == 1);
    TEST_ASSERT(report_map->reports[1].report_type == ESP_HID_REPORT_TYPE_INPUT);
    TEST_ASSERT(report_map->reports[1].protocol_mode == ESP_HID_PROTOCOL_MODE_BOOT);
    TEST_ASSERT(report_map->reports[1].usage == ESP_HID_USAGE_MOUSE);
    TEST_ASSERT(report_map->reports[1].value_len == 3);
    esp_hid_free_report_map(report_map);
}

TEST_CASE("can parse keyboardReportMap", "[esp_hid]")
{
    esp_hid_report_map_t * report_map = esp_hid_parse_report_map(keyboardReportMap, sizeof(keyboardReportMap));
    TEST_ASSERT_NOT_NULL(report_map);
    TEST_ASSERT(report_map->usage == ESP_HID_USAGE_KEYBOARD);
    TEST_ASSERT(report_map->appearance == 0x03C1);
    TEST_ASSERT(report_map->reports_len == 4);
    TEST_ASSERT(report_map->reports[0].report_id == 1);
    TEST_ASSERT(report_map->reports[0].report_type == ESP_HID_REPORT_TYPE_INPUT);
    TEST_ASSERT(report_map->reports[0].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT);
    TEST_ASSERT(report_map->reports[0].usage == ESP_HID_USAGE_KEYBOARD);
    TEST_ASSERT(report_map->reports[0].value_len == 7);
    TEST_ASSERT(report_map->reports[1].report_id == 1);
    TEST_ASSERT(report_map->reports[1].report_type == ESP_HID_REPORT_TYPE_INPUT);
    TEST_ASSERT(report_map->reports[1].protocol_mode == ESP_HID_PROTOCOL_MODE_BOOT);
    TEST_ASSERT(report_map->reports[1].usage == ESP_HID_USAGE_KEYBOARD);
    TEST_ASSERT(report_map->reports[1].value_len == 8);
    TEST_ASSERT(report_map->reports[2].report_id == 1);
    TEST_ASSERT(report_map->reports[2].report_type == ESP_HID_REPORT_TYPE_OUTPUT);
    TEST_ASSERT(report_map->reports[2].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT);
    TEST_ASSERT(report_map->reports[2].usage == ESP_HID_USAGE_KEYBOARD);
    TEST_ASSERT(report_map->reports[2].value_len == 1);
    TEST_ASSERT(report_map->reports[3].report_id == 1);
    TEST_ASSERT(report_map->reports[3].report_type == ESP_HID_REPORT_TYPE_OUTPUT);
    TEST_ASSERT(report_map->reports[3].protocol_mode == ESP_HID_PROTOCOL_MODE_BOOT);
    TEST_ASSERT(report_map->reports[3].usage == ESP_HID_USAGE_KEYBOARD);
    TEST_ASSERT(report_map->reports[3].value_len == 1);
    esp_hid_free_report_map(report_map);
}

TEST_CASE("can parse joystickReportMap", "[esp_hid]")
{
    esp_hid_report_map_t * report_map = esp_hid_parse_report_map(joystickReportMap, sizeof(joystickReportMap));
    TEST_ASSERT_NOT_NULL(report_map);
    TEST_ASSERT(report_map->usage == ESP_HID_USAGE_JOYSTICK);
    TEST_ASSERT(report_map->appearance == 0x03C3);
    TEST_ASSERT(report_map->reports_len == 1);
    TEST_ASSERT(report_map->reports[0].report_id == 1);
    TEST_ASSERT(report_map->reports[0].report_type == ESP_HID_REPORT_TYPE_INPUT);
    TEST_ASSERT(report_map->reports[0].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT);
    TEST_ASSERT(report_map->reports[0].usage == ESP_HID_USAGE_JOYSTICK);
    TEST_ASSERT(report_map->reports[0].value_len == 8);
    esp_hid_free_report_map(report_map);
}

TEST_CASE("can parse mediaReportMap", "[esp_hid]")
{
    esp_hid_report_map_t * report_map = esp_hid_parse_report_map(mediaReportMap, sizeof(mediaReportMap));
    TEST_ASSERT_NOT_NULL(report_map);
    TEST_ASSERT(report_map->usage == ESP_HID_USAGE_CCONTROL);
    TEST_ASSERT(report_map->appearance == 0x03C1);
    TEST_ASSERT(report_map->reports_len == 1);
    TEST_ASSERT(report_map->reports[0].report_id == 3);
    TEST_ASSERT(report_map->reports[0].report_type == ESP_HID_REPORT_TYPE_INPUT);
    TEST_ASSERT(report_map->reports[0].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT);
    TEST_ASSERT(report_map->reports[0].usage == ESP_HID_USAGE_CCONTROL);
    TEST_ASSERT(report_map->reports[0].value_len == 6);
    esp_hid_free_report_map(report_map);
}

TEST_CASE("can parse mediaReportMap2", "[esp_hid]")
{
    esp_hid_report_map_t * report_map = esp_hid_parse_report_map(mediaReportMap2, sizeof(mediaReportMap2));
    TEST_ASSERT_NOT_NULL(report_map);
    TEST_ASSERT(report_map->usage == ESP_HID_USAGE_CCONTROL);
    TEST_ASSERT(report_map->appearance == 0x03C1);
    TEST_ASSERT(report_map->reports_len == 1);
    TEST_ASSERT(report_map->reports[0].report_id == 3);
    TEST_ASSERT(report_map->reports[0].report_type == ESP_HID_REPORT_TYPE_INPUT);
    TEST_ASSERT(report_map->reports[0].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT);
    TEST_ASSERT(report_map->reports[0].usage == ESP_HID_USAGE_CCONTROL);
    TEST_ASSERT(report_map->reports[0].value_len == 2);
    esp_hid_free_report_map(report_map);
}

TEST_CASE("can parse hidapiReportMap", "[esp_hid]")
{
    esp_hid_report_map_t * report_map = esp_hid_parse_report_map(hidapiReportMap, sizeof(hidapiReportMap));
    TEST_ASSERT_NOT_NULL(report_map);
    TEST_ASSERT(report_map->usage == ESP_HID_USAGE_GENERIC);
    TEST_ASSERT(report_map->appearance == 0x03C0);
    TEST_ASSERT(report_map->reports_len == 3);
    TEST_ASSERT(report_map->reports[0].report_id == 1);
    TEST_ASSERT(report_map->reports[0].report_type == ESP_HID_REPORT_TYPE_INPUT);
    TEST_ASSERT(report_map->reports[0].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT);
    TEST_ASSERT(report_map->reports[0].usage == ESP_HID_USAGE_VENDOR);
    TEST_ASSERT(report_map->reports[0].value_len == 8);
    TEST_ASSERT(report_map->reports[1].report_id == 1);
    TEST_ASSERT(report_map->reports[1].report_type == ESP_HID_REPORT_TYPE_OUTPUT);
    TEST_ASSERT(report_map->reports[1].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT);
    TEST_ASSERT(report_map->reports[1].usage == ESP_HID_USAGE_VENDOR);
    TEST_ASSERT(report_map->reports[1].value_len == 8);
    TEST_ASSERT(report_map->reports[2].report_id == 1);
    TEST_ASSERT(report_map->reports[2].report_type == ESP_HID_REPORT_TYPE_FEATURE);
    TEST_ASSERT(report_map->reports[2].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT);
    TEST_ASSERT(report_map->reports[2].usage == ESP_HID_USAGE_VENDOR);
    TEST_ASSERT(report_map->reports[2].value_len == 8);
    esp_hid_free_report_map(report_map);
}
