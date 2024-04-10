/******************************************************************************
 *
 *  Copyright (C) 2002-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  This file contains the HID HOST API entry points
 *
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common/bt_target.h"
#include "osi/allocator.h"
#include "stack/bt_types.h"
#include "stack/hiddefs.h"
#include "stack/hidh_api.h"
#include "hid_int.h"
#include "stack/btm_api.h"
#include "stack/btu.h"
#include "btm_int.h"

#if (HID_HOST_INCLUDED == TRUE)

#if HID_DYNAMIC_MEMORY == FALSE
tHID_HOST_CTB hh_cb;
#else
tHID_HOST_CTB *hidh_cb_ptr = NULL;
#endif

static void hidh_search_callback (UINT16 sdp_result);

/*******************************************************************************
**
** Function         HID_HostGetSDPRecord
**
** Description      This function reads the device SDP record
**
** Returns          tHID_STATUS
**
*******************************************************************************/
tHID_STATUS HID_HostGetSDPRecord ( BD_ADDR addr, tSDP_DISCOVERY_DB *p_db, UINT32 db_len,
                                   tHID_HOST_SDP_CALLBACK *sdp_cback )
{
    tSDP_UUID   uuid_list;

    if ( hh_cb.sdp_busy ) {
        return HID_ERR_SDP_BUSY;
    }

    uuid_list.len = 2;
    uuid_list.uu.uuid16 = UUID_SERVCLASS_HUMAN_INTERFACE;

    hh_cb.p_sdp_db = p_db;
    SDP_InitDiscoveryDb (p_db, db_len, 1, &uuid_list, 0, NULL);

    if (SDP_ServiceSearchRequest (addr, p_db, hidh_search_callback)) {
        hh_cb.sdp_cback = sdp_cback ;
        hh_cb.sdp_busy = TRUE;
        return HID_SUCCESS;
    } else {
        return HID_ERR_NO_RESOURCES;
    }
}

void hidh_get_str_attr( tSDP_DISC_REC *p_rec, UINT16 attr_id, UINT16 max_len, char *str )
{
    tSDP_DISC_ATTR          *p_attr;
    UINT16                  name_len;

    if ((p_attr = SDP_FindAttributeInRec(p_rec, attr_id)) != NULL) {
        if ((name_len = SDP_DISC_ATTR_LEN(p_attr->attr_len_type)) < max_len ) {
            memcpy( str, (char *) p_attr->attr_value.v.array, name_len );
            str[name_len] = '\0';
        } else {
            memcpy( str, (char *) p_attr->attr_value.v.array, max_len - 1 );
            str[max_len - 1] = '\0';
        }
    } else {
        str[0] = '\0';
    }
}


