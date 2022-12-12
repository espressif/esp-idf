/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __HWCRYPTO_REG_H__
#define __HWCRYPTO_REG_H__

#include "soc.h"

/* registers for RSA acceleration via Multiple Precision Integer ops */
#define RSA_MEM_M_BLOCK_BASE          ((DR_REG_RSA_BASE)+0x000)
/* RB & Z use the same memory block, depending on phase of operation */
#define RSA_MEM_RB_BLOCK_BASE         ((DR_REG_RSA_BASE)+0x200)
#define RSA_MEM_Z_BLOCK_BASE          ((DR_REG_RSA_BASE)+0x200)
#define RSA_MEM_Y_BLOCK_BASE          ((DR_REG_RSA_BASE)+0x400)
#define RSA_MEM_X_BLOCK_BASE          ((DR_REG_RSA_BASE)+0x600)

/* Configuration registers */
#define RSA_M_DASH_REG                (DR_REG_RSA_BASE + 0x800)
#define RSA_LENGTH_REG                (DR_REG_RSA_BASE + 0x804)
#define RSA_CONSTANT_TIME_REG         (DR_REG_RSA_BASE + 0x820)
#define RSA_SEARCH_OPEN_REG           (DR_REG_RSA_BASE + 0x824)
#define RSA_SEARCH_POS_REG            (DR_REG_RSA_BASE + 0x828)

/* Initialization registers */
#define RSA_QUERY_CLEAN_REG           (DR_REG_RSA_BASE + 0x808)

/* Calculation start registers */
#define RSA_MODEXP_START_REG          (DR_REG_RSA_BASE + 0x80c)
#define RSA_MOD_MULT_START_REG        (DR_REG_RSA_BASE + 0x810)
#define RSA_MULT_START_REG            (DR_REG_RSA_BASE + 0x814)

/* Interrupt registers */
#define RSA_QUERY_INTERRUPT_REG       (DR_REG_RSA_BASE + 0x818)
#define RSA_CLEAR_INTERRUPT_REG       (DR_REG_RSA_BASE + 0x81C)
#define RSA_INTERRUPT_REG             (DR_REG_RSA_BASE + 0x82C)

#define SHA_MODE_SHA1       0
#define SHA_MODE_SHA224     1
#define SHA_MODE_SHA256     2
#define SHA_MODE_SHA384     3
#define SHA_MODE_SHA512     4
#define SHA_MODE_SHA512_224 5
#define SHA_MODE_SHA512_256 6
#define SHA_MODE_SHA512_T   7

/* SHA acceleration registers */
#define SHA_MODE_REG                  ((DR_REG_SHA_BASE) + 0x00)
#define SHA_T_STRING_REG              ((DR_REG_SHA_BASE) + 0x04)
#define SHA_T_LENGTH_REG              ((DR_REG_SHA_BASE) + 0x08)
#define SHA_BLOCK_NUM_REG             ((DR_REG_SHA_BASE) + 0x0C)
#define SHA_START_REG                 ((DR_REG_SHA_BASE) + 0x10)
#define SHA_CONTINUE_REG              ((DR_REG_SHA_BASE) + 0x14)
#define SHA_BUSY_REG                  ((DR_REG_SHA_BASE) + 0x18)
#define SHA_DMA_START_REG             ((DR_REG_SHA_BASE) + 0x1C)
#define SHA_DMA_CONTINUE_REG          ((DR_REG_SHA_BASE) + 0x20)
#define SHA_CLEAR_IRQ_REG             ((DR_REG_SHA_BASE) + 0x24)
#define SHA_INT_ENA_REG               ((DR_REG_SHA_BASE) + 0x28)

#define SHA_H_BASE                    ((DR_REG_SHA_BASE) + 0x40)
#define SHA_TEXT_BASE                 ((DR_REG_SHA_BASE) + 0x80)

/* AES acceleration registers */
#define AES_MODE_REG            ((DR_REG_AES_BASE) + 0x40)
#define AES_ENDIAN_REG          ((DR_REG_AES_BASE) + 0x44)
#define AES_TRIGGER_REG         ((DR_REG_AES_BASE) + 0x48)
#define AES_STATE_REG           ((DR_REG_AES_BASE) + 0x4c)
#define AES_DMA_ENABLE_REG      ((DR_REG_AES_BASE) + 0x90)
#define AES_BLOCK_MODE_REG      ((DR_REG_AES_BASE) + 0x94)
#define AES_BLOCK_NUM_REG       ((DR_REG_AES_BASE) + 0x98)
#define AES_INC_SEL_REG         ((DR_REG_AES_BASE) + 0x9C)
#define AES_AAD_BLOCK_NUM_REG   ((DR_REG_AES_BASE) + 0xA0)
#define AES_BIT_VALID_NUM_REG   ((DR_REG_AES_BASE) + 0xA4)
#define AES_CONTINUE_REG        ((DR_REG_AES_BASE) + 0xA8)
#define AES_INT_CLEAR_REG       ((DR_REG_AES_BASE) + 0xAC)
#define AES_INT_ENA_REG         ((DR_REG_AES_BASE) + 0xB0)
#define AES_DATE_REG            ((DR_REG_AES_BASE) + 0xB4)
#define AES_DMA_EXIT_REG        ((DR_REG_AES_BASE) + 0xB8)

