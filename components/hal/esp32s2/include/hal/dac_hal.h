/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for DAC (esp32s2 specific part)

#pragma once

#include "hal/dac_ll.h"
#include "hal/dac_types.h"
#include "hal/spi_ll.h"
#include "soc/lldesc.h"

#include_next "hal/dac_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DAC_DMA_HAL_INTR    (SPI_LL_INTR_OUT_TOTAL_EOF)

typedef struct {
    void *dev;
    uint32_t dma_chan;
} dac_hal_context_t;

typedef struct {
    dac_digi_convert_mode_t mode;
    uint32_t interval;
} dac_hal_ctrl_config_t;


/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/
/**
 * Digital controller initialization.
 */
void dac_hal_digi_init(void);

/**
 * Digital controller deinitialization.
 */
void dac_hal_digi_deinit(void);

/**
 * Setting the DAC digital controller.
 *
 * @param cfg Pointer to digital controller paramter.
 */
void dac_hal_digi_controller_configure(const dac_hal_ctrl_config_t *ctrl_cfg);

/**
 * DAC digital controller start output voltage.
 */
void dac_hal_digi_start(void);

/**
 * DAC digital controller stop output voltage.
 */
void dac_hal_digi_stop(void);

/**
 * Reset DAC digital controller FIFO.
 */
#define dac_hal_digi_fifo_reset() dac_ll_digi_fifo_reset()

/**
 * Reset DAC digital controller.
 */
#define dac_hal_digi_reset() dac_ll_digi_reset()

/*******************************************************
 * DAC-DMA hal layer functions.
 * On ESP32-S2, DAC shares the DMA with SPI3.
*******************************************************/

/**
 * DAC DMA HAL initialization
 *
 * @param hal Context of the HAL layer
 */
void dac_dma_hal_init(dac_hal_context_t *hal);

/**
 * DAC DMA HAL interrupt clear.
 *
 * @param hal Context of the HAL layer
 */
void dac_dma_hal_clr_intr(dac_hal_context_t *hal);

/**
 * DAC DMA HAL transaction start.
 *
 * @param hal Context of the HAL layer
 */
void dac_dma_hal_trans_start(dac_hal_context_t *hal, lldesc_t *desc);

/**
 * Get if interrupt is triggered or not.
 *
 * @param hal Context of the HAL layer
 *
 * @return if the intr is triggered. 1: triggered, 0: not triggered.
 */
bool dac_dma_hal_get_intr_status(dac_hal_context_t *hal);

/**
 * DAC DMA HAL deinitialization
 *
 * @param hal Context of the HAL layer
 */
void dac_dma_hal_deinit(dac_hal_context_t *hal);

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
void __attribute__((deprecated)) dac_hal_digi_controller_config(const dac_digi_config_t *cfg);

#ifdef __cplusplus
}
#endif