static void hidh_search_callback (UINT16 sdp_result)
{
    tSDP_DISCOVERY_DB       *p_db = hh_cb.p_sdp_db;
    tSDP_DISC_REC           *p_rec;
    tSDP_DISC_ATTR          *p_attr, *p_subattr1, *p_subattr2, *p_repdesc;
    tBT_UUID                hid_uuid;
    tHID_DEV_SDP_INFO       *p_nvi = &hh_cb.sdp_rec;
    UINT16                  attr_mask = 0;

    hid_uuid.len       = LEN_UUID_16;
    hid_uuid.uu.uuid16 = UUID_SERVCLASS_HUMAN_INTERFACE;

    hh_cb.sdp_busy = FALSE;

    if (sdp_result != SDP_SUCCESS) {
        hh_cb.sdp_cback(sdp_result, 0, NULL);
        return;
    }

    if ((p_rec = SDP_FindServiceUUIDInDb (p_db, &hid_uuid, NULL)) == NULL) {
        hh_cb.sdp_cback(HID_SDP_NO_SERV_UUID, 0, NULL);
        return;
    }

    memset (&hh_cb.sdp_rec, 0, sizeof( tHID_DEV_SDP_INFO ));

    /* First, verify the mandatory fields we care about */
    if (((p_attr = SDP_FindAttributeInRec (p_rec, ATTR_ID_HID_DESCRIPTOR_LIST)) == NULL)
            || (SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) != DATA_ELE_SEQ_DESC_TYPE)
            || ((p_subattr1 = p_attr->attr_value.v.p_sub_attr) == NULL)
            || (SDP_DISC_ATTR_TYPE(p_subattr1->attr_len_type) != DATA_ELE_SEQ_DESC_TYPE)
            || ((p_subattr2 = p_subattr1->attr_value.v.p_sub_attr) == NULL)
            || ((p_repdesc = p_subattr2->p_next_attr) == NULL)
            || (SDP_DISC_ATTR_TYPE(p_repdesc->attr_len_type) != TEXT_STR_DESC_TYPE)) {
        hh_cb.sdp_cback(HID_SDP_MANDATORY_MISSING, 0, NULL);
        return;
    }

    if ((p_nvi->dscp_info.dl_len = SDP_DISC_ATTR_LEN(p_repdesc->attr_len_type)) != 0) {
        p_nvi->dscp_info.dsc_list = (UINT8 *) &p_repdesc->attr_value;
    }

    if (((p_attr = SDP_FindAttributeInRec (p_rec, ATTR_ID_HID_VIRTUAL_CABLE)) != NULL) &&
            (p_attr->attr_value.v.u8) ) {
        attr_mask |= HID_VIRTUAL_CABLE;
    }

    if (((p_attr = SDP_FindAttributeInRec (p_rec, ATTR_ID_HID_RECONNECT_INITIATE)) != NULL) &&
            (p_attr->attr_value.v.u8) ) {
        attr_mask |= HID_RECONN_INIT;
    }

    if (((p_attr = SDP_FindAttributeInRec (p_rec, ATTR_ID_HID_NORMALLY_CONNECTABLE)) != NULL) &&
            (p_attr->attr_value.v.u8) ) {
        attr_mask |= HID_NORMALLY_CONNECTABLE;
    }

    if (((p_attr = SDP_FindAttributeInRec (p_rec, ATTR_ID_HID_SDP_DISABLE)) != NULL) &&
            (p_attr->attr_value.v.u8) ) {
        attr_mask |= HID_SDP_DISABLE;
    }

    if (((p_attr = SDP_FindAttributeInRec (p_rec, ATTR_ID_HID_BATTERY_POWER)) != NULL) &&
            (p_attr->attr_value.v.u8) ) {
        attr_mask |= HID_BATTERY_POWER;
    }

    if (((p_attr = SDP_FindAttributeInRec (p_rec, ATTR_ID_HID_REMOTE_WAKE)) != NULL) &&
            (p_attr->attr_value.v.u8) ) {
        attr_mask |= HID_REMOTE_WAKE;
    }

    hidh_get_str_attr( p_rec, ATTR_ID_SERVICE_NAME, HID_MAX_SVC_NAME_LEN, p_nvi->svc_name );
    hidh_get_str_attr( p_rec, ATTR_ID_SERVICE_DESCRIPTION, HID_MAX_SVC_DESCR_LEN, p_nvi->svc_descr );
    hidh_get_str_attr( p_rec, ATTR_ID_PROVIDER_NAME, HID_MAX_PROV_NAME_LEN, p_nvi->prov_name );

    if (((p_attr = SDP_FindAttributeInRec (p_rec, ATTR_ID_HID_DEVICE_RELNUM)) != NULL)) {
        p_nvi->rel_num = p_attr->attr_value.v.u16;
    }

    if (((p_attr = SDP_FindAttributeInRec (p_rec, ATTR_ID_HID_COUNTRY_CODE)) != NULL)) {
        p_nvi->ctry_code = p_attr->attr_value.v.u8;
    }

    if (((p_attr = SDP_FindAttributeInRec (p_rec, ATTR_ID_HID_DEVICE_SUBCLASS)) != NULL)) {
        p_nvi->sub_class = p_attr->attr_value.v.u8;
    }

    if (((p_attr = SDP_FindAttributeInRec (p_rec, ATTR_ID_HID_PARSER_VERSION)) != NULL)) {
        p_nvi->hpars_ver = p_attr->attr_value.v.u16;
    }

    if (((p_attr = SDP_FindAttributeInRec (p_rec, ATTR_ID_HID_LINK_SUPERVISION_TO)) != NULL)) {
        attr_mask |= HID_SUP_TOUT_AVLBL;
        p_nvi->sup_timeout = p_attr->attr_value.v.u16;
    }

    if (((p_attr = SDP_FindAttributeInRec (p_rec, ATTR_ID_HID_SSR_HOST_MAX_LAT)) != NULL)) {
        attr_mask |= HID_SSR_MAX_LATENCY;
        p_nvi->ssr_max_latency = p_attr->attr_value.v.u16;
    } else {
        p_nvi->ssr_max_latency = HID_SSR_PARAM_INVALID;
    }

    if (((p_attr = SDP_FindAttributeInRec (p_rec, ATTR_ID_HID_SSR_HOST_MIN_TOUT)) != NULL)) {
        attr_mask |= HID_SSR_MIN_TOUT;
        p_nvi->ssr_min_tout = p_attr->attr_value.v.u16;
    } else {
        p_nvi->ssr_min_tout = HID_SSR_PARAM_INVALID;
    }

    hh_cb.sdp_rec.p_sdp_layer_rec = p_rec;
    hh_cb.sdp_cback(SDP_SUCCESS, attr_mask, &hh_cb.sdp_rec);
}


