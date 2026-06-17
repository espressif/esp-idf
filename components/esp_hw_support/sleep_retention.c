/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <sys/lock.h>
#include <sys/param.h>

#include "esp_err.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_sleep.h"
#include "soc/soc_caps.h"
#include "esp_private/esp_regdma.h"
#include "esp_private/esp_pau.h"
#include "esp_private/sleep_retention.h"
#include "sdkconfig.h"
#include "esp_pmu.h"

#if SOC_PM_PAU_REGDMA_UPDATE_CACHE_BEFORE_WAIT_COMPARE
#include "soc/pmu_reg.h" // for PMU_DATE_REG, it can provide full 32 bit read and write access
#endif
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#include "hal/cache_ll.h"
#endif

ESP_LOG_ATTR_TAG(TAG, "sleep");
static int acquire_cnt; //for the force acquire lock


struct sleep_retention_module_object {
    sleep_retention_module_callbacks_t cbs;             /* A callback list that can extend more sleep retention event callbacks */
    sleep_retention_module_bitmap_t    dependents;      /* A bitmap identifying all modules that the current module depends on */
    union {
        sleep_retention_module_bitmap_t    references;  /* A bitmap indicating all other modules that depend on (or reference)
                                                         * the current module, It will update at runtime (allocate or free)
                                                         * based on whether the module is referenced by other modules */
        sleep_retention_module_bitmap_t    refarray[2]; /* Bitmap array to indicating all other modules that depend on (or
                                                         * reference) the current module, It will update at runtime (allocate/
                                                         * attach or free/detach) based on whether the module is referenced by
                                                         * other modules, refarray[0] is equivalent to references and is used to
                                                         * indicate allocate/free operations, and refarray[1] is used to indicate
                                                         * attach/detach operations.*/
    };
    sleep_retention_module_attribute_t attributes;  /* A bitmap indicating attribute of the current module */
};

static inline void sleep_retention_module_object_ctor(struct sleep_retention_module_object * const self, sleep_retention_module_callbacks_t *cbs)
{
    self->cbs = *cbs;
    self->dependents = (sleep_retention_module_bitmap_t){ .bitmap = { 0 } };
    self->references = (sleep_retention_module_bitmap_t){ .bitmap = { 0 } };
    self->refarray[1] = (sleep_retention_module_bitmap_t){ .bitmap = { 0 } };
    self->attributes = 0;
}

static inline void sleep_retention_module_object_dtor(struct sleep_retention_module_object * const self)
{
    self->cbs = (sleep_retention_module_callbacks_t) { .create = { .handle = NULL, .arg = NULL } };
}

static inline void set_dependencies(struct sleep_retention_module_object * const self, sleep_retention_module_bitmap_t depends)
{
    self->dependents = depends;
}

static inline void clr_dependencies(struct sleep_retention_module_object * const self)
{
    self->dependents = (sleep_retention_module_bitmap_t){ .bitmap = { 0 } };
}

static inline sleep_retention_module_bitmap_t get_dependencies(struct sleep_retention_module_object * const self)
{
    return self->dependents;
}

static inline void set_reference(struct sleep_retention_module_object * const self, sleep_retention_module_t module)
{
    self->references.bitmap[module >> 5] |= BIT(module % 32);
}

static inline void clr_reference(struct sleep_retention_module_object * const self, sleep_retention_module_t module)
{
    self->references.bitmap[module >> 5] &= ~BIT(module % 32);
}

static inline sleep_retention_module_bitmap_t get_references(struct sleep_retention_module_object * const self)
{
    return self->references;
}

static inline void refarray_set_bit(struct sleep_retention_module_object * const self, int n, sleep_retention_module_t module)
{
    if (n >= 0 && n < ARRAY_SIZE(self->refarray)) {
        self->refarray[n].bitmap[module >> 5] |= BIT(module % 32);
    }
}

static inline void refarray_clr_bit(struct sleep_retention_module_object * const self, int n, sleep_retention_module_t module)
{
    if (n >= 0 && n < ARRAY_SIZE(self->refarray)) {
        self->refarray[n].bitmap[module >> 5] &= ~BIT(module % 32);
    }
}

static inline sleep_retention_module_bitmap_t refarray_get(struct sleep_retention_module_object * const self, int n)
{
    if (n >= 0 && n < ARRAY_SIZE(self->refarray)) {
        return self->refarray[n];
    }
    return (sleep_retention_module_bitmap_t){ .bitmap = { 0 } };
}

static inline bool refarray_zero(struct sleep_retention_module_object * const self, int n)
{
    if (n >= 0 && n < ARRAY_SIZE(self->refarray)) {
        uint32_t val = 0;
        sleep_retention_module_bitmap_t map = refarray_get(self, n);
        for (int i = 0; i < SLEEP_RETENTION_MODULE_BITMAP_SZ; i++) {
            val |= map.bitmap[i];
        }
        return (val == 0);
    }
    return false;
}

static inline bool references_exist(struct sleep_retention_module_object * const self)
{
    uint32_t references = 0;
    sleep_retention_module_bitmap_t map = get_references(self);
    for (int i = 0; i < SLEEP_RETENTION_MODULE_BITMAP_SZ; i++) {
        references |= map.bitmap[i];
    }
    return (references != 0);
}

static inline void set_attributes(struct sleep_retention_module_object * const self, sleep_retention_module_attribute_t attributes)
{
    self->attributes = attributes;
}

static inline void clr_attributes(struct sleep_retention_module_object * const self)
{
    self->attributes = 0;
}

static inline sleep_retention_module_attribute_t get_attributes(struct sleep_retention_module_object * const self)
{
    return self->attributes;
}

static inline bool module_runtime_attach(struct sleep_retention_module_object * const self)
{
    if (self) {
        return (get_attributes(self) & SLEEP_RETENTION_MODULE_ATTR_ATTACH) ? true : false;
    }
    return false; /* for SLEEP_RETENTION_MODULE_INVALID */
}

static inline bool module_is_passive(struct sleep_retention_module_object * const self)
{
    return (get_attributes(self) & SLEEP_RETENTION_MODULE_ATTR_PASSIVE) ? true : false;
}

/**
 * Internal structure which holds all requested sleep retention parameters
 */
struct module_sleep_retention_context {
#define SLEEP_RETENTION_REGDMA_LINK_NR_PRIORITIES       (8u)
#define SLEEP_RETENTION_REGDMA_LINK_HIGHEST_PRIORITY    (0)
#define SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY     (SLEEP_RETENTION_REGDMA_LINK_NR_PRIORITIES - 1)
#define SLEEP_RETENTION_MODULE_INVALID                  ((sleep_retention_module_t)(-1)) /* the final node does not belong to any module */
    struct {
        sleep_retention_entries_t entries;
        uint32_t entries_bitmap: REGDMA_LINK_ENTRY_NUM;
        uint32_t runtime_bitmap: REGDMA_LINK_ENTRY_NUM;
#if REGDMA_LINK_ENTRY_NUM < 16
        uint32_t reserved: 32-(2*REGDMA_LINK_ENTRY_NUM);
#endif
        void *entries_tail;
    } lists[SLEEP_RETENTION_REGDMA_LINK_NR_PRIORITIES];
};