#define AES_DMA_ENABLE_REG      ((DR_REG_AES_BASE) + 0x90)
#define AES_BLOCK_MODE_REG      ((DR_REG_AES_BASE) + 0x94)
#define AES_BLOCK_NUM_REG       ((DR_REG_AES_BASE) + 0x98)
#define AES_INC_SEL_REG         ((DR_REG_AES_BASE) + 0x9C)
#define AES_AAD_BLOCK_NUM_REG   ((DR_REG_AES_BASE) + 0xA0)
#define AES_BIT_VALID_NUM_REG   ((DR_REG_AES_BASE) + 0xA4)
#define AES_CONTINUE_REG        ((DR_REG_AES_BASE) + 0xA8)

#define AES_KEY_BASE            ((DR_REG_AES_BASE) + 0x00)
#define AES_TEXT_IN_BASE        ((DR_REG_AES_BASE) + 0x20)
#define AES_TEXT_OUT_BASE       ((DR_REG_AES_BASE) + 0x30)
#define AES_IV_BASE             ((DR_REG_AES_BASE) + 0x50)
#define AES_H_BASE              ((DR_REG_AES_BASE) + 0x60)
#define AES_J_BASE              ((DR_REG_AES_BASE) + 0x70)
#define AES_T_BASE              ((DR_REG_AES_BASE) + 0x80)

/* AES_STATE_REG values */
#define AES_STATE_IDLE 0
#define AES_STATE_BUSY 1
#define AES_STATE_DONE 2


/* Crypto DMA */
#define CRYPTO_DMA_CONF0_REG             ((DR_REG_CRYPTO_DMA_BASE) + 0x00)
#define CRYPTO_DMA_INT_RAW_REG           ((DR_REG_CRYPTO_DMA_BASE) + 0x04)
#define CRYPTO_DMA_INT_ST_REG            ((DR_REG_CRYPTO_DMA_BASE) + 0x08)
#define CRYPTO_DMA_INT_ENA_REG           ((DR_REG_CRYPTO_DMA_BASE) + 0x0C)
#define CRYPTO_DMA_INT_CLR_REG           ((DR_REG_CRYPTO_DMA_BASE) + 0x10)
#define CRYPTO_DMA_OUT_STATUS_REG        ((DR_REG_CRYPTO_DMA_BASE) + 0x14)
#define CRYPTO_DMA_OUT_PUSH_REG          ((DR_REG_CRYPTO_DMA_BASE) + 0x18)
#define CRYPTO_DMA_IN_STATUS_REG         ((DR_REG_CRYPTO_DMA_BASE) + 0x1C)
#define CRYPTO_DMA_IN_POP_REG            ((DR_REG_CRYPTO_DMA_BASE) + 0x20)
#define CRYPTO_DMA_OUT_LINK_REG          ((DR_REG_CRYPTO_DMA_BASE) + 0x24)
#define CRYPTO_DMA_IN_LINK_REG           ((DR_REG_CRYPTO_DMA_BASE) + 0x28)
#define CRYPTO_DMA_CONF1_REG             ((DR_REG_CRYPTO_DMA_BASE) + 0x2C)
#define CRYPTO_DMA_STATE0_REG            ((DR_REG_CRYPTO_DMA_BASE) + 0x30)
#define CRYPTO_DMA_STATE1_REG            ((DR_REG_CRYPTO_DMA_BASE) + 0x34)
#define CRYPTO_DMA_OUT_EOF_DES_ADDR_REG  ((DR_REG_CRYPTO_DMA_BASE) + 0x38)
#define CRYPTO_DMA_IN_SUC_EOF_DES_ADDR   ((DR_REG_CRYPTO_DMA_BASE) + 0x3C)
#define CRYPTO_DMA_IN_ERR_EOF_DES_ADDR   ((DR_REG_CRYPTO_DMA_BASE) + 0x40)
#define CRYPTO_DMA_OUT_EOF_BFR_DES_ADDR  ((DR_REG_CRYPTO_DMA_BASE) + 0x44)
#define CRYPTO_DMA_AHB_TEST              ((DR_REG_CRYPTO_DMA_BASE) + 0x48)
#define CRYPTO_DMA_IN_DSCR_REG           ((DR_REG_CRYPTO_DMA_BASE) + 0x4C)
#define CRYPTO_DMA_IN_DSCR_BF0_REG       ((DR_REG_CRYPTO_DMA_BASE) + 0x50)
#define CRYPTO_DMA_IN_DSCR_BF1_REG       ((DR_REG_CRYPTO_DMA_BASE) + 0x54)
#define CRYPTO_DMA_OUT_DSCR_REG          ((DR_REG_CRYPTO_DMA_BASE) + 0x58)
#define CRYPTO_DMA_OUT_DSCR_BF0_REG      ((DR_REG_CRYPTO_DMA_BASE) + 0x5C)
#define CRYPTO_DMA_OUT_DSCR_BF1_REG      ((DR_REG_CRYPTO_DMA_BASE) + 0x60)
#define CRYPTO_DMA_AES_SHA_SELECT_REG    ((DR_REG_CRYPTO_DMA_BASE) + 0x64)
#define CRYPTO_DMA_DATE_REG              ((DR_REG_CRYPTO_DMA_BASE) + 0xFC)

