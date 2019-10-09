/******************************************************************************
 *
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
#include "bta_ag_int.h"
#include "bta/bta_ag_api.h"
#include "bta/bta_sys.h"
#include "bta/bta_ag_api.h"
#include "bta/utl.h"
#include "stack/sdp_api.h"
#include "stack/btm_api.h"
#include "common/bt_trace.h"
#include "osi/allocator.h"

#if (BTA_AG_INCLUDED == TRUE)

/* Number of protocol elements in protocol element list. */
#define BTA_AG_NUM_PROTO_ELEMS      2

/* Number of elements in service class id list. */
#define BTA_AG_NUM_SVC_ELEMS        2

/* size of database for service discovery */
#ifndef BTA_AG_DISC_BUF_SIZE
#define BTA_AG_DISC_BUF_SIZE        (4096+16)
#endif

/* declare sdp callback functions */
void bta_ag_sdp_cback_1(UINT16 status);
void bta_ag_sdp_cback_2(UINT16 status);
void bta_ag_sdp_cback_3(UINT16 status);

/* SDP callback function table */
typedef tSDP_DISC_CMPL_CB *tBTA_AG_SDP_CBACK;
const tBTA_AG_SDP_CBACK bta_ag_sdp_cback_tbl[] =
{
    bta_ag_sdp_cback_1,
    bta_ag_sdp_cback_2,
    bta_ag_sdp_cback_3
};

/*******************************************************************************
**
** Function         bta_ag_sdp_cback
**
** Description      SDP callback function.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_ag_sdp_cback(UINT16 status, UINT8 idx)
{
    tBTA_AG_DISC_RESULT *p_buf;
    UINT16              event;
    tBTA_AG_SCB         *p_scb;

    APPL_TRACE_DEBUG("bta_ag_sdp_cback status:0x%x", status);

    if ((p_scb = bta_ag_scb_by_idx(idx)) != NULL) {
        /* set event according to int/acp */
        if (p_scb->role == BTA_AG_ACP) {
            event = BTA_AG_DISC_ACP_RES_EVT;
        } else {
            event = BTA_AG_DISC_INT_RES_EVT;
        }

        if ((p_buf = (tBTA_AG_DISC_RESULT *) osi_malloc(sizeof(tBTA_AG_DISC_RESULT))) != NULL) {
            p_buf->hdr.event = event;
            p_buf->hdr.layer_specific = idx;
            p_buf->status = status;
            bta_sys_sendmsg(p_buf);
        }
    }
}

