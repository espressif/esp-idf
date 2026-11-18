/******************************************************************************
 *
 *  Controller-to-Host ACL credit.
 *  Host (BTU) accounts into host tab and handoffs; HCI merges mail + pending
 *  and btsnd. Host and HCI resources have independent lifecycles.
 *
 ******************************************************************************/
#include "common/bt_target.h"

#if (C2H_FLOW_CONTROL_INCLUDED == TRUE)

#include <string.h>

#include "common/bt_trace.h"
#include "hci/hci_layer.h"
#include "osi/alarm.h"
#include "osi/allocator.h"
#include "osi/mutex.h"
#include "osi/thread.h"
#include "stack/btu.h"
#include "stack/c2h_credit.h"
#include "stack/hcimsgs.h"

#define C2H_CREDIT_FLAG_ENABLED  0x01
#define C2H_CREDIT_FLAG_CLOSING  0x02
#define C2H_CREDIT_FLAG_PAUSED   0x04

#define C2H_CREDIT_FLUSH_MIN       (4)
#define C2H_CREDIT_DEFER_DELAY_MS  (10)
#define C2H_CREDIT_RETRY_DELAY_MS  (100)

typedef struct tC2H_DYNAMIC_ENTRY {
    UINT16 handle;
    UINT16 num;
    struct tC2H_DYNAMIC_ENTRY *next;
} tC2H_DYNAMIC_ENTRY;

typedef struct {
    UINT16 handles[C2H_CREDIT_TAB_CAP];
    UINT16 num_pkts[C2H_CREDIT_TAB_CAP];
    UINT8  ents;
    UINT32 pending_pkts;
    tC2H_DYNAMIC_ENTRY *dynamic;
} tC2H_CREDIT_TAB;

typedef struct {
    tC2H_CREDIT_TAB tab;
    UINT8 flags;
    struct osi_event *drain_ready; /* alarm → BTU: handoff host then kick HCI */
} tC2H_CREDIT_HOST;

typedef struct {
    tC2H_CREDIT_TAB pending;
    tC2H_CREDIT_TAB mail;
    osi_mutex_t lock; /* pending + mail + force_send + closing */
    BOOLEAN closing;
    BOOLEAN force_send;
    osi_alarm_t *send_alarm; /* defer 10ms or retry 100ms */
    struct osi_event *flush_ready;
} tC2H_CREDIT_HCI;

static tC2H_CREDIT_HOST *s_host = NULL;
static tC2H_CREDIT_HCI *s_hci = NULL;

#if defined(BT_BLUEDROID_UNIT_TEST)
/* Default true: unit harness has no HCI command_queue. */
static bool s_force_btsnd_fail = true;
#endif

static void hci_c2h_credit_flush_now(BOOLEAN force);
static void hci_c2h_credit_kick(BOOLEAN force);
static void host_drain_and_kick(void);

#if defined(BT_BLUEDROID_UNIT_TEST)
static bool c2h_btsnd_host_num_xmitted_pkts(UINT8 num_ents, UINT16 *handles, UINT16 *num_pkts)
{
    UNUSED(num_ents);
    UNUSED(handles);
    UNUSED(num_pkts);

    /* Never touch hci_host_env.command_queue in the unit-test harness. */
    return !s_force_btsnd_fail;
}
#else
static bool c2h_btsnd_host_num_xmitted_pkts(UINT8 num_ents, UINT16 *handles, UINT16 *num_pkts)
{
    return btsnd_hcic_host_num_xmitted_pkts(num_ents, handles, num_pkts);
}
#endif

static void hci_lock(void)
{
    osi_mutex_lock(&s_hci->lock, OSI_MUTEX_MAX_TIMEOUT);
}

static void hci_unlock(void)
{
    osi_mutex_unlock(&s_hci->lock);
}

static UINT16 tab_sat_add(UINT16 a, UINT16 b)
{
    UINT32 sum = (UINT32)a + b;

    return (sum > 0xFFFF) ? 0xFFFF : (UINT16)sum;
}

static void tab_dynamic_free(tC2H_DYNAMIC_ENTRY *node)
{
    while (node != NULL) {
        tC2H_DYNAMIC_ENTRY *next = node->next;

        osi_free(node);
        node = next;
    }
}

