/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ------------------------------------ */
/*     BLE Log - Task-id Registry       */
/*                                      */
/* Owns the name-to-id registry, its    */
/* broadcast sequence and its dedicated */
/* transport; a separate .c/.h pair     */
/* like the UART redirection writer.    */
/* ------------------------------------ */

/* INCLUDE */
#include "ble_log_task_registry.h"
#include "ble_log_rt.h"

#include "freertos/task.h"
#include "esp_timer.h"

/* MACRO */
/* Registry entries are word-compared on the record path. */
#define BLE_LOG_TASK_NAME_WORDS                    (BLE_LOG_TASK_NAME_LEN / sizeof(uint32_t))
/* ble_log_task_lookup() and ble_log_task_register() compare the four name
 * words literally; a different name width must rework them. */
_Static_assert(BLE_LOG_TASK_NAME_WORDS == 4,
               "Unexpected BLE Log task name word count");

/* VARIABLE */
/* Plain static variables (NOBITS .bss): the registry has no IRAM path, and zero
 * page-image bytes beat DRAM_ATTR placement for a table this size. */
BLE_LOG_STATIC uint32_t s_task_names[CONFIG_BLE_LOG_TASK_ID_MAX][BLE_LOG_TASK_NAME_WORDS];
BLE_LOG_STATIC volatile uint8_t s_task_cnt;
BLE_LOG_STATIC ble_log_atomic_lock_t s_task_reg_lock;

/* The binding broadcast keeps its own 24-bit frame sequence, separate
 * from the snapshot sequence (see ble_log_lbm_v2.h): a gap counts a
 * broadcast window skipped despite a non-empty registry, never a lost
 * snapshot. ble_log_task_registry_init resets it. */
BLE_LOG_STATIC uint32_t g_task_binding_sn;
#define BLE_LOG_GET_TASK_BINDING_SN()            BLE_LOG_GET_FRAME_SN(g_task_binding_sn)

/* Module lifetime gate, closed by begin_deinit (LBM teardown starts):
 * publish is system output beyond the public producer gate, and only
 * module teardown stops new broadcasts. The publish path never blocks,
 * so the begin_deinit drain is immediate. */
BLE_LOG_STATIC volatile uint32_t s_registry_ref_count = 0;
BLE_LOG_STATIC uint32_t s_registry_inited = 0;
/* The dedicated single-frame transport. It shares the INTERNAL owner
 * kind: recycled by the peripheral tx-done straight back to FREE, with
 * no pool bookkeeping. */
BLE_LOG_STATIC ble_log_prph_trans_t *binding_trans;

/* ------------------------------- */
/*     Registry Lookup/Insert      */
/* ------------------------------- */
/* - Entries are append-only: an inserter (holding s_task_reg_lock)
 *   writes all name words first and publishes the entry by storing the
 *   new count with RELEASE; readers ACQUIRE-load the count and scan
 *   only the entries below it, so the read path is lock-free.
 * - Names are matched by content, not by TCB pointer, so a recycled
 *   TCB cannot alias two different tasks onto one id.
 * - Names longer than the storage are truncated; tasks sharing the
 *   truncated prefix share an id (identical names are indistinguishable
 *   in the decoded log anyway).
 * - A full registry, or a contended registration attempt, degrades that
 *   record to the unknown id (BLE_LOG_TASK_ID_UNKNOWN) and still emits
 *   it; the task retries the registration with its next record.
 * - Bindings are broadcast on every periodic snapshot window, so a
 *   receiver that joined late or lost a frame converges on the next
 *   window; broadcasts are idempotent on the wire and a lost one
 *   self-heals on the next window. A task that starts logging between
 *   windows emits records tagged with its id; the receiver binds the id
 *   at the next window. */

/* Normalizes a task name to NUL-padded words. Word-level compare
 * replaces strcmp on the record path: the first word acts as the hash,
 * a miss costs one load per entry, and there is no call overhead. Inline:
 * every ENCODE record resolves its writer id, so this runs per record. */
BLE_LOG_STATIC inline void ble_log_task_name_to_words(
    const char *name, uint32_t w[BLE_LOG_TASK_NAME_WORDS])
{
    BLE_LOG_MEMSET(w, 0, BLE_LOG_TASK_NAME_LEN);
    BLE_LOG_MEMCPY(w, name, strnlen(name, BLE_LOG_TASK_NAME_LEN));
}

