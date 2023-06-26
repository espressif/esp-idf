/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif


#define RMT_CHANNEL_FLAGS_AWARE_DFS (1 << 0) /*!< Channel can work during APB clock scaling */
#define RMT_CHANNEL_FLAGS_INVERT_SIG (1 << 1) /*!< Invert RMT signal */

/**
 * @brief Define memory space of each RMT channel (in words = 4 bytes)
 */
#define RMT_MEM_ITEM_NUM SOC_RMT_MEM_WORDS_PER_CHANNEL

/**
 * @brief Definition of RMT item
 */
typedef struct {
    union {
        struct {
            uint32_t duration0 : 15; /*!< Duration of level0 */
            uint32_t level0 : 1;     /*!< Level of the first part */
            uint32_t duration1 : 15; /*!< Duration of level1 */
            uint32_t level1 : 1;     /*!< Level of the second part */
        };
        uint32_t val; /*!< Equivalent unsigned value for the RMT item */
    };
} rmt_item32_t;


#if SOC_RMT_SUPPORTED
/**
 * @brief RMT hardware memory layout
 */
typedef struct {
    struct {
        volatile rmt_item32_t data32[SOC_RMT_MEM_WORDS_PER_CHANNEL];
    } chan[SOC_RMT_CHANNELS_PER_GROUP];
} rmt_mem_t;
#endif // SOC_RMT_SUPPORTED

/**
 * @brief RMT channel ID
 */
typedef enum {
    RMT_CHANNEL_0,  /*!< RMT channel number 0 */
    RMT_CHANNEL_1,  /*!< RMT channel number 1 */
    RMT_CHANNEL_2,  /*!< RMT channel number 2 */
    RMT_CHANNEL_3,  /*!< RMT channel number 3 */
#if SOC_RMT_CHANNELS_PER_GROUP > 4
    RMT_CHANNEL_4,  /*!< RMT channel number 4 */
    RMT_CHANNEL_5,  /*!< RMT channel number 5 */
    RMT_CHANNEL_6,  /*!< RMT channel number 6 */
    RMT_CHANNEL_7,  /*!< RMT channel number 7 */
#endif
    RMT_CHANNEL_MAX /*!< Number of RMT channels */
} rmt_channel_t;

/**
 * @brief RMT Internal Memory Owner
 */
typedef enum {
    RMT_MEM_OWNER_TX, /*!< RMT RX mode, RMT transmitter owns the memory block*/
    RMT_MEM_OWNER_RX, /*!< RMT RX mode, RMT receiver owns the memory block*/
    RMT_MEM_OWNER_MAX,
} rmt_mem_owner_t;

/**
 * @brief Clock Source of RMT Channel
 */
#if SOC_RMT_SUPPORTED
typedef soc_periph_rmt_clk_src_legacy_t rmt_source_clk_t;
#else
typedef int rmt_source_clk_t;
#endif // SOC_RMT_SUPPORTED

/**
 * @brief RMT Data Mode
 *
 * @note We highly recommended to use MEM mode not FIFO mode since there will be some gotcha in FIFO mode.
 */
typedef enum {
    RMT_DATA_MODE_FIFO, /*<! RMT memory access in FIFO mode */
    RMT_DATA_MODE_MEM,  /*<! RMT memory access in memory mode */
    RMT_DATA_MODE_MAX,
} rmt_data_mode_t;

/**
 * @brief RMT Channel Working Mode (TX or RX)
 */
typedef enum {
    RMT_MODE_TX, /*!< RMT TX mode */
    RMT_MODE_RX, /*!< RMT RX mode */
    RMT_MODE_MAX
} rmt_mode_t;

/**
 * @brief RMT Idle Level
 *
 */
typedef enum {
    RMT_IDLE_LEVEL_LOW,  /*!< RMT TX idle level: low Level */
    RMT_IDLE_LEVEL_HIGH, /*!< RMT TX idle level: high Level */
    RMT_IDLE_LEVEL_MAX,
} rmt_idle_level_t;

/**
 * @brief RMT Carrier Level
 */
typedef enum {
    RMT_CARRIER_LEVEL_LOW,  /*!< RMT carrier wave is modulated for low Level output */
    RMT_CARRIER_LEVEL_HIGH, /*!< RMT carrier wave is modulated for high Level output */
    RMT_CARRIER_LEVEL_MAX
} rmt_carrier_level_t;

/**
 * @brief RMT Channel Status
 */
typedef enum {
    RMT_CHANNEL_UNINIT, /*!< RMT channel uninitialized */
    RMT_CHANNEL_IDLE,   /*!< RMT channel status idle */
    RMT_CHANNEL_BUSY,   /*!< RMT channel status busy */
} rmt_channel_status_t;

/**
 * @brief Data struct of RMT channel status
 */
typedef struct {
    rmt_channel_status_t status[RMT_CHANNEL_MAX]; /*!< Store the current status of each channel */
} rmt_channel_status_result_t;

/**
 * @brief Data struct of RMT TX configure parameters
 */