typedef struct {
    /* The hardware retention module (REGDMA and PMU) uses 4 linked lists to
     * record the hardware context information that needs to be backed up and
     * restored when switching between different power states. The 4 linked
     * lists are linked by 8 types of nodes. The 4 linked lists can reuse some
     * nodes with each other, or separate their own unique nodes after branch
     * type nodes.
     * The REGDMA module iterates the entire linked list from the head of a
     * linked list and backs up and restores the corresponding register context
     * information according to the configuration information of the linked list
     * nodes.
     * The PMU module triggers REGDMA to use the corresponding linked list when
     * switching between different power states. For example:
     *
     * +---------------+---------------+-------------------+-----------+
     * |    Current    |   The next    | The entry will be | Retention |
     * |   PMU state   |   PMU state   |  used by REGDMA   |   clock   |
     * +---------------+---------------+-------------------+-----------+
     * | PMU_HP_ACTIVE | PMU_HP_SLEEP  |     entry0        |    XTAL   |
     * | PMU_HP_SLEEP  | PMU_HP_ACTIVE |     entry0        |    XTAL   |
     * | PMU_HP_MODEM  | PMU_HP_SLEEP  |     ------        |    XTAL   |
     * | PMU_HP_SLEEP  | PMU_HP_MODEM  |     entry1        |    XTAL   |
     * | PMU_HP_MODEM  | PMU_HP_ACTIVE |     entry2        |    PLL    |
     * |---------------------------------------------------------------|
     * | PMU_HP_ACTIVE | PMU_HP_ACTIVE |     entry3        |    PLL    | (Clock BUG)
     * +---------------+---------------+-------------------+-----------+
     *
     *           +--------+    +-------------------------+    +-------------+                            +-----------+    +--------+    +-----+
     * entry2 -> |        | -> | WiFi MAC Minimum System | -> |             | -------------------------> | ######### | -> | ###### | -> | End |
     *           |  SOC   |    +-------------------------+    |   Digital   |                            | Bluetooth |    | Zigbee |    +-----+
     *           | System |             +--------+            | Peripherals |    +------+    +------+    |   / BLE   |    |        |    +-----+
     * entry0 -> |        | ----------> |        | ---------> |             | -> |      | -> |      | -> |           | -> |        | -> | End |
     *           +--------+             | Modem  |            +-------------+    | WiFi |    | WiFi |    +-----------+    +--------+    +-----+
     *                                  | System |                               | MAC  |    |  BB  |    +-----+
     * entry1 ------------------------> |        |-----------------------------> |      | -> |      | -> | End |
     *                                  +--------+                               +------+    +------+    +-----+
     *
     * The entry3 (alias: extra linked list) is used for backup and restore of
     * modules (such as BLE or 15.4 modules) with retention clock bugs.
     *
     *           +---------+    +----------+    +-------------+    +-----+
     * entry3 -> | BLE MAC | -> | 15.4 MAC | -> | BLE/15.4 BB | -> | End |
     *           +---------+    +----------+    +-------------+    +-----+
     *
     * Using it (extra linked list) for retention has the following constraints:
     * 1. The PLL clock must be enabled (can be done with esp_pm_lock_acquire()
     *    interface to acquire a pm lock of type ESP_PM_APB_FREQ_MAX.
     * 2. When using the sleep_retention_entries_create() interface to create an
     *    extra linked list, the node owner must be equal to BIT(3).
     * 3. Use the sleep_retention_do_extra_retention() interface to backup or
     *    restore the register context, which ensures only one backup or restore
     *    when multiple modules (BLE and 15.4) exists.
     */
    union {
        struct module_sleep_retention_context retention;
        struct module_sleep_retention_context context[2];
    };
    _lock_t lock;
    regdma_link_priority_t highpri;
    sleep_retention_module_bitmap_t inited_modules;
    sleep_retention_module_bitmap_t created_modules;
    sleep_retention_module_bitmap_t retention_modules;

    void *final_default;

    struct sleep_retention_module_object instance[SLEEP_RETENTION_MODULE_MAX + 1];

#define EXTRA_LINK_NUM  (REGDMA_LINK_ENTRY_NUM - 1)
} sleep_retention_t;

static DRAM_ATTR __attribute__((unused)) sleep_retention_t s_retention = {
    .highpri = (uint8_t)-1,
    .inited_modules = (sleep_retention_module_bitmap_t){ .bitmap = { 0 } },
    .created_modules = (sleep_retention_module_bitmap_t){ .bitmap = { 0 } },
    .retention_modules = (sleep_retention_module_bitmap_t){ .bitmap = { 0 } },
    .final_default = NULL
};

#define SLEEP_RETENTION_ENTRY_BITMAP_MASK       (BIT(REGDMA_LINK_ENTRY_NUM) - 1)
#define SLEEP_RETENTION_ENTRY_BITMAP(bitmap)    ((bitmap) & SLEEP_RETENTION_ENTRY_BITMAP_MASK)

static esp_err_t entries_create(const sleep_retention_entries_config_t retent[], int num, regdma_link_priority_t priority, sleep_retention_module_t module);
static void retention_entries_join(void);

static struct sleep_retention_module_object * instance(sleep_retention_module_t module)
{
    return (module == SLEEP_RETENTION_MODULE_INVALID) ? NULL : &s_retention.instance[module];
}

static inline bool module_is_inited(sleep_retention_module_t module)
{
    sleep_retention_module_bitmap_t inited_modules = sleep_retention_get_inited_modules();
    return (inited_modules.bitmap[module >> 5] & BIT(module % 32)) ? true : false;
}

static inline bool module_is_created(sleep_retention_module_t module)
{
    sleep_retention_module_bitmap_t created_modules = sleep_retention_get_created_modules();
    return (created_modules.bitmap[module >> 5] & BIT(module % 32)) ? true : false;
}

static inline bool module_is_retained(sleep_retention_module_t module)
{
    if (module_runtime_attach(instance(module))) {
        sleep_retention_module_bitmap_t retained_modules = sleep_retention_get_retained_modules();
        return (retained_modules.bitmap[module >> 5] & BIT(module % 32)) ? true : false;
    } else {
        return false;
    }
}

static inline bool entries_require_branch(uint32_t owner, uint32_t runtime_bitmap)
{
    bool use_new_entry      = SLEEP_RETENTION_ENTRY_BITMAP(owner & ~runtime_bitmap) ? true : false;
    bool intersection_exist = SLEEP_RETENTION_ENTRY_BITMAP(owner &  runtime_bitmap) ? true : false;
    return use_new_entry && intersection_exist;
}

static esp_err_t check_and_create_default(uint32_t owner, uint32_t runtime_bitmap, uint32_t entries_bitmap, regdma_link_priority_t priority, sleep_retention_module_t module)
{
    assert(entries_require_branch(owner, runtime_bitmap));

    static sleep_retention_entries_config_t dummy = { REGDMA_LINK_WAIT_INIT(0xffff, 0, 0, 0, 1, 1), 0 };
    dummy.owner = SLEEP_RETENTION_ENTRY_BITMAP(owner & ~entries_bitmap);
    if (dummy.owner) {
        return entries_create(&dummy, 1, priority, module);
    }
    return ESP_OK;
}

static esp_err_t check_and_create_final_default(void)
{
    static const sleep_retention_entries_config_t final_dummy = { REGDMA_LINK_WAIT_INIT(0xffff, 0, 0, 0, 1, 1), SLEEP_RETENTION_ENTRY_BITMAP_MASK };

    esp_err_t err = ESP_OK;
    _lock_acquire_recursive(&s_retention.lock);
    if (s_retention.retention.lists[SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY].entries_bitmap == 0) {
        err = entries_create(&final_dummy, 1, SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY, SLEEP_RETENTION_MODULE_INVALID);
        if (err == ESP_OK) {
            s_retention.final_default = s_retention.retention.lists[SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY].entries[0];
        }
    }
    _lock_release_recursive(&s_retention.lock);
    return err;
}

