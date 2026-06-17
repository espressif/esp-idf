/** @file
 *  @brief Bluetooth device address definitions and utilities.
 */

/*
 * SPDX-FileCopyrightText: 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_ADDR_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_ADDR_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Bluetooth device address definitions and utilities.
 * @defgroup bt_addr Device Address
 * @ingroup bluetooth
 * @{
 */

#define BT_ADDR_LE_PUBLIC       0x00
#define BT_ADDR_LE_RANDOM       0x01
#define BT_ADDR_LE_PUBLIC_ID    0x02
#define BT_ADDR_LE_RANDOM_ID    0x03
#define BT_ADDR_LE_UNRESOLVED   0xFE /* Resolvable Private Address
                      * (Controller unable to resolve)
                      */
#define BT_ADDR_LE_ANONYMOUS    0xFF /* No address provided
                      * (anonymous advertisement)
                      */

/** Length in bytes of a standard Bluetooth address */
#define BT_ADDR_SIZE 6

/** Bluetooth Device Address */
typedef struct {
    uint8_t  val[BT_ADDR_SIZE];
} bt_addr_t;
/**/

/** Length in bytes of an LE Bluetooth address. Not packed, so no sizeof() */
#define BT_ADDR_LE_SIZE 7

/** Bluetooth LE Device Address */
typedef struct {
    uint8_t      type;
    bt_addr_t a;
} bt_addr_le_t;

/* Global Bluetooth address constants defined in bluetooth/common/addr.c */
extern const bt_addr_t bt_addr_any;
extern const bt_addr_t bt_addr_none;
extern const bt_addr_le_t bt_addr_le_any;
extern const bt_addr_le_t bt_addr_le_none;

/** Bluetooth device "any" address, not a valid address */
#define BT_ADDR_ANY     (&bt_addr_any)
/** Bluetooth device "none" address, not a valid address */
#define BT_ADDR_NONE    (&bt_addr_none)
/** Bluetooth LE device "any" address, not a valid address */
#define BT_ADDR_LE_ANY  (&bt_addr_le_any)
/** Bluetooth LE device "none" address, not a valid address */
#define BT_ADDR_LE_NONE (&bt_addr_le_none)

/** @brief Compare Bluetooth device addresses.
 *
 *  @param a First Bluetooth device address to compare
 *  @param b Second Bluetooth device address to compare
 *
 *  @return negative value if @a a < @a b, 0 if @a a == @a b, else positive
 */
static inline int bt_addr_cmp(const bt_addr_t *a, const bt_addr_t *b)
{
    return memcmp(a, b, sizeof(*a));
}

/** @brief Determine equality of two Bluetooth device addresses.
 *
 *  @retval #true if the two addresses are equal
 *  @retval #false otherwise
 */
static inline bool bt_addr_eq(const bt_addr_t *a, const bt_addr_t *b)
{
    return bt_addr_cmp(a, b) == 0;
}

/** @brief Compare Bluetooth LE device addresses.
 *
 *  @param a First Bluetooth LE device address to compare
 *  @param b Second Bluetooth LE device address to compare
 *
 *  @return negative value if @a a < @a b, 0 if @a a == @a b, else positive
 *
 *  @sa bt_addr_le_eq
 */
static inline int bt_addr_le_cmp(const bt_addr_le_t *a, const bt_addr_le_t *b)
{
    return memcmp(a, b, sizeof(*a));
}

/** @brief Determine equality of two Bluetooth LE device addresses.
 *
 *  The Bluetooth LE addresses are equal if and only if both the types and
 *  the 48-bit addresses are numerically equal.
 *
 *  @retval #true if the two addresses are equal
 *  @retval #false otherwise
 */
static inline bool bt_addr_le_eq(const bt_addr_le_t *a, const bt_addr_le_t *b)
{
    return bt_addr_le_cmp(a, b) == 0;
}

/** @brief Copy Bluetooth device address.
 *
 *  @param dst Bluetooth device address destination buffer.
 *  @param src Bluetooth device address source buffer.
 */
static inline void bt_addr_copy(bt_addr_t *dst, const bt_addr_t *src)
{
    memcpy(dst, src, sizeof(*dst));
}

/** @brief Copy Bluetooth LE device address.
 *
 *  @param dst Bluetooth LE device address destination buffer.
 *  @param src Bluetooth LE device address source buffer.
 */
static inline void bt_addr_le_copy(bt_addr_le_t *dst, const bt_addr_le_t *src)
{
    memcpy(dst, src, sizeof(*dst));
}

