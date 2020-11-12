/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
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
 *  this file contains SDP interface functions
 *
 ******************************************************************************/

//#include <stdlib.h>
#include <string.h>
//#include <stdio.h>

#include "common/bt_target.h"
//#include "bt_utils.h"
#include "stack/l2cdefs.h"
#include "stack/hcidefs.h"
#include "stack/hcimsgs.h"
#include "common/bt_defs.h"
#include "stack/sdp_api.h"
#include "sdpint.h"
#include "stack/btu.h"

#if (SDP_INCLUDED == TRUE)
/**********************************************************************
**   C L I E N T    F U N C T I O N    P R O T O T Y P E S            *
***********************************************************************/

/*******************************************************************************
**
** Function         SDP_InitDiscoveryDb
**
** Description      This function is called to initialize a discovery database.
**
** Parameters:      p_db        - (input) address of an area of memory where the
**                                        discovery database is managed.
**                  len         - (input) size (in bytes) of the memory
**                                  NOTE: This must be larger than sizeof(tSDP_DISCOVERY_DB)
**                  num_uuid    - (input) number of UUID filters applied
**                  p_uuid_list - (input) list of UUID filters
**                  num_attr    - (input) number of attribute filters applied
**                  p_attr_list - (input) list of attribute filters
**
**
** Returns          BOOLEAN
**                          TRUE if successful
**                          FALSE if one or more parameters are bad
**
*******************************************************************************/
BOOLEAN SDP_InitDiscoveryDb (tSDP_DISCOVERY_DB *p_db, UINT32 len, UINT16 num_uuid,
                             tSDP_UUID *p_uuid_list, UINT16 num_attr, UINT16 *p_attr_list)
{
#if SDP_CLIENT_ENABLED == TRUE
    UINT16  xx;

    /* verify the parameters */
    if (p_db == NULL || (sizeof (tSDP_DISCOVERY_DB) > len) ||
            num_attr > SDP_MAX_ATTR_FILTERS || num_uuid > SDP_MAX_UUID_FILTERS) {
        SDP_TRACE_ERROR("SDP_InitDiscoveryDb Illegal param: p_db 0x%x, len %d, num_uuid %d, num_attr %d",
                        (UINT32)p_db, len, num_uuid, num_attr);

        return (FALSE);
    }

    memset (p_db, 0, (size_t)len);

    p_db->mem_size = len - sizeof (tSDP_DISCOVERY_DB);
    p_db->mem_free = p_db->mem_size;
    p_db->p_first_rec = NULL;
    p_db->p_free_mem = (UINT8 *)(p_db + 1);

    for (xx = 0; xx < num_uuid; xx++) {
        p_db->uuid_filters[xx] = *p_uuid_list++;
    }

    p_db->num_uuid_filters = num_uuid;

    for (xx = 0; xx < num_attr; xx++) {
        p_db->attr_filters[xx] = *p_attr_list++;
    }

    /* sort attributes */
    sdpu_sort_attr_list( num_attr, p_db );

    p_db->num_attr_filters = num_attr;
#endif
    return (TRUE);
}



/*******************************************************************************
**
** Function         SDP_CancelServiceSearch
**
** Description      This function cancels an active query to an SDP server.
**
** Returns          TRUE if discovery cancelled, FALSE if a matching activity is not found.
**
*******************************************************************************/
BOOLEAN SDP_CancelServiceSearch (tSDP_DISCOVERY_DB *p_db)
{
#if SDP_CLIENT_ENABLED == TRUE
    tCONN_CB     *p_ccb = sdpu_find_ccb_by_db (p_db);
    if (!p_ccb) {
        return (FALSE);
    }

    sdp_disconnect (p_ccb, SDP_CANCEL);
    p_ccb->disc_state = SDP_DISC_WAIT_CANCEL;
#endif
    return (TRUE);
}



/*******************************************************************************
**
** Function         SDP_ServiceSearchRequest
**
** Description      This function queries an SDP server for information.
**
** Returns          TRUE if discovery started, FALSE if failed.
**
*******************************************************************************/
BOOLEAN SDP_ServiceSearchRequest (UINT8 *p_bd_addr, tSDP_DISCOVERY_DB *p_db,
                                  tSDP_DISC_CMPL_CB *p_cb)
{
#if SDP_CLIENT_ENABLED == TRUE
    tCONN_CB     *p_ccb;

    /* Specific BD address */
    p_ccb = sdp_conn_originate (p_bd_addr);

    if (!p_ccb) {
        return (FALSE);
    }

    p_ccb->disc_state = SDP_DISC_WAIT_CONN;
    p_ccb->p_db       = p_db;
    p_ccb->p_cb       = p_cb;

    return (TRUE);
#else
    return (FALSE);
#endif
}