static void tab_recalc(tC2H_CREDIT_TAB *tab)
{
    UINT8 i;
    UINT32 sum = 0;
    tC2H_DYNAMIC_ENTRY *node;

    for (i = 0; i < tab->ents; i++) {
        sum += tab->num_pkts[i];
    }

    for (node = tab->dynamic; node != NULL; node = node->next) {
        sum += node->num;
    }

    tab->pending_pkts = sum;
}

static void tab_clear(tC2H_CREDIT_TAB *tab)
{
    if (tab == NULL) {
        return;
    }

    tab_dynamic_free(tab->dynamic);
    tab->dynamic = NULL;
    tab->ents = 0;
    tab->pending_pkts = 0;
}

/* dst must be empty: shallow copy transfers src->dynamic and drops any list dst owns. */
static void tab_move(tC2H_CREDIT_TAB *dst, tC2H_CREDIT_TAB *src)
{
    *dst = *src;
    src->ents = 0;
    src->pending_pkts = 0;
    src->dynamic = NULL;
}

static void tab_compact(tC2H_CREDIT_TAB *tab)
{
    while (tab->ents < C2H_CREDIT_TAB_CAP && tab->dynamic != NULL) {
        tC2H_DYNAMIC_ENTRY *node = tab->dynamic;

        tab->dynamic = node->next;
        tab->handles[tab->ents] = node->handle;
        tab->num_pkts[tab->ents] = node->num;
        tab->ents++;
        osi_free(node);
    }
}

static bool tab_coalesce(tC2H_CREDIT_TAB *tab, UINT16 handle, UINT16 num)
{
    UINT8 i;
    tC2H_DYNAMIC_ENTRY *node;
    UINT16 new_pkts;

    for (i = 0; i < tab->ents; i++) {
        if (tab->handles[i] != handle) {
            continue;
        }

        new_pkts = tab_sat_add(tab->num_pkts[i], num);
        tab->pending_pkts += (UINT32)(new_pkts - tab->num_pkts[i]);
        tab->num_pkts[i] = new_pkts;
        return true;
    }

    for (node = tab->dynamic; node != NULL; node = node->next) {
        if (node->handle != handle) {
            continue;
        }

        new_pkts = tab_sat_add(node->num, num);
        tab->pending_pkts += (UINT32)(new_pkts - node->num);
        node->num = new_pkts;
        return true;
    }

    return false;
}

static bool tab_has_handle(const tC2H_CREDIT_TAB *tab, UINT16 handle)
{
    UINT8 i;
    const tC2H_DYNAMIC_ENTRY *node;

    handle = HCID_GET_HANDLE(handle);

    for (i = 0; i < tab->ents; i++) {
        if (tab->handles[i] == handle) {
            return true;
        }
    }

    for (node = tab->dynamic; node != NULL; node = node->next) {
        if (node->handle == handle) {
            return true;
        }
    }

    return false;
}

static bool tab_add(tC2H_CREDIT_TAB *tab, UINT16 handle, UINT16 num)
{
    tC2H_DYNAMIC_ENTRY *node;

    if (tab == NULL || num == 0) {
        return true;
    }

    handle = HCID_GET_HANDLE(handle);

    if (tab_coalesce(tab, handle, num)) {
        return true;
    }

    if (tab->ents < C2H_CREDIT_TAB_CAP) {
        tab->handles[tab->ents] = handle;
        tab->num_pkts[tab->ents] = num;
        tab->ents++;
        tab->pending_pkts += num;
        return true;
    }

    node = (tC2H_DYNAMIC_ENTRY *)osi_malloc(sizeof(*node));
    if (node == NULL) {
        /* Intentional: OOM on tC2H_DYNAMIC_ENTRY means the system cannot run. */
        assert(0);
        return false;
    }

    node->handle = handle;
    node->num = num;
    node->next = tab->dynamic;
    tab->dynamic = node;
    tab->pending_pkts += num;
    return true;
}

