/******************************************************************************
 *
 *  Copyright (c) 2014 The Android Open Source Project
 *  Copyright (C) 2003-2012 Broadcom Corporation
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
 *  This file contains the audio gateway functions performing SDP
 *  operations.
 *
 ******************************************************************************/

#include <string.h>
#include "bta/bta_api.h"
#include "bta/bta_sys.h"
#include "common/bt_defs.h"
#include "bta/bta_hf_client_api.h"
#include "bta_hf_client_int.h"
#include "osi/allocator.h"

#if (BTA_HF_INCLUDED == TRUE)
/* Number of protocol elements in protocol element list. */
#define BTA_HF_CLIENT_NUM_PROTO_ELEMS      2

/* Number of elements in service class id list. */
#define BTA_HF_CLIENT_NUM_SVC_ELEMS        2

/*******************************************************************************
**
** Function         bta_hf_client_sdp_cback
**
** Description      SDP callback function.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_hf_client_sdp_cback(UINT16 status)
{
    tBTA_HF_CLIENT_DISC_RESULT *p_buf;
    UINT16                     event;

    APPL_TRACE_DEBUG("bta_hf_client_sdp_cback status:0x%x", status);

    /* set event according to int/acp */
    if (bta_hf_client_cb.scb.role == BTA_HF_CLIENT_ACP) {
        event = BTA_HF_CLIENT_DISC_ACP_RES_EVT;
    } else {
        event = BTA_HF_CLIENT_DISC_INT_RES_EVT;
    }

    if ((p_buf = (tBTA_HF_CLIENT_DISC_RESULT *) osi_malloc(sizeof(tBTA_HF_CLIENT_DISC_RESULT))) != NULL) {
        p_buf->hdr.event = event;
        p_buf->status = status;
        bta_sys_sendmsg(p_buf);
    }
}

/******************************************************************************
**
** Function         bta_hf_client_add_record
**
** Description      This function is called by a server application to add
**                  HFP Client information to an SDP record.  Prior to
**                  calling this function the application must call
**                  SDP_CreateRecord() to create an SDP record.
**
** Returns          TRUE if function execution succeeded,
**                  FALSE if function execution failed.
**
******************************************************************************/
BOOLEAN bta_hf_client_add_record(char *p_service_name, UINT8 scn,
                                 tBTA_HF_CLIENT_FEAT features, UINT32 sdp_handle)
{
    tSDP_PROTOCOL_ELEM  proto_elem_list[BTA_HF_CLIENT_NUM_PROTO_ELEMS];
    UINT16              svc_class_id_list[BTA_HF_CLIENT_NUM_SVC_ELEMS];
    UINT16              browse_list[] = {UUID_SERVCLASS_PUBLIC_BROWSE_GROUP};
    UINT16              version;
    UINT16              profile_uuid;
    BOOLEAN             result = TRUE;
    UINT8               buf[2];
    UINT16              sdp_features = 0;

    APPL_TRACE_DEBUG("bta_hf_client_add_record");

    memset( proto_elem_list, 0 , BTA_HF_CLIENT_NUM_PROTO_ELEMS * sizeof(tSDP_PROTOCOL_ELEM));

    /* add the protocol element sequence */
    proto_elem_list[0].protocol_uuid = UUID_PROTOCOL_L2CAP;
    proto_elem_list[0].num_params = 0;
    proto_elem_list[1].protocol_uuid = UUID_PROTOCOL_RFCOMM;
    proto_elem_list[1].num_params = 1;
    proto_elem_list[1].params[0] = scn;
    result &= SDP_AddProtocolList(sdp_handle, BTA_HF_CLIENT_NUM_PROTO_ELEMS, proto_elem_list);

    /* add service class id list */
    svc_class_id_list[0] = UUID_SERVCLASS_HF_HANDSFREE;
    svc_class_id_list[1] = UUID_SERVCLASS_GENERIC_AUDIO;
    result &= SDP_AddServiceClassIdList(sdp_handle, BTA_HF_CLIENT_NUM_SVC_ELEMS, svc_class_id_list);

    /* add profile descriptor list */
    profile_uuid = UUID_SERVCLASS_HF_HANDSFREE;
    version = HFP_VERSION_1_8;

    result &= SDP_AddProfileDescriptorList(sdp_handle, profile_uuid, version);

    /* add service name */
    if (p_service_name != NULL && p_service_name[0] != 0) {
        result &= SDP_AddAttribute(sdp_handle, ATTR_ID_SERVICE_NAME, TEXT_STR_DESC_TYPE,
                                   (UINT32)(strlen(p_service_name) + 1), (UINT8 *) p_service_name);
    }

    /* add features */
    if (features & BTA_HF_CLIENT_FEAT_ECNR) {
        sdp_features |= BTA_HF_CLIENT_FEAT_ECNR;
    }

    if (features & BTA_HF_CLIENT_FEAT_3WAY) {
        sdp_features |= BTA_HF_CLIENT_FEAT_3WAY;
    }

    if (features & BTA_HF_CLIENT_FEAT_CLI) {
        sdp_features |= BTA_HF_CLIENT_FEAT_CLI;
    }

    if (features & BTA_HF_CLIENT_FEAT_VREC) {
        sdp_features |= BTA_HF_CLIENT_FEAT_VREC;
    }

    if (features & BTA_HF_CLIENT_FEAT_VOL) {
        sdp_features |= BTA_HF_CLIENT_FEAT_VOL;
    }

    /* Codec bit position is different in SDP (bit 5) and in BRSF (bit 7) */
    if (features & BTA_HF_CLIENT_FEAT_CODEC) {
        sdp_features |= 0x0020;
    }

    UINT16_TO_BE_FIELD(buf, sdp_features);
    result &= SDP_AddAttribute(sdp_handle, ATTR_ID_SUPPORTED_FEATURES, UINT_DESC_TYPE, 2, buf);

    /* add browse group list */
    result &= SDP_AddUuidSequence(sdp_handle, ATTR_ID_BROWSE_GROUP_LIST, 1, browse_list);

    return result;
}

