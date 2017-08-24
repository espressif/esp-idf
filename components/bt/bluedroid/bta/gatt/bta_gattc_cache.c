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
 *  This file contains the GATT client discovery procedures and cache
 *  related functions.
 *
 ******************************************************************************/

#include "bt_target.h"

#if defined(GATTC_INCLUDED) && (GATTC_INCLUDED == TRUE)

#include <string.h>
#include "utl.h"
#include "bta_sys.h"
#include "sdp_api.h"
#include "sdpdefs.h"
#include "bta_gattc_int.h"
#include "btm_api.h"
#include "btm_ble_api.h"
#include "allocator.h"
#include "l2c_api.h"

#define LOG_TAG "bt_bta_gattc"
// #include "osi/include/log.h"

static void bta_gattc_char_dscpt_disc_cmpl(UINT16 conn_id, tBTA_GATTC_SERV *p_srvc_cb);
#if (SDP_INCLUDED == TRUE)
static tBTA_GATT_STATUS bta_gattc_sdp_service_disc(UINT16 conn_id, tBTA_GATTC_SERV *p_server_cb);
#define BTA_GATT_SDP_DB_SIZE 4096
#endif  ///SDP_INCLUDED == TRUE

/*****************************************************************************
**  Constants
*****************************************************************************/

#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
static char *bta_gattc_attr_type[] = {
    "I", /* Included Service */
    "C", /* Characteristic */
    "D" /* Characteristic Descriptor */
};
/* utility functions */

/*******************************************************************************
**
** Function         bta_gattc_display_cache_server
**
** Description      debug function to display the server cache.
**
** Returns          none.
**
*******************************************************************************/
static void bta_gattc_display_cache_server(tBTA_GATTC_CACHE *p_cache)
{
    UINT8 i = 0, j;
    tBTA_GATTC_CACHE    *p_cur_srvc = p_cache;
    tBTA_GATTC_CACHE_ATTR   *p_attr;

    APPL_TRACE_ERROR("<================Start Server Cache =============>");

    while (p_cur_srvc) {
        APPL_TRACE_ERROR("Service[%d]: handle[%d ~ %d] %s[0x%04x] inst[%d]",
                         i, p_cur_srvc->s_handle, p_cur_srvc->e_handle,
                         ((p_cur_srvc->service_uuid.id.uuid.len == 2) ? "uuid16" : "uuid128"),
                         p_cur_srvc->service_uuid.id.uuid.uu.uuid16,
                         p_cur_srvc->service_uuid.id.inst_id);
        i ++;

        p_attr = p_cur_srvc->p_attr;

        for (j = 0; p_attr; j ++ ) {
            APPL_TRACE_ERROR("\t Attr[0x%04x] handle[%d] uuid[0x%04x] inst[%d] type[%s] prop[0x%1x]",
                             j + 1, p_attr->attr_handle, p_attr->p_uuid->uuid16, p_attr->inst_id,
                             bta_gattc_attr_type[p_attr->attr_type], p_attr->property);

            p_attr = p_attr->p_next;
        }
        p_cur_srvc = p_cur_srvc->p_next;
    }

    APPL_TRACE_ERROR("<================End Server Cache =============>");
    APPL_TRACE_ERROR(" ");
}

/*******************************************************************************
**
** Function         bta_gattc_display_explore_record
**
** Description      debug function to display the exploration list
**
** Returns          none.
**
*******************************************************************************/
static void bta_gattc_display_explore_record(tBTA_GATTC_ATTR_REC *p_rec, UINT8 num_rec)
{
    UINT8 i;
    tBTA_GATTC_ATTR_REC *pp = p_rec;

    APPL_TRACE_ERROR("<================Start Explore Queue =============>");
    for (i = 0; i < num_rec; i ++, pp ++) {
        APPL_TRACE_ERROR("\t rec[%d] uuid[0x%04x] s_handle[%d] e_handle[%d] is_primary[%d]",
                         i + 1, pp->uuid.uu.uuid16, pp->s_handle, pp->e_handle, pp->is_primary);
    }
    APPL_TRACE_ERROR("<================ End Explore Queue =============>");
    APPL_TRACE_ERROR(" ");

}
#endif  /* BTA_GATT_DEBUG == TRUE */


/*******************************************************************************
**
** Function         bta_gattc_alloc_cache_buf
**
** Description      Allocate a buffer for database cache.
**
** Returns          status
**
*******************************************************************************/
BT_HDR *bta_gattc_alloc_cache_buf(tBTA_GATTC_SERV *p_srvc_cb)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *)osi_calloc(GATT_DB_BUF_SIZE)) == NULL) {
        APPL_TRACE_DEBUG("No resources: buffer allocation failed.");
        utl_freebuf((void **)&p_srvc_cb->p_srvc_list);
        p_srvc_cb->free_byte = 0;
    } else {
        p_srvc_cb->p_free = (UINT8 *) p_buf;
        p_srvc_cb->free_byte = GATT_DB_BUF_SIZE;

        /* link into buffer queue */
        fixed_queue_enqueue(p_srvc_cb->cache_buffer, p_buf);
    }
#if BTA_GATT_DEBUG== TRUE
    APPL_TRACE_DEBUG("allocating new buffer: free byte = %d", p_srvc_cb->free_byte);
#endif
    return p_buf;
}
/*******************************************************************************
**
** Function         bta_gattc_init_cache
**
** Description      Initialize the database cache and discovery related resources.
**
** Returns          status
**
*******************************************************************************/
tBTA_GATT_STATUS bta_gattc_init_cache(tBTA_GATTC_SERV *p_srvc_cb)
{
    tBTA_GATT_STATUS    status = BTA_GATT_OK;

    while (!fixed_queue_is_empty(p_srvc_cb->cache_buffer)) {
        osi_free (fixed_queue_dequeue(p_srvc_cb->cache_buffer));
    }

    utl_freebuf((void **)&p_srvc_cb->p_srvc_list);

    if ((p_srvc_cb->p_srvc_list = (tBTA_GATTC_ATTR_REC *)osi_malloc(BTA_GATTC_ATTR_LIST_SIZE)) == NULL) {
        APPL_TRACE_DEBUG("No resources: buffer allocation failed.");
        status = GATT_NO_RESOURCES;
    } else {
        p_srvc_cb->total_srvc = 0;
        p_srvc_cb->cur_srvc_idx =
            p_srvc_cb->cur_char_idx =
                p_srvc_cb->next_avail_idx = 0;

        if (bta_gattc_alloc_cache_buf(p_srvc_cb) == NULL) {
            status = GATT_NO_RESOURCES;
        } else {
            p_srvc_cb->p_cur_srvc = p_srvc_cb->p_srvc_cache = NULL;
        }
    }

    return status;
}
/*******************************************************************************
**
** Function         bta_gattc_get_srvc_inst_id
**
** Description      get service instance number
**
** Returns          instance ID of the service.
**
*******************************************************************************/
static UINT8 bta_gattc_get_srvc_inst_id(tBTA_GATTC_SERV *p_srvc_cb, tBT_UUID uuid)
{
    UINT8 i = 0, inst = 0;
    tBTA_GATTC_ATTR_REC   *p_srvc_rec;

    for (i = 0; i < p_srvc_cb->total_srvc; i ++)
        /*
        for (; i < p_srvc_cb->cur_srvc_idx; i ++)*/
    {
        p_srvc_rec = p_srvc_cb->p_srvc_list + i;

        if (bta_gattc_uuid_compare(&p_srvc_rec->uuid, &uuid, TRUE)) {
            inst ++;
        }
    }
    return inst ;
}
/*******************************************************************************
**
** Function         bta_gattc_get_char_inst_id
**
** Description      get characteristic instance number
**
** Returns          characteristic instance ID.
**
*******************************************************************************/
static UINT8 bta_gattc_get_char_inst_id(tBTA_GATTC_CACHE *p_service_cache, tBT_UUID *p_uuid)
{
    UINT8 inst = 0;
    tBTA_GATTC_CACHE_ATTR   *p_attr;
    tBT_UUID    attr_uuid;

    p_attr = p_service_cache->p_attr;

    while (p_attr) {
        bta_gattc_pack_attr_uuid(p_attr, &attr_uuid);

        if (bta_gattc_uuid_compare(&attr_uuid, p_uuid, TRUE)) {
            inst ++;
        }

        p_attr = p_attr->p_next;
    }

    return inst ;
}
/*******************************************************************************
**
** Function         bta_gattc_get_char_descr_inst_id
**
** Description      get characteristic descriptor instance number
**
** Returns          characteristic instance ID.
**
*******************************************************************************/
static UINT8 bta_gattc_get_char_descr_inst_id(tBTA_GATTC_CACHE_ATTR *p_char_attr, tBT_UUID *p_uuid)
{
    UINT8 inst = 0;
    tBT_UUID    attr_uuid;

    if (p_char_attr != NULL) {
        p_char_attr = p_char_attr->p_next;
    }

    while (p_char_attr) {
        bta_gattc_pack_attr_uuid(p_char_attr, &attr_uuid);

        if (bta_gattc_uuid_compare(&attr_uuid, p_uuid, TRUE)) {
            inst ++;
        }

        p_char_attr = p_char_attr->p_next;
    }
    return inst ;
}

