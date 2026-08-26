/** @file
 *  @brief Byte order helpers.
 */

/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_SYS_BYTEORDER_H_
#define ZEPHYR_INCLUDE_SYS_BYTEORDER_H_

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys/util_macro.h>
#include <zephyr/toolchain.h>

#define BSWAP_16(x) ((uint16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))
#define BSWAP_24(x) ((uint32_t) ((((x) >> 16) & 0xff) | \
                   (((x)) & 0xff00) | \
                   (((x) & 0xff) << 16)))
#define BSWAP_32(x) ((uint32_t) ((((x) >> 24) & 0xff) | \
                   (((x) >> 8) & 0xff00) | \
                   (((x) & 0xff00) << 8) | \
                   (((x) & 0xff) << 24)))
#define BSWAP_40(x) ((uint64_t) ((((x) >> 32) & 0xff) | \
                   (((x) >> 16) & 0xff00) | \
                   (((x)) & 0xff0000) | \
                   (((x) & 0xff00) << 16) | \
                   (((x) & 0xff) << 32)))
#define BSWAP_48(x) ((uint64_t) ((((x) >> 40) & 0xff) | \
                   (((x) >> 24) & 0xff00) | \
                   (((x) >> 8) & 0xff0000) | \
                   (((x) & 0xff0000) << 8) | \
                   (((x) & 0xff00) << 24) | \
                   (((x) & 0xff) << 40)))
#define BSWAP_64(x) ((uint64_t) ((((x) >> 56) & 0xff) | \
                   (((x) >> 40) & 0xff00) | \
                   (((x) >> 24) & 0xff0000) | \
                   (((x) >> 8) & 0xff000000) | \
                   (((x) & 0xff000000) << 8) | \
                   (((x) & 0xff0000) << 24) | \
                   (((x) & 0xff00) << 40) | \
                   (((x) & 0xff) << 56)))

/** @def sys_le16_to_cpu
 *  @brief Convert 16-bit integer from little-endian to host endianness.
 *
 *  @param val 16-bit integer in little-endian format.
 *
 *  @return 16-bit integer in host endianness.
 */

/** @def sys_cpu_to_le16
 *  @brief Convert 16-bit integer from host endianness to little-endian.
 *
 *  @param val 16-bit integer in host endianness.
 *
 *  @return 16-bit integer in little-endian format.
 */

/** @def sys_le24_to_cpu
 *  @brief Convert 24-bit integer from little-endian to host endianness.
 *
 *  @param val 24-bit integer in little-endian format.
 *
 *  @return 24-bit integer in host endianness.
 */

/** @def sys_cpu_to_le24
 *  @brief Convert 24-bit integer from host endianness to little-endian.
 *
 *  @param val 24-bit integer in host endianness.
 *
 *  @return 24-bit integer in little-endian format.
 */

/** @def sys_le32_to_cpu
 *  @brief Convert 32-bit integer from little-endian to host endianness.
 *
 *  @param val 32-bit integer in little-endian format.
 *
 *  @return 32-bit integer in host endianness.
 */

/** @def sys_cpu_to_le32
 *  @brief Convert 32-bit integer from host endianness to little-endian.
 *
 *  @param val 32-bit integer in host endianness.
 *
 *  @return 32-bit integer in little-endian format.
 */

/** @def sys_le48_to_cpu
 *  @brief Convert 48-bit integer from little-endian to host endianness.
 *
 *  @param val 48-bit integer in little-endian format.
 *
 *  @return 48-bit integer in host endianness.
 */

/** @def sys_cpu_to_le48
 *  @brief Convert 48-bit integer from host endianness to little-endian.
 *
 *  @param val 48-bit integer in host endianness.
 *
 *  @return 48-bit integer in little-endian format.
 */

/** @def sys_be16_to_cpu
 *  @brief Convert 16-bit integer from big-endian to host endianness.
 *
 *  @param val 16-bit integer in big-endian format.
 *
 *  @return 16-bit integer in host endianness.
 */

/** @def sys_cpu_to_be16
 *  @brief Convert 16-bit integer from host endianness to big-endian.
 *
 *  @param val 16-bit integer in host endianness.
 *
 *  @return 16-bit integer in big-endian format.
 */

/** @def sys_be24_to_cpu
 *  @brief Convert 24-bit integer from big-endian to host endianness.
 *
 *  @param val 24-bit integer in big-endian format.
 *
 *  @return 24-bit integer in host endianness.
 */

