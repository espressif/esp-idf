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

static __attribute__((unused)) const char *TAG = "sleep";

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
    self->dependents = 0;
    self->references = 0;
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
    self->dependents = 0;
}

static inline sleep_retention_module_bitmap_t get_dependencies(struct sleep_retention_module_object * const self)
{
    return self->dependents;
}

static inline void set_reference(struct sleep_retention_module_object * const self, sleep_retention_module_t module)
{
    self->references |= BIT(module);
}

static inline void clr_reference(struct sleep_retention_module_object * const self, sleep_retention_module_t module)
{
    self->references &= ~BIT(module);
}

static inline sleep_retention_module_bitmap_t get_references(struct sleep_retention_module_object * const self)
{
    return self->references;
}

static inline bool references_exist(struct sleep_retention_module_object * const self)
{
    return (get_references(self) != 0);
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
    return (sleep_retention_get_inited_modules() & BIT(module)) ? true : false;
}

static inline bool module_is_created(sleep_retention_module_t module)
{
    return (sleep_retention_get_created_modules() & BIT(module)) ? true : false;
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
        uint32_t entries_bitmap: REGDMA_LINK_ENTRY_NUM,
                 runtime_bitmap: REGDMA_LINK_ENTRY_NUM,
                 reserved: 32-(2*REGDMA_LINK_ENTRY_NUM);
        void *entries_tail;
    } lists[SLEEP_RETENTION_REGDMA_LINK_NR_PRIORITIES];
    _lock_t lock;
    regdma_link_priority_t highpri;
    uint32_t inited_modules;
    uint32_t created_modules;

    struct sleep_retention_module_object instance[32];

#if SOC_PM_RETENTION_HAS_CLOCK_BUG
#define EXTRA_LINK_NUM  (REGDMA_LINK_ENTRY_NUM - 1)
#endif
} sleep_retention_t;

static DRAM_ATTR __attribute__((unused)) sleep_retention_t s_retention = {
    .highpri = (uint8_t)-1, .inited_modules = 0, .created_modules = 0
};

#define SLEEP_RETENTION_ENTRY_BITMAP_MASK       (BIT(REGDMA_LINK_ENTRY_NUM) - 1)
#define SLEEP_RETENTION_ENTRY_BITMAP(bitmap)    ((bitmap) & SLEEP_RETENTION_ENTRY_BITMAP_MASK)

static esp_err_t sleep_retention_entries_create_impl(const sleep_retention_entries_config_t retent[], int num, regdma_link_priority_t priority, sleep_retention_module_t module);
static void sleep_retention_entries_join(void);

static inline sleep_retention_module_bitmap_t module_num2map(sleep_retention_module_t module)
{
    return (module == SLEEP_RETENTION_MODULE_INVALID) ? 0 : BIT(module);
}

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
            s_retention.lists[priority].entries_bitmap, priority, module_num2map(module)) == ESP_OK) { /* branch node can't as tail node */
            link = regdma_link_init_safe(
                        config, true, module_num2map(module),
                        (owner & BIT(0)) ? s_retention.lists[priority].entries[0] : NULL,
                        (owner & BIT(1)) ? s_retention.lists[priority].entries[1] : NULL,
                        (owner & BIT(2)) ? s_retention.lists[priority].entries[2] : NULL,
                        (owner & BIT(3)) ? s_retention.lists[priority].entries[3] : NULL
                    );
        }
    } else {
        link = regdma_link_init_safe(config, false, module_num2map(module), s_retention.lists[priority].entries[__builtin_ffs(owner) - 1]);
    }
    _lock_release_recursive(&s_retention.lock);
    return link;
}