/*******************************************************************************
**
** Function         bta_ag_sdp_cback_1 to 3
**
** Description      SDP callback functions.  Since there is no way to
**                  distinguish scb from the callback we need separate
**                  callbacks for each scb.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_sdp_cback_1(UINT16 status) {bta_ag_sdp_cback(status, 1);}
void bta_ag_sdp_cback_2(UINT16 status) {bta_ag_sdp_cback(status, 2);}
void bta_ag_sdp_cback_3(UINT16 status) {bta_ag_sdp_cback(status, 3);}

/******************************************************************************
**
** Function         bta_ag_add_record
**
** Description      This function is called by a server application to add
**                  HSP or HFP information to an SDP record.  Prior to
**                  calling this function the application must call
**                  SDP_CreateRecord() to create an SDP record.
**
** Returns          TRUE if function execution succeeded,
**                  FALSE if function execution failed.
**
******************************************************************************/
BOOLEAN bta_ag_add_record(UINT16 service_uuid, char *p_service_name, UINT8 scn,
                          tBTA_AG_FEAT features, UINT32 sdp_handle)
{
    tSDP_PROTOCOL_ELEM  proto_elem_list[BTA_AG_NUM_PROTO_ELEMS];
    UINT16              svc_class_id_list[BTA_AG_NUM_SVC_ELEMS];
    UINT16              browse_list[] = {UUID_SERVCLASS_PUBLIC_BROWSE_GROUP};
    UINT16              version;
    UINT16              profile_uuid;
    UINT8               network;
    BOOLEAN             result = TRUE;
    BOOLEAN             codec_supported = FALSE;
    UINT8               buf[2];

    APPL_TRACE_DEBUG("bta_ag_add_record uuid: %x", service_uuid);

    memset( proto_elem_list, 0 , BTA_AG_NUM_PROTO_ELEMS*sizeof(tSDP_PROTOCOL_ELEM));
    /* add the protocol element sequence */
    proto_elem_list[0].protocol_uuid = UUID_PROTOCOL_L2CAP;
    proto_elem_list[0].num_params = 0;
    proto_elem_list[1].protocol_uuid = UUID_PROTOCOL_RFCOMM;
    proto_elem_list[1].num_params = 1;
    proto_elem_list[1].params[0] = scn;
    result &= SDP_AddProtocolList(sdp_handle, BTA_AG_NUM_PROTO_ELEMS, proto_elem_list);

    /* add service class id list */
    svc_class_id_list[0] = service_uuid;
    svc_class_id_list[1] = UUID_SERVCLASS_GENERIC_AUDIO;
    result &= SDP_AddServiceClassIdList(sdp_handle, BTA_AG_NUM_SVC_ELEMS, svc_class_id_list);

    /* add profile descriptor list */
    if (service_uuid == UUID_SERVCLASS_AG_HANDSFREE) {
        profile_uuid = UUID_SERVCLASS_HF_HANDSFREE;
        version = HFP_VERSION_1_6;
    } else {
        profile_uuid = UUID_SERVCLASS_HEADSET;
        version = HSP_VERSION_1_2;
    }
    result &= SDP_AddProfileDescriptorList(sdp_handle, profile_uuid, version);

    /* add service name */
    if (p_service_name != NULL && p_service_name[0] != 0) {
        result &= SDP_AddAttribute(sdp_handle, ATTR_ID_SERVICE_NAME, TEXT_STR_DESC_TYPE,
                    (UINT32)(strlen(p_service_name)+1), (UINT8 *) p_service_name);
    }

    /* add features and network */
    if (service_uuid == UUID_SERVCLASS_AG_HANDSFREE) {
        network = (features & BTA_AG_FEAT_REJECT) ? 1 : 0;
        result &= SDP_AddAttribute(sdp_handle, ATTR_ID_DATA_STORES_OR_NETWORK,
                    UINT_DESC_TYPE, 1, &network);

        if (features & BTA_AG_FEAT_CODEC) {
            codec_supported = TRUE;
        }
        features &= BTA_AG_SDP_FEAT_SPEC;
        /* Codec bit position is different in SDP and in BRSF */
        if (codec_supported) {
            features |= 0x0020;
        }
        UINT16_TO_BE_FIELD(buf, features);
        result &= SDP_AddAttribute(sdp_handle, ATTR_ID_SUPPORTED_FEATURES, UINT_DESC_TYPE, 2, buf);
    }
    /* add browse group list */
    result &= SDP_AddUuidSequence(sdp_handle, ATTR_ID_BROWSE_GROUP_LIST, 1, browse_list);
    return result;
}

/*******************************************************************************
**
** Function         bta_ag_create_records
**
** Description      Create SDP records for registered services.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_create_records(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    tBTA_SERVICE_MASK   services;
    services = p_scb->reg_services >> BTA_HSP_SERVICE_ID;

    for (int i = 0; i < BTA_AG_NUM_IDX && services != 0; i++, services >>= 1) {
        /* if service is set in mask */
        if (services & 1) {
            /* add sdp record if not already registered */
            if (bta_ag_cb.profile[i].sdp_handle == 0) {
                bta_ag_cb.profile[i].sdp_handle = SDP_CreateRecord();
                bta_ag_cb.profile[i].scn = BTM_AllocateSCN();
                bta_ag_add_record(bta_ag_uuid[i], p_data->api_register.p_name[i],
                    bta_ag_cb.profile[i].scn, p_data->api_register.features,
                    bta_ag_cb.profile[i].sdp_handle);
                bta_sys_add_uuid(bta_ag_uuid[i]);
            }
        }
    }
    p_scb->hsp_version = HSP_VERSION_1_2;
}

