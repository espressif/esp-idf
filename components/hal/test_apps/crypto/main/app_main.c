/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "unity_fixture.h"
#include "unity_fixture_extras.h"

static void run_all_tests(void)
{
#if CONFIG_SOC_MPI_SUPPORTED
    RUN_TEST_GROUP(mpi);
#endif

#if CONFIG_SOC_ECC_SUPPORTED
    RUN_TEST_GROUP(ecc);
#endif

#if CONFIG_IDF_ENV_FPGA

#if CONFIG_SOC_HMAC_SUPPORTED
    RUN_TEST_GROUP(hmac);
#endif

#if CONFIG_SOC_DIG_SIGN_SUPPORTED
    RUN_TEST_GROUP(ds);
#endif

#if CONFIG_SOC_ECDSA_SUPPORTED
    RUN_TEST_GROUP(ecdsa)
#endif

#endif /* CONFIG_IDF_ENV_FPGA */
}

void app_main(void)
{
    UNITY_MAIN_FUNC(run_all_tests);
}