/*******************************************************************************
**
** Function         SDP_ServiceSearchAttributeRequest
**
** Description      This function queries an SDP server for information.
**
**                  The difference between this API function and the function
**                  SDP_ServiceSearchRequest2 is that this one does a
**                  combined ServiceSearchAttributeRequest SDP function.
**
** Returns          TRUE if discovery started, FALSE if failed.
**
*******************************************************************************/
BOOLEAN SDP_ServiceSearchAttributeRequest (UINT8 *p_bd_addr, tSDP_DISCOVERY_DB *p_db,
        tSDP_DISC_CMPL_CB *p_cb)
{
#if SDP_CLIENT_ENABLED == TRUE
    tCONN_CB     *p_ccb;

    /* Specific BD address */
    p_ccb = sdp_conn_originate (p_bd_addr);

    if (!p_ccb) {
        return (FALSE);
    }

    p_ccb->disc_state = SDP_DISC_WAIT_CONN;
    p_ccb->p_db       = p_db;
    p_ccb->p_cb       = p_cb;

    p_ccb->is_attr_search = TRUE;

    return (TRUE);
#else
    return (FALSE);
#endif
}
/*******************************************************************************
**
** Function         SDP_ServiceSearchAttributeRequest2
**
** Description      This function queries an SDP server for information.
**
**                  The difference between this API function and the function
**                  SDP_ServiceSearchRequest is that this one does a
**                  combined ServiceSearchAttributeRequest SDP function with the
**                  user data piggyback
**
** Returns          TRUE if discovery started, FALSE if failed.
**
*******************************************************************************/
BOOLEAN SDP_ServiceSearchAttributeRequest2 (UINT8 *p_bd_addr, tSDP_DISCOVERY_DB *p_db,
        tSDP_DISC_CMPL_CB2 *p_cb2, void *user_data)
{
#if SDP_CLIENT_ENABLED == TRUE
    tCONN_CB     *p_ccb;

    /* Specific BD address */
    p_ccb = sdp_conn_originate (p_bd_addr);

    if (!p_ccb) {
        return (FALSE);
    }

    p_ccb->disc_state = SDP_DISC_WAIT_CONN;
    p_ccb->p_db       = p_db;
    p_ccb->p_cb2       = p_cb2;

    p_ccb->is_attr_search = TRUE;
    p_ccb->user_data = user_data;

    return (TRUE);
#else
    return (FALSE);
#endif
}

#if SDP_CLIENT_ENABLED == TRUE
void SDP_SetIdleTimeout (BD_ADDR addr, UINT16 timeout)
{
    UNUSED(addr);
    UNUSED(timeout);
}
#endif

/*******************************************************************************
**
** Function         SDP_FindAttributeInDb
**
** Description      This function queries an SDP database for a specific attribute.
**                  If the p_start_rec pointer is NULL, it looks from the beginning
**                  of the database, else it continues from the next record after
**                  p_start_rec.
**
** Returns          Pointer to matching record, or NULL
**
*******************************************************************************/
tSDP_DISC_REC *SDP_FindAttributeInDb (tSDP_DISCOVERY_DB *p_db, UINT16 attr_id,
                                      tSDP_DISC_REC *p_start_rec)
{
#if SDP_CLIENT_ENABLED == TRUE
    tSDP_DISC_REC   *p_rec;
    tSDP_DISC_ATTR  *p_attr;

    /* Must have a valid database */
    if (p_db == NULL) {
        return (NULL);
    }

    if (!p_start_rec) {
        p_rec = p_db->p_first_rec;
    } else {
        p_rec = p_start_rec->p_next_rec;
    }

    while (p_rec) {
        p_attr = p_rec->p_first_attr;
        while (p_attr) {
            if (p_attr->attr_id == attr_id) {
                return (p_rec);
            }

            p_attr = p_attr->p_next_attr;
        }

        p_rec = p_rec->p_next_rec;
    }
#endif
    /* If here, no matching attribute found */
    return (NULL);
}


/*******************************************************************************
**
** Function         SDP_FindAttributeInRec
**
** Description      This function searches an SDP discovery record for a specific
**                  attribute.
**
** Returns          Pointer to matching attribute entry, or NULL
**
*******************************************************************************/
tSDP_DISC_ATTR *SDP_FindAttributeInRec (tSDP_DISC_REC *p_rec, UINT16 attr_id)
{
#if SDP_CLIENT_ENABLED == TRUE
    tSDP_DISC_ATTR  *p_attr;

    p_attr = p_rec->p_first_attr;
    while (p_attr) {
        if (p_attr->attr_id == attr_id) {
            return (p_attr);
        }

        p_attr = p_attr->p_next_attr;
    }
#endif
    /* If here, no matching attribute found */
    return (NULL);
}

/*******************************************************************************
**
** Function         SDP_FindServiceUUIDInRec
**
** Description      This function is called to read the service UUID within a record
**                  if there is any.
**
** Parameters:      p_rec      - pointer to a SDP record.
**                  p_uuid     - output parameter to save the UUID found.
**
** Returns          TRUE if found, otherwise FALSE.
**
*******************************************************************************/
BOOLEAN SDP_FindServiceUUIDInRec(tSDP_DISC_REC *p_rec, tBT_UUID *p_uuid)
{
#if SDP_CLIENT_ENABLED == TRUE
    tSDP_DISC_ATTR  *p_attr, *p_sattr, *p_extra_sattr;

    p_attr = p_rec->p_first_attr;

    while (p_attr) {
        if ((p_attr->attr_id == ATTR_ID_SERVICE_CLASS_ID_LIST)
                && (SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == DATA_ELE_SEQ_DESC_TYPE)) {
            for (p_sattr = p_attr->attr_value.v.p_sub_attr; p_sattr; p_sattr = p_sattr->p_next_attr) {
                if (SDP_DISC_ATTR_TYPE(p_sattr->attr_len_type) == UUID_DESC_TYPE) {
                    if (SDP_DISC_ATTR_LEN(p_sattr->attr_len_type) == LEN_UUID_16) {
                        p_uuid->len = LEN_UUID_16;
                        p_uuid->uu.uuid16 = p_sattr->attr_value.v.u16;
                    } else if (SDP_DISC_ATTR_LEN(p_sattr->attr_len_type) == LEN_UUID_128) {
                        p_uuid->len = LEN_UUID_128;
                        for (uint8_t i = 0; i != LEN_UUID_128; ++i) {
                            p_uuid->uu.uuid128[i] = p_sattr->attr_value.v.array[LEN_UUID_128 - i - 1];
                        }
                    } else if (SDP_DISC_ATTR_LEN(p_sattr->attr_len_type) == LEN_UUID_32) {
                        p_uuid->len = LEN_UUID_32;
                        p_uuid->uu.uuid32 = p_sattr->attr_value.v.u32;
                    }

                    return (TRUE);
                }

                /* Checking for Toyota G Block Car Kit:
                **  This car kit puts an extra data element sequence
                **  where the UUID is suppose to be!!!
                */
                else {
                    if (SDP_DISC_ATTR_TYPE(p_sattr->attr_len_type) == DATA_ELE_SEQ_DESC_TYPE) {
                        /* Look through data element sequence until no more UUIDs */
                        for (p_extra_sattr = p_sattr->attr_value.v.p_sub_attr; p_extra_sattr; p_extra_sattr = p_extra_sattr->p_next_attr) {
                            /* Increment past this to see if the next attribut is UUID */
                            if ((SDP_DISC_ATTR_TYPE(p_extra_sattr->attr_len_type) == UUID_DESC_TYPE)
                                    /* only support 16 bits UUID for now */
                                    && (SDP_DISC_ATTR_LEN(p_extra_sattr->attr_len_type) == 2)) {
                                p_uuid->len = 2;
                                p_uuid->uu.uuid16 = p_extra_sattr->attr_value.v.u16;
                                return (TRUE);
                            }
                        }
                    }
                }
            }
            break;
        } else if (p_attr->attr_id == ATTR_ID_SERVICE_ID) {
            if ((SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == UUID_DESC_TYPE)
                    /* only support 16 bits UUID for now */
                    && (SDP_DISC_ATTR_LEN(p_attr->attr_len_type) == 2)) {
                p_uuid->len = 2;
                p_uuid->uu.uuid16 = p_attr->attr_value.v.u16;
                return (TRUE);
            }
        }
        p_attr = p_attr->p_next_attr;
    }
    return FALSE;
#else
    return FALSE;
#endif
}

