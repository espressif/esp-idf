/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BTDM_ENDIAN_H_
#define _BTDM_ENDIAN_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void
btdm_put_le16(void *buf, uint16_t x)
{
    extern void r_btdm_put_le16(void *buf, uint16_t x);
    r_btdm_put_le16(buf, x);
}

static inline void
btdm_put_le24(void *buf, uint32_t x)
{
    extern void r_btdm_put_le24(void *buf, uint32_t x);
    r_btdm_put_le24(buf, x);
}

static inline void
btdm_put_le32(void *buf, uint32_t x)
{
    extern void r_btdm_put_le32(void *buf, uint32_t x);
    r_btdm_put_le32(buf, x);
}

static inline void
btdm_put_le64(void *buf, uint64_t x)
{
    extern void r_btdm_put_le64(void *buf, uint64_t x);
    r_btdm_put_le64(buf, x);
}

static inline uint16_t
btdm_get_le16(const void *buf)
{
    extern uint16_t r_btdm_get_le16(const void *buf);
    return r_btdm_get_le16(buf);
}

static inline uint32_t
btdm_get_le24(const void *buf)
{
    extern uint32_t r_btdm_get_le24(const void *buf);
    return r_btdm_get_le24(buf);
}

static inline uint32_t
btdm_get_le32(const void *buf)
{
    extern uint32_t r_btdm_get_le32(const void *buf);
    return r_btdm_get_le32(buf);
}

static inline uint64_t
btdm_get_le64(const void *buf)
{
    extern uint64_t r_btdm_get_le64(const void *buf);
    return r_btdm_get_le64(buf);
}

static inline void
btdm_put_be16(void *buf, uint16_t x)
{
    extern void r_btdm_put_be16(void *buf, uint16_t x);
    r_btdm_put_be16(buf, x);
}

static inline void
btdm_put_be24(void *buf, uint32_t x)
{
    extern void r_btdm_put_be24(void *buf, uint32_t x);
    r_btdm_put_be24(buf, x);
}

static inline void
btdm_put_be32(void *buf, uint32_t x)
{
    extern void r_btdm_put_be32(void *buf, uint32_t x);
    r_btdm_put_be32(buf, x);
}

static inline void
btdm_put_be64(void *buf, uint64_t x)
{
    extern void r_btdm_put_be64(void *buf, uint64_t x);
    r_btdm_put_be64(buf, x);
}

static inline uint16_t
btdm_get_be16(const void *buf)
{
    extern uint16_t r_btdm_get_be16(const void *buf);
    return r_btdm_get_be16(buf);
}

static inline uint32_t
btdm_get_be24(const void *buf)
{
    extern uint32_t r_btdm_get_be24(const void *buf);
    return r_btdm_get_be24(buf);
}

static inline uint32_t
btdm_get_be32(const void *buf)
{
    extern uint32_t r_btdm_get_be32(const void *buf);
    return r_btdm_get_be32(buf);
}

static inline uint64_t
btdm_get_be64(const void *buf)
{
    extern uint64_t r_btdm_get_be64(const void *buf);
    return r_btdm_get_be64(buf);
}

static inline void
btdm_swap_in_place(void *buf, int len)
{
    extern void r_btdm_swap_in_place(void *buf, int len);
    r_btdm_swap_in_place(buf, len);
}

static inline void
btdm_swap_buf(uint8_t *dst, const uint8_t *src, int len)
{
    extern void r_btdm_swap_buf(uint8_t *dst, const uint8_t *src, int len);
    r_btdm_swap_buf(dst, src, len);
}

#ifdef __cplusplus
}
#endif

#endif /* _BTDM_ENDIAN_H_ */