/*******************************************************************************
**
** Function         bta_gattc_add_srvc_to_cache
**
** Description      Add a service into database cache.
**
** Returns          status
**
*******************************************************************************/
static tBTA_GATT_STATUS bta_gattc_add_srvc_to_cache(tBTA_GATTC_SERV *p_srvc_cb,
        UINT16 s_handle, UINT16 e_handle,
        tBT_UUID *p_uuid,
        BOOLEAN is_primary, UINT8 srvc_inst)
{
    tBTA_GATTC_CACHE    *p_new_srvc = NULL;
    tBTA_GATT_STATUS    status = BTA_GATT_OK;

#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
    APPL_TRACE_DEBUG("Add a service into Service");
    APPL_TRACE_DEBUG("free byte = %d,  req %d bytes.", p_srvc_cb->free_byte, sizeof(tBTA_GATTC_CACHE))
#endif

    if (p_srvc_cb->free_byte < sizeof(tBTA_GATTC_CACHE)) {
        if (bta_gattc_alloc_cache_buf(p_srvc_cb) == NULL) {
            return GATT_NO_RESOURCES;
        }
    }

    p_new_srvc = (tBTA_GATTC_CACHE *)p_srvc_cb->p_free;
    /* update service information */
    p_new_srvc->s_handle = s_handle;
    p_new_srvc->e_handle = e_handle;
    p_new_srvc->service_uuid.is_primary = is_primary;
    memcpy(&p_new_srvc->service_uuid.id.uuid, p_uuid, sizeof(tBT_UUID));
    p_new_srvc->service_uuid.id.inst_id = srvc_inst;
    p_new_srvc->p_next  = NULL;

    if (p_srvc_cb->p_cur_srvc != NULL) {
        p_srvc_cb->p_cur_srvc->p_next = p_new_srvc;
    }
    p_srvc_cb->p_cur_srvc = p_new_srvc;
    p_srvc_cb->p_cur_srvc->p_cur_char = NULL;

    /* first service */
    if (p_srvc_cb->p_srvc_cache == NULL) {
        p_srvc_cb->p_srvc_cache = p_new_srvc;
    }

    /* update buffer managament info */
    p_srvc_cb->p_free += sizeof(tBTA_GATTC_CACHE);
    p_srvc_cb->free_byte -= sizeof(tBTA_GATTC_CACHE);


    return status;
}
/*******************************************************************************
**
** Function         bta_gattc_add_attr_to_cache
**
** Description      Add an attribute into database cache buffer.
**
** Returns          status
**
*******************************************************************************/
static tBTA_GATT_STATUS bta_gattc_add_attr_to_cache(tBTA_GATTC_SERV *p_srvc_cb,
        UINT16 handle,
        tBT_UUID *p_uuid,
        UINT8 property,
        tBTA_GATTC_ATTR_TYPE type)
{
    tBTA_GATTC_CACHE_ATTR *p_attr;
    tBTA_GATT_STATUS    status = BTA_GATT_OK;
    UINT16  len = sizeof(tBTA_GATTC_CACHE_ATTR) + p_uuid->len;
    UINT8   *pp;

#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
    APPL_TRACE_DEBUG("bta_gattc_add_attr_to_cache: Add a [%s] into Service", bta_gattc_attr_type[type]);
    APPL_TRACE_DEBUG("handle=%d uuid16=0x%x property=0x%x type=%d", handle, p_uuid->uu.uuid16, property, type);
    APPL_TRACE_DEBUG("free byte = %d,  req %d bytes.", p_srvc_cb->free_byte, len);
#endif

    if (p_srvc_cb->p_cur_srvc == NULL) {
        APPL_TRACE_ERROR("Illegal action to add char/descr/incl srvc before adding a service!");
        return GATT_WRONG_STATE;
    }

    if (p_srvc_cb->free_byte < len) {
        if (bta_gattc_alloc_cache_buf(p_srvc_cb) == NULL) {
            return GATT_NO_RESOURCES;
        }
    }

    p_attr = (tBTA_GATTC_CACHE_ATTR *)p_srvc_cb->p_free;

    p_attr->attr_handle = handle;
    p_attr->attr_type   = type;
    p_attr->property    = property;
    p_attr->uuid_len    = p_uuid->len;
    p_attr->p_uuid      = (tBTA_GATTC_UUID *)(p_attr + 1);
    p_attr->p_next      = NULL;

    pp = (UINT8 *)p_attr->p_uuid;

    if (p_uuid->len == LEN_UUID_16) {
        UINT16_TO_STREAM(pp, p_uuid->uu.uuid16);
    } else if (p_uuid->len == LEN_UUID_128) {
        memcpy(pp, p_uuid->uu.uuid128, LEN_UUID_128);
    }

    if (type == BTA_GATTC_ATTR_TYPE_CHAR) {
        p_attr->inst_id = bta_gattc_get_char_inst_id(p_srvc_cb->p_cur_srvc, p_uuid);
        p_srvc_cb->p_cur_srvc->p_cur_char = p_attr;
    } else if (type == BTA_GATTC_ATTR_TYPE_CHAR_DESCR) {
        p_attr->inst_id = bta_gattc_get_char_descr_inst_id(p_srvc_cb->p_cur_srvc->p_cur_char, p_uuid);
    } else { /* TODO: --->> temp treat included service as single instance */
        p_attr->inst_id = 0;
    }

    /* update service information */
    p_srvc_cb->p_free += len;
    p_srvc_cb->free_byte -= len;

    /* first attribute within the service, update the attribute pointer */
    if (p_srvc_cb->p_cur_srvc->p_attr == NULL) {
        p_srvc_cb->p_cur_srvc->p_attr = p_attr;
    }
    if (p_srvc_cb->p_cur_srvc->p_last_attr != NULL) {
        p_srvc_cb->p_cur_srvc->p_last_attr->p_next = p_attr;
    }

    p_srvc_cb->p_cur_srvc->p_last_attr = p_attr;

    return status;
}

