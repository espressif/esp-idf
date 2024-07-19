/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "hal/dw_gdma_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of DW_GDMA channel handle
 */
typedef struct dw_gdma_channel_t *dw_gdma_channel_handle_t;

/**
 * @brief Type of DW_GDMA link list handle
 */
typedef struct dw_gdma_link_list_t *dw_gdma_link_list_handle_t;

/**
 * @brief Type of DW_GDMA link list item handle
 */
typedef struct dw_gdma_link_list_item_t *dw_gdma_lli_handle_t;

/**
 * @brief A group of channel's static configurations
 *
 * @note By static, we mean these channel end configurations shouldn't be changed after the DMA channel is created.
 */
typedef struct {
    dw_gdma_block_transfer_type_t block_transfer_type; /*!< Block transfer type */
    dw_gdma_role_t role;                               /*!< Role of the DMA channel end */
    dw_gdma_handshake_type_t handshake_type;           /*!< Handshake type */
    uint8_t num_outstanding_requests;                  /*!< Number of R/W requests that the AXI master can issue to the slave before receiving a response.
                                                            Suggest value range: [1,16] */
    uint32_t status_fetch_addr;                        /*!< Address where to fetch the status of the peripheral */
} dw_gdma_channel_static_config_t;

/**
 * @brief Configurations for allocating a DMA channel
 */
typedef struct {
    dw_gdma_channel_static_config_t src;       /*!< source end static configuration */
    dw_gdma_channel_static_config_t dst;       /*!< destination end static configuration */
    dw_gdma_flow_controller_t flow_controller; /*!< Transfer flow controller */
    int chan_priority;                         /*!< DMA channel priority */
    int intr_priority;                         /*!< DMA interrupt priority,
                                                    if set to 0, the driver will try to allocate an interrupt with a relative low priority (1,2,3) */
} dw_gdma_channel_alloc_config_t;

/**
 * @brief Create a DMA channel
 *
 * @param[in] config Channel allocation configuration
 * @param[out] ret_chan Returned channel handle
 * @return
 *      - ESP_OK: Create DMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Create DMA channel failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create DMA channel failed because out of memory
 *      - ESP_FAIL: Create DMA channel failed because of other error
 */
esp_err_t dw_gdma_new_channel(const dw_gdma_channel_alloc_config_t *config, dw_gdma_channel_handle_t *ret_chan);

/**
 * @brief Delete DMA channel
 *
 * @param[in] chan DMA channel handle, allocated by `dw_gdma_new_channel`
 * @return
 *      - ESP_OK: Delete DMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Delete DMA channel failed because of invalid argument
 *      - ESP_FAIL: Delete DMA channel failed because of other error
 */
esp_err_t dw_gdma_del_channel(dw_gdma_channel_handle_t chan);

/**
 * @brief Get the DMA channel ID
 *
 * @note This API breaks the encapsulation of DW_GDMA Channel Object.
 *       With the returned channel ID, you can even bypass all other driver API and access Low Level API directly.
 *
 * @param[in] chan DMA channel handle, allocated by `dw_gdma_new_channel`
 * @param[out] channel_id Returned channel ID
 * @return
 *      - ESP_OK: Get DW_GDMA channel ID successfully
 *      - ESP_ERR_INVALID_ARG: Get DW_GDMA channel ID failed because of invalid argument
 *      - ESP_FAIL: Get DW_GDMA channel ID failed because of other error
 */
esp_err_t dw_gdma_channel_get_id(dw_gdma_channel_handle_t chan, int *channel_id);

/**
 * @brief A group of channel's dynamic configurations
 *
 * @note By dynamic, we mean these channel end configurations can be changed in each transfer.
 */
typedef struct {
    uint32_t addr;                     /*!< Memory address */
    dw_gdma_transfer_width_t width;    /*!< Transfer width */
    dw_gdma_burst_mode_t burst_mode;   /*!< Burst mode */
    dw_gdma_burst_items_t burst_items; /*!< Number of data items that are contained in one burst transaction */
    uint8_t burst_len;                 /*!< Burst transaction length, if set to 0, the hardware will apply a possible value as burst length */
    struct {
        uint32_t en_status_write_back: 1; /*!< Enable peripheral status write back */
    } flags;
} dw_gdma_channel_dynamic_config_t;

/**
 * @brief Channel block transfer configurations
 */
typedef struct {
    dw_gdma_channel_dynamic_config_t src; /*!< source configuration */
    dw_gdma_channel_dynamic_config_t dst; /*!< destination configuration */
    size_t size;                          /*!< Transfer size */
} dw_gdma_block_transfer_config_t;

