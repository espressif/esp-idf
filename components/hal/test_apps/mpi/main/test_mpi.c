/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_private/periph_ctrl.h"
#include "unity.h"

#if CONFIG_IDF_TARGET_ESP32
#define ESP_MPI_USE_MONT_EXP
#endif

#include "hal/mpi_hal.h"
#include "test_params.h"

#define _DEBUG_ 0


static void esp_mpi_enable_hardware_hw_op( void )
{
    /* Enable RSA hardware */
    periph_module_enable(PERIPH_RSA_MODULE);

    mpi_hal_enable_hardware_hw_op();
}


static void esp_mpi_disable_hardware_hw_op( void )
{
    mpi_hal_disable_hardware_hw_op();

    /* Disable RSA hardware */
    periph_module_disable(PERIPH_RSA_MODULE);
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
        uint32_t* Z_p = (uint32_t*)calloc(test_cases_Z_words[i], sizeof(uint32_t));
        mpi_hal_read_result_hw_op(Z_p, test_cases_Z_words[i], test_cases_Z_words[i]);

        printf("Test Case %d: ", i+1);

#if _DEBUG_
        printf("\n");
        ESP_LOG_BUFFER_HEX("Expected Z:", test_cases_Z_p[i], test_cases_Z_words[i]*4);
        ESP_LOG_BUFFER_HEX("Got Z:", Z_p, test_cases_Z_words[i]*4);
#endif

        TEST_ASSERT_EQUAL_HEX32_ARRAY_MESSAGE(test_cases_Z_p[i], Z_p, test_cases_Z_words[i], "Result");
        printf("PASS\n");
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
        uint32_t* Z_p = (uint32_t*)calloc(exp_test_cases_m_words[i], sizeof(uint32_t));
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
    }
}

TEST_CASE("Test MPI multiplication", "[mpi][hal]")
{
    mpi_mul_mpi_mod_hw_op();
}

TEST_CASE("Test MPI exponentiation", "[mpi][hal]")
{
    mpi_exp_mpi_mod_hw_op();
}
