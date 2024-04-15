/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_assert.h"
#include "esp_bit_defs.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PAU_SUPPORTED

#define REGDMA_LINK_ENTRY_NUM   (SOC_PM_PAU_LINK_NUM) /* Maximum number of REG DMA linked list entries */

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(a)  (sizeof(a) / sizeof((a)[0]))
#endif

#define ENTRY(n)    (BIT(n))

#define REGDMA_PHY_LINK(_pri)               ((0x00 << 8) | _pri)
#define REGDMA_PCR_LINK(_pri)               ((0x01 << 8) | _pri)
#define REGDMA_MODEMSYSCON_LINK(_pri)       ((0x02 << 8) | _pri)
#define REGDMA_MODEMLPCON_LINK(_pri)        ((0x03 << 8) | _pri)
#define REGDMA_PAU_LINK(_pri)               ((0x04 << 8) | _pri)

#define REGDMA_INTMTX_LINK(_pri)            ((0x0d << 8) | _pri)
#define REGDMA_HPSYS_LINK(_pri)             ((0x0e << 8) | _pri)
#define REGDMA_TEEAPM_LINK(_pri)            ((0x0f << 8) | _pri)

#define REGDMA_UART_LINK(_pri)              ((0x10 << 8) | _pri)
#define REGDMA_TIMG_LINK(_pri)              ((0x11 << 8) | _pri)
#define REGDMA_IOMUX_LINK(_pri)             ((0x12 << 8) | _pri)
#define REGDMA_SPIMEM_LINK(_pri)            ((0x13 << 8) | _pri)
#define REGDMA_SYSTIMER_LINK(_pri)          ((0x14 << 8) | _pri)
#define REGDMA_BLE_MAC_LINK(_pri)           ((0x15 << 8) | _pri)
#define REGDMA_MODEM_BT_BB_LINK(_pri)       ((0x16 << 8) | _pri)
#define REGDMA_MODEM_IEEE802154_LINK(_pri)  ((0x17 << 8) | _pri)
#define REGDMA_GDMA_LINK(_pri)              ((0x18 << 8) | _pri)
#define REGDMA_I2C_LINK(_pri)               ((0x19 << 8) | _pri)
#define REGDMA_RMT_LINK(_pri)               ((0x20 << 8) | _pri)
#define REGDMA_MODEM_FE_LINK(_pri)          ((0xFF << 8) | _pri)

#define REGDMA_LINK_PRI_SYS_CLK                 REGDMA_LINK_PRI_0
#define REGDMA_LINK_PRI_MODEM_CLK               REGDMA_LINK_PRI_1
#define REGDMA_LINK_PRI_CRITICAL_TEE_APM        REGDMA_LINK_PRI_2
#define REGDMA_LINK_PRI_WIFI_MAC_BB             REGDMA_LINK_PRI_3
#define REGDMA_LINK_PRI_NON_CRITICAL_TEE_APM    REGDMA_LINK_PRI_4
#define REGDMA_LINK_PRI_BT_MAC_BB               REGDMA_LINK_PRI_5
#define REGDMA_LINK_PRI_SYS_PERIPH_HIGH         REGDMA_LINK_PRI_5 // INT_MTX & HP_SYSTEM & Console UART
#define REGDMA_LINK_PRI_SYS_PERIPH_LOW          REGDMA_LINK_PRI_6 // TG0 & IO MUX & SPI MEM & Systimer
#define REGDMA_LINK_PRI_IEEE802154              REGDMA_LINK_PRI_7
#define REGDMA_LINK_PRI_GDMA                    REGDMA_LINK_PRI_7
#define REGDMA_LINK_PRI_RMT                     REGDMA_LINK_PRI_7

typedef enum {
    REGDMA_LINK_PRI_0 = 0,
    REGDMA_LINK_PRI_1,
    REGDMA_LINK_PRI_2,
    REGDMA_LINK_PRI_3,
    REGDMA_LINK_PRI_4,
    REGDMA_LINK_PRI_5,
    REGDMA_LINK_PRI_6,
    REGDMA_LINK_PRI_7,
} regdma_link_priority_t;