/*******************************************************************************
**
** Function         bta_hf_client_create_record
**
** Description      Create SDP record for registered service.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_create_record(tBTA_HF_CLIENT_DATA *p_data)
{
    /* add sdp record if not already registered */
    if (bta_hf_client_cb.sdp_handle == 0) {
        bta_hf_client_cb.sdp_handle = SDP_CreateRecord();
        bta_hf_client_cb.scn = BTM_AllocateSCN();
        bta_hf_client_add_record(p_data->api_register.name,
                                 bta_hf_client_cb.scn,
                                 p_data->api_register.features,
                                 bta_hf_client_cb.sdp_handle);

        bta_sys_add_uuid(UUID_SERVCLASS_HF_HANDSFREE);
    }

}

/*******************************************************************************
**
** Function         bta_hf_client_del_record
**
** Description      Delete SDP record for registered service.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_del_record(tBTA_HF_CLIENT_DATA *p_data)
{
    UNUSED(p_data);

    APPL_TRACE_DEBUG("bta_hf_client_del_record");

    if (bta_hf_client_cb.sdp_handle != 0) {
        SDP_DeleteRecord(bta_hf_client_cb.sdp_handle);
        bta_hf_client_cb.sdp_handle = 0;
        BTM_FreeSCN(bta_hf_client_cb.scn);
        BTM_SecClrService(BTM_SEC_SERVICE_HF_HANDSFREE);
        bta_sys_remove_uuid(UUID_SERVCLASS_HF_HANDSFREE);
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_sdp_find_attr
**
** Description      Process SDP discovery results to find requested attribute
**
**
** Returns          TRUE if results found, FALSE otherwise.
**
*******************************************************************************/
BOOLEAN bta_hf_client_sdp_find_attr(void)
{
    tSDP_DISC_REC       *p_rec = NULL;
    tSDP_DISC_ATTR      *p_attr;
    tSDP_PROTOCOL_ELEM  pe;
    BOOLEAN             result = FALSE;

    bta_hf_client_cb.scb.peer_version = HFP_VERSION_1_1;   /* Default version */

    /* loop through all records we found */
    while (TRUE) {
        /* get next record; if none found, we're done */
        if ((p_rec = SDP_FindServiceInDb(bta_hf_client_cb.scb.p_disc_db, UUID_SERVCLASS_AG_HANDSFREE, p_rec)) == NULL) {
            break;
        }

        /* get scn from proto desc list if initiator */
        if (bta_hf_client_cb.scb.role == BTA_HF_CLIENT_INT) {
            if (SDP_FindProtocolListElemInRec(p_rec, UUID_PROTOCOL_RFCOMM, &pe)) {
                bta_hf_client_cb.scb.peer_scn = (UINT8) pe.params[0];
            } else {
                continue;
            }
        }

        /* get profile version (if failure, version parameter is not updated) */
        SDP_FindProfileVersionInRec(p_rec, UUID_SERVCLASS_HF_HANDSFREE, &bta_hf_client_cb.scb.peer_version);

        /* get features */
        if ((p_attr = SDP_FindAttributeInRec(p_rec, ATTR_ID_SUPPORTED_FEATURES)) != NULL) {
            /* Found attribute. Get value. */
            /* There might be race condition between SDP and BRSF.  */
            /* Do not update if we already received BRSF.           */
            if (bta_hf_client_cb.scb.peer_features == 0) {
                bta_hf_client_cb.scb.peer_features = p_attr->attr_value.v.u16;

                /* SDP and BRSF WBS bit are different, correct it if set */
                if (bta_hf_client_cb.scb.peer_features & 0x0020) {
                    bta_hf_client_cb.scb.peer_features &= ~0x0020;
                    bta_hf_client_cb.scb.peer_features |= BTA_HF_CLIENT_PEER_CODEC;
                }

                /* get network for ability to reject calls */
                if ((p_attr = SDP_FindAttributeInRec(p_rec, ATTR_ID_NETWORK)) != NULL) {
                    if (p_attr->attr_value.v.u16 == 0x01) {
                        bta_hf_client_cb.scb.peer_features |= BTA_HF_CLIENT_PEER_REJECT;
                    }
                }
            }
        }

        /* found what we needed */
        result = TRUE;
        break;
    }

    APPL_TRACE_DEBUG("%s peer_version=0x%x peer_features=0x%x",
                     __FUNCTION__, bta_hf_client_cb.scb.peer_version,
                     bta_hf_client_cb.scb.peer_features);

    return result;
}

