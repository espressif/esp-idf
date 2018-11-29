// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _DRIVER_RMT_CTRL_H_
#define _DRIVER_RMT_CTRL_H_
#include "esp_err.h"
#include "soc/rmt_reg.h"
#include "soc/dport_reg.h"
#include "soc/rmt_struct.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/ringbuf.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RMT_MEM_BLOCK_BYTE_NUM (256)
#define RMT_MEM_ITEM_NUM  (RMT_MEM_BLOCK_BYTE_NUM/4)

typedef enum {
    RMT_CHANNEL_0 = 0, /*!< RMT Channel 0 */
    RMT_CHANNEL_1,     /*!< RMT Channel 1 */
    RMT_CHANNEL_2,     /*!< RMT Channel 2 */
    RMT_CHANNEL_3,     /*!< RMT Channel 3 */
    RMT_CHANNEL_4,     /*!< RMT Channel 4 */
    RMT_CHANNEL_5,     /*!< RMT Channel 5 */
    RMT_CHANNEL_6,     /*!< RMT Channel 6 */
    RMT_CHANNEL_7,     /*!< RMT Channel 7 */
    RMT_CHANNEL_MAX
} rmt_channel_t;

typedef enum {
    RMT_MEM_OWNER_TX = 0,  /*!< RMT RX mode, RMT transmitter owns the memory block*/
    RMT_MEM_OWNER_RX = 1,  /*!< RMT RX mode, RMT receiver owns the memory block*/
    RMT_MEM_OWNER_MAX,
}rmt_mem_owner_t;

typedef enum {
    RMT_BASECLK_REF = 0,   /*!< RMT source clock system reference tick, 1MHz by default (not supported in this version) */
    RMT_BASECLK_APB,       /*!< RMT source clock is APB CLK, 80Mhz by default */
    RMT_BASECLK_MAX,
} rmt_source_clk_t;

typedef enum {
    RMT_DATA_MODE_FIFO = 0,  /*<! RMT memory access in FIFO mode */
    RMT_DATA_MODE_MEM = 1,   /*<! RMT memory access in memory mode */
    RMT_DATA_MODE_MAX,
} rmt_data_mode_t;

typedef enum {
    RMT_MODE_TX = 0,   /*!< RMT TX mode */
    RMT_MODE_RX,       /*!< RMT RX mode */
    RMT_MODE_MAX
} rmt_mode_t;

typedef enum {
    RMT_IDLE_LEVEL_LOW = 0,   /*!< RMT TX idle level: low Level */
    RMT_IDLE_LEVEL_HIGH,      /*!< RMT TX idle level: high Level */
    RMT_IDLE_LEVEL_MAX,
} rmt_idle_level_t;

typedef enum {
    RMT_CARRIER_LEVEL_LOW = 0,  /*!< RMT carrier wave is modulated for low Level output */
    RMT_CARRIER_LEVEL_HIGH,     /*!< RMT carrier wave is modulated for high Level output */
    RMT_CARRIER_LEVEL_MAX
} rmt_carrier_level_t;

typedef enum {
    RMT_CHANNEL_UNINIT = 0, /*!< RMT channel uninitialized */
    RMT_CHANNEL_IDLE = 1,   /*!< RMT channel status idle */
    RMT_CHANNEL_BUSY = 2,   /*!< RMT channel status busy */
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
    bool loop_en;                         /*!< Enable sending RMT items in a loop */
    uint32_t carrier_freq_hz;             /*!< RMT carrier frequency */
    uint8_t carrier_duty_percent;         /*!< RMT carrier duty (%) */
    rmt_carrier_level_t carrier_level;    /*!< Level of the RMT output, when the carrier is applied */
    bool carrier_en;                      /*!< RMT carrier enable */
    rmt_idle_level_t idle_level;          /*!< RMT idle level */
    bool idle_output_en;                  /*!< RMT idle level output enable */
}rmt_tx_config_t;