static UINT16 tab_detach(tC2H_CREDIT_TAB *tab, UINT16 handle)
{
    UINT8 i;
    tC2H_DYNAMIC_ENTRY *node;
    tC2H_DYNAMIC_ENTRY *prev = NULL;
    UINT16 pkts;

    if (tab == NULL) {
        return 0;
    }

    handle = HCID_GET_HANDLE(handle);

    for (i = 0; i < tab->ents; i++) {
        if (tab->handles[i] != handle) {
            continue;
        }

        pkts = tab->num_pkts[i];
        tab->ents--;
        /* Keep index < CAP for -Warray-bounds when CAP==1. */
        if (i < tab->ents && tab->ents < C2H_CREDIT_TAB_CAP) {
            tab->handles[i] = tab->handles[tab->ents];
            tab->num_pkts[i] = tab->num_pkts[tab->ents];
        }

        tab_compact(tab);
        tab_recalc(tab);
        return pkts;
    }

    for (node = tab->dynamic; node != NULL; prev = node, node = node->next) {
        if (node->handle != handle) {
            continue;
        }

        pkts = node->num;
        if (prev == NULL) {
            tab->dynamic = node->next;
        } else {
            prev->next = node->next;
        }

        osi_free(node);
        tab_recalc(tab);
        return pkts;
    }

    return 0;
}

/* Moves src into dst. OOM leftovers stay in src. */
static void tab_merge(tC2H_CREDIT_TAB *dst, tC2H_CREDIT_TAB *src)
{
    UINT8 i;
    UINT8 kept = 0;
    tC2H_DYNAMIC_ENTRY *node;
    tC2H_DYNAMIC_ENTRY *next;

    if (dst == NULL || src == NULL || src == dst) {
        return;
    }

    for (i = 0; i < src->ents; i++) {
        if (tab_add(dst, src->handles[i], src->num_pkts[i])) {
            continue;
        }

        if (kept != i) {
            src->handles[kept] = src->handles[i];
            src->num_pkts[kept] = src->num_pkts[i];
        }
        kept++;
    }
    src->ents = kept;

    node = src->dynamic;
    src->dynamic = NULL;

    while (node != NULL) {
        next = node->next;

        if (tab_coalesce(dst, node->handle, node->num)) {
            osi_free(node);
        } else if (dst->ents < C2H_CREDIT_TAB_CAP) {
            dst->handles[dst->ents] = node->handle;
            dst->num_pkts[dst->ents] = node->num;
            dst->ents++;
            dst->pending_pkts += node->num;
            osi_free(node);
        } else {
            node->next = dst->dynamic;
            dst->dynamic = node;
            dst->pending_pkts += node->num;
        }

        node = next;
    }

    tab_recalc(src);
    tab_recalc(dst);
}

static bool tab_extract_cmd(tC2H_CREDIT_TAB *tab, UINT8 *num_ents,
                            UINT16 *handles, UINT16 *num_pkts)
{
    UINT8 n;

    if (tab->ents == 0) {
        tab_compact(tab);
    }

    if (tab->ents == 0) {
        *num_ents = 0;
        return false;
    }

    n = tab->ents;
    memcpy(handles, tab->handles, n * sizeof(UINT16));
    memcpy(num_pkts, tab->num_pkts, n * sizeof(UINT16));
    tab->ents = 0;

    tab_compact(tab);
    tab_recalc(tab);

    *num_ents = n;
    return true;
}

static BOOLEAN host_blocked(void)
{
    return (s_host->flags & (C2H_CREDIT_FLAG_CLOSING | C2H_CREDIT_FLAG_PAUSED)) ||
           btu_c2h_acl_is_congested();
}

static UINT32 host_handoff_to_mail(void)
{
    tC2H_CREDIT_TAB snap;
    UINT32 handed;

    if (s_host == NULL || s_hci == NULL || s_host->tab.pending_pkts == 0) {
        return 0;
    }

    tab_move(&snap, &s_host->tab);
    handed = snap.pending_pkts;

    hci_lock();
    if (!s_hci->closing) {
        tab_merge(&s_hci->mail, &snap);
    } else {
        tab_clear(&snap);
        handed = 0;
    }
    hci_unlock();

    if (snap.pending_pkts != 0) {
        handed -= snap.pending_pkts;
        tab_merge(&s_host->tab, &snap);
        tab_clear(&snap);
    }

    return handed;
}

static void hci_cancel_send_alarm(void)
{
    if (s_hci != NULL && s_hci->send_alarm != NULL &&
            osi_alarm_is_active(s_hci->send_alarm)) {
        osi_alarm_cancel(s_hci->send_alarm);
    }
}

