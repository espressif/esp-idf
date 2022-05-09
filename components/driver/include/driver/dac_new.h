/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/dac_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32
#define DAC_CHANNEL1_IO     25      /*!< ESP32 DAC channel 1 GPIO number: GPIO_NUM_25 */
#define DAC_CHANNEL2_IO     26      /*!< ESP32 DAC channel 2 GPIO number: GPIO_NUM_26 */
#elif  CONFIG_IDF_TARGET_ESP32S2
#define DAC_CHANNEL1_IO     17      /*!< ESP32S2 DAC channel 1 GPIO number: GPIO_NUM_17 */
#define DAC_CHANNEL2_IO     18      /*!< ESP32S2 DAC channel 2 GPIO number: GPIO_NUM_17 */
#endif

/**
 * @brief DAC channel configuration
 *
 */
typedef struct {
    dac_channel_mask_t  chan_sel;     /*!< Using DAC channel mask to select the channel in the channel group */
} dac_group_config_t;

/**
 * @brief DAC DMA configration
 *
 */
typedef struct {
    uint32_t                freq_hz;        /*!< The frequency of DAC converting each data in DMA mode, unit: Hz */
    uint32_t                desc_num;       /*!< The number of DMA descriptor , directly proportional to the max data buffer size while converting in cyclic way */
    dac_dma_channel_mode_t  chan_mode;      /*!< DMA channel mode, only take effect when multiple channels enabled in a group, depends converting the buffer alternately or simultaneously */
} dac_dma_config_t;

/**
 * @brief DAC cosine wave gnerator configuration
 *
 */
typedef struct {
    uint32_t            freq_hz;    /*!< The frequency of cosine wave, unit: Hz */
    dac_cosine_scale_t  scale;      /*!< The scale of cosine wave amplitude */
    dac_cosine_phase_t  phase;      /*!< The phase of cosine wave */
    int8_t              offset;     /*!< The DC offset of cosine wave */
} dac_cosine_config_t;

typedef struct dac_channel_group_s     *dac_channel_group_handle_t;     /*!< DAC group handle of DAC peripheral, one or multiple DAC channels can be controlled by the group handle */

/*--------------------------------------------------------------------------
                            DAC common APIs
 ---------------------------------------------------------------------------*/
/**
 * @brief Allocate a new DAC channel group
 *
 * @param[in]  dac_cfg   DAC basic configuration
 * @param[out] handle    DAC channel group handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_NO_MEM        No memory for a new DAC handle
 *      - ESP_ERR_INVALID_STATE The specified DAC channel is occupied already
 *      - ESP_OK                Success to allocate DAC channel group
 */
esp_err_t dac_new_channel_group(const dac_group_config_t *dac_cfg, dac_channel_group_handle_t *handle);

/**
 * @brief Delete and free the DAC channel group
 *
 * @param[in]  handle   DAC channel group handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The channel group is not disabled
 *      - ESP_OK                Success to delete the channel group
 */
esp_err_t dac_del_channel_group(dac_channel_group_handle_t handle);

/**
 * @brief Enabled the DAC channels in the channel group
 * @note GPIOs of DAC channles will be enabled in this step
 *
 * @param[in]  handle   DAC channel group handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The channel group has enabled already or the channels are running
 *      - ESP_OK                Success to enable the channel group
 */
esp_err_t dac_channel_group_enable(dac_channel_group_handle_t handle);

/**
 * @brief Disable the DAC channels in the channel group
 *
 * @param[in]  handle   DAC channel group handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The channel group has disabled already or the channels are running
 *      - ESP_OK                Success to enable the channel group
 */
esp_err_t dac_channel_group_disable(dac_channel_group_handle_t handle);

/*--------------------------------------------------------------------------
                    DAC constant voltage outputting APIs
 ---------------------------------------------------------------------------*/
/**
 * @brief DAC channel group output a constant voltage
 * @note  This function is available when DAC chennel group is enbled
 *
 * @param[in]  handle   DAC channel group handle
 * @param[in]  value    The digital value of the constant voltage
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The channel group is not enabled
 *      - ESP_OK                Success to enable the channel group
 */
esp_err_t dac_channel_group_output_constant_voltage(dac_channel_group_handle_t handle, uint8_t value);

/*--------------------------------------------------------------------------
                          DAC continuous outputting APIs
 ---------------------------------------------------------------------------*/
/**
 * @brief Initialize the DAC channel group to DMA mode
 * @note  DAC can convert digital data continuously in DMA mode
 *
 * @param[in]  handle   DAC channel group handle
 * @param[in]  dma_cfg  DAC DMA configuration
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC channel group has been initialized already
 *      - ESP_ERR_NO_MEM        No memory for DAC DMA mode
 *      - ESP_OK                Success to initializing the DAC channel group to DMA mode
 */