/**
 * @brief Data struct of RMT RX configure parameters
 */
typedef struct {
    bool filter_en;                    /*!< RMT receiver filter enable */
    uint8_t filter_ticks_thresh;       /*!< RMT filter tick number */
    uint16_t idle_threshold;           /*!< RMT RX idle threshold */
}rmt_rx_config_t;

/**
 * @brief Data struct of RMT configure parameters
 */
typedef struct {
    rmt_mode_t rmt_mode;               /*!< RMT mode: transmitter or receiver */
    rmt_channel_t channel;             /*!< RMT channel */
    uint8_t clk_div;                   /*!< RMT channel counter divider */
    gpio_num_t gpio_num;               /*!< RMT GPIO number */
    uint8_t mem_block_num;             /*!< RMT memory block number */
    union{
        rmt_tx_config_t tx_config;     /*!< RMT TX parameter */
        rmt_rx_config_t rx_config;     /*!< RMT RX parameter */
    };
} rmt_config_t;

typedef intr_handle_t rmt_isr_handle_t;

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
 *
 * @param[out] dest Pointer to the buffer storing the rmt format data.
 *
 * @param  src_size The raw data size.
 *
 * @param  wanted_num The number of rmt format data that wanted to get.
 *
 * @param[out] translated_size The size of the raw data that has been converted to rmt format,
 *             it should return 0 if no data is converted in user callback.
 *
 * @param[out] item_num The number of the rmt format data that actually converted to, it can be less than wanted_num if there is not enough raw data,
 *             but cannot exceed wanted_num. it should return 0 if no data was converted.
 *
 *       @note
 *       In fact, item_num should be a multiple of translated_size, e.g. :
 *       When we convert each byte of uint8_t type data to rmt format data,
 *       the relation between item_num and translated_size should be `item_num = translated_size*8`.
 */
typedef void (*sample_to_rmt_t)(const void* src, rmt_item32_t* dest, size_t src_size, size_t wanted_num, size_t* translated_size, size_t* item_num);