/*******************************************************************************
**
** Function         SDP_FindServiceUUIDInRec_128bit
**
** Description      This function is called to read the 128-bit service UUID within a record
**                  if there is any.
**
** Parameters:      p_rec      - pointer to a SDP record.
**                  p_uuid     - output parameter to save the UUID found.
**
** Returns          TRUE if found, otherwise FALSE.
**
*******************************************************************************/
BOOLEAN SDP_FindServiceUUIDInRec_128bit(tSDP_DISC_REC *p_rec, tBT_UUID *p_uuid)
{
#if SDP_CLIENT_ENABLED == TRUE
    tSDP_DISC_ATTR  *p_attr, *p_sattr;

    p_attr = p_rec->p_first_attr;

    while (p_attr) {
        if ((p_attr->attr_id == ATTR_ID_SERVICE_CLASS_ID_LIST)
                && (SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == DATA_ELE_SEQ_DESC_TYPE)) {
            for (p_sattr = p_attr->attr_value.v.p_sub_attr; p_sattr; p_sattr = p_sattr->p_next_attr) {
                if (SDP_DISC_ATTR_TYPE(p_sattr->attr_len_type) == UUID_DESC_TYPE) {
                    /* only support 128 bits UUID for now */
                    if (SDP_DISC_ATTR_LEN(p_sattr->attr_len_type) == 16) {
                        p_uuid->len = LEN_UUID_128;
                        for (uint8_t i = 0; i != LEN_UUID_128; ++i) {
                            p_uuid->uu.uuid128[i] = p_sattr->attr_value.v.array[LEN_UUID_128 - i - 1];
                        }
                    }
                    return (TRUE);
                }
            }
            break;
        } else if (p_attr->attr_id == ATTR_ID_SERVICE_ID) {
            if ((SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == UUID_DESC_TYPE)
                    /* only support 128 bits UUID for now */
                    && (SDP_DISC_ATTR_LEN(p_attr->attr_len_type) == 16)) {
                p_uuid->len = LEN_UUID_128;
                for (uint8_t i = 0; i != LEN_UUID_128; ++i) {
                    p_uuid->uu.uuid128[i] = p_attr->attr_value.v.array[LEN_UUID_128 - i - 1];
                }
                return (TRUE);
            }
        }
        p_attr = p_attr->p_next_attr;
    }
    return FALSE;
#else
    return FALSE;
#endif
}