static void entries_stats(struct module_sleep_retention_context *ctx)
{
    _lock_acquire_recursive(&s_retention.lock);
    for (regdma_link_priority_t priority = 0; priority < SLEEP_RETENTION_REGDMA_LINK_NR_PRIORITIES; priority++) {
        for (int entry = 0; entry < ARRAY_SIZE(ctx->lists[priority].entries); entry++) {
            regdma_link_stats(ctx->lists[priority].entries[entry], entry);
        }
    }
    _lock_release_recursive(&s_retention.lock);
}

static void entries_context_update(struct module_sleep_retention_context *ctx, uint32_t owner, void *new_link, regdma_link_priority_t priority)
{
    _lock_acquire_recursive(&s_retention.lock);
    sleep_retention_entries_t entries = {
        (owner & BIT(0)) ? new_link : ctx->lists[priority].entries[0],
        (owner & BIT(1)) ? new_link : ctx->lists[priority].entries[1],
        (owner & BIT(2)) ? new_link : ctx->lists[priority].entries[2],
        (owner & BIT(3)) ? new_link : ctx->lists[priority].entries[3]
#if (REGDMA_LINK_ENTRY_NUM == 5)
      , (owner & BIT(4)) ? new_link : ctx->lists[priority].entries[4]
#endif
    };
    if (ctx->lists[priority].entries_bitmap == 0) {
        ctx->lists[priority].entries_tail = new_link;
    }
    memcpy(ctx->lists[priority].entries, entries, sizeof(sleep_retention_entries_t));
    ctx->lists[priority].runtime_bitmap  = owner;
    ctx->lists[priority].entries_bitmap |= owner;
    entries_stats(ctx);
    _lock_release_recursive(&s_retention.lock);
}

static void * entries_try_create(const regdma_link_config_t *config, uint32_t owner, regdma_link_priority_t priority, sleep_retention_module_t module)
{
    void *link = NULL;
    assert(owner > 0 && owner < BIT(REGDMA_LINK_ENTRY_NUM));

    _lock_acquire_recursive(&s_retention.lock);
    int index = module_runtime_attach(instance(module)) ? 1 : 0;
    struct module_sleep_retention_context *ctx = &s_retention.context[index];
    uint32_t bm_runtime = ctx->lists[priority].runtime_bitmap;
    uint32_t bm_entries = ctx->lists[priority].entries_bitmap;
    if (entries_require_branch(owner, bm_runtime)) {
        /* branch node can't as tail node */
        esp_err_t err = check_and_create_default(owner, bm_runtime, bm_entries, priority, module);
        if (err == ESP_OK) {
            link = regdma_link_init_safe(config, true, module,
                        (owner & BIT(0)) ? ctx->lists[priority].entries[0] : NULL,
                        (owner & BIT(1)) ? ctx->lists[priority].entries[1] : NULL,
                        (owner & BIT(2)) ? ctx->lists[priority].entries[2] : NULL,
                        (owner & BIT(3)) ? ctx->lists[priority].entries[3] : NULL
#if (REGDMA_LINK_ENTRY_NUM == 5)
                      , (owner & BIT(4)) ? ctx->lists[priority].entries[4] : NULL
#endif
                    );
        }
    } else {
        link = regdma_link_init_safe(config, false, module, ctx->lists[priority].entries[__builtin_ffs(owner) - 1]);
    }
    _lock_release_recursive(&s_retention.lock);
    return link;
}

static void * entries_try_create_bonding(const regdma_link_config_t *config, uint32_t owner, regdma_link_priority_t priority, sleep_retention_module_t module)
{
    assert(owner > 0 && owner < BIT(REGDMA_LINK_ENTRY_NUM));
    _lock_acquire_recursive(&s_retention.lock);
    int index = module_runtime_attach(instance(module)) ? 1 : 0;
    struct module_sleep_retention_context *ctx = &s_retention.context[index];
    void *link = regdma_link_init_safe(config, true, module,
                (owner & BIT(0)) ? ctx->lists[priority].entries[0] : NULL,
                (owner & BIT(1)) ? ctx->lists[priority].entries[1] : NULL,
                (owner & BIT(2)) ? ctx->lists[priority].entries[2] : NULL,
                (owner & BIT(3)) ? ctx->lists[priority].entries[3] : NULL
#if (REGDMA_LINK_ENTRY_NUM == 5)
              , (owner & BIT(4)) ? ctx->lists[priority].entries[4] : NULL
#endif
            );
    _lock_release_recursive(&s_retention.lock);
    return link;
}

void sleep_retention_dump_modules(FILE *out)
{
    for (int i = SLEEP_RETENTION_MODULE_MIN; i <= SLEEP_RETENTION_MODULE_MAX; i++) {
        bool inited = sleep_retention_is_module_inited(i);
        bool created = sleep_retention_is_module_created(i);
        bool is_top = is_top_domain_module(i);

        const char* status = !inited? "-":
                        created? "CREATED":
                                "INITED";
        const char* domain = is_top? "TOP": "-";
        fprintf(out, "%2d: %4s %8s\n", i, domain, status);
    }
}

void sleep_retention_dump_entries(FILE *out)
{
    _lock_acquire_recursive(&s_retention.lock);
    if (s_retention.highpri >= SLEEP_RETENTION_REGDMA_LINK_HIGHEST_PRIORITY && s_retention.highpri <= SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY) {
        for (int entry = 0; entry < ARRAY_SIZE(s_retention.retention.lists[s_retention.highpri].entries); entry++) {
            fprintf(out, "\nsleep retention entries[%d] context:\n", entry);
            regdma_link_dump(out, s_retention.retention.lists[s_retention.highpri].entries[entry], entry);
        }
    }
    for (int n = 0; n < ARRAY_SIZE(s_retention.context); n++) {
        for (regdma_link_priority_t priority = 0; priority < SLEEP_RETENTION_REGDMA_LINK_NR_PRIORITIES; priority++) {
            for (int e = 0; e < ARRAY_SIZE(s_retention.context[0].lists[priority].entries); e++) {
                fprintf(out, "\nsleep retention context[%d] priority %d entries[%d] context:\n", n, priority, e);
                void *head = s_retention.context[n].lists[priority].entries[e];
                void *tail = s_retention.context[n].lists[priority].entries_tail;
                regdma_link_dump_sublink(out, head, tail, e);
            }
        }
    }
    fflush(out);
    _lock_release_recursive(&s_retention.lock);
}

void * sleep_retention_find_link_by_id(int id)
{
    void *link = NULL;
    _lock_acquire_recursive(&s_retention.lock);
    if (s_retention.highpri >= SLEEP_RETENTION_REGDMA_LINK_HIGHEST_PRIORITY &&
        s_retention.highpri <= SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY) {
        for (int entry = 0; (link == NULL && entry < ARRAY_SIZE(s_retention.retention.lists[s_retention.highpri].entries)); entry++) {
            link = regdma_find_link_by_id(s_retention.retention.lists[s_retention.highpri].entries[entry], entry, id);
        }
    }
    _lock_release_recursive(&s_retention.lock);
    return link;
}