/* HMAC Module */
#define HMAC_SET_START_REG             ((DR_REG_HMAC_BASE) + 0x40)
#define HMAC_SET_PARA_PURPOSE_REG      ((DR_REG_HMAC_BASE) + 0x44)
#define HMAC_SET_PARA_KEY_REG          ((DR_REG_HMAC_BASE) + 0x48)
#define HMAC_SET_PARA_FINISH_REG       ((DR_REG_HMAC_BASE) + 0x4c)
#define HMAC_SET_MESSAGE_ONE_REG       ((DR_REG_HMAC_BASE) + 0x50)
#define HMAC_SET_MESSAGE_ING_REG       ((DR_REG_HMAC_BASE) + 0x54)
#define HMAC_SET_MESSAGE_END_REG       ((DR_REG_HMAC_BASE) + 0x58)
#define HMAC_SET_RESULT_FINISH_REG     ((DR_REG_HMAC_BASE) + 0x5c)
#define HMAC_SET_INVALIDATE_JTAG_REG   ((DR_REG_HMAC_BASE) + 0x60)
#define HMAC_SET_INVALIDATE_DS_REG     ((DR_REG_HMAC_BASE) + 0x64)
#define HMAC_QUERY_ERROR_REG           ((DR_REG_HMAC_BASE) + 0x68)
#define HMAC_QUERY_BUSY_REG            ((DR_REG_HMAC_BASE) + 0x6c)

#define HMAC_WDATA_BASE                ((DR_REG_HMAC_BASE) + 0x80)
#define HMAC_RDATA_BASE                ((DR_REG_HMAC_BASE) + 0xC0)
#define HMAC_SET_MESSAGE_PAD_REG       ((DR_REG_HMAC_BASE) + 0xF0)
#define HMAC_ONE_BLOCK_REG             ((DR_REG_HMAC_BASE) + 0xF4)

/* AES-XTS registers */
#define AES_XTS_PLAIN_BASE        ((DR_REG_AES_BASE) + 0x100)
#define AES_XTS_SIZE_REG          ((DR_REG_AES_BASE) + 0x140)
#define AES_XTS_DESTINATION_REG   ((DR_REG_AES_BASE) + 0x144)
#define AES_XTS_PHYSICAL_ADDR_REG ((DR_REG_AES_BASE) + 0x148)

#define AES_XTS_TRIGGER_REG       ((DR_REG_AES_BASE) + 0x14C)
#define AES_XTS_RELEASE_REG       ((DR_REG_AES_BASE) + 0x150)
#define AES_XTS_DESTROY_REG       ((DR_REG_AES_BASE) + 0x154)
#define AES_XTS_STATE_REG         ((DR_REG_AES_BASE) + 0x158)

/* Digital Signature registers*/
#define DS_C_BASE                 ((DR_REG_DIGITAL_SIGNATURE_BASE) + 0x000 )
#define DS_IV_BASE                ((DR_REG_DIGITAL_SIGNATURE_BASE) + 0x630 )
#define DS_X_BASE                 ((DR_REG_DIGITAL_SIGNATURE_BASE) + 0x800 )
#define DS_Z_BASE                 ((DR_REG_DIGITAL_SIGNATURE_BASE) + 0xA00 )

#define DS_SET_START_REG          ((DR_REG_DIGITAL_SIGNATURE_BASE) + 0xE00)
#define DS_SET_ME_REG             ((DR_REG_DIGITAL_SIGNATURE_BASE) + 0xE04)
#define DS_SET_FINISH_REG         ((DR_REG_DIGITAL_SIGNATURE_BASE) + 0xE08)