static void hci_schedule_send(UINT32 delay_ms)
{
    BOOLEAN closing;

    if (s_hci == NULL || s_hci->send_alarm == NULL) {
        return;
    }

    hci_lock();
    closing = s_hci->closing;
    hci_unlock();

    if (closing) {
        return;
    }

    /*
     * Prefer the earlier deadline: keep the current one if it fires sooner,
     * otherwise cancel and re-arm (e.g. 100 ms retry → 10 ms defer).
     */
    if (osi_alarm_is_active(s_hci->send_alarm)) {
        UINT32 remaining = (UINT32)osi_alarm_get_remaining_ms(s_hci->send_alarm);

        if (remaining <= delay_ms) {
            return;
        }
        osi_alarm_cancel(s_hci->send_alarm);
    }

    osi_alarm_set(s_hci->send_alarm, delay_ms);
}

static void hci_c2h_credit_kick(BOOLEAN force)
{
    BOOLEAN closing;

    if (s_hci == NULL) {
        return;
    }

    hci_lock();
    closing = s_hci->closing;
    if (!closing && force) {
        s_hci->force_send = TRUE;
    }
    hci_unlock();

    if (closing) {
        return;
    }

    if (force) {
        hci_cancel_send_alarm();
    }

    if (s_hci->flush_ready != NULL && hci_get_current_thread() != NULL) {
        if (!osi_thread_post_event(s_hci->flush_ready, 0)) {
            hci_schedule_send(C2H_CREDIT_RETRY_DELAY_MS);
        }
        return;
    }

    hci_c2h_credit_flush_now(force);
}

static void tab_restore(tC2H_CREDIT_TAB *tab, UINT8 num_ents,
                        const UINT16 *handles, const UINT16 *num_pkts)
{
    UINT8 i;

    for (i = 0; i < num_ents; i++) {
        if (!tab_add(tab, handles[i], num_pkts[i])) {
            HCI_TRACE_ERROR("c2h credit restore failed handle=0x%x pkts=%u",
                            handles[i], num_pkts[i]);
        }
    }
}

static void hci_c2h_credit_flush_now(BOOLEAN force)
{
    UINT16 handles[C2H_CREDIT_TAB_CAP];
    UINT16 num_pkts[C2H_CREDIT_TAB_CAP];
    UINT8 num_ents;
    UINT32 pending_pkts;
    BOOLEAN alarm_active;

    if (s_hci == NULL) {
        return;
    }

    hci_lock();
    tab_merge(&s_hci->pending, &s_hci->mail);
    pending_pkts = s_hci->pending.pending_pkts;
    alarm_active = (s_hci->send_alarm != NULL && osi_alarm_is_active(s_hci->send_alarm));
    hci_unlock();

    if (pending_pkts == 0) {
        return;
    }

    if (!force && pending_pkts < C2H_CREDIT_FLUSH_MIN) {
        if (!alarm_active) {
            hci_schedule_send(C2H_CREDIT_DEFER_DELAY_MS);
        }
        return;
    }

    hci_cancel_send_alarm();

    for (;;) {
        hci_lock();
        if (!tab_extract_cmd(&s_hci->pending, &num_ents, handles, num_pkts)) {
            hci_unlock();
            return;
        }
        hci_unlock();

        if (!c2h_btsnd_host_num_xmitted_pkts(num_ents, handles, num_pkts)) {
            hci_lock();
            tab_restore(&s_hci->pending, num_ents, handles, num_pkts);
            hci_unlock();

            hci_schedule_send(C2H_CREDIT_RETRY_DELAY_MS);
            return;
        }

        if (!force) {
            hci_lock();
            pending_pkts = s_hci->pending.pending_pkts;
            hci_unlock();

            if (pending_pkts != 0) {
                hci_schedule_send(C2H_CREDIT_DEFER_DELAY_MS);
            }
            return;
        }
    }
}

static void hci_flush_ready_handler(void *param)
{
    BOOLEAN force = FALSE;

    UNUSED(param);

    if (s_hci != NULL) {
        hci_lock();
        force = s_hci->force_send;
        s_hci->force_send = FALSE;
        hci_unlock();
    }

    hci_c2h_credit_flush_now(force);
}

static BOOLEAN hci_tabs_empty(void)
{
    BOOLEAN empty;

    if (s_hci == NULL) {
        return TRUE;
    }

    hci_lock();
    empty = (s_hci->mail.pending_pkts == 0 && s_hci->pending.pending_pkts == 0);
    hci_unlock();

    return empty;
}

