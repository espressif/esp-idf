/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct asrc_dev_t *asrc_soc_handle_t;  // ASRC SOC layer handle

/**
 * @brief  Context of the HAL
 */
typedef struct asrc_hal_context {
    asrc_soc_handle_t  dev;  /*<! ASRC peripheral address */
} asrc_hal_context_t;

/**
 * @brief  Audio information structure for HAL layer
 */
typedef struct {
    uint32_t  sample_rate;      /*!< Audio sample rate in Hz */
    uint8_t   channel;          /*!< Number of audio channels */
    uint8_t   bits_per_sample;  /*!< Bits per audio sample */
} asrc_hal_audio_info_t;

/**
 * @brief  ASRC HAL configuration structure
 */
typedef struct asrc_hal_cfg {
    asrc_hal_audio_info_t  src_info;    /*!< Source audio information */
    asrc_hal_audio_info_t  dest_info;   /*!< Destination audio information */
    float                 *weight;      /*!< Channel mixing weight array */
    uint8_t                weight_len;  /*!< Weight array length */
} asrc_hal_config_t;

/**
 * @brief  Initialize ASRC HAL context and configure hardware
 *
 * @param[in]  hal  Pointer to ASRC HAL context
 */
void asrc_hal_init(asrc_hal_context_t *hal);

/**
 * @brief  Enable or disable ASRC module
 *
 * @param[in]  hal     Pointer to ASRC HAL context
 * @param[in]  enable  True to enable, false to disable
 */
void asrc_hal_enable_asrc_module(asrc_hal_context_t *hal, bool enable);

/**
 * @brief  Initialize a single ASRC stream
 *
 * @param[in]  hal       Pointer to ASRC HAL context
 * @param[in]  asrc_cfg  Pointer to ASRC configuration structure
 * @param[in]  asrc_idx  ASRC stream index
 */
void asrc_hal_init_stream(asrc_hal_context_t *hal, asrc_hal_config_t *asrc_cfg, uint8_t asrc_idx);

/**
 * @brief  Set input sample count for ASRC processing
 *
 * @param[in]  hal             Pointer to ASRC HAL context
 * @param[in]  asrc_idx        ASRC stream index
 * @param[in]  in_samples_num  Number of input samples to process
 */
void asrc_hal_set_in_bytes_num(asrc_hal_context_t *hal, uint8_t asrc_idx, uint32_t in_samples_num);

/**
 * @brief  Set expected output sample count for ASRC processing
 *
 * @param[in]  hal              Pointer to ASRC HAL context
 * @param[in]  asrc_idx         ASRC stream index
 * @param[in]  out_samples_num  Expected number of output samples
 */
void asrc_hal_set_out_bytes_num(asrc_hal_context_t *hal, uint8_t asrc_idx, uint32_t out_samples_num);

/**
 * @brief  Start ASRC processing
 *
 * @param[in]  hal       Pointer to ASRC HAL context
 * @param[in]  asrc_idx  ASRC stream index
 */
void asrc_hal_start(asrc_hal_context_t *hal, uint8_t asrc_idx);

/**
 * @brief  Get the number of output data bytes processed
 *
 * @param[in]  hal       Pointer to ASRC HAL context
 * @param[in]  asrc_idx  ASRC stream index
 *
 * @return
 *       - Number  of output data bytes
 */
uint32_t asrc_hal_get_out_data_bytes(asrc_hal_context_t *hal, uint8_t asrc_idx);

/**
 * @brief  Deinitialize a single ASRC stream
 *
 * @param[in]  hal       Pointer to ASRC HAL context
 * @param[in]  asrc_idx  ASRC stream index
 */
void asrc_hal_deinit_stream(asrc_hal_context_t *hal, uint8_t asrc_idx);

/**
 * @brief  Deinitialize ASRC HAL context
 *
 * @param[in]  hal  Pointer to ASRC HAL context
 */
void asrc_hal_deinit(asrc_hal_context_t *hal);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
