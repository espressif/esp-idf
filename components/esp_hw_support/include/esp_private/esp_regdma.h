/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_assert.h"
#include "esp_macros.h"
#include "esp_err.h"
#include "esp_bit_defs.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_PAU_SUPPORTED
#include "hal/pau_types.h"

#define REGDMA_LINK_DBG 0       /* Enable REGDMA link info dump apis*/
#define REGDMA_LINK_ENTRY_NUM   (PAU_REGDMA_LINK_NUM) /* Maximum number of REG DMA linked list entries */

#define ENTRY(n)    (BIT(n))

#define REGDMA_PHY_LINK(_pri)               ((0x00 << 8) | _pri)
#define REGDMA_PCR_LINK(_pri)               ((0x01 << 8) | _pri)
#define REGDMA_MODEMSYSCON_LINK(_pri)       ((0x02 << 8) | _pri)
#define REGDMA_MODEMLPCON_LINK(_pri)        ((0x03 << 8) | _pri)

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
#define REGDMA_MODEM_GDMA_LINK(_pri)        ((0x18 << 8) | _pri)
#define REGDMA_MODEM_FE_LINK(_pri)          ((0xFF << 8) | _pri)

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

typedef pau_regdma_link_addr_t  regdma_entry_buf_t;

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


#define REGDMA_LINK_HEAD(plink)  (((regdma_link_config_t *)plink)->head)


#ifndef ARRAY_SIZE
# define ARRAY_SIZE(a)  (sizeof(a) / sizeof((a)[0]))
#endif

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


/**
 * @brief Create a REGDMA continuous type linked list node without retention buffer and the retention buffer is passed in by the caller
 * @param  backup  Register address to be backed up by REGDMA
 * @param  buff    Retention buffer, it needs to be allocated by the caller and passed in by this argument
 * @param  len     Number of registers to be backed up
 * @param  restore Register address to be restored by REGDMA
 * @param  next    The next REGDMA linked list node
 * @param  skip_b  Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r  Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id      REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module  The module identifier of the current linked list node
 * @return         Created REGDMA linked list node pointer
 */
void *regdma_link_new_continuous(void *backup, void *buff, int len, void *restore, void *next, bool skip_b, bool skip_r, int id, uint32_t module);

/**
 * @brief Create a REGDMA addr_map type linked list node without retention buffer and the retention buffer is passed in by the caller
 * @param  backup  Register address to be backed up by REGDMA
 * @param  buff    Retention buffer, it needs to be allocated by the caller and passed in by this argument
 * @param  bitmap  The register bitmap that needs to be backed up and restored. when the bitmap corresponding to the
 *                 register is 1, it needs to be backed up or restored, otherwise the corresponding register is skipped.
 * @param  len     Number of registers to be backed up
 * @param  restore Register address to be restored by REGDMA
 * @param  next    The next REGDMA linked list node
 * @param  skip_b  Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r  Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id      REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module  The module identifier of the current linked list node
 * @return         Created REGDMA linked list node pointer
 */
void *regdma_link_new_addr_map(void *backup, void *buff, uint32_t bitmap[4], int len, void *restore, void *next, bool skip_b, bool skip_r, int id, uint32_t module);

/**
 * @brief  Create a REGDMA write type linked list node without retention buffer and the retention buffer is passed in by the caller
 * @param  backup Register address to be backed up by REGDMA
 * @param  value  The value to be written to the register
 * @param  mask   The mask of value
 * @param  next   The next REGDMA linked list node
 * @param  skip_b Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id     REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module The module identifier of the current linked list node
 * @return        Created REGDMA linked list node pointer
 */
void *regdma_link_new_write(void *backup, uint32_t value, uint32_t mask, void *next, bool skip_b, bool skip_r, int id, uint32_t module);

/**
 * @brief Create a REGDMA write type linked list node without retention buffer and the retention buffer is passed in by the caller
 * @param  backup Register address to be backed up by REGDMA
 * @param  value  The register value that needs to be waited for
 * @param  mask   The mask of value
 * @param  next   The next REGDMA linked list node
 * @param  skip_b Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id     REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module The module identifier of the current linked list node
 * @return        Created REGDMA linked list node pointer
 */
void *regdma_link_new_wait(void *backup, uint32_t value, uint32_t mask, void *next, bool skip_b, bool skip_r, int id, uint32_t module);

/**
 * @brief Create a REGDMA continuouos branch list node without retention buffer and the retention buffer is passed in by the caller
 * @param  backup  Register address to be backed up by REGDMA
 * @param  buff    Retention buffer, it needs to be allocated by the caller and passed in by this argument
 * @param  len     Number of registers to be backed up
 * @param  restore Register address to be restored by REGDMA
 * @param  next    The next REGDMA linked list node (supports up to 4 next pointers)
 * @param  skip_b  Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r  Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id      REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module  The module identifier of the current linked list node
 * @return         Created REGDMA linked list node pointer
 */
