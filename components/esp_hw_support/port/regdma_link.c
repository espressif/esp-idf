/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <sys/cdefs.h>

#include "esp_private/regdma_link.h"

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_regdma.h"



#define REGDMA_LINK_ADDR_ALIGN      (4)
#define REGDMA_LINK_MEM_TYPE_CAPS   (MALLOC_CAP_DMA | MALLOC_CAP_DEFAULT)

void * regdma_link_new_continuous(void *backup, void *buff, int len, void *restore, void *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    regdma_link_continuous_t *link = (regdma_link_continuous_t *)heap_caps_aligned_alloc(
            REGDMA_LINK_ADDR_ALIGN,
            buff ? sizeof(regdma_link_continuous_t) : (sizeof(regdma_link_continuous_t) + (len<<2)),
            REGDMA_LINK_MEM_TYPE_CAPS
        );
    if (link) {
        memset(link, 0, buff ? sizeof(regdma_link_continuous_t) : (sizeof(regdma_link_continuous_t) + (len<<2)));
        void *buf = buff ? buff : (void *)(link->buff);
        link = regdma_link_init_continuous(link, buf, backup, len, restore, next, skip_b, skip_r, id, module);
        return (void *)((void *)link + offsetof(regdma_link_continuous_t, head));
    }
    return NULL;
}

void * regdma_link_new_addr_map(void *backup, void *buff, uint32_t bitmap[4], int len, void *restore, void *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    regdma_link_addr_map_t *link = (regdma_link_addr_map_t *)heap_caps_aligned_alloc(
            REGDMA_LINK_ADDR_ALIGN,
            buff ? sizeof(regdma_link_addr_map_t) : (sizeof(regdma_link_addr_map_t) + (len<<2)),
            REGDMA_LINK_MEM_TYPE_CAPS
        );
    if (link) {
        memset(link, 0, buff ? sizeof(regdma_link_addr_map_t) : (sizeof(regdma_link_addr_map_t) + (len<<2)));
        void *buf = buff ? buff : (void *)(link->buff);
        link = regdma_link_init_addr_map(link, buf, backup, bitmap, len, restore, next, skip_b, skip_r, id, module);
        return (void *)((void *)link + offsetof(regdma_link_addr_map_t, head));
    }
    return NULL;
}

void * regdma_link_new_write(void *backup, uint32_t value, uint32_t mask, void *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    regdma_link_write_wait_t *link = (regdma_link_write_wait_t *)heap_caps_aligned_alloc(
            REGDMA_LINK_ADDR_ALIGN, sizeof(regdma_link_write_wait_t), REGDMA_LINK_MEM_TYPE_CAPS);
    if (link) {
        memset(link, 0, sizeof(regdma_link_write_wait_t));
        link = regdma_link_init_write(link, backup, value, mask, next, skip_b, skip_r, id, module);
        return (void *)((void *)link + offsetof(regdma_link_write_wait_t, head));
    }
    return NULL;
}

void * regdma_link_new_wait(void *backup, uint32_t value, uint32_t mask, void *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    regdma_link_write_wait_t *link = (regdma_link_write_wait_t *)heap_caps_aligned_alloc(
            REGDMA_LINK_ADDR_ALIGN, sizeof(regdma_link_write_wait_t), REGDMA_LINK_MEM_TYPE_CAPS);
    if (link) {
        memset(link, 0, sizeof(regdma_link_write_wait_t));
        link = regdma_link_init_wait(link, backup, value, mask, next, skip_b, skip_r, id, module);
        return (void *)((void *)link + offsetof(regdma_link_write_wait_t, head));
    }
    return NULL;
}

void * regdma_link_new_branch_continuous(void *backup, void *buff, int len, void *restore, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    regdma_link_branch_continuous_t *link = (regdma_link_branch_continuous_t *)heap_caps_aligned_alloc(
            REGDMA_LINK_ADDR_ALIGN,
            buff ? sizeof(regdma_link_branch_continuous_t) : (sizeof(regdma_link_branch_continuous_t) + (len<<2)),
            REGDMA_LINK_MEM_TYPE_CAPS
        );
    if (link) {
        memset(link, 0, buff ? sizeof(regdma_link_branch_continuous_t) : (sizeof(regdma_link_branch_continuous_t) + (len<<2)));
        void *buf = buff ? buff : (void *)(link->buff);
        link = regdma_link_init_branch_continuous(link, buf, backup, len, restore, next, skip_b, skip_r, id, module);
        return (void *)((void *)link + offsetof(regdma_link_branch_continuous_t, head));
    }
    return NULL;
}