/*******************************************************************************
**
** Function         HID_HostInit
**
** Description      This function initializes the control block and trace variable
**
** Returns          tHID_STATUS
**
*******************************************************************************/
tHID_STATUS HID_HostInit (void)
{
#if (HID_DYNAMIC_MEMORY)
    if (!hidh_cb_ptr) {
        hidh_cb_ptr = (tHID_HOST_CTB *)osi_malloc(sizeof(tHID_HOST_CTB));
        if (!hidh_cb_ptr) {
            return HID_ERR_NO_RESOURCES;
        }
    }
#endif /* #if (HID_DYNAMIC_MEMORY) */
    memset(&hh_cb, 0, sizeof(tHID_HOST_CTB));

#if defined(HIDH_INITIAL_TRACE_LEVEL)
    hh_cb.trace_level = HIDH_INITIAL_TRACE_LEVEL;
#else
    hh_cb.trace_level = BT_TRACE_LEVEL_NONE;
#endif
    return HID_SUCCESS;
}

/*******************************************************************************
**
** Function         HID_HostInit
**
** Description      This function deinitializes the control block
**
** Returns          void
**
*******************************************************************************/
void HID_HostDeinit (void)
{
#if (HID_DYNAMIC_MEMORY)
    if (hidh_cb_ptr) {
        osi_free(hidh_cb_ptr);
        hidh_cb_ptr = NULL;
    }
#endif /* #if (HID_DYNAMIC_MEMORY) */
}

/*******************************************************************************
**
** Function         HID_HostSetTraceLevel
**
** Description      This function sets the trace level for HID Host. If called with
**                  a value of 0xFF, it simply reads the current trace level.
**
** Returns          the new (current) trace level
**
*******************************************************************************/
UINT8 HID_HostSetTraceLevel (UINT8 new_level)
{
    if (new_level != 0xFF) {
        hh_cb.trace_level = new_level;
    }

    return (hh_cb.trace_level);
}

/*******************************************************************************
**
** Function         HID_HostRegister
**
** Description      This function registers HID-Host with lower layers
**
** Returns          tHID_STATUS
**
*******************************************************************************/
tHID_STATUS HID_HostRegister (tHID_HOST_DEV_CALLBACK *dev_cback)
{
    tHID_STATUS st;

    if ( hh_cb.reg_flag ) {
        return HID_ERR_ALREADY_REGISTERED;
    }

    if ( dev_cback == NULL ) {
        return HID_ERR_INVALID_PARAM;
    }

    /* Register with L2CAP */
    if ( (st = hidh_conn_reg()) != HID_SUCCESS ) {
        return st;
    }

    hh_cb.callback = dev_cback ;
    hh_cb.reg_flag = TRUE;

    return (HID_SUCCESS);
}

/*******************************************************************************
**
** Function         HID_HostDeregister
**
** Description      This function is called when the host is about power down.
**
** Returns          tHID_STATUS
**
*******************************************************************************/
tHID_STATUS HID_HostDeregister(void)
{
    UINT8 i;

    if ( !hh_cb.reg_flag ) {
        return (HID_ERR_NOT_REGISTERED);
    }

    for ( i = 0; i < HID_HOST_MAX_DEVICES; i++ ) {
        HID_HostRemoveDev( i ) ;
    }

    hidh_conn_dereg();
    hh_cb.reg_flag = FALSE;

    return (HID_SUCCESS) ;
}