void *regdma_link_new_branch_continuous(void *backup, void *buff, int len, void *restore, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module);

/**
 * @brief Create a REGDMA addr_map branch list node without retention buffer and the retention buffer is passed in by the caller
 * @param  backup  Register address to be backed up by REGDMA
 * @param  buff    Retention buffer, it needs to be allocated by the caller and passed in by this argument
 * @param  bitmap  The register bitmap that needs to be backed up and restored. when the bitmap corresponding to the
 *                 register is 1, it needs to be backed up or restored, otherwise the corresponding register is skipped.
 * @param  len     Number of registers to be backed up
 * @param  restore Register address to be restored by REGDMA
 * @param  next    The next REGDMA linked list node (supports up to 4 next pointers)
 * @param  skip_b  Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r  Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id      REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module  The module identifier of the current linked list node
 * @return         Created REGDMA linked list node pointer
 */
void *regdma_link_new_branch_addr_map(void *backup, void *buff, uint32_t bitmap[4], int len, void *restore, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module);

/**
 * @brief Create a REGDMA write branch list node without retention buffer and the retention buffer is passed in by the caller
 * @param  backup Register address to be backed up by REGDMA
 * @param  value  The value to be written to the register
 * @param  mask   The mask of value
 * @param  next   The next REGDMA linked list node (supports up to 4 next pointers)
 * @param  skip_b Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id     REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module The module identifier of the current linked list node
 * @return        Created REGDMA linked list node pointer
 */
void *regdma_link_new_branch_write(void *backup, uint32_t value, uint32_t mask, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module);

/**
 * @brief Create a REGDMA wait branch list node without retention buffer and the retention buffer is passed in by the caller
 * @param  backup Register address to be backed up by REGDMA
 * @param  value  The register value that needs to be waited for
 * @param  mask   The mask of value
 * @param  next   The next REGDMA linked list node (supports up to 4 next pointers)
 * @param  skip_b Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id     REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module The module identifier of the current linked list node
 * @return        Created REGDMA linked list node pointer
 */
void *regdma_link_new_branch_wait(void *backup, uint32_t value, uint32_t mask, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module);

/**
 * @brief Create a default REGDMA continuous type linked list node with retention buffer
 * @param  backup  Register address to be backed up by REGDMA
 * @param  len     Number of registers to be backed up
 * @param  restore Register address to be restored by REGDMA
 * @param  next    The next REGDMA linked list node
 * @param  skip_b  Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r  Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id      REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module  The module identifier of the current linked list node
 * @return         Created REGDMA linked list node pointer
 */
void *regdma_link_new_continuous_default(void *backup, int len, void *restore, void *next, bool skip_b, bool skip_r, int id, uint32_t module);

/**
 * @brief Create a default REGDMA addr_map type linked list node with retention buffer
 * @param  backup  Register address to be backed up by REGDMA
 * @param  bitmap  The register bitmap that needs to be backed up and restored. when the bitmap corresponding to the
 *                 register is 1, it needs to be backed up or restored, otherwise the corresponding register is skipped.
 * @param  len     Number of registers to be backed up
 * @param  restore Register address to be restored by REGDMA
 * @param  next    The next REGDMA linked list node
 * @param  skip_b  Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r  Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id      REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module  The module identifier of the current linked list node
 * @return         Created REGDMA linked list node pointer
 */
void *regdma_link_new_addr_map_default(void *backup, uint32_t bitmap[4], int len, void *restore, void *next, bool skip_b, bool skip_r, int id, uint32_t module);

/**
 * @brief Create a default REGDMA write type linked list node with retention buffer
 * @param  backup Register address to be backed up by REGDMA
 * @param  value  The register value that needs to be waited for
 * @param  mask   The mask of value
 * @param  next   The next REGDMA linked list node
 * @param  skip_b Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id     REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module The module identifier of the current linked list node
 * @return        Created REGDMA linked list node pointer
 */
void *regdma_link_new_write_default(void *backup, uint32_t value, uint32_t mask, void *next, bool skip_b, bool skip_r, int id, uint32_t module);

/**
 * @brief Create a default REGDMA wait type linked list node with retention buffer
 * @param  backup Register address to be backed up by REGDMA
 * @param  value  The register value that needs to be waited for
 * @param  mask   The mask of value
 * @param  next   The next REGDMA linked list node
 * @param  skip_b Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id     REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module The module identifier of the current linked list node
 * @return        Created REGDMA linked list node pointer
 */