/*******************************************************************************
**
** Function         SDP_FindServiceInDb
**
** Description      This function queries an SDP database for a specific service.
**                  If the p_start_rec pointer is NULL, it looks from the beginning
**                  of the database, else it continues from the next record after
**                  p_start_rec.
**
** Returns          Pointer to record containing service class, or NULL
**
*******************************************************************************/
tSDP_DISC_REC *SDP_FindServiceInDb (tSDP_DISCOVERY_DB *p_db, UINT16 service_uuid, tSDP_DISC_REC *p_start_rec)
{
#if SDP_CLIENT_ENABLED == TRUE
    tSDP_DISC_REC   *p_rec;
    tSDP_DISC_ATTR  *p_attr, *p_sattr, *p_extra_sattr;

    /* Must have a valid database */
    if (p_db == NULL) {
        return (NULL);
    }

    if (!p_start_rec) {
        p_rec = p_db->p_first_rec;
    } else {
        p_rec = p_start_rec->p_next_rec;
    }

    while (p_rec) {
        p_attr = p_rec->p_first_attr;
        while (p_attr) {
            if ((p_attr->attr_id == ATTR_ID_SERVICE_CLASS_ID_LIST)
                    && (SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == DATA_ELE_SEQ_DESC_TYPE)) {
                for (p_sattr = p_attr->attr_value.v.p_sub_attr; p_sattr; p_sattr = p_sattr->p_next_attr) {

                    if ((SDP_DISC_ATTR_TYPE(p_sattr->attr_len_type) == UUID_DESC_TYPE)
                            && (SDP_DISC_ATTR_LEN(p_sattr->attr_len_type) == 2) ) {
                        SDP_TRACE_DEBUG("SDP_FindServiceInDb - p_sattr value = 0x%x serviceuuid = 0x%x\r\n",
                                        p_sattr->attr_value.v.u16, service_uuid);
                        if (service_uuid == UUID_SERVCLASS_HDP_PROFILE) {
                            if ( (p_sattr->attr_value.v.u16 == UUID_SERVCLASS_HDP_SOURCE) || ( p_sattr->attr_value.v.u16 == UUID_SERVCLASS_HDP_SINK)) {
                                SDP_TRACE_DEBUG("SDP_FindServiceInDb found HDP source or sink\n" );
                                return (p_rec);
                            }
                        }

                    }

                    if (SDP_DISC_ATTR_TYPE(p_sattr->attr_len_type) == UUID_DESC_TYPE && (service_uuid == 0
                            || (SDP_DISC_ATTR_LEN(p_sattr->attr_len_type) == 2
                                && p_sattr->attr_value.v.u16 == service_uuid)))
                        /* for a specific uuid, or any one */
                    {
                        return (p_rec);
                    }

                    /* Checking for Toyota G Block Car Kit:
                    **  This car kit puts an extra data element sequence
                    **  where the UUID is suppose to be!!!
                    */
                    else {
                        if (SDP_DISC_ATTR_TYPE(p_sattr->attr_len_type) == DATA_ELE_SEQ_DESC_TYPE) {
                            /* Look through data element sequence until no more UUIDs */
                            for (p_extra_sattr = p_sattr->attr_value.v.p_sub_attr; p_extra_sattr; p_extra_sattr = p_extra_sattr->p_next_attr) {
                                /* Increment past this to see if the next attribut is UUID */
                                if ((SDP_DISC_ATTR_TYPE(p_extra_sattr->attr_len_type) == UUID_DESC_TYPE)
                                        && (SDP_DISC_ATTR_LEN(p_extra_sattr->attr_len_type) == 2)
                                        /* for a specific uuid, or any one */
                                        && ((p_extra_sattr->attr_value.v.u16 == service_uuid) || (service_uuid == 0))) {
                                    return (p_rec);
                                }
                            }
                        }
                    }
                }
                break;
            } else if (p_attr->attr_id == ATTR_ID_SERVICE_ID) {
                if ((SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == UUID_DESC_TYPE)
                        && (SDP_DISC_ATTR_LEN(p_attr->attr_len_type) == 2)
                        /* find a specific UUID or anyone */
                        && ((p_attr->attr_value.v.u16 == service_uuid) || service_uuid == 0)) {
                    return (p_rec);
                }
            }

            p_attr = p_attr->p_next_attr;
        }

        p_rec = p_rec->p_next_rec;
    }
#endif
    /* If here, no matching UUID found */
    return (NULL);
}

/*******************************************************************************
**
** Function         SDP_FindServiceInDb_128bit
**
** Description      This function queries an SDP database for a specific service.
**                  If the p_start_rec pointer is NULL, it looks from the beginning
**                  of the database, else it continues from the next record after
**                  p_start_rec.
**
**                  This function is kept separate from SDP_FindServiceInDb since
**                  that API is expected to return only 16-bit UUIDs
**
** Returns          Pointer to record containing service class, or NULL
**
*******************************************************************************/
tSDP_DISC_REC *SDP_FindServiceInDb_128bit(tSDP_DISCOVERY_DB *p_db, tSDP_DISC_REC *p_start_rec)
{
#if SDP_CLIENT_ENABLED == TRUE
    tSDP_DISC_REC   *p_rec;
    tSDP_DISC_ATTR  *p_attr, *p_sattr;

    /* Must have a valid database */
    if (p_db == NULL) {
        return (NULL);
    }

    if (!p_start_rec) {
        p_rec = p_db->p_first_rec;
    } else {
        p_rec = p_start_rec->p_next_rec;
    }

    while (p_rec) {
        p_attr = p_rec->p_first_attr;
        while (p_attr) {
            if ((p_attr->attr_id == ATTR_ID_SERVICE_CLASS_ID_LIST)
                    && (SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == DATA_ELE_SEQ_DESC_TYPE)) {
                for (p_sattr = p_attr->attr_value.v.p_sub_attr; p_sattr; p_sattr = p_sattr->p_next_attr) {
                    if ((SDP_DISC_ATTR_TYPE(p_sattr->attr_len_type) == UUID_DESC_TYPE)
                            && (SDP_DISC_ATTR_LEN(p_sattr->attr_len_type) == 16)) {
                        return (p_rec);
                    }
                }
                break;
            } else if (p_attr->attr_id == ATTR_ID_SERVICE_ID) {
                if ((SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == UUID_DESC_TYPE)
                        && (SDP_DISC_ATTR_LEN(p_attr->attr_len_type) == 16)) {
                    return (p_rec);
                }
            }

            p_attr = p_attr->p_next_attr;
        }

        p_rec = p_rec->p_next_rec;
    }
#endif
    /* If here, no matching UUID found */
    return (NULL);
}


/*******************************************************************************
**
** Function         SDP_FindServiceUUIDInDb
**
** Description      This function queries an SDP database for a specific service.
**                  If the p_start_rec pointer is NULL, it looks from the beginning
**                  of the database, else it continues from the next record after
**                  p_start_rec.
**
** NOTE             the only difference between this function and the previous function
**                  "SDP_FindServiceInDb()" is that this function takes a tBT_UUID input
**
** Returns          Pointer to record containing service class, or NULL
**
*******************************************************************************/
tSDP_DISC_REC *SDP_FindServiceUUIDInDb (tSDP_DISCOVERY_DB *p_db, tBT_UUID *p_uuid, tSDP_DISC_REC *p_start_rec)
{
#if SDP_CLIENT_ENABLED == TRUE
    tSDP_DISC_REC   *p_rec;
    tSDP_DISC_ATTR  *p_attr, *p_sattr;

    /* Must have a valid database */
    if (p_db == NULL) {
        return (NULL);
    }

    if (!p_start_rec) {
        p_rec = p_db->p_first_rec;
    } else {
        p_rec = p_start_rec->p_next_rec;
    }

    while (p_rec) {
        p_attr = p_rec->p_first_attr;
        while (p_attr) {
            if ((p_attr->attr_id == ATTR_ID_SERVICE_CLASS_ID_LIST)
                    && (SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == DATA_ELE_SEQ_DESC_TYPE)) {
                for (p_sattr = p_attr->attr_value.v.p_sub_attr; p_sattr; p_sattr = p_sattr->p_next_attr) {
                    if (SDP_DISC_ATTR_TYPE(p_sattr->attr_len_type) == UUID_DESC_TYPE) {
                        if (sdpu_compare_uuid_with_attr (p_uuid, p_sattr)) {
                            return (p_rec);
                        }
                    }
                }
                break;
            } else if (p_attr->attr_id == ATTR_ID_SERVICE_ID) {
                if (SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == UUID_DESC_TYPE ) {
                    if (sdpu_compare_uuid_with_attr (p_uuid, p_attr)) {
                        return (p_rec);
                    }
                }
            }

            p_attr = p_attr->p_next_attr;
        }

        p_rec = p_rec->p_next_rec;
    }
#endif  /* CLIENT_ENABLED == TRUE */
    /* If here, no matching UUID found */
    return (NULL);
}