static uint32_t entries_owner_bitmap(sleep_retention_entries_t *entries, sleep_retention_entries_t *tails)
{
    uint32_t owner = 0;
    _lock_acquire_recursive(&s_retention.lock);
    for (int entry = 0; entry < ARRAY_SIZE(*entries); entry++) {
        owner |= regdma_link_get_owner_bitmap((*entries)[entry], (*tails)[entry], entry);
    }
    _lock_release_recursive(&s_retention.lock);
    return owner;
}

static bool module_entries_get(struct module_sleep_retention_context *ctx, regdma_link_priority_t priority, sleep_retention_module_t module, sleep_retention_entries_t *entries, void **tail, sleep_retention_entries_t *next_entries, void **prev_tail)
{
    bool exist = false;
    sleep_retention_entries_t tails, prev_tails;

    memset(&tails, 0, sizeof(sleep_retention_entries_t));
    memset(&prev_tails, 0, sizeof(sleep_retention_entries_t));

    _lock_acquire_recursive(&s_retention.lock);
    for (int entry = 0; entry < ARRAY_SIZE(ctx->lists[priority].entries); entry++) {
        (*entries)[entry] = regdma_find_module_link_head(
                ctx->lists[priority].entries[entry],
                ctx->lists[priority].entries_tail,
                entry, module);
        tails     [entry] = regdma_find_module_link_tail(
                ctx->lists[priority].entries[entry],
                ctx->lists[priority].entries_tail,
                entry, module);
        (*next_entries)   [entry] = regdma_find_next_module_link_head(
                ctx->lists[priority].entries[entry],
                ctx->lists[priority].entries_tail,
                entry, module);
        prev_tails        [entry] = regdma_find_prev_module_link_tail(
                ctx->lists[priority].entries[entry],
                ctx->lists[priority].entries_tail,
                entry, module);
        if ((*entries)[entry] && tails[entry]) {
            exist = true;
        }
        assert(tails[entry] == tails[0]);
        assert(prev_tails[entry] == prev_tails[0]);
    }
    *tail = tails[0];
    *prev_tail = prev_tails[0];
    _lock_release_recursive(&s_retention.lock);
    return exist;
}

static void entries_context_refresh(struct module_sleep_retention_context *ctx, regdma_link_priority_t priority)
{
    _lock_acquire_recursive(&s_retention.lock);
    sleep_retention_entries_t tails = {
        ctx->lists[priority].entries_tail, ctx->lists[priority].entries_tail,
        ctx->lists[priority].entries_tail, ctx->lists[priority].entries_tail
#if (REGDMA_LINK_ENTRY_NUM == 5)
      , ctx->lists[priority].entries_tail
#endif
    };
    ctx->lists[priority].entries_bitmap = entries_owner_bitmap(&ctx->lists[priority].entries, &tails);
    ctx->lists[priority].runtime_bitmap = entries_owner_bitmap(&ctx->lists[priority].entries, &ctx->lists[priority].entries);
    _lock_release_recursive(&s_retention.lock);
}

static bool entries_detach(struct module_sleep_retention_context *ctx, regdma_link_priority_t priority,
        sleep_retention_entries_t *entries, void *tail, sleep_retention_entries_t *next_entries, void *prev_tail)
{
    _lock_acquire_recursive(&s_retention.lock);
    bool is_head = (memcmp(entries, ctx->lists[priority].entries, sizeof(sleep_retention_entries_t)) == 0);
    bool is_tail = (tail == ctx->lists[priority].entries_tail);

    if (is_head && is_tail) {
        memset(ctx->lists[priority].entries, 0, sizeof(sleep_retention_entries_t));
        ctx->lists[priority].entries_tail = NULL;
    } else if (is_head) {
        memcpy(ctx->lists[priority].entries, next_entries, sizeof(sleep_retention_entries_t));
    } else if (is_tail) {
        ctx->lists[priority].entries_tail = prev_tail;
#if (REGDMA_LINK_ENTRY_NUM == 5)
        regdma_link_update_next_safe(prev_tail, NULL, NULL, NULL, NULL, NULL);
#else
        regdma_link_update_next_safe(prev_tail, NULL, NULL, NULL, NULL);
#endif
    } else {
#if (REGDMA_LINK_ENTRY_NUM == 5)
        regdma_link_update_next_safe(prev_tail, (*next_entries)[0], (*next_entries)[1], (*next_entries)[2], (*next_entries)[3], (*next_entries)[4]);
#else
        regdma_link_update_next_safe(prev_tail, (*next_entries)[0], (*next_entries)[1], (*next_entries)[2], (*next_entries)[3]);
#endif
    }
    entries_context_refresh(ctx, priority);
#if (REGDMA_LINK_ENTRY_NUM == 5)
    regdma_link_update_next_safe(tail, NULL, NULL, NULL, NULL, NULL);
#else
    regdma_link_update_next_safe(tail, NULL, NULL, NULL, NULL);
#endif
    _lock_release_recursive(&s_retention.lock);
    return (is_head || is_tail);
}

static void entries_attach(struct module_sleep_retention_context *ctx, regdma_link_priority_t priority, sleep_retention_entries_t *entries, void *tail)
{
    _lock_acquire_recursive(&s_retention.lock);
    regdma_link_update_next_safe(tail, ctx->lists[priority].entries[0], ctx->lists[priority].entries[1],
            ctx->lists[priority].entries[2], ctx->lists[priority].entries[3]
#if (REGDMA_LINK_ENTRY_NUM == 5)
          , ctx->lists[priority].entries[4]
#endif
        );
    memcpy(ctx->lists[priority].entries, entries, sizeof(sleep_retention_entries_t));
    if (ctx->lists[priority].entries_tail == NULL) {
        ctx->lists[priority].entries_tail = tail;
    }
    entries_context_refresh(ctx, priority);
    _lock_release_recursive(&s_retention.lock);
}

static void module_entries_destroy(sleep_retention_entries_t *entries)
{
    for (int entry = 0; entry < ARRAY_SIZE(*entries); entry++) {
        regdma_link_destroy((*entries)[entry], entry);
    }
}

static void check_and_destroy_final_default(void)
{
    _lock_acquire_recursive(&s_retention.lock);
    assert(s_retention.highpri == SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY);
    uint32_t created_modules = 0;
    for (int i = 0; i < SLEEP_RETENTION_MODULE_BITMAP_SZ; i++) {
        created_modules |= s_retention.created_modules.bitmap[i];
    }
    assert(created_modules == 0);
    module_entries_destroy(&s_retention.retention.lists[SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY].entries);
    _lock_release_recursive(&s_retention.lock);
}

static void entries_do_destroy(sleep_retention_module_t module)
{
    void *tail = NULL, *prev_tail = NULL;
    sleep_retention_entries_t entries, next_entries;

    memset(&entries, 0, sizeof(sleep_retention_entries_t));
    memset(&next_entries, 0, sizeof(sleep_retention_entries_t));

    _lock_acquire_recursive(&s_retention.lock);
    int index = module_runtime_attach(instance(module)) ? 1 : 0;
    struct module_sleep_retention_context *ctx = &s_retention.context[index];
    regdma_link_priority_t priority = 0;
    do {
        bool exist = module_entries_get(ctx, priority, module, &entries, &tail, &next_entries, &prev_tail);
        if (exist) {
            if (entries_detach(ctx, priority, &entries, tail, &next_entries, prev_tail)) {
                if (!module_runtime_attach(instance(module))) {
                    retention_entries_join();
                }
            }
            module_entries_destroy(&entries);
        } else {
            priority++;
        }
    } while (priority < SLEEP_RETENTION_REGDMA_LINK_NR_PRIORITIES);
    s_retention.created_modules.bitmap[module >> 5] &= ~BIT(module % 32);
    _lock_release_recursive(&s_retention.lock);
}