/*******************************************************************************
**
** Function         bta_gattc_get_disc_range
**
** Description      get discovery stating and ending handle range.
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_get_disc_range(tBTA_GATTC_SERV *p_srvc_cb, UINT16 *p_s_hdl, UINT16 *p_e_hdl, BOOLEAN is_srvc)
{
    tBTA_GATTC_ATTR_REC *p_rec = NULL;

    if (is_srvc) {
        p_rec = p_srvc_cb->p_srvc_list + p_srvc_cb->cur_srvc_idx;
        *p_s_hdl = p_rec->s_handle;
    } else {
        p_rec = p_srvc_cb->p_srvc_list + p_srvc_cb->cur_char_idx;
        *p_s_hdl = p_rec->s_handle + 1;
    }

    *p_e_hdl = p_rec->e_handle;
#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
    APPL_TRACE_DEBUG("discover range [%d ~ %d]", p_rec->s_handle, p_rec->e_handle);
#endif
    return;
}
/*******************************************************************************
**
** Function         bta_gattc_discover_pri_service
**
** Description      Start primary service discovery
**
** Returns          status of the operation.
**
*******************************************************************************/
tBTA_GATT_STATUS bta_gattc_discover_pri_service(UINT16 conn_id, tBTA_GATTC_SERV *p_server_cb,
        UINT8 disc_type)
{
    tBTA_GATTC_CLCB     *p_clcb = bta_gattc_find_clcb_by_conn_id(conn_id);
    tBTA_GATT_STATUS    status =  BTA_GATT_ERROR;

    if (p_clcb) {
        if (p_clcb->transport == BTA_TRANSPORT_LE) {
            status = bta_gattc_discover_procedure(conn_id, p_server_cb, disc_type);
        } else {
 #if (SDP_INCLUDED == TRUE)
            status = bta_gattc_sdp_service_disc(conn_id, p_server_cb);
 #endif ///SDP_INCLUDED == TRUE
        }
    }

    return status;
}
/*******************************************************************************
**
** Function         bta_gattc_discover_procedure
**
** Description      Start a particular type of discovery procedure on server.
**
** Returns          status of the operation.
**
*******************************************************************************/
tBTA_GATT_STATUS bta_gattc_discover_procedure(UINT16 conn_id, tBTA_GATTC_SERV *p_server_cb,
        UINT8 disc_type)
{
    tGATT_DISC_PARAM param;
    BOOLEAN is_service = TRUE;

    memset(&param, 0, sizeof(tGATT_DISC_PARAM));

    if (disc_type == GATT_DISC_SRVC_ALL || disc_type == GATT_DISC_SRVC_BY_UUID) {
        param.s_handle = 1;
        param.e_handle = 0xFFFF;
    } else {
        if (disc_type == GATT_DISC_CHAR_DSCPT) {
            is_service = FALSE;
        }

        bta_gattc_get_disc_range(p_server_cb, &param.s_handle, &param.e_handle, is_service);

        if (param.s_handle > param.e_handle) {
            return GATT_ERROR;
        }
    }
    return GATTC_Discover (conn_id, disc_type, &param);

}
/*******************************************************************************
**
** Function         bta_gattc_start_disc_include_srvc
**
** Description      Start discovery for included service
**
** Returns          status of the operation.
**
*******************************************************************************/
tBTA_GATT_STATUS bta_gattc_start_disc_include_srvc(UINT16 conn_id, tBTA_GATTC_SERV *p_srvc_cb)
{
    return bta_gattc_discover_procedure(conn_id, p_srvc_cb, GATT_DISC_INC_SRVC);
}
/*******************************************************************************
**
** Function         bta_gattc_start_disc_char
**
** Description      Start discovery for characteristic
**
** Returns          status of the operation.
**
*******************************************************************************/
tBTA_GATT_STATUS bta_gattc_start_disc_char(UINT16 conn_id, tBTA_GATTC_SERV *p_srvc_cb)
{
    p_srvc_cb->total_char = 0;

    return bta_gattc_discover_procedure(conn_id, p_srvc_cb, GATT_DISC_CHAR);
}
/*******************************************************************************
**
** Function         bta_gattc_start_disc_char_dscp
**
** Description      Start discovery for characteristic descriptor
**
** Returns          none.
**
*******************************************************************************/
void bta_gattc_start_disc_char_dscp(UINT16 conn_id, tBTA_GATTC_SERV *p_srvc_cb)
{
    APPL_TRACE_DEBUG("starting discover characteristics descriptor");

    if (bta_gattc_discover_procedure(conn_id, p_srvc_cb, GATT_DISC_CHAR_DSCPT) != 0) {
        bta_gattc_char_dscpt_disc_cmpl(conn_id, p_srvc_cb);
    }

}
/*******************************************************************************
**
** Function         bta_gattc_explore_srvc
**
** Description      process the service discovery complete event
**
** Returns          status
**
*******************************************************************************/
static void bta_gattc_explore_srvc(UINT16 conn_id, tBTA_GATTC_SERV *p_srvc_cb)
{
    tBTA_GATTC_ATTR_REC *p_rec = p_srvc_cb->p_srvc_list + p_srvc_cb->cur_srvc_idx;
    tBTA_GATTC_CLCB *p_clcb = bta_gattc_find_clcb_by_conn_id(conn_id);

    APPL_TRACE_DEBUG("Start service discovery: srvc_idx = %d", p_srvc_cb->cur_srvc_idx);

    p_srvc_cb->cur_char_idx = p_srvc_cb->next_avail_idx = p_srvc_cb->total_srvc;

    if (p_clcb == NULL) {
        APPL_TRACE_ERROR("unknown connection ID");
        return;
    }
    /* start expore a service if there is service not been explored */
    if (p_srvc_cb->cur_srvc_idx < p_srvc_cb->total_srvc) {
        /* add the first service into cache */
        if (bta_gattc_add_srvc_to_cache (p_srvc_cb,
                                         p_rec->s_handle,
                                         p_rec->e_handle,
                                         &p_rec->uuid,
                                         p_rec->is_primary,
                                         p_rec->srvc_inst_id) == 0) {
            /* start discovering included services */
            bta_gattc_start_disc_include_srvc(conn_id, p_srvc_cb);
            return;
        }
    }
    /* no service found at all, the end of server discovery*/
    LOG_DEBUG("%s no more services found", __func__);

#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
    bta_gattc_display_cache_server(p_srvc_cb->p_srvc_cache);
#endif

    //server discover end, update connection parameters
#if BLE_INCLUDED == TRUE
    if (p_clcb->transport == BTA_TRANSPORT_LE) {
        L2CA_EnableUpdateBleConnParams(p_clcb->p_srcb->server_bda, TRUE);
    }
#endif
    /* save cache to NV */
    p_clcb->p_srcb->state = BTA_GATTC_SERV_SAVE;
    bta_gattc_co_cache_open(p_srvc_cb->server_bda, BTA_GATTC_CI_CACHE_OPEN_EVT,
                            conn_id, TRUE);
}
/*******************************************************************************
**
** Function         bta_gattc_incl_srvc_disc_cmpl
**
** Description      process the relationship discovery complete event
**
** Returns          status
**
*******************************************************************************/
static void bta_gattc_incl_srvc_disc_cmpl(UINT16 conn_id, tBTA_GATTC_SERV *p_srvc_cb)
{
    p_srvc_cb->cur_char_idx = p_srvc_cb->total_srvc;

    /* start discoverying characteristic */
    bta_gattc_start_disc_char(conn_id, p_srvc_cb);
}
/*******************************************************************************
**
** Function         bta_gattc_char_disc_cmpl
**
** Description      process the characteristic discovery complete event
**
** Returns          status
**
*******************************************************************************/
static void bta_gattc_char_disc_cmpl(UINT16 conn_id, tBTA_GATTC_SERV *p_srvc_cb)
{
    tBTA_GATTC_ATTR_REC *p_rec = p_srvc_cb->p_srvc_list + p_srvc_cb->cur_char_idx;

    /* if there are characteristic needs to be explored */
    if (p_srvc_cb->total_char > 0) {
        /* add the first characteristic into cache */
        bta_gattc_add_attr_to_cache (p_srvc_cb,
                                     p_rec->s_handle,
                                     &p_rec->uuid,
                                     p_rec->property,
                                     BTA_GATTC_ATTR_TYPE_CHAR);

        /* start discoverying characteristic descriptor , if failed, disc for next char*/
        bta_gattc_start_disc_char_dscp(conn_id, p_srvc_cb);
    } else { /* otherwise start with next service */
        p_srvc_cb->cur_srvc_idx ++;

        bta_gattc_explore_srvc (conn_id, p_srvc_cb);
    }
}
/*******************************************************************************
**
** Function         bta_gattc_char_dscpt_disc_cmpl
**
** Description      process the char descriptor discovery complete event
**
** Returns          status
**
*******************************************************************************/
static void bta_gattc_char_dscpt_disc_cmpl(UINT16 conn_id, tBTA_GATTC_SERV *p_srvc_cb)
{
    tBTA_GATTC_ATTR_REC *p_rec = NULL;

    if (-- p_srvc_cb->total_char > 0) {
        p_rec = p_srvc_cb->p_srvc_list + (++ p_srvc_cb->cur_char_idx);
        /* add the next characteristic into cache */
        bta_gattc_add_attr_to_cache (p_srvc_cb,
                                     p_rec->s_handle,
                                     &p_rec->uuid,
                                     p_rec->property,
                                     BTA_GATTC_ATTR_TYPE_CHAR);

        /* start discoverying next characteristic for char descriptor */
        bta_gattc_start_disc_char_dscp(conn_id, p_srvc_cb);
    } else
        /* all characteristic has been explored, start with next service if any */
    {
#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
        APPL_TRACE_ERROR("all char has been explored");
#endif
        p_srvc_cb->cur_srvc_idx ++;
        bta_gattc_explore_srvc (conn_id, p_srvc_cb);
    }

}
static BOOLEAN bta_gattc_srvc_in_list(tBTA_GATTC_SERV *p_srvc_cb, UINT16 s_handle,
                                      UINT16 e_handle, tBT_UUID uuid)
{
    tBTA_GATTC_ATTR_REC *p_rec = NULL;
    UINT8   i;
    BOOLEAN exist_srvc = FALSE;
    UNUSED(uuid);

    if (!GATT_HANDLE_IS_VALID(s_handle) || !GATT_HANDLE_IS_VALID(e_handle)) {
        APPL_TRACE_ERROR("invalid included service handle: [0x%04x ~ 0x%04x]", s_handle, e_handle);
        exist_srvc = TRUE;
    } else {
        for (i = 0; i < p_srvc_cb->next_avail_idx; i ++) {
            p_rec = p_srvc_cb->p_srvc_list + i;

            /* a new service should not have any overlap with other service handle range */
            if (p_rec->s_handle == s_handle || p_rec->e_handle == e_handle) {
                exist_srvc = TRUE;
                break;
            }
        }
    }
    return exist_srvc;
}
/*******************************************************************************
**
** Function         bta_gattc_add_srvc_to_list
**
** Description      Add a service into explore pending list
**
** Returns          status
**
*******************************************************************************/
static tBTA_GATT_STATUS bta_gattc_add_srvc_to_list(tBTA_GATTC_SERV *p_srvc_cb,
        UINT16 s_handle, UINT16 e_handle,
        tBT_UUID uuid, BOOLEAN is_primary)
{
    tBTA_GATTC_ATTR_REC *p_rec = NULL;
    tBTA_GATT_STATUS    status = BTA_GATT_OK;

    if (p_srvc_cb->p_srvc_list && p_srvc_cb->next_avail_idx < BTA_GATTC_MAX_CACHE_CHAR) {
        p_rec = p_srvc_cb->p_srvc_list + p_srvc_cb->next_avail_idx;

        APPL_TRACE_DEBUG("%s handle=%d, service type=0x%04x",
                         __func__, s_handle, uuid.uu.uuid16);

        p_rec->s_handle     = s_handle;
        p_rec->e_handle     = e_handle;
        p_rec->is_primary   = is_primary;
        p_rec->srvc_inst_id = bta_gattc_get_srvc_inst_id(p_srvc_cb, uuid);
        memcpy(&p_rec->uuid, &uuid, sizeof(tBT_UUID));

        p_srvc_cb->total_srvc ++;
        p_srvc_cb->next_avail_idx ++;
    } else {
        /* allocate bigger buffer ?? */
        status = GATT_DB_FULL;

        APPL_TRACE_ERROR("service not added, no resources or wrong state");
    }
    return status;
}
/*******************************************************************************
**
** Function         bta_gattc_add_char_to_list
**
** Description      Add a characteristic into explore pending list
**
** Returns          status
**
*******************************************************************************/
static tBTA_GATT_STATUS bta_gattc_add_char_to_list(tBTA_GATTC_SERV *p_srvc_cb,
        UINT16 decl_handle, UINT16 value_handle,
        tBT_UUID uuid, UINT8 property)
{
    tBTA_GATTC_ATTR_REC *p_rec = NULL;
    tBTA_GATT_STATUS    status = BTA_GATT_OK;

    if (p_srvc_cb->p_srvc_list == NULL) {
        APPL_TRACE_ERROR("No service available, unexpected char discovery result");
        status = BTA_GATT_INTERNAL_ERROR;
    } else if (p_srvc_cb->next_avail_idx < BTA_GATTC_MAX_CACHE_CHAR) {

        p_rec = p_srvc_cb->p_srvc_list + p_srvc_cb->next_avail_idx;

        p_srvc_cb->total_char ++;

        p_rec->s_handle = value_handle;
        p_rec->property = property;
        p_rec->e_handle = (p_srvc_cb->p_srvc_list + p_srvc_cb->cur_srvc_idx)->e_handle;
        memcpy(&p_rec->uuid, &uuid, sizeof(tBT_UUID));

        /* update the endind handle of pervious characteristic if available */
        if (p_srvc_cb->total_char > 1) {
            p_rec -= 1;
            p_rec->e_handle = decl_handle - 1;
        }
        p_srvc_cb->next_avail_idx ++;
    } else {
        APPL_TRACE_ERROR("char not added, no resources");
        /* allocate bigger buffer ?? */
        status = BTA_GATT_DB_FULL;
    }
    return status;

}
/*******************************************************************************
**
** Function         bta_gattc_sdp_callback
**
** Description      Process the discovery result from sdp
**
** Returns          void
**
*******************************************************************************/
#if (SDP_INCLUDED == TRUE)
void bta_gattc_sdp_callback (UINT16 sdp_status)
{
    tSDP_DISC_REC       *p_sdp_rec = NULL;
    tBT_UUID            service_uuid;
    tSDP_PROTOCOL_ELEM  pe;
    UINT16              start_handle = 0, end_handle = 0;
    tBTA_GATTC_SERV     *p_srvc_cb = bta_gattc_find_scb_by_cid(bta_gattc_cb.sdp_conn_id);

    if (((sdp_status == SDP_SUCCESS) || (sdp_status == SDP_DB_FULL)) && p_srvc_cb != NULL) {
        do {
            /* find a service record, report it */
            p_sdp_rec = SDP_FindServiceInDb(bta_gattc_cb.p_sdp_db,
                                            0, p_sdp_rec);
            if (p_sdp_rec) {
                if (SDP_FindServiceUUIDInRec(p_sdp_rec, &service_uuid)) {

                    if (SDP_FindProtocolListElemInRec(p_sdp_rec, UUID_PROTOCOL_ATT, &pe)) {
                        start_handle    = (UINT16) pe.params[0];
                        end_handle      = (UINT16) pe.params[1];

#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
                        APPL_TRACE_EVENT("Found ATT service [0x%04x] handle[0x%04x ~ 0x%04x]",
                                         service_uuid.uu.uuid16, start_handle, end_handle);
#endif

                        if (GATT_HANDLE_IS_VALID(start_handle) && GATT_HANDLE_IS_VALID(end_handle) &&
                                p_srvc_cb != NULL) {
                            /* discover services result, add services into a service list */
                            bta_gattc_add_srvc_to_list(p_srvc_cb,
                                                       start_handle,
                                                       end_handle,
                                                       service_uuid,
                                                       TRUE);
                        } else {
                            APPL_TRACE_ERROR("invalid start_handle = %d end_handle = %d",
                                             start_handle, end_handle);
                        }
                    }


                }
            }
        } while (p_sdp_rec);
    }

    if ( p_srvc_cb != NULL)
        /* start discover primary service */
    {
        bta_gattc_explore_srvc(bta_gattc_cb.sdp_conn_id, p_srvc_cb);
    } else {
        APPL_TRACE_ERROR("GATT service discovery is done on unknown connection");
    }

    osi_free(bta_gattc_cb.p_sdp_db);
    bta_gattc_cb.p_sdp_db  = NULL;
    bta_gattc_cb.sdp_conn_id = 0;
}
#endif  ///SDP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         bta_gattc_sdp_service_disc
**
** Description      Start DSP Service Discovert
**
** Returns          void
**
*******************************************************************************/
#if (SDP_INCLUDED == TRUE)
static tBTA_GATT_STATUS bta_gattc_sdp_service_disc(UINT16 conn_id, tBTA_GATTC_SERV *p_server_cb)
{
    tSDP_UUID       uuid;
    UINT16          num_attrs = 2;
    UINT16          attr_list[2];
    tBTA_GATT_STATUS    status = BTA_GATT_ERROR;

    memset (&uuid, 0, sizeof(tSDP_UUID));

    uuid.len = LEN_UUID_16;
    uuid.uu.uuid16 = UUID_PROTOCOL_ATT;

    if ((bta_gattc_cb.p_sdp_db = (tSDP_DISCOVERY_DB *)osi_malloc(BTA_GATT_SDP_DB_SIZE)) != NULL) {
        attr_list[0] = ATTR_ID_SERVICE_CLASS_ID_LIST;
        attr_list[1] = ATTR_ID_PROTOCOL_DESC_LIST;

        SDP_InitDiscoveryDb (bta_gattc_cb.p_sdp_db, BTA_GATT_SDP_DB_SIZE, 1,
                             &uuid, num_attrs, attr_list);

        if (!SDP_ServiceSearchAttributeRequest (p_server_cb->server_bda,
                                                bta_gattc_cb.p_sdp_db, &bta_gattc_sdp_callback)) {
            osi_free(bta_gattc_cb.p_sdp_db);
            bta_gattc_cb.p_sdp_db = NULL;
        } else {
            bta_gattc_cb.sdp_conn_id = conn_id;
            status = BTA_GATT_OK;
        }
    }
    return status;
}
#endif  ///SDP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         bta_gattc_disc_res_cback
**                  bta_gattc_disc_cmpl_cback
**
** Description      callback functions to GATT client stack.
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_disc_res_cback (UINT16 conn_id, tGATT_DISC_TYPE disc_type, tGATT_DISC_RES *p_data)
{
    tBTA_GATTC_SERV *p_srvc_cb = NULL;
    BOOLEAN          pri_srvc;
    tBTA_GATTC_CLCB *p_clcb = bta_gattc_find_clcb_by_conn_id(conn_id);

    p_srvc_cb = bta_gattc_find_scb_by_cid(conn_id);

    if (p_srvc_cb != NULL && p_clcb != NULL && p_clcb->state == BTA_GATTC_DISCOVER_ST) {
        switch (disc_type) {
        case GATT_DISC_SRVC_ALL:
            /* discover services result, add services into a service list */
            bta_gattc_add_srvc_to_list(p_srvc_cb,
                                       p_data->handle,
                                       p_data->value.group_value.e_handle,
                                       p_data->value.group_value.service_type,
                                       TRUE);

            break;
        case GATT_DISC_SRVC_BY_UUID:
            bta_gattc_add_srvc_to_list(p_srvc_cb,
                                       p_data->handle,
                                       p_data->value.group_value.e_handle,
                                       p_data->value.group_value.service_type,
                                       TRUE);
            break;

        case GATT_DISC_INC_SRVC:
            /* add included service into service list if it's secondary or it never showed up
               in the primary service search */
            pri_srvc = bta_gattc_srvc_in_list(p_srvc_cb,
                                              p_data->value.incl_service.s_handle,
                                              p_data->value.incl_service.e_handle,
                                              p_data->value.incl_service.service_type);

            if (!pri_srvc)
                bta_gattc_add_srvc_to_list(p_srvc_cb,
                                           p_data->value.incl_service.s_handle,
                                           p_data->value.incl_service.e_handle,
                                           p_data->value.incl_service.service_type,
                                           FALSE);
            /* add into database */
            bta_gattc_add_attr_to_cache(p_srvc_cb,
                                        p_data->handle,
                                        &p_data->value.incl_service.service_type,
                                        pri_srvc,
                                        BTA_GATTC_ATTR_TYPE_INCL_SRVC);
            break;

        case GATT_DISC_CHAR:
            /* add char value into database */
            bta_gattc_add_char_to_list(p_srvc_cb,
                                       p_data->handle,
                                       p_data->value.dclr_value.val_handle,
                                       p_data->value.dclr_value.char_uuid,
                                       p_data->value.dclr_value.char_prop);
            break;

        case GATT_DISC_CHAR_DSCPT:
            bta_gattc_add_attr_to_cache(p_srvc_cb, p_data->handle, &p_data->type, 0,
                                        BTA_GATTC_ATTR_TYPE_CHAR_DESCR);
            break;
        }
    }
}
void bta_gattc_disc_cmpl_cback (UINT16 conn_id, tGATT_DISC_TYPE disc_type, tGATT_STATUS status)
{
    tBTA_GATTC_SERV *p_srvc_cb;
    tBTA_GATTC_CLCB *p_clcb = bta_gattc_find_clcb_by_conn_id(conn_id);

    if ( p_clcb && (status != GATT_SUCCESS || p_clcb->status != GATT_SUCCESS) ) {
        if (p_clcb->status == GATT_SUCCESS) {
            p_clcb->status = status;
        }
        bta_gattc_sm_execute(p_clcb, BTA_GATTC_DISCOVER_CMPL_EVT, NULL);
        return;
    }
    p_srvc_cb = bta_gattc_find_scb_by_cid(conn_id);

    if (p_srvc_cb != NULL) {
        switch (disc_type) {
        case GATT_DISC_SRVC_ALL:
        case GATT_DISC_SRVC_BY_UUID:
#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
            bta_gattc_display_explore_record(p_srvc_cb->p_srvc_list, p_srvc_cb->next_avail_idx);
#endif
            bta_gattc_explore_srvc(conn_id, p_srvc_cb);
            break;

        case GATT_DISC_INC_SRVC:
            bta_gattc_incl_srvc_disc_cmpl(conn_id, p_srvc_cb);

            break;

        case GATT_DISC_CHAR:
#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
            bta_gattc_display_explore_record(p_srvc_cb->p_srvc_list, p_srvc_cb->next_avail_idx);
#endif
            bta_gattc_char_disc_cmpl(conn_id, p_srvc_cb);
            break;

        case GATT_DISC_CHAR_DSCPT:
            bta_gattc_char_dscpt_disc_cmpl(conn_id, p_srvc_cb);
            break;
        }
    }
}
/*******************************************************************************
**
** Function         bta_gattc_id2handle
**
** Description      map GATT ID to handle in a given cache.
**
** Returns          the handle mapped. 0 if not found.
**
*******************************************************************************/
UINT16 bta_gattc_id2handle(tBTA_GATTC_SERV *p_srcb, tBTA_GATT_SRVC_ID *p_service_id,
                           tBTA_GATT_ID *p_char_id, tBTA_GATT_ID *p_descr_uuid)
{
    tBTA_GATTC_CACHE    *p_cache = p_srcb->p_srvc_cache;
    tBTA_GATTC_CACHE_ATTR   *p_attr;
    UINT8       j;
    UINT16      handle = 0;
    tBT_UUID    attr_uuid;
    BOOLEAN     char_map = FALSE, done = FALSE;

    while (p_service_id && p_cache && !done) {
#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
        APPL_TRACE_DEBUG("Service: handle[%d] uuid[0x%04x] inst[%d]",
                         p_cache->s_handle, p_cache->service_uuid.id.uuid.uu.uuid16,
                         p_cache->service_uuid.id.inst_id);
#endif
        p_attr = p_cache->p_attr;

        if (bta_gattc_srvcid_compare(p_service_id, &p_cache->service_uuid)) {
            for (j = 0; p_attr; j ++) {
#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
                APPL_TRACE_DEBUG("\t Attr[0x%04x] handle[0x%04x] uuid[0x%04x] inst[%d] type[%d]",
                                 j + 1, p_attr->attr_handle, p_attr->p_uuid->uuid16,
                                 p_attr->inst_id, p_attr->attr_type);
#endif
                bta_gattc_pack_attr_uuid(p_attr, &attr_uuid);

                if (bta_gattc_uuid_compare(&p_char_id->uuid, &attr_uuid, TRUE) &&
                        p_char_id->inst_id == p_attr->inst_id) {
                    if (p_descr_uuid == NULL) {
                        handle = p_attr->attr_handle;
                        done = TRUE;
                        break;
                    } else {
#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
                        APPL_TRACE_DEBUG("found matching characteristic for the descriptor");
#endif
                        char_map = TRUE;
                    }
                } else if (char_map == TRUE) {
                    if (p_attr->attr_type == BTA_GATTC_ATTR_TYPE_CHAR_DESCR) {

                        if (p_descr_uuid != NULL &&
                                bta_gattc_uuid_compare(&p_descr_uuid->uuid, &attr_uuid, TRUE) &&
                                p_descr_uuid->inst_id == p_attr->inst_id) {
#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
                            APPL_TRACE_DEBUG("found descriptor!!");
#endif
                            handle = p_attr->attr_handle;
                            done = TRUE;
                            break;
                        } else {
#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
                            APPL_TRACE_DEBUG("descriptor UUID not matching");
#endif
                        }
                    } else { /* another char */
#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
                        APPL_TRACE_DEBUG("no matching descptr found!!start of next characteristic");
#endif
                        char_map = FALSE;
                        done = TRUE;
                        break;
                    }
                }
                p_attr = p_attr->p_next;
            }
        }
        p_cache = p_cache->p_next;
    }

    return handle;
}
/*******************************************************************************
**
** Function         bta_gattc_handle2id
**
** Description      map a handle to GATT ID in a given cache.
**
** Returns          FALSE if map can not be found.
**
*******************************************************************************/