void * regdma_link_new_branch_addr_map(void *backup, void *buff, uint32_t bitmap[4], int len, void *restore, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    regdma_link_branch_addr_map_t *link = (regdma_link_branch_addr_map_t *)heap_caps_aligned_alloc(
            REGDMA_LINK_ADDR_ALIGN,
            buff ? sizeof(regdma_link_branch_addr_map_t) : (sizeof(regdma_link_branch_addr_map_t) + (len<<2)),
            REGDMA_LINK_MEM_TYPE_CAPS
        );
    if (link) {
        memset(link, 0, buff ? sizeof(regdma_link_branch_addr_map_t) : (sizeof(regdma_link_branch_addr_map_t) + (len<<2)));
        void *buf = buff ? buff : (void *)(link->buff);
        link = regdma_link_init_branch_addr_map(link, buf, backup, bitmap, len, restore, next, skip_b, skip_r, id, module);
        return (void *)((void *)link + offsetof(regdma_link_branch_addr_map_t, head));
    }
    return NULL;
}

void * regdma_link_new_branch_write(void *backup, uint32_t value, uint32_t mask, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    regdma_link_branch_write_wait_t *link = (regdma_link_branch_write_wait_t *)heap_caps_aligned_alloc(
            REGDMA_LINK_ADDR_ALIGN, sizeof(regdma_link_branch_write_wait_t), REGDMA_LINK_MEM_TYPE_CAPS);
    if (link) {
        memset(link, 0, sizeof(regdma_link_branch_write_wait_t));
        link = regdma_link_init_branch_write(link, backup, value, mask, next, skip_b, skip_r, id, module);
        return (void *)((void *)link + offsetof(regdma_link_branch_write_wait_t, head));
    }
    return NULL;
}

void * regdma_link_new_branch_wait(void *backup, uint32_t value, uint32_t mask, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    regdma_link_branch_write_wait_t *link = (regdma_link_branch_write_wait_t *)heap_caps_aligned_alloc(
            REGDMA_LINK_ADDR_ALIGN, sizeof(regdma_link_branch_write_wait_t), REGDMA_LINK_MEM_TYPE_CAPS);
    if (link) {
        memset(link, 0, sizeof(regdma_link_branch_write_wait_t));
        link = regdma_link_init_branch_wait(link, backup, value, mask, next, skip_b, skip_r, id, module);
        return (void *)((void *)link + offsetof(regdma_link_branch_write_wait_t, head));
    }
    return NULL;
}

void * regdma_link_new_continuous_default(void *backup, int len, void *restore, void *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    return regdma_link_new_continuous(backup, NULL, len, restore, next, skip_b, skip_r, id, module);
}

void * regdma_link_new_addr_map_default(void *backup, uint32_t bitmap[4], int len, void *restore, void *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    return regdma_link_new_addr_map(backup, NULL, bitmap, len, restore, next, skip_b, skip_r, id, module);
}

void * regdma_link_new_write_default(void *backup, uint32_t value, uint32_t mask, void *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    return regdma_link_new_write(backup, value, mask, next, skip_b, skip_r, id, module);
}

void * regdma_link_new_wait_default(void *backup, uint32_t value, uint32_t mask, void *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    return regdma_link_new_wait(backup, value, mask, next, skip_b, skip_r, id, module);
}

void * regdma_link_new_branch_continuous_default(void *backup, int len, void *restore, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    return regdma_link_new_branch_continuous(backup, NULL, len, restore, next, skip_b, skip_r, id, module);
}

void * regdma_link_new_branch_addr_map_default(void *backup, uint32_t bitmap[4], int len, void *restore, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    return regdma_link_new_branch_addr_map(backup, NULL, bitmap, len, restore, next, skip_b, skip_r, id, module);
}

void * regdma_link_new_branch_write_default(void *backup, uint32_t value, uint32_t mask, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    return regdma_link_new_branch_write(backup, value, mask, next, skip_b, skip_r, id, module);
}

void * regdma_link_new_branch_wait_default(void *backup, uint32_t value, uint32_t mask, regdma_entry_buf_t *next, bool skip_b, bool skip_r, int id, uint32_t module)
{
    return regdma_link_new_branch_wait(backup, value, mask, next, skip_b, skip_r, id, module);
}