/**
 * @brief Set RMT clock divider, channel clock is divided from source clock.
 *
 * @param channel RMT channel (0-7)
 *
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
 * @param channel RMT channel (0-7)
 *
 * @param div_cnt pointer to accept RMT counter divider
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_get_clk_div(rmt_channel_t channel, uint8_t* div_cnt);

/**
 * @brief Set RMT RX idle threshold value
 *
 *        In receive mode, when no edge is detected on the input signal
 *        for longer than idle_thres channel clock cycles,
 *        the receive process is finished.
 *
 * @param channel RMT channel (0-7)
 *
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
 * @param channel RMT channel (0-7)
 *
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
 * @param channel RMT channel (0-7)
 *
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
 * @param channel RMT channel (0-7)
 *
 * @param rmt_mem_num Pointer to accept RMT RX memory block number
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_get_mem_block_num(rmt_channel_t channel, uint8_t* rmt_mem_num);

/**
 * @brief Configure RMT carrier for TX signal.
 *
 *        Set different values for carrier_high and carrier_low to set different frequency of carrier.
 *        The unit of carrier_high/low is the source clock tick, not the divided channel counter clock.
 *
 * @param channel RMT channel (0-7)
 *
 * @param carrier_en Whether to enable output carrier.
 *
 * @param high_level High level duration of carrier
 *
 * @param low_level Low level duration of carrier.
 *
 * @param carrier_level Configure the way carrier wave is modulated for channel 0-7.
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
 * @param channel RMT channel (0-7)
 *
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
 * @param channel RMT channel (0-7)
 *
 * @param pd_en Pointer to accept RMT memory low power mode.
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_get_mem_pd(rmt_channel_t channel, bool* pd_en);

/**
 * @brief Set RMT start sending data from memory.
 *
 * @param channel RMT channel (0-7)
 *
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
 * @param channel RMT channel (0-7)
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_tx_stop(rmt_channel_t channel);

/**
 * @brief Set RMT start receiving data.
 *
 * @param channel RMT channel (0-7)
 *
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
 * @param channel RMT channel (0-7)
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_rx_stop(rmt_channel_t channel);

/**
 * @brief Reset RMT TX/RX memory index.
 *
 * @param channel RMT channel (0-7)
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_memory_rw_rst(rmt_channel_t channel);

/**
 * @brief Set RMT memory owner.
 *
 * @param channel RMT channel (0-7)
 *
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
 * @param channel RMT channel (0-7)
 *
 * @param owner Pointer to get memory owner.
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_get_memory_owner(rmt_channel_t channel, rmt_mem_owner_t* owner);

/**
 * @brief Set RMT tx loop mode.
 *
 * @param channel RMT channel (0-7)
 *
 * @param loop_en Enable RMT transmitter loop sending mode.
 *                If set true, transmitter will continue sending from the first data
 *                to the last data in channel 0-7 over and over again in a loop.
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_set_tx_loop_mode(rmt_channel_t channel, bool loop_en);

/**
 * @brief Get RMT tx loop mode.
 *
 * @param channel RMT channel (0-7)
 *
 * @param loop_en Pointer to accept RMT transmitter loop sending mode.
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_get_tx_loop_mode(rmt_channel_t channel, bool* loop_en);

/**
 * @brief Set RMT RX filter.
 *
 *        In receive mode, channel 0-7 will ignore input pulse when the pulse width is smaller than threshold.
 *        Counted in source clock, not divided counter clock.
 *
 * @param channel RMT channel (0-7)
 *
 * @param rx_filter_en To enable RMT receiver filter.
 *
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
 * @param channel RMT channel (0-7)
 *
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
 * @param channel RMT channel (0-7)
 *
 * @param src_clk Pointer to accept source clock for RMT module.
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_get_source_clk(rmt_channel_t channel, rmt_source_clk_t* src_clk);

/**
 * @brief Set RMT idle output level for transmitter
 *
 * @param channel RMT channel (0-7)
 *
 * @param idle_out_en To enable idle level output.
 *
 * @param level To set the output signal's level for channel 0-7 in idle state.
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_set_idle_level(rmt_channel_t channel, bool idle_out_en, rmt_idle_level_t level);

/**
 * @brief Get RMT status
 *
 * @param channel RMT channel (0-7)
 *
 * @param status Pointer to accept channel status.
 *        Please refer to RMT_CHnSTATUS_REG(n=0~7) in `rmt_reg.h` for more details of each field.
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_get_status(rmt_channel_t channel, uint32_t* status);

/**
 * @brief Set mask value to RMT interrupt enable register.
 *
 * @param mask Bit mask to set to the register
 *
 */
void rmt_set_intr_enable_mask(uint32_t mask);

/**
 * @brief Clear mask value to RMT interrupt enable register.
 *
 * @param mask Bit mask to clear the register
 *
 */
void rmt_clr_intr_enable_mask(uint32_t mask);

