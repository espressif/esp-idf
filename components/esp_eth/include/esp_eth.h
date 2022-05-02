/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

/**
 * Purpose of this file is to create a common header for the typical ethernet usecase,
 * so using the ethernet driver and the default glue layer to its network interface.
 *
 * If you prefer to create a custom network interface or use the Ethernet as a driver only,
 * then it is recommended to include the "esp_eth_driver.h" only.
 */
#include "esp_eth_driver.h"
#include "esp_eth_netif_glue.h"