/* Per-record lookup: inline for the same reason as the normalizer above. */
BLE_LOG_STATIC inline uint8_t ble_log_task_lookup(
    const uint32_t w[BLE_LOG_TASK_NAME_WORDS])
{
    uint8_t cnt = BLE_LOG_ATOMIC_LOAD_ACQUIRE(s_task_cnt);
    for (uint8_t i = 0; i < cnt; i++) {
        if (s_task_names[i][0] != w[0]) {
            continue;
        }
        if (w[1] == s_task_names[i][1] && w[2] == s_task_names[i][2] &&
            w[3] == s_task_names[i][3]) {
            return i;
        }
    }
    return BLE_LOG_TASK_ID_UNKNOWN;
}

/* Cold path, once per task lifetime. A single CAS attempt serializes
 * inserters: on contention this record degrades to the unknown id
 * instead of blocking, and the next record of the task retries. */
BLE_LOG_STATIC uint8_t ble_log_task_register(const uint32_t w[BLE_LOG_TASK_NAME_WORDS])
{
    uint8_t id = BLE_LOG_TASK_ID_UNKNOWN;

    if (!BLE_LOG_CAS_ACQUIRE(&s_task_reg_lock)) {
        return BLE_LOG_TASK_ID_UNKNOWN;
    }
    uint8_t cnt = s_task_cnt;
    for (uint8_t i = 0; i < cnt; i++) {
        if (w[0] == s_task_names[i][0] && w[1] == s_task_names[i][1] &&
            w[2] == s_task_names[i][2] && w[3] == s_task_names[i][3]) {
            BLE_LOG_CAS_RELEASE(&s_task_reg_lock);
            return i;                   /* raced with another first lookup */
        }
    }
    if (cnt < CONFIG_BLE_LOG_TASK_ID_MAX) {
        BLE_LOG_MEMCPY(s_task_names[cnt], w, BLE_LOG_TASK_NAME_LEN);
        id = cnt;
        BLE_LOG_ATOMIC_STORE_RELEASE(s_task_cnt, cnt + 1);
    }
    BLE_LOG_CAS_RELEASE(&s_task_reg_lock);
    return id;
}

uint8_t ble_log_task_id_current(void)
{
    const char *cur_name = pcTaskGetName(NULL);
    if (!cur_name) {
        return BLE_LOG_TASK_ID_UNKNOWN;
    }
    uint32_t w[BLE_LOG_TASK_NAME_WORDS];
    ble_log_task_name_to_words(cur_name, w);
    uint8_t id = ble_log_task_lookup(w);
    /* Registration mutates shared state; ISR callers stop at the
     * lookup-hit path and read only. */
    if (id == BLE_LOG_TASK_ID_UNKNOWN && !BLE_LOG_IN_ISR()) {
        id = ble_log_task_register(w);
    }
    return id;
}

/* ----------------------------------- */
/*     Periodic Binding Broadcast      */
/* ----------------------------------- */
/* One binding frame on the dedicated registry transport, packing every
 * registered entry in id order. Best effort, single CAS attempt: a busy
 * transport (the previous broadcast still in DMA) skips this window and
 * the next one rebroadcasts. The registry is only read here (ACQUIRE
 * count, append-only entries), so this races no inserter. */