static void * regdma_link_init_continuous_wrapper(const regdma_link_config_t *config, uint32_t module, int n, va_list args)
{
    regdma_entry_buf_t next;

    memset(next, 0, sizeof(regdma_entry_buf_t));
    for (int i = 0; i < n && i < REGDMA_LINK_ENTRY_NUM; i++) { // Ignore more arguments
        next[i] = va_arg(args, void *);
    }
    return regdma_link_new_continuous_default((void *)(config->continuous.backup), config->head.length,
            (void *)(config->continuous.restore), next[0], config->head.skip_b,
                config->head.skip_r, config->id, module);
}

static void * regdma_link_init_addr_map_wrapper(const regdma_link_config_t *config, uint32_t module, int n, va_list args)
{
    regdma_entry_buf_t next;

    memset(next, 0, sizeof(regdma_entry_buf_t));
    for (int i = 0; i < n && i < REGDMA_LINK_ENTRY_NUM; i++) { // Ignore more arguments
        next[i] = va_arg(args, void *);
    }
    return regdma_link_new_addr_map_default((void *)(config->addr_map.backup), (void *)(config->addr_map.map),
            config->head.length, (void *)(config->addr_map.restore), next[0], config->head.skip_b,
                config->head.skip_r, config->id, module);
}

static void * regdma_link_init_write_wrapper(const regdma_link_config_t *config, uint32_t module, int n, va_list args)
{
    regdma_entry_buf_t next;

    memset(next, 0, sizeof(regdma_entry_buf_t));
    for (int i = 0; i < n && i < REGDMA_LINK_ENTRY_NUM; i++) { // Ignore more arguments
        next[i] = va_arg(args, void *);
    }
    return regdma_link_new_write_default((void *)(config->write_wait.backup), config->write_wait.value,
            config->write_wait.mask, next[0], config->head.skip_b, config->head.skip_r,
                config->id, module);
}

static void * regdma_link_init_wait_wrapper(const regdma_link_config_t *config, uint32_t module, int n, va_list args)
{
    regdma_entry_buf_t next;

    memset(next, 0, sizeof(regdma_entry_buf_t));
    for (int i = 0; i < n && i < REGDMA_LINK_ENTRY_NUM; i++) { // Ignore more arguments
        next[i] = va_arg(args, void *);
    }
    return regdma_link_new_wait_default((void *)(config->write_wait.backup), config->write_wait.value,
            config->write_wait.mask, next[0], config->head.skip_b, config->head.skip_r,
                config->id, module);
}

static void * regdma_link_init_branch_continuous_wrapper(const regdma_link_config_t *config, uint32_t module, int n, va_list args)
{
    regdma_entry_buf_t next;

    memset(next, 0, sizeof(regdma_entry_buf_t));
    for (int i = 0; i < n && i < REGDMA_LINK_ENTRY_NUM; i++) { // Ignore more arguments
        next[i] = va_arg(args, void *);
    }
    return regdma_link_new_branch_continuous_default((void *)(config->continuous.backup),
            config->head.length, (void *)(config->continuous.restore), &next,
                config->head.skip_b, config->head.skip_r, config->id, module);
}

static void * regdma_link_init_branch_addr_map_wrapper(const regdma_link_config_t *config, uint32_t module, int n, va_list args)
{
    regdma_entry_buf_t next;

    memset(next, 0, sizeof(regdma_entry_buf_t));
    for (int i = 0; i < n && i < REGDMA_LINK_ENTRY_NUM; i++) { // Ignore more arguments
        next[i] = va_arg(args, void *);
    }
    return regdma_link_new_branch_addr_map_default((void *)(config->addr_map.backup),
            (void *)(config->addr_map.map), config->head.length, (void *)(config->addr_map.restore),
                &next, config->head.skip_b, config->head.skip_r, config->id, module);
}

static void * regdma_link_init_branch_write_wrapper(const regdma_link_config_t *config, uint32_t module, int n, va_list args)
{
    regdma_entry_buf_t next;

    memset(next, 0, sizeof(regdma_entry_buf_t));
    for (int i = 0; i < n && i < REGDMA_LINK_ENTRY_NUM; i++) { // Ignore more arguments
        next[i] = va_arg(args, void *);
    }
    return regdma_link_new_branch_write_default((void *)(config->write_wait.backup),
            config->write_wait.value, config->write_wait.mask, &next, config->head.skip_b,
                config->head.skip_r, config->id, module);
}