#if SDP_CLIENT_ENABLED == TRUE
/*******************************************************************************
**
** Function         sdp_fill_proto_elem
**
** Description      This function retrieves the protocol element.
**
** Returns          TRUE if found, FALSE if not
**                  If found, the passed protocol list element is filled in.
**
*******************************************************************************/
static BOOLEAN sdp_fill_proto_elem( tSDP_DISC_ATTR  *p_attr, UINT16 layer_uuid,
                                    tSDP_PROTOCOL_ELEM *p_elem)
{
    tSDP_DISC_ATTR  *p_sattr;

    /* Walk through the protocol descriptor list */
    for (p_attr = p_attr->attr_value.v.p_sub_attr; p_attr; p_attr = p_attr->p_next_attr) {
        /* Safety check - each entry should itself be a sequence */
        if (SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) != DATA_ELE_SEQ_DESC_TYPE) {
            return (FALSE);
        }

        /* Now, see if the entry contains the layer we are interested in */
        for (p_sattr = p_attr->attr_value.v.p_sub_attr; p_sattr; p_sattr = p_sattr->p_next_attr) {
            /* SDP_TRACE_DEBUG ("SDP - p_sattr 0x%x, layer_uuid:0x%x, u16:0x%x####",
                p_sattr, layer_uuid, p_sattr->attr_value.v.u16); */

            if ((SDP_DISC_ATTR_TYPE(p_sattr->attr_len_type) == UUID_DESC_TYPE)
                    && (SDP_DISC_ATTR_LEN(p_sattr->attr_len_type) == 2)
                    && (p_sattr->attr_value.v.u16 == layer_uuid)) {
                /* Bingo. Now fill in the passed element */
                p_elem->protocol_uuid = layer_uuid;
                p_elem->num_params = 0;

                /* Store the parameters, if any */
                for (p_sattr = p_sattr->p_next_attr; p_sattr; p_sattr = p_sattr->p_next_attr) {
                    if (SDP_DISC_ATTR_TYPE(p_sattr->attr_len_type) != UINT_DESC_TYPE) {
                        break;
                    }

                    if (SDP_DISC_ATTR_LEN(p_sattr->attr_len_type) == 2) {
                        p_elem->params[p_elem->num_params++] = p_sattr->attr_value.v.u16;
                    } else {
                        p_elem->params[p_elem->num_params++] = p_sattr->attr_value.v.u8;
                    }

                    if (p_elem->num_params >= SDP_MAX_PROTOCOL_PARAMS) {
                        break;
                    }
                }
                return (TRUE);
            }
        }
    }

    return (FALSE);
}
#endif  /* CLIENT_ENABLED == TRUE */

/*******************************************************************************
**
** Function         SDP_FindProtocolListElemInRec
**
** Description      This function looks at a specific discovery record for a protocol
**                  list element.
**
** Returns          TRUE if found, FALSE if not
**                  If found, the passed protocol list element is filled in.
**
*******************************************************************************/
BOOLEAN SDP_FindProtocolListElemInRec (tSDP_DISC_REC *p_rec, UINT16 layer_uuid, tSDP_PROTOCOL_ELEM *p_elem)
{
#if SDP_CLIENT_ENABLED == TRUE
    tSDP_DISC_ATTR  *p_attr;

    p_attr = p_rec->p_first_attr;
    while (p_attr) {
        /* Find the protocol descriptor list */
        if ((p_attr->attr_id == ATTR_ID_PROTOCOL_DESC_LIST)
                && (SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == DATA_ELE_SEQ_DESC_TYPE)) {
            return sdp_fill_proto_elem(p_attr, layer_uuid, p_elem);
        }
        p_attr = p_attr->p_next_attr;
    }
#endif
    /* If here, no match found */
    return (FALSE);
}


/*******************************************************************************
**
** Function         SDP_FindAddProtoListsElemInRec
**
** Description      This function looks at a specific discovery record for a protocol
**                  list element.
**
** Returns          TRUE if found, FALSE if not
**                  If found, the passed protocol list element is filled in.
**
*******************************************************************************/
BOOLEAN SDP_FindAddProtoListsElemInRec (tSDP_DISC_REC *p_rec, UINT16 layer_uuid, tSDP_PROTOCOL_ELEM *p_elem)
{
#if SDP_CLIENT_ENABLED == TRUE
    tSDP_DISC_ATTR  *p_attr, *p_sattr;
    BOOLEAN         ret = FALSE;

    p_attr = p_rec->p_first_attr;
    while (p_attr) {
        /* Find the additional protocol descriptor list attribute */
        if ((p_attr->attr_id == ATTR_ID_ADDITION_PROTO_DESC_LISTS)
                && (SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == DATA_ELE_SEQ_DESC_TYPE)) {
            for (p_sattr = p_attr->attr_value.v.p_sub_attr; p_sattr; p_sattr = p_sattr->p_next_attr) {
                /* Safety check - each entry should itself be a sequence */
                if (SDP_DISC_ATTR_TYPE(p_sattr->attr_len_type) == DATA_ELE_SEQ_DESC_TYPE) {
                    if ( (ret = sdp_fill_proto_elem(p_sattr, layer_uuid, p_elem)) == TRUE) {
                        break;
                    }
                }
            }
            return ret;
        }
        p_attr = p_attr->p_next_attr;
    }
#endif
    /* If here, no match found */
    return (FALSE);
}


