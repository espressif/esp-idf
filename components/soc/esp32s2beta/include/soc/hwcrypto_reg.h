// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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

/* SHA acceleration registers */
#define SHA_MODE_REG                  ((DR_REG_SHA_BASE) + 0x00)

#define SHA_MODE_SHA1 0
#define SHA_MODE_SHA224 1
#define SHA_MODE_SHA256 2
#define SHA_MODE_SHA384 3
#define SHA_MODE_SHA512 4
#define SHA_MODE_SHA512_224 5
#define SHA_MODE_SHA512_256 6
#define SHA_MODE_SHA512_T 7

#define SHA_T_STRING_REG              ((DR_REG_SHA_BASE) + 0x04)
#define SHA_T_LENGTH_REG              ((DR_REG_SHA_BASE) + 0x08)
#define SHA_START_REG                 ((DR_REG_SHA_BASE) + 0x0c)
#define SHA_CONTINUE_REG              ((DR_REG_SHA_BASE) + 0x10)
#define SHA_BUSY_REG                  ((DR_REG_SHA_BASE) + 0x14)

#define SHA_H_BASE                    ((DR_REG_SHA_BASE) + 0x40)
#define SHA_M_BASE                    ((DR_REG_SHA_BASE) + 0x80)

/* AES acceleration registers */
#define AES_MODE_REG            ((DR_REG_AES_BASE) + 0x40)
#define AES_ENDIAN_REG          ((DR_REG_AES_BASE) + 0x44)
#define AES_TRIGGER_REG         ((DR_REG_AES_BASE) + 0x48)
#define AES_STATE_REG           ((DR_REG_AES_BASE) + 0x4c)

#define AES_KEY_BASE            ((DR_REG_AES_BASE) + 0x00)
#define AES_TEXT_IN_BASE        ((DR_REG_AES_BASE) + 0x20)
#define AES_TEXT_OUT_BASE       ((DR_REG_AES_BASE) + 0x30)

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

/* AES-XTS registers */
#define AES_XTS_PLAIN_BASE        ((DR_REG_AES_BASE) + 0x80)
#define AES_XTS_SIZE_REG          ((DR_REG_AES_BASE) + 0xC0)
#define AES_XTS_DESTINATION_REG   ((DR_REG_AES_BASE) + 0xC4)
#define AES_XTS_PHYSICAL_ADDR_REG ((DR_REG_AES_BASE) + 0xC8)

#define AES_XTS_TRIGGER_REG       ((DR_REG_AES_BASE) + 0xCC)
#define AES_XTS_RELEASE_REG       ((DR_REG_AES_BASE) + 0xD0)
#define AES_XTS_DESTROY_REG       ((DR_REG_AES_BASE) + 0xD4)
#define AES_XTS_STATE_REG         ((DR_REG_AES_BASE) + 0xD8)

#endif