BLE_LOG_STATIC bool ble_log_task_binding_send(uint8_t cnt, uint32_t timestamp)
{
    /* Snapshot-sequence semantics: the window SN is consumed before
     * contending for the transport, so a skipped window burns it and
     * the gap in the binding sequence counts skipped windows. */
    uint32_t frame_sn = BLE_LOG_GET_TASK_BINDING_SN();
    if (!BLE_LOG_CAS_ACQUIRE(&binding_trans->atomic_lock)) {
        return false;
    }
    /* Acceptance load: pairs with the dedicated transport's lock-free
     * recycle publication (pos=0, STORE_RELEASE(FREE)). */
    if (BLE_LOG_ATOMIC_LOAD_ACQUIRE(binding_trans->state) !=
        BLE_LOG_TRANS_STATE_FREE) {
        BLE_LOG_CAS_RELEASE(&binding_trans->atomic_lock);
        return false;
    }

    ble_log_frame_head_t frame_head = {
        .length = (uint16_t)(sizeof(uint32_t) +
                             cnt * sizeof(ble_log_task_binding_t)),
        .frame_meta = BLE_LOG_MAKE_FRAME_META(BLE_LOG_SRC_INTERNAL, frame_sn),
    };
    uint8_t *buf = binding_trans->buf;
    BLE_LOG_MEMCPY(buf, &frame_head, sizeof(frame_head));
    size_t pos = BLE_LOG_FRAME_HEAD_LEN;
    BLE_LOG_MEMCPY(buf + pos, &timestamp, sizeof(timestamp));
    pos += sizeof(timestamp);
    for (uint8_t i = 0; i < cnt; i++) {
        ble_log_task_binding_t binding = {
            .int_src_code = BLE_LOG_INT_SRC_TASK_BINDING,
            .task_id = i,
        };
        BLE_LOG_MEMCPY(binding.task_name, s_task_names[i],
                       BLE_LOG_TASK_NAME_LEN);
        BLE_LOG_MEMCPY(buf + pos, &binding, sizeof(binding));
        pos += sizeof(binding);
    }
    uint32_t checksum = ble_log_fast_checksum(
        buf, BLE_LOG_FRAME_HEAD_LEN + frame_head.length);
    BLE_LOG_MEMCPY(buf + BLE_LOG_FRAME_HEAD_LEN + frame_head.length,
                   &checksum, sizeof(checksum));

    binding_trans->pos = (uint16_t)(pos + BLE_LOG_FRAME_TAIL_LEN);
    BLE_LOG_ATOMIC_STORE_RELAXED(binding_trans->state,
                                 BLE_LOG_TRANS_STATE_SENDING);
    BLE_LOG_CAS_RELEASE(&binding_trans->atomic_lock);
    ble_log_rt_submit_trans(binding_trans);
    return true;
}

void ble_log_task_bindings_publish(void)
{
    if (!ble_log_ref_count_try_acquire(&s_registry_ref_count, &s_registry_inited)) {
        return;
    }
    /* Empty registry: nothing to broadcast; no window attempt, no SN. */
    uint8_t cnt = BLE_LOG_ATOMIC_LOAD_ACQUIRE(s_task_cnt);
    if (cnt != 0) {
        uint32_t timestamp = (uint32_t)esp_timer_get_time();
        (void)ble_log_task_binding_send(cnt, timestamp);
    }
    BLE_LOG_REF_COUNT_RELEASE(&s_registry_ref_count);
}

/* --------------------------- */
/*     Module Lifetime         */
/* --------------------------- */
bool ble_log_task_registry_init(void)
{
    if (BLE_LOG_ATOMIC_LOAD_ACQUIRE(s_registry_inited)) {
        return true;
    }

    /* Fresh epoch: empty registry, no broadcast sequence. init runs
     * single-threaded (from ble_log_lbm_init), so the registry state and
     * its lock are cleared by value. */
    BLE_LOG_MEMSET(s_task_names, 0, sizeof(s_task_names));
    BLE_LOG_ATOMIC_STORE_RELEASE(s_task_cnt, 0);
    s_task_reg_lock = 0;
    g_task_binding_sn = 0;

    if (!ble_log_prph_trans_init(&binding_trans, BLE_LOG_TASK_BINDING_TRANS_SIZE)) {
        return false;
    }
    binding_trans->id = BLE_LOG_TRANS_ID_NONE;
    binding_trans->owner_kind = BLE_LOG_TRANS_OWNER_INTERNAL;

    BLE_LOG_ATOMIC_STORE_RELEASE(s_registry_inited, true);
    return true;
}

void ble_log_task_registry_begin_deinit(void)
{
    /* Closing gate: pairs with the seq_cst acquire in publish. */
    BLE_LOG_ATOMIC_STORE_SEQ_CST(s_registry_inited, false);
    while (!ble_log_ref_count_wait(&s_registry_ref_count, 0)) {
        BLE_LOG_ASSERT(false);
    }
}

void ble_log_task_registry_deinit(void)
{
    ble_log_task_registry_begin_deinit();
    ble_log_prph_trans_deinit(&binding_trans);
}

#if CONFIG_BLE_LOG_PRPH_TEST
/* Test-only: wipes the task registry so test cases stay order-independent.
 * Call between cases, with no writers in flight. */
void ble_log_test_task_registry_reset(void)
{
    if (BLE_LOG_CAS_ACQUIRE(&s_task_reg_lock)) {
        BLE_LOG_ATOMIC_STORE_RELEASE(s_task_cnt, 0);
        BLE_LOG_CAS_RELEASE(&s_task_reg_lock);
    }
}
#endif