/*******************************************************************************
**
** Function         SDP_FindProfileVersionInRec
**
** Description      This function looks at a specific discovery record for the
**                  Profile list descriptor, and pulls out the version number.
**                  The version number consists of an 8-bit major version and
**                  an 8-bit minor version.
**
** Returns          TRUE if found, FALSE if not
**                  If found, the major and minor version numbers that were passed
**                  in are filled in.
**
*******************************************************************************/
BOOLEAN SDP_FindProfileVersionInRec (tSDP_DISC_REC *p_rec, UINT16 profile_uuid, UINT16 *p_version)
{
#if SDP_CLIENT_ENABLED == TRUE
    tSDP_DISC_ATTR  *p_attr, *p_sattr;

    p_attr = p_rec->p_first_attr;
    while (p_attr) {
        /* Find the profile descriptor list */
        if ((p_attr->attr_id == ATTR_ID_BT_PROFILE_DESC_LIST)
                && (SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) == DATA_ELE_SEQ_DESC_TYPE)) {
            /* Walk through the protocol descriptor list */
            for (p_attr = p_attr->attr_value.v.p_sub_attr; p_attr; p_attr = p_attr->p_next_attr) {
                /* Safety check - each entry should itself be a sequence */
                if (SDP_DISC_ATTR_TYPE(p_attr->attr_len_type) != DATA_ELE_SEQ_DESC_TYPE) {
                    return (FALSE);
                }

                /* Now, see if the entry contains the profile UUID we are interested in */
                for (p_sattr = p_attr->attr_value.v.p_sub_attr; p_sattr; p_sattr = p_sattr->p_next_attr) {
                    if ((SDP_DISC_ATTR_TYPE(p_sattr->attr_len_type) == UUID_DESC_TYPE)
                            && (SDP_DISC_ATTR_LEN(p_sattr->attr_len_type) == 2)  /* <- This is bytes, not size code! */
                            && (p_sattr->attr_value.v.u16 == profile_uuid)) {
                        /* Now fill in the major and minor numbers */
                        /* if the attribute matches the description for version (type UINT, size 2 bytes) */
                        p_sattr = p_sattr->p_next_attr;

                        if ((SDP_DISC_ATTR_TYPE(p_sattr->attr_len_type) == UINT_DESC_TYPE) &&
                                (SDP_DISC_ATTR_LEN(p_sattr->attr_len_type) == 2)) {
                            /* The high order 8 bits is the major number, low order is the minor number (big endian) */
                            *p_version = p_sattr->attr_value.v.u16;

                            return (TRUE);
                        } else {
                            return (FALSE);    /* The type and/or size was not valid for the profile list version */
                        }
                    }
                }
            }

            return (FALSE);
        }
        p_attr = p_attr->p_next_attr;
    }
#endif  /* CLIENT_ENABLED == TRUE */

    /* If here, no match found */
    return (FALSE);
}

/*******************************************************************************
**                   Device Identification (DI) Client Functions
*******************************************************************************/

/*******************************************************************************
**
** Function         SDP_DiDiscover
**
** Description      This function queries a remote device for DI information.
**
** Returns          SDP_SUCCESS if query started successfully, else error
**
*******************************************************************************/
UINT16 SDP_DiDiscover( BD_ADDR remote_device, tSDP_DISCOVERY_DB *p_db,
                       UINT32 len, tSDP_DISC_CMPL_CB *p_cb )
{
#if SDP_CLIENT_ENABLED == TRUE
    UINT16  result   = SDP_DI_DISC_FAILED;
    UINT16  num_uuids = 1;
    UINT16  di_uuid   = UUID_SERVCLASS_PNP_INFORMATION;

    /* build uuid for db init */
    tSDP_UUID init_uuid;
    init_uuid.len = 2;
    init_uuid.uu.uuid16 = di_uuid;

    if ( SDP_InitDiscoveryDb(p_db, len, num_uuids, &init_uuid, 0, NULL) ) {
        if ( SDP_ServiceSearchRequest(remote_device, p_db, p_cb) ) {
            result = SDP_SUCCESS;
        }
    }
    return result;
#else
    return SDP_DI_DISC_FAILED;
#endif
}

/*******************************************************************************
**
** Function         SDP_GetNumDiRecords
**
** Description      Searches specified database for DI records
**
** Returns          number of DI records found
**
*******************************************************************************/
UINT8 SDP_GetNumDiRecords( tSDP_DISCOVERY_DB *p_db )
{
#if SDP_CLIENT_ENABLED == TRUE
    UINT8   num_records = 0;
    tSDP_DISC_REC *p_curr_record = NULL;

    do {
        p_curr_record = SDP_FindServiceInDb( p_db, UUID_SERVCLASS_PNP_INFORMATION,
                                             p_curr_record );
        if ( p_curr_record ) {
            num_records++;
        }
    } while ( p_curr_record );

    return num_records;
#else
    return 0;
#endif
}

#if SDP_CLIENT_ENABLED == TRUE
/*******************************************************************************
**
** Function         SDP_AttrStringCopy
**
** Description      This function copy given attribute to specified buffer as a string
**
** Returns          none
**
*******************************************************************************/
static void SDP_AttrStringCopy(char *dst, tSDP_DISC_ATTR *p_attr, UINT16 dst_size)
{
    if ( dst == NULL ) {
        return;
    }
    if ( p_attr ) {
        UINT16 len = SDP_DISC_ATTR_LEN(p_attr->attr_len_type);
        if ( len > dst_size - 1 ) {
            len = dst_size - 1;
        }
        memcpy(dst, (char *)p_attr->attr_value.v.array, len);
        dst[len] = '\0';
    } else {
        dst[0] = '\0';
    }
}
#endif