static void entries_destroy(sleep_retention_module_t module)
{
    assert(SLEEP_RETENTION_MODULE_MIN <= module && module <= SLEEP_RETENTION_MODULE_MAX);
    _lock_acquire_recursive(&s_retention.lock);
    if (!module_runtime_attach(instance(module))) {
        retention_entries_join();
    }
    int index = module_runtime_attach(instance(module)) ? 1 : 0;
    entries_stats(&s_retention.context[index]);
    entries_do_destroy(module);
    _lock_release_recursive(&s_retention.lock);
}

static void sleep_retention_entries_destroy(sleep_retention_module_t module)
{
    assert(SLEEP_RETENTION_MODULE_MIN <= module && module <= SLEEP_RETENTION_MODULE_MAX);
    _lock_acquire_recursive(&s_retention.lock);
    entries_destroy(module);
    uint32_t created_modules = 0;
    for (int i = 0; i < SLEEP_RETENTION_MODULE_BITMAP_SZ; i++) {
        created_modules |= s_retention.created_modules.bitmap[i];
    }
    if (created_modules == 0) {
        check_and_destroy_final_default();
#if SOC_LIGHT_SLEEP_SUPPORTED
        pmu_sleep_disable_regdma_backup();
#endif
        memset((void *)s_retention.context, 0, sizeof(struct module_sleep_retention_context) * 2);
        s_retention.highpri = (uint8_t)-1;
        s_retention.final_default = NULL;
    }
    _lock_release_recursive(&s_retention.lock);
}

static esp_err_t entries_create(const sleep_retention_entries_config_t retent[], int num, regdma_link_priority_t priority, sleep_retention_module_t module)
{
    esp_err_t err = ESP_OK;
    _lock_acquire_recursive(&s_retention.lock);
    for (int i = num - 1; (i >= 0) && (err == ESP_OK); i--) {
#if SOC_PM_RETENTION_HAS_CLOCK_BUG
        if ((retent[i].owner > BIT(EXTRA_LINK_NUM)) && (retent[i].config.id != 0xffff)) {
            _lock_release_recursive(&s_retention.lock);
            entries_destroy(module);
            return ESP_ERR_NOT_SUPPORTED;
        }
#endif
#if SOC_PM_PAU_REGDMA_UPDATE_CACHE_BEFORE_WAIT_COMPARE
        /* There is a bug in REGDMA wait mode, when two wait nodes need to wait for the
         * same value (_val & _mask), the second wait node will immediately return to
         * wait done, The reason is that the wait mode comparison output logic immediate
         * compares the value of the previous wait register cached inside the
         * digital logic before reading out he register contents specified by _backup.
         */
        #define config_is_wait_mode(_config)   (regdma_link_get_config_mode(_config) == REGDMA_LINK_MODE_WAIT)
        if ((retent[i].config.id != 0xffff) && config_is_wait_mode(&(retent[i].config)) && (retent[i].config.id != 0xfffe)) {
            uint32_t value = retent[i].config.write_wait.value;
            uint32_t mask  = retent[i].config.write_wait.mask;
            bool skip_b = retent[i].config.head.skip_b;
            bool skip_r = retent[i].config.head.skip_r;
            sleep_retention_entries_config_t wait_bug_workaround[] = {
                [0] = { .config = REGDMA_LINK_WRITE_INIT(0xfffe, PMU_DATE_REG, ~value, mask, skip_b, skip_r), .owner = retent[i].owner },
                [1] = { .config = REGDMA_LINK_WAIT_INIT (0xfffe, PMU_DATE_REG, ~value, mask, skip_b, skip_r), .owner = retent[i].owner }
            };
            err = entries_create(wait_bug_workaround, ARRAY_SIZE(wait_bug_workaround), priority, module);
        }
#endif
        if (err == ESP_OK) {
            void *link = entries_try_create(&retent[i].config, retent[i].owner, priority, module);
            if (link == NULL) {
                _lock_release_recursive(&s_retention.lock);
                entries_destroy(module);
                return ESP_ERR_NO_MEM;
            }

            int index = module_runtime_attach(instance(module)) ? 1 : 0;
            struct module_sleep_retention_context *ctx = &s_retention.context[index];
            entries_context_update(ctx, retent[i].owner, link, priority);
        }
    }
    _lock_release_recursive(&s_retention.lock);
    return err;
}

static esp_err_t entries_create_bonding(regdma_link_priority_t priority, sleep_retention_module_t module)
{
    static const sleep_retention_entries_config_t bonding_dummy = { REGDMA_LINK_WAIT_INIT(0xffff, 0, 0, 0, 1, 1), SLEEP_RETENTION_ENTRY_BITMAP_MASK };

    _lock_acquire_recursive(&s_retention.lock);
    void *link = entries_try_create_bonding(&bonding_dummy.config, bonding_dummy.owner, priority, module);
    if (link == NULL) {
        _lock_release_recursive(&s_retention.lock);
        entries_destroy(module);
        return ESP_ERR_NO_MEM;
    }
    int index = module_runtime_attach(instance(module)) ? 1 : 0;
    struct module_sleep_retention_context *ctx = &s_retention.context[index];
    entries_context_update(ctx, bonding_dummy.owner, link, priority);
    _lock_release_recursive(&s_retention.lock);
    return ESP_OK;
}

static void retention_entries_join(void)
{
    void *entries_tail = NULL;
    _lock_acquire_recursive(&s_retention.lock);
    s_retention.highpri = SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY;
    for (regdma_link_priority_t priority = 0; priority < SLEEP_RETENTION_REGDMA_LINK_NR_PRIORITIES; priority++) {
        if (s_retention.retention.lists[priority].entries_bitmap == 0) continue;
        if (priority < s_retention.highpri) { s_retention.highpri = priority; }
        if (entries_tail) {
            regdma_link_update_next_safe(
                    entries_tail,
                    s_retention.retention.lists[priority].entries[0],
                    s_retention.retention.lists[priority].entries[1],
                    s_retention.retention.lists[priority].entries[2],
                    s_retention.retention.lists[priority].entries[3]
#if (REGDMA_LINK_ENTRY_NUM == 5)
                  , s_retention.retention.lists[priority].entries[4]
#endif
                );
        }
        entries_tail = s_retention.retention.lists[priority].entries_tail;
    }

    bool final_default = s_retention.final_default == s_retention.retention.lists[SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY].entries[0];
    bool ready = (s_retention.highpri != SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY) || !final_default;
    if (ready) {
        pau_regdma_set_entry_link_addr(&(s_retention.retention.lists[s_retention.highpri].entries));
#if SOC_LIGHT_SLEEP_SUPPORTED
        pmu_sleep_enable_regdma_backup();
#endif
#if SOC_LIGHT_SLEEP_SUPPORTED && SOC_DEEP_SLEEP_SUPPORTED
        ESP_ERROR_CHECK(esp_deep_sleep_register_hook(&pmu_sleep_disable_regdma_backup));
#endif
    } else {
#if SOC_LIGHT_SLEEP_SUPPORTED
        pmu_sleep_disable_regdma_backup();
#endif
    }
    _lock_release_recursive(&s_retention.lock);
}