static void host_drain_and_kick(void)
{
    BOOLEAN handed = FALSE;

    if (s_host != NULL && !host_blocked() && s_host->tab.pending_pkts != 0) {
        handed = (host_handoff_to_mail() != 0);
    }

    /* Stale drain_ready after HCI already sent: skip the extra kick. */
    if (!handed && hci_tabs_empty()) {
        return;
    }

    hci_c2h_credit_kick(TRUE);
}

static void host_drain_ready_handler(void *param)
{
    UNUSED(param);
    host_drain_and_kick();
}

static void hci_send_alarm_cb(void *param)
{
    BOOLEAN closing;

    UNUSED(param);

    if (s_hci == NULL) {
        return;
    }

    hci_lock();
    closing = s_hci->closing;
    hci_unlock();

    if (closing) {
        return;
    }

    /* Host tab is BTU-owned: never drain it on the alarm thread. */
    if (s_host != NULL && s_host->drain_ready != NULL && btu_get_current_thread() != NULL) {
        if (osi_thread_post_event(s_host->drain_ready, 0)) {
            return;
        }
        hci_schedule_send(C2H_CREDIT_RETRY_DELAY_MS);
    }

    hci_c2h_credit_kick(TRUE);
}

static bool hci_bind_flush_ready(void)
{
    osi_thread_t *thread = hci_get_current_thread();

    if (s_hci->flush_ready != NULL) {
        return true;
    }

#if defined(BT_BLUEDROID_UNIT_TEST)
    /* Harness never starts hciT; bind to the test worker so init cannot
     * succeed with a NULL flush_ready. Kick still inlines when
     * hci_get_current_thread() is NULL. */
    if (thread == NULL) {
        thread = btu_get_current_thread();
    }
#endif

    if (thread == NULL) {
        return false;
    }

    s_hci->flush_ready = osi_event_create(hci_flush_ready_handler, NULL);
    if (s_hci->flush_ready == NULL) {
        return false;
    }

    if (!osi_event_bind(s_hci->flush_ready, thread, 0)) {
        osi_event_delete(s_hci->flush_ready);
        s_hci->flush_ready = NULL;
        return false;
    }

    return true;
}

bool hci_c2h_credit_init(void)
{
    if (s_hci == NULL) {
        s_hci = (tC2H_CREDIT_HCI *)osi_malloc(sizeof(*s_hci));
        if (s_hci == NULL) {
            return false;
        }

        memset(s_hci, 0, sizeof(*s_hci));
        if (osi_mutex_new(&s_hci->lock) != 0) {
            osi_free(s_hci);
            s_hci = NULL;
            return false;
        }
    }

#if defined(BT_BLUEDROID_UNIT_TEST)
    s_force_btsnd_fail = true;
#endif

    hci_lock();
    s_hci->closing = FALSE;
    s_hci->force_send = FALSE;
    tab_clear(&s_hci->pending);
    tab_clear(&s_hci->mail);
    hci_unlock();

    if (s_hci->send_alarm == NULL) {
        s_hci->send_alarm = osi_alarm_new("c2h_send", hci_send_alarm_cb, NULL, 0);
        if (s_hci->send_alarm == NULL) {
            HCI_TRACE_ERROR("c2h credit send_alarm create failed");
            hci_c2h_credit_deinit();
            return false;
        }
    }

    if (!hci_bind_flush_ready()) {
        HCI_TRACE_ERROR("c2h credit bind flush_ready failed");
        hci_c2h_credit_deinit();
        return false;
    }

    return true;
}

void hci_c2h_credit_close(void)
{
    osi_alarm_t *send_alarm;
    struct osi_event *flush_ready;

    if (s_hci == NULL) {
        return;
    }

    hci_cancel_send_alarm();

    /* Send mail + pending before refusing new credits. */
    hci_c2h_credit_flush_now(TRUE);

    hci_lock();
    s_hci->closing = TRUE;
    s_hci->force_send = FALSE;
    hci_unlock();

    /* Catch account/handoff that raced in before closing was set. */
    hci_c2h_credit_flush_now(TRUE);

    hci_lock();
    tab_clear(&s_hci->mail);
    tab_clear(&s_hci->pending);
    hci_unlock();

    send_alarm = s_hci->send_alarm;
    s_hci->send_alarm = NULL;
    flush_ready = s_hci->flush_ready;
    s_hci->flush_ready = NULL;

    if (send_alarm != NULL) {
        osi_alarm_free(send_alarm);
    }

    if (flush_ready != NULL) {
        osi_event_delete(flush_ready);
    }
}

