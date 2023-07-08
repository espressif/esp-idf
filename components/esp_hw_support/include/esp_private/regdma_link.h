/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "esp_bit_defs.h"
#include "soc/soc_caps.h"

#if SOC_PAU_SUPPORTED
#include "esp_regdma.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FILL_PLINK_HEAD(_pl, _len, _mode, _branch, _sr, _sb, _eof) { \
    _pl->head.length = _len;     \
    _pl->head.mode   = _mode;    \
    _pl->head.branch = _branch;   \
    _pl->head.skip_r = _sr;      \
    _pl->head.skip_b = _sb;      \
    _pl->head.eof    = _eof;     \
}

#define FILL_PLINK_STAT(_pl, _ref, _id, _module) { \
    _pl->stat.ref    = _ref;     \
    _pl->stat.id     = _id;      \
    _pl->stat.module = _module;  \
}

static inline void * regdma_link_init_continuous(
        regdma_link_continuous_t *plink, void *buff, void *backup, int len,
            void *restore, void *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    assert(plink != NULL);
    assert(buff !=NULL);

    FILL_PLINK_HEAD(plink, len, REGDMA_LINK_MODE_CONTINUOUS, 0, skip_r, skip_b, !next);
    plink->body.next    = next;
    plink->body.backup  = backup;
    plink->body.restore = restore;
    plink->body.mem     = buff;
    FILL_PLINK_STAT(plink, 0, (uint16_t)id, module);

    return (void *)plink;
}

static inline int regdma_link_addr_map_count(uint32_t bitmap[4])
{
    return  __builtin_popcount(bitmap[0]) + \
            __builtin_popcount(bitmap[1]) + \
            __builtin_popcount(bitmap[2]) + \
            __builtin_popcount(bitmap[3]);
}

static inline void * regdma_link_init_addr_map(
        regdma_link_addr_map_t *plink, void *buff, void *backup, uint32_t bitmap[4],
            int len, void *restore, void *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    assert(plink != NULL);
    assert(buff != NULL);
    assert(len == regdma_link_addr_map_count(bitmap));

    FILL_PLINK_HEAD(plink, len, REGDMA_LINK_MODE_ADDR_MAP, 0, skip_r, skip_b, !next);
    plink->body.next    = next;
    plink->body.backup  = backup;
    plink->body.restore = restore;
    plink->body.mem     = buff;
    plink->body.map[0]  = bitmap[0];
    plink->body.map[1]  = bitmap[1];
    plink->body.map[2]  = bitmap[2];
    plink->body.map[3]  = bitmap[3];
    FILL_PLINK_STAT(plink, 0, (uint16_t)id, module);

    return (void *)plink;
}

static inline void * regdma_link_init_write(
        regdma_link_write_wait_t *plink, void *backup, uint32_t value,
            uint32_t mask, void *next, bool skip_b, bool skip_r, int id,
                uint32_t module)
{
    assert(plink != NULL);

    FILL_PLINK_HEAD(plink, 0, REGDMA_LINK_MODE_WRITE, 0, skip_r, skip_b, !next);
    plink->body.next    = next;
    plink->body.backup  = backup;
    plink->body.value   = value;
    plink->body.mask    = mask;
    FILL_PLINK_STAT(plink, 0, (uint16_t)id, module);

    return (void *)plink;
}

static inline void * regdma_link_init_wait(
        regdma_link_write_wait_t *plink, void *backup, uint32_t value,
            uint32_t mask, void *next, bool skip_b, bool skip_r, int id,
                uint32_t module)
{
    assert(plink != NULL);

    FILL_PLINK_HEAD(plink, 0, REGDMA_LINK_MODE_WAIT, 0, skip_r, skip_b, !next);
    plink->body.next    = next;
    plink->body.backup  = backup;
    plink->body.value   = value;
    plink->body.mask    = mask;
    FILL_PLINK_STAT(plink, 0, (uint16_t)id, module);

    return (void *)plink;
}

static inline void * regdma_link_init_branch_continuous(
        regdma_link_branch_continuous_t *plink, void *buff, void *backup, int len, void *restore,
            regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    assert(plink != NULL);
    assert(buff !=NULL);

    FILL_PLINK_HEAD(plink, len, REGDMA_LINK_MODE_CONTINUOUS, 1, skip_r, skip_b, 0);
    plink->body.backup  = backup;
    plink->body.restore = restore;
    plink->body.mem     = buff;
    for (int i = 0; i < REGDMA_LINK_ENTRY_NUM; i++) {
        plink->body.next[i] = (*next)[i];
    }
    FILL_PLINK_STAT(plink, 0, (uint16_t)id, module);

    return (void *)plink;
}

static inline void * regdma_link_init_branch_addr_map(
        regdma_link_branch_addr_map_t *plink, void *buff, void *backup, uint32_t bitmap[4],
            int len, void *restore, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id,
                uint32_t module)
{
    assert(plink != NULL);
    assert(buff != NULL);

    FILL_PLINK_HEAD(plink, len, REGDMA_LINK_MODE_ADDR_MAP, 1, skip_r, skip_b, 0);
    plink->body.backup  = backup;
    plink->body.restore = restore;
    plink->body.mem     = buff;
    memcpy(plink->body.next, *next, REGDMA_LINK_ENTRY_NUM * sizeof((*next)[0]));
    plink->body.map[0]  = bitmap[0];
    plink->body.map[1]  = bitmap[1];
    plink->body.map[2]  = bitmap[2];
    plink->body.map[3]  = bitmap[3];
    FILL_PLINK_STAT(plink, 0, (uint16_t)id, module);

    return (void *)plink;
}

static inline void * regdma_link_init_branch_write(
        regdma_link_branch_write_wait_t *plink, void *backup, uint32_t value, uint32_t mask,
            regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    assert(plink != NULL);

    FILL_PLINK_HEAD(plink, 0, REGDMA_LINK_MODE_WRITE, 1, skip_r, skip_b, 0);
    plink->body.backup  = backup;
    plink->body.value   = value;
    plink->body.mask    = mask;
    for (int i = 0; i < REGDMA_LINK_ENTRY_NUM; i++) {
        plink->body.next[i] = (*next)[i];
    }
    FILL_PLINK_STAT(plink, 0, (uint16_t)id, module);

    return (void *)plink;
}

static inline void * regdma_link_init_branch_wait(
        regdma_link_branch_write_wait_t *plink, void *backup, uint32_t value, uint32_t mask,
            regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    assert(plink != NULL);

    FILL_PLINK_HEAD(plink, 0, REGDMA_LINK_MODE_WAIT, 1, skip_r, skip_b, 0);
    plink->body.backup  = backup;
    plink->body.value   = value;
    plink->body.mask    = mask;
    for (int i = 0; i < REGDMA_LINK_ENTRY_NUM; i++) {
        plink->body.next[i] = (*next)[i];
    }
    FILL_PLINK_STAT(plink, 0, (uint16_t)id, module);

    return (void *)plink;
}

static inline void regdma_link_update_stats(regdma_link_stats_t *stats, int entry, int depth)
{
    assert(stats != NULL);

    stats->ref |= BIT(entry);
}

#ifdef __cplusplus
}
#endif

#endif // SOC_PAU_SUPPORTED
