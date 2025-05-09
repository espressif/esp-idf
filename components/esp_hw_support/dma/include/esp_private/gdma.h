/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_etm.h"
#include "soc/gdma_channel.h"
#include "hal/gdma_types.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of GDMA channel handle
 *
 */
typedef struct gdma_channel_t *gdma_channel_handle_t;

/**
 * @brief Collection of configuration items that used for allocating GDMA channel
 *
 */
typedef struct {
    gdma_channel_handle_t sibling_chan; /*!< DMA sibling channel handle (NULL means having sibling is not necessary) */
    gdma_channel_direction_t direction; /*!< DMA channel direction */
    struct {
        int reserve_sibling: 1; /*!< If set, DMA channel allocator would prefer to allocate new channel in a new pair, and reserve sibling channel for future use */
        int isr_cache_safe: 1;  /*!< If set, DMA channel allocator would allocate interrupt in cache-safe region, and ISR is serviceable when cache is disabled */
    } flags;
} gdma_channel_alloc_config_t;

/**
 * @brief Type of GDMA event data
 */
typedef struct {
    union {
        intptr_t rx_eof_desc_addr; /*!< EOF descriptor address of RX channel (only valid for EOF event) */
        intptr_t tx_eof_desc_addr; /*!< EOF descriptor address of TX channel (only valid for EOF event) */
    };
    struct {
        uint32_t abnormal_eof: 1; /*!< If set, means the current DMA block has an abnormal/error EOF flag being set.
                                       It doesn't mean GDMA goes into an error condition, but indicates peripheral (e.g. UHCI) goes into an abnormal state */
        uint32_t normal_eof: 1;   /*!< If set, means the current DMA block has a normal/successful EOF flag being set */
    } flags;
} gdma_event_data_t;

/**
 * @brief Type of GDMA event callback
 * @param dma_chan GDMA channel handle, created from `gdma_new_channel`
 * @param event_data GDMA event data. Different event share the same data structure, but the caller may only use a few or none of the data members.
 * @param user_data User registered data from `gdma_register_tx_event_callbacks` or `gdma_register_rx_event_callbacks`
 *
 * @return Whether a task switch is needed after the callback function returns,
 *         this is usually due to the callback wakes up some high priority task.
 */
typedef bool (*gdma_event_callback_t)(gdma_channel_handle_t dma_chan, gdma_event_data_t *event_data, void *user_data);

/**
 * @brief Group of supported GDMA TX callbacks
 * @note The callbacks are all running under ISR environment
 */
typedef struct {
    gdma_event_callback_t on_trans_eof; /*!< Invoked when TX engine meets EOF descriptor */
    gdma_event_callback_t on_descr_err; /*!< Invoked when DMA encounters a descriptor error */
} gdma_tx_event_callbacks_t;

/**
 * @brief Group of supported GDMA RX callbacks
 * @note The callbacks are all running under ISR environment
 */
typedef struct {
    gdma_event_callback_t on_recv_eof;  /*!< Invoked when RX engine meets EOF descriptor */
    gdma_event_callback_t on_descr_err; /*!< Invoked when DMA encounters a descriptor error */
    gdma_event_callback_t on_recv_done; /*!< Invoked when finished to receive one RX descriptor */
} gdma_rx_event_callbacks_t;

/**
 * @brief Type of GDMA engine trigger
 * @note It's recommended to initialize this structure with `GDMA_MAKE_TRIGGER`.
 *
 */
typedef struct {
    gdma_trigger_peripheral_t periph; /*!< Target peripheral which will trigger DMA operations */
    int instance_id;                  /*!< Peripheral instance ID. Supported IDs are listed in `soc/gdma_channel.h`, e.g. SOC_GDMA_TRIG_PERIPH_UHCI0 */
    int bus_id;                       /*!< Which system bus should the DMA attached to */
} gdma_trigger_t;