/**
 * @brief Set RMT RX interrupt enable
 *
 * @param channel RMT channel (0 - 7)
 *
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
 * @param channel RMT channel (0 - 7)
 *
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
 * @param channel RMT channel (0 - 7)
 *
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
 * @param channel RMT channel (0 - 7)
 *
 * @param en enable or disable TX event interrupt.
 *
 * @param evt_thresh RMT event interrupt threshold value
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_set_tx_thr_intr_en(rmt_channel_t channel, bool en, uint16_t evt_thresh);

/**
 * @brief Set RMT pin
 *
 * @param channel RMT channel (0 - 7)
 *
 * @param mode TX or RX mode for RMT
 *
 * @param gpio_num GPIO number to transmit or receive the signal.
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_set_pin(rmt_channel_t channel, rmt_mode_t mode, gpio_num_t gpio_num);

/**
 * @brief Configure RMT parameters
 *
 * @param rmt_param RMT parameter struct
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_config(const rmt_config_t* rmt_param);

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
esp_err_t rmt_isr_register(void (* fn)(void* ), void * arg, int intr_alloc_flags, rmt_isr_handle_t *handle);

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
 * @param channel RMT channel (0 - 7)
 *
 * @param item Pointer of items.
 *
 * @param item_num RMT sending items number.
 *
 * @param mem_offset Index offset of memory.
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_fill_tx_items(rmt_channel_t channel, const rmt_item32_t* item, uint16_t item_num, uint16_t mem_offset);

/**
 * @brief Initialize RMT driver
 *
 * @param channel RMT channel (0 - 7)
 *
 * @param rx_buf_size Size of RMT RX ringbuffer. Can be 0 if the RX ringbuffer is not used.
 *
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
 * @param channel RMT channel (0 - 7)
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
 * @brief RMT send waveform from rmt_item array.
 *
 *        This API allows user to send waveform with any length.
 *
 * @param channel RMT channel (0 - 7)
 *
 * @param rmt_item head point of RMT items array.
 *        If ESP_INTR_FLAG_IRAM is used, please do not use the memory allocated from psram when calling rmt_write_items.
 *
 * @param item_num RMT data item number.
 *
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
esp_err_t rmt_write_items(rmt_channel_t channel, const rmt_item32_t* rmt_item, int item_num, bool wait_tx_done);

/**
 * @brief Wait RMT TX finished.
 *
 * @param channel RMT channel (0 - 7)
 *
 * @param wait_time Maximum time in ticks to wait for transmission to be complete 
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
 * @param channel RMT channel (0 - 7)
 *
 * @param buf_handle Pointer to buffer handle to accept RX ringbuffer handle.
 *
 * @return
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_OK Success
 */
esp_err_t rmt_get_ringbuf_handle(rmt_channel_t channel, RingbufHandle_t* buf_handle);

/**
 * @brief Init rmt translator and register user callback.
 *        The callback will convert the raw data that needs to be sent to rmt format.
 *        If a channel is initialized more than once, tha user callback will be replaced by the later.
 *
 * @param channel RMT channel (0 - 7).
 *
 * @param fn Point to the data conversion function.
 *
 * @return
 *     - ESP_FAIL Init fail.
 *     - ESP_OK Init success.
 */
esp_err_t rmt_translator_init(rmt_channel_t channel, sample_to_rmt_t fn);

/**
 * @brief Translate uint8_t type of data into rmt format and send it out.
 *        Requires rmt_translator_init to init the translator first.
 *
 * @param channel RMT channel (0 - 7).
 *
 * @param src Pointer to the raw data.
 *
 * @param src_size The size of the raw data.
 *
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


/*
 * ----------------EXAMPLE OF RMT INTERRUPT ------------------
 * @code{c}
 *
 * rmt_isr_register(rmt_isr, NULL, 0);           //hook the ISR handler for RMT interrupt
 * @endcode
 * @note
 *     0. If you have called rmt_driver_install, you don't need to set ISR handler any more.
 *
 * ----------------EXAMPLE OF INTERRUPT HANDLER ---------------
 * @code{c}
 * #include "esp_attr.h"
 * void IRAM_ATTR rmt_isr_handler(void* arg)
 * {
 *    //read RMT interrupt status.
 *    uint32_t intr_st = RMT.int_st.val;
 *
 *    //you will find which channels have triggered an interrupt here,
 *    //then, you can post some event to RTOS queue to process the event.
 *    //later we will add a queue in the driver code.
 *
 *   //clear RMT interrupt status.
 *    RMT.int_clr.val = intr_st;
 * }
 * @endcode
 *
 *--------------------------END OF EXAMPLE --------------------------
 */



#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_RMT_CTRL_H_ */
