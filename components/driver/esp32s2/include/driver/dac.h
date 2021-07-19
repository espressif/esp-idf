/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/dac_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DAC digital controller (DMA mode) configuration parameters.
 */
typedef struct {
    dac_digi_convert_mode_t mode;           /*!< DAC digital controller (DMA mode) work mode. See ``dac_digi_convert_mode_t``. */
    uint32_t dac_chan_msk;                  /*!< DAC channel select mask, use BIT(CHAN_X) to mask the channesl */
    uint32_t interval;                      /*!< The number of interval clock cycles for the DAC digital controller to output voltage.
                                                 The unit is the divided clock. Range: 1 ~ 4095.
                                                 Expression: `dac_output_freq` = `controller_clk` / interval. Refer to ``adc_digi_clk_t``.
                                                 Note: The sampling rate of each channel is also related to the conversion mode (See ``dac_digi_convert_mode_t``) and pattern table settings. */
    uint32_t dac_dma_cnt;                   /*!< DMA buffer count, number of buffer. */
    uint32_t dac_dma_length;                /*!< DMA buffer length, length of each buffer. */
    dac_dma_link_type_t dac_dma_link_type;  /*!< The type of the link, see `dac_dma_link_type_t` */
} dac_digi_init_config_t;

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/

/**
 * @brief Initialize the Digital DAC controller.
 *
 * @param init_cfg Pointer to Digital DAC initialization config. Refer to ``dac_digi_config_t``.
 *
 * @return
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_NOT_FOUND     No free interrupt found with the specified flags
 *         - ESP_ERR_NO_MEM        If out of memory
 *         - ESP_OK                On success
 */
esp_err_t dac_digi_initialize(const dac_digi_init_config_t *init_cfg);

/**
 * @brief Deinitialize the Digital DAC controller.
 *
 * @return
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 *         - ESP_OK                On success
 */
esp_err_t dac_digi_deinitialize(void);

/**
 * @brief Write bytes to Digital DAC through DMA.
 *
 * @param length the buffer length.
 * @param[in] buffer Buffer to write to DAC.
 * @param ticks_to_wait Ticks to wait until there's room in the queue; use portMAX_DELAY to
 *                      never time out.
 *
 * @return
 *         - ESP_ERR_INVALID_STATE Driver state is invalid. Usually it means the ADC sampling rate is faster than the task processing rate.
 *         - ESP_ERR_TIMEOUT       Operation timed out
 *         - ESP_OK                On success
 */
esp_err_t dac_digi_write_bytes(uint32_t length, const void *buffer, TickType_t ticks_to_wait);

/**
 * @brief DAC digital controller start output voltage.
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_digi_start(void);

/**
 * @brief DAC digital controller stop output voltage.
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_digi_stop(void);

/**
 * @brief Reset DAC digital controller FIFO.
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_digi_fifo_reset(void);

/**
 * @brief Reset DAC digital controller.
 * @return
 *     - ESP_OK success
 */
esp_err_t dac_digi_reset(void);

/************************************************************
 *                    Deprecated APIs
 ***********************************************************/
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
/**
 * @brief Setting the DAC digital controller.
 *
 * @param cfg Pointer to digital controller paramter. See ``dac_digi_config_t``.
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t __attribute__((deprecated)) dac_digi_controller_config(const dac_digi_config_t *cfg);

/**
 * @brief DAC digital controller deinitialization.
 * @return
 *     - ESP_OK success
 */
esp_err_t __attribute__((deprecated)) dac_digi_deinit(void);

/**
 * @brief DAC digital controller initialization.
 * @return
 *     - ESP_OK success
 */
esp_err_t __attribute__((deprecated)) dac_digi_init(void);

#ifdef __cplusplus
}
#endif