/**
 * @brief Configure transfer parameters for a DMA channel
 *
 * @note This is an "all-in-one" function for set up the block transfer.
 * @note This function can't work with Link-List transfer type. For Link-List transfer, please use `dw_gdma_lli_config_transfer` instead.
 *
 * @param[in] chan DMA channel handle, allocated by `dw_gdma_new_channel`
 * @param[in] config Block transfer configurations
 * @return
 *      - ESP_OK: Configure DMA channel block transfer successfully
 *      - ESP_ERR_INVALID_ARG: Configure DMA channel block transfer failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Configure DMA channel block transfer failed because the channel has Link-List transfer type
 *      - ESP_FAIL: Configure DMA channel block transfer failed because of other error
 */
esp_err_t dw_gdma_channel_config_transfer(dw_gdma_channel_handle_t chan, const dw_gdma_block_transfer_config_t *config);

/**
 * @brief Enable or disable a DMA channel
 *
 * @note Before enabling a channel, you need to setup the channel transfer by either `dw_gdma_channel_config_transfer` or `dw_gdma_lli_config_transfer`
 * @note When a DMA channel is disabled, the DMA engine will stop working. You need to reconfigure the channel before enabling it again.
 * @note After all block transfers are completed, the DMA channel will be disabled automatically.
 *
 * @param[in] chan DMA channel handle, allocated by `dw_gdma_new_channel`
 * @param[in] en_or_dis True to enable, false to disable the DMA channel
 * @return
 *      - ESP_OK: Enable or disable DMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Enable or disable DMA channel failed because of invalid argument
 *      - ESP_FAIL: Enable or disable DMA channel failed because of other error
 */
esp_err_t dw_gdma_channel_enable_ctrl(dw_gdma_channel_handle_t chan, bool en_or_dis);

/**
 * @brief Suspend or resume a DMA channel
 *
 * @note When a DMA channel is suspended, the DMA engine will stop working gracefully and the channel's status will be saved.
 * @note The channel will exit the suspend state automatically if it is disabled.
 *
 * @param[in] chan DMA channel handle, allocated by `dw_gdma_new_channel`
 * @param[in] enter_or_exit True to suspend, false to resume the DMA channel
 * @return
 *      - ESP_OK: Suspend or resume DMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Suspend or resume DMA channel failed because of invalid argument
 *      - ESP_FAIL: Suspend or resume DMA channel failed because of other error
 */
esp_err_t dw_gdma_channel_suspend_ctrl(dw_gdma_channel_handle_t chan, bool enter_or_exit);

/**
 * @brief Abort the DMA channel
 *
 * @note If the channel is aborted, it will be disabled immediately, which may cause AXI bus protocol violation.
 * @note This function is recommended to only be used when the channel hangs. Recommend to try `dw_gdma_channel_enable_ctrl` first, then opt for aborting.
 *
 * @param[in] chan DMA channel handle, allocated by `dw_gdma_new_channel`
 * @return
 *      - ESP_OK: Abort DMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Abort DMA channel failed because of invalid argument
 *      - ESP_FAIL: Abort DMA channel failed because of other error
 */
esp_err_t dw_gdma_channel_abort(dw_gdma_channel_handle_t chan);

/**
 * @brief Lock the DMA channel at specific transfer level
 *
 * @note When a DMA channel is locked, no other channels are granted control of the master bus for the duration specified by the lock level.
 * @note Only lock the channel if you want to exclusive access to the master bus.
 * @note Channel locking feature is only for M2M transfer.
 *
 * @param[in] chan DMA channel handle, allocated by `dw_gdma_new_channel`
 * @param[in] level Transfer level
 * @return
 *      - ESP_OK: Lock DMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Lock DMA channel failed because of invalid argument
 *      - ESP_FAIL: Lock DMA channel failed because of other error
 */
esp_err_t dw_gdma_channel_lock(dw_gdma_channel_handle_t chan, dw_gdma_lock_level_t level);

/**
 * @brief Unlock the DMA channel
 *
 * @param[in] chan DMA channel handle, allocated by `dw_gdma_new_channel`
 * @return
 *      - ESP_OK: Unlock DMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Unlock DMA channel failed because of invalid argument
 *      - ESP_FAIL: Unlock DMA channel failed because of other error
 */
esp_err_t dw_gdma_channel_unlock(dw_gdma_channel_handle_t chan);