/** @def sys_cpu_to_be24
 *  @brief Convert 24-bit integer from host endianness to big-endian.
 *
 *  @param val 24-bit integer in host endianness.
 *
 *  @return 24-bit integer in big-endian format.
 */

/** @def sys_be32_to_cpu
 *  @brief Convert 32-bit integer from big-endian to host endianness.
 *
 *  @param val 32-bit integer in big-endian format.
 *
 *  @return 32-bit integer in host endianness.
 */

/** @def sys_cpu_to_be32
 *  @brief Convert 32-bit integer from host endianness to big-endian.
 *
 *  @param val 32-bit integer in host endianness.
 *
 *  @return 32-bit integer in big-endian format.
 */

/** @def sys_be48_to_cpu
 *  @brief Convert 48-bit integer from big-endian to host endianness.
 *
 *  @param val 48-bit integer in big-endian format.
 *
 *  @return 48-bit integer in host endianness.
 */

/** @def sys_cpu_to_be48
 *  @brief Convert 48-bit integer from host endianness to big-endian.
 *
 *  @param val 48-bit integer in host endianness.
 *
 *  @return 48-bit integer in big-endian format.
 */

/** @def sys_uint16_to_array
 *  @brief Convert 16-bit unsigned integer to byte array.
 *
 *  @details Byte order aware macro to treat an unsigned integer
 *           as an array, rather than an integer literal. For example,
 *           `0x0123` would be converted to `{0x01, 0x23}` for big endian
 *           machines, and `{0x23, 0x01}` for little endian machines.
 *
 *  @param val 16-bit unsigned integer.
 *
 *  @return 16-bit unsigned integer as byte array.
 */

/** @def sys_uint32_to_array
 *  @brief Convert 32-bit unsigned integer to byte array.
 *
 *  @details Byte order aware macro to treat an unsigned integer
 *           as an array, rather than an integer literal. For example,
 *           `0x01234567` would be converted to `{0x01, 0x23, 0x45, 0x67}`
 *           for big endian machines, and `{0x67, 0x45, 0x23, 0x01}` for
 *           little endian machines.
 *
 *  @param val 32-bit unsigned integer.
 *
 *  @return 32-bit unsigned integer as byte array.
 */

/** @def sys_uint64_to_array
 *  @brief Convert 64-bit unsigned integer to byte array.
 *
 *  @details Byte order aware macro to treat an unsigned integer
 *           as an array, rather than an integer literal. For example,
 *           `0x0123456789abcdef` would be converted to
 *           `{0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef}`
 *           for big endian machines, and
 *           `{0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01}` for
 *           little endian machines.
 *
 *  @param val 64-bit unsigned integer.
 *
 *  @return 64-bit unsigned integer as byte array.
 */

#ifdef CONFIG_LITTLE_ENDIAN
#define sys_le16_to_cpu(val) (val)
#define sys_cpu_to_le16(val) (val)
#define sys_le24_to_cpu(val) (val)
#define sys_cpu_to_le24(val) (val)
#define sys_le32_to_cpu(val) (val)
#define sys_cpu_to_le32(val) (val)
#define sys_le40_to_cpu(val) (val)
#define sys_cpu_to_le40(val) (val)
#define sys_le48_to_cpu(val) (val)
#define sys_cpu_to_le48(val) (val)
#define sys_le64_to_cpu(val) (val)
#define sys_cpu_to_le64(val) (val)
#define sys_be16_to_cpu(val) BSWAP_16(val)
#define sys_cpu_to_be16(val) BSWAP_16(val)
#define sys_be24_to_cpu(val) BSWAP_24(val)
#define sys_cpu_to_be24(val) BSWAP_24(val)
#define sys_be32_to_cpu(val) BSWAP_32(val)
#define sys_cpu_to_be32(val) BSWAP_32(val)
#define sys_be40_to_cpu(val) BSWAP_40(val)
#define sys_cpu_to_be40(val) BSWAP_40(val)
#define sys_be48_to_cpu(val) BSWAP_48(val)
#define sys_cpu_to_be48(val) BSWAP_48(val)
#define sys_be64_to_cpu(val) BSWAP_64(val)
#define sys_cpu_to_be64(val) BSWAP_64(val)

#define sys_uint16_to_array(val) {      \
    ((val) & 0xff),             \
    (((val) >> 8) & 0xff)}