/*******************************************************************************
**
** Function         bta_ag_del_records
**
** Description      Delete SDP records for any registered services.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_del_records(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    tBTA_AG_SCB         *p = &bta_ag_cb.scb[0];
    tBTA_SERVICE_MASK   services;
    tBTA_SERVICE_MASK   others = 0;
    int                 i;
    UNUSED(p_data);

    /* get services of all other registered servers */
    for (i = 0; i < BTA_AG_NUM_IDX; i++, p++) {
        if (p_scb == p) {
            continue;
        }
        if (p->in_use && p->dealloc == FALSE) {
            others |= p->reg_services;
        }
    }
    others >>= BTA_HSP_SERVICE_ID;
    services = p_scb->reg_services >> BTA_HSP_SERVICE_ID;
    for (i = 0; i < BTA_AG_NUM_IDX && services != 0; i++, services >>= 1, others >>= 1)
    {
        /* if service registered for this scb and not registered for any other scb */
        if (((services & 1) == 1) && ((others & 1) == 0)) {
            APPL_TRACE_DEBUG("bta_ag_del_records %d", i);
            if (bta_ag_cb.profile[i].sdp_handle != 0) {
                SDP_DeleteRecord(bta_ag_cb.profile[i].sdp_handle);
                bta_ag_cb.profile[i].sdp_handle = 0;
            }
            BTM_FreeSCN(bta_ag_cb.profile[i].scn);
            BTM_SecClrService(bta_ag_sec_id[i]);
            bta_sys_remove_uuid(bta_ag_uuid[i]);
        }
    }
}

/*******************************************************************************
**
** Function         bta_ag_sdp_find_attr
**
** Description      Process SDP discovery results to find requested attributes
**                  for requested service.
**
**
** Returns          TRUE if results found, FALSE otherwise.
**
*******************************************************************************/
BOOLEAN bta_ag_sdp_find_attr(tBTA_AG_SCB *p_scb, tBTA_SERVICE_MASK service)
{
    tSDP_DISC_REC       *p_rec = NULL;
    tSDP_DISC_ATTR      *p_attr;
    tSDP_PROTOCOL_ELEM  pe;
    UINT16              uuid;
    BOOLEAN             result = FALSE;

    if (service & BTA_HFP_SERVICE_MASK) {
        uuid = UUID_SERVCLASS_HF_HANDSFREE;
        p_scb->peer_version = HFP_VERSION_1_1;   /* Default version */
    } else if (service & BTA_HSP_SERVICE_MASK && p_scb->role == BTA_AG_INT) {
        uuid = UUID_SERVCLASS_HEADSET_HS;
        p_scb->peer_version = 0x0100;   /* Default version */
    } else {
        return result;
    }

    /* loop through all records we found */
    while (TRUE)
    {
        /* get next record; if none found, we're done */
        if ((p_rec = SDP_FindServiceInDb(p_scb->p_disc_db, uuid, p_rec)) == NULL) {
            if (uuid == UUID_SERVCLASS_HEADSET_HS) {
                /* Search again in case the peer device is HSP v1.0 */
                uuid = UUID_SERVCLASS_HEADSET;
                if ((p_rec = SDP_FindServiceInDb(p_scb->p_disc_db, uuid, p_rec)) == NULL) {
                    break;
                }
            } else {
                break;
            }
        }

        /* get scn from proto desc list if initiator */
        if (p_scb->role == BTA_AG_INT) {
            if (SDP_FindProtocolListElemInRec(p_rec, UUID_PROTOCOL_RFCOMM, &pe)) {
                p_scb->peer_scn = (UINT8) pe.params[0];
            } else {
                continue;
            }
        }
        /* get profile version (if failure, version parameter is not updated) */
        SDP_FindProfileVersionInRec(p_rec, uuid, &p_scb->peer_version);
        /* get features if HFP */
        if (service & BTA_HFP_SERVICE_MASK) {
            if ((p_attr = SDP_FindAttributeInRec(p_rec, ATTR_ID_SUPPORTED_FEATURES)) != NULL) {
                /* Found attribute. Get value. */
                /* There might be race condition between SDP and BRSF.  */
                /* Do not update if we already received BRSF.           */
                if (p_scb->peer_features == 0)
                    p_scb->peer_features = p_attr->attr_value.v.u16;
            }
        } else {
            /* HSP */
            if ((p_attr = SDP_FindAttributeInRec(p_rec, ATTR_ID_REMOTE_AUDIO_VOLUME_CONTROL)) != NULL) {
                /* Remote volume control of HSP */
                if (p_attr->attr_value.v.u8) {
                    p_scb->peer_features |= BTA_AG_PEER_FEAT_VOL;
                } else {
                    p_scb->peer_features &= ~BTA_AG_PEER_FEAT_VOL;
                }
            }
        }
        /* found what we needed */
        result = TRUE;
        break;
    }
    return result;
}

