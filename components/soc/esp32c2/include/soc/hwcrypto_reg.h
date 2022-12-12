/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __HWCRYPTO_REG_H__
#define __HWCRYPTO_REG_H__

#include "soc.h"

#define SHA_MODE_SHA1       0
#define SHA_MODE_SHA224     1
#define SHA_MODE_SHA256     2

/* SHA acceleration registers */
#define SHA_MODE_REG                  ((DR_REG_SHA_BASE) + 0x00)
#define SHA_BLOCK_NUM_REG             ((DR_REG_SHA_BASE) + 0x0C)
#define SHA_START_REG                 ((DR_REG_SHA_BASE) + 0x10)
#define SHA_CONTINUE_REG              ((DR_REG_SHA_BASE) + 0x14)
#define SHA_BUSY_REG                  ((DR_REG_SHA_BASE) + 0x18)
#define SHA_DMA_START_REG             ((DR_REG_SHA_BASE) + 0x1C)
#define SHA_DMA_CONTINUE_REG          ((DR_REG_SHA_BASE) + 0x20)
#define SHA_CLEAR_IRQ_REG             ((DR_REG_SHA_BASE) + 0x24)
#define SHA_INT_ENA_REG               ((DR_REG_SHA_BASE) + 0x28)
#define SHA_DATE_REG                  ((DR_REG_SHA_BASE) + 0x2C)

#define SHA_H_BASE                    ((DR_REG_SHA_BASE) + 0x40)
#define SHA_TEXT_BASE                 ((DR_REG_SHA_BASE) + 0x80)

/* AES-XTS registers */
#define AES_XTS_PLAIN_BASE        ((DR_REG_AES_XTS_BASE) + 0x00)
#define AES_XTS_SIZE_REG          ((DR_REG_AES_XTS_BASE) + 0x40)
#define AES_XTS_DESTINATION_REG   ((DR_REG_AES_XTS_BASE) + 0x44)
#define AES_XTS_PHYSICAL_ADDR_REG ((DR_REG_AES_XTS_BASE) + 0x48)

#define AES_XTS_TRIGGER_REG       ((DR_REG_AES_XTS_BASE) + 0x4C)
#define AES_XTS_RELEASE_REG       ((DR_REG_AES_XTS_BASE) + 0x50)
#define AES_XTS_DESTROY_REG       ((DR_REG_AES_XTS_BASE) + 0x54)
#define AES_XTS_STATE_REG         ((DR_REG_AES_XTS_BASE) + 0x58)

#endif
