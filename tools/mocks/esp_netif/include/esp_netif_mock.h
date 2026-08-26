/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * Since esp_netif_obj is an opaque type (not defined, just declared), we define it here
 * since its size is required by some of the mock functions.
 */
struct esp_netif_obj {
    void *dummy;
};