/**
 * @brief Helper macro to initialize GDMA trigger
 * @note value of `peri` must be selected from `gdma_trigger_peripheral_t` enum.
 *       e.g. GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_I2S,0)
 *
 */
#define GDMA_MAKE_TRIGGER(peri, id) \
    (gdma_trigger_t) { .periph = peri, .instance_id = SOC_##peri##id, .bus_id = SOC_##peri##id##_BUS }

/**
 * @brief A collection of strategy item that each GDMA channel could apply
 *
 */
typedef struct {
    bool owner_check;      /*!< If set / clear, DMA channel enables / disables checking owner validity */
    bool auto_update_desc; /*!< If set / clear, DMA channel enables / disables hardware to update descriptor automatically (TX channel only) */
    bool eof_till_data_popped;   /*!< If set / clear, DMA channel out_eof event is triggered on out / in DMA hardware fifo */
} gdma_strategy_config_t;

/**
 * @brief Create AHB-GDMA channel
 * @note This API won't install interrupt service for the allocated channel.
 *       If interrupt service is needed, user has to register GDMA event callback by `gdma_register_tx_event_callbacks` or `gdma_register_rx_event_callbacks`.
 *
 * @param[in] config Pointer to a collection of configurations for allocating GDMA channel
 * @param[out] ret_chan Returned channel handle
 * @return
 *      - ESP_OK: Create DMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Create DMA channel failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create DMA channel failed because out of memory
 *      - ESP_FAIL: Create DMA channel failed because of other error
 */
esp_err_t gdma_new_ahb_channel(const gdma_channel_alloc_config_t *config, gdma_channel_handle_t *ret_chan);

/**
 * @brief Create AXI-GDMA channel
 * @note This API won't install interrupt service for the allocated channel.
 *       If interrupt service is needed, user has to register GDMA event callback by `gdma_register_tx_event_callbacks` or `gdma_register_rx_event_callbacks`.
 *
 * @param[in] config Pointer to a collection of configurations for allocating GDMA channel
 * @param[out] ret_chan Returned channel handle
 * @return
 *      - ESP_OK: Create DMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Create DMA channel failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create DMA channel failed because out of memory
 *      - ESP_FAIL: Create DMA channel failed because of other error
 */
esp_err_t gdma_new_axi_channel(const gdma_channel_alloc_config_t *config, gdma_channel_handle_t *ret_chan);

/**
 * @brief Connect GDMA channel to trigger peripheral
 *
 * @note Suggest to use helper macro `GDMA_MAKE_TRIGGER` to construct parameter `trig_periph`. e.g. GDMA_MAKE_TRIGGER(GDMA_TRIG_PERIPH_SHA,0)
 * @note Connecting to a peripheral will also reset the DMA FIFO and FSM automatically
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[in] trig_periph GDMA trigger peripheral
 * @return
 *      - ESP_OK: Connect GDMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Connect GDMA channel failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Connect GDMA channel failed because DMA channel is working with another peripheral
 *      - ESP_FAIL: Connect GDMA channel failed because of other error
 */
esp_err_t gdma_connect(gdma_channel_handle_t dma_chan, gdma_trigger_t trig_periph);

/**
 * @brief Disconnect GMA channel from peripheral
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @return
 *      - ESP_OK: Disconnect GDMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Disconnect GDMA channel failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Disconnect GDMA channel failed because DMA channel is not connected to any peripheral
 *      - ESP_FAIL: Disconnect DMA channel failed because of other error
 */
esp_err_t gdma_disconnect(gdma_channel_handle_t dma_chan);

/**
 * @brief Channel transfer configurations
 */
typedef struct {
    uint32_t max_data_burst_size; /*!< Set the max burst size when DMA read/write the data buffer.
                                       Set to 0 means to disable the data burst.
                                       Other value must be power of 2, e.g., 4/8/16/32/64 */
    bool access_ext_mem;          /*!< Set this if the DMA transfer will access external memory */
} gdma_transfer_config_t;

/**
 * @brief Configure transfer parameters for a DMA channel
 *
 * @note It's highly recommended to enable the burst mode and set proper burst size for the DMA channel,
 *       which can improve the performance in accessing external memory by a lot.
 *
 * @param[in] chan DMA channel handle, allocated by `gdma_new_channel`
 * @param[in] config Transfer configurations
 * @return
 *      - ESP_OK: Configure DMA transfer parameters successfully
 *      - ESP_ERR_INVALID_ARG: Configure DMA transfer parameters failed because of invalid argument
 *      - ESP_FAIL: Configure DMA transfer parameters failed because of other error
 */
esp_err_t gdma_config_transfer(gdma_channel_handle_t dma_chan, const gdma_transfer_config_t *config);

/**
 * @brief Get the alignment constraints for internal and external memory
 *
 * @note You should call this function after `gdma_config_transfer`, the later one can
 *       adjust the alignment constraints based on various conditions, e.g. burst size, memory encryption, etc.
 * @note You can use returned alignment value to validate if a DMA buffer provided by the upper layer meets the constraints.
 * @note The returned alignment doesn't take the cache line size into account, if you want to do aligned memory allocation,
 *       you should align the buffer size to the cache line size by yourself if the DMA buffer is behind a cache.
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[out] int_mem_alignment Internal memory alignment
 * @param[out] ext_mem_alignment External memory alignment
 * @return
 *      - ESP_OK: Get alignment constraints successfully
 *      - ESP_ERR_INVALID_ARG: Get alignment constraints failed because of invalid argument
 *      - ESP_FAIL: Get alignment constraints failed because of other error
 */
esp_err_t gdma_get_alignment_constraints(gdma_channel_handle_t dma_chan, size_t *int_mem_alignment, size_t *ext_mem_alignment);

/**
 * @brief Apply channel strategy for GDMA channel
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[in] config Configuration of GDMA channel strategy
 *      - ESP_OK: Apply channel strategy successfully
 *      - ESP_ERR_INVALID_ARG: Apply channel strategy failed because of invalid argument
 *      - ESP_FAIL: Apply channel strategy failed because of other error
 */
esp_err_t gdma_apply_strategy(gdma_channel_handle_t dma_chan, const gdma_strategy_config_t *config);

/**
 * @brief Set GDMA channel priority
 *
 * @note By default, all GDMA channels are with the same priority: 0. Channels with the same priority are served in round-robin manner.
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[in] priority Priority of GDMA channel, higher value means higher priority
 * @return
 *      - ESP_OK: Set GDMA channel priority successfully
 *      - ESP_ERR_INVALID_ARG: Set GDMA channel priority failed because of invalid argument, e.g. priority out of range [0,GDMA_LL_CHANNEL_MAX_PRIORITY]
 *      - ESP_FAIL: Set GDMA channel priority failed because of other error
 */
esp_err_t gdma_set_priority(gdma_channel_handle_t dma_chan, uint32_t priority);

/**
 * @brief Delete GDMA channel
 * @note If you call `gdma_new_channel` several times for a same peripheral, make sure you call this API the same times.
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @return
 *      - ESP_OK: Delete GDMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Delete GDMA channel failed because of invalid argument
 *      - ESP_FAIL: Delete GDMA channel failed because of other error
 */
esp_err_t gdma_del_channel(gdma_channel_handle_t dma_chan);

/**
 * @brief Get the group ID and the channel ID
 *
 * @note This API breaks the encapsulation of GDMA Channel Object.
 *       With the returned group/channel ID, you can even bypass all other GDMA driver API and access Low Level API directly.
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[out] group_id Returned group ID
 * @param[out] channel_id Returned channel ID
 * @return
 *      - ESP_OK: Get GDMA channel/group ID successfully
 *      - ESP_ERR_INVALID_ARG: Get GDMA channel/group ID failed because of invalid argument
 *      - ESP_FAIL: Get GDMA channel ID failed because of other error
 */
esp_err_t gdma_get_group_channel_id(gdma_channel_handle_t dma_chan, int *group_id, int *channel_id);

#define gdma_get_channel_id(dma_chan, channel_id) gdma_get_group_channel_id(dma_chan, NULL, channel_id)

/**
 * @brief Set GDMA event callbacks for TX channel
 * @note This API will install GDMA interrupt service for the channel internally
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t gdma_register_tx_event_callbacks(gdma_channel_handle_t dma_chan, gdma_tx_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Set GDMA event callbacks for RX channel
 * @note This API will install GDMA interrupt service for the channel internally
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t gdma_register_rx_event_callbacks(gdma_channel_handle_t dma_chan, gdma_rx_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Set DMA descriptor address and start engine
 *
 * @note This function is allowed to run within ISR context
 * @note This function is also allowed to run when Cache is disabled, if `CONFIG_GDMA_CTRL_FUNC_IN_IRAM` is enabled
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[in] desc_base_addr Base address of descriptors (usually the descriptors are chained into a link or ring)
 * @return
 *      - ESP_OK: Start DMA engine successfully
 *      - ESP_ERR_INVALID_ARG: Start DMA engine failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Start DMA engine failed because of invalid state, e.g. the channel is controlled by ETM, so can't start it manually
 *      - ESP_FAIL: Start DMA engine failed because of other error
 */
esp_err_t gdma_start(gdma_channel_handle_t dma_chan, intptr_t desc_base_addr);

/**
 * @brief Stop DMA engine
 *
 * @note This function is allowed to run within ISR context
 * @note This function is also allowed to run when Cache is disabled, if `CONFIG_GDMA_CTRL_FUNC_IN_IRAM` is enabled
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @return
 *      - ESP_OK: Stop DMA engine successfully
 *      - ESP_ERR_INVALID_ARG: Stop DMA engine failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Stop DMA engine failed because of invalid state, e.g. the channel is controlled by ETM, so can't stop it manually
 *      - ESP_FAIL: Stop DMA engine failed because of other error
 */
esp_err_t gdma_stop(gdma_channel_handle_t dma_chan);

/**
 * @brief Make the appended descriptors be aware to the DMA engine
 *
 * @note This function is allowed to run within ISR context
 * @note This function is also allowed to run when Cache is disabled, if `CONFIG_GDMA_CTRL_FUNC_IN_IRAM` is enabled
 * @note This API could also resume a paused DMA engine, make sure new descriptors have been appended to the descriptor chain before calling it.
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @return
 *      - ESP_OK: Send append command to DMA engine successfully
 *      - ESP_ERR_INVALID_ARG: Send append command to DMA engine failed because of invalid argument
 *      - ESP_FAIL: Send append command to DMA engine failed because of other error
 */
esp_err_t gdma_append(gdma_channel_handle_t dma_chan);

/**
 * @brief Reset DMA channel FIFO and internal finite state machine
 *
 * @note This function is allowed to run within ISR context
 * @note This function is also allowed to run when Cache is disabled, if `CONFIG_GDMA_CTRL_FUNC_IN_IRAM` is enabled
 * @note Resetting a DMA channel won't break the connection with the target peripheral
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @return
 *      - ESP_OK: DMA channel reset successfully
 *      - ESP_ERR_INVALID_ARG: DMA channel reset failed due to invalid arguments
 *      - ESP_FAIL: DMA channel reset failed due to other errors
 */
esp_err_t gdma_reset(gdma_channel_handle_t dma_chan);

#if SOC_GDMA_SUPPORT_ETM
/**
 * @brief GDMA ETM event configuration
 */
typedef struct {
    gdma_etm_event_type_t event_type; /*!< GDMA ETM event type */
} gdma_etm_event_config_t;

/**
 * @brief Get the ETM event for GDMA channel
 *
 * @note The created ETM event object can be deleted later by calling `esp_etm_del_event`
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[in] config GDMA ETM event configuration
 * @param[out] out_event Returned ETM event handle
 * @return
 *      - ESP_OK: Get ETM event successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM event failed because of invalid argument
 *      - ESP_ERR_NOT_SUPPORTED: Get ETM event failed because the GDMA hardware doesn't support ETM event
 *      - ESP_FAIL: Get ETM event failed because of other error
 */
esp_err_t gdma_new_etm_event(gdma_channel_handle_t dma_chan, const gdma_etm_event_config_t *config, esp_etm_event_handle_t *out_event);

/**
 * @brief GDMA ETM task configuration
 */
typedef struct {
    gdma_etm_task_type_t task_type; /*!< GDMA ETM task type */
} gdma_etm_task_config_t;

/**
 * @brief Get the ETM task for GDMA channel
 *
 * @note The created ETM task object can be deleted later by calling `esp_etm_del_task`
 * @note If the GDMA task (e.g. start/stop) is controlled by ETM, then you can't use `gdma_start`/`gdma_stop` to control it.
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[in] config GDMA ETM task configuration
 * @param[out] out_task Returned ETM task handle
 * @return
 *      - ESP_OK: Get ETM task successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM task failed because of invalid argument
 *      - ESP_ERR_NOT_SUPPORTED: Get ETM task failed because the gdma hardware doesn't support ETM task
 *      - ESP_FAIL: Get ETM task failed because of other error
 */
esp_err_t gdma_new_etm_task(gdma_channel_handle_t dma_chan, const gdma_etm_task_config_t *config, esp_etm_task_handle_t *out_task);
#endif // SOC_GDMA_SUPPORT_ETM

/**
 * @brief Get the mask of free M2M trigger IDs
 *
 * @note On some ESP targets (e.g. ESP32C3/S3), DMA trigger used for memory copy can be any of valid peripheral's trigger ID,
 *       which can bring conflict if the peripheral is also using the same trigger ID. This function can return the free IDs
 *       for memory copy, at the runtime.
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[out] mask Returned mask of free M2M trigger IDs
 * @return
 *      - ESP_OK: Get free M2M trigger IDs successfully
 *      - ESP_ERR_INVALID_ARG: Get free M2M trigger IDs failed because of invalid argument
 *      - ESP_FAIL: Get free M2M trigger IDs failed because of other error
 */
esp_err_t gdma_get_free_m2m_trig_id_mask(gdma_channel_handle_t dma_chan, uint32_t *mask);

#if SOC_GDMA_SUPPORT_CRC
/**
 * @brief CRC Calculator configuration
 */
typedef struct {
    uint32_t init_value;    /*!< CRC initial value */
    uint32_t crc_bit_width; /*!< CRC bit width */
    uint32_t poly_hex;      /*!< Polynomial Formula, in hex */
    bool reverse_data_mask; /*!< Reverse data mask, used when you want to reverse the input data (a.k.a, refin) */
} gdma_crc_calculator_config_t;

/**
 * @brief Configure CRC Calculator
 *
 * @note This function must be called before `gdma_start`.
 * @note The CRC Calculator will reset itself automatically if the DMA stops and starts again.
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[in] config CRC Calculator configuration
 * @return
 *      - ESP_OK: Configure CRC Calculator successfully
 *      - ESP_ERR_INVALID_ARG: Configure CRC Calculator failed because of invalid argument
 *      - ESP_FAIL: Configure CRC Calculator failed because of other error
 */
esp_err_t gdma_config_crc_calculator(gdma_channel_handle_t dma_chan, const gdma_crc_calculator_config_t *config);

/**
 * @brief Get CRC Calculator result
 *
 * @note You need to call this function before a new DMA transaction starts, otherwise the CRC results may be overridden.
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[out] result Returned CRC result
 * @return
 *      - ESP_OK: Get CRC result successfully
 *      - ESP_ERR_INVALID_ARG: Get CRC result failed because of invalid argument
 *      - ESP_FAIL: Get CRC result failed because of other error
 */
esp_err_t gdma_crc_get_result(gdma_channel_handle_t dma_chan, uint32_t *result);
#endif // SOC_GDMA_SUPPORT_CRC

#if SOC_GDMA_SUPPORT_WEIGHTED_ARBITRATION
/**
 * @brief Set GDMA channel weight (0 ~ 15), default weight is 1
 *
 * @note Once weight arbitration is enabled, The arbitrator will allocate the corresponding number of tokens based on the weight of each channel.
 *       You need to set weights for each channel. If the weight is 0, the channel requests will no longer be responded to.
 * @note If channels have different weights, the channel with a larger weight can get a larger bandwidth.
 *       e.g. if channel A has weight 2, channel B has weight 3 and channel C has weight 4, the percentage of bandwidth allocated to channel A is 2/9,
 *       the percentage of bandwidth allocated to channel B is 3/9, and the percentage of bandwidth allocated to channel C is 4/9.
 * @note Weighted arbitration is different from priority arbitration. "Weight" is used after comparing "priority"
 *       After the priority comparison, then arbitrator checks whether there are still unused tokens in the channel.
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[in] weight Weight of GDMA channel, higher value means higher priority in weighted arbitration.
 * @return
 *      - ESP_OK: Set GDMA channel weight successfully
 *      - ESP_ERR_INVALID_ARG: Set GDMA channel weight failed because of invalid argument. e.g. weight out of range [0,GDMA_LL_CHANNEL_MAX_WEIGHT]
 *      - ESP_FAIL: Set GDMA channel weight failed because of other error
 */
esp_err_t gdma_set_weight(gdma_channel_handle_t dma_chan, uint32_t weight);
#endif // SOC_GDMA_SUPPORT_WEIGHTED_ARBITRATION

/****************************************************************************************
 * Deprecated APIs (will be removed in esp-idf 6.0)
 ****************************************************************************************/

/** @cond */
/**
 * @brief Create GDMA channel (Legacy API)
 *
 * @param[in] config Pointer to a collection of configurations for allocating GDMA channel
 * @param[out] ret_chan Returned channel handle
 * @return
 *      - ESP_OK: Create DMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Create DMA channel failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create DMA channel failed because out of memory
 *      - ESP_FAIL: Create DMA channel failed because of other error
 */
esp_err_t gdma_new_channel(const gdma_channel_alloc_config_t *config, gdma_channel_handle_t *ret_chan)
__attribute__((deprecated("please use gdma_new_ahb_channel or gdma_new_axi_channel respectively")));

/**
 * @brief GDMA transfer ability
 *
 * @note The alignment set in this structure is **not** a guarantee that gdma driver will take care of the nonalignment cases.
 *       Actually the GDMA driver has no knowledge about the DMA buffer (address and size) used by upper layer.
 *       So it's the responsibility of the **upper layer** to take care of the buffer address and size.
 *
 */
typedef struct {
    size_t sram_trans_align;  /*!< DMA transfer alignment for memory in SRAM, in bytes. The driver enables/disables burst mode based on this value. 0 means no alignment is required */
    size_t psram_trans_align; /*!< DMA transfer alignment for memory in PSRAM, in bytes. The driver sets proper burst block size based on the alignment value. 0 means no alignment is required */
} gdma_transfer_ability_t;

/**
 * @brief Set DMA channel transfer ability
 *
 * @param[in] dma_chan GDMA channel handle, allocated by `gdma_new_channel`
 * @param[in] ability Transfer ability, e.g. alignment
 * @return
 *      - ESP_OK: Set DMA channel transfer ability successfully
 *      - ESP_ERR_INVALID_ARG: Set DMA channel transfer ability failed because of invalid argument
 *      - ESP_FAIL: Set DMA channel transfer ability failed because of other error
 */
esp_err_t gdma_set_transfer_ability(gdma_channel_handle_t dma_chan, const gdma_transfer_ability_t *ability)
__attribute__((deprecated("please use gdma_config_transfer instead")));
/** @endcond */

#ifdef __cplusplus
}
#endif