typedef void * regdma_entry_buf_t[REGDMA_LINK_ENTRY_NUM];

typedef enum regdma_link_mode {
    REGDMA_LINK_MODE_CONTINUOUS = 0, /*!< Link used to backup registers with consecutive addresses */
    REGDMA_LINK_MODE_ADDR_MAP,       /*!< Link used to backup selected registers according to bitmap */
    REGDMA_LINK_MODE_WRITE,          /*!< Link used to direct write to registers*/
    REGDMA_LINK_MODE_WAIT            /*!< Link used to wait for register value to meet condition*/
} regdma_link_mode_t;


typedef struct regdma_link_head {
    volatile uint32_t length: 10, /* total count of registers that need to be backup or restore, unit: 1 word = 4 bytes */
             reserve0: 6,
             mode    : 4, /* mode of current link */
             reserve1: 8,
             branch  : 1, /* branch link flag */
             skip_r  : 1, /* skip the current linked node when restore the register */
             skip_b  : 1, /* skip the current linked node when backup the register */
             eof     : 1; /* end of link */
} regdma_link_head_t;

/* Continuous type linked list node body type definition */
typedef struct regdma_link_continuous_body {
    volatile void       *next;
    volatile void       *backup;
    volatile void       *restore;
    volatile void       *mem;
} regdma_link_continuous_body_t;

/* Address Map type linked list node body type definition */
typedef struct regdma_link_addr_map_body {
    volatile void       *next;
    volatile void       *backup;
    volatile void       *restore;
    volatile void       *mem;
    volatile uint32_t   map[4];
} regdma_link_addr_map_body_t;

/* Write/Wait type linked list node body type definition */
typedef struct regdma_link_write_wait_body {
    volatile void       *next;
    volatile void       *backup;
    volatile uint32_t   value;
    volatile uint32_t   mask;
} regdma_link_write_wait_body_t;

/* Branch Continuous type linked list node body type definition */
typedef struct regdma_link_branch_continuous_body {
    regdma_entry_buf_t  next;
    volatile void       *backup;
    volatile void       *restore;
    volatile void       *mem;
} regdma_link_branch_continuous_body_t;

/* Branch Address Map type linked list node body type definition */
typedef struct regdma_link_branch_addr_map_body {
    regdma_entry_buf_t  next;
    volatile void       *backup;
    volatile void       *restore;
    volatile void       *mem;
    volatile uint32_t   map[4];
} regdma_link_branch_addr_map_body_t;

/* Branch Write/Wait type linked list node body type definition */
typedef struct regdma_link_branch_write_wait_body {
    regdma_entry_buf_t  next;
    volatile void       *backup;
    volatile uint32_t   value;
    volatile uint32_t   mask;
} regdma_link_branch_write_wait_body_t;

ESP_STATIC_ASSERT(REGDMA_LINK_ENTRY_NUM < 16, "regdma link entry number should less 16");
typedef struct regdma_link_stats {
    volatile uint32_t   ref: REGDMA_LINK_ENTRY_NUM, /* a bitmap, identifies which entry has referenced the current link */
             reserve: 16-REGDMA_LINK_ENTRY_NUM,
             id: 16; /* REGDMA linked list node unique identifier */
    volatile uint32_t   module; /* a bitmap used to identify the module to which the current node belongs */
} regdma_link_stats_t;

typedef struct regdma_link_continuous {
    regdma_link_stats_t             stat;
    regdma_link_head_t              head;
    regdma_link_continuous_body_t   body;
    volatile uint32_t               buff[0];
} regdma_link_continuous_t;

typedef struct regdma_link_addr_map {
    regdma_link_stats_t             stat;
    regdma_link_head_t              head;
    regdma_link_addr_map_body_t     body;
    volatile uint32_t               buff[0];
} regdma_link_addr_map_t;

typedef struct regdma_link_write_wait {
    regdma_link_stats_t             stat;
    regdma_link_head_t              head;
    regdma_link_write_wait_body_t   body;
} regdma_link_write_wait_t;

typedef struct regdma_link_branch_continuous {
    regdma_link_stats_t                  stat;
    regdma_link_head_t                   head;
    regdma_link_branch_continuous_body_t body;
    volatile uint32_t                    buff[0];
} regdma_link_branch_continuous_t;