static void * regdma_link_init_branch_wait_wrapper(const regdma_link_config_t *config, uint32_t module, int n, va_list args)
{
    regdma_entry_buf_t next;

    memset(next, 0, sizeof(regdma_entry_buf_t));
    for (int i = 0; i < n && i < REGDMA_LINK_ENTRY_NUM; i++) { // Ignore more arguments
        next[i] = va_arg(args, void *);
    }
    return regdma_link_new_branch_wait_default((void *)(config->write_wait.backup),
            config->write_wait.value, config->write_wait.mask, &next, config->head.skip_b,
                config->head.skip_r, config->id, module);
}

static void * regdma_link_init_wrapper(const regdma_link_config_t *config, bool branch, uint32_t module, int nentry, va_list args)
{
    typedef void * (*init_fn_t)(const void *, uint32_t, int, va_list);

    const static init_fn_t initfn[] = {
        [0] = (init_fn_t)regdma_link_init_continuous_wrapper,  /* REGDMA_LINK_MODE_CONTINUOUS */
        [1] = (init_fn_t)regdma_link_init_addr_map_wrapper,    /* REGDMA_LINK_MODE_ADDR_MAP */
        [2] = (init_fn_t)regdma_link_init_write_wrapper,       /* REGDMA_LINK_MODE_WRITE */
        [3] = (init_fn_t)regdma_link_init_wait_wrapper         /* REGDMA_LINK_MODE_WAIT */
    };
    const static init_fn_t initfn_b[] = {
        [0] = (init_fn_t)regdma_link_init_branch_continuous_wrapper,
        [1] = (init_fn_t)regdma_link_init_branch_addr_map_wrapper,
        [2] = (init_fn_t)regdma_link_init_branch_write_wrapper,
        [3] = (init_fn_t)regdma_link_init_branch_wait_wrapper
    };

    assert((config->head.mode < ARRAY_SIZE(initfn)) && (config->head.mode < ARRAY_SIZE(initfn_b)));

    init_fn_t pfn = branch ? initfn_b[config->head.mode] : initfn[config->head.mode];
    return (*pfn)(config, module, nentry, args);
}

void * regdma_link_init(const regdma_link_config_t *config, bool branch, uint32_t module, int nentry, ...)
{
    assert(config != NULL);

    va_list args;
    va_start(args, nentry);
    void * link = regdma_link_init_wrapper(config, branch, module, nentry, args);
    va_end(args);
    return link;
}

static void * regdma_link_get_next_continuous_wrapper(void *link)
{
    regdma_link_continuous_t *continuous = __containerof(link, regdma_link_continuous_t, head);
    return (void *)(continuous->body.next);
}

static void * regdma_link_get_next_addr_map_wrapper(void *link)
{
    regdma_link_addr_map_t *addr_map = __containerof(link, regdma_link_addr_map_t, head);
    return (void *)(addr_map->body.next);
}

static void * regdma_link_get_next_write_wait_wrapper(void *link)
{
    regdma_link_write_wait_t *write_wait = __containerof(link, regdma_link_write_wait_t, head);
    return (void *)(write_wait->body.next);
}

static regdma_entry_buf_t * regdma_link_get_next_branch_continuous_wrapper(void *link)
{
    regdma_link_branch_continuous_t *branch_continuous = __containerof(link, regdma_link_branch_continuous_t, head);
    return &branch_continuous->body.next;
}

static regdma_entry_buf_t * regdma_link_get_next_branch_addr_map_wrapper(void *link)
{
    regdma_link_branch_addr_map_t *branch_addr_map = __containerof(link, regdma_link_branch_addr_map_t, head);
    return &branch_addr_map->body.next;
}

static regdma_entry_buf_t * regdma_link_get_next_branch_write_wait_wrapper(void *link)
{
    regdma_link_branch_write_wait_t *branch_write_wait = __containerof(link, regdma_link_branch_write_wait_t, head);
    return &branch_write_wait->body.next;
}

