/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "soc/soc_caps.h"
#include "hal/dma_types.h"
#include "hal/adc_types.h"
#include "hal/adc_ll.h"
#include "hal/adc_hal_common.h"
#include "esp_err.h"

#if SOC_GDMA_SUPPORTED
#include "soc/gdma_struct.h"
#include "hal/gdma_ll.h"
#endif

#if CONFIG_IDF_TARGET_ESP32S2
//ADC utilises SPI3 DMA on ESP32S2
#include "hal/spi_ll.h"
#endif

#if CONFIG_IDF_TARGET_ESP32
//ADC utilises I2S0 DMA on ESP32
#include "hal/i2s_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_GDMA_SUPPORTED
#define ADC_HAL_DMA_INTR_MASK                           GDMA_LL_EVENT_RX_SUC_EOF
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_HAL_DMA_INTR_MASK                           SPI_LL_INTR_IN_SUC_EOF
#else //CONFIG_IDF_TARGET_ESP32
#define ADC_HAL_DMA_INTR_MASK                           BIT(9)
#endif

/**
 * @brief Enum for DMA descriptor status
 */
typedef enum adc_hal_dma_desc_status_t {
    ADC_HAL_DMA_DESC_VALID   = 0,            ///< This DMA descriptor is written by HW already
    ADC_HAL_DMA_DESC_WAITING = 1,            ///< This DMA descriptor is not written by HW yet
    ADC_HAL_DMA_DESC_NULL    = 2             ///< This DMA descriptor is NULL
} adc_hal_dma_desc_status_t;

/**
 * @brief Configuration of the HAL
 */
typedef struct adc_hal_dma_config_t {
    void                *dev;               ///< DMA peripheral address
    uint32_t            eof_desc_num;       ///< Number of dma descriptors that is eof
    uint32_t            eof_step;           ///< Number of linked descriptors that is one eof
    uint32_t            dma_chan;           ///< DMA channel to be used
    uint32_t            eof_num;            ///< Bytes between 2 in_suc_eof interrupts
} adc_hal_dma_config_t;

/**
 * @brief Context of the HAL
 */
typedef struct adc_hal_dma_ctx_t {
    /**< this needs to be malloced by the driver layer first */
    dma_descriptor_t    *rx_desc;           ///< DMA descriptors

    /**< these will be assigned by hal layer itself */
    dma_descriptor_t    desc_dummy_head;    ///< Dummy DMA descriptor for ``cur_desc_ptr`` to start
    dma_descriptor_t    *cur_desc_ptr;      ///< Pointer to the current descriptor

    /**< these need to be configured by `adc_hal_dma_config_t` via driver layer*/
    void                *dev;               ///< DMA address
    uint32_t            eof_desc_num;       ///< Number of dma descriptors that is eof
    uint32_t            eof_step;           ///< Number of linked descriptors that is one eof
    uint32_t            dma_chan;           ///< DMA channel to be used
    uint32_t            eof_num;            ///< Words between 2 in_suc_eof interrupts
} adc_hal_dma_ctx_t;

typedef struct adc_hal_digi_ctrlr_cfg_t {
    uint32_t                    adc_pattern_len;    //total pattern item number, including ADC1 and ADC2
    adc_digi_pattern_config_t   *adc_pattern;       //pattern item
    uint32_t                    sample_freq_hz;     //ADC sample frequency
    adc_digi_convert_mode_t     conv_mode;          //controller work mode
    uint32_t                    bit_width;          //output data width
    adc_continuous_clk_src_t    clk_src;            ///< Clock source
    uint32_t                    clk_src_freq_hz;    ///< Clock source frequency in hz
} adc_hal_digi_ctrlr_cfg_t;


/*---------------------------------------------------------------
                    PWDET(Power detect) controller setting
---------------------------------------------------------------*/
/**
 * Set adc cct for PWDET controller.
 *
 * @note Capacitor tuning of the PA power monitor. cct set to the same value with PHY.
 * @prarm cct Range: 0 ~ 7.
 */