BOOLEAN bta_gattc_handle2id(tBTA_GATTC_SERV *p_srcb, UINT16 handle, tBTA_GATT_SRVC_ID *p_service_id,
                            tBTA_GATT_ID *p_char_id, tBTA_GATT_ID *p_descr_type)
{
    tBTA_GATTC_CACHE    *p_cache = p_srcb->p_srvc_cache;
    tBTA_GATTC_CACHE_ATTR   *p_attr, *p_char = NULL;
    UINT8       j;

    memset(p_service_id, 0, sizeof(tBTA_GATT_SRVC_ID));
    memset(p_char_id, 0, sizeof(tBTA_GATT_ID));
    memset(p_descr_type, 0, sizeof(tBTA_GATT_ID));

    while (p_cache) {
#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
        APPL_TRACE_DEBUG("Service: handle[%d] uuid[0x%04x] inst[%d]",
                         p_cache->s_handle, p_cache->service_uuid.id.uuid.uu.uuid16,
                         p_cache->service_uuid.id.inst_id);
#endif
        /* a service found */
        if (p_cache->s_handle == handle) {
            memcpy(p_service_id, &p_cache->service_uuid, sizeof(tBTA_GATT_SRVC_ID));

            return TRUE;
        } else { /* start looking for attributes within the service */
            p_attr = p_cache->p_attr;

            for (j = 0; p_attr; j ++) {
#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
                APPL_TRACE_DEBUG("\t Attr[0x%04x] handle[0x%04x] uuid[0x%04x] inst[%d] type[%d]",
                                 j + 1, p_attr->attr_handle, p_attr->p_uuid->uuid16,
                                 p_attr->inst_id, p_attr->attr_type);
#endif
                if (p_attr->attr_type == BTA_GATTC_ATTR_TYPE_CHAR) {
                    p_char = p_attr;
                }

                if (handle == p_attr->attr_handle) {
                    memcpy(p_service_id, &p_cache->service_uuid, sizeof(tBTA_GATT_SRVC_ID));

                    if (p_attr->attr_type == BTA_GATTC_ATTR_TYPE_CHAR_DESCR) {
                        bta_gattc_pack_attr_uuid(p_attr, &p_descr_type->uuid);
                        p_descr_type->inst_id = p_attr->inst_id;

                        if (p_char != NULL) {
                            bta_gattc_pack_attr_uuid(p_char, &p_char_id->uuid);
                            p_char_id->inst_id = p_char->inst_id;
                        } else {
                            APPL_TRACE_ERROR("descptr does not belong to any chracteristic");
                        }
                    } else
                        /* is a characterisitc value or included service */
                    {
                        bta_gattc_pack_attr_uuid(p_attr, &p_char_id->uuid);
                        p_char_id->inst_id = p_attr->inst_id;
                    }
                    return TRUE;
                }
                p_attr = p_attr->p_next;
            }
        }
        p_cache = p_cache->p_next;
    }

    return FALSE;
}