#define DS_QUERY_BUSY_REG         ((DR_REG_DIGITAL_SIGNATURE_BASE) + 0xE0C)
#define DS_QUERY_KEY_WRONG_REG    ((DR_REG_DIGITAL_SIGNATURE_BASE) + 0xE10)
#define DS_QUERY_CHECK_REG        ((DR_REG_DIGITAL_SIGNATURE_BASE) + 0xE14)

#define DS_QUERY_CHECK_INVALID_DIGEST  (1<<0)
#define DS_QUERY_CHECK_INVALID_PADDING (1<<1)

#define DS_DATE_REG               ((DR_REG_DIGITAL_SIGNATURE_BASE) + 0xE20)

/* Crypto DMA */
#define CRYPTO_DMA_CONF0_REG             ((DR_REG_CRYPTO_DMA_BASE) + 0x00)
#define CRYPTO_DMA_INT_RAW_REG           ((DR_REG_CRYPTO_DMA_BASE) + 0x04)
#define CRYPTO_DMA_INT_ST_REG            ((DR_REG_CRYPTO_DMA_BASE) + 0x08)
#define CRYPTO_DMA_INT_ENA_REG           ((DR_REG_CRYPTO_DMA_BASE) + 0x0C)
#define CRYPTO_DMA_INT_CLR_REG           ((DR_REG_CRYPTO_DMA_BASE) + 0x10)
#define CRYPTO_DMA_OUT_STATUS_REG        ((DR_REG_CRYPTO_DMA_BASE) + 0x14)
#define CRYPTO_DMA_OUT_PUSH_REG          ((DR_REG_CRYPTO_DMA_BASE) + 0x18)
#define CRYPTO_DMA_IN_STATUS_REG         ((DR_REG_CRYPTO_DMA_BASE) + 0x1C)
#define CRYPTO_DMA_IN_POP_REG            ((DR_REG_CRYPTO_DMA_BASE) + 0x20)
#define CRYPTO_DMA_OUT_LINK_REG          ((DR_REG_CRYPTO_DMA_BASE) + 0x24)
#define CRYPTO_DMA_IN_LINK_REG           ((DR_REG_CRYPTO_DMA_BASE) + 0x28)
#define CRYPTO_DMA_CONF1_REG             ((DR_REG_CRYPTO_DMA_BASE) + 0x2C)
#define CRYPTO_DMA_STATE0_REG            ((DR_REG_CRYPTO_DMA_BASE) + 0x30)
#define CRYPTO_DMA_STATE1_REG            ((DR_REG_CRYPTO_DMA_BASE) + 0x34)
#define CRYPTO_DMA_OUT_EOF_DES_ADDR_REG  ((DR_REG_CRYPTO_DMA_BASE) + 0x38)
#define CRYPTO_DMA_IN_SUC_EOF_DES_ADDR   ((DR_REG_CRYPTO_DMA_BASE) + 0x3C)
#define CRYPTO_DMA_IN_ERR_EOF_DES_ADDR   ((DR_REG_CRYPTO_DMA_BASE) + 0x40)
#define CRYPTO_DMA_OUT_EOF_BFR_DES_ADDR  ((DR_REG_CRYPTO_DMA_BASE) + 0x44)
#define CRYPTO_DMA_AHB_TEST              ((DR_REG_CRYPTO_DMA_BASE) + 0x48)
#define CRYPTO_DMA_IN_DSCR_REG           ((DR_REG_CRYPTO_DMA_BASE) + 0x4C)
#define CRYPTO_DMA_IN_DSCR_BF0_REG       ((DR_REG_CRYPTO_DMA_BASE) + 0x50)
#define CRYPTO_DMA_IN_DSCR_BF1_REG       ((DR_REG_CRYPTO_DMA_BASE) + 0x54)
#define CRYPTO_DMA_OUT_DSCR_REG          ((DR_REG_CRYPTO_DMA_BASE) + 0x58)
#define CRYPTO_DMA_OUT_DSCR_BF0_REG      ((DR_REG_CRYPTO_DMA_BASE) + 0x5C)
#define CRYPTO_DMA_OUT_DSCR_BF1_REG      ((DR_REG_CRYPTO_DMA_BASE) + 0x60)
#define CRYPTO_DMA_AES_SHA_SELECT_REG    ((DR_REG_CRYPTO_DMA_BASE) + 0x64)
#define CRYPTO_DMA_DATE_REG              ((DR_REG_CRYPTO_DMA_BASE) + 0xFC)

#endif