/** Check if a Bluetooth LE random address is resolvable private address. */
#define BT_ADDR_IS_RPA(a)     (((a)->val[5] & 0xc0) == 0x40)
/** Check if a Bluetooth LE random address is a non-resolvable private address.
 */
#define BT_ADDR_IS_NRPA(a)    (((a)->val[5] & 0xc0) == 0x00)
/** Check if a Bluetooth LE random address is a static address. */
#define BT_ADDR_IS_STATIC(a)  (((a)->val[5] & 0xc0) == 0xc0)

/** Set a Bluetooth LE random address as a resolvable private address. */
#define BT_ADDR_SET_RPA(a)    ((a)->val[5] = (((a)->val[5] & 0x3f) | 0x40))
/** Set a Bluetooth LE random address as a non-resolvable private address. */
#define BT_ADDR_SET_NRPA(a)   ((a)->val[5] &= 0x3f)
/** Set a Bluetooth LE random address as a static address. */
#define BT_ADDR_SET_STATIC(a) ((a)->val[5] |= 0xc0)

/** @brief Check if a Bluetooth LE address is a random private resolvable
 *         address.
 *
 *  @param addr Bluetooth LE device address.
 *
 *  @return true if address is a random private resolvable address.
 */
static inline bool bt_addr_le_is_rpa(const bt_addr_le_t *addr)
{
    if (addr->type != BT_ADDR_LE_RANDOM) {
        return false;
    }

    return BT_ADDR_IS_RPA(&addr->a);
}

/** @brief Check if a Bluetooth LE address is valid identity address.
 *
 *  Valid Bluetooth LE identity addresses are either public address or
 *  random static address.
 *
 *  @param addr Bluetooth LE device address.
 *
 *  @return true if address is a valid identity address.
 */
static inline bool bt_addr_le_is_identity(const bt_addr_le_t *addr)
{
    if (addr->type == BT_ADDR_LE_PUBLIC) {
        return true;
    }

    return BT_ADDR_IS_STATIC(&addr->a);
}

/**
 *  @brief Recommended length of user string buffer for Bluetooth address
 *
 *  @details The recommended length guarantee the output of address
 *  conversion will not lose valuable information about address being
 *  processed.
 */
#define BT_ADDR_STR_LEN 18

/**
 *  @brief Recommended length of user string buffer for Bluetooth LE address
 *
 *  @details The recommended length guarantee the output of address
 *  conversion will not lose valuable information about address being
 *  processed.
 */
#define BT_ADDR_LE_STR_LEN 30

/** @brief Converts binary Bluetooth address to string.
 *
 *  @param addr Address of buffer containing binary Bluetooth address.
 *  @param str Address of user buffer with enough room to store formatted
 *  string containing binary address.
 *  @param len Length of data to be copied to user string buffer. Refer to
 *  BT_ADDR_STR_LEN about recommended value.
 *
 *  @return Number of successfully formatted bytes from binary address.
 */
static inline int bt_addr_to_str(const bt_addr_t *addr, char *str, size_t len)
{
    return snprintf(str, len, "%02X:%02X:%02X:%02X:%02X:%02X",
                    addr->val[5], addr->val[4], addr->val[3],
                    addr->val[2], addr->val[1], addr->val[0]);
}

/** @brief Converts binary LE Bluetooth address to string.
 *
 *  @param addr Address of buffer containing binary LE Bluetooth address.
 *  @param str Address of user buffer with enough room to store
 *  formatted string containing binary LE address.
 *  @param len Length of data to be copied to user string buffer. Refer to
 *  BT_ADDR_LE_STR_LEN about recommended value.
 *
 *  @return Number of successfully formatted bytes from binary address.
 */
static inline int bt_addr_le_to_str(const bt_addr_le_t *addr, char *str,
                                    size_t len)
{
    char type[10];

    switch (addr->type) {
    case BT_ADDR_LE_PUBLIC:
        strcpy(type, "public");
        break;
    case BT_ADDR_LE_RANDOM:
        strcpy(type, "random");
        break;
    case BT_ADDR_LE_PUBLIC_ID:
        strcpy(type, "public-id");
        break;
    case BT_ADDR_LE_RANDOM_ID:
        strcpy(type, "random-id");
        break;
    default:
        snprintf(type, sizeof(type), "0x%02x", addr->type);
        break;
    }

    return snprintf(str, len, "%02X:%02X:%02X:%02X:%02X:%02X (%s)",
                    addr->a.val[5], addr->a.val[4], addr->a.val[3],
                    addr->a.val[2], addr->a.val[1], addr->a.val[0], type);
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_ADDR_H_ */
