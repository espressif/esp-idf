/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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

static __attribute__((unused)) const char *TAG = "sleep";
static int acquire_cnt; //for the force acquire lock


struct sleep_retention_module_object {
    sleep_retention_module_callbacks_t cbs;         /* A callback list that can extend more sleep retention event callbacks */
    sleep_retention_module_bitmap_t    dependents;  /* A bitmap identifying all modules that the current module depends on */
    sleep_retention_module_bitmap_t    references;  /* A bitmap indicating all other modules that depend on (or reference) the current module,
                                                     * It will update at runtime based on whether the module is referenced by other modules */
    sleep_retention_module_attribute_t attributes;  /* A bitmap indicating attribute of the current module */
};

static inline void sleep_retention_module_object_ctor(struct sleep_retention_module_object * const self, sleep_retention_module_callbacks_t *cbs)
{
    self->cbs = *cbs;
    self->dependents = (sleep_retention_module_bitmap_t){ .bitmap = { 0 } };
    self->references = (sleep_retention_module_bitmap_t){ .bitmap = { 0 } };
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

static inline bool module_is_passive(struct sleep_retention_module_object * const self)
{
    return (get_attributes(self) & SLEEP_RETENTION_MODULE_ATTR_PASSIVE) ? true : false;
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

/**
 * Internal structure which holds all requested sleep retention parameters
 */
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
    _lock_t lock;
    regdma_link_priority_t highpri;
    sleep_retention_module_bitmap_t inited_modules;
    sleep_retention_module_bitmap_t created_modules;

    struct sleep_retention_module_object instance[SLEEP_RETENTION_MODULE_MAX + 1];

#define EXTRA_LINK_NUM  (REGDMA_LINK_ENTRY_NUM - 1)
} sleep_retention_t;

static DRAM_ATTR __attribute__((unused)) sleep_retention_t s_retention = {
    .highpri = (uint8_t)-1,
    .inited_modules = (sleep_retention_module_bitmap_t){ .bitmap = { 0 } },
    .created_modules = (sleep_retention_module_bitmap_t){ .bitmap = { 0 } }
};

#define SLEEP_RETENTION_ENTRY_BITMAP_MASK       (BIT(REGDMA_LINK_ENTRY_NUM) - 1)
#define SLEEP_RETENTION_ENTRY_BITMAP(bitmap)    ((bitmap) & SLEEP_RETENTION_ENTRY_BITMAP_MASK)

static esp_err_t sleep_retention_entries_create_impl(const sleep_retention_entries_config_t retent[], int num, regdma_link_priority_t priority, sleep_retention_module_t module);
static void sleep_retention_entries_join(void);

static inline bool sleep_retention_entries_require_branch(uint32_t owner, uint32_t runtime_bitmap)
{
    bool use_new_entry      = SLEEP_RETENTION_ENTRY_BITMAP(owner & ~runtime_bitmap) ? true : false;
    bool intersection_exist = SLEEP_RETENTION_ENTRY_BITMAP(owner &  runtime_bitmap) ? true : false;
    return use_new_entry && intersection_exist;
}

static esp_err_t sleep_retention_entries_check_and_create_default(uint32_t owner, uint32_t runtime_bitmap, uint32_t entries_bitmap, regdma_link_priority_t priority, sleep_retention_module_t module)
{
    assert(sleep_retention_entries_require_branch(owner, runtime_bitmap));

    static sleep_retention_entries_config_t dummy = { REGDMA_LINK_WAIT_INIT(0xffff, 0, 0, 0, 1, 1), 0 };
    dummy.owner = SLEEP_RETENTION_ENTRY_BITMAP(owner & ~entries_bitmap);
    if (dummy.owner) {
        return sleep_retention_entries_create_impl(&dummy, 1, priority, module);
    }
    return ESP_OK;
}

static esp_err_t sleep_retention_entries_check_and_create_final_default(void)
{
    static const sleep_retention_entries_config_t final_dummy = { REGDMA_LINK_WAIT_INIT(0xffff, 0, 0, 0, 1, 1), SLEEP_RETENTION_ENTRY_BITMAP_MASK };

    esp_err_t err = ESP_OK;
    _lock_acquire_recursive(&s_retention.lock);
    if (s_retention.lists[SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY].entries_bitmap == 0) {
        err = sleep_retention_entries_create_impl(&final_dummy, 1, SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY, SLEEP_RETENTION_MODULE_INVALID);
    }
    _lock_release_recursive(&s_retention.lock);
    return err;
}

static void sleep_retention_entries_update(uint32_t owner, void *new_link, regdma_link_priority_t priority)
{
    _lock_acquire_recursive(&s_retention.lock);
    sleep_retention_entries_t retention_entries = {
        (owner & BIT(0)) ? new_link : s_retention.lists[priority].entries[0],
        (owner & BIT(1)) ? new_link : s_retention.lists[priority].entries[1],
        (owner & BIT(2)) ? new_link : s_retention.lists[priority].entries[2],
        (owner & BIT(3)) ? new_link : s_retention.lists[priority].entries[3]
#if (REGDMA_LINK_ENTRY_NUM == 5)
        , (owner & BIT(4)) ? new_link : s_retention.lists[priority].entries[4]
#endif
    };
    if (s_retention.lists[priority].entries_bitmap == 0) {
        s_retention.lists[priority].entries_tail = new_link;
    }
    memcpy(s_retention.lists[priority].entries, retention_entries, sizeof(sleep_retention_entries_t));
    s_retention.lists[priority].runtime_bitmap  = owner;
    s_retention.lists[priority].entries_bitmap |= owner;
    _lock_release_recursive(&s_retention.lock);
}

static void * sleep_retention_entries_try_create(const regdma_link_config_t *config, uint32_t owner, regdma_link_priority_t priority, sleep_retention_module_t module)
{
    void *link = NULL;
    assert(owner > 0 && owner < BIT(REGDMA_LINK_ENTRY_NUM));

    _lock_acquire_recursive(&s_retention.lock);
    if (sleep_retention_entries_require_branch(owner, s_retention.lists[priority].runtime_bitmap)) {
        if (sleep_retention_entries_check_and_create_default(owner, s_retention.lists[priority].runtime_bitmap,
            s_retention.lists[priority].entries_bitmap, priority, module) == ESP_OK) { /* branch node can't as tail node */
            link = regdma_link_init_safe(config, true, module,
                        (owner & BIT(0)) ? s_retention.lists[priority].entries[0] : NULL,
                        (owner & BIT(1)) ? s_retention.lists[priority].entries[1] : NULL,
                        (owner & BIT(2)) ? s_retention.lists[priority].entries[2] : NULL,
                        (owner & BIT(3)) ? s_retention.lists[priority].entries[3] : NULL
#if (REGDMA_LINK_ENTRY_NUM == 5)
                        , (owner & BIT(4)) ? s_retention.lists[priority].entries[4] : NULL
#endif
                    );
        }
    } else {
        link = regdma_link_init_safe(config, false, module, s_retention.lists[priority].entries[__builtin_ffs(owner) - 1]);
    }
    _lock_release_recursive(&s_retention.lock);
    return link;
}

static void * sleep_retention_entries_try_create_bonding(const regdma_link_config_t *config, uint32_t owner, regdma_link_priority_t priority, sleep_retention_module_t module)
{
    assert(owner > 0 && owner < BIT(REGDMA_LINK_ENTRY_NUM));
    _lock_acquire_recursive(&s_retention.lock);
    void *link = regdma_link_init_safe(config, true, module,
                (owner & BIT(0)) ? s_retention.lists[priority].entries[0] : NULL,
                (owner & BIT(1)) ? s_retention.lists[priority].entries[1] : NULL,
                (owner & BIT(2)) ? s_retention.lists[priority].entries[2] : NULL,
                (owner & BIT(3)) ? s_retention.lists[priority].entries[3] : NULL
#if (REGDMA_LINK_ENTRY_NUM == 5)
                , (owner & BIT(4)) ? s_retention.lists[priority].entries[4] : NULL
#endif
            );
    _lock_release_recursive(&s_retention.lock);
    return link;
}

static void sleep_retention_entries_stats(void)
{
    _lock_acquire_recursive(&s_retention.lock);
    if (s_retention.highpri >= SLEEP_RETENTION_REGDMA_LINK_HIGHEST_PRIORITY && s_retention.highpri <= SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY) {
        for (int entry = 0; entry < ARRAY_SIZE(s_retention.lists[s_retention.highpri].entries); entry++) {
            regdma_link_stats(s_retention.lists[s_retention.highpri].entries[entry], entry);
        }
    }
    _lock_release_recursive(&s_retention.lock);
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
        for (int entry = 0; entry < ARRAY_SIZE(s_retention.lists[s_retention.highpri].entries); entry++) {
            fprintf(out, "\nsleep retention entries[%d] context:\n", entry);
            regdma_link_dump(out, s_retention.lists[s_retention.highpri].entries[entry], entry);
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
        for (int entry = 0; (link == NULL && entry < ARRAY_SIZE(s_retention.lists[s_retention.highpri].entries)); entry++) {
            link = regdma_find_link_by_id(s_retention.lists[s_retention.highpri].entries[entry], entry, id);
        }
    }
    _lock_release_recursive(&s_retention.lock);
    return link;
}

static uint32_t sleep_retention_entries_owner_bitmap(sleep_retention_entries_t *entries, sleep_retention_entries_t *tails)
{
    uint32_t owner = 0;
    _lock_acquire_recursive(&s_retention.lock);
    for (int entry = 0; entry < ARRAY_SIZE(*entries); entry++) {
        owner |= regdma_link_get_owner_bitmap((*entries)[entry], (*tails)[entry], entry);
    }
    _lock_release_recursive(&s_retention.lock);
    return owner;
}

static bool sleep_retention_entries_get_destroy_context(regdma_link_priority_t priority, sleep_retention_module_t module, sleep_retention_entries_t *destroy_entries, void **destroy_tail, sleep_retention_entries_t *next_entries, void **prev_tail)
{
    bool exist = false;
    sleep_retention_entries_t destroy_tails, prev_tails;

    memset(&destroy_tails, 0, sizeof(sleep_retention_entries_t));
    memset(&prev_tails, 0, sizeof(sleep_retention_entries_t));

    _lock_acquire_recursive(&s_retention.lock);
    for (int entry = 0; entry < ARRAY_SIZE(s_retention.lists[priority].entries); entry++) {
        (*destroy_entries)[entry] = regdma_find_module_link_head(
                s_retention.lists[priority].entries[entry], s_retention.lists[priority].entries_tail, entry, module);
        destroy_tails     [entry] = regdma_find_module_link_tail(
                s_retention.lists[priority].entries[entry], s_retention.lists[priority].entries_tail, entry, module);
        (*next_entries)   [entry] = regdma_find_next_module_link_head(
                s_retention.lists[priority].entries[entry], s_retention.lists[priority].entries_tail, entry, module);
        prev_tails        [entry] = regdma_find_prev_module_link_tail(
                s_retention.lists[priority].entries[entry], s_retention.lists[priority].entries_tail, entry, module);
        if ((*destroy_entries)[entry] && destroy_tails[entry]) {
            exist = true;
        }
        assert(destroy_tails[entry] == destroy_tails[0]);
        assert(prev_tails[entry] == prev_tails[0]);
    }
    *destroy_tail = destroy_tails[0];
    *prev_tail = prev_tails[0];
    _lock_release_recursive(&s_retention.lock);
    return exist;
}

static void sleep_retention_entries_context_update(regdma_link_priority_t priority)
{
    _lock_acquire_recursive(&s_retention.lock);
    sleep_retention_entries_t tails = {
        s_retention.lists[priority].entries_tail, s_retention.lists[priority].entries_tail,
        s_retention.lists[priority].entries_tail, s_retention.lists[priority].entries_tail
    };
    s_retention.lists[priority].entries_bitmap = sleep_retention_entries_owner_bitmap(&s_retention.lists[priority].entries, &tails);
    s_retention.lists[priority].runtime_bitmap = sleep_retention_entries_owner_bitmap(&s_retention.lists[priority].entries, &s_retention.lists[priority].entries);
    _lock_release_recursive(&s_retention.lock);
}

static bool sleep_retention_entries_dettach(regdma_link_priority_t priority, sleep_retention_entries_t *destroy_entries, void *destroy_tail, sleep_retention_entries_t *next_entries, void *prev_tail)
{
    _lock_acquire_recursive(&s_retention.lock);
    bool is_head = (memcmp(destroy_entries, &s_retention.lists[priority].entries, sizeof(sleep_retention_entries_t)) == 0);
    bool is_tail = (destroy_tail == s_retention.lists[priority].entries_tail);

    if (is_head && is_tail) {
        memset(s_retention.lists[priority].entries, 0, sizeof(sleep_retention_entries_t));
        s_retention.lists[priority].entries_tail = NULL;
    } else if (is_head) {
        memcpy(&s_retention.lists[priority].entries, next_entries, sizeof(sleep_retention_entries_t));
    } else if (is_tail) {
        s_retention.lists[priority].entries_tail = prev_tail;
    } else {
#if (REGDMA_LINK_ENTRY_NUM == 5)
        regdma_link_update_next_safe(prev_tail, (*next_entries)[0], (*next_entries)[1], (*next_entries)[2], (*next_entries)[3], (*next_entries)[4]);
#else
        regdma_link_update_next_safe(prev_tail, (*next_entries)[0], (*next_entries)[1], (*next_entries)[2], (*next_entries)[3]);
#endif
    }
    sleep_retention_entries_context_update(priority);
#if (REGDMA_LINK_ENTRY_NUM == 5)
    regdma_link_update_next_safe(destroy_tail, NULL, NULL, NULL, NULL, NULL);
#else
    regdma_link_update_next_safe(destroy_tail, NULL, NULL, NULL, NULL);
#endif
    _lock_release_recursive(&s_retention.lock);
    return (is_head || is_tail);
}

static void sleep_retention_entries_destroy_wrapper(sleep_retention_entries_t *destroy_entries)
{
    for (int entry = 0; entry < ARRAY_SIZE(*destroy_entries); entry++) {
        regdma_link_destroy((*destroy_entries)[entry], entry);
    }
}

static void sleep_retention_entries_check_and_distroy_final_default(void)
{
    _lock_acquire_recursive(&s_retention.lock);
    assert(s_retention.highpri == SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY);
    uint32_t created_modules = 0;
    for (int i = 0; i < SLEEP_RETENTION_MODULE_BITMAP_SZ; i++) {
        created_modules |= s_retention.created_modules.bitmap[i];
    }
    assert(created_modules == 0);
    sleep_retention_entries_destroy_wrapper(&s_retention.lists[SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY].entries);
    _lock_release_recursive(&s_retention.lock);
}

static void sleep_retention_entries_all_destroy_wrapper(sleep_retention_module_t module)
{
    void *destroy_tail = NULL, *prev_tail = NULL;
    sleep_retention_entries_t destroy_entries, next_entries;

    memset(&destroy_entries, 0, sizeof(sleep_retention_entries_t));
    memset(&next_entries, 0, sizeof(sleep_retention_entries_t));

    _lock_acquire_recursive(&s_retention.lock);
    regdma_link_priority_t priority = 0;
    do {
        bool exist = sleep_retention_entries_get_destroy_context(priority, module, &destroy_entries, &destroy_tail, &next_entries, &prev_tail);
        if (s_retention.lists[priority].entries_bitmap && exist) {
            if (sleep_retention_entries_dettach(priority, &destroy_entries, destroy_tail, &next_entries, prev_tail)) {
                sleep_retention_entries_join();
            }
            sleep_retention_entries_destroy_wrapper(&destroy_entries);
        } else {
            priority++;
        }
    } while (priority < SLEEP_RETENTION_REGDMA_LINK_NR_PRIORITIES);
    s_retention.created_modules.bitmap[module >> 5] &= ~BIT(module % 32);
    _lock_release_recursive(&s_retention.lock);
}

static void sleep_retention_entries_do_destroy(sleep_retention_module_t module)
{
    assert(SLEEP_RETENTION_MODULE_MIN <= module && module <= SLEEP_RETENTION_MODULE_MAX);
    _lock_acquire_recursive(&s_retention.lock);
    sleep_retention_entries_join();
    sleep_retention_entries_stats();
    sleep_retention_entries_all_destroy_wrapper(module);
    _lock_release_recursive(&s_retention.lock);
}

static void sleep_retention_entries_destroy(sleep_retention_module_t module)
{
    assert(SLEEP_RETENTION_MODULE_MIN <= module && module <= SLEEP_RETENTION_MODULE_MAX);
    _lock_acquire_recursive(&s_retention.lock);
    sleep_retention_entries_do_destroy(module);
    uint32_t created_modules = 0;
    for (int i = 0; i < SLEEP_RETENTION_MODULE_BITMAP_SZ; i++) {
        created_modules |= s_retention.created_modules.bitmap[i];
    }
    if (created_modules == 0) {
        sleep_retention_entries_check_and_distroy_final_default();
        pmu_sleep_disable_regdma_backup();
        memset((void *)s_retention.lists, 0, sizeof(s_retention.lists));
        s_retention.highpri = (uint8_t)-1;
    }
    _lock_release_recursive(&s_retention.lock);
}

static esp_err_t sleep_retention_entries_create_impl(const sleep_retention_entries_config_t retent[], int num, regdma_link_priority_t priority, sleep_retention_module_t module)
{
    esp_err_t err = ESP_OK;
    _lock_acquire_recursive(&s_retention.lock);
    for (int i = num - 1; (i >= 0) && (err == ESP_OK); i--) {
#if SOC_PM_RETENTION_HAS_CLOCK_BUG
        if ((retent[i].owner > BIT(EXTRA_LINK_NUM)) && (retent[i].config.id != 0xffff)) {
            _lock_release_recursive(&s_retention.lock);
            sleep_retention_entries_do_destroy(module);
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
            err = sleep_retention_entries_create_impl(wait_bug_workaround, ARRAY_SIZE(wait_bug_workaround), priority, module);
        }
#endif
        if (err == ESP_OK) {
            void *link = sleep_retention_entries_try_create(&retent[i].config, retent[i].owner, priority, module);
            if (link == NULL) {
                _lock_release_recursive(&s_retention.lock);
                sleep_retention_entries_do_destroy(module);
                return ESP_ERR_NO_MEM;
            }
            sleep_retention_entries_update(retent[i].owner, link, priority);
        }
    }
    _lock_release_recursive(&s_retention.lock);
    return err;
}

static esp_err_t sleep_retention_entries_create_bonding(regdma_link_priority_t priority, sleep_retention_module_t module)
{
    static const sleep_retention_entries_config_t bonding_dummy = { REGDMA_LINK_WAIT_INIT(0xffff, 0, 0, 0, 1, 1), SLEEP_RETENTION_ENTRY_BITMAP_MASK };

    _lock_acquire_recursive(&s_retention.lock);
    void *link = sleep_retention_entries_try_create_bonding(&bonding_dummy.config, bonding_dummy.owner, priority, module);
    if (link == NULL) {
        _lock_release_recursive(&s_retention.lock);
        sleep_retention_entries_do_destroy(module);
        return ESP_ERR_NO_MEM;
    }
    sleep_retention_entries_update(bonding_dummy.owner, link, priority);
    _lock_release_recursive(&s_retention.lock);
    return ESP_OK;
}

static void sleep_retention_entries_join(void)
{
    void *entries_tail = NULL;
    _lock_acquire_recursive(&s_retention.lock);
    s_retention.highpri = SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY;
    for (regdma_link_priority_t priority = 0; priority < SLEEP_RETENTION_REGDMA_LINK_NR_PRIORITIES; priority++) {
        if (s_retention.lists[priority].entries_bitmap == 0) continue;
        if (priority < s_retention.highpri) { s_retention.highpri = priority; }
        if (entries_tail) {
            regdma_link_update_next_safe(
                    entries_tail,
                    s_retention.lists[priority].entries[0],
                    s_retention.lists[priority].entries[1],
                    s_retention.lists[priority].entries[2],
                    s_retention.lists[priority].entries[3]
#if (REGDMA_LINK_ENTRY_NUM == 5)
                    , s_retention.lists[priority].entries[4]
#endif
                );
        }
        entries_tail = s_retention.lists[priority].entries_tail;
    }
    pau_regdma_set_entry_link_addr(&(s_retention.lists[s_retention.highpri].entries));
    _lock_release_recursive(&s_retention.lock);
}

static esp_err_t sleep_retention_entries_create_wrapper(const sleep_retention_entries_config_t retent[], int num, regdma_link_priority_t priority, sleep_retention_module_t module)
{
    _lock_acquire_recursive(&s_retention.lock);
    esp_err_t err = sleep_retention_entries_create_bonding(priority, module);
    if(err) goto error;
    err = sleep_retention_entries_create_impl(retent, num, priority, module);
    if(err) goto error;
    err = sleep_retention_entries_create_bonding(priority, module);
    if(err) goto error;
    s_retention.created_modules.bitmap[module >> 5] |= BIT(module % 32);
    sleep_retention_entries_join();

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
    esp_err_t err = sleep_retention_entries_check_and_create_final_default();
    if (err)  goto error;
    err = sleep_retention_entries_create_wrapper(retent, num, priority, module);
    if (err)  goto error;
    pmu_sleep_enable_regdma_backup();
    ESP_ERROR_CHECK(esp_deep_sleep_register_hook(&pmu_sleep_disable_regdma_backup));

error:
    return err;
}

void sleep_retention_entries_get(sleep_retention_entries_t *entries)
{
    memset(entries, 0, sizeof(sleep_retention_entries_t));
    _lock_acquire_recursive(&s_retention.lock);
    if (s_retention.highpri >= SLEEP_RETENTION_REGDMA_LINK_HIGHEST_PRIORITY &&
        s_retention.highpri <= SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY) {
        memcpy(entries, &s_retention.lists[s_retention.highpri].entries, sizeof(sleep_retention_entries_t));
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
        sleep_retention_module_object_ctor(&s_retention.instance[module], &param->cbs);
        set_dependencies(&s_retention.instance[module], param->depends);
        set_attributes(&s_retention.instance[module], param->attribute);
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
        clr_attributes(&s_retention.instance[module]);
        clr_dependencies(&s_retention.instance[module]);
        sleep_retention_module_object_dtor(&s_retention.instance[module]);
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

static esp_err_t sleep_retention_passive_module_allocate(sleep_retention_module_t module)
{
    assert(module >= SLEEP_RETENTION_MODULE_MIN && module <= SLEEP_RETENTION_MODULE_MAX);

    esp_err_t err = ESP_OK;
    _lock_acquire_recursive(&s_retention.lock);
    assert(module_is_passive(&s_retention.instance[module]) && "Illegal dependency");
    assert(module_is_inited(module) && "All passive module must be inited first!");
    if (!module_is_created(module)) {
        sleep_retention_module_bitmap_t depends = get_dependencies(&s_retention.instance[module]);
        for (int i = 0; ((err == ESP_OK) && (i < SLEEP_RETENTION_MODULE_BITMAP_SZ)); i++) {
            uint32_t bitmap = depends.bitmap[i];
            for (int j = 0; (err == ESP_OK) && bitmap; bitmap >>= 1, j++) {
                if (bitmap & BIT(0)) {
                    set_reference(&s_retention.instance[(i << 5) + j], module);
                    err = sleep_retention_passive_module_allocate((i << 5) + j);
                }
            }
        }
        if (err == ESP_OK) {
            sleep_retention_callback_t fn = s_retention.instance[module].cbs.create.handle;
            if (fn) {
                err = (*fn)(s_retention.instance[module].cbs.create.arg);
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
    if (!module_is_passive(&s_retention.instance[module])) {
        if (module_is_inited(module) && !module_is_created(module)) {
            sleep_retention_module_bitmap_t depends = get_dependencies(&s_retention.instance[module]);
            for (int i = 0; ((err == ESP_OK) && (i < SLEEP_RETENTION_MODULE_BITMAP_SZ)); i++) {
                uint32_t bitmap = depends.bitmap[i];
                for (int j = 0; (err == ESP_OK) && bitmap; bitmap >>= 1, j++) {
                    if (bitmap & BIT(0)) {
                        set_reference(&s_retention.instance[(i << 5) + j], module);
                        if (module_is_passive(&s_retention.instance[(i << 5) + j])) { /* the callee ensures this module is inited */
                            err = sleep_retention_passive_module_allocate((i << 5) + j);
                        }
                    }
                }
            }
            if (err == ESP_OK) {
                sleep_retention_callback_t fn = s_retention.instance[module].cbs.create.handle;
                if (fn) {
                    err = (*fn)(s_retention.instance[module].cbs.create.arg);
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

static esp_err_t sleep_retention_passive_module_free(sleep_retention_module_t module)
{
    assert(module >= SLEEP_RETENTION_MODULE_MIN && module <= SLEEP_RETENTION_MODULE_MAX);

    esp_err_t err = ESP_OK;
    _lock_acquire_recursive(&s_retention.lock);
    assert(module_is_passive(&s_retention.instance[module]) && "Illegal dependency");
    assert(module_is_inited(module) && "All passive module must be inited first!");
    if (module_is_created(module)) {
        if (!references_exist(&s_retention.instance[module])) {
            sleep_retention_entries_destroy(module);

            sleep_retention_module_bitmap_t depends = get_dependencies(&s_retention.instance[module]);
            for (int i = 0; ((err == ESP_OK) && (i < SLEEP_RETENTION_MODULE_BITMAP_SZ)); i++) {
                uint32_t bitmap = depends.bitmap[i];
                for (int j = 0; (err == ESP_OK) && bitmap; bitmap >>= 1, j++) {
                    if (bitmap & BIT(0)) {
                        clr_reference(&s_retention.instance[(i << 5) + j], module);
                        err = sleep_retention_passive_module_free((i << 5) + j);
                    }
                }
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
    if (!module_is_passive(&s_retention.instance[module])) {
        if (module_is_inited(module) && module_is_created(module)) {
            sleep_retention_entries_destroy(module);

            sleep_retention_module_bitmap_t depends = get_dependencies(&s_retention.instance[module]);
            for (int i = 0; ((err == ESP_OK) && (i < SLEEP_RETENTION_MODULE_BITMAP_SZ)); i++) {
                uint32_t bitmap = depends.bitmap[i];
                for (int j = 0; (err == ESP_OK) && bitmap; bitmap >>= 1, j++) {
                    if (bitmap & BIT(0)) {
                        clr_reference(&s_retention.instance[(i << 5) + j], module);
                        if (module_is_passive(&s_retention.instance[(i << 5) + j])) {
                            err = sleep_retention_passive_module_free((i << 5) + j);
                        }
                    }
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
    pau_regdma_enable_aon_link_entry(false);
#endif
    // Set extra linked list head pointer to hardware
    pau_regdma_set_extra_link_addr(s_retention.lists[s_retention.highpri].entries[EXTRA_LINK_NUM]);
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
}

#if SOC_PM_RETENTION_SW_TRIGGER_REGDMA
void IRAM_ATTR sleep_retention_do_system_retention(bool backup_or_restore)
{
    #define SYSTEM_LINK_NUM (0)
    if (s_retention.highpri >= SLEEP_RETENTION_REGDMA_LINK_HIGHEST_PRIORITY &&
        s_retention.highpri <= SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY) {
        // Set extra linked list head pointer to hardware
        pau_regdma_set_system_link_addr(s_retention.lists[s_retention.highpri].entries[SYSTEM_LINK_NUM]);
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
