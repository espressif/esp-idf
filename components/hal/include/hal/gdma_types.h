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
    GDMA_TRIG_PERIPH_PARLIO, /*!< GDMA trigger peripheral: PARLIO */
    GDMA_TRIG_PERIPH_I3C,  /*!< GDMA trigger peripheral: I3C */
} gdma_trigger_peripheral_t;

/**
 * @brief Enumeration of GDMA channel direction
 */
typedef enum {
    GDMA_CHANNEL_DIRECTION_TX, /*!< GDMA channel direction: TX */
    GDMA_CHANNEL_DIRECTION_RX, /*!< GDMA channel direction: RX */
} gdma_channel_direction_t;

/**
 * @brief GDMA channel events that supported by the ETM module
 */
typedef enum {
    GDMA_ETM_EVENT_EOF, /*!< Event that the GDMA engine meets EOF descriptor */
    GDMA_ETM_EVENT_MAX, /*!< Maximum number of events */
} gdma_etm_event_type_t;

/**
 * @brief GDMA channel tasks that supported by the ETM module
 */
typedef enum {
    GDMA_ETM_TASK_START, /*!< Start the GDMA machine */
    GDMA_ETM_TASK_MAX,   /*!< Maximum number of events */
} gdma_etm_task_type_t;

#ifdef __cplusplus
}
#endif
