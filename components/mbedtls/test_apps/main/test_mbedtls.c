/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* mbedTLS self-tests as unit tests

   Focus on testing functionality where we use ESP32 hardware
   accelerated crypto features.

   See also test_hwcrypto.c in esp32 component, which tests hardware crypto without mbedTLS.
*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <esp_system.h>
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "mbedtls/aes.h"
#include "mbedtls/bignum.h"
#include "mbedtls/rsa.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "sdkconfig.h"
#include "test_apb_dport_access.h"
#include "test_utils.h"

TEST_CASE("mbedtls AES self-tests", "[aes]")
{
    start_apb_access_loop();
    TEST_ASSERT_FALSE_MESSAGE(mbedtls_aes_self_test(1), "AES self-tests should pass.");
    verify_apb_access_loop();
}

TEST_CASE("mbedtls MPI self-tests", "[bignum]")
{
    start_apb_access_loop();
    TEST_ASSERT_FALSE_MESSAGE(mbedtls_mpi_self_test(1), "MPI self-tests should pass.");
    verify_apb_access_loop();
}

TEST_CASE("mbedtls RSA self-tests", "[bignum]")
{
    start_apb_access_loop();
    TEST_ASSERT_FALSE_MESSAGE(mbedtls_rsa_self_test(1), "RSA self-tests should pass.");
    verify_apb_access_loop();
}