void *regdma_link_new_wait_default(void *backup, uint32_t value, uint32_t mask, void *next, bool skip_b, bool skip_r, int id, uint32_t module);

/**
 * @brief Create a default REGDMA continuous branch list node with retention buffer
 * @param  backup  Register address to be backed up by REGDMA
 * @param  len     Number of registers to be backed up
 * @param  restore Register address to be restored by REGDMA
 * @param  next    The next REGDMA linked list node (supports up to 4 next pointers)
 * @param  skip_b  Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r  Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id      REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module  The module identifier of the current linked list node
 * @return         Created REGDMA linked list node pointer
 */
void *regdma_link_new_branch_continuous_default(void *backup, int len, void *restore, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module);

/**
 * @brief Create a default REGDMA addr_map branch list node with retention buffer
 * @param  backup  Register address to be backed up by REGDMA
 * @param  bitmap  The register bitmap that needs to be backed up and restored. when the bitmap corresponding to the
 *                 register is 1, it needs to be backed up or restored, otherwise the corresponding register is skipped.
 * @param  len     Number of registers to be backed up
 * @param  restore Register address to be restored by REGDMA
 * @param  next    The next REGDMA linked list node (supports up to 4 next pointers)
 * @param  skip_b  Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r  Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id      REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module  The module identifier of the current linked list node
 * @return         Created REGDMA linked list node pointer
 */
void *regdma_link_new_branch_addr_map_default(void *backup, uint32_t bitmap[4], int len, void *restore, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module);

/**
 * @brief Create a default REGDMA write branch list node with retention buffer
 * @param  backup Register address to be backed up by REGDMA
 * @param  value  The value to be written to the register
 * @param  mask   The mask of value
 * @param  next   The next REGDMA linked list node (supports up to 4 next pointers)
 * @param  skip_b Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id     REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module The module identifier of the current linked list node
 * @return        Created REGDMA linked list node pointer
 */
void *regdma_link_new_branch_write_default(void *backup, uint32_t value, uint32_t mask, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module);

/**
 * @brief Create a default REGDMA wait branch list node with retention buffer
 * @param  backup Register address to be backed up by REGDMA
 * @param  value  The register value that needs to be waited for
 * @param  mask   The mask of value
 * @param  next   The next REGDMA linked list node (supports up to 4 next pointers)
 * @param  skip_b Skip backup, True means that REGDMA skips the current node when executing the backup task
 * @param  skip_r Skip restore, True means that REGDMA skips the current node when executing the restore task
 * @param  id     REGDMA linked list node unique identifier, the caller needs to ensure that the id of each node is unique
 * @param  module The module identifier of the current linked list node
 * @return        Created REGDMA linked list node pointer
 */
void *regdma_link_new_branch_wait_default(void *backup, uint32_t value, uint32_t mask, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module);

/**
 * @brief Create and initialize a REGDMA linked list node through configuration parameters
 * @param  config REGDMA linked node configuration parameters
 * @param  branch Is it a branch node
 * @param  module The module identifier of the current linked list node
 * @param  nentry The number of next pointers
 * @param  args   next pointer, Since REGDMA supports 4 entries, it supports up to 4 variable parameter next pointers, and more will be ignored
 * @return        Initialized REGDMA linked list head node pointer
 */
void *regdma_link_init(const regdma_link_config_t *config, bool branch, uint32_t module, int nentry, ...);

/**
 * @brief Recurse the REGDMA linked list and call the hook subroutine for each node
 * @param  link  The REGDMA linkded list head pointer
 * @param  entry For nodes that support branching, use the branch specified by entry argument recursively
 * @param  hook  Subroutines called during recursion, argument 1 is the pointer to the
 *               recursive node object, argument 2 is the entry to which the node belongs
 *               and the argument 3 is the position of the node in the current linked
 *               list (from head to tail, the position of the head node is 0)
 * @return       The REGDMA linked list node pointer indicated by the link argument
 */
void *regdma_link_recursive(void *link, int entry, void (*hook)(void *, int, int));

/**
 * @brief Find the linked list node object by node position
 * @param  link  The REGDMA linkded list head pointer
 * @param  entry For nodes that support branching, use the branch specified by entry argument recursively
 * @param  pos   Node position
 * @return       The linked list node object pointer or NULL
 */
void *regdma_find_link_by_pos(void *link, int entry, int pos);

/**
 * @brief Find the linked list node object by node unique identifier
 * @param  link  The REGDMA linkded list head pointer
 * @param  entry For nodes that support branching, use the branch specified by entry argument recursively
 * @param  id    REGDMA linked list node unique identifier
 * @return       The linked list node object pointer or NULL
 */