typedef struct {
    uint32_t carrier_freq_hz;          /*!< RMT carrier frequency */
    rmt_carrier_level_t carrier_level; /*!< Level of the RMT output, when the carrier is applied */
    rmt_idle_level_t idle_level;       /*!< RMT idle level */
    uint8_t carrier_duty_percent;      /*!< RMT carrier duty (%) */
    uint32_t loop_count;               /*!< Maximum loop count, only take effect for chips that is capable of `SOC_RMT_SUPPORT_TX_LOOP_COUNT` */
    bool carrier_en;                   /*!< RMT carrier enable */
    bool loop_en;                      /*!< Enable sending RMT items in a loop */
    bool idle_output_en;               /*!< RMT idle level output enable */
} rmt_tx_config_t;

/**
 * @brief Data struct of RMT RX configure parameters
 */
typedef struct {
    uint16_t idle_threshold;     /*!< RMT RX idle threshold */
    uint8_t filter_ticks_thresh; /*!< RMT filter tick number */
    bool filter_en;              /*!< RMT receiver filter enable */
#if SOC_RMT_SUPPORT_RX_DEMODULATION
    bool rm_carrier;                   /*!< RMT receiver remove carrier enable */
    uint32_t carrier_freq_hz;          /*!< RMT carrier frequency */
    uint8_t carrier_duty_percent;      /*!< RMT carrier duty (%) */
    rmt_carrier_level_t carrier_level; /*!< The level to remove the carrier */
#endif
} rmt_rx_config_t;

/**
 * @brief Data struct of RMT configure parameters
 */
typedef struct {
    rmt_mode_t rmt_mode;   /*!< RMT mode: transmitter or receiver */
    rmt_channel_t channel; /*!< RMT channel */
    gpio_num_t gpio_num;   /*!< RMT GPIO number */
    uint8_t clk_div;       /*!< RMT channel counter divider */
    uint8_t mem_block_num; /*!< RMT memory block number */
    uint32_t flags;        /*!< RMT channel extra configurations, OR'd with RMT_CHANNEL_FLAGS_[*] */
    union {
        rmt_tx_config_t tx_config; /*!< RMT TX parameter */
        rmt_rx_config_t rx_config; /*!< RMT RX parameter */
    };
} rmt_config_t;

#if CONFIG_IDF_TARGET_ESP32H2
#define RMT_DEFAULT_CLK_DIV 32
#else
#define RMT_DEFAULT_CLK_DIV 80
#endif

/**
 * @brief Default configuration for Tx channel
 *
 */
#define RMT_DEFAULT_CONFIG_TX(gpio, channel_id)      \
    {                                                \
        .rmt_mode = RMT_MODE_TX,                     \
        .channel = channel_id,                       \
        .gpio_num = gpio,                            \
        .clk_div = RMT_DEFAULT_CLK_DIV,              \
        .mem_block_num = 1,                          \
        .flags = 0,                                  \
        .tx_config = {                               \
            .carrier_freq_hz = 38000,                \
            .carrier_level = RMT_CARRIER_LEVEL_HIGH, \
            .idle_level = RMT_IDLE_LEVEL_LOW,        \
            .carrier_duty_percent = 33,              \
            .loop_count = 0,                         \
            .carrier_en = false,                     \
            .loop_en = false,                        \
            .idle_output_en = true,                  \
        }                                            \
    }

/**
 * @brief Default configuration for RX channel
 *
 */
#define RMT_DEFAULT_CONFIG_RX(gpio, channel_id) \
    {                                           \
        .rmt_mode = RMT_MODE_RX,                \
        .channel = channel_id,                  \
        .gpio_num = gpio,                       \
        .clk_div = RMT_DEFAULT_CLK_DIV,         \
        .mem_block_num = 1,                     \
        .flags = 0,                             \
        .rx_config = {                          \
            .idle_threshold = 12000,            \
            .filter_ticks_thresh = 100,         \
            .filter_en = true,                  \
        }                                       \
    }

/**
 * @brief RMT interrupt handle
 */
typedef intr_handle_t rmt_isr_handle_t;

/**
 * @brief Type of RMT Tx End callback function
 */
typedef void (*rmt_tx_end_fn_t)(rmt_channel_t channel, void *arg);

/**
 * @brief Structure encapsulating a RMT TX end callback
 */
typedef struct {
    rmt_tx_end_fn_t function; /*!< Function which is called on RMT TX end */
    void *arg;                /*!< Optional argument passed to function */
} rmt_tx_end_callback_t;

/**
 * @brief User callback function to convert uint8_t type data to rmt format(rmt_item32_t).
 *
 *        This function may be called from an ISR, so, the code should be short and efficient.
 *
 * @param  src Pointer to the buffer storing the raw data that needs to be converted to rmt format.
 * @param[out] dest Pointer to the buffer storing the rmt format data.
 * @param  src_size The raw data size.
 * @param  wanted_num The number of rmt format data that wanted to get.
 * @param[out] translated_size The size of the raw data that has been converted to rmt format,
 *             it should return 0 if no data is converted in user callback.
 * @param[out] item_num The number of the rmt format data that actually converted to,
 *             it can be less than wanted_num if there is not enough raw data, but cannot exceed wanted_num.
 *             it should return 0 if no data was converted.
 *
 * @note
 *       In fact, item_num should be a multiple of translated_size, e.g. :
 *       When we convert each byte of uint8_t type data to rmt format data,
 *       the relation between item_num and translated_size should be `item_num = translated_size*8`.
 */
typedef void (*sample_to_rmt_t)(const void *src, rmt_item32_t *dest, size_t src_size, size_t wanted_num, size_t *translated_size, size_t *item_num);

#ifdef __cplusplus
}
#endif