/*******************************************************************************
**
** Function         bta_ag_do_disc
**
** Description      Do service discovery.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_do_disc(tBTA_AG_SCB *p_scb, tBTA_SERVICE_MASK service)
{
    tSDP_UUID       uuid_list[2];
    UINT16          num_uuid = 1;
    UINT16          attr_list[4];
    UINT8           num_attr;
    BOOLEAN         db_inited = FALSE;

    /* HFP initiator; get proto list and features */
    if (service & BTA_HFP_SERVICE_MASK && p_scb->role == BTA_AG_INT) {
        attr_list[0] = ATTR_ID_SERVICE_CLASS_ID_LIST;
        attr_list[1] = ATTR_ID_PROTOCOL_DESC_LIST;
        attr_list[2] = ATTR_ID_BT_PROFILE_DESC_LIST;
        attr_list[3] = ATTR_ID_SUPPORTED_FEATURES;
        num_attr = 4;
        uuid_list[0].uu.uuid16 = UUID_SERVCLASS_HF_HANDSFREE;
    } else if (service & BTA_HFP_SERVICE_MASK && p_scb->role == BTA_AG_ACP) {
        /* HFP acceptor; get features */
        attr_list[0] = ATTR_ID_SERVICE_CLASS_ID_LIST;
        attr_list[1] = ATTR_ID_BT_PROFILE_DESC_LIST;
        attr_list[2] = ATTR_ID_SUPPORTED_FEATURES;
        num_attr = 3;
        uuid_list[0].uu.uuid16 = UUID_SERVCLASS_HF_HANDSFREE;
    } else if (service & BTA_HSP_SERVICE_MASK && p_scb->role == BTA_AG_INT) {
        /* HSP initiator; get proto list */
        attr_list[0] = ATTR_ID_SERVICE_CLASS_ID_LIST;
        attr_list[1] = ATTR_ID_PROTOCOL_DESC_LIST;
        attr_list[2] = ATTR_ID_BT_PROFILE_DESC_LIST;
        attr_list[3] = ATTR_ID_REMOTE_AUDIO_VOLUME_CONTROL;
        num_attr = 4;
        uuid_list[0].uu.uuid16 = UUID_SERVCLASS_HEADSET;        /* Legacy from HSP v1.0 */
        if (p_scb->hsp_version >= HSP_VERSION_1_2) {
            uuid_list[1].uu.uuid16 = UUID_SERVCLASS_HEADSET_HS;
            num_uuid = 2;
        }
    } else {
        /* HSP acceptor; no discovery */
        return;
    }

    /* allocate buffer for sdp database */
    p_scb->p_disc_db = (tSDP_DISCOVERY_DB *) osi_malloc(BTA_AG_DISC_BUF_SIZE);
    if(p_scb->p_disc_db) {
        /* set up service discovery database; attr happens to be attr_list len */
        uuid_list[0].len = LEN_UUID_16;
        uuid_list[1].len = LEN_UUID_16;
        db_inited = SDP_InitDiscoveryDb(p_scb->p_disc_db, BTA_AG_DISC_BUF_SIZE, num_uuid,
                            uuid_list, num_attr, attr_list);
    }

    if(db_inited) {
        /*Service discovery not initiated */
        db_inited = SDP_ServiceSearchAttributeRequest(p_scb->peer_addr, p_scb->p_disc_db,
                                      bta_ag_sdp_cback_tbl[bta_ag_scb_to_idx(p_scb) - 1]);
    }
    if(!db_inited) {
        /*free discover db */
        bta_ag_free_db(p_scb, NULL);
        /* sent failed event */
        bta_ag_sm_execute(p_scb, BTA_AG_DISC_FAIL_EVT, NULL);
    }
}

/*******************************************************************************
**
** Function         bta_ag_free_db
**
** Description      Free discovery database.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_free_db(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    UNUSED(p_data);
    if (p_scb->p_disc_db != NULL) {
        osi_free(p_scb->p_disc_db);
        p_scb->p_disc_db = NULL;
    }
}

#endif /* #if (BTA_AG_INCLUDED == TRUE) */