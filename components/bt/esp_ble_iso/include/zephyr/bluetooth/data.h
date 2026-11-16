/**
 * @file
 * @brief Bluetooth data types and helpers.
 */

/*
 * SPDX-FileCopyrightText: 2017 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_DATA_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_DATA_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the size of a serialized @ref bt_data given its data length.
 *
 * Size of 'AD Structure'->'Length' field, equal to 1.
 * Size of 'AD Structure'->'Data'->'AD Type' field, equal to 1.
 * Size of 'AD Structure'->'Data'->'AD Data' field, equal to data_len.
 *
 * See Core Specification Version 5.4 Vol. 3 Part C, 11, Figure 11.1.
 */
#define BT_DATA_SERIALIZED_SIZE(data_len) ((data_len) + 2)

/**
 * @brief Bluetooth data.
 *
 * @details Description of different AD Types that can be encoded into advertising data. Used to
 * form arrays that are passed to the @ref bt_le_adv_start function. The @ref BT_DATA define can
 * be used as a helpter to declare the elements of an @ref bt_data array.
 */
struct bt_data {
    /** Type of scan response data or advertisement data. */
    uint8_t type;
    /** Length of scan response data or advertisement data. */
    uint8_t data_len;
    /** Pointer to Scan response or advertisement data. */
    const uint8_t *data;
};

/**
 * @brief Helper to declare elements of bt_data arrays
 *
 * This macro is mainly for creating an array of struct bt_data
 * elements which is then passed to e.g. @ref bt_le_adv_start function.
 *
 * @param _type Type of advertising data field
 * @param _data Pointer to the data field payload
 * @param _data_len Number of octets behind the _data pointer
 */
#define BT_DATA(_type, _data, _data_len) \
    { \
        .type = (_type), \
        .data_len = (_data_len), \
        .data = (const uint8_t *)(_data), \
    }

/**
 * @brief Helper to declare elements of bt_data arrays
 *
 * This macro is mainly for creating an array of struct bt_data
 * elements which is then passed to e.g. @ref bt_le_adv_start function.
 *
 * @param _type Type of advertising data field
 * @param _bytes Variable number of single-byte parameters
 */
#define BT_DATA_BYTES(_type, _bytes...) \
    BT_DATA(_type, ((uint8_t []) { _bytes }), \
        sizeof((uint8_t []) { _bytes }))

#if 0
/**
 * @brief Get the total size (in octets) of a given set of @ref bt_data
 * structures.
 *
 * The total size includes the length (1 octet) and type (1 octet) fields for each element, plus
 * their respective data lengths.
 *
 * @param[in] data Array of @ref bt_data structures.
 * @param[in] data_count Number of @ref bt_data structures in @p data.
 *
 * @return Size of the concatenated data, built from the @ref bt_data structure set.
 */
size_t bt_data_get_len(const struct bt_data data[], size_t data_count);

/**
 * @brief Serialize a @ref bt_data struct into an advertising structure (a flat array).
 *
 * The data are formatted according to the Bluetooth Core Specification v. 5.4,
 * vol. 3, part C, 11.
 *
 * @param[in]  input Single @ref bt_data structure to read from.
 * @param[out] output Buffer large enough to store the advertising structure in
 *             @p input. The size of it must be at least the size of the
 *             `input->data_len + 2` (for the type and the length).
 *
 * @return Number of octets written in @p output.
 */
size_t bt_data_serialize(const struct bt_data *input, uint8_t *output);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_DATA_H_ */
