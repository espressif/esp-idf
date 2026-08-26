/** @file
 *  @brief Bluetooth byteorder API
 */

/*
 * SPDX-FileCopyrightText: 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_BYTEORDER_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_BYTEORDER_H_

/**
 * @brief Byteorder
 * @defgroup bt_byteorder Byteorder
 * @ingroup bluetooth
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Encode 16-bit value into array values in little-endian format.
 *
 *  Helper macro to encode 16-bit values into comma separated values.
 *
 *  @note @p _v is evaluated 2 times.
 *
 *  @param _v 16-bit integer in host endianness.
 *
 *  @return The comma separated values for the 16-bit value.
 */
#define BT_BYTES_LIST_LE16(_v)         \
    (((_v) >>  0) & 0xFFU),     \
    (((_v) >>  8) & 0xFFU)      \

/** @brief Encode 24-bit value into array values in little-endian format.
 *
 *  Helper macro to encode 24-bit values into comma separated values.
 *
 *  @note @p _v is evaluated 3 times.
 *
 *  @param _v 24-bit integer in host endianness.
 *
 *  @return The comma separated values for the 24-bit value.
 */
#define BT_BYTES_LIST_LE24(_v)         \
    BT_BYTES_LIST_LE16(_v),        \
    (((_v) >> 16) & 0xFFU)      \

/** @brief Encode 32-bit value into array values in little-endian format.
 *
 *  Helper macro to encode 32-bit values into comma separated values.
 *
 *  @note @p _v is evaluated 4 times.
 *
 *  @param _v 32-bit integer in host endianness.
 *
 *  @return The comma separated values for the 32-bit value.
 */
#define BT_BYTES_LIST_LE32(_v)         \
    BT_BYTES_LIST_LE24(_v),        \
    (((_v) >> 24) & 0xFFU)      \

/** @brief Encode 40-bit value into array values in little-endian format.
 *
 *  Helper macro to encode 40-bit values into comma separated values.
 *
 *  @note @p _v is evaluated 5 times.
 *
 *  @param _v 40-bit integer in host endianness.
 *
 *  @return The comma separated values for the 40-bit value.
 */
#define BT_BYTES_LIST_LE40(_v)         \
    BT_BYTES_LIST_LE24(_v),        \
    BT_BYTES_LIST_LE16((_v) >> 24) \

/** @brief Encode 48-bit value into array values in little-endian format.
 *
 *  Helper macro to encode 48-bit values into comma separated values.
 *
 *  @note @p _v is evaluated 6 times.
 *
 *  @param _v 48-bit integer in host endianness.
 *
 *  @return The comma separated values for the 48-bit value.
 */
#define BT_BYTES_LIST_LE48(_v)         \
    BT_BYTES_LIST_LE32(_v),        \
    BT_BYTES_LIST_LE16((_v) >> 32) \

/** @brief Encode 64-bit value into array values in little-endian format.
 *
 *  Helper macro to encode 64-bit values into comma separated values.
 *
 *  @note @p _v is evaluated 8 times.
 *
 *  @param _v 64-bit integer in host endianness.
 *
 *  @return The comma separated values for the 64-bit value.
 */
#define BT_BYTES_LIST_LE64(_v)         \
    BT_BYTES_LIST_LE32(_v),        \
    BT_BYTES_LIST_LE32((_v) >> 32) \

/** @brief Encode 16-bit value into array values in big-endian format.
 *
 *  Helper macro to encode 16-bit values into comma separated values.
 *
 *  @note @p _v is evaluated 2 times.
 *
 *  @param _v 16-bit integer in host endianness.
 *
 *  @return The comma separated values for the 16-bit value.
 */
#define BT_BYTES_LIST_BE16(_v) (((_v) >> 8) & 0xFFU), (((_v) >> 0) & 0xFFU)

/** @brief Encode 24-bit value into array values in big-endian format.
 *
 *  Helper macro to encode 24-bit values into comma separated values.
 *
 *  @note @p _v is evaluated 3 times.
 *
 *  @param _v 24-bit integer in host endianness.
 *
 *  @return The comma separated values for the 24-bit value.
 */
#define BT_BYTES_LIST_BE24(_v) (((_v) >> 16) & 0xFFU), BT_BYTES_LIST_BE16(_v)

/** @brief Encode 32-bit value into array values in big-endian format.
 *
 *  Helper macro to encode 32-bit values into comma separated values.
 *
 *  @note @p _v is evaluated 4 times.
 *
 *  @param _v 32-bit integer in host endianness.
 *
 *  @return The comma separated values for the 32-bit value.
 */
#define BT_BYTES_LIST_BE32(_v) (((_v) >> 24) & 0xFFU), BT_BYTES_LIST_BE24(_v)

/** @brief Encode 40-bit value into array values in big-endian format.
 *
 *  Helper macro to encode 40-bit values into comma separated values.
 *
 *  @note @p _v is evaluated 5 times.
 *
 *  @param _v 40-bit integer in host endianness.
 *
 *  @return The comma separated values for the 40-bit value.
 */
#define BT_BYTES_LIST_BE40(_v) BT_BYTES_LIST_BE16((_v) >> 24), BT_BYTES_LIST_BE24(_v)

/** @brief Encode 48-bit value into array values in big-endian format.
 *
 *  Helper macro to encode 48-bit values into comma separated values.
 *
 *  @note @p _v is evaluated 6 times.
 *
 *  @param _v 48-bit integer in host endianness.
 *
 *  @return The comma separated values for the 48-bit value.
 */
#define BT_BYTES_LIST_BE48(_v) BT_BYTES_LIST_BE16((_v) >> 32), BT_BYTES_LIST_BE32(_v)

/** @brief Encode 64-bit value into array values in big-endian format.
 *
 *  Helper macro to encode 64-bit values into comma separated values.
 *
 *  @note @p _v is evaluated 8 times.
 *
 *  @param _v 64-bit integer in host endianness.
 *
 *  @return The comma separated values for the 64-bit value.
 */
#define BT_BYTES_LIST_BE64(_v) BT_BYTES_LIST_BE32((_v) >> 32), BT_BYTES_LIST_BE32(_v)

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_BYTEORDER_H_ */