/*******************************************************************************
**
** Function         HID_HostAddDev
**
** Description      This is called so HID-host may manage this device.
**
** Returns          tHID_STATUS
**
*******************************************************************************/
tHID_STATUS HID_HostAddDev ( BD_ADDR addr, UINT16 attr_mask, UINT8 *handle )
{
    int i;
    /* Find an entry for this device in hh_cb.devices array */
    if ( !hh_cb.reg_flag ) {
        return (HID_ERR_NOT_REGISTERED);
    }

    for ( i = 0; i < HID_HOST_MAX_DEVICES; i++) {
        if ((hh_cb.devices[i].in_use) &&
                (!memcmp(addr, hh_cb.devices[i].addr, BD_ADDR_LEN))) {
            break;
        }
    }

    if (i == HID_HOST_MAX_DEVICES ) {
        for ( i = 0; i < HID_HOST_MAX_DEVICES; i++) {
            if ( !hh_cb.devices[i].in_use) {
                break;
            }
        }
    }

    if ( i == HID_HOST_MAX_DEVICES ) {
        return HID_ERR_NO_RESOURCES;
    }

    if (!hh_cb.devices[i].in_use) {
        hh_cb.devices[i].in_use = TRUE;
        hh_cb.devices[i].delay_remove = FALSE;
        hh_cb.devices[i].is_orig = FALSE;
        memcpy( hh_cb.devices[i].addr, addr, sizeof( BD_ADDR ) ) ;
        hh_cb.devices[i].state = HID_DEV_NO_CONN;
        hh_cb.devices[i].conn_tries = 0 ;
    }

    if (attr_mask != HID_ATTR_MASK_IGNORE) {
        hh_cb.devices[i].attr_mask = attr_mask;
    }

    *handle = i;

    return (HID_SUCCESS);
}

/*******************************************************************************
**
** Function         HID_HostGetDev
**
** Description      This is called so HID-host can find this device.
**
** Returns          tHID_STATUS
**
*******************************************************************************/
tHID_STATUS HID_HostGetDev(BD_ADDR addr, UINT8 *handle)
{
    int i;
    /* Find an entry for this device in hh_cb.devices array */
    if (!hh_cb.reg_flag) {
        return (HID_ERR_NOT_REGISTERED);
    }

    for (i = 0; i < HID_HOST_MAX_DEVICES; i++) {
        if ((hh_cb.devices[i].in_use) && (!memcmp(addr, hh_cb.devices[i].addr, BD_ADDR_LEN))) {
            break;
        }
    }

    if (i == HID_HOST_MAX_DEVICES) {
        *handle = 0xff;
    } else {
        *handle = i;
    }
    return (HID_SUCCESS);
}

/*******************************************************************************
**
** Function         HID_HostRemoveDev
**
** Description      This removes the device from list devices that host has to manage.
**
** Returns          tHID_STATUS
**
*******************************************************************************/
tHID_STATUS HID_HostRemoveDev ( UINT8 dev_handle )
{
    if ( !hh_cb.reg_flag ) {
        return (HID_ERR_NOT_REGISTERED);
    }

    if ( (dev_handle >= HID_HOST_MAX_DEVICES) || (!hh_cb.devices[dev_handle].in_use) ) {
        return HID_ERR_INVALID_PARAM;
    }

    HID_HostCloseDev( dev_handle ) ;

    if (hh_cb.devices[dev_handle].conn.conn_state == HID_CONN_STATE_DISCONNECTING_INTR ||
        hh_cb.devices[dev_handle].conn.conn_state == HID_CONN_STATE_DISCONNECTING_CTRL) {
        // delay the remove action, to close the control and the interrupt channel
        hh_cb.devices[dev_handle].delay_remove = TRUE;
    } else {
        HIDH_TRACE_WARNING("%s dev_handle:%d conn_state:%d", __func__, dev_handle,
                           hh_cb.devices[dev_handle].conn.conn_state);
        hh_cb.devices[dev_handle].in_use = FALSE;
        hh_cb.devices[dev_handle].conn.conn_state = HID_CONN_STATE_UNUSED;
        hh_cb.devices[dev_handle].conn.ctrl_cid = hh_cb.devices[dev_handle].conn.intr_cid = 0;
        hh_cb.devices[dev_handle].attr_mask = 0;
    }

    return HID_SUCCESS;
}

