/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of GDMA link list handle
 */
typedef struct gdma_link_list_t *gdma_link_list_handle_t;

/**
 * @brief DMA link list configurations
 */
typedef struct {
    uint32_t num_items;              //!< Number of nodes in the link list
    size_t item_alignment;           //!< Alignment of each list item required by the DMA. By default, it's 4 bytes alignment.
    size_t buffer_alignment;         //!< Alignment of each buffer required by the DMA. By default, it's 1 byte alignment.
    struct gdma_link_list_flags {
        uint32_t items_in_ext_mem: 1; //!< Whether the link list items are allocated from external memory
        uint32_t check_owner: 1;      //!< Whether the link list is responsible for checking the ownership when mount data buffers
    } flags;
} gdma_link_list_config_t;

/**
 * @brief Create a DMA link list
 *
 * @note This function will allocate memory for the link list.
 *
 * @param[in] config Link list configurations
 * @param[out] ret_list Returned link list handle
 * @return
 *      - ESP_OK: Create DMA link list successfully
 *      - ESP_ERR_INVALID_ARG: Create DMA link list failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create DMA link list failed because out of memory
 *      - ESP_FAIL: Create DMA link list failed because of other error
 */
esp_err_t gdma_new_link_list(const gdma_link_list_config_t *config, gdma_link_list_handle_t *ret_list);

/**
 * @brief Delete a DMA link list
 *
 * @param[in] list Link list handle, allocated by `gdma_new_link_list`
 * @return
 *      - ESP_OK: Delete DMA link list successfully
 *      - ESP_ERR_INVALID_ARG: Delete DMA link list failed because of invalid argument
 *      - ESP_FAIL: Delete DMA link list failed because of other error
 */
esp_err_t gdma_del_link_list(gdma_link_list_handle_t list);

/**
 * @brief DMA buffer mount configurations
 */
typedef struct {
    void *buffer;   //!< Buffer to be mounted to the DMA link list
    size_t length;  //!< Number of bytes that are expected to be transferred
    struct gdma_buffer_mount_flags {
        uint32_t mark_eof: 1;   /*!< Whether to mark the list item as the "EOF" item.
                                     Note, an "EOF" descriptor can be interrupted differently by peripheral.
                                     But it doesn't mean to terminate a DMA link (use `mark_final` instead).
                                     EOF link list item can also trigger an interrupt. */
        uint32_t mark_final: 1; /*!< Whether to terminate the DMA link list at this item.
                                     Note, DMA engine will stop at this item and trigger an interrupt.
                                     If `mark_final` is not set, this list item will point to the next item, and
                                     wrap around to the head item if it's the last one in the list. */
        uint32_t bypass_buffer_align_check: 1; /*!< Whether to bypass the buffer alignment check.
                                                    Only enable it when you know what you are doing. */
    } flags; //!< Flags for buffer mount configurations
} gdma_buffer_mount_config_t;

/**
 * @brief Mount one or more buffers to a given link list
 *
 * @note Different buffers won't be mounted to the same DMA link list item
 * @note After mount to the last list item, the next list item will be the head item (wrap around)
 *
 * @param[in] list Link list handle, allocated by `gdma_new_link_list`
 * @param[in] start_item_index Index of the first item in the link list to be mounted
 * @param[in] buf_config_array Array of buffer mount configurations
 * @param[in] num_buf Number of buffers to be mounted
 * @param[out] end_item_index Index of the last item in the link list that has been mounted
 * @return
 *      - ESP_OK: Mount the buffer successfully
 *      - ESP_ERR_INVALID_ARG: Mount the buffer failed because of invalid argument
 *      - ESP_FAIL: Mount the buffer failed because of other error
 */
esp_err_t gdma_link_mount_buffers(gdma_link_list_handle_t list, int start_item_index, const gdma_buffer_mount_config_t *buf_config_array, size_t num_buf, int *end_item_index);

/**
 * @brief Get the address of the head item in the link list
 *
 * @note The head address can be used to start a DMA transfer
 *
 * @param[in] list Link list handle, allocated by `gdma_new_link_list`
 * @return
 *      - Address of the head item in the link list
 *      - NULL: Get the address failed
 */
uintptr_t gdma_link_get_head_addr(gdma_link_list_handle_t list);

/**
 * @brief Concatenate two link lists as follows:
 *
 *        Link A: A1 --> A2 --> A3 --> A4
 *                    | item_index
 *                    +-----+
 *                          |
 *                          v item_index
 *        Link B: B1 --> B2 --> B3 --> B4
 *
 *      After concatenation:
 *       Link A: A1 --> B3 --> B4
 *       Link B: B1 --> B2 --> B3 --> B4
 *
 * @param[in] first_link First link list handle, allocated by `gdma_new_link_list`
 * @param[in] first_link_item_index Index of the item in the first link list (-1 means the last item)
 * @param[in] second_link Second link list handle, allocated by `gdma_new_link_list`
 * @param[in] second_link_item_index Index of the item in the second link list (-1 means the last item)
 * @return
 *      - ESP_OK: Concatenate the link lists successfully
 *      - ESP_ERR_INVALID_ARG: Concatenate the link lists failed because of invalid argument
 *      - ESP_FAIL: Concatenate the link lists failed because of other error
 */
esp_err_t gdma_link_concat(gdma_link_list_handle_t first_link, int first_link_item_index, gdma_link_list_handle_t second_link, int second_link_item_index);

/**
 * @brief GDMA link list item owner
 */
typedef enum {
    GDMA_LLI_OWNER_CPU = 0, /*!< GDMA link list item is only allowed to be accessed by CPU */
    GDMA_LLI_OWNER_DMA = 1, /*!< GDMA link list item is only allowed to be accessed by DMA */
} gdma_lli_owner_t;

/**
 * @brief Set the ownership for a DMA link list item
 *
 * @param[in] list Link list handle, allocated by `gdma_new_link_list`
 * @param[in] item_index Index of the link list item (-1 means the last item)
 * @param[in] owner Ownership
 * @return
 *      - ESP_OK: Set the ownership successfully
 *      - ESP_ERR_INVALID_ARG: Set the ownership failed because of invalid argument
 *      - ESP_FAIL: Set the ownership failed because of other error
 */
esp_err_t gdma_link_set_owner(gdma_link_list_handle_t list, int item_index, gdma_lli_owner_t owner);

/**
 * @brief Get the ownership of a DMA link list item
 *
 * @param[in] list Link list handle, allocated by `gdma_new_link_list`
 * @param[in] item_index Index of the link list item (-1 means the last item)
 * @param[out] owner Ownership
 * @return
 *      - ESP_OK: Get the ownership successfully
 *      - ESP_ERR_INVALID_ARG: Get the ownership failed because of invalid argument
 *      - ESP_FAIL: Get the ownership failed because of other error
 */
esp_err_t gdma_link_get_owner(gdma_link_list_handle_t list, int item_index, gdma_lli_owner_t *owner);

/**
 * @brief Get the size of the buffer that is mounted to the link list until the eof item (inclusive)
 *
 * @param[in] list Link list handle, allocated by `gdma_new_link_list`
 * @param[in] start_item_index Index of the first item in the link list to be calculated
 * @return Size of the buffer that is mounted to the link list until the eof item (inclusive).
 *         If the link list is empty or invalid, return 0.
 */
size_t gdma_link_count_buffer_size_till_eof(gdma_link_list_handle_t list, int start_item_index);

#ifdef __cplusplus
}
#endif