/*******************************************************************************
**
** Function         bta_hf_client_do_disc
**
** Description      Do service discovery.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_do_disc(void)
{
    tSDP_UUID       uuid_list[2];
    UINT16          num_uuid = 1;
    UINT16          attr_list[4];
    UINT8           num_attr;
    BOOLEAN         db_inited = FALSE;

    /* initiator; get proto list and features */
    if (bta_hf_client_cb.scb.role == BTA_HF_CLIENT_INT) {
        attr_list[0] = ATTR_ID_SERVICE_CLASS_ID_LIST;
        attr_list[1] = ATTR_ID_PROTOCOL_DESC_LIST;
        attr_list[2] = ATTR_ID_BT_PROFILE_DESC_LIST;
        attr_list[3] = ATTR_ID_SUPPORTED_FEATURES;
        num_attr = 4;
        uuid_list[0].uu.uuid16 = UUID_SERVCLASS_AG_HANDSFREE;
    }
    /* acceptor; get features */
    else {
        attr_list[0] = ATTR_ID_SERVICE_CLASS_ID_LIST;
        attr_list[1] = ATTR_ID_BT_PROFILE_DESC_LIST;
        attr_list[2] = ATTR_ID_SUPPORTED_FEATURES;
        num_attr = 3;
        uuid_list[0].uu.uuid16 = UUID_SERVCLASS_AG_HANDSFREE;
    }

    /* allocate buffer for sdp database */
    bta_hf_client_cb.scb.p_disc_db = (tSDP_DISCOVERY_DB *) osi_malloc(BT_DEFAULT_BUFFER_SIZE);

    if (bta_hf_client_cb.scb.p_disc_db) {
        /* set up service discovery database; attr happens to be attr_list len */
        uuid_list[0].len = LEN_UUID_16;
        uuid_list[1].len = LEN_UUID_16;
        db_inited = SDP_InitDiscoveryDb(bta_hf_client_cb.scb.p_disc_db, BT_DEFAULT_BUFFER_SIZE, num_uuid,
                                        uuid_list, num_attr, attr_list);
    }

    if (db_inited) {
        /*Service discovery not initiated */
        db_inited = SDP_ServiceSearchAttributeRequest(bta_hf_client_cb.scb.peer_addr,
                    bta_hf_client_cb.scb.p_disc_db, bta_hf_client_sdp_cback);
    }

    if (!db_inited) {
        /*free discover db */
        bta_hf_client_free_db(NULL);
        /* sent failed event */
        bta_hf_client_sm_execute(BTA_HF_CLIENT_DISC_FAIL_EVT, NULL);
    }

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
void bta_hf_client_free_db(tBTA_HF_CLIENT_DATA *p_data)
{
    UNUSED(p_data);

    if (bta_hf_client_cb.scb.p_disc_db != NULL) {
        osi_free(bta_hf_client_cb.scb.p_disc_db);
        bta_hf_client_cb.scb.p_disc_db = NULL;
    }
}
#endif /* #if (BTA_HF_INCLUDED == TRUE) */