/*******************************************************************************
**
** Function         bta_gattc_search_service
**
** Description      search local cache for matching service record.
**
** Returns          FALSE if map can not be found.
**
*******************************************************************************/
void bta_gattc_search_service(tBTA_GATTC_CLCB *p_clcb, tBT_UUID *p_uuid)
{
    tBTA_GATTC_SERV     *p_srcb = p_clcb->p_srcb;
    tBTA_GATTC_CACHE    *p_cache = p_srcb->p_srvc_cache;
    tBTA_GATTC          cb_data;

    while (p_cache) {
        if (bta_gattc_uuid_compare(p_uuid, &p_cache->service_uuid.id.uuid, FALSE)) {
#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
            APPL_TRACE_DEBUG("found service [0x%04x], inst[%d] handle [%d]",
                             p_cache->service_uuid.id.uuid.uu.uuid16,
                             p_cache->service_uuid.id.inst_id,
                             p_cache->s_handle);
#endif
            if (p_clcb->p_rcb->p_cback) {
                memset(&cb_data, 0, sizeof(tBTA_GATTC));

                cb_data.srvc_res.conn_id = p_clcb->bta_conn_id;
                memcpy(&cb_data.srvc_res.service_uuid, &p_cache->service_uuid,
                       sizeof(tBTA_GATT_SRVC_ID));

                (* p_clcb->p_rcb->p_cback)(BTA_GATTC_SEARCH_RES_EVT, &cb_data);
            }
        }
        p_cache = p_cache->p_next;
    }
}
/*******************************************************************************
**
** Function         bta_gattc_find_record
**
** Description      search local cache for matching attribute record.
**
** Parameter        p_result: output parameter to store the characteristic/
**                            included service GATT ID.
**
** Returns          GATT_ERROR is no recording found. BTA_GATT_OK if record found.
**
*******************************************************************************/
static tBTA_GATT_STATUS bta_gattc_find_record(tBTA_GATTC_SERV *p_srcb,
        tBTA_GATTC_ATTR_TYPE attr_type,
        tBTA_GATT_SRVC_ID *p_service_id,
        tBTA_GATT_ID  *p_start_rec,
        tBT_UUID       *p_uuid_cond,
        tBTA_GATT_ID  *p_result,
        void *p_param)
{
    tBTA_GATTC_CACHE    *p_cache = p_srcb->p_srvc_cache;
    tBTA_GATT_STATUS    status = BTA_GATT_ERROR;
    UINT8               i, j;
    tBTA_GATTC_CACHE_ATTR   *p_attr;
    BOOLEAN             char_found = FALSE, descr_found = FALSE;
    tBTA_GATT_ID        *p_descr_id = (tBTA_GATT_ID *)p_param;;

    for (i = 0; p_cache && status != BTA_GATT_OK; i ++) {
        if (bta_gattc_srvcid_compare(p_service_id, &p_cache->service_uuid)) {
#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
            APPL_TRACE_DEBUG("found matching service [0x%04x], inst[%d]",
                             p_cache->service_uuid.id.uuid.uu.uuid16,
                             p_cache->service_uuid.id.inst_id);
#endif
            p_attr = p_cache->p_attr;

            for (j = 0; p_attr; j ++) {
#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
                APPL_TRACE_DEBUG("\t Attr[%d] handle[0x%04x] uuid[0x%04x] inst[%d] type[%d]",
                                 j + 1, p_attr->attr_handle,
                                 p_attr->p_uuid->uuid16,
                                 p_attr->inst_id,
                                 p_attr->attr_type);
#endif
                bta_gattc_pack_attr_uuid(p_attr, &p_result->uuid);

                if (p_start_rec != NULL && char_found == FALSE) {
                    /* find the starting record first */
                    if (bta_gattc_uuid_compare(&p_start_rec->uuid, &p_result->uuid, FALSE) &&
                            p_start_rec->inst_id  == p_attr->inst_id &&
                            (attr_type == p_attr->attr_type ||
                             /* find descriptor would look for characteristic first */
                             (attr_type == BTA_GATTC_ATTR_TYPE_CHAR_DESCR &&
                              p_attr->attr_type == BTA_GATTC_ATTR_TYPE_CHAR))) {
                        char_found = TRUE;
                    }
                } else {
                    /* if looking for descriptor, here is the where the descrptor to be found */
                    if (attr_type == BTA_GATTC_ATTR_TYPE_CHAR_DESCR) {
                        /* next characeteristic already, return error */
                        if (p_attr->attr_type != BTA_GATTC_ATTR_TYPE_CHAR_DESCR) {
                            break;
                        } else {
                            /* find starting descriptor */
                            if (p_descr_id != NULL && !descr_found) {
                                if (bta_gattc_uuid_compare(&p_descr_id->uuid, &p_result->uuid, TRUE)
                                        && p_descr_id->inst_id == p_attr->inst_id) {
                                    descr_found = TRUE;
                                }
                            } else {
                                /* with matching descriptor */
                                if (bta_gattc_uuid_compare(p_uuid_cond, &p_result->uuid, FALSE)) {
                                    p_result->inst_id = p_attr->inst_id;
                                    status = BTA_GATT_OK;
                                    break;
                                }
                            }
                        }
                    } else {
                        if (bta_gattc_uuid_compare(p_uuid_cond, &p_result->uuid, FALSE) &&
                                attr_type == p_attr->attr_type) {

#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
                            APPL_TRACE_DEBUG("found char handle mapping characteristic");
#endif
                            p_result->inst_id = p_attr->inst_id;

                            if (p_param != NULL) {
                                if (attr_type == BTA_GATTC_ATTR_TYPE_CHAR ||
                                        attr_type == BTA_GATTC_ATTR_TYPE_INCL_SRVC) {
                                    *(tBTA_GATT_CHAR_PROP *)p_param = p_attr->property;
                                }
                            }

                            status = BTA_GATT_OK;
                            break;
                        }
                    }
                }
                p_attr = p_attr->p_next;
            }
#if (defined BTA_GATT_DEBUG && BTA_GATT_DEBUG == TRUE)
            if (status) {
                APPL_TRACE_ERROR("In the given service, can not find matching record");
            }
#endif
            break;
        }

        p_cache = p_cache->p_next;
    }
    return status;

}

