/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include "soc/soc_caps.h"

/**
 * Connection test fixture setup, so we expect the broker is available
 * on network
 */
void connect_test_fixture_setup(void);

/**
 * Cleans up the connection
 */
void connect_test_fixture_teardown(void);