static void * regdma_link_get_next(void *link, int entry)
{
    if (link) {
        regdma_link_head_t head = REGDMA_LINK_HEAD(link);
        if (head.branch) {
            typedef regdma_entry_buf_t * (*get_nextfn1_t)(void *);
            const static get_nextfn1_t nextfn1[] = {
                [0] = (get_nextfn1_t)regdma_link_get_next_branch_continuous_wrapper,
                [1] = (get_nextfn1_t)regdma_link_get_next_branch_addr_map_wrapper,
                [2] = (get_nextfn1_t)regdma_link_get_next_branch_write_wait_wrapper,
                [3] = (get_nextfn1_t)regdma_link_get_next_branch_write_wait_wrapper
            };
            assert(head.mode < ARRAY_SIZE(nextfn1));
            regdma_entry_buf_t *next = (*nextfn1[head.mode])(link);
            if ((entry < REGDMA_LINK_ENTRY_NUM) && (*next)[entry] && (head.eof == 0)) {
                return (*next)[entry];
            }
        } else {
            typedef void * (*get_nextfn0_t)(void *);
            const static get_nextfn0_t nextfn0[] = {
                [0] = (get_nextfn0_t)regdma_link_get_next_continuous_wrapper,
                [1] = (get_nextfn0_t)regdma_link_get_next_addr_map_wrapper,
                [2] = (get_nextfn0_t)regdma_link_get_next_write_wait_wrapper,
                [3] = (get_nextfn0_t)regdma_link_get_next_write_wait_wrapper
            };
            assert(head.mode < ARRAY_SIZE(nextfn0));
            void *next = (*nextfn0[head.mode])(link);
            if (next && (head.eof == 0)) {
                return next;
            }
        }
    }
    return NULL;
}

static void * regdma_link_recursive_impl(void *link, int entry, int depth, void (*hook)(void *, int, int))
{
    assert(entry < REGDMA_LINK_ENTRY_NUM);

    if (link) {
        regdma_link_recursive_impl(regdma_link_get_next(link, entry), entry, depth+1, hook);
        if (hook) {
            (*hook)(link, entry, depth);
        }
    }
    return link;
}

void * regdma_link_recursive(void *link, int entry, void (*hook)(void *, int, int))
{
    return regdma_link_recursive_impl(link, entry, 0, hook);
}

static void * regdma_link_get_instance(void *link)
{
    void * container_memaddr[] = {
        (void *)__containerof(link, regdma_link_continuous_t, head),
        (void *)__containerof(link, regdma_link_addr_map_t, head),
        (void *)__containerof(link, regdma_link_write_wait_t, head),
        (void *)__containerof(link, regdma_link_write_wait_t, head),
        (void *)__containerof(link, regdma_link_branch_continuous_t, head),
        (void *)__containerof(link, regdma_link_branch_addr_map_t, head),
        (void *)__containerof(link, regdma_link_branch_write_wait_t, head),
        (void *)__containerof(link, regdma_link_branch_write_wait_t, head)
    };
    regdma_link_head_t head = REGDMA_LINK_HEAD(link);
    int it = (head.branch << 2) | head.mode;
    assert(it < ARRAY_SIZE(container_memaddr));

    return container_memaddr[it];
}
static regdma_link_stats_t * regdma_link_get_stats(void *link)
{
    const static size_t stats_offset[] = {
        offsetof(regdma_link_continuous_t, stat),
        offsetof(regdma_link_addr_map_t, stat),
        offsetof(regdma_link_write_wait_t, stat),
        offsetof(regdma_link_write_wait_t, stat),
        offsetof(regdma_link_branch_continuous_t, stat),
        offsetof(regdma_link_branch_addr_map_t, stat),
        offsetof(regdma_link_branch_write_wait_t, stat),
        offsetof(regdma_link_branch_write_wait_t, stat)
    };
    regdma_link_head_t head = REGDMA_LINK_HEAD(link);
    int it = (head.branch << 2) | head.mode;
    assert(it < ARRAY_SIZE(stats_offset));

    return (regdma_link_stats_t *)(regdma_link_get_instance(link) + stats_offset[it]);
}

static void regdma_link_update_stats_wrapper(void *link, int entry, int depth)
{
    if (link == NULL) {
        return;
    }
    regdma_link_update_stats(regdma_link_get_stats(link), entry, depth);
}

void regdma_link_stats(void *link, int entry)
{
    regdma_link_recursive_impl(link, entry, 0, regdma_link_update_stats_wrapper);
}

static void regdma_link_destroy_wrapper(void *link, int entry, int depth)
{
    if (link == NULL) {
        return;
    }
    regdma_link_stats_t *stat = regdma_link_get_stats(link);
    stat->ref &= ~BIT(entry);
    if (stat->ref == 0) {
        free(regdma_link_get_instance(link));
    }
}

void regdma_link_destroy(void *link, int entry)
{
    regdma_link_recursive_impl(link, entry, 0, regdma_link_destroy_wrapper);
}

void * regdma_find_link_by_pos(void *link, int entry, int pos)
{
    assert(entry < REGDMA_LINK_ENTRY_NUM);

    void *next = link;
    if (link) {
        int iter = 0;
        do {
            if (pos == iter++) {
                break;
            }
        } while ((next = regdma_link_get_next(next, entry)) != NULL);
    }
    return next;
}