/*******************************************************************************
**
** Function         HID_HostOpenDev
**
** Description      This function is called when the user wants to initiate a
**                  connection attempt to a device.
**
** Returns          void
**
*******************************************************************************/
tHID_STATUS HID_HostOpenDev ( UINT8 dev_handle )
{
    if ( !hh_cb.reg_flag ) {
        return (HID_ERR_NOT_REGISTERED);
    }

    if ( (dev_handle >= HID_HOST_MAX_DEVICES) || (!hh_cb.devices[dev_handle].in_use) ) {
        return HID_ERR_INVALID_PARAM;
    }

    if ( hh_cb.devices[dev_handle].state != HID_DEV_NO_CONN ) {
        return HID_ERR_ALREADY_CONN;
    }

    hh_cb.devices[dev_handle].conn_tries = 1;
    hh_cb.devices[dev_handle].is_orig = TRUE;
    return hidh_conn_initiate( dev_handle );
}

/*******************************************************************************
**
** Function         HID_HostWriteDev
**
** Description      This function is called when the host has a report to send.
**
**                  report_id: is only used on GET_REPORT transaction if is specified.
**                              only valid when it's a non-zero value.
**
** Returns          void
**
*******************************************************************************/
tHID_STATUS HID_HostWriteDev( UINT8 dev_handle, UINT8 t_type,
                              UINT8 param, UINT16 data, UINT8 report_id, BT_HDR *pbuf  )
{
    tHID_STATUS status = HID_SUCCESS;

    if ( !hh_cb.reg_flag ) {
        HIDH_TRACE_ERROR("HID_ERR_NOT_REGISTERED");
        status = HID_ERR_NOT_REGISTERED;
    }

    if ( (dev_handle >= HID_HOST_MAX_DEVICES) || (!hh_cb.devices[dev_handle].in_use) ) {
        HIDH_TRACE_ERROR("HID_ERR_INVALID_PARAM");
        status = HID_ERR_INVALID_PARAM;
    }

    else if ( hh_cb.devices[dev_handle].state != HID_DEV_CONNECTED ) {
        HIDH_TRACE_ERROR("HID_ERR_NO_CONNECTION dev_handle %d", dev_handle);
        status = HID_ERR_NO_CONNECTION;
    }

    if (status != HID_SUCCESS) {
        if (pbuf) {
            osi_free ((void *)pbuf);
        }
    } else {
        status = hidh_conn_snd_data( dev_handle, t_type, param, data, report_id, pbuf ) ;
    }

    return status;
}

/*******************************************************************************
**
** Function         HID_HostCloseDev
**
** Description      This function disconnects the device.
**
** Returns          void
**
*******************************************************************************/
tHID_STATUS HID_HostCloseDev( UINT8 dev_handle )
{
    if ( !hh_cb.reg_flag ) {
        return (HID_ERR_NOT_REGISTERED);
    }

    if ( (dev_handle >= HID_HOST_MAX_DEVICES) || (!hh_cb.devices[dev_handle].in_use) ) {
        return HID_ERR_INVALID_PARAM;
    }

    hh_cb.devices[dev_handle].conn_tries = HID_HOST_MAX_CONN_RETRY + 1;
    btu_stop_timer( &(hh_cb.devices[dev_handle].conn.timer_entry) ) ;

    if ( hh_cb.devices[dev_handle].state != HID_DEV_CONNECTED ) {
        return HID_ERR_NO_CONNECTION;
    }

    hh_cb.devices[dev_handle].conn_tries = HID_HOST_MAX_CONN_RETRY + 1;
    return hidh_conn_disconnect( dev_handle );
}