#define sys_uint32_to_array(val) {      \
    ((val) & 0xff),             \
    (((val) >> 8) & 0xff),          \
    (((val) >> 16) & 0xff),         \
    (((val) >> 24) & 0xff)}

#define sys_uint64_to_array(val) {      \
    ((val) & 0xff),             \
    (((val) >> 8) & 0xff),          \
    (((val) >> 16) & 0xff),         \
    (((val) >> 24) & 0xff),         \
    (((val) >> 32) & 0xff),         \
    (((val) >> 40) & 0xff),         \
    (((val) >> 48) & 0xff),         \
    (((val) >> 56) & 0xff)}

#else
#define sys_le16_to_cpu(val) BSWAP_16(val)
#define sys_cpu_to_le16(val) BSWAP_16(val)
#define sys_le24_to_cpu(val) BSWAP_24(val)
#define sys_cpu_to_le24(val) BSWAP_24(val)
#define sys_le32_to_cpu(val) BSWAP_32(val)
#define sys_cpu_to_le32(val) BSWAP_32(val)
#define sys_le40_to_cpu(val) BSWAP_40(val)
#define sys_cpu_to_le40(val) BSWAP_40(val)
#define sys_le48_to_cpu(val) BSWAP_48(val)
#define sys_cpu_to_le48(val) BSWAP_48(val)
#define sys_le64_to_cpu(val) BSWAP_64(val)
#define sys_cpu_to_le64(val) BSWAP_64(val)
#define sys_be16_to_cpu(val) (val)
#define sys_cpu_to_be16(val) (val)
#define sys_be24_to_cpu(val) (val)
#define sys_cpu_to_be24(val) (val)
#define sys_be32_to_cpu(val) (val)
#define sys_cpu_to_be32(val) (val)
#define sys_be40_to_cpu(val) (val)
#define sys_cpu_to_be40(val) (val)
#define sys_be48_to_cpu(val) (val)
#define sys_cpu_to_be48(val) (val)
#define sys_be64_to_cpu(val) (val)
#define sys_cpu_to_be64(val) (val)

#define sys_uint16_to_array(val) {      \
    (((val) >> 8) & 0xff),          \
    ((val) & 0xff)}

#define sys_uint32_to_array(val) {      \
    (((val) >> 24) & 0xff),         \
    (((val) >> 16) & 0xff),         \
    (((val) >> 8) & 0xff),          \
    ((val) & 0xff)}

#define sys_uint64_to_array(val) {      \
    (((val) >> 56) & 0xff),         \
    (((val) >> 48) & 0xff),         \
    (((val) >> 40) & 0xff),         \
    (((val) >> 32) & 0xff),         \
    (((val) >> 24) & 0xff),         \
    (((val) >> 16) & 0xff),         \
    (((val) >> 8) & 0xff),          \
    ((val) & 0xff)}

#endif