/*******************************************************************************
**
** Function         SDP_GetDiRecord
**
** Description      This function retrieves a remote device's DI record from
**                  the specified database.
**
** Returns          SDP_SUCCESS if record retrieved, else error
**
*******************************************************************************/
UINT16 SDP_GetDiRecord( UINT8 get_record_index, tSDP_DI_GET_RECORD *p_device_info,
                        tSDP_DISCOVERY_DB *p_db )
{
#if SDP_CLIENT_ENABLED == TRUE
    UINT16  result = SDP_NO_DI_RECORD_FOUND;
    UINT8  curr_record_index = 1;

    tSDP_DISC_REC *p_curr_record = NULL;

    /* find the requested SDP record in the discovery database */
    do {
        p_curr_record = SDP_FindServiceInDb( p_db, UUID_SERVCLASS_PNP_INFORMATION,
                                             p_curr_record );
        if ( p_curr_record ) {
            if ( curr_record_index++ == get_record_index ) {
                result = SDP_SUCCESS;
                break;
            }
        }
    } while ( p_curr_record );

    if ( result == SDP_SUCCESS ) {
        /* copy the information from the SDP record to the DI record */
        tSDP_DISC_ATTR *p_curr_attr = NULL;

        /* ClientExecutableURL is optional */
        p_curr_attr = SDP_FindAttributeInRec( p_curr_record, ATTR_ID_CLIENT_EXE_URL );
        SDP_AttrStringCopy( p_device_info->rec.client_executable_url, p_curr_attr,
                            SDP_MAX_ATTR_LEN );

        /* Service Description is optional */
        p_curr_attr = SDP_FindAttributeInRec( p_curr_record, ATTR_ID_SERVICE_DESCRIPTION );
        SDP_AttrStringCopy( p_device_info->rec.service_description, p_curr_attr, SDP_MAX_ATTR_LEN );

        /* DocumentationURL is optional */
        p_curr_attr = SDP_FindAttributeInRec( p_curr_record, ATTR_ID_DOCUMENTATION_URL );
        SDP_AttrStringCopy( p_device_info->rec.documentation_url, p_curr_attr, SDP_MAX_ATTR_LEN );

        p_curr_attr = SDP_FindAttributeInRec( p_curr_record, ATTR_ID_SPECIFICATION_ID );
        if ( p_curr_attr ) {
            p_device_info->spec_id = p_curr_attr->attr_value.v.u16;
        } else {
            result = SDP_ERR_ATTR_NOT_PRESENT;
        }

        p_curr_attr = SDP_FindAttributeInRec( p_curr_record, ATTR_ID_VENDOR_ID );
        if ( p_curr_attr ) {
            p_device_info->rec.vendor = p_curr_attr->attr_value.v.u16;
        } else {
            result = SDP_ERR_ATTR_NOT_PRESENT;
        }

        p_curr_attr = SDP_FindAttributeInRec( p_curr_record, ATTR_ID_VENDOR_ID_SOURCE );
        if ( p_curr_attr ) {
            p_device_info->rec.vendor_id_source = p_curr_attr->attr_value.v.u16;
        } else {
            result = SDP_ERR_ATTR_NOT_PRESENT;
        }

        p_curr_attr = SDP_FindAttributeInRec( p_curr_record, ATTR_ID_PRODUCT_ID );
        if ( p_curr_attr ) {
            p_device_info->rec.product = p_curr_attr->attr_value.v.u16;
        } else {
            result = SDP_ERR_ATTR_NOT_PRESENT;
        }

        p_curr_attr = SDP_FindAttributeInRec( p_curr_record, ATTR_ID_PRODUCT_VERSION );
        if ( p_curr_attr ) {
            p_device_info->rec.version = p_curr_attr->attr_value.v.u16;
        } else {
            result = SDP_ERR_ATTR_NOT_PRESENT;
        }

        p_curr_attr = SDP_FindAttributeInRec( p_curr_record, ATTR_ID_PRIMARY_RECORD );
        if ( p_curr_attr ) {
            p_device_info->rec.primary_record = (BOOLEAN)p_curr_attr->attr_value.v.u8;
        } else {
            result = SDP_ERR_ATTR_NOT_PRESENT;
        }
    }

    return result;
#else   /* SDP_CLIENT_ENABLED is FALSE */
    return SDP_NO_DI_RECORD_FOUND;
#endif
}

/*******************************************************************************
**                   Device Identification (DI) Server Functions
*******************************************************************************/

