/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "osi/allocator.h"
#include "common/bt_defs.h"
#include "stack/sdp_api.h"
#include "bta/bta_api.h"
#include "bta/bta_pba_defs.h"
#include "bta/bta_pba_client_api.h"
#include "bta_pba_client_int.h"

#if BTA_PBA_CLIENT_INCLUDED

/* Number of elements in service class id list. */
#define BTA_PBA_CLIENT_NUM_SVC_ELEMS        1

/*******************************************************************************
**
** Function         bta_pba_client_sdp_cback
**
** Description      SDP callback function.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_pba_client_sdp_cback(UINT16 status, void *user_data)
{
    tBTA_PBA_CLIENT_DISC_RESULT *p_buf;
    tBTA_PBA_CLIENT_CCB *p_ccb = (tBTA_PBA_CLIENT_CCB *)user_data;

    APPL_TRACE_DEBUG("bta_pba_client_sdp_cback status:0x%x", status);

    if ((p_buf = (tBTA_PBA_CLIENT_DISC_RESULT *) osi_malloc(sizeof(tBTA_PBA_CLIENT_DISC_RESULT))) != NULL) {
        p_buf->hdr.event = BTA_PBA_CLIENT_DISC_RES_EVT;
        p_buf->hdr.layer_specific = p_ccb->allocated;
        p_buf->status = status;
        bta_sys_sendmsg(p_buf);
    }
}

/******************************************************************************
**
** Function         bta_pba_client_add_record
**
** Description      Add PBA Client information to an SDP record. Prior to
**                  calling this function the application must call
**                  SDP_CreateRecord() to create an SDP record.
**
** Returns          TRUE if function execution succeeded,
**                  FALSE if function execution failed.
**
******************************************************************************/
static BOOLEAN bta_pba_client_add_record(const char *p_service_name, UINT32 sdp_handle)
{
    UINT16              svc_class_id_list[BTA_PBA_CLIENT_NUM_SVC_ELEMS];
    UINT16              version;
    UINT16              profile_uuid;
    BOOLEAN             result = TRUE;

    APPL_TRACE_DEBUG("bta_pba_client_add_record");

    /* add service class id list */
    svc_class_id_list[0] = UUID_SERVCLASS_PBAP_PCE;
    result &= SDP_AddServiceClassIdList(sdp_handle, BTA_PBA_CLIENT_NUM_SVC_ELEMS, svc_class_id_list);

    /* add service name */
    if (p_service_name != NULL && p_service_name[0] != 0) {
        result &= SDP_AddAttribute(sdp_handle, ATTR_ID_SERVICE_NAME, TEXT_STR_DESC_TYPE,
                                   (UINT32)(strlen(p_service_name) + 1), (UINT8 *) p_service_name);
    }

    /* add profile descriptor list */
    profile_uuid = UUID_SERVCLASS_PHONE_ACCESS;
    version = PBAP_PCE_VERSION;
    result &= SDP_AddProfileDescriptorList(sdp_handle, profile_uuid, version);

    return result;
}

/*******************************************************************************
**
** Function         bta_pba_client_create_record
**
** Description      Create SDP record for registered service.
**
**
** Returns          void
**
*******************************************************************************/
void bta_pba_client_create_record(const char *p_service_name)
{
    /* add sdp record if not already registered */
    if (bta_pba_client_cb.sdp_handle == 0) {
        bta_pba_client_cb.sdp_handle = SDP_CreateRecord();
        bta_pba_client_add_record(p_service_name, bta_pba_client_cb.sdp_handle);
        bta_sys_add_uuid(UUID_SERVCLASS_PBAP_PCE);
    }

}

/*******************************************************************************
**
** Function         bta_pba_client_del_record
**
** Description      Delete SDP record for registered service.
**
**
** Returns          void
**
*******************************************************************************/
void bta_pba_client_del_record(void)
{
    APPL_TRACE_DEBUG("bta_pba_client_del_record");

    if (bta_pba_client_cb.sdp_handle != 0) {
        SDP_DeleteRecord(bta_pba_client_cb.sdp_handle);
        bta_pba_client_cb.sdp_handle = 0;
        bta_sys_remove_uuid(UUID_SERVCLASS_PBAP_PCE);
    }
}