void *regdma_find_link_by_id(void *link, int entry, int id);

/**
 * @brief Destroy the REGDMA linked list indicated by the entry argument
 * @param link  The REGDMA linkded list head pointer
 * @param entry For nodes that support branching, use the branch specified by entry argument recursively
 */
void regdma_link_destroy(void *link, int entry);

/**
 * @brief Generate the statistics information of the REGDMA linked list indicated by the entry argument
 * @param link  The REGDMA linkded list head pointer
 * @param entry For nodes that support branching, use the branch specified by entry argument recursively
 */
void regdma_link_stats(void *link, int entry);

/**
 * @brief Set the value and mask of write or wait type REGDMA linked list node
 * @param link  Write or wait type REGDMA linked list node pointer
 * @param value The value to be written to the register
 * @param mask  The mask of value
 */
void regdma_link_set_write_wait_content(void *link, uint32_t value, uint32_t mask);

/**
 * @brief Print all node information of the REGDMA linked list indicated by the entry argument
 * @param link  The REGDMA linkded list head pointer
 * @param entry For nodes that support branching, use the branch specified by entry argument recursively
 */
void regdma_link_show_memories(void *link, int entry);

/**
 * @brief Update REGDMA linked list node next pointers
 * @param link  The pointer of the REGDMA linked list node whose next field will be modified
 * @param nentry The number of next pointers
 */
void regdma_link_update_next(void *link, int nentry, ...);

/**
 * @brief Get all node entry reference bitmaps from the start of the link argument to the
 * end of the tail argument in the REGDMA linked list indicated by the entry argument
 * @param  link  The REGDMA linkded list head pointer
 * @param  tail  The REGDMA linkded list tail pointer
 * @param  entry For nodes that support branching, use the branch specified by entry argument recursively
 * @return       The entry reference bitmap of all nodes starting from the link argument to the end of the tail argument
 */
uint32_t regdma_link_get_owner_bitmap(void *link, void *tail, int entry);

/**
 * @brief Find the head node of the specified module in the REGDMA linked list indicated by the
 * entry argument starting from the link argument to the end of the tail argument
 * @param  link   The REGDMA linkded list head pointer
 * @param  tail   The REGDMA linkded list tail pointer
 * @param  entry  For nodes that support branching, use the branch specified by entry argument recursively
 * @param  module Module bitmap Identification
 * @return        The found head node pointer or NULL
 */
void *regdma_find_module_link_head(void *link, void *tail, int entry, uint32_t module);

/**
 * @brief Find the tail node of the specified module in the REGDMA linked list indicated by the
 * entry argument starting from the link argument to the end of the tail argument
 * @param  link   The REGDMA linkded list head pointer
 * @param  tail   The REGDMA linkded list tail pointer
 * @param  entry  For nodes that support branching, use the branch specified by entry argument recursively
 * @param  module Module bitmap Identification
 * @return        The found tail node pointer or NULL
 */
void *regdma_find_module_link_tail(void *link, void *tail, int entry, uint32_t module);

/**
 * @brief Find the tail node of the previous module of the specified module in the REGDMA linked list
 * indicated by the entry argument starting from the link argment to the end of the tail argument
 * @param  link   The REGDMA linkded list head pointer
 * @param  tail   The REGDMA linkded list tail pointer
 * @param  entry  For nodes that support branching, use the branch specified by entry argument recursively
 * @param  module Module bitmap Identification
 * @return        The found tail node pointer or NULL
 */
void *regdma_find_prev_module_link_tail(void *link, void *tail, int entry, uint32_t module);

/**
 * @brief Find the head node of the next module of the specified module in the REGDMA linked list
 * indicated by the entry argument starting from the link argment to the end of the tail argument
 * @param  link   The REGDMA linkded list head pointer
 * @param  tail   The REGDMA linkded list tail pointer
 * @param  entry  For nodes that support branching, use the branch specified by entry argument recursively
 * @param  module Module bitmap Identification
 * @return        The found head node pointer or NULL
 */
void *regdma_find_next_module_link_head(void *link, void *tail, int entry, uint32_t module);

#define regdma_link_init_safe(pcfg, branch, module, ...)    regdma_link_init((pcfg), (branch), (module), __VA_NARG__(__VA_ARGS__), ##__VA_ARGS__)

#define regdma_link_update_next_safe(link, ...)             regdma_link_update_next((link), __VA_NARG__(__VA_ARGS__), ##__VA_ARGS__)

#endif // SOC_PAU_SUPPORTED

#ifdef __cplusplus
}
#endif