/*******************************************************************************
**
** Function         bta_gattc_query_cache
**
** Description      search local cache for matching attribute record.
**
** Parameters       conn_id: connection ID which identify the server.
**                  p_srvc_id: the service ID of which the characteristic is belonged to.
**                  *p_start_rec: start the search from the next record
**                                  after the one identified by *p_start_rec.
**                  p_uuid_cond: UUID, if NULL find the first available
**                               characteristic/included service.
**                  p_output:   output parameter which will store the GATT ID
**                              of the characteristic /included service found.
**
** Returns          BTA_GATT_ERROR is no recording found. BTA_GATT_OK if record found.
**
*******************************************************************************/
tBTA_GATT_STATUS bta_gattc_query_cache(UINT16 conn_id,
                                       tBTA_GATTC_ATTR_TYPE query_type,
                                       tBTA_GATT_SRVC_ID *p_srvc_id,
                                       tBTA_GATT_ID *p_start_rec,
                                       tBT_UUID *p_uuid_cond,
                                       tBTA_GATT_ID *p_output,
                                       void *p_param)
{
    tBTA_GATTC_CLCB *p_clcb = bta_gattc_find_clcb_by_conn_id(conn_id);
    tBTA_GATT_STATUS status = BTA_GATT_ILLEGAL_PARAMETER;

    if (p_clcb != NULL ) {
        if (p_clcb->state == BTA_GATTC_CONN_ST) {
            if (p_clcb->p_srcb &&
                    !p_clcb->p_srcb->p_srvc_list && /* no active discovery */
                    p_clcb->p_srcb->p_srvc_cache) {
                status = bta_gattc_find_record(p_clcb->p_srcb,
                                               query_type,
                                               p_srvc_id,
                                               p_start_rec,
                                               p_uuid_cond,
                                               p_output,
                                               p_param);
            } else {
                status = BTA_GATT_ERROR;
                APPL_TRACE_ERROR("No server cache available");
            }
        } else {
            APPL_TRACE_ERROR("server cache not available, CLCB state = %d", p_clcb->state);

            status = (p_clcb->state == BTA_GATTC_DISCOVER_ST) ? BTA_GATT_BUSY : BTA_GATT_ERROR;
        }
    } else {
        APPL_TRACE_ERROR("Unknown conn ID: %d", conn_id);
    }

    return status;
}