/**
 * @brief Continue the temporarily stopped DMA transfer because of invalid block
 *
 * @note You should only call this API when the block becomes valid again,
 *       by calling `dw_gdma_lli_set_block_markers`/`dw_gdma_channel_set_block_markers` with `is_valid` set to true.
 *
 * @param[in] chan DMA channel handle, allocated by `dw_gdma_new_channel`
 * @return
 *      - ESP_OK: Continue DMA transfer successfully
 *      - ESP_ERR_INVALID_ARG: Continue DMA transfer failed because of invalid argument
 *      - ESP_FAIL: Continue DMA transfer failed because of other error
 */
esp_err_t dw_gdma_channel_continue(dw_gdma_channel_handle_t chan);

/**
 * @brief Type of DW_GDMA trans done event data
 */
typedef struct {
} dw_gdma_trans_done_event_data_t;

/**
 * @brief Type of DW_GDMA trans_done event callback
 * @param chan GDMA channel handle, created from `dw_gdma_new_channel`
 * @param event_data GDMA event data
 * @param user_data User registered data from `dw_gdma_channel_register_event_callbacks`
 *
 * @return Whether a task switch is needed after the callback function returns,
 *         this is usually due to the callback wakes up some high priority task.
 */
typedef bool (*dw_gdma_trans_done_event_callback_t)(dw_gdma_channel_handle_t chan, const dw_gdma_trans_done_event_data_t *event_data, void *user_data);

/**
 * @brief Type of DW_GDMA break event data
 */
typedef struct {
    dw_gdma_lli_handle_t invalid_lli; /*!< Invalid link list item */
} dw_gdma_break_event_data_t;

/**
 * @brief Type of DW_GDMA break event callback
 * @param chan GDMA channel handle, created from `dw_gdma_new_channel`
 * @param event_data GDMA event data
 * @param user_data User registered data from `dw_gdma_channel_register_event_callbacks`
 *
 * @return Whether a task switch is needed after the callback function returns,
 *         this is usually due to the callback wakes up some high priority task.
 */
typedef bool (*dw_gdma_break_event_callback_t)(dw_gdma_channel_handle_t chan, const dw_gdma_break_event_data_t *event_data, void *user_data);

/**
 * @brief Group of supported DW_GDMA callbacks
 * @note The callbacks are all running under ISR environment
 */
typedef struct {
    dw_gdma_trans_done_event_callback_t on_block_trans_done; /*!< Invoked when a block transfer is completed */
    dw_gdma_trans_done_event_callback_t on_full_trans_done;  /*!< Invoked when all block transfers are completed */
    dw_gdma_break_event_callback_t on_invalid_block;         /*!< Invoked when an invalid block is detected */
} dw_gdma_event_callbacks_t;

/**
 * @brief Set DW_GDMA event callbacks for a channel
 * @note This API will lazy install the DW_GDMA interrupt service
 *
 * @param[in] chan DW_GDMA channel handle, allocated by `dw_gdma_new_channel`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t dw_gdma_channel_register_event_callbacks(dw_gdma_channel_handle_t chan, dw_gdma_event_callbacks_t *cbs, void *user_data);

/**
 * @brief DMA link list type
 */
typedef enum {
    DW_GDMA_LINKED_LIST_TYPE_SINGLY,   /*!< Singly linked list */
    DW_GDMA_LINKED_LIST_TYPE_CIRCULAR, /*!< Circular linked list */
} dw_gdma_link_list_type_t;

/**
 * @brief DMA link list configurations
 */
typedef struct {
    uint32_t num_items;                 //!< Number of link list items
    dw_gdma_link_list_type_t link_type; //!< Link list type
} dw_gdma_link_list_config_t;

/**
 * @brief Create a DMA link list
 *
 * @param[in] config Link list configurations
 * @param[out] ret_list Returned link list handle
 * @return
 *      - ESP_OK: Create DMA link list successfully
 *      - ESP_ERR_INVALID_ARG: Create DMA link list failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create DMA link list failed because out of memory
 *      - ESP_FAIL: Create DMA link list failed because of other error
 */
esp_err_t dw_gdma_new_link_list(const dw_gdma_link_list_config_t *config, dw_gdma_link_list_handle_t *ret_list);

/**
 * @brief Delete a DMA link list
 *
 * @param[in] list Link list handle, allocated by `dw_gdma_new_link_list`
 * @return
 *      - ESP_OK: Delete DMA link list successfully
 *      - ESP_ERR_INVALID_ARG: Delete DMA link list failed because of invalid argument
 *      - ESP_FAIL: Delete DMA link list failed because of other error
 */
