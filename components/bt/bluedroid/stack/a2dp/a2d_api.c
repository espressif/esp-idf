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
 *  ommon API for the Advanced Audio Distribution Profile (A2DP)
 *
 ******************************************************************************/
#include <string.h>
#include "bt_target.h"
#include "sdpdefs.h"
#include "a2d_api.h"
#include "a2d_int.h"
#include "avdt_api.h"

/*****************************************************************************
**  Global data
*****************************************************************************/
#if A2D_DYNAMIC_MEMORY == FALSE
tA2D_CB a2d_cb;
#endif


/******************************************************************************
**
** Function         a2d_sdp_cback
**
** Description      This is the SDP callback function used by A2D_FindService.
**                  This function will be executed by SDP when the service
**                  search is completed.  If the search is successful, it
**                  finds the first record in the database that matches the
**                  UUID of the search.  Then retrieves various parameters
**                  from the record.  When it is finished it calls the
**                  application callback function.
**
** Returns          Nothing.
**
******************************************************************************/
static void a2d_sdp_cback(UINT16 status)
{
    tSDP_DISC_REC       *p_rec = NULL;
    tSDP_DISC_ATTR      *p_attr;
    BOOLEAN             found = FALSE;
    tA2D_Service        a2d_svc;
    tSDP_PROTOCOL_ELEM  elem;

    A2D_TRACE_API("a2d_sdp_cback status: %d", status);

    if (status == SDP_SUCCESS)
    {
        /* loop through all records we found */
        do
        {
            /* get next record; if none found, we're done */
            if ((p_rec = SDP_FindServiceInDb(a2d_cb.find.p_db,
                            a2d_cb.find.service_uuid, p_rec)) == NULL)
            {
                break;
            }
            memset(&a2d_svc, 0, sizeof(tA2D_Service));

            /* get service name */
            if ((p_attr = SDP_FindAttributeInRec(p_rec,
                            ATTR_ID_SERVICE_NAME)) != NULL)
            {
                a2d_svc.p_service_name = (char *) p_attr->attr_value.v.array;
                a2d_svc.service_len    = SDP_DISC_ATTR_LEN(p_attr->attr_len_type);
            }

            /* get provider name */
            if ((p_attr = SDP_FindAttributeInRec(p_rec,
                            ATTR_ID_PROVIDER_NAME)) != NULL)
            {
                a2d_svc.p_provider_name = (char *) p_attr->attr_value.v.array;
                a2d_svc.provider_len    = SDP_DISC_ATTR_LEN(p_attr->attr_len_type);
            }

            /* get supported features */
            if ((p_attr = SDP_FindAttributeInRec(p_rec,
                            ATTR_ID_SUPPORTED_FEATURES)) != NULL)
            {
                a2d_svc.features = p_attr->attr_value.v.u16;
            }

            /* get AVDTP version */
            if (SDP_FindProtocolListElemInRec(p_rec, UUID_PROTOCOL_AVDTP, &elem))
            {
                a2d_svc.avdt_version = elem.params[0];
                A2D_TRACE_DEBUG("avdt_version: 0x%x", a2d_svc.avdt_version);
            }

            /* we've got everything, we're done */
            found = TRUE;
            break;

        } while (TRUE);
    }

    a2d_cb.find.service_uuid = 0;
    /* return info from sdp record in app callback function */
    if (a2d_cb.find.p_cback != NULL)
    {
        (*a2d_cb.find.p_cback)(found, &a2d_svc);
    }

    return;
}

/*******************************************************************************
**
** Function         a2d_set_avdt_sdp_ver
**
** Description      This function allows the script wrapper to change the
**                  avdt version of a2dp.
**
** Returns          None
**
*******************************************************************************/
void a2d_set_avdt_sdp_ver (UINT16 avdt_sdp_ver)
{
    a2d_cb.avdt_sdp_ver = avdt_sdp_ver;
}