esp_err_t dac_channel_group_init_dma_output(dac_channel_group_handle_t handle, const dac_dma_config_t *dma_cfg);

/**
 * @brief Deinitialize the DMA mode of the DAC channel group
 * @note  It can only be deinitialized when the DMA output is stopped
 *
 * @param[in]  handle   DAC channel group handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC DMA is not stopped yet
 *      - ESP_OK                Success to deinitialize the DAC DMA mode
 */
esp_err_t dac_channel_group_deinit_dma_output(dac_channel_group_handle_t handle);

/**
 * @brief Start the DAC DMA output
 *
 * @param[in]  handle   DAC channel group handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC DMA has not been enabled yet or started already
 *      - ESP_OK                Success to start the DMA output
 */
esp_err_t dac_channel_group_start_dma_output(dac_channel_group_handle_t handle);

/**
 * @brief Stop the DAC DMA output
 *
 * @param[in]  handle   DAC channel group handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC DMA is stopped or not disabled already
 *      - ESP_OK                Success to stop the DMA output
 */
esp_err_t dac_channel_group_stop_dma_output(dac_channel_group_handle_t handle);

/**
 * @brief Write DAC DMA data acyclicly
 * @note  The data in buffer will only be converted one time,
 *        This function will be blocked until all data sent successfully or timeout
 *        then the DAC output will keep outputting the voltage of the last data in the buffer
 *
 * @param[in]  handle   DAC channel group handle
 * @param[in]  buf      The digital data buffer to convert
 * @param[in]  buf_size The buffer size of digital data buffer
 * @param[in]  timeout_ms The timeout time in mili-second
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC DMA has not been started or enabled yet
 *      - ESP_ERR_TIMEOUT       Waiting for semaphore or message queue timeout
 *      - ESP_OK                Success to output the acyclic DAC data by DMA
 */
esp_err_t dac_channel_group_write_acyclicly(dac_channel_group_handle_t handle, uint8_t *buf, size_t buf_size, uint32_t timeout_ms);

/**
 * @brief Write DAC DMA data cyclicly
 * @note  The data in buffer will be converted cyclicly once this function is called,
 *        so the input buffer needs to stay accessable during the convertion,
 *        but this function won't be blocked, it will return once the data loaded into DMA descriptors
 * @note  The buffer size of cyclicly output is limited by the descriptor number while initializing the DMA mode,
 *        Concretely, in order to load all the data into descriptors,
 *        the cyclic buffer size is not supposed to be greater than `desc_num * 4092`
 *
 * @param[in]  handle   DAC channel group handle
 * @param[in]  buf      The digital data buffer to convert
 * @param[in]  buf_size The buffer size of digital data buffer
 * @param[in]  timeout_ms The timeout time in mili-second
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC DMA has not been started or enabled yet
 *      - ESP_ERR_TIMEOUT       Waiting for semaphore or message queue timeout
 *      - ESP_OK                Success to output the acyclic DAC data by DMA
 */
esp_err_t dac_channel_group_write_cyclicly(dac_channel_group_handle_t handle, uint8_t *buf, size_t buf_size, uint32_t timeout_ms);

/*--------------------------------------------------------------------------
                        DAC cosine wave outputting APIs
 ---------------------------------------------------------------------------*/
/**
 * @brief Initialize the DAC channel group to cosine wave mode
 *
 * @param[in]  handle   DAC channel group handle
 * @param[in]  cw_cfg   DAC cosine wave generater configuration
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC channel group has been initialized already
 *      - ESP_OK                Success to initialize the DAC channel group into cosine wave mode
 */
esp_err_t dac_channel_group_init_cosine_output(dac_channel_group_handle_t handle, const dac_cosine_config_t *cw_cfg);

/**
 * @brief Deinitialize the DAC channel group to cosine wave mode
 *
 * @param[in]  handle   DAC channel group handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC cosine wave generator is not stopped yet
 *      - ESP_OK                Success to deinitialize the DAC DMA mode
 */
esp_err_t dac_channel_group_deinit_cosine_output(dac_channel_group_handle_t handle);

/**
 * @brief Start the DAC cosine wave generator output
 *
 * @param[in]  handle   DAC channel group handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC channel group has not been enabled yet or started already
 *      - ESP_OK                Success to start cosine wave generator
 */
esp_err_t dac_channel_group_start_cosine_output(dac_channel_group_handle_t handle);

/**
 * @brief Stop the DAC cosine wave generator output
 *
 * @param[in]  handle   DAC channel group handle
 * @return
 *      - ESP_ERR_INVALID_ARG   The input parameter is invalid
 *      - ESP_ERR_INVALID_STATE The DAC channel group has not been enabled yet or stoppped already
 *      - ESP_OK                Success to stop cosine wave generator
 */
esp_err_t dac_channel_group_stop_cosine_output(dac_channel_group_handle_t handle);

#ifdef __cplusplus
}
#endif