static void * sleep_retention_entries_try_create_bonding(const regdma_link_config_t *config, uint32_t owner, regdma_link_priority_t priority, sleep_retention_module_t module)
{
    assert(owner > 0 && owner < BIT(REGDMA_LINK_ENTRY_NUM));
    _lock_acquire_recursive(&s_retention.lock);
    void *link = regdma_link_init_safe(
                config, true, module_num2map(module),
                (owner & BIT(0)) ? s_retention.lists[priority].entries[0] : NULL,
                (owner & BIT(1)) ? s_retention.lists[priority].entries[1] : NULL,
                (owner & BIT(2)) ? s_retention.lists[priority].entries[2] : NULL,
                (owner & BIT(3)) ? s_retention.lists[priority].entries[3] : NULL
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

#if REGDMA_LINK_DBG
void sleep_retention_dump_entries(FILE *out)
{
    _lock_acquire_recursive(&s_retention.lock);
    if (s_retention.highpri >= SLEEP_RETENTION_REGDMA_LINK_HIGHEST_PRIORITY && s_retention.highpri <= SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY) {
        for (int entry = 0; entry < ARRAY_SIZE(s_retention.lists[s_retention.highpri].entries); entry++) {
            fprintf(out, "\nsleep retention entries[%d] context:\n", entry);
            regdma_link_dump(out, s_retention.lists[s_retention.highpri].entries[entry], entry);
        }
    }
    _lock_release_recursive(&s_retention.lock);
}
#endif

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
                s_retention.lists[priority].entries[entry], s_retention.lists[priority].entries_tail, entry, module_num2map(module));
        destroy_tails     [entry] = regdma_find_module_link_tail(
                s_retention.lists[priority].entries[entry], s_retention.lists[priority].entries_tail, entry, module_num2map(module));
        (*next_entries)   [entry] = regdma_find_next_module_link_head(
                s_retention.lists[priority].entries[entry], s_retention.lists[priority].entries_tail, entry, module_num2map(module));
        prev_tails        [entry] = regdma_find_prev_module_link_tail(
                s_retention.lists[priority].entries[entry], s_retention.lists[priority].entries_tail, entry, module_num2map(module));
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
        regdma_link_update_next_safe(prev_tail, (*next_entries)[0], (*next_entries)[1], (*next_entries)[2], (*next_entries)[3]);
    }
    sleep_retention_entries_context_update(priority);

    regdma_link_update_next_safe(destroy_tail, NULL, NULL, NULL, NULL);
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
    assert(s_retention.created_modules == 0);
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
    s_retention.created_modules &= ~module_num2map(module);
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
    if (s_retention.created_modules == 0) {
        sleep_retention_entries_check_and_distroy_final_default();
        pmu_sleep_disable_regdma_backup();
        memset((void *)s_retention.lists, 0, sizeof(s_retention.lists));
        s_retention.highpri = (uint8_t)-1;
    }
    _lock_release_recursive(&s_retention.lock);
}

static esp_err_t sleep_retention_entries_create_impl(const sleep_retention_entries_config_t retent[], int num, regdma_link_priority_t priority, sleep_retention_module_t module)
{
    _lock_acquire_recursive(&s_retention.lock);
    for (int i = num - 1; i >= 0; i--) {
#if SOC_PM_RETENTION_HAS_CLOCK_BUG
        if ((retent[i].owner > BIT(EXTRA_LINK_NUM)) && (retent[i].config.id != 0xffff)) {
            _lock_release_recursive(&s_retention.lock);
            sleep_retention_entries_do_destroy(module);
            return ESP_ERR_NOT_SUPPORTED;
        }
#endif
        void *link = sleep_retention_entries_try_create(&retent[i].config, retent[i].owner, priority, module);
        if (link == NULL) {
            _lock_release_recursive(&s_retention.lock);
            sleep_retention_entries_do_destroy(module);
            return ESP_ERR_NO_MEM;
        }
        sleep_retention_entries_update(retent[i].owner, link, priority);
    }
    _lock_release_recursive(&s_retention.lock);
    return ESP_OK;
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
    s_retention.created_modules |= module_num2map(module);
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

uint32_t IRAM_ATTR sleep_retention_get_inited_modules(void)
{
    return s_retention.inited_modules;
}

uint32_t IRAM_ATTR sleep_retention_get_created_modules(void)
{
    return s_retention.created_modules;
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
        s_retention.inited_modules |= module_num2map(module);
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
        s_retention.inited_modules &= ~module_num2map(module);
        do_lock_release = (sleep_retention_get_inited_modules() == 0);
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
        for (int i = 0; (err == ESP_OK) && depends; depends >>= 1, i++) {
            if (depends & BIT(0)) {
                set_reference(&s_retention.instance[i], module);
                err = sleep_retention_passive_module_allocate(i);
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
            for (int i = 0; (err == ESP_OK) && depends; depends >>= 1, i++) {
                if (depends & BIT(0)) {
                    set_reference(&s_retention.instance[i], module);
                    if (module_is_passive(&s_retention.instance[i])) { /* the callee ensures this module is inited */
                        err = sleep_retention_passive_module_allocate(i);
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
            for (int i = 0; (err == ESP_OK) && depends; depends >>= 1, i++) {
                if (depends & BIT(0)) {
                    clr_reference(&s_retention.instance[i], module);
                    err = sleep_retention_passive_module_free(i);
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
            for (int i = 0; (err == ESP_OK) && depends; depends >>= 1, i++) {
                if (depends & BIT(0)) {
                    clr_reference(&s_retention.instance[i], module);
                    if (module_is_passive(&s_retention.instance[i])) {
                        err = sleep_retention_passive_module_free(i);
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

#if SOC_PM_RETENTION_HAS_CLOCK_BUG
void IRAM_ATTR sleep_retention_do_extra_retention(bool backup_or_restore)
{
    if (s_retention.highpri < SLEEP_RETENTION_REGDMA_LINK_HIGHEST_PRIORITY ||
        s_retention.highpri > SLEEP_RETENTION_REGDMA_LINK_LOWEST_PRIORITY) {
        return;
    }
    // Set extra linked list head pointer to hardware
    pau_regdma_set_extra_link_addr(s_retention.lists[s_retention.highpri].entries[EXTRA_LINK_NUM]);
    if (backup_or_restore) {
        pau_regdma_trigger_extra_link_backup();
    } else {
        pau_regdma_trigger_extra_link_restore();
    }
}
#endif

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