/*******************************************************************************
**
** Function         bta_gattc_rebuild_cache
**
** Description      rebuild server cache from NV cache.
**
** Parameters
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_rebuild_cache(tBTA_GATTC_SERV *p_srvc_cb, UINT16 num_attr,
                             tBTA_GATTC_NV_ATTR *p_attr, UINT16 attr_index)
{
    /* first attribute loading, initialize buffer */
    APPL_TRACE_ERROR("bta_gattc_rebuild_cache");
    if (attr_index == 0) {
        while (!fixed_queue_is_empty(p_srvc_cb->cache_buffer)) {
            osi_free(fixed_queue_dequeue(p_srvc_cb->cache_buffer));
        }

        if (bta_gattc_alloc_cache_buf(p_srvc_cb) == NULL) {
            APPL_TRACE_ERROR("allocate cache buffer failed, no resources");
        } else {
            p_srvc_cb->p_cur_srvc = p_srvc_cb->p_srvc_cache = NULL;
        }
    }

    while (num_attr > 0 && p_attr != NULL) {
        switch (p_attr->attr_type) {
        case BTA_GATTC_ATTR_TYPE_SRVC:
            bta_gattc_add_srvc_to_cache(p_srvc_cb,
                                        p_attr->s_handle,
                                        p_attr->e_handle,
                                        &p_attr->uuid,
                                        p_attr->is_primary,
                                        p_attr->id);
            break;

        case BTA_GATTC_ATTR_TYPE_CHAR:
        case BTA_GATTC_ATTR_TYPE_CHAR_DESCR:
        case BTA_GATTC_ATTR_TYPE_INCL_SRVC:
            bta_gattc_add_attr_to_cache(p_srvc_cb,
                                        p_attr->s_handle,
                                        &p_attr->uuid,
                                        p_attr->prop,
                                        p_attr->attr_type);
            break;
        }
        p_attr ++;
        num_attr --;
    }
}