esp_err_t dw_gdma_del_link_list(dw_gdma_link_list_handle_t list);

/**
 * @brief Apply a link list to a DMA channel
 *
 * @note This function can only work with Link-List transfer type.
 *
 * @param[in] chan DMA channel handle, allocated by `dw_gdma_new_channel`
 * @param[in] list Link list handle, allocated by `dw_gdma_new_link_list`
 * @return
 *      - ESP_OK: Apply link list to DMA channel successfully
 *      - ESP_ERR_INVALID_ARG: Apply link list to DMA channel failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Apply link list to DMA channel failed because the channel is not with Link-List transfer type
 *      - ESP_FAIL: Apply link list to DMA channel failed because of other error
 */
esp_err_t dw_gdma_channel_use_link_list(dw_gdma_channel_handle_t chan, dw_gdma_link_list_handle_t list);

/**
 * @brief A helper function to return an item from a given link list, by index
 *
 * @note The address of the returned item is not behind the cache
 *
 * @param[in] list Link list handle, allocated by `dw_gdma_new_link_list`
 * @param[in] item_index Index of the item
 * @return
 *      - NULL: Invalid argument
 *      - Others: Link list item handle
 */
dw_gdma_lli_handle_t dw_gdma_link_list_get_item(dw_gdma_link_list_handle_t list, int item_index);

/**
 * @brief Configure transfer parameters for a DMA link list item
 *
 * @note This is an "all-in-one" function for set up the link list item.
 * @note This function can only work with Link-List transfer type. For other transfer types, please use `dw_gdma_channel_config_transfer` instead.
 *
 * @param[in] lli Link list item
 * @param[in] config Block transfer configurations
 * @return
 *      - ESP_OK: Configure link list item block transfer successfully
 *      - ESP_ERR_INVALID_ARG: Configure link list item block transfer failed because of invalid argument
 *      - ESP_FAIL: Configure link list item block transfer failed because of other error
 */
esp_err_t dw_gdma_lli_config_transfer(dw_gdma_lli_handle_t lli, const dw_gdma_block_transfer_config_t *config);

/**
 * @brief Set the next link list item for a given DMA link list item
 *
 * @param[in] lli Current link list item, can be obtained from `dw_gdma_link_list_get_item`
 * @param[in] next Next link list item, can be obtained from `dw_gdma_link_list_get_item`
 * @return
 *      - ESP_OK: Set next link list item successfully
 *      - ESP_ERR_INVALID_ARG: Set next link list item failed because of invalid argument
 *      - ESP_FAIL: Set next link list item failed because of other error
 */
esp_err_t dw_gdma_lli_set_next(dw_gdma_lli_handle_t lli, dw_gdma_lli_handle_t next);

/**
 * @brief Markers of a DW_GDMA block
 */
typedef struct {
    uint32_t is_last: 1;             /*!< Set if this block is the last one */
    uint32_t is_valid: 1;            /*!< Set if this block is valid */
    uint32_t en_trans_done_intr: 1;  /*!< Set if to enable the transfer done interrupt for this block */
} dw_gdma_block_markers_t;

/**
 * @brief Set block markers for a DMA channel
 *
 * @note This function doesn't work for Link-List transfer type. For Link-List transfer, please use `dw_gdma_lli_set_block_markers` instead.
 * @note Setting the markers should always be the last step of configuring a block transfer, before enabling/continuing the channel.
 *
 * @param[in] chan DMA channel handle, allocated by `dw_gdma_new_channel`
 * @param[in] markers Block markers
 * @return
 *      - ESP_OK: Set block markers successfully
 *      - ESP_ERR_INVALID_ARG: Set block markers failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Set block markers failed because the channel has Link-List transfer type
 *      - ESP_FAIL: Set block markers failed because of other error
 */
esp_err_t dw_gdma_channel_set_block_markers(dw_gdma_channel_handle_t chan, dw_gdma_block_markers_t markers);

/**
 * @brief Set block markers for a DMA link list item
 *
 * @note Setting the markers should always be the last step of configuring a block transfer, before enabling/continuing the channel.
 *
 * @param[in] lli Link list item
 * @param[in] markers Block markers
 * @return
 *      - ESP_OK: Set block markers successfully
 *      - ESP_ERR_INVALID_ARG: Set block markers failed because of invalid argument
 *      - ESP_FAIL: Set block markers failed because of other error
 */
esp_err_t dw_gdma_lli_set_block_markers(dw_gdma_lli_handle_t lli, dw_gdma_block_markers_t markers);

#ifdef __cplusplus
}
#endif
