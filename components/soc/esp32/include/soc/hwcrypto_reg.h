/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __HWCRYPTO_REG_H__
#define __HWCRYPTO_REG_H__

#include "soc/soc.h"

/* registers for RSA acceleration via Multiple Precision Integer ops */
#define RSA_MEM_M_BLOCK_BASE          ((DR_REG_RSA_BASE)+0x000)
/* RB & Z use the same memory block, depending on phase of operation */
#define RSA_MEM_RB_BLOCK_BASE         ((DR_REG_RSA_BASE)+0x200)
#define RSA_MEM_Z_BLOCK_BASE          ((DR_REG_RSA_BASE)+0x200)
#define RSA_MEM_Y_BLOCK_BASE          ((DR_REG_RSA_BASE)+0x400)
#define RSA_MEM_X_BLOCK_BASE          ((DR_REG_RSA_BASE)+0x600)

#define RSA_M_DASH_REG                (DR_REG_RSA_BASE + 0x800)
#define RSA_MODEXP_MODE_REG           (DR_REG_RSA_BASE + 0x804)
#define RSA_MODEXP_START_REG          (DR_REG_RSA_BASE + 0x808)
#define RSA_MULT_MODE_REG             (DR_REG_RSA_BASE + 0x80c)
#define RSA_MULT_START_REG            (DR_REG_RSA_BASE + 0x810)

#define RSA_CLEAR_INTERRUPT_REG       (DR_REG_RSA_BASE + 0x814)
#define RSA_QUERY_INTERRUPT_REG       (DR_REG_RSA_BASE + 0x814) /* same */

#define RSA_QUERY_CLEAN_REG           (DR_REG_RSA_BASE + 0x818)

/* Backwards compatibility register names used pre-ESP32S2 */
#define RSA_CLEAN_REG                 (RSA_QUERY_CLEAN_REG)
#define RSA_INTERRUPT_REG             (RSA_CLEAR_INTERRUPT_REG)
#define RSA_START_MODEXP_REG          (RSA_MODEXP_START_REG)

/* SHA acceleration registers */
#define SHA_TEXT_BASE           ((DR_REG_SHA_BASE) + 0x00)

#define SHA_1_START_REG         ((DR_REG_SHA_BASE) + 0x80)
#define SHA_1_CONTINUE_REG      ((DR_REG_SHA_BASE) + 0x84)
#define SHA_1_LOAD_REG          ((DR_REG_SHA_BASE) + 0x88)
#define SHA_1_BUSY_REG          ((DR_REG_SHA_BASE) + 0x8c)

#define SHA_256_START_REG       ((DR_REG_SHA_BASE) + 0x90)
#define SHA_256_CONTINUE_REG    ((DR_REG_SHA_BASE) + 0x94)
#define SHA_256_LOAD_REG        ((DR_REG_SHA_BASE) + 0x98)
#define SHA_256_BUSY_REG        ((DR_REG_SHA_BASE) + 0x9c)

#define SHA_384_START_REG       ((DR_REG_SHA_BASE) + 0xa0)
#define SHA_384_CONTINUE_REG    ((DR_REG_SHA_BASE) + 0xa4)
#define SHA_384_LOAD_REG        ((DR_REG_SHA_BASE) + 0xa8)
#define SHA_384_BUSY_REG        ((DR_REG_SHA_BASE) + 0xac)

#define SHA_512_START_REG       ((DR_REG_SHA_BASE) + 0xb0)
#define SHA_512_CONTINUE_REG    ((DR_REG_SHA_BASE) + 0xb4)
#define SHA_512_LOAD_REG        ((DR_REG_SHA_BASE) + 0xb8)
#define SHA_512_BUSY_REG        ((DR_REG_SHA_BASE) + 0xbc)

/* AES acceleration registers */
#define AES_START_REG           ((DR_REG_AES_BASE) + 0x00)
#define AES_IDLE_REG            ((DR_REG_AES_BASE) + 0x04)
#define AES_MODE_REG            ((DR_REG_AES_BASE) + 0x08)
#define AES_KEY_BASE            ((DR_REG_AES_BASE) + 0x10)
#define AES_TEXT_BASE           ((DR_REG_AES_BASE) + 0x30)
#define AES_ENDIAN              ((DR_REG_AES_BASE) + 0x40)

#endif