#define adc_hal_pwdet_set_cct(cct) adc_ll_pwdet_set_cct(cct)

/**
 * Get adc cct for PWDET controller.
 *
 * @note Capacitor tuning of the PA power monitor. cct set to the same value with PHY.
 * @return cct Range: 0 ~ 7.
 */
#define adc_hal_pwdet_get_cct() adc_ll_pwdet_get_cct()

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/
/**
 * @brief Initialize the HW
 *
 * @param hal Context of the HAL
 */
void adc_hal_digi_init(adc_hal_dma_ctx_t *hal);

/**
 * Digital controller deinitialization.
 *
 * @param hal Context of the HAL
 */
void adc_hal_digi_deinit(adc_hal_dma_ctx_t *hal);

/**
 * @brief Initialize the hal context
 *
 * @param hal    Context of the HAL
 * @param config Configuration of the HAL
 */
void adc_hal_dma_ctx_config(adc_hal_dma_ctx_t *hal, const adc_hal_dma_config_t *config);

/**
 * Setting the digital controller.
 *
 * @param hal    Context of the HAL
 * @param cfg    Pointer to digital controller paramter.
 */
void adc_hal_digi_controller_config(adc_hal_dma_ctx_t *hal, const adc_hal_digi_ctrlr_cfg_t *cfg);

/**
 * @brief Start Conversion
 *
 * @param hal Context of the HAL
 * @param data_buf Pointer to the data buffer, the length should be multiple of ``desc_max_num`` and ``eof_num`` in ``adc_hal_dma_ctx_t``
 */
void adc_hal_digi_start(adc_hal_dma_ctx_t *hal, uint8_t *data_buf);

#if !SOC_GDMA_SUPPORTED
/**
 * @brief Get the DMA descriptor that Hardware has finished processing.
 *
 * @param hal Context of the HAL
 *
 * @return DMA descriptor address
 */
intptr_t adc_hal_get_desc_addr(adc_hal_dma_ctx_t *hal);

/**
 * @brief Check the hardware interrupt event
 *
 * @param hal Context of the HAL
 * @param mask Event mask
 *
 * @return True: the event is triggered. False: the event is not triggered yet.
 */
bool adc_hal_check_event(adc_hal_dma_ctx_t *hal, uint32_t mask);
#endif

/**
 * @brief Get the ADC reading result
 *
 * @param      hal           Context of the HAL
 * @param      eof_desc_addr The last descriptor that is finished by HW. Should be got from DMA
 * @param[out] buffer        ADC reading result buffer
 * @param[out] len           ADC reading result len
 *
 * @return                   See ``adc_hal_dma_desc_status_t``
 */
adc_hal_dma_desc_status_t adc_hal_get_reading_result(adc_hal_dma_ctx_t *hal, const intptr_t eof_desc_addr, uint8_t **buffer, uint32_t *len);

/**
 * @brief Clear interrupt
 *
 * @param hal  Context of the HAL
 * @param mask mask of the interrupt
 */
void adc_hal_digi_clr_intr(adc_hal_dma_ctx_t *hal, uint32_t mask);

/**
 * @brief Enable interrupt
 *
 * @param hal  Context of the HAL
 * @param mask mask of the interrupt
 */
void adc_hal_digi_dis_intr(adc_hal_dma_ctx_t *hal, uint32_t mask);

/**
 * @brief Stop conversion
 *
 * @param hal Context of the HAL
 */
void adc_hal_digi_stop(adc_hal_dma_ctx_t *hal);

#if ADC_LL_WORKAROUND_CLEAR_EOF_COUNTER
/**
 * @brief Clear the ADC sample counter
 */
void adc_hal_digi_clr_eof(void);
#endif

#ifdef __cplusplus
}
#endif