void * regdma_find_link_by_id(void *link, int entry, int id)
{
    assert(entry < REGDMA_LINK_ENTRY_NUM);

    void *find_addr = NULL;
    void *next = link;
    if (link) {
        int linkid = 0;
        do {
            regdma_link_head_t head = REGDMA_LINK_HEAD(next);
            if (head.branch) {
                regdma_link_branch_continuous_t *continuous = (regdma_link_branch_continuous_t *)regdma_link_get_instance(next);
                linkid = continuous->stat.id;
            } else {
                regdma_link_continuous_t *continuous = (regdma_link_continuous_t *)regdma_link_get_instance(next);
                linkid = continuous->stat.id;
            }
            if (linkid == id) {
                find_addr = next;
                break;
            }
        } while ((next = regdma_link_get_next(next, entry)) != NULL);
    }
    return find_addr;
}

void regdma_link_set_write_wait_content(void *link, uint32_t value, uint32_t mask)
{
    if (link) {
        regdma_link_head_t head = REGDMA_LINK_HEAD(link);
        if (head.mode == REGDMA_LINK_MODE_WRITE || head.mode == REGDMA_LINK_MODE_WAIT) {
            if (head.branch) {
                regdma_link_branch_write_wait_t *write_wait = (regdma_link_branch_write_wait_t *)regdma_link_get_instance(link);
                write_wait->body.value = value;
                write_wait->body.mask = mask;
            } else {
                regdma_link_write_wait_t *write_wait = (regdma_link_write_wait_t *)regdma_link_get_instance(link);
                write_wait->body.value = value;
                write_wait->body.mask = mask;
            }
        }
    }
}

static void regdma_link_update_continuous_next_wrapper(void *link, void *next)
{
    regdma_link_continuous_t *continuous = __containerof(link, regdma_link_continuous_t, head);
    continuous->body.next = next;
}

static void regdma_link_update_addr_map_next_wrapper(void *link, void *next)
{
    regdma_link_addr_map_t *addr_map = __containerof(link, regdma_link_addr_map_t, head);
    addr_map->body.next = next;
}

static void regdma_link_update_write_wait_next_wrapper(void *link, void *next)
{
    regdma_link_write_wait_t *write_wait = __containerof(link, regdma_link_write_wait_t, head);
    write_wait->body.next = next;
}

static void regdma_link_update_branch_continuous_next_wrapper(void *link, regdma_entry_buf_t *next)
{
    regdma_link_branch_continuous_t *branch_continuous = __containerof(link, regdma_link_branch_continuous_t, head);
    for (int i = 0; i < REGDMA_LINK_ENTRY_NUM; i++) {
        branch_continuous->body.next[i] = (*next)[i];
    }
}

static void regdma_link_update_branch_addr_map_next_wrapper(void *link, regdma_entry_buf_t *next)
{
    regdma_link_branch_addr_map_t *branch_addr_map = __containerof(link, regdma_link_branch_addr_map_t, head);
    for (int i = 0; i < REGDMA_LINK_ENTRY_NUM; i++) {
        branch_addr_map->body.next[i] = (*next)[i];
    }
}

static void regdma_link_update_branch_write_wait_next_wrapper(void *link, regdma_entry_buf_t *next)
{
    regdma_link_branch_write_wait_t *branch_write_wait = __containerof(link, regdma_link_branch_write_wait_t, head);
    for (int i = 0; i < REGDMA_LINK_ENTRY_NUM; i++) {
        branch_write_wait->body.next[i] = (*next)[i];
    }
}

void regdma_link_update_next(void *link, int nentry, ...)
{
    va_list args;
    va_start(args, nentry);
    if (link) {
        regdma_entry_buf_t next;
        memset(next, 0, sizeof(regdma_entry_buf_t));
        for (int i = 0; i < nentry && i < REGDMA_LINK_ENTRY_NUM; i++) { // Ignore more arguments
            next[i] = va_arg(args, void *);
        }

        regdma_link_head_t head = REGDMA_LINK_HEAD(link);
        if (head.branch) {
            typedef void (*update_branch_fn_t)(void *, regdma_entry_buf_t *);
            static const update_branch_fn_t updatefn_b[] = {
                [0] = regdma_link_update_branch_continuous_next_wrapper,
                [1] = regdma_link_update_branch_addr_map_next_wrapper,
                [2] = regdma_link_update_branch_write_wait_next_wrapper,
                [3] = regdma_link_update_branch_write_wait_next_wrapper
            };
            assert((head.mode < ARRAY_SIZE(updatefn_b)));
            (*updatefn_b[head.mode])(link, &next);
        } else {
            typedef void (*update_fn_t)(void *, void *);
            static const update_fn_t updatefn[] = {
                [0] = regdma_link_update_continuous_next_wrapper,
                [1] = regdma_link_update_addr_map_next_wrapper,
                [2] = regdma_link_update_write_wait_next_wrapper,
                [3] = regdma_link_update_write_wait_next_wrapper
            };
            assert((head.mode < ARRAY_SIZE(updatefn)));
            (*updatefn[head.mode])(link, next[0]);
        }
    }
    va_end(args);
}