/******************************************************************************
**
** Function         A2D_AddRecord
**
** Description      This function is called by a server application to add
**                  SRC or SNK information to an SDP record.  Prior to
**                  calling this function the application must call
**                  SDP_CreateRecord() to create an SDP record.
**
**                  Input Parameters:
**                      service_uuid:  Indicates SRC or SNK.
**
**                      p_service_name:  Pointer to a null-terminated character
**                      string containing the service name.
**
**                      p_provider_name:  Pointer to a null-terminated character
**                      string containing the provider name.
**
**                      features:  Profile supported features.
**
**                      sdp_handle:  SDP handle returned by SDP_CreateRecord().
**
**                  Output Parameters:
**                      None.
**
** Returns          A2D_SUCCESS if function execution succeeded,
**                  A2D_INVALID_PARAMS if bad parameters are given.
**                  A2D_FAIL if function execution failed.
**
******************************************************************************/
tA2D_STATUS A2D_AddRecord(UINT16 service_uuid, char *p_service_name, char *p_provider_name,
        UINT16 features, UINT32 sdp_handle)
{
    UINT16      browse_list[1];
    BOOLEAN     result = TRUE;
    UINT8       temp[8];
    UINT8       *p;
    tSDP_PROTOCOL_ELEM  proto_list [A2D_NUM_PROTO_ELEMS];

    A2D_TRACE_API("A2D_AddRecord uuid: %x", service_uuid);

    if( (sdp_handle == 0) ||
        (service_uuid != UUID_SERVCLASS_AUDIO_SOURCE && service_uuid != UUID_SERVCLASS_AUDIO_SINK) )
        return A2D_INVALID_PARAMS;

    /* add service class id list */
    result &= SDP_AddServiceClassIdList(sdp_handle, 1, &service_uuid);

    memset((void*) proto_list, 0 , A2D_NUM_PROTO_ELEMS*sizeof(tSDP_PROTOCOL_ELEM));

    /* add protocol descriptor list   */
    proto_list[0].protocol_uuid = UUID_PROTOCOL_L2CAP;
    proto_list[0].num_params = 1;
    proto_list[0].params[0] = AVDT_PSM;
    proto_list[1].protocol_uuid = UUID_PROTOCOL_AVDTP;
    proto_list[1].num_params = 1;
    proto_list[1].params[0] = a2d_cb.avdt_sdp_ver;

    result &= SDP_AddProtocolList(sdp_handle, A2D_NUM_PROTO_ELEMS, proto_list);

    /* add profile descriptor list   */
    result &= SDP_AddProfileDescriptorList(sdp_handle, UUID_SERVCLASS_ADV_AUDIO_DISTRIBUTION, A2D_VERSION);

    /* add supported feature */
    if (features != 0)
    {
        p = temp;
        UINT16_TO_BE_STREAM(p, features);
        result &= SDP_AddAttribute(sdp_handle, ATTR_ID_SUPPORTED_FEATURES, UINT_DESC_TYPE,
                  (UINT32)2, (UINT8*)temp);
    }

    /* add provider name */
    if (p_provider_name != NULL)
    {
        result &= SDP_AddAttribute(sdp_handle, ATTR_ID_PROVIDER_NAME, TEXT_STR_DESC_TYPE,
                    (UINT32)(strlen(p_provider_name)+1), (UINT8 *) p_provider_name);
    }

    /* add service name */
    if (p_service_name != NULL)
    {
        result &= SDP_AddAttribute(sdp_handle, ATTR_ID_SERVICE_NAME, TEXT_STR_DESC_TYPE,
                    (UINT32)(strlen(p_service_name)+1), (UINT8 *) p_service_name);
    }

    /* add browse group list */
    browse_list[0] = UUID_SERVCLASS_PUBLIC_BROWSE_GROUP;
    result &= SDP_AddUuidSequence(sdp_handle, ATTR_ID_BROWSE_GROUP_LIST, 1, browse_list);


    return (result ? A2D_SUCCESS : A2D_FAIL);
}

