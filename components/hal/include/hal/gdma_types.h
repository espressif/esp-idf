/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration of peripherals which have the DMA capability
 * @note Some peripheral might not be available on certain chip, please refer to `soc_caps.h` for detail.
 *
 */
typedef enum {
    GDMA_TRIG_PERIPH_M2M,  /*!< GDMA trigger peripheral: M2M */
    GDMA_TRIG_PERIPH_UHCI, /*!< GDMA trigger peripheral: UHCI */
    GDMA_TRIG_PERIPH_SPI,  /*!< GDMA trigger peripheral: SPI */
    GDMA_TRIG_PERIPH_I2S,  /*!< GDMA trigger peripheral: I2S */
    GDMA_TRIG_PERIPH_AES,  /*!< GDMA trigger peripheral: AES */
    GDMA_TRIG_PERIPH_SHA,  /*!< GDMA trigger peripheral: SHA */
    GDMA_TRIG_PERIPH_ADC,  /*!< GDMA trigger peripheral: ADC */
    GDMA_TRIG_PERIPH_DAC,  /*!< GDMA trigger peripheral: DAC */
    GDMA_TRIG_PERIPH_LCD,  /*!< GDMA trigger peripheral: LCD */
    GDMA_TRIG_PERIPH_CAM,  /*!< GDMA trigger peripheral: CAM */
    GDMA_TRIG_PERIPH_RMT,  /*!< GDMA trigger peripheral: RMT */
} gdma_trigger_peripheral_t;

/**
 * @brief Enumeration of GDMA channel direction
 *
 */
typedef enum {
    GDMA_CHANNEL_DIRECTION_TX, /*!< GDMA channel direction: TX */
    GDMA_CHANNEL_DIRECTION_RX, /*!< GDMA channel direction: RX */
} gdma_channel_direction_t;

#ifdef __cplusplus
}
#endif