void hci_c2h_credit_deinit(void)
{
    if (s_hci == NULL) {
        return;
    }

    hci_c2h_credit_close();
    osi_mutex_free(&s_hci->lock);
    osi_free(s_hci);
    s_hci = NULL;
}

static bool hci_try_account(UINT16 handle, UINT16 num)
{
    bool added;

    hci_lock();

    if (s_hci->closing) {
        hci_unlock();
        return true; /* treat as consumed / ignored */
    }

    /* Prefer sending a CAP batch over growing the dynamic list. */
    if (s_hci->pending.ents >= C2H_CREDIT_TAB_CAP &&
            !tab_has_handle(&s_hci->pending, handle)) {
        hci_unlock();
        hci_c2h_credit_flush_now(TRUE);

        hci_lock();
        if (s_hci->closing) {
            hci_unlock();
            return true;
        }
    }

    added = tab_add(&s_hci->pending, handle, num);
    hci_unlock();

    return added;
}

void hci_c2h_credit_account(UINT16 handle, UINT16 num)
{
    UINT16 h;

    if (s_hci == NULL || num == 0) {
        return;
    }

    if (hci_try_account(handle, num)) {
        return;
    }

    /* Dynamic alloc failed: flush a batch and retry once. */
    hci_c2h_credit_flush_now(TRUE);
    if (hci_try_account(handle, num)) {
        return;
    }

    h = HCID_GET_HANDLE(handle);
    if (c2h_btsnd_host_num_xmitted_pkts(1, &h, &num)) {
        return;
    }

    /* Command buffer exhausted: keep the credit on pending for retry.
     * Do not use hci_try_account here: closing is treated as success there. */
    hci_lock();
    if (!s_hci->closing && !tab_add(&s_hci->pending, h, num)) {
        HCI_TRACE_ERROR("c2h credit restore failed handle=0x%x pkts=%u", h, num);
    }
    hci_unlock();
    hci_schedule_send(C2H_CREDIT_RETRY_DELAY_MS);
}

void hci_c2h_credit_flush(void)
{
    UINT32 pending_pkts;
    BOOLEAN alarm_active;
    BOOLEAN no_alarm;

    if (s_hci == NULL) {
        return;
    }

    hci_lock();
    if (s_hci->closing) {
        hci_unlock();
        return;
    }

    tab_merge(&s_hci->pending, &s_hci->mail);
    pending_pkts = s_hci->pending.pending_pkts;
    alarm_active = (s_hci->send_alarm != NULL && osi_alarm_is_active(s_hci->send_alarm));
    no_alarm = (s_hci->send_alarm == NULL);
    hci_unlock();

    if (pending_pkts == 0) {
        return;
    }

    if (alarm_active && pending_pkts < C2H_CREDIT_FLUSH_MIN) {
        return;
    }

    if (pending_pkts >= C2H_CREDIT_FLUSH_MIN || no_alarm) {
        hci_c2h_credit_flush_now(TRUE);
        return;
    }

    hci_schedule_send(C2H_CREDIT_DEFER_DELAY_MS);
}

bool btu_c2h_credit_init(void)
{
    osi_thread_t *btu_thread;

    if (s_host == NULL) {
        s_host = (tC2H_CREDIT_HOST *)osi_malloc(sizeof(*s_host));
        if (s_host == NULL) {
            return false;
        }
        memset(s_host, 0, sizeof(*s_host));
    }

    s_host->flags = C2H_CREDIT_FLAG_ENABLED;
    tab_clear(&s_host->tab);

    btu_thread = btu_get_current_thread();
    if (btu_thread == NULL) {
        s_host->flags = C2H_CREDIT_FLAG_CLOSING;
        return false;
    }

    if (s_host->drain_ready == NULL) {
        s_host->drain_ready = osi_event_create(host_drain_ready_handler, NULL);
        if (s_host->drain_ready == NULL ||
                !osi_event_bind(s_host->drain_ready, btu_thread, 0)) {
            if (s_host->drain_ready != NULL) {
                osi_event_delete(s_host->drain_ready);
                s_host->drain_ready = NULL;
            }
            s_host->flags = C2H_CREDIT_FLAG_CLOSING;
            return false;
        }
    }

    return true;
}