uint32_t regdma_link_get_owner_bitmap(void *link, void *tail, int entry)
{
    assert(entry < REGDMA_LINK_ENTRY_NUM);

    uint32_t owner = 0;
    void *next = link;
    if (link) {
        do {
            owner |= regdma_link_get_stats(next)->ref;
            if (next == tail) {
                break;
            }
        } while ((next = regdma_link_get_next(next, entry)) != NULL);
    }
    return owner;
}

void * regdma_find_module_link_head(void *link, void *tail, int entry, uint32_t module)
{
    assert(entry < REGDMA_LINK_ENTRY_NUM);

    void *find_link = NULL;
    void *next = link;
    if (link) {
        do {
            if (regdma_link_get_stats(next)->module & module) {
                find_link = next;
                break;
            }
            if (next == tail) {
                break;
            }
        } while ((next = regdma_link_get_next(next, entry)) != NULL);
    }
    return find_link;
}

void * regdma_find_module_link_tail(void *link, void *tail, int entry, uint32_t module)
{
    assert(entry < REGDMA_LINK_ENTRY_NUM);

    void *find_tail = NULL;
    void *next = link;
    if (link) {
        do {
            if (next != tail) {
                void *temp = regdma_link_get_next(next, entry);
                if ((regdma_link_get_stats(next)->module & module) &&
                   !(regdma_link_get_stats(temp)->module & module)) {
                    find_tail = next;
                    break;
                }
            } else {
                if (regdma_link_get_stats(next)->module & module) {
                    find_tail = next;
                    break;
                }
            }
        } while ((next = regdma_link_get_next(next, entry)) != NULL);
    }
    return find_tail;
}

void * regdma_find_next_module_link_head(void *link, void *tail, int entry, uint32_t module)
{
    assert(entry < REGDMA_LINK_ENTRY_NUM);
    void *find_tail = regdma_find_module_link_tail(link, tail, entry, module);
    if (find_tail && find_tail != tail) {
        return regdma_link_get_next(find_tail, entry);
    }
    return NULL;
}

void * regdma_find_prev_module_link_tail(void *link, void *tail, int entry, uint32_t module)
{
    assert(entry < REGDMA_LINK_ENTRY_NUM);
    void *find_head = regdma_find_module_link_head(link, tail, entry, module);
    void *next = link;
    if (find_head && find_head != link) {
        do {
            if (regdma_link_get_next(next, entry) == find_head) {
                return next;
            }
            if (next == tail) {
                break;
            }
        } while ((next = regdma_link_get_next(next, entry)) != NULL);
    }
    return NULL;
}

#if REGDMA_LINK_DBG
static const char *TAG = "regdma_link";

static void print_info_continuous_wrapper(void *link)
{
    regdma_link_head_t head = REGDMA_LINK_HEAD(link);
    regdma_link_continuous_t *cons = __containerof(link, regdma_link_continuous_t, head);
    ESP_EARLY_LOGI(TAG, "[%08x/%04x] link:%x, head:%x, next:%x, backup:%x, restore:%x, buff:%x",
             cons->stat.module, cons->stat.id, link, cons->head, cons->body.next,
             cons->body.backup, cons->body.restore, cons->body.mem);
    ESP_LOG_BUFFER_HEX(TAG, (const void *)cons->body.mem, head.length);
}

static void print_info_addr_map_wrapper(void *link)
{
    regdma_link_head_t head = REGDMA_LINK_HEAD(link);
    regdma_link_addr_map_t *map = __containerof(link, regdma_link_addr_map_t, head);
    ESP_EARLY_LOGI(TAG, "[%08x/%04x] link:%x, head:%x, next:%x, backup:%x, restore:%x, buff:%x, map:{%x,%x,%x,%x}",
            map->stat.module, map->stat.id, link, map->head, map->body.next, map->body.backup,
            map->body.restore, map->body.mem, map->body.map[0], map->body.map[1],
            map->body.map[2], map->body.map[3]);
    ESP_LOG_BUFFER_HEX(TAG, (const void *)map->body.mem, head.length);
}

