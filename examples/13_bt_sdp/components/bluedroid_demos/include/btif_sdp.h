#ifndef __BTIF_SDP_H__
#define __BTIF_SDP_H__

#include "bt_sdp.h"

/** Callback for SDP search */
typedef void (*btsdp_search_callback)(bt_status_t status, bt_bdaddr_t *bd_addr, uint8_t* uuid, int num_records, bluetooth_sdp_record *records);

typedef struct {
    btsdp_search_callback  sdp_search_cb;
} btsdp_callbacks_t;

/** Register BT SDP search callbacks */
bt_status_t BTIF_SdpInit(btsdp_callbacks_t *callbacks);

/** Unregister BT SDP */
bt_status_t BTIF_SdpDeinit(void);

/** Search for SDP records with specific uuid on remote device */
bt_status_t BTIF_SdpSearch(bt_bdaddr_t *bd_addr,  const uint8_t* uuid);

/**
 * Use listen in the socket interface to create rfcomm and/or l2cap PSM channels,
 * (without UUID and service_name and set the BTSOCK_FLAG_NO_SDP flag in flags).
 * Then use createSdpRecord to create the SDP record associated with the rfcomm/l2cap channels.
 *
 * Returns a handle to the SDP record, which can be parsed to remove_sdp_record.
 *
 * record           (in) The SDP record to create
 * record_handle    (out)The corresponding record handle will be written to this pointer.
 */
bt_status_t BTIF_SdpCreateRecord(bluetooth_sdp_record *record, int* record_handle);

/** Remove a SDP record created by BTIF_SdpCreateRecord */
bt_status_t BTIF_SdpRemoveRecord(int record_handle);


#endif /* __BTIF_SDP_H__ */
