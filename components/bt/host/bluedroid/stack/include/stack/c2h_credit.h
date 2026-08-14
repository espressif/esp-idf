/******************************************************************************
 *
 *  Controller-to-Host ACL credit (Host_Number_Of_Completed_Packets).
 *  Host (BTU) accounts; HCI coalesces and btsnd.
 *  Host and HCI resources have independent lifecycles.
 *
 ******************************************************************************/
#ifndef C2H_CREDIT_H
#define C2H_CREDIT_H

#include "common/bt_target.h"

#if (C2H_FLOW_CONTROL_INCLUDED == TRUE)

#include <stdbool.h>
#include "stack/bt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Match BTU_ACL_QUEUE_BATCH_SIZE (btu.h). Unique handles beyond this go to a
 * heap overflow list so tables do not scale with MAX_ACL. */
#define C2H_CREDIT_TAB_BATCH_CAP  (12)
#define C2H_CREDIT_TAB_CAP                                                                                             \
    (((MAX_L2CAP_LINKS + BTM_MAX_SCO_LINKS) < C2H_CREDIT_TAB_BATCH_CAP)                                                 \
         ? (MAX_L2CAP_LINKS + BTM_MAX_SCO_LINKS)                                                                          \
         : C2H_CREDIT_TAB_BATCH_CAP)

/* HCI-only: pending/mail, flush_ready, send_alarm. Bound to HCI thread. */
bool hci_c2h_credit_init(void);
void hci_c2h_credit_close(void);
void hci_c2h_credit_deinit(void);

/* BTU-only: host tab, drain_ready. Bound to BTU thread. */
bool btu_c2h_credit_init(void);
void btu_c2h_credit_close(void);
void btu_c2h_credit_deinit(void);

void btu_c2h_credit_add(UINT16 handle, UINT16 num);
void btu_c2h_credit_add_n(UINT8 num_ents, const UINT16 *handles, const UINT16 *num_pkts);
void btu_c2h_credit_post(UINT16 handle, UINT16 num);
void btu_c2h_credit_flush(void);
void btu_c2h_credit_flush_and_purge(UINT16 handle);

void btu_c2h_credit_reset(void);
void btu_c2h_credit_set_enabled(BOOLEAN enabled);

/* HCI-only accounting / flush */
void hci_c2h_credit_account(UINT16 handle, UINT16 num);
void hci_c2h_credit_flush(void);

#if defined(BT_BLUEDROID_UNIT_TEST)
bool c2h_credit_init(void);
void c2h_credit_close(void);
void c2h_credit_deinit(void);

void btu_c2h_credit_rollback(UINT8 num_ents, UINT16 *handles, UINT16 *num_pkts);
void btu_c2h_credit_pause(void);
void btu_c2h_credit_resume(void);
UINT8 btu_c2h_credit_pkts_rcvd(UINT16 *num_pkts, UINT16 *handles);
bool hci_c2h_credit_flush_alarm_is_active(void);
UINT32 hci_c2h_credit_flush_alarm_remaining_ms(void);

/* Force btsnd failure (default) or simulated success without a live HCI queue. */
void hci_c2h_credit_test_set_btsnd_fail(bool fail);
/* Sum of HCI mail + pending credits still waiting to be sent. */
UINT32 hci_c2h_credit_pending_pkts(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* C2H_FLOW_CONTROL_INCLUDED == TRUE */

#endif /* C2H_CREDIT_H */