static esp_err_t entries_create_wrapper(const sleep_retention_entries_config_t retent[], int num, regdma_link_priority_t priority, sleep_retention_module_t module)
{
    _lock_acquire_recursive(&s_retention.lock);
    esp_err_t err = entries_create_bonding(priority, module);
    if(err) goto error;
    err = entries_create(retent, num, priority, module);
    if(err) goto error;
    err = entries_create_bonding(priority, module);
    if(err) goto error;
    s_retention.created_modules.bitmap[module >> 5] |= BIT(module % 32);
    if (!module_runtime_attach(instance(module))) {
        retention_entries_join();
    }
error:
    _lock_release_recursive(&s_retention.lock);
    return err;
}

esp_err_t sleep_retention_entries_create(const sleep_retention_entries_config_t retent[], int num, regdma_link_priority_t priority, sleep_retention_module_t module)
{
    if (retent == NULL || num <= 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (priority >= SLEEP_RETENTION_REGDMA_LINK_NR_PRIORITIES) {
        return ESP_ERR_INVALID_ARG;
    }
    if (module < SLEEP_RETENTION_MODULE_MIN || module > SLEEP_RETENTION_MODULE_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = check_and_create_final_default();
    if (err == ESP_OK) {
        err = entries_create_wrapper(retent, num, priority, module);
    }
    return err;
}

void sleep_retention_entries_get(sleep_retention_entries_t *entries)
{
    memset(entries, 0, sizeof(sleep_retention_entries_t));
    _lock_acquire_recursive(&s_retention.lock);
    if (s_retention.highpri >= SLEEP_RETENTION_REGDMA_LINK_HIGHEST_PRIORITY &&
        s_retention.highpri <= SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY) {
        memcpy(entries, &s_retention.retention.lists[s_retention.highpri].entries, sizeof(sleep_retention_entries_t));
    }
    _lock_release_recursive(&s_retention.lock);
}

sleep_retention_module_bitmap_t IRAM_ATTR sleep_retention_get_inited_modules(void)
{
    return s_retention.inited_modules;
}

sleep_retention_module_bitmap_t IRAM_ATTR sleep_retention_get_created_modules(void)
{
    return s_retention.created_modules;
}

sleep_retention_module_bitmap_t IRAM_ATTR sleep_retention_get_retained_modules(void)
{
    return s_retention.retention_modules;
}

bool sleep_retention_is_module_inited(sleep_retention_module_t module)
{
    if (module < SLEEP_RETENTION_MODULE_MIN || module > SLEEP_RETENTION_MODULE_MAX) {
        return false;
    }
    _lock_acquire_recursive(&s_retention.lock);
    bool inited = module_is_inited(module);
    _lock_release_recursive(&s_retention.lock);
    return inited;
}

bool sleep_retention_is_module_created(sleep_retention_module_t module)
{
    if (module < SLEEP_RETENTION_MODULE_MIN || module > SLEEP_RETENTION_MODULE_MAX) {
        return false;
    }
    _lock_acquire_recursive(&s_retention.lock);
    bool created = module_is_created(module);
    _lock_release_recursive(&s_retention.lock);
    return created;
}

bool sleep_retention_is_module_attached(sleep_retention_module_t module)
{
    if (module < SLEEP_RETENTION_MODULE_MIN || module > SLEEP_RETENTION_MODULE_MAX) {
        return false;
    }
    _lock_acquire_recursive(&s_retention.lock);
    bool attached = module_is_retained(module);
    _lock_release_recursive(&s_retention.lock);
    return attached;
}

sleep_retention_module_bitmap_t IRAM_ATTR sleep_retention_module_bitmap_and(sleep_retention_module_bitmap_t op0, sleep_retention_module_bitmap_t op1)
{
    sleep_retention_module_bitmap_t and;
    for (int i = 0; i < ARRAY_SIZE(and.bitmap); i++) {
        and.bitmap[i] = op0.bitmap[i] & op1.bitmap[i];
    }
    return and;
}
sleep_retention_module_bitmap_t IRAM_ATTR sleep_retention_module_bitmap_or(sleep_retention_module_bitmap_t op0, sleep_retention_module_bitmap_t op1)
{
    sleep_retention_module_bitmap_t or;
    for (int i = 0; i < ARRAY_SIZE(or.bitmap); i++) {
        or.bitmap[i] = op0.bitmap[i] | op1.bitmap[i];
    }
    return or;
}
sleep_retention_module_bitmap_t IRAM_ATTR sleep_retention_module_bitmap_not(sleep_retention_module_bitmap_t op)
{
    sleep_retention_module_bitmap_t not;
    for (int i = 0; i < ARRAY_SIZE(not.bitmap); i++) {
        not.bitmap[i] = ~op.bitmap[i];
    }
    return not;
}
bool IRAM_ATTR sleep_retention_module_bitmap_eq(sleep_retention_module_bitmap_t op0, sleep_retention_module_bitmap_t op1)
{
    for (int i = 0; i < ARRAY_SIZE(op0.bitmap); i++) {
        if (op0.bitmap[i] != op1.bitmap[i]) {
            return false;
        }
    }
    return true;
}

static void module_action(sleep_retention_module_t module, sleep_retention_module_t dep_module, int action)
{
    switch (action)
    {
    case 0: break;  /* Nothing to do */
    case 1: { /* allocate */
            set_reference(instance(dep_module), module);
            if (module_is_passive(instance(dep_module)) && module_runtime_attach(instance(dep_module))) {
                assert(module_runtime_attach(instance(module)));
            }
        }
        break;
    case 2: { /* free */
            clr_reference(instance(dep_module), module);
            if (module_is_passive(instance(dep_module)) && module_runtime_attach(instance(dep_module))) {
                assert(module_runtime_attach(instance(module)));
            }
        }
        break;
    case 3: refarray_set_bit(instance(dep_module), 1, module);  break;  /* attach */
    case 4: refarray_clr_bit(instance(dep_module), 1, module);  break;  /* detach */
    default:  break;
    }
}

static esp_err_t module_action_wrapper(sleep_retention_module_t module, int arg, esp_err_t (*rec)(sleep_retention_module_t))
{
    esp_err_t err = ESP_OK;

    sleep_retention_module_bitmap_t depends = get_dependencies(instance(module));
    for (int i = 0; ((err == ESP_OK) && (i < SLEEP_RETENTION_MODULE_BITMAP_SZ)); i++) {
        uint32_t bitmap = depends.bitmap[i];
        for (int j = 0; (err == ESP_OK) && bitmap; bitmap >>= 1, j++) {
            if (bitmap & BIT(0)) {
                sleep_retention_module_t dep_module = (sleep_retention_module_t)((i << 5) + j);

                #define action(x)   ((x) & 0xf)
                module_action(module, dep_module, action(arg));

                if ((arg & BIT(31)) || module_is_passive(instance(dep_module))) {
                    err = (*rec)(dep_module);
                }
            }
        }
    }
    return err;
}

esp_err_t sleep_retention_module_init(sleep_retention_module_t module, sleep_retention_module_init_param_t *param)
{
    if (module < SLEEP_RETENTION_MODULE_MIN || module > SLEEP_RETENTION_MODULE_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (param == NULL || param->cbs.create.handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_retention.lock == NULL) {
        /* Passive modules will be initialized during the system startup, with the
         * operating system scheduler not yet enabled. There is no risk of contention
         * for lock initialization here. */
        _lock_init_recursive(&s_retention.lock);
        if (s_retention.lock == NULL) {
            ESP_LOGE(TAG, "Create sleep retention lock failed");
            return ESP_ERR_NO_MEM;
        }
    }

    esp_err_t err = ESP_OK;
    _lock_acquire_recursive(&s_retention.lock);
    if (module_is_created(module) || module_is_inited(module)) {
        err = ESP_ERR_INVALID_STATE;
    } else {
        sleep_retention_module_object_ctor(instance(module), &param->cbs);
        set_dependencies(instance(module), param->depends);
        set_attributes(instance(module), param->attribute);
        s_retention.inited_modules.bitmap[module >> 5] |= BIT(module % 32);
    }
    _lock_release_recursive(&s_retention.lock);
    return err;
}

esp_err_t sleep_retention_module_deinit(sleep_retention_module_t module)
{
    if (module < SLEEP_RETENTION_MODULE_MIN || module > SLEEP_RETENTION_MODULE_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = ESP_OK;
    bool do_lock_release = false;
    _lock_acquire_recursive(&s_retention.lock);
    if (module_is_created(module) || !module_is_inited(module)) {
        err = ESP_ERR_INVALID_STATE;
    } else {
        clr_attributes(instance(module));
        clr_dependencies(instance(module));
        sleep_retention_module_object_dtor(instance(module));
        s_retention.inited_modules.bitmap[module >> 5] &= ~BIT(module % 32);
        uint32_t inited_modules = 0;
        for (int i = 0; i < SLEEP_RETENTION_MODULE_BITMAP_SZ; i++) {
            inited_modules |= s_retention.inited_modules.bitmap[i];
        }
        do_lock_release = (inited_modules == 0);
    }
    _lock_release_recursive(&s_retention.lock);

    if (do_lock_release) {
        _lock_close_recursive(&s_retention.lock);
        s_retention.lock = NULL;
    }
    return err;
}

static esp_err_t passive_module_allocate(sleep_retention_module_t module)
{
    assert(module >= SLEEP_RETENTION_MODULE_MIN && module <= SLEEP_RETENTION_MODULE_MAX);

    esp_err_t err = ESP_OK;
    _lock_acquire_recursive(&s_retention.lock);
    assert(module_is_passive(instance(module)) && "Illegal dependency");
    assert(module_is_inited(module) && "All passive module must be inited first!");
    if (!module_is_created(module)) {
        err = module_action_wrapper(module, (BIT(31) | action(1)), passive_module_allocate);
        if (err == ESP_OK) {
            sleep_retention_callback_t fn = instance(module)->cbs.create.handle;
            if (fn) {
                err = (*fn)(instance(module)->cbs.create.arg);
            }
        }
    }
    _lock_release_recursive(&s_retention.lock);
    return err;
}

esp_err_t sleep_retention_module_allocate(sleep_retention_module_t module)
{
    if (module < SLEEP_RETENTION_MODULE_MIN || module > SLEEP_RETENTION_MODULE_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = ESP_OK;
    _lock_acquire_recursive(&s_retention.lock);
    if (!module_is_passive(instance(module))) {
        if (module_is_inited(module) && !module_is_created(module)) {
            err = module_action_wrapper(module, action(1), passive_module_allocate);
            if (err == ESP_OK) {
                sleep_retention_callback_t fn = instance(module)->cbs.create.handle;
                if (fn) {
                    err = (*fn)(instance(module)->cbs.create.arg);
                }
            }
        } else {
            err = ESP_ERR_INVALID_STATE;
        }
    } else {
        err = ESP_ERR_NOT_ALLOWED;
    }
    _lock_release_recursive(&s_retention.lock);
    return err;
}

static esp_err_t passive_module_free(sleep_retention_module_t module)
{
    assert(module >= SLEEP_RETENTION_MODULE_MIN && module <= SLEEP_RETENTION_MODULE_MAX);

    esp_err_t err = ESP_OK;
    _lock_acquire_recursive(&s_retention.lock);
    assert(module_is_passive(instance(module)) && "Illegal dependency");
    assert(module_is_inited(module) && "All passive module must be inited first!");
    if (module_is_created(module)) {
        if (!references_exist(instance(module))) {
            if (!module_is_retained(module)) {
                sleep_retention_entries_destroy(module);
                err = module_action_wrapper(module, (BIT(31) | action(2)), passive_module_free);
            } else {
                err = ESP_ERR_INVALID_STATE;
            }
        }
    }
    _lock_release_recursive(&s_retention.lock);
    return err;
}

esp_err_t sleep_retention_module_free(sleep_retention_module_t module)
{
    if (module < SLEEP_RETENTION_MODULE_MIN || module > SLEEP_RETENTION_MODULE_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = ESP_OK;
    _lock_acquire_recursive(&s_retention.lock);
    if (!module_is_passive(instance(module))) {
        if (module_is_inited(module) && module_is_created(module) && !module_is_retained(module)) {
            sleep_retention_entries_destroy(module);
            err = module_action_wrapper(module, action(2), passive_module_free);
        } else {
            err = ESP_ERR_INVALID_STATE;
        }
    } else {
        err = ESP_ERR_NOT_ALLOWED;
    }
    _lock_release_recursive(&s_retention.lock);
    return err;
}

static void module_entries_move(sleep_retention_module_t module, struct module_sleep_retention_context *s, struct module_sleep_retention_context *d)
{
    void *tail = NULL, *prev_tail = NULL;
    sleep_retention_entries_t entries, next_entries;
    regdma_link_priority_t priority = 0;

    memset(&entries, 0, sizeof(sleep_retention_entries_t));
    memset(&next_entries, 0, sizeof(sleep_retention_entries_t));

    _lock_acquire_recursive(&s_retention.lock);
    do {
        bool exist = module_entries_get(s, priority, module, &entries, &tail, &next_entries, &prev_tail);
        if (exist) {
            entries_detach(s, priority, &entries, tail, &next_entries, prev_tail);
            entries_attach(d, priority, &entries, tail);
        } else {
            priority++;
        }
        retention_entries_join();
    } while (priority < SLEEP_RETENTION_REGDMA_LINK_NR_PRIORITIES);
    _lock_release_recursive(&s_retention.lock);
}

static esp_err_t passive_module_attach(sleep_retention_module_t module)
{
    assert(module >= SLEEP_RETENTION_MODULE_MIN && module <= SLEEP_RETENTION_MODULE_MAX);

    esp_err_t err = ESP_OK;
    _lock_acquire_recursive(&s_retention.lock);
    assert(module_is_passive(instance(module)) && "Illegal dependency");
    assert(module_runtime_attach(instance(module)) && "Illegal dependency");
    assert(module_is_inited(module) && "All passive module must be inited first!");
    if (module_is_inited(module) && module_is_created(module) && !module_is_retained(module)) {
        module_entries_move(module, &s_retention.context[1], &s_retention.retention);
        s_retention.retention_modules.bitmap[module >> 5] |= BIT(module % 32);
        err = module_action_wrapper(module, (BIT(31) | action(3)), passive_module_attach);
    }
    _lock_release_recursive(&s_retention.lock);
    return err;
}

esp_err_t sleep_retention_module_attach(sleep_retention_module_t module)
{
    if (module < SLEEP_RETENTION_MODULE_MIN || module > SLEEP_RETENTION_MODULE_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = ESP_OK;
    _lock_acquire_recursive(&s_retention.lock);
    if (!module_is_passive(instance(module))) {
        if (module_is_inited(module) && module_is_created(module) && !module_is_retained(module)) {
            if (module_runtime_attach(instance(module))) {
                module_entries_move(module, &s_retention.context[1], &s_retention.retention);
                s_retention.retention_modules.bitmap[module >> 5] |= BIT(module % 32);
                err = module_action_wrapper(module, action(3), passive_module_attach);
            } else {
                err = ESP_ERR_NOT_SUPPORTED;
            }
        } else {
            err = ESP_ERR_INVALID_STATE;
        }
    } else {
        err = ESP_ERR_NOT_ALLOWED;
    }
    _lock_release_recursive(&s_retention.lock);
    return err;
}

static esp_err_t passive_module_detach(sleep_retention_module_t module)
{
    assert(module >= SLEEP_RETENTION_MODULE_MIN && module <= SLEEP_RETENTION_MODULE_MAX);

    esp_err_t err = ESP_OK;
    _lock_acquire_recursive(&s_retention.lock);
    assert(module_is_passive(instance(module)) && "Illegal dependency");
    assert(module_runtime_attach(instance(module)) && "Illegal dependency");
    assert(module_is_inited(module) && "All passive module must be inited first!");
    if (module_is_inited(module) && module_is_created(module) && module_is_retained(module)) {
        if (refarray_zero(instance(module), 1)) {
            module_entries_move(module, &s_retention.retention, &s_retention.context[1]);
            s_retention.retention_modules.bitmap[module >> 5] &= ~BIT(module % 32);
            err = module_action_wrapper(module, (BIT(31) | action(4)), passive_module_detach);
        }
    }
    _lock_release_recursive(&s_retention.lock);
    return err;
}

esp_err_t sleep_retention_module_detach(sleep_retention_module_t module)
{
    if (module < SLEEP_RETENTION_MODULE_MIN || module > SLEEP_RETENTION_MODULE_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = ESP_OK;
    _lock_acquire_recursive(&s_retention.lock);
    if (!module_is_passive(instance(module))) {
        if (module_is_inited(module) && module_is_created(module) && module_is_retained(module)) {
            if (module_runtime_attach(instance(module))) {
                module_entries_move(module, &s_retention.retention, &s_retention.context[1]);
                s_retention.retention_modules.bitmap[module >> 5] &= ~BIT(module % 32);
                err = module_action_wrapper(module, action(4), passive_module_detach);
            } else {
                err = ESP_ERR_NOT_SUPPORTED;
            }
        } else {
            err = ESP_ERR_INVALID_STATE;
        }
    } else {
        err = ESP_ERR_NOT_ALLOWED;
    }
    _lock_release_recursive(&s_retention.lock);
    return err;
}

static esp_err_t empty_create(void *args)
{
    return ESP_OK;
}

esp_err_t sleep_retention_power_lock_acquire(void)
{
    _lock_acquire_recursive(&s_retention.lock);
    if (acquire_cnt == 0) {
        sleep_retention_module_init_param_t init_param = {
            .cbs = { .create = {.handle = empty_create},},
        };
        esp_err_t ret = sleep_retention_module_init(SLEEP_RETENTION_MODULE_NULL, &init_param);
        if (ret != ESP_OK) {
            _lock_release_recursive(&s_retention.lock);
            return ret;
        }
    }
    acquire_cnt++;
    _lock_release_recursive(&s_retention.lock);
    return ESP_OK;
}

esp_err_t sleep_retention_power_lock_release(void)
{
    esp_err_t ret = ESP_OK;
    _lock_acquire_recursive(&s_retention.lock);
    acquire_cnt--;
    assert(acquire_cnt >= 0);
    if (acquire_cnt == 0) {
        ret = sleep_retention_module_deinit(SLEEP_RETENTION_MODULE_NULL);
    }
    _lock_release_recursive(&s_retention.lock);
    return ret;
}

void IRAM_ATTR sleep_retention_do_extra_retention(bool backup_or_restore)
{
    if (s_retention.highpri < SLEEP_RETENTION_REGDMA_LINK_HIGHEST_PRIORITY ||
        s_retention.highpri > SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY) {
        return;
    }
#if SOC_PAU_IN_TOP_DOMAIN
    bool origin_bypass_en = pau_regdma_enable_aon_link_entry(false);
#endif
    // Set extra linked list head pointer to hardware
    pau_regdma_set_extra_link_addr(s_retention.retention.lists[s_retention.highpri].entries[EXTRA_LINK_NUM]);
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    /* Data of retention link may be temporarily stored in L1 DCache, which is not accessible by
       REGDMA, write it back to L2MEM before starting REGDMA. */
    cache_ll_writeback_all(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA, CACHE_LL_ID_ALL);
#endif
    if (backup_or_restore) {
        pau_regdma_trigger_extra_link_backup();
    } else {
        pau_regdma_trigger_extra_link_restore();
    }
#if SOC_PAU_IN_TOP_DOMAIN
    pau_regdma_enable_aon_link_entry(origin_bypass_en);
#endif
}

#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
void IRAM_ATTR sleep_retention_do_system_retention(bool backup_or_restore)
{
    #define SYSTEM_LINK_NUM (0)
    if (s_retention.highpri >= SLEEP_RETENTION_REGDMA_LINK_HIGHEST_PRIORITY &&
        s_retention.highpri <= SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY) {
        // Set extra linked list head pointer to hardware
        pau_regdma_set_system_link_addr(s_retention.retention.lists[s_retention.highpri].entries[SYSTEM_LINK_NUM]);
        // When PD TOP, we need to prevent the PMU from triggering the REGDMA backup, because REGDMA will power off
        pmu_sleep_disable_regdma_backup();
        if (backup_or_restore) {
            pau_regdma_trigger_system_link_backup();
        } else {
            pau_regdma_trigger_system_link_restore();
        }
    }
}
#endif

#if SOC_PM_SUPPORT_PMU_MODEM_STATE
void IRAM_ATTR sleep_retention_do_phy_retention(bool backup_or_restore, bool wifimac_link_is_sel)
{
/* since the PHY link and other module links are within the sleep-retention entry (4) context，
*  add mutex protection to avoid data race.
*/
#if SOC_PM_PAU_REGDMA_COMMON_PHY_LINK_ENTRY
    _lock_acquire_recursive(&s_retention.lock);
#endif
    if (backup_or_restore) {
#if SOC_PM_PAU_REGDMA_MODEM_WIFIMAC_WORKAROUND
        if (wifimac_link_is_sel) {
            pau_regdma_trigger_wifimac_link_backup();
        } else
#endif
        {
            pau_regdma_trigger_modem_link_backup();
        }
    } else {
#if SOC_PM_PAU_REGDMA_MODEM_WIFIMAC_WORKAROUND
        if (wifimac_link_is_sel) {
            pau_regdma_trigger_wifimac_link_restore();
        } else
#endif
        {
            pau_regdma_trigger_modem_link_restore();
        }
    }
#if SOC_PM_PAU_REGDMA_COMMON_PHY_LINK_ENTRY
    _lock_release_recursive(&s_retention.lock);
#endif
}
#endif /*SOC_PM_SUPPORT_PMU_MODEM_STATE */
