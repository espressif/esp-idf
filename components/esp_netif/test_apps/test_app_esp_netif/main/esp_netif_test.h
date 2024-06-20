/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include "unity.h"
#include "unity_fixture.h"
#include "esp_netif.h"

// List of tests that are common for both configurations
void create_delete_multiple_netifs(void);
void get_from_if_key(void);
