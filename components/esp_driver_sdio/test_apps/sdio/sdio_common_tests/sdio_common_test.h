/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_RX_BUFFER_SIZE           2048

/*---------------------------------------------------------------
                Function Tests
---------------------------------------------------------------*/
/*---------------------------------------------------------------
                SDMMC_SDIO: test reset
---------------------------------------------------------------*/
#define TEST_RESET_DATA_LEN           12
#define TEST_RESET_BUF_NUMS           10

/*---------------------------------------------------------------
                Transaction Tests
---------------------------------------------------------------*/
#define TEST_TARNS_PARAM_NUMS         2
/*---------------------------------------------------------------
                SDMMC_SDIO: test from host
---------------------------------------------------------------*/
#define TEST_TRANS_NUMS               10000

#ifdef __cplusplus
}
#endif