void btu_c2h_credit_close(void)
{
    struct osi_event *drain_ready;

    if (s_host == NULL) {
        return;
    }

    s_host->flags |= C2H_CREDIT_FLAG_CLOSING;
    /* Handoff while HCI resources may still be alive. */
    (void)host_handoff_to_mail();

    drain_ready = s_host->drain_ready;
    s_host->drain_ready = NULL;
    if (drain_ready != NULL) {
        osi_event_delete(drain_ready);
    }
}

void btu_c2h_credit_deinit(void)
{
    if (s_host == NULL) {
        return;
    }

    s_host->flags = C2H_CREDIT_FLAG_CLOSING;
    tab_clear(&s_host->tab);

    if (s_host->drain_ready != NULL) {
        osi_event_delete(s_host->drain_ready);
        s_host->drain_ready = NULL;
    }

    osi_free(s_host);
    s_host = NULL;
}

void btu_c2h_credit_add(UINT16 handle, UINT16 num)
{
    if (num == 0) {
        return;
    }

    btu_c2h_credit_add_n(1, &handle, &num);
}

static void host_make_room(UINT16 handle)
{
    if (host_blocked()) {
        return;
    }

    if (s_host->tab.ents < C2H_CREDIT_TAB_CAP || tab_has_handle(&s_host->tab, handle)) {
        return;
    }

    (void)host_handoff_to_mail();
    hci_c2h_credit_kick(TRUE);
}

void btu_c2h_credit_add_n(UINT8 num_ents, const UINT16 *handles, const UINT16 *num_pkts)
{
    UINT8 i;

    if (num_ents == 0 || handles == NULL || num_pkts == NULL || s_host == NULL) {
        return;
    }

    if ((s_host->flags & (C2H_CREDIT_FLAG_ENABLED | C2H_CREDIT_FLAG_CLOSING)) !=
            C2H_CREDIT_FLAG_ENABLED) {
        return;
    }

    for (i = 0; i < num_ents; i++) {
        if (num_pkts[i] == 0) {
            continue;
        }

        host_make_room(handles[i]);
        if (tab_add(&s_host->tab, handles[i], num_pkts[i])) {
            continue;
        }

        /* Dynamic alloc failed: hand off and retry once. */
        if (!host_blocked()) {
            (void)host_handoff_to_mail();
            hci_c2h_credit_kick(TRUE);
        }

        if (!tab_add(&s_host->tab, handles[i], num_pkts[i])) {
            HCI_TRACE_ERROR("c2h credit host drop handle=0x%x pkts=%u",
                            HCID_GET_HANDLE(handles[i]), num_pkts[i]);
        }
    }
}

void btu_c2h_credit_post(UINT16 handle, UINT16 num)
{
    if (num == 0) {
        return;
    }

    btu_c2h_credit_add(handle, num);
    btu_c2h_credit_flush();
}

void btu_c2h_credit_flush(void)
{
    if (s_host == NULL || host_blocked() || s_host->tab.pending_pkts == 0) {
        return;
    }

    /*
     * Coalesce on the BTU host tab below FLUSH_MIN to avoid per-packet lock.
     * Arm 10 ms so a slow trickle still drains via BTU→mail→hciT.
     */
    if (s_host->tab.pending_pkts < C2H_CREDIT_FLUSH_MIN) {
        hci_schedule_send(C2H_CREDIT_DEFER_DELAY_MS);
        return;
    }

    if (host_handoff_to_mail() != 0) {
        hci_schedule_send(C2H_CREDIT_DEFER_DELAY_MS);
    }
}

void btu_c2h_credit_flush_and_purge(UINT16 handle)
{
    UINT16 h = HCID_GET_HANDLE(handle);
    UINT16 n = 0;
    bool added = true;
    BOOLEAN closing = FALSE;

    if (s_host != NULL) {
        n = tab_detach(&s_host->tab, h);
    }

    if (s_hci == NULL) {
        if (n != 0 && s_host != NULL && !tab_add(&s_host->tab, h, n)) {
            HCI_TRACE_ERROR("c2h credit purge drop handle=0x%x pkts=%u", h, n);
        }
        return;
    }

    hci_lock();
    closing = s_hci->closing;
    if (!closing && n != 0) {
        added = tab_add(&s_hci->mail, h, n);
        n = added ? 0 : n;
    }
    hci_unlock();

    if (n != 0) {
        /* closing / mail add failed: put credits back on host, else try one-shot. */
        if (s_host != NULL && tab_add(&s_host->tab, h, n)) {
            n = 0;
        } else if (!c2h_btsnd_host_num_xmitted_pkts(1, &h, &n)) {
            HCI_TRACE_ERROR("c2h credit purge drop handle=0x%x pkts=%u", h, n);
        }
    }

    if (!closing) {
        hci_c2h_credit_kick(TRUE);
    }
}

