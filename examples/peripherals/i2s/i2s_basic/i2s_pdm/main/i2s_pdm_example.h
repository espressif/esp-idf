/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define EXAMPLE_BUFF_SIZE   2048

/**
 * @brief I2S PDM TX example task
 *
 * @param args  The user data given from task creating, not used in this example
 */
void i2s_example_pdm_tx_task(void *args);

/**
 * @brief I2S PDM RX example task
 *
 * @param args  The user data given from task creating, not used in this example
 */
void i2s_example_pdm_rx_task(void *args);

#ifdef __cplusplus
}
#endif
