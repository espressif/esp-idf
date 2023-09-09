/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_BYTEORDER_H_
#define _BLE_MESH_BYTEORDER_H_

#include "mesh/types.h"
#include "mesh/trace.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Internal helpers only used by the sys_* APIs further below */
#ifndef __bswap_16
#define __bswap_16(x) ((uint16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))
#endif

#ifndef __bswap_24
#define __bswap_24(x) ((uint32_t) ((((x) >> 16) & 0xff) | \
                                   (((x)) & 0xff00) | \
                                   (((x) & 0xff) << 16)))
#endif

#ifndef __bswap_32
#define __bswap_32(x) ((uint32_t) ((((x) >> 24) & 0xff) | \
                                   (((x) >> 8) & 0xff00) | \
                                   (((x) & 0xff00) << 8) | \
                                   (((x) & 0xff) << 24)))
#endif

#ifndef __bswap_48
#define __bswap_48(x) ((uint64_t) ((((x) >> 40) & 0xff) | \
                                   (((x) >> 24) & 0xff00) | \
                                   (((x) >> 8) & 0xff0000) | \
                                   (((x) & 0xff0000) << 8) | \
                                   (((x) & 0xff00) << 24) | \
                                   (((x) & 0xff) << 40)))
#endif

#ifndef __bswap_64
#define __bswap_64(x) ((uint64_t) ((((x) >> 56) & 0xff) | \
                                   (((x) >> 40) & 0xff00) | \
                                   (((x) >> 24) & 0xff0000) | \
                                   (((x) >> 8) & 0xff000000) | \
                                   (((x) & 0xff000000) << 8) | \
                                   (((x) & 0xff0000) << 24) | \
                                   (((x) & 0xff00) << 40) | \
                                   (((x) & 0xff) << 56)))
#endif

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

#ifndef sys_le16_to_cpu
#define sys_le16_to_cpu(val) (val)
#endif
#ifndef sys_cpu_to_le16
#define sys_cpu_to_le16(val) (val)
#endif
#ifndef sys_le24_to_cpu
#define sys_le24_to_cpu(val) (val)
#endif
#ifndef sys_cpu_to_le24
#define sys_cpu_to_le24(val) (val)
#endif
#ifndef sys_le32_to_cpu
#define sys_le32_to_cpu(val) (val)
#endif
#ifndef sys_cpu_to_le32
#define sys_cpu_to_le32(val) (val)
#endif
#ifndef sys_le48_to_cpu
#define sys_le48_to_cpu(val) (val)
#endif
#ifndef sys_cpu_to_le48
#define sys_cpu_to_le48(val) (val)
#endif
#ifndef sys_le64_to_cpu
#define sys_le64_to_cpu(val) (val)
#endif
#ifndef sys_cpu_to_le64
#define sys_cpu_to_le64(val) (val)
#endif
#ifndef sys_be16_to_cpu
#define sys_be16_to_cpu(val) __bswap_16(val)
#endif
#ifndef sys_cpu_to_be16
#define sys_cpu_to_be16(val) __bswap_16(val)
#endif
#ifndef sys_be24_to_cpu
#define sys_be24_to_cpu(val) __bswap_24(val)
#endif
#ifndef sys_cpu_to_be24
#define sys_cpu_to_be24(val) __bswap_24(val)
#endif
#ifndef sys_be32_to_cpu
#define sys_be32_to_cpu(val) __bswap_32(val)
#endif
#ifndef sys_cpu_to_be32
#define sys_cpu_to_be32(val) __bswap_32(val)
#endif
#ifndef sys_be48_to_cpu
#define sys_be48_to_cpu(val) __bswap_48(val)
#endif
#ifndef sys_cpu_to_be48
#define sys_cpu_to_be48(val) __bswap_48(val)
#endif
#ifndef sys_be64_to_cpu
#define sys_be64_to_cpu(val) __bswap_64(val)
#endif
#ifndef sys_cpu_to_be64
#define sys_cpu_to_be64(val) __bswap_64(val)
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
    return ((uint64_t)sys_get_be32(&src[0]) << 32) | sys_get_be16(&src[4]);
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
 *  @brief Get a 24-bit integer stored in big-endian format.
 *
 *  Get a 24-bit integer, stored in big-endian format in a potentially
 *  unaligned memory location, and convert it to the host endianness.
 *
 *  @param src Location of the big-endian 24-bit integer to get.
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
    return ((uint64_t)sys_get_le32(&src[2]) << 32) | sys_get_le16(&src[0]);
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

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_BYTEORDER_H_ */