/*******************************************************************************
**
** Function         bta_gattc_fill_nv_attr
**
** Description      fill a NV attribute entry value
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_fill_nv_attr(tBTA_GATTC_NV_ATTR *p_attr, UINT8 type, UINT16 s_handle,
                            UINT16 e_handle, UINT8 id, tBT_UUID uuid, UINT8 prop,
                            BOOLEAN is_primary)
{
    p_attr->s_handle    = s_handle;
    p_attr->e_handle    = e_handle;
    p_attr->attr_type   = type;
    p_attr->is_primary  = is_primary;
    p_attr->id          = id;
    p_attr->prop        = prop;

    memcpy(&p_attr->uuid, &uuid, sizeof(tBT_UUID));
}
/*******************************************************************************
**
** Function         bta_gattc_cache_save
**
** Description      save the server cache into NV
**
** Returns          None.
**
*******************************************************************************/
BOOLEAN bta_gattc_cache_save(tBTA_GATTC_SERV *p_srvc_cb, UINT16 conn_id)
{
    tBTA_GATTC_CACHE        *p_cur_srvc = p_srvc_cb->p_srvc_cache;
    UINT8                   i = 0;
    UINT16                  offset = 0;
    tBTA_GATTC_NV_ATTR      nv_attr[BTA_GATTC_NV_LOAD_MAX];
    tBTA_GATTC_CACHE_ATTR   *p_attr;
    tBT_UUID                uuid;

    while (p_cur_srvc && i < BTA_GATTC_NV_LOAD_MAX) {
        if (offset ++ >= p_srvc_cb->attr_index) {
            bta_gattc_fill_nv_attr(&nv_attr[i++],
                                   BTA_GATTC_ATTR_TYPE_SRVC,
                                   p_cur_srvc->s_handle,
                                   p_cur_srvc->e_handle,
                                   p_cur_srvc->service_uuid.id.inst_id,
                                   p_cur_srvc->service_uuid.id.uuid,
                                   0,
                                   p_cur_srvc->service_uuid.is_primary);
        }

        p_attr = p_cur_srvc->p_attr;

        for (; p_attr && i < BTA_GATTC_NV_LOAD_MAX ; offset ++, p_attr = p_attr->p_next) {
            if (offset >= p_srvc_cb->attr_index) {
                if ((uuid.len = p_attr->uuid_len) == LEN_UUID_16) {
                    uuid.uu.uuid16 = p_attr->p_uuid->uuid16;
                } else {
                    memcpy(uuid.uu.uuid128, p_attr->p_uuid->uuid128, LEN_UUID_128);
                }

                bta_gattc_fill_nv_attr(&nv_attr[i++],
                                       p_attr->attr_type,
                                       p_attr->attr_handle,
                                       0,
                                       p_attr->inst_id,
                                       uuid,
                                       p_attr->property,
                                       FALSE);
            }
        }
        p_cur_srvc = p_cur_srvc->p_next;
    }

    if (i > 0) {
        bta_gattc_co_cache_save(p_srvc_cb->server_bda, BTA_GATTC_CI_CACHE_SAVE_EVT, i,
                                nv_attr, p_srvc_cb->attr_index, conn_id);

        p_srvc_cb->attr_index += i;

        return TRUE;
    } else {
        return FALSE;
    }
}
#endif /* GATTC_INCLUDED */