/*******************************************************************************
**
** Function         SDP_SetLocalDiRecord
**
** Description      This function adds a DI record to the local SDP database.
**
**
**
** Returns          Returns SDP_SUCCESS if record added successfully, else error
**
*******************************************************************************/
UINT16 SDP_SetLocalDiRecord( tSDP_DI_RECORD *p_device_info, UINT32 *p_handle )
{
#if SDP_SERVER_ENABLED == TRUE
    UINT16  result = SDP_SUCCESS;
    UINT32  handle;
    UINT16  di_uuid = UUID_SERVCLASS_PNP_INFORMATION;
    UINT16  di_specid = BLUETOOTH_DI_SPECIFICATION;
    UINT8   temp_u16[2];
    UINT8   *p_temp;
    UINT8   u8;

    *p_handle = 0;
    if ( p_device_info == NULL ) {
        return SDP_ILLEGAL_PARAMETER;
    }

    /* if record is to be primary record, get handle to replace old primary */
    if ( p_device_info->primary_record == TRUE && sdp_cb.server_db.di_primary_handle ) {
        handle = sdp_cb.server_db.di_primary_handle;
    } else {
        if ( (handle = SDP_CreateRecord()) == 0 ) {
            return SDP_NO_RESOURCES;
        }
    }

    *p_handle = handle;

    /* build the SDP entry */
    /* Add the UUID to the Service Class ID List */
    if ((SDP_AddServiceClassIdList(handle, 1, &di_uuid)) == FALSE) {
        result = SDP_DI_REG_FAILED;
    }

    /* mandatory */
    if ( result == SDP_SUCCESS) {
        p_temp = temp_u16;
        UINT16_TO_BE_STREAM(p_temp, di_specid);
        if ( !(SDP_AddAttribute(handle, ATTR_ID_SPECIFICATION_ID,
                                UINT_DESC_TYPE, sizeof(di_specid),
                                temp_u16)) ) {
            result = SDP_DI_REG_FAILED;
        }
    }

    /* optional - if string is null, do not add attribute */
    if ( result == SDP_SUCCESS ) {
        if ( p_device_info->client_executable_url[0] != '\0' ) {
            if ( !((strlen(p_device_info->client_executable_url) + 1 <= SDP_MAX_ATTR_LEN) &&
                    SDP_AddAttribute(handle, ATTR_ID_CLIENT_EXE_URL, URL_DESC_TYPE,
                                     (UINT32)(strlen(p_device_info->client_executable_url) + 1),
                                     (UINT8 *)p_device_info->client_executable_url)) ) {
                result = SDP_DI_REG_FAILED;
            }
        }
    }

    /* optional - if string is null, do not add attribute */
    if ( result == SDP_SUCCESS ) {
        if ( p_device_info->service_description[0] != '\0' ) {
            if ( !((strlen(p_device_info->service_description) + 1 <= SDP_MAX_ATTR_LEN) &&
                    SDP_AddAttribute(handle, ATTR_ID_SERVICE_DESCRIPTION,
                                     TEXT_STR_DESC_TYPE,
                                     (UINT32)(strlen(p_device_info->service_description) + 1),
                                     (UINT8 *)p_device_info->service_description)) ) {
                result = SDP_DI_REG_FAILED;
            }
        }
    }

    /* optional - if string is null, do not add attribute */
    if ( result == SDP_SUCCESS ) {
        if ( p_device_info->documentation_url[0] != '\0' ) {
            if ( !((strlen(p_device_info->documentation_url) + 1 <= SDP_MAX_ATTR_LEN) &&
                    SDP_AddAttribute(handle, ATTR_ID_DOCUMENTATION_URL, URL_DESC_TYPE,
                                     (UINT32)(strlen(p_device_info->documentation_url) + 1),
                                     (UINT8 *)p_device_info->documentation_url)) ) {
                result = SDP_DI_REG_FAILED;
            }
        }
    }

    /* mandatory */
    if ( result == SDP_SUCCESS) {
        p_temp = temp_u16;
        UINT16_TO_BE_STREAM(p_temp, p_device_info->vendor);
        if ( !(SDP_AddAttribute(handle, ATTR_ID_VENDOR_ID, UINT_DESC_TYPE,
                                sizeof(p_device_info->vendor), temp_u16)) ) {
            result = SDP_DI_REG_FAILED;
        }
    }

    /* mandatory */
    if ( result == SDP_SUCCESS) {
        p_temp = temp_u16;
        UINT16_TO_BE_STREAM (p_temp, p_device_info->product);
        if ( !(SDP_AddAttribute(handle, ATTR_ID_PRODUCT_ID,
                                UINT_DESC_TYPE, sizeof(p_device_info->product), temp_u16)) ) {
            result = SDP_DI_REG_FAILED;
        }
    }

    /* mandatory */
    if ( result == SDP_SUCCESS) {
        p_temp = temp_u16;
        UINT16_TO_BE_STREAM (p_temp, p_device_info->version);
        if ( !(SDP_AddAttribute(handle, ATTR_ID_PRODUCT_VERSION, UINT_DESC_TYPE,
                                sizeof(p_device_info->version), temp_u16)) ) {
            result = SDP_DI_REG_FAILED;
        }
    }

    /* mandatory */
    if ( result == SDP_SUCCESS) {
        u8 = (UINT8)p_device_info->primary_record;
        if ( !(SDP_AddAttribute(handle, ATTR_ID_PRIMARY_RECORD,
                                BOOLEAN_DESC_TYPE, 1, &u8)) ) {
            result = SDP_DI_REG_FAILED;
        }
    }

    /* mandatory */
    if ( result == SDP_SUCCESS) {
        p_temp = temp_u16;
        UINT16_TO_BE_STREAM(p_temp, p_device_info->vendor_id_source);
        if ( !(SDP_AddAttribute(handle, ATTR_ID_VENDOR_ID_SOURCE, UINT_DESC_TYPE,
                                sizeof(p_device_info->vendor_id_source), temp_u16)) ) {
            result = SDP_DI_REG_FAILED;
        }
    }

    if ( result != SDP_SUCCESS ) {
        SDP_DeleteRecord( handle );
    } else if (p_device_info->primary_record == TRUE) {
        sdp_cb.server_db.di_primary_handle = handle;
    }

    return result;
#else   /* SDP_SERVER_ENABLED is FALSE */
    return SDP_DI_REG_FAILED;
#endif  /* if SDP_SERVER_ENABLED */
}

/*******************************************************************************
**
** Function         SDP_SetTraceLevel
**
** Description      This function sets the trace level for SDP. If called with
**                  a value of 0xFF, it simply reads the current trace level.
**
** Returns          the new (current) trace level
**
*******************************************************************************/
UINT8 SDP_SetTraceLevel (UINT8 new_level)
{
    if (new_level != 0xFF) {
        sdp_cb.trace_level = new_level;
    }

    return (sdp_cb.trace_level);
}

#endif  ///SDP_INCLUDED == TRUE