/**
 *  @brief Put a 16-bit integer as big-endian to arbitrary location.
 *
 *  Put a 16-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in big-endian format.
 *
 *  @param val 16-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_be16(uint16_t val, uint8_t dst[2])
{
    dst[0] = val >> 8;
    dst[1] = val;
}

/**
 *  @brief Put a 24-bit integer as big-endian to arbitrary location.
 *
 *  Put a 24-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in big-endian format.
 *
 *  @param val 24-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_be24(uint32_t val, uint8_t dst[3])
{
    dst[0] = val >> 16;
    sys_put_be16(val, &dst[1]);
}

/**
 *  @brief Put a 32-bit integer as big-endian to arbitrary location.
 *
 *  Put a 32-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in big-endian format.
 *
 *  @param val 32-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_be32(uint32_t val, uint8_t dst[4])
{
    sys_put_be16(val >> 16, dst);
    sys_put_be16(val, &dst[2]);
}
/**
 *  @brief Put a 40-bit integer as big-endian to arbitrary location.
 *
 *  Put a 40-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in big-endian format.
 *
 *  @param val 40-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_be40(uint64_t val, uint8_t dst[5])
{
    dst[0] = val >> 32;
    sys_put_be32(val, &dst[1]);
}

/**
 *  @brief Put a 48-bit integer as big-endian to arbitrary location.
 *
 *  Put a 48-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in big-endian format.
 *
 *  @param val 48-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_be48(uint64_t val, uint8_t dst[6])
{
    sys_put_be16(val >> 32, dst);
    sys_put_be32(val, &dst[2]);
}

/**
 *  @brief Put a 64-bit integer as big-endian to arbitrary location.
 *
 *  Put a 64-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in big-endian format.
 *
 *  @param val 64-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_be64(uint64_t val, uint8_t dst[8])
{
    sys_put_be32(val >> 32, dst);
    sys_put_be32(val, &dst[4]);
}

/**
 *  @brief Put a 16-bit integer as little-endian to arbitrary location.
 *
 *  Put a 16-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in little-endian format.
 *
 *  @param val 16-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_le16(uint16_t val, uint8_t dst[2])
{
    dst[0] = val;
    dst[1] = val >> 8;
}

/**
 *  @brief Put a 24-bit integer as little-endian to arbitrary location.
 *
 *  Put a 24-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in little-endian format.
 *
 *  @param val 24-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_le24(uint32_t val, uint8_t dst[3])
{
    sys_put_le16(val, dst);
    dst[2] = val >> 16;
}

/**
 *  @brief Put a 32-bit integer as little-endian to arbitrary location.
 *
 *  Put a 32-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in little-endian format.
 *
 *  @param val 32-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_le32(uint32_t val, uint8_t dst[4])
{
    sys_put_le16(val, dst);
    sys_put_le16(val >> 16, &dst[2]);
}

/**
 *  @brief Put a 40-bit integer as little-endian to arbitrary location.
 *
 *  Put a 40-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in little-endian format.
 *
 *  @param val 40-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_le40(uint64_t val, uint8_t dst[5])
{
    sys_put_le32(val, dst);
    dst[4] = val >> 32;
}

/**
 *  @brief Put a 48-bit integer as little-endian to arbitrary location.
 *
 *  Put a 48-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in little-endian format.
 *
 *  @param val 48-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_le48(uint64_t val, uint8_t dst[6])
{
    sys_put_le32(val, dst);
    sys_put_le16(val >> 32, &dst[4]);
}

/**
 *  @brief Put a 64-bit integer as little-endian to arbitrary location.
 *
 *  Put a 64-bit integer, originally in host endianness, to a
 *  potentially unaligned memory location in little-endian format.
 *
 *  @param val 64-bit integer in host endianness.
 *  @param dst Destination memory address to store the result.
 */
static inline void sys_put_le64(uint64_t val, uint8_t dst[8])
{
    sys_put_le32(val, dst);
    sys_put_le32(val >> 32, &dst[4]);
}

/**
 *  @brief Get a 16-bit integer stored in big-endian format.
 *
 *  Get a 16-bit integer, stored in big-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the big-endian 16-bit integer to get.
 *
 *  @return 16-bit integer in host endianness.
 */
static inline uint16_t sys_get_be16(const uint8_t src[2])
{
    return ((uint16_t)src[0] << 8) | src[1];
}

/**
 *  @brief Get a 24-bit integer stored in big-endian format.
 *
 *  Get a 24-bit integer, stored in big-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the big-endian 24-bit integer to get.
 *
 *  @return 24-bit integer in host endianness.
 */
static inline uint32_t sys_get_be24(const uint8_t src[3])
{
    return ((uint32_t)src[0] << 16) | sys_get_be16(&src[1]);
}

/**
 *  @brief Get a 32-bit integer stored in big-endian format.
 *
 *  Get a 32-bit integer, stored in big-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the big-endian 32-bit integer to get.
 *
 *  @return 32-bit integer in host endianness.
 */
static inline uint32_t sys_get_be32(const uint8_t src[4])
{
    return ((uint32_t)sys_get_be16(&src[0]) << 16) | sys_get_be16(&src[2]);
}

/**
 *  @brief Get a 40-bit integer stored in big-endian format.
 *
 *  Get a 40-bit integer, stored in big-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the big-endian 40-bit integer to get.
 *
 *  @return 40-bit integer in host endianness.
 */
static inline uint64_t sys_get_be40(const uint8_t src[5])
{
    return ((uint64_t)sys_get_be32(&src[0]) << 8) | src[4];
}

/**
 *  @brief Get a 48-bit integer stored in big-endian format.
 *
 *  Get a 48-bit integer, stored in big-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the big-endian 48-bit integer to get.
 *
 *  @return 48-bit integer in host endianness.
 */
static inline uint64_t sys_get_be48(const uint8_t src[6])
{
    return ((uint64_t)sys_get_be32(&src[0]) << 16) | sys_get_be16(&src[4]);
}