typedef struct regdma_link_branch_addr_map {
    regdma_link_stats_t                  stat;
    regdma_link_head_t                   head;
    regdma_link_branch_addr_map_body_t   body;
    volatile uint32_t                    buff[0];
} regdma_link_branch_addr_map_t;

typedef struct regdma_link_branch_write_wait {
    regdma_link_stats_t                  stat;
    regdma_link_head_t                   head;
    regdma_link_branch_write_wait_body_t body;
} regdma_link_branch_write_wait_t;

typedef struct regdma_link_config {
    regdma_link_head_t  head;
    union {
        regdma_link_continuous_body_t        continuous;
        regdma_link_addr_map_body_t          addr_map;
        regdma_link_write_wait_body_t        write_wait;
    };
    int id; /* REGDMA linked list node unique identifier */
} regdma_link_config_t;

typedef struct {
    regdma_link_config_t    config;
    uint32_t                owner;  /**< Indicates which regdma entries the current node will insert into */
} regdma_entries_config_t;

#define REGDMA_LINK_HEAD(plink)  (((regdma_link_config_t *)plink)->head)

#define REGDMA_LINK_HEAD_INIT(_l, _m, _b, _sr, _sb) \
    {                       \
        .length = (_l),     \
        .mode   = (_m),     \
        .branch = (_b),     \
        .skip_r = (_sr),    \
        .skip_b = (_sb),    \
        .eof    = 0         \
    }

#define REGDMA_LINK_CONTINUOUS_INIT(_id, _backup, _restore, _len, _skip_b, _skip_r) \
    {                                       \
        .head = REGDMA_LINK_HEAD_INIT(      \
                _len,                       \
                REGDMA_LINK_MODE_CONTINUOUS,\
                0,                          \
                _skip_r,                    \
                _skip_b                     \
            ),                              \
        .continuous = {                     \
            .next    = NULL,                \
            .backup  = (void *)_backup,     \
            .restore = (void *)_restore,    \
            .mem     = NULL                 \
        },                                  \
        .id = (_id)                         \
    }

#define REGDMA_LINK_ADDR_MAP_INIT(_id, _backup, _restore, _len, _skip_b, _skip_r, ...) \
    {                                       \
        .head = REGDMA_LINK_HEAD_INIT(      \
                _len,                       \
                REGDMA_LINK_MODE_ADDR_MAP,  \
                0,                          \
                _skip_r,                    \
                _skip_b                     \
            ),                              \
        .addr_map = {                       \
            .next    = NULL,                \
            .backup  = (void *)_backup,     \
            .restore = (void *)_restore,    \
            .mem     = NULL,                \
            .map     = {__VA_ARGS__}        \
        },                                  \
        .id = (_id)                         \
    }

#define REGDMA_LINK_WRITE_INIT(_id, _backup, _val, _mask, _skip_b, _skip_r) \
    {                                       \
        .head = REGDMA_LINK_HEAD_INIT(      \
                0,                          \
                REGDMA_LINK_MODE_WRITE,     \
                0,                          \
                _skip_r,                    \
                _skip_b                     \
            ),                              \
        .write_wait = {                     \
            .next    = NULL,                \
            .backup  = (void *)_backup,     \
            .value   = (_val),              \
            .mask    = (_mask)              \
        },                                  \
        .id = (_id)                         \
    }

#define REGDMA_LINK_WAIT_INIT(_id, _backup, _val, _mask, _skip_b, _skip_r) \
    {                                       \
        .head = REGDMA_LINK_HEAD_INIT(      \
                0,                          \
                REGDMA_LINK_MODE_WAIT,      \
                0,                          \
                _skip_r,                    \
                _skip_b                     \
            ),                              \
        .write_wait = {                     \
            .next    = NULL,                \
            .backup  = (void *)_backup,     \
            .value   = (_val),              \
            .mask    = (_mask)              \
        },                                  \
        .id = (_id)                         \
    }

#endif // SOC_PAU_SUPPORTED

#ifdef __cplusplus
}
#endif
