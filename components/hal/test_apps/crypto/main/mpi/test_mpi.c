/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>

#include "esp_private/esp_crypto_lock_internal.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "memory_checks.h"
#include "unity_fixture.h"

#if CONFIG_IDF_TARGET_ESP32
#define ESP_MPI_USE_MONT_EXP
#endif

#include "hal/mpi_hal.h"
#include "hal/mpi_ll.h"
#include "mpi_params.h"

#define _DEBUG_ 0

static void esp_mpi_enable_hardware_hw_op( void )
{
    /* Enable RSA hardware */
    MPI_RCC_ATOMIC() {
        mpi_ll_enable_bus_clock(true);
        mpi_ll_reset_register();
    }

    mpi_hal_enable_hardware_hw_op();
}


static void esp_mpi_disable_hardware_hw_op( void )
{
    mpi_hal_disable_hardware_hw_op();

    /* Disable RSA hardware */
    MPI_RCC_ATOMIC() {
        mpi_ll_enable_bus_clock(false);
    }
}


static void mpi_mul_mpi_mod_hw_op(void)
{
    esp_mpi_enable_hardware_hw_op();
    for(int i = 0; i < TEST_CASES_NUM; i++){
#if CONFIG_IDF_TARGET_ESP32
        mpi_hal_set_mode((test_cases_num_words[i] / 16) - 1);
#else
        mpi_hal_set_mode(test_cases_num_words[i] - 1);
#endif
        /* Load M, X, Rinv, Mprime (Mprime is mod 2^32) */
        mpi_hal_write_to_mem_block(MPI_PARAM_M, 0, test_cases_M_p[i], test_cases_M_n[i], test_cases_num_words[i]);
        mpi_hal_write_to_mem_block(MPI_PARAM_X, 0, test_cases_X_p[i], test_cases_X_n[i], test_cases_num_words[i]);

#if !CONFIG_IDF_TARGET_ESP32
        mpi_hal_write_to_mem_block(MPI_PARAM_Y, 0, test_cases_Y_p[i], test_cases_Y_n[i], test_cases_num_words[i]);
#endif

        mpi_hal_write_to_mem_block(MPI_PARAM_Z, 0, test_cases_Rinv_p[i], test_cases_Rinv_n[i], test_cases_num_words[i]);
        mpi_hal_write_m_prime(test_cases_Mprime[i]);

#if CONFIG_IDF_TARGET_ESP32
        mpi_hal_start_op(MPI_MULT);
        mpi_hal_wait_op_complete();
        /* execute second stage */
        /* Load Y to X input memory block, rerun */
        mpi_hal_write_to_mem_block(MPI_PARAM_X, 0, test_cases_Y_p[i], test_cases_Y_n[i], test_cases_num_words[i]);
        mpi_hal_start_op(MPI_MULT);
#else
        mpi_hal_start_op(MPI_MODMULT);
#endif
        uint32_t* Z_p = (uint32_t*)heap_caps_calloc(test_cases_Z_words[i], sizeof(uint32_t), MALLOC_CAP_INTERNAL);
        mpi_hal_read_result_hw_op(Z_p, test_cases_Z_words[i], test_cases_Z_words[i]);

        printf("Test Case %d: ", i+1);

#if _DEBUG_
        printf("\n");
        ESP_LOG_BUFFER_HEX("Expected Z:", test_cases_Z_p[i], test_cases_Z_words[i]*4);
        ESP_LOG_BUFFER_HEX("Got Z:", Z_p, test_cases_Z_words[i]*4);
#endif

        TEST_ASSERT_EQUAL_HEX32_ARRAY_MESSAGE(test_cases_Z_p[i], Z_p, test_cases_Z_words[i], "Result");
        printf("PASS\n");
        heap_caps_free(Z_p);
    }
    esp_mpi_disable_hardware_hw_op();
}


static void mpi_exp_mpi_mod_hw_op(void)
{

    for (int i = 0; i < EXP_TEST_CASES_NUM; i++) {
        if (i == 14 || i == 16 || i == 18 || i == 22) // cases when Y == 0 (in Z = X ^ Y mod M) should be handled in the software level
            continue;
#ifdef ESP_MPI_USE_MONT_EXP     // CONFIG_IDF_TARGET_ESP32
        printf("Support for montgomery exponentiation to be added.\n");
        break;
#else
        esp_mpi_enable_hardware_hw_op();

        mpi_hal_set_mode(exp_test_cases_num_words[i] - 1);
        mpi_hal_write_to_mem_block(MPI_PARAM_X, 0, exp_test_cases_X_p[i], exp_test_cases_X_n[i], exp_test_cases_num_words[i]);
        mpi_hal_write_to_mem_block(MPI_PARAM_Y, 0, exp_test_cases_Y_p[i], exp_test_cases_Y_n[i], exp_test_cases_num_words[i]);
        mpi_hal_write_to_mem_block(MPI_PARAM_M, 0, exp_test_cases_M_p[i], exp_test_cases_M_n[i], exp_test_cases_num_words[i]);
        mpi_hal_write_to_mem_block(MPI_PARAM_Z, 0, exp_test_cases_Rinv_p[i], exp_test_cases_Rinv_n[i], exp_test_cases_num_words[i]);

        mpi_hal_write_m_prime(exp_test_cases_Mprime[i]);

        /* Enable acceleration options */
        mpi_hal_enable_constant_time(false);
        mpi_hal_enable_search(true);
        mpi_hal_set_search_position(exp_test_cases_y_bits[i] - 1);

        /* Execute first stage montgomery multiplication */
        mpi_hal_start_op(MPI_MODEXP);

        mpi_hal_enable_search(false);
#endif
        uint32_t* Z_p = (uint32_t*)heap_caps_calloc(exp_test_cases_m_words[i], sizeof(uint32_t), MALLOC_CAP_INTERNAL);
        /* Read back the result */
        mpi_hal_read_result_hw_op(Z_p, exp_test_cases_m_words[i], exp_test_cases_m_words[i]);
        esp_mpi_disable_hardware_hw_op();

        printf("Test Case %d: ", i+1);

#if _DEBUG_
        printf("\n");
        ESP_LOG_BUFFER_HEX("Expected Z:", test_cases_Z_p[i], test_cases_Z_words[i]*4);
        ESP_LOG_BUFFER_HEX("Got Z:", Z_p, test_cases_Z_words[i]*4);
#endif

        TEST_ASSERT_EQUAL_HEX32_ARRAY_MESSAGE(exp_test_cases_Z_p[i], Z_p, exp_test_cases_m_words[i], "Result");
        printf("PASS\n");
        heap_caps_free(Z_p);
    }
}

TEST_GROUP(mpi);

TEST_SETUP(mpi)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(mpi)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

TEST(mpi, mpi_multiplication)
{
    mpi_mul_mpi_mod_hw_op();
}

TEST(mpi, mpi_exponentiation)
{
    mpi_exp_mpi_mod_hw_op();
}

TEST_GROUP_RUNNER(mpi)
{
    RUN_TEST_CASE(mpi, mpi_multiplication);
    RUN_TEST_CASE(mpi, mpi_exponentiation);
}