/**
 *  @brief Get a 64-bit integer stored in big-endian format.
 *
 *  Get a 64-bit integer, stored in big-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the big-endian 64-bit integer to get.
 *
 *  @return 64-bit integer in host endianness.
 */
static inline uint64_t sys_get_be64(const uint8_t src[8])
{
    return ((uint64_t)sys_get_be32(&src[0]) << 32) | sys_get_be32(&src[4]);
}

/**
 *  @brief Get a 16-bit integer stored in little-endian format.
 *
 *  Get a 16-bit integer, stored in little-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the little-endian 16-bit integer to get.
 *
 *  @return 16-bit integer in host endianness.
 */
static inline uint16_t sys_get_le16(const uint8_t src[2])
{
    return ((uint16_t)src[1] << 8) | src[0];
}

/**
 *  @brief Get a 24-bit integer stored in little-endian format.
 *
 *  Get a 24-bit integer, stored in little-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the little-endian 24-bit integer to get.
 *
 *  @return 24-bit integer in host endianness.
 */
static inline uint32_t sys_get_le24(const uint8_t src[3])
{
    return ((uint32_t)src[2] << 16) | sys_get_le16(&src[0]);
}

/**
 *  @brief Get a 32-bit integer stored in little-endian format.
 *
 *  Get a 32-bit integer, stored in little-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the little-endian 32-bit integer to get.
 *
 *  @return 32-bit integer in host endianness.
 */
static inline uint32_t sys_get_le32(const uint8_t src[4])
{
    return ((uint32_t)sys_get_le16(&src[2]) << 16) | sys_get_le16(&src[0]);
}

/**
 *  @brief Get a 40-bit integer stored in little-endian format.
 *
 *  Get a 40-bit integer, stored in little-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the little-endian 40-bit integer to get.
 *
 *  @return 40-bit integer in host endianness.
 */
static inline uint64_t sys_get_le40(const uint8_t src[5])
{
    return ((uint64_t)sys_get_le32(&src[1]) << 8) | src[0];
}

/**
 *  @brief Get a 48-bit integer stored in little-endian format.
 *
 *  Get a 48-bit integer, stored in little-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the little-endian 48-bit integer to get.
 *
 *  @return 48-bit integer in host endianness.
 */
static inline uint64_t sys_get_le48(const uint8_t src[6])
{
    return ((uint64_t)sys_get_le32(&src[2]) << 16) | sys_get_le16(&src[0]);
}

/**
 *  @brief Get a 64-bit integer stored in little-endian format.
 *
 *  Get a 64-bit integer, stored in little-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the little-endian 64-bit integer to get.
 *
 *  @return 64-bit integer in host endianness.
 */
static inline uint64_t sys_get_le64(const uint8_t src[8])
{
    return ((uint64_t)sys_get_le32(&src[4]) << 32) | sys_get_le32(&src[0]);
}

/**
 * @brief Swap one buffer content into another
 *
 * Copy the content of src buffer into dst buffer in reversed order,
 * i.e.: src[n] will be put in dst[end-n]
 * Where n is an index and 'end' the last index in both arrays.
 * The 2 memory pointers must be pointing to different areas, and have
 * a minimum size of given length.
 *
 * @param dst A valid pointer on a memory area where to copy the data in
 * @param src A valid pointer on a memory area where to copy the data from
 * @param length Size of both dst and src memory areas
 */
static inline void sys_memcpy_swap(void *dst, const void *src, size_t length)
{
    uint8_t *pdst = (uint8_t *)dst;
    const uint8_t *psrc = (const uint8_t *)src;

    __ASSERT(((psrc < pdst && (psrc + length) <= pdst) ||
              (psrc > pdst && (pdst + length) <= psrc)),
             "Source and destination buffers must not overlap");

    psrc += length - 1;

    for (; length > 0; length--) {
        *pdst++ = *psrc--;
    }
}

/**
 * @brief Swap buffer content
 *
 * In-place memory swap, where final content will be reversed.
 * I.e.: buf[n] will be put in buf[end-n]
 * Where n is an index and 'end' the last index of buf.
 *
 * @param buf A valid pointer on a memory area to swap
 * @param length Size of buf memory area
 */