tHID_STATUS HID_HostSetSecurityLevel( char serv_name[], UINT8 sec_lvl )
{
    if (!BTM_SetSecurityLevel (FALSE, serv_name, BTM_SEC_SERVICE_HIDH_SEC_CTRL,
                               sec_lvl, HID_PSM_CONTROL, BTM_SEC_PROTO_HID, HID_SEC_CHN)) {
        HIDH_TRACE_ERROR ("Security Registration 1 failed");
        return (HID_ERR_NO_RESOURCES);
    }

    if (!BTM_SetSecurityLevel (TRUE, serv_name, BTM_SEC_SERVICE_HIDH_SEC_CTRL,
                               sec_lvl, HID_PSM_CONTROL, BTM_SEC_PROTO_HID, HID_SEC_CHN)) {
        HIDH_TRACE_ERROR ("Security Registration 2 failed");
        return (HID_ERR_NO_RESOURCES);
    }

    if (!BTM_SetSecurityLevel (FALSE, serv_name, BTM_SEC_SERVICE_HIDH_NOSEC_CTRL,
                               BTM_SEC_NONE, HID_PSM_CONTROL, BTM_SEC_PROTO_HID, HID_NOSEC_CHN)) {
        HIDH_TRACE_ERROR ("Security Registration 3 failed");
        return (HID_ERR_NO_RESOURCES);
    }

    if (!BTM_SetSecurityLevel (TRUE, serv_name, BTM_SEC_SERVICE_HIDH_NOSEC_CTRL,
                               BTM_SEC_NONE, HID_PSM_CONTROL, BTM_SEC_PROTO_HID, HID_NOSEC_CHN)) {
        HIDH_TRACE_ERROR ("Security Registration 4 failed");
        return (HID_ERR_NO_RESOURCES);
    }

    if (!BTM_SetSecurityLevel (TRUE, serv_name, BTM_SEC_SERVICE_HIDH_INTR,
                               BTM_SEC_NONE, HID_PSM_INTERRUPT, BTM_SEC_PROTO_HID, 0)) {
        HIDH_TRACE_ERROR ("Security Registration 5 failed");
        return (HID_ERR_NO_RESOURCES);
    }

    if (!BTM_SetSecurityLevel (FALSE, serv_name, BTM_SEC_SERVICE_HIDH_INTR,
                               BTM_SEC_NONE, HID_PSM_INTERRUPT, BTM_SEC_PROTO_HID, 0)) {
        HIDH_TRACE_ERROR ("Security Registration 6 failed");
        return (HID_ERR_NO_RESOURCES);
    }

    return ( HID_SUCCESS );
}

/******************************************************************************
**
** Function         hid_known_hid_device
**
** Description      check if this device is  of type HID Device
**
** Returns          TRUE if device is HID Device else FALSE
**
*******************************************************************************/
BOOLEAN hid_known_hid_device (BD_ADDR bd_addr)
{
    UINT8 i;
    tBTM_INQ_INFO *p_inq_info = BTM_InqDbRead(bd_addr);

    if ( !hh_cb.reg_flag ) {
        return FALSE;
    }

    /* First  check for class of device , if Inq DB has information about this device*/
    if (p_inq_info != NULL) {
        /* Check if remote major device class is of type BTM_COD_MAJOR_PERIPHERAL */
        if ((p_inq_info->results.dev_class[1] & BTM_COD_MAJOR_CLASS_MASK)
                == BTM_COD_MAJOR_PERIPHERAL ) {
            HIDH_TRACE_DEBUG("hid_known_hid_device:dev found in InqDB & COD matches HID dev");
            return TRUE;
        }
    } else {
        /* Look for this device in security device DB */
        tBTM_SEC_DEV_REC  *p_dev_rec = btm_find_dev (bd_addr);
        if ((p_dev_rec != NULL) &&
                ((p_dev_rec->dev_class[1] & BTM_COD_MAJOR_CLASS_MASK) == BTM_COD_MAJOR_PERIPHERAL )) {
            HIDH_TRACE_DEBUG("hid_known_hid_device:dev found in SecDevDB & COD matches HID dev");
            return TRUE;
        }
    }

    /* Find an entry for this device in hh_cb.devices array */
    for ( i = 0; i < HID_HOST_MAX_DEVICES; i++) {
        if ((hh_cb.devices[i].in_use) &&
                (memcmp(bd_addr, hh_cb.devices[i].addr, BD_ADDR_LEN) == 0)) {
            return TRUE;
        }
    }
    /* Check if this device is marked as HID Device in IOP Dev */
    HIDH_TRACE_DEBUG("hid_known_hid_device:remote is not HID device");
    return FALSE;
}

BOOLEAN HID_HostConnectOrig(UINT8 dev_handle)
{
    BOOLEAN ret = FALSE;

    do {
        if (!hh_cb.reg_flag) {
            break;
        }

        if ((dev_handle >= HID_HOST_MAX_DEVICES) || (!hh_cb.devices[dev_handle].in_use)) {
            break;
        }

        ret = hh_cb.devices[dev_handle].is_orig;
    } while (0);

    return ret;
}

#endif //HID_HOST_INCLUDED
