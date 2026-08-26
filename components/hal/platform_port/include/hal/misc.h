/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_SWAP16(d) __builtin_bswap16((d))
#define HAL_SWAP32(d) __builtin_bswap32((d))
#define HAL_SWAP64(d) __builtin_bswap64((d))

/** @cond */    //Doxy command to hide preprocessor definitions from docs */

/**
 * @brief Macro to force a 32-bit read, modify, then write on a peripheral register
 *
 * Due to a GCC bug, the compiler may still try to optimize read/writes to peripheral register fields by using 8/16 bit
 * access, even if they are marked volatile (i.e., -fstrict-volatile-bitfields has no effect).
 *
 * For ESP chips, the peripheral bus only allows 32-bit read/writes. The following macro works around the compiler issue
 * by forcing a 32-bit read/modify/write.
 *
 * @note This macro should only be called on register fields of xxx_struct.h type headers, as it depends on the presence
 *       of a 'val' field of the register union.
 * @note Current implementation reads into a uint32_t instead of copy base_reg direclty to temp_reg. The reason being
 *       that C++ does not create a copy constructor for volatile structs.
 */
#define HAL_FORCE_MODIFY_U32_REG_FIELD(base_reg, reg_field, field_val)    \
{                                                           \
    uint32_t temp_val = base_reg.val;                       \
    typeof(base_reg) temp_reg;                              \
    temp_reg.val = temp_val;                                \
    temp_reg.reg_field = (field_val);                       \
    (base_reg).val = temp_reg.val;                          \
}

/**
 * @brief Macro to force a 32-bit read on a peripheral register
 *
 * @note This macro should only be called on register fields of xxx_struct.h type headers. See description above for
 *       more details.
 * @note Current implementation reads into a uint32_t. See description above for more details.
 */
#define HAL_FORCE_READ_U32_REG_FIELD(base_reg, reg_field) ({    \
    uint32_t temp_val = base_reg.val;                       \
    typeof(base_reg) temp_reg;                              \
    temp_reg.val = temp_val;                                \
    temp_reg.reg_field;                                     \
})

/** @endcond */

/**
 * @brief Copy data from memory array to another memory
 *
 * This helps bypass the -Warray-bounds, -Wstringop-overread and -Wstringop-overflow bugs.
 *
 * @param dst_mem Pointer to the destination of data
 * @param src_mem Pointer to the source of data to be copied
 * @param len The number of bytes to be copied
 * @return a pointer to destination
 */
#define hal_memcpy(dst_mem, src_mem, len) (__extension__({memcpy(dst_mem, src_mem, len);}))

/**
 * @brief Sets the first num bytes of the block of memory pointed by ptr to the specified value
 *
 * This helps bypass the -Warray-bounds, -Wstringop-overread and -Wstringop-overflow bugs.
 *
 * @param dst_reg Pointer to the block of memory to fill
 * @param value The value to be set.
 * @param len The number of bytes to be copied
 * @return a pointer to the memory area
 */
#define hal_memset(dst_mem, value, len) (__extension__({memset(dst_mem, value, len);}))

#ifdef __cplusplus
}
#endif