/*******************************************************************************
**
** Function         bta_pba_client_sdp_find_attr
**
** Description      Process SDP discovery results to find requested attribute
**
**
** Returns          TRUE if results found, FALSE otherwise.
**
*******************************************************************************/
BOOLEAN bta_pba_client_sdp_find_attr(tBTA_PBA_CLIENT_CCB *p_ccb)
{
    tSDP_DISC_REC       *p_rec = NULL;
    tSDP_DISC_ATTR      *p_attr;
    tSDP_PROTOCOL_ELEM  pe;
    BOOLEAN             result = FALSE;

    /* loop through all records we found */
    while (TRUE) {
        /* get next record; if none found, we're done */
        if ((p_rec = SDP_FindServiceInDb(p_ccb->p_disc_db, UUID_SERVCLASS_PBAP_PSE, p_rec)) == NULL) {
            break;
        }

        /* get rfcomm scn from proto desc list */
        if (SDP_FindProtocolListElemInRec(p_rec, UUID_PROTOCOL_RFCOMM, &pe)) {
            p_ccb->peer_rfcomm_scn = (UINT8) pe.params[0];
        }
        else {
            /* not found, go to next record */
            continue;
        }

        /* get supported repositories */
        if ((p_attr = SDP_FindAttributeInRec(p_rec, ATTR_ID_SUPPORTED_REPOSITORIES)) != NULL) {
            /* Found attribute, get value */
            p_ccb->peer_supported_repo = p_attr->attr_value.v.u8;
        }
        else {
            /* not found, clear rfcomm scn and go to next record */
            p_ccb->peer_rfcomm_scn = 0;
            continue;
        }

        /* get profile version */
        SDP_FindProfileVersionInRec(p_rec, UUID_SERVCLASS_PHONE_ACCESS, &p_ccb->peer_version);

        /* get GOEP L2CAP PSM */
        if ((p_attr = SDP_FindAttributeInRec(p_rec, ATTR_ID_GOEP_L2CAP_PSM)) != NULL) {
            /* Found attribute, get value */
            p_ccb->peer_l2cap_psm = p_attr->attr_value.v.u16;
        }

        /* try to get supported features */
        if ((p_attr = SDP_FindAttributeInRec(p_rec, ATTR_ID_PBAP_SUPPORTED_FEATURES)) != NULL) {
            /* found attribute, get value */
            p_ccb->peer_supported_feat = p_attr->attr_value.v.u32;
            p_ccb->send_supported_feat = TRUE;
        }
        else {
            /* assume as default value if not found in sdp record */
            p_ccb->peer_supported_feat = BTA_PBAP_DEFAULT_SUPPORTED_FEATURES;
            p_ccb->send_supported_feat = FALSE;
        }

        /* found what we needed */
        result = TRUE;
        break;
    }

    APPL_TRACE_DEBUG("%s peer_version:0x%x, supported repositories:0x%x, supported features:0x%x",
                     __FUNCTION__, p_ccb->peer_version, p_ccb->peer_supported_repo, p_ccb->peer_supported_feat);

    return result;
}

/*******************************************************************************
**
** Function         bta_pba_client_do_disc
**
** Description      Do service discovery.
**
**
** Returns          TRUE if start service discovery successfully
**
*******************************************************************************/
BOOLEAN bta_pba_client_do_disc(tBTA_PBA_CLIENT_CCB *p_ccb)
{
    tSDP_UUID       uuid_list[1];
    UINT16          num_uuid = 1;
    UINT16          attr_list[6];
    UINT8           num_attr = 6;
    BOOLEAN         db_inited = FALSE;

    /* get proto list and features */
    attr_list[0] = ATTR_ID_SERVICE_CLASS_ID_LIST;
    attr_list[1] = ATTR_ID_PROTOCOL_DESC_LIST;
    attr_list[2] = ATTR_ID_BT_PROFILE_DESC_LIST;
    attr_list[3] = ATTR_ID_GOEP_L2CAP_PSM;
    attr_list[4] = ATTR_ID_SUPPORTED_REPOSITORIES;
    attr_list[5] = ATTR_ID_PBAP_SUPPORTED_FEATURES;
    uuid_list[0].uu.uuid16 = UUID_SERVCLASS_PBAP_PSE;
    uuid_list[0].len = LEN_UUID_16;

    if (p_ccb->p_disc_db != NULL) {
        APPL_TRACE_WARNING("%s service discovery already in progress", __FUNCTION__);
        return FALSE;
    }

    /* allocate buffer for sdp database */
    p_ccb->p_disc_db = (tSDP_DISCOVERY_DB *) osi_malloc(BT_DEFAULT_BUFFER_SIZE);

    if (p_ccb->p_disc_db) {
        /* set up service discovery database; attr happens to be attr_list len */
        db_inited = SDP_InitDiscoveryDb(p_ccb->p_disc_db, BT_DEFAULT_BUFFER_SIZE, num_uuid,
                                        uuid_list, num_attr, attr_list);
    }

    if (db_inited) {
        /*start service discovery */
        /* todo: avoid p_ccb being free during sdp */
        db_inited = SDP_ServiceSearchAttributeRequest2(p_ccb->bd_addr, p_ccb->p_disc_db,
                                                      bta_pba_client_sdp_cback, p_ccb);
    }

    if (!db_inited) {
        /*free discover db */
        bta_pba_client_free_db(p_ccb);
        APPL_TRACE_ERROR("%s start service discovery failed", __FUNCTION__);
        return FALSE;
    }
    return TRUE;
}

/*******************************************************************************
**
** Function         bta_hf_client_free_db
**
** Description      Free discovery database.
**
**
** Returns          void
**
*******************************************************************************/
void bta_pba_client_free_db(tBTA_PBA_CLIENT_CCB *p_ccb)
{
    if (p_ccb->p_disc_db != NULL) {
        osi_free(p_ccb->p_disc_db);
        p_ccb->p_disc_db = NULL;
    }
}

#endif