static void print_info_write_wait_wrapper(void *link)
{
    regdma_link_write_wait_t *ww = __containerof(link, regdma_link_write_wait_t, head);
    ESP_EARLY_LOGI(TAG, "[%08x/%04x] link:%x, head:%x, next:%x, backup:%x, value:%x, mask:%x",
            ww->stat.module, ww->stat.id, link, ww->head, ww->body.next,
            ww->body.backup, ww->body.value, ww->body.mask);
}

static void print_info_branch_continuous_wrapper(void *link)
{
    regdma_link_head_t head = REGDMA_LINK_HEAD(link);
    regdma_link_branch_continuous_t *cons = __containerof(link, regdma_link_branch_continuous_t, head);
    ESP_EARLY_LOGI(TAG, "[%08x/%04x] link:%x, head:%x, next:{%x,%x,%x,%x}, backup:%x, restore:%x, buff:%x",
            cons->stat.module, cons->stat.id, link, cons->head, cons->body.next[0], cons->body.next[1],
            cons->body.next[2], cons->body.next[3], cons->body.backup, cons->body.restore,
            cons->body.mem);
    ESP_LOG_BUFFER_HEX(TAG, (const void *)cons->body.mem, head.length);
}

static void print_info_branch_addr_map_wrapper(void *link)
{
    regdma_link_head_t head = REGDMA_LINK_HEAD(link);
    regdma_link_branch_addr_map_t *map = __containerof(link, regdma_link_branch_addr_map_t, head);
    ESP_EARLY_LOGI(TAG, "[%08x/%04x] link:%x, head:%x, next:{%x,%x,%x,%x}, backup:%x, restore:%x, buff:%x, map:{%x,%x,%x,%x}",
            map->stat.module, map->stat.id, link, map->head, map->body.next[0], map->body.next[1], map->body.next[2],
            map->body.next[3], map->body.backup, map->body.restore, map->body.mem, map->body.map[0],
            map->body.map[1], map->body.map[2], map->body.map[3]);
    ESP_LOG_BUFFER_HEX(TAG, (const void *)map->body.mem, head.length);
}

static void print_info_branch_write_wait_wrapper(void *link)
{
    regdma_link_branch_write_wait_t *ww = __containerof(link, regdma_link_branch_write_wait_t, head);
    ESP_EARLY_LOGI(TAG, "[%08x/%04x] link:%x, head:%x, next:{%x,%x,%x,%x}, backup:%x, value:%x, mask:%x",
            ww->stat.module, ww->stat.id, link, ww->head, ww->body.next[0], ww->body.next[1],
            ww->body.next[2], ww->body.next[3], ww->body.backup, ww->body.value,
            ww->body.mask);
}

static void print_link_info(void *args, int entry, int depth)
{
    typedef void (*prinf_fn_t)(void *);

    const static prinf_fn_t prinf_fn[] = {
        [0] = (prinf_fn_t)print_info_continuous_wrapper,
        [1] = (prinf_fn_t)print_info_addr_map_wrapper,
        [2] = (prinf_fn_t)print_info_write_wait_wrapper,
        [3] = (prinf_fn_t)print_info_write_wait_wrapper,
        [4] = (prinf_fn_t)print_info_branch_continuous_wrapper,
        [5] = (prinf_fn_t)print_info_branch_addr_map_wrapper,
        [6] = (prinf_fn_t)print_info_branch_write_wait_wrapper,
        [7] = (prinf_fn_t)print_info_branch_write_wait_wrapper
    };

    regdma_link_head_t head = REGDMA_LINK_HEAD(args);
    int it = (head.branch << 2) | head.mode;
    assert(it < ARRAY_SIZE(prinf_fn));

    (*prinf_fn[it])(args);
}

void regdma_link_show_memories(void *link, int entry)
{
    assert(entry < REGDMA_LINK_ENTRY_NUM);

    void *next = link;
    if (link) {
        do {
            print_link_info(next, entry, 0);
        } while ((next = regdma_link_get_next(next, entry)) != NULL);
    } else {
        ESP_EARLY_LOGW(TAG, "This REGDMA linked list is empty!\n");
    }
}
#endif