static inline void sys_mem_swap(void *buf, size_t length)
{
    size_t i;

    for (i = 0; i < (length / 2); i++) {
        uint8_t tmp = ((uint8_t *)buf)[i];

        ((uint8_t *)buf)[i] = ((uint8_t *)buf)[length - 1 - i];
        ((uint8_t *)buf)[length - 1 - i] = tmp;
    }
}

/**
 *  @brief Convert buffer from little-endian to host endianness.
 *
 * @param buf A valid pointer on a memory area to convert from little-endian to host endianness.
 * @param length Size of buf memory area
 */
static inline void sys_le_to_cpu(void *buf, size_t length)
{
    if (IS_ENABLED(CONFIG_BIG_ENDIAN)) {
        sys_mem_swap(buf, length);
    }
}

/**
 *  @brief Convert buffer from host endianness to little-endian.
 *
 * @param buf A valid pointer on a memory area to convert from host endianness to little-endian.
 * @param length Size of buf memory area
 */
static inline void sys_cpu_to_le(void *buf, size_t length)
{
    if (IS_ENABLED(CONFIG_BIG_ENDIAN)) {
        sys_mem_swap(buf, length);
    }
}

/**
 *  @brief Convert buffer from big-endian to host endianness.
 *
 * @param buf A valid pointer on a memory area to convert from big-endian to host endianness.
 * @param length Size of buf memory area
 */
static inline void sys_be_to_cpu(void *buf, size_t length)
{
    if (IS_ENABLED(CONFIG_LITTLE_ENDIAN)) {
        sys_mem_swap(buf, length);
    }
}

/**
 *  @brief Convert buffer from host endianness to big-endian.
 *
 * @param buf A valid pointer on a memory area to convert from host endianness to big-endian.
 * @param length Size of buf memory area
 */
static inline void sys_cpu_to_be(void *buf, size_t length)
{
    if (IS_ENABLED(CONFIG_LITTLE_ENDIAN)) {
        sys_mem_swap(buf, length);
    }
}

/**
 *  @brief Put a buffer as little-endian to arbitrary location.
 *
 *  Put a buffer originally in host endianness, to a
 *  potentially unaligned memory location in little-endian format.
 *
 * @param dst A valid pointer on a memory area where to copy the data in
 * @param src A valid pointer on a memory area where to copy the data from
 * @param length Size of both dst and src memory areas
 */
static inline void sys_put_le(void *dst, const void *src, size_t length)
{
    if (IS_ENABLED(CONFIG_LITTLE_ENDIAN)) {
        (void)memcpy(dst, src, length);
    } else {
        sys_memcpy_swap(dst, src, length);
    }
}

/**
 *  @brief Put a buffer as big-endian to arbitrary location.
 *
 *  Put a buffer originally in host endianness, to a
 *  potentially unaligned memory location in big-endian format.
 *
 * @param dst A valid pointer on a memory area where to copy the data in
 * @param src A valid pointer on a memory area where to copy the data from
 * @param length Size of both dst and src memory areas
 */
static inline void sys_put_be(void *dst, const void *src, size_t length)
{
    if (IS_ENABLED(CONFIG_LITTLE_ENDIAN)) {
        sys_memcpy_swap(dst, src, length);
    } else {
        (void)memcpy(dst, src, length);
    }
}

/**
 *  @brief Get a buffer stored in little-endian format.
 *
 *  Get a buffer, stored in little-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 * @param dst A valid pointer on a memory area where to copy the data in
 * @param src A valid pointer on a memory area where to copy the data from
 * @param length Size of both dst and src memory areas
 */
static inline void sys_get_le(void *dst, const void *src, size_t length)
{
    if (IS_ENABLED(CONFIG_LITTLE_ENDIAN)) {
        (void)memcpy(dst, src, length);
    } else {
        sys_memcpy_swap(dst, src, length);
    }
}

/**
 *  @brief Get a buffer stored in big-endian format.
 *
 *  Get a buffer, stored in big-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 * @param dst A valid pointer on a memory area where to copy the data in
 * @param src A valid pointer on a memory area where to copy the data from
 * @param length Size of both dst and src memory areas
 */
static inline void sys_get_be(void *dst, const void *src, size_t length)
{
    if (IS_ENABLED(CONFIG_LITTLE_ENDIAN)) {
        sys_memcpy_swap(dst, src, length);
    } else {
        (void)memcpy(dst, src, length);
    }
}

#endif /* ZEPHYR_INCLUDE_SYS_BYTEORDER_H_ */
