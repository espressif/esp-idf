/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "soc/soc_caps.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "soc/rmt_struct.h"
#include "hal/rmt_types.h"

#define RMT_CHANNEL_FLAGS_AWARE_DFS (1 << 0) /*!< Channel can work during APB clock scaling */
#define RMT_CHANNEL_FLAGS_INVERT_SIG (1 << 1) /*!< Invert RMT signal */

/** @cond */
#define RMT_CHANNEL_FLAGS_ALWAYS_ON RMT_CHANNEL_FLAGS_AWARE_DFS  /*!< Deprecated name, defined here for compatibility */
/** @endcond */

/**
 * @brief Define memory space of each RMT channel (in words = 4 bytes)
 *
 */
#define RMT_MEM_ITEM_NUM SOC_RMT_MEM_WORDS_PER_CHANNEL

/**
* @brief Data struct of RMT TX configure parameters
*/
typedef struct {
    uint32_t carrier_freq_hz;          /*!< RMT carrier frequency */
    rmt_carrier_level_t carrier_level; /*!< Level of the RMT output, when the carrier is applied */
    rmt_idle_level_t idle_level;       /*!< RMT idle level */
    uint8_t carrier_duty_percent;      /*!< RMT carrier duty (%) */
#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
    uint32_t loop_count;               /*!< Maximum loop count */
#endif
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

/**
 * @brief Default configuration for Tx channel
 *
 */
#define RMT_DEFAULT_CONFIG_TX(gpio, channel_id)      \
    {                                                \
        .rmt_mode = RMT_MODE_TX,                     \
        .channel = channel_id,                       \
        .gpio_num = gpio,                            \
        .clk_div = 80,                               \
        .mem_block_num = 1,                          \
        .flags = 0,                                  \
        .tx_config = {                               \
            .carrier_freq_hz = 38000,                \
            .carrier_level = RMT_CARRIER_LEVEL_HIGH, \
            .idle_level = RMT_IDLE_LEVEL_LOW,        \
            .carrier_duty_percent = 33,              \
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
        .clk_div = 80,                          \
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
*
*/
typedef intr_handle_t rmt_isr_handle_t;

/**
* @brief Type of RMT Tx End callback function
*
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

/**
* @brief Set RMT clock divider, channel clock is divided from source clock.
*
* @param channel RMT channel
* @param div_cnt RMT counter clock divider
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_set_clk_div(rmt_channel_t channel, uint8_t div_cnt);

/**
* @brief Get RMT clock divider, channel clock is divided from source clock.
*
* @param channel RMT channel
* @param div_cnt pointer to accept RMT counter divider
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_get_clk_div(rmt_channel_t channel, uint8_t *div_cnt);

/**
* @brief Set RMT RX idle threshold value
*
*        In receive mode, when no edge is detected on the input signal
*        for longer than idle_thres channel clock cycles,
*        the receive process is finished.
*
* @param channel RMT channel
* @param thresh RMT RX idle threshold
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_set_rx_idle_thresh(rmt_channel_t channel, uint16_t thresh);

/**
* @brief Get RMT idle threshold value.
*
*        In receive mode, when no edge is detected on the input signal
*        for longer than idle_thres channel clock cycles,
*        the receive process is finished.
*
* @param channel RMT channel
* @param thresh pointer to accept RMT RX idle threshold value
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_get_rx_idle_thresh(rmt_channel_t channel, uint16_t *thresh);

/**
* @brief Set RMT memory block number for RMT channel
*
*        This function is used to configure the amount of memory blocks allocated to channel n
*        The 8 channels share a 512x32-bit RAM block which can be read and written
*        by the processor cores over the APB bus, as well as read by the transmitters
*        and written by the receivers.
*
*        The RAM address range for channel n is start_addr_CHn to end_addr_CHn, which are defined by:
*        Memory block start address is RMT_CHANNEL_MEM(n) (in soc/rmt_reg.h),
*        that is, start_addr_chn = RMT base address + 0x800 + 64 ∗ 4 ∗ n, and
*        end_addr_chn = RMT base address + 0x800 +  64 ∗ 4 ∗ n + 64 ∗ 4 ∗ RMT_MEM_SIZE_CHn mod 512 ∗ 4
*
*        @note
*        If memory block number of one channel is set to a value greater than 1, this channel will occupy the memory
*        block of the next channel.
*        Channel 0 can use at most 8 blocks of memory, accordingly channel 7 can only use one memory block.
*
* @param channel RMT channel
* @param rmt_mem_num RMT RX memory block number, one block has 64 * 32 bits.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_set_mem_block_num(rmt_channel_t channel, uint8_t rmt_mem_num);

/**
* @brief Get RMT memory block number
*
* @param channel RMT channel
* @param rmt_mem_num Pointer to accept RMT RX memory block number
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_get_mem_block_num(rmt_channel_t channel, uint8_t *rmt_mem_num);

/**
* @brief Configure RMT carrier for TX signal.
*
*        Set different values for carrier_high and carrier_low to set different frequency of carrier.
*        The unit of carrier_high/low is the source clock tick, not the divided channel counter clock.
*
* @param channel RMT channel
* @param carrier_en Whether to enable output carrier.
* @param high_level High level duration of carrier
* @param low_level Low level duration of carrier.
* @param carrier_level Configure the way carrier wave is modulated for channel.
*     - 1'b1:transmit on low output level
*     - 1'b0:transmit on high output level
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_set_tx_carrier(rmt_channel_t channel, bool carrier_en, uint16_t high_level, uint16_t low_level, rmt_carrier_level_t carrier_level);

/**
* @brief Set RMT memory in low power mode.
*
*        Reduce power consumed by memory. 1:memory is in low power state.
*
* @param channel RMT channel
* @param pd_en RMT memory low power enable.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_set_mem_pd(rmt_channel_t channel, bool pd_en);

/**
* @brief Get RMT memory low power mode.
*
* @param channel RMT channel
* @param pd_en Pointer to accept RMT memory low power mode.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_get_mem_pd(rmt_channel_t channel, bool *pd_en);

/**
* @brief Set RMT start sending data from memory.
*
* @param channel RMT channel
* @param tx_idx_rst Set true to reset memory index for TX.
*                   Otherwise, transmitter will continue sending from the last index in memory.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_tx_start(rmt_channel_t channel, bool tx_idx_rst);

/**
* @brief Set RMT stop sending.
*
* @param channel RMT channel
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_tx_stop(rmt_channel_t channel);

/**
* @brief Set RMT start receiving data.
*
* @param channel RMT channel
* @param rx_idx_rst Set true to reset memory index for receiver.
*                   Otherwise, receiver will continue receiving data to the last index in memory.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_rx_start(rmt_channel_t channel, bool rx_idx_rst);

/**
* @brief Set RMT stop receiving data.
*
* @param channel RMT channel
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_rx_stop(rmt_channel_t channel);

/**
* @brief Reset RMT TX memory
*
* @param channel RMT channel
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_tx_memory_reset(rmt_channel_t channel);

/**
* @brief Reset RMT RX memory
*
* @param channel RMT channel
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_rx_memory_reset(rmt_channel_t channel);

/**
* @brief Set RMT memory owner.
* @note Setting memroy is only valid for RX channel.
*
* @param channel RMT channel
* @param owner To set when the transmitter or receiver can process the memory of channel.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_set_memory_owner(rmt_channel_t channel, rmt_mem_owner_t owner);

/**
* @brief Get RMT memory owner.
*
* @param channel RMT channel
* @param owner Pointer to get memory owner.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_get_memory_owner(rmt_channel_t channel, rmt_mem_owner_t *owner);

/**
* @brief Set RMT tx loop mode.
*
* @param channel RMT channel
* @param loop_en Enable RMT transmitter loop sending mode.
*                If set true, transmitter will continue sending from the first data
*                to the last data in channel over and over again in a loop.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_set_tx_loop_mode(rmt_channel_t channel, bool loop_en);

/**
* @brief Get RMT tx loop mode.
*
* @param channel RMT channel
* @param loop_en Pointer to accept RMT transmitter loop sending mode.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_get_tx_loop_mode(rmt_channel_t channel, bool *loop_en);

/**
* @brief Set RMT RX filter.
*
*        In receive mode, channel will ignore input pulse when the pulse width is smaller than threshold.
*        Counted in source clock, not divided counter clock.
*
* @param channel RMT channel
* @param rx_filter_en To enable RMT receiver filter.
* @param thresh Threshold of pulse width for receiver.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_set_rx_filter(rmt_channel_t channel, bool rx_filter_en, uint8_t thresh);

/**
* @brief Set RMT source clock
*
*        RMT module has two clock sources:
*        1. APB clock which is 80Mhz
*        2. REF tick clock, which would be 1Mhz (not supported in this version).
*
* @param channel RMT channel
* @param base_clk To choose source clock for RMT module.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_set_source_clk(rmt_channel_t channel, rmt_source_clk_t base_clk);

/**
* @brief Get RMT source clock
*
*        RMT module has two clock sources:
*        1. APB clock which is 80Mhz
*        2. REF tick clock, which would be 1Mhz (not supported in this version).
*
* @param channel RMT channel
* @param src_clk Pointer to accept source clock for RMT module.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_get_source_clk(rmt_channel_t channel, rmt_source_clk_t *src_clk);

/**
* @brief Set RMT idle output level for transmitter
*
* @param channel RMT channel
* @param idle_out_en To enable idle level output.
* @param level To set the output signal's level for channel in idle state.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_set_idle_level(rmt_channel_t channel, bool idle_out_en, rmt_idle_level_t level);

/**
* @brief Get RMT idle output level for transmitter
*
* @param channel RMT channel
* @param idle_out_en Pointer to accept value of enable idle.
* @param level Pointer to accept value of output signal's level in idle state for specified channel.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_get_idle_level(rmt_channel_t channel, bool *idle_out_en, rmt_idle_level_t *level);

/**
* @brief Get RMT status
*
* @param channel RMT channel
* @param status Pointer to accept channel status.
*        Please refer to RMT_CHnSTATUS_REG(n=0~7) in `rmt_reg.h` for more details of each field.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_get_status(rmt_channel_t channel, uint32_t *status);

/**
* @brief Set RMT RX interrupt enable
*
* @param channel RMT channel
* @param en enable or disable RX interrupt.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_set_rx_intr_en(rmt_channel_t channel, bool en);

/**
* @brief Set RMT RX error interrupt enable
*
* @param channel RMT channel
* @param en enable or disable RX err interrupt.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_set_err_intr_en(rmt_channel_t channel, bool en);

/**
* @brief Set RMT TX interrupt enable
*
* @param channel RMT channel
* @param en enable or disable TX interrupt.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_set_tx_intr_en(rmt_channel_t channel, bool en);

/**
* @brief Set RMT TX threshold event interrupt enable
*
* An interrupt will be triggered when the number of transmitted items reaches the threshold value
*
* @param channel RMT channel
* @param en enable or disable TX event interrupt.
* @param evt_thresh RMT event interrupt threshold value
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_set_tx_thr_intr_en(rmt_channel_t channel, bool en, uint16_t evt_thresh);

/**
* @brief Configure the GPIO used by RMT channel
*
* @param channel RMT channel
* @param mode RMT mode, either RMT_MODE_TX or RMT_MODE_RX
* @param gpio_num GPIO number, which is connected with certain RMT signal
* @param invert_signal Invert RMT signal physically by GPIO matrix
*
* @return
*     - ESP_ERR_INVALID_ARG Configure RMT GPIO failed because of wrong parameter
*     - ESP_OK Configure RMT GPIO successfully
*/
esp_err_t rmt_set_gpio(rmt_channel_t channel, rmt_mode_t mode, gpio_num_t gpio_num, bool invert_signal);

/**
* @brief Configure RMT parameters
*
* @param rmt_param RMT parameter struct
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_config(const rmt_config_t *rmt_param);

/**
* @brief Register RMT interrupt handler, the handler is an ISR.
*
*        The handler will be attached to the same CPU core that this function is running on.
*
* @note  If you already called rmt_driver_install to use system RMT driver,
*        please do not register ISR handler again.
*
* @param fn Interrupt handler function.
* @param arg Parameter for the handler function
* @param intr_alloc_flags Flags used to allocate the interrupt. One or multiple (ORred)
*        ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info.
* @param handle If non-zero, a handle to later clean up the ISR gets stored here.
*
* @return
*     - ESP_OK Success
*     - ESP_ERR_INVALID_ARG Function pointer error.
*     - ESP_FAIL System driver installed, can not register ISR handler for RMT
*/
esp_err_t rmt_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, rmt_isr_handle_t *handle);

/**
* @brief   Deregister previously registered RMT interrupt handler
*
* @param handle Handle obtained from rmt_isr_register
*
* @return
*     - ESP_OK Success
*     - ESP_ERR_INVALID_ARG Handle invalid
*/
esp_err_t rmt_isr_deregister(rmt_isr_handle_t handle);

/**
* @brief Fill memory data of channel with given RMT items.
*
* @param channel RMT channel
* @param item Pointer of items.
* @param item_num RMT sending items number.
* @param mem_offset Index offset of memory.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_fill_tx_items(rmt_channel_t channel, const rmt_item32_t *item, uint16_t item_num, uint16_t mem_offset);

/**
* @brief Initialize RMT driver
*
* @param channel RMT channel
* @param rx_buf_size Size of RMT RX ringbuffer. Can be 0 if the RX ringbuffer is not used.
* @param intr_alloc_flags Flags for the RMT driver interrupt handler. Pass 0 for default flags. See esp_intr_alloc.h for details.
*        If ESP_INTR_FLAG_IRAM is used, please do not use the memory allocated from psram when calling rmt_write_items.
*
* @return
*     - ESP_ERR_INVALID_STATE Driver is already installed, call rmt_driver_uninstall first.
*     - ESP_ERR_NO_MEM Memory allocation failure
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_driver_install(rmt_channel_t channel, size_t rx_buf_size, int intr_alloc_flags);

/**
* @brief Uninstall RMT driver.
*
* @param channel RMT channel
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_driver_uninstall(rmt_channel_t channel);

/**
* @brief Get the current status of eight channels.
*
* @note Do not call this function if it is possible that `rmt_driver_uninstall` will be called at the same time.
*
* @param[out] channel_status store the current status of each channel
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter is NULL
*     - ESP_OK Success
*/
esp_err_t rmt_get_channel_status(rmt_channel_status_result_t *channel_status);

/**
* @brief Get speed of channel's internal counter clock.
*
* @param channel RMT channel
* @param[out] clock_hz counter clock speed, in hz
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter is NULL
*     - ESP_OK Success
*/
esp_err_t rmt_get_counter_clock(rmt_channel_t channel, uint32_t *clock_hz);

/**
* @brief RMT send waveform from rmt_item array.
*
*        This API allows user to send waveform with any length.
*
* @param channel RMT channel
* @param rmt_item head point of RMT items array.
*        If ESP_INTR_FLAG_IRAM is used, please do not use the memory allocated from psram when calling rmt_write_items.
* @param item_num RMT data item number.
* @param wait_tx_done
*        - If set 1, it will block the task and wait for sending done.
*        - If set 0, it will not wait and return immediately.
*
*         @note
*         This function will not copy data, instead, it will point to the original items,
*         and send the waveform items.
*         If wait_tx_done is set to true, this function will block and will not return until
*         all items have been sent out.
*         If wait_tx_done is set to false, this function will return immediately, and the driver
*         interrupt will continue sending the items. We must make sure the item data will not be
*         damaged when the driver is still sending items in driver interrupt.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_write_items(rmt_channel_t channel, const rmt_item32_t *rmt_item, int item_num, bool wait_tx_done);

/**
* @brief Wait RMT TX finished.
*
* @param channel RMT channel
* @param wait_time Maximum time in ticks to wait for transmission to be complete.  If set 0, return immediately with ESP_ERR_TIMEOUT if TX is busy (polling).
*
* @return
*     - ESP_OK RMT Tx done successfully
*     - ESP_ERR_TIMEOUT Exceeded the 'wait_time' given
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_FAIL Driver not installed
*/
esp_err_t rmt_wait_tx_done(rmt_channel_t channel, TickType_t wait_time);

/**
* @brief Get ringbuffer from RMT.
*
*        Users can get the RMT RX ringbuffer handle, and process the RX data.
*
* @param channel RMT channel
* @param buf_handle Pointer to buffer handle to accept RX ringbuffer handle.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_get_ringbuf_handle(rmt_channel_t channel, RingbufHandle_t *buf_handle);

/**
* @brief Init rmt translator and register user callback.
*        The callback will convert the raw data that needs to be sent to rmt format.
*        If a channel is initialized more than once, tha user callback will be replaced by the later.
*
* @param channel RMT channel .
* @param fn Point to the data conversion function.
*
* @return
*     - ESP_FAIL Init fail.
*     - ESP_OK Init success.
*/
esp_err_t rmt_translator_init(rmt_channel_t channel, sample_to_rmt_t fn);

/**
* @brief Set user context for the translator of specific channel
*
* @param channel RMT channel number
* @param context User context
*
* @return
*     - ESP_FAIL Set context fail
*     - ESP_OK Set context success
*/
esp_err_t rmt_translator_set_context(rmt_channel_t channel, void *context);

/**
* @brief Get the user context set by 'rmt_translator_set_context'
*
* @note This API must be invoked in the RMT translator callback function,
*       and the first argument must be the actual parameter 'item_num' you got in that callback function.
*
* @param item_num Address of the memory which contains the number of translated items (It's from driver's internal memroy)
* @param context Returned User context
*
* @return
*     - ESP_FAIL Get context fail
*     - ESP_OK Get context success
*/
esp_err_t rmt_translator_get_context(const size_t *item_num, void **context);

/**
* @brief Translate uint8_t type of data into rmt format and send it out.
*        Requires rmt_translator_init to init the translator first.
*
* @param channel RMT channel .
* @param src Pointer to the raw data.
* @param src_size The size of the raw data.
* @param wait_tx_done Set true to wait all data send done.
*
* @return
*     - ESP_FAIL Send fail
*     - ESP_OK Send success
*/
esp_err_t rmt_write_sample(rmt_channel_t channel, const uint8_t *src, size_t src_size, bool wait_tx_done);

/**
* @brief Registers a callback that will be called when transmission ends.
*
*        Called by rmt_driver_isr_default in interrupt context.
*
* @note Requires rmt_driver_install to install the default ISR handler.
*
* @param function Function to be called from the default interrupt handler or NULL.
* @param arg Argument which will be provided to the callback when it is called.
*
* @return the previous callback settings (members will be set to NULL if there was none)
*/
rmt_tx_end_callback_t rmt_register_tx_end_callback(rmt_tx_end_fn_t function, void *arg);

#if SOC_RMT_SUPPORT_RX_PINGPONG
/**
* @brief Set RMT RX threshold event interrupt enable
*
* An interrupt will be triggered when the number of received items reaches the threshold value
*
* @param channel RMT channel
* @param en enable or disable RX event interrupt.
* @param evt_thresh RMT event interrupt threshold value
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_set_rx_thr_intr_en(rmt_channel_t channel, bool en, uint16_t evt_thresh);
#endif

#if SOC_RMT_SUPPORT_TX_SYNCHRO
/**
* @brief Add channel into a synchronous group (channels in the same group can start transaction simultaneously)
*
* @param channel RMT channel
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_add_channel_to_group(rmt_channel_t channel);

/**
* @brief Remove channel out of a group
*
* @param channel RMT channel
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_remove_channel_from_group(rmt_channel_t channel);
#endif

#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
/**
 * @brief Set loop count threshold value for RMT TX channel
 *
 * When tx loop count reaches this value, an ISR callback will notify user
 *
 * @param channel RMT channel
 * @param count loop count, 1 ~ 1023
 * @return
 *      - ESP_ERR_INVALID_ARG Parameter error
 *      - ESP_OK Success
 */
esp_err_t rmt_set_tx_loop_count(rmt_channel_t channel, uint32_t count);

/**
 * @brief Enable or disable the feature that when loop count reaches the threshold, RMT will stop transmitting.
 *
 * - When the loop auto-stop feature is enabled will halt RMT transmission after the loop count reaches a certain threshold
 * - When disabled, the RMT transmission continue indefinitely until halted by the users
 *
 * @note The auto-stop feature is implemented in hardware on particular targets (i.e. those with SOC_RMT_SUPPORT_TX_LOOP_AUTOSTOP defined).
 *       Otherwise, the auto-stop feature is implemented in software via the interrupt.
 *
 * @param channel RMT channel
 * @param en enable bit
 * @return
 *      - ESP_ERR_INVALID_ARG Parameter error
 *      - ESP_OK Success
 */
esp_err_t rmt_enable_tx_loop_autostop(rmt_channel_t channel, bool en);
#endif // SOC_RMT_SUPPORT_TX_LOOP_COUNT

/**
* @brief Reset RMT TX/RX memory index.
*
* @param channel RMT channel
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_memory_rw_rst(rmt_channel_t channel)
__attribute__((deprecated("use rmt_tx_memory_reset or rmt_rx_memory_reset instead")));

/**
* @brief Set mask value to RMT interrupt enable register.
*
* @param mask Bit mask to set to the register
*
*/
void rmt_set_intr_enable_mask(uint32_t mask)
__attribute__((deprecated("interrupt should be handled by driver")));

/**
* @brief Clear mask value to RMT interrupt enable register.
*
* @param mask Bit mask to clear the register
*
*/
void rmt_clr_intr_enable_mask(uint32_t mask)
__attribute__((deprecated("interrupt should be handled by driver")));

/**
* @brief Set RMT pin
*
* @param channel RMT channel
* @param mode TX or RX mode for RMT
* @param gpio_num GPIO number to transmit or receive the signal.
*
* @return
*     - ESP_ERR_INVALID_ARG Parameter error
*     - ESP_OK Success
*/
esp_err_t rmt_set_pin(rmt_channel_t channel, rmt_mode_t mode, gpio_num_t gpio_num)
__attribute__((deprecated("use rmt_set_gpio instead")));

#ifdef __cplusplus
}
#endif