void btu_c2h_credit_reset(void)
{
    if (s_host != NULL) {
        tab_clear(&s_host->tab);
        s_host->flags &= ~C2H_CREDIT_FLAG_PAUSED;
    }

    if (s_hci != NULL) {
        hci_cancel_send_alarm();

        hci_lock();
        tab_clear(&s_hci->mail);
        tab_clear(&s_hci->pending);
        s_hci->force_send = FALSE;
        hci_unlock();
    }
}

void btu_c2h_credit_set_enabled(BOOLEAN enabled)
{
    if (s_host == NULL) {
        return;
    }

    if (enabled) {
        s_host->flags |= C2H_CREDIT_FLAG_ENABLED;
    } else {
        s_host->flags &= ~C2H_CREDIT_FLAG_ENABLED;
    }
}

#if defined(BT_BLUEDROID_UNIT_TEST)
bool c2h_credit_init(void)
{
    if (!hci_c2h_credit_init()) {
        return false;
    }

    if (!btu_c2h_credit_init()) {
        hci_c2h_credit_close();
        return false;
    }

    return true;
}

void c2h_credit_close(void)
{
    btu_c2h_credit_close();
    hci_c2h_credit_close();
}

void c2h_credit_deinit(void)
{
    btu_c2h_credit_deinit();
    hci_c2h_credit_deinit();
}

void btu_c2h_credit_rollback(UINT8 num_ents, UINT16 *handles, UINT16 *num_pkts)
{
    UINT8 i;

    if (num_ents == 0 || handles == NULL || num_pkts == NULL || s_host == NULL) {
        return;
    }

    for (i = 0; i < num_ents; i++) {
        if (num_pkts[i] != 0) {
            (void)tab_add(&s_host->tab, handles[i], num_pkts[i]);
        }
    }

    if (!host_blocked()) {
        (void)host_handoff_to_mail();
        hci_schedule_send(C2H_CREDIT_RETRY_DELAY_MS);
    }
}

UINT8 btu_c2h_credit_pkts_rcvd(UINT16 *num_pkts, UINT16 *handles)
{
    UINT8 n = 0;

    if (num_pkts == NULL || handles == NULL || s_host == NULL ||
            !(s_host->flags & C2H_CREDIT_FLAG_ENABLED)) {
        return 0;
    }

    (void)tab_extract_cmd(&s_host->tab, &n, handles, num_pkts);
    return n;
}

void btu_c2h_credit_pause(void)
{
    if (s_host != NULL) {
        s_host->flags |= C2H_CREDIT_FLAG_PAUSED;
    }
}

void btu_c2h_credit_resume(void)
{
    if (s_host != NULL) {
        s_host->flags &= ~C2H_CREDIT_FLAG_PAUSED;
    }
}

bool hci_c2h_credit_flush_alarm_is_active(void)
{
    return s_hci != NULL && s_hci->send_alarm != NULL &&
           osi_alarm_is_active(s_hci->send_alarm);
}

UINT32 hci_c2h_credit_flush_alarm_remaining_ms(void)
{
    if (s_hci == NULL || s_hci->send_alarm == NULL ||
            !osi_alarm_is_active(s_hci->send_alarm)) {
        return 0;
    }

    return (UINT32)osi_alarm_get_remaining_ms(s_hci->send_alarm);
}

void hci_c2h_credit_test_set_btsnd_fail(bool fail)
{
    s_force_btsnd_fail = fail;
}

UINT32 hci_c2h_credit_pending_pkts(void)
{
    UINT32 pending;

    if (s_hci == NULL) {
        return 0;
    }

    hci_lock();
    pending = s_hci->mail.pending_pkts + s_hci->pending.pending_pkts;
    hci_unlock();
    return pending;
}
#endif /* defined(BT_BLUEDROID_UNIT_TEST) */

#endif /* C2H_FLOW_CONTROL_INCLUDED == TRUE */
