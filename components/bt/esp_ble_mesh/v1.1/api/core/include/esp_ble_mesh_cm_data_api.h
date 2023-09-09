/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_CM_DATA_API_H_
#define _ESP_BLE_MESH_CM_DATA_API_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!< Definitions of the format of Extended_Model_Items indicator */
#define ESP_BLE_MESH_MODEL_ITEM_SHORT   0
#define ESP_BLE_MESH_MODEL_ITEM_LONG    1

/* The value of the Element_Offset field is mapped to an integer value.
 * |----------------|-------------------|
 * | Element_Offset | Represented Value |
 * |----------------|-------------------|
 * |        0       |         0         |
 * |        1       |         1         |
 * |        2       |         2         |
 * |        3       |         3         |
 * |        4       |        -4         |
 * |        5       |        -3         |
 * |        6       |        -2         |
 * |        7       |        -1         |
 * |----------------|-------------------|
 */

/** Format of Extended Model Item */
typedef struct {
    /** Union of Extended Model Item */
    union {
        uint8_t element_offset:3,                   /*!< Element address modifier, in the range -4 to 3. See above. */
                model_item_idx:5;                   /*!< Model Index, in the range 0 to 31 */
        /** Extended Model Item long format */
        struct {
            int8_t  element_offset;                 /*!< Element address modifier, in the range -128 to 127 */
            uint8_t model_item_idx;                 /*!< Model index, in the range 0 to 255 */
        } long_fmt;                                 /*!< Extended Model Item long format */
    };
} esp_ble_mesh_extended_model_item_t;               /*!< Format of Extended Model Item */

/** Format of Model Item */
typedef struct {
    uint8_t corresponding_present:1,                /*!< Corresponding_Group_ID field indicator */
            format:1,                               /*!< Format of Extended_Model_Items indicator */
            extended_items_count:6;                 /*!< Number of Extended Model Items in the Extended_Model_Items field */
    uint8_t corresponding_group_id;                 /*!< Corresponding group identifier */
    esp_ble_mesh_extended_model_item_t *const extended_model_items; /*!< List of Extended Model Items */
} esp_ble_mesh_model_item_t;                        /*!< Format of Model Item */

/** Format of element of Composition Data Page 1 */
typedef struct {
    const uint8_t num_s;                            /*!< A count of SIG Models Items in this element */
    const uint8_t num_v;                            /*!< A count of Vendor Models Items in this element */

    esp_ble_mesh_model_item_t *const model_items_s; /*!< A sequence of "num_s" SIG Model Items */
    esp_ble_mesh_model_item_t *const model_items_v; /*!< A sequence of "num_v" Vendor Model Items */
} esp_ble_mesh_comp_1_elem_t;                       /*!< Format of element of Composition Data Page 1 */

/** Format of Composition Data Page 1 */
typedef struct {
    size_t element_count;                   /*!< Element count */
    esp_ble_mesh_comp_1_elem_t *elements;   /*!< A sequence of element descriptions */
} esp_ble_mesh_comp_1_t;                    /*!< Format of Composition Data Page 1 */

/** Format of Metadata entry */
typedef struct {
    uint16_t metadata_len;                  /*!< Size of the Metadata field */
    uint16_t metadata_id;                   /*!< Bluetooth assigned number for the Metadata Identifier */
    const uint8_t *metadata;                /*!< Model’s metadata */
} esp_ble_mesh_metadata_entry_t;            /*!< Format of Metadata entry */

/** Format of Metadata item */
typedef struct {
    /** Union of model ID */
    union {
        uint16_t model_id;                  /*!< Model ID */
        /** Vendor model identifier */
        struct {
            uint16_t company_id;            /*!< Company ID */
            uint16_t model_id;              /*!< Model ID */
        } vnd;                              /*!< Vendor model identifier */
    };
    uint8_t metadata_entries_num;           /*!< Number of metadata entries */
    esp_ble_mesh_metadata_entry_t *const metadata_entries;  /*!< List of model’s metadata */
} esp_ble_mesh_metadata_item_t;             /*!< Format of Metadata item */

/** Format of Metadata element of Models Metadata Page 0/128 */
typedef struct {
    const uint8_t items_num_s;              /*!< Number of metadata items for SIG models in the element */
    const uint8_t items_num_v;              /*!< Number of metadata items for Vendor models in the element */

    esp_ble_mesh_metadata_item_t *const metadata_items_s;   /*!< List of metadata items for SIG models in the element */
    esp_ble_mesh_metadata_item_t *const metadata_items_v;   /*!< List of metadata items for Vendor models in the element */
} esp_ble_mesh_metadata_elem_t;                             /*!< Format of Metadata element of Models Metadata Page 0/128 */

/** Format of the Models Metadata Page 0/128 */
typedef struct {
    size_t element_count;                   /*!< Element count */
    esp_ble_mesh_metadata_elem_t *elements; /*!< List of metadata for models for each element */
} esp_ble_mesh_models_metadata_t;           /*!< Format of the Models Metadata Page 0/128 */

/**
 * @brief       Register Composition Data Page 1.
 *
 * @param[in]   comp: Pointer to Composition Data Page 1.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_comp_1_register(const esp_ble_mesh_comp_1_t *comp);

/**
 * @brief       Register Models Metadata Page 0 or 128.
 *
 * @param[in]   metadata:      Pointer to Models Metadata Page 0 or 128.
 * @param[in]   metadata_page: Models Metadata Page number, i.e. 0 or 128.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_models_metadata_register(const esp_ble_mesh_models_metadata_t *metadata,
                                                uint8_t metadata_page);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_CM_DATA_API_H_ */