/******************************************************************************
**
** Function         A2D_FindService
**
** Description      This function is called by a client application to
**                  perform service discovery and retrieve SRC or SNK SDP
**                  record information from a server.  Information is
**                  returned for the first service record found on the
**                  server that matches the service UUID.  The callback
**                  function will be executed when service discovery is
**                  complete.  There can only be one outstanding call to
**                  A2D_FindService() at a time; the application must wait
**                  for the callback before it makes another call to
**                  the function.
**
**                  Input Parameters:
**                      service_uuid:  Indicates SRC or SNK.
**
**                      bd_addr:  BD address of the peer device.
**
**                      p_db:  Pointer to the information to initialize
**                             the discovery database.
**
**                      p_cback:  Pointer to the A2D_FindService()
**                      callback function.
**
**                  Output Parameters:
**                      None.
**
** Returns          A2D_SUCCESS if function execution succeeded,
**                  A2D_INVALID_PARAMS if bad parameters are given.
**                  A2D_BUSY if discovery is already in progress.
**                  A2D_FAIL if function execution failed.
**
******************************************************************************/
tA2D_STATUS A2D_FindService(UINT16 service_uuid, BD_ADDR bd_addr,
                        tA2D_SDP_DB_PARAMS *p_db, tA2D_FIND_CBACK *p_cback)
{
    tSDP_UUID   uuid_list;
    BOOLEAN     result = TRUE;
    UINT16      a2d_attr_list[] = {ATTR_ID_SERVICE_CLASS_ID_LIST, /* update A2D_NUM_ATTR, if changed */
                                   ATTR_ID_BT_PROFILE_DESC_LIST,
                                   ATTR_ID_SUPPORTED_FEATURES,
                                   ATTR_ID_SERVICE_NAME,
                                   ATTR_ID_PROTOCOL_DESC_LIST,
                                   ATTR_ID_PROVIDER_NAME};

    A2D_TRACE_API("A2D_FindService uuid: %x", service_uuid);
    if( (service_uuid != UUID_SERVCLASS_AUDIO_SOURCE && service_uuid != UUID_SERVCLASS_AUDIO_SINK) ||
        p_db == NULL || p_db->p_db == NULL || p_cback == NULL)
        return A2D_INVALID_PARAMS;

    if( a2d_cb.find.service_uuid == UUID_SERVCLASS_AUDIO_SOURCE ||
        a2d_cb.find.service_uuid == UUID_SERVCLASS_AUDIO_SINK)
        return A2D_BUSY;

    /* set up discovery database */
    uuid_list.len = LEN_UUID_16;
    uuid_list.uu.uuid16 = service_uuid;

    if(p_db->p_attrs == NULL || p_db->num_attr == 0)
    {
        p_db->p_attrs  = a2d_attr_list;
        p_db->num_attr = A2D_NUM_ATTR;
    }

    result = SDP_InitDiscoveryDb(p_db->p_db, p_db->db_len, 1, &uuid_list, p_db->num_attr,
                                 p_db->p_attrs);

    if (result == TRUE)
    {
        /* store service_uuid and discovery db pointer */
        a2d_cb.find.p_db = p_db->p_db;
        a2d_cb.find.service_uuid = service_uuid;
        a2d_cb.find.p_cback = p_cback;

        /* perform service search */
        result = SDP_ServiceSearchAttributeRequest(bd_addr, p_db->p_db, a2d_sdp_cback);
        if(FALSE == result)
        {
            a2d_cb.find.service_uuid = 0;
        }
    }

    return (result ? A2D_SUCCESS : A2D_FAIL);
}

/******************************************************************************
**
** Function         A2D_SetTraceLevel
**
** Description      Sets the trace level for A2D. If 0xff is passed, the
**                  current trace level is returned.
**
**                  Input Parameters:
**                      new_level:  The level to set the A2D tracing to:
**                      0xff-returns the current setting.
**                      0-turns off tracing.
**                      >= 1-Errors.
**                      >= 2-Warnings.
**                      >= 3-APIs.
**                      >= 4-Events.
**                      >= 5-Debug.
**
** Returns          The new trace level or current trace level if
**                  the input parameter is 0xff.
**
******************************************************************************/
UINT8 A2D_SetTraceLevel (UINT8 new_level)
{
    if (new_level != 0xFF)
        a2d_cb.trace_level = new_level;

    return (a2d_cb.trace_level);
}

/******************************************************************************
** Function         A2D_BitsSet
**
** Description      Check the given num for the number of bits set
** Returns          A2D_SET_ONE_BIT, if one and only one bit is set
**                  A2D_SET_ZERO_BIT, if all bits clear
**                  A2D_SET_MULTL_BIT, if multiple bits are set
******************************************************************************/
UINT8 A2D_BitsSet(UINT8 num)
{
    UINT8   count;
    BOOLEAN res;
    if(num == 0)
        res = A2D_SET_ZERO_BIT;
    else
    {
        count = (num & (num - 1));
        res = ((count==0)?A2D_SET_ONE_BIT:A2D_SET_MULTL_BIT);
    }
    return res;
}

/*******************************************************************************
**
** Function         A2D_Init
**
** Description      This function is called to initialize the control block
**                  for this layer.  It must be called before accessing any
**                  other API functions for this layer.  It is typically called
**                  once during the start up of the stack.
**
** Returns          void
**
*******************************************************************************/
void A2D_Init(void)
{
    memset(&a2d_cb, 0, sizeof(tA2D_CB));

    a2d_cb.avdt_sdp_ver = AVDT_VERSION;

#if defined(A2D_INITIAL_TRACE_LEVEL)
    a2d_cb.trace_level  = A2D_INITIAL_TRACE_LEVEL;
#else
    a2d_cb.trace_level  = BT_TRACE_LEVEL_NONE;
#endif
}

