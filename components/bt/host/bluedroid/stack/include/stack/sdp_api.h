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
#ifndef SDP_API_H
#define SDP_API_H

#include "common/bt_target.h"
#include "stack/sdpdefs.h"
#if (SDP_INCLUDED == TRUE)
/*****************************************************************************
**  Constants
*****************************************************************************/

/* Success code and error codes */
#define  SDP_SUCCESS                        0x0000
#define  SDP_INVALID_VERSION                0x0001
#define  SDP_INVALID_SERV_REC_HDL           0x0002
#define  SDP_INVALID_REQ_SYNTAX             0x0003
#define  SDP_INVALID_PDU_SIZE               0x0004
#define  SDP_INVALID_CONT_STATE             0x0005
#define  SDP_NO_RESOURCES                   0x0006
#define  SDP_DI_REG_FAILED                  0x0007
#define  SDP_DI_DISC_FAILED                 0x0008
#define  SDP_NO_DI_RECORD_FOUND             0x0009
#define  SDP_ERR_ATTR_NOT_PRESENT           0x000A
#define  SDP_ILLEGAL_PARAMETER              0x000B

#define  SDP_NO_RECS_MATCH                  0xFFF0
#define  SDP_CONN_FAILED                    0xFFF1
#define  SDP_CFG_FAILED                     0xFFF2
#define  SDP_GENERIC_ERROR                  0xFFF3
#define  SDP_DB_FULL                        0xFFF4
#define  SDP_INVALID_PDU                    0xFFF5
#define  SDP_SECURITY_ERR                   0xFFF6
#define  SDP_CONN_REJECTED                  0xFFF7
#define  SDP_CANCEL                         0xFFF8

/* Define the PSM that SDP uses */
#define SDP_PSM     0x0001

/* Legacy #define to avoid code changes - SDP UUID is same as BT UUID */
#define tSDP_UUID   tBT_UUID

/* Masks for attr_value field of tSDP_DISC_ATTR */
#define SDP_DISC_ATTR_LEN_MASK          0x0FFF
#define SDP_DISC_ATTR_TYPE(len_type)    (len_type >> 12)
#define SDP_DISC_ATTR_LEN(len_type)     (len_type & SDP_DISC_ATTR_LEN_MASK)

/* Maximum number of protocol list items (list_elem in tSDP_PROTOCOL_ELEM) */
#define SDP_MAX_LIST_ELEMS      3


/*****************************************************************************
**  Type Definitions
*****************************************************************************/

/* Define a callback function for when discovery is complete. */
typedef void (tSDP_DISC_CMPL_CB) (UINT16 result);
typedef void (tSDP_DISC_CMPL_CB2) (UINT16 result, void *user_data);

typedef struct {
    BD_ADDR         peer_addr;
    UINT16          peer_mtu;
} tSDP_DR_OPEN;

typedef struct {
    UINT8           *p_data;
    UINT16          data_len;
} tSDP_DR_DATA;

typedef union {
    tSDP_DR_OPEN    open;
    tSDP_DR_DATA    data;
} tSDP_DATA;

/* Define a callback function for when discovery result is received. */
typedef void (tSDP_DISC_RES_CB) (UINT16 event, tSDP_DATA *p_data);

/* Define a structure to hold the discovered service information. */
typedef struct {
    union {
        UINT8       u8;                         /* 8-bit integer            */
        UINT16      u16;                        /* 16-bit integer           */
        UINT32      u32;                        /* 32-bit integer           */
        UINT8       array[4];                   /* Variable length field    */
        struct t_sdp_disc_attr *p_sub_attr;     /* Addr of first sub-attr (list)*/
    } v;

} tSDP_DISC_ATVAL;

typedef struct t_sdp_disc_attr {
    struct t_sdp_disc_attr *p_next_attr;        /* Addr of next linked attr     */
    UINT16                  attr_id;            /* Attribute ID                 */
    UINT16                  attr_len_type;      /* Length and type fields       */
    tSDP_DISC_ATVAL         attr_value;         /* Variable length entry data   */
} tSDP_DISC_ATTR;

typedef struct t_sdp_disc_rec {
    tSDP_DISC_ATTR          *p_first_attr;      /* First attribute of record    */
    struct t_sdp_disc_rec   *p_next_rec;        /* Addr of next linked record   */
    UINT32                  time_read;          /* The time the record was read */
    BD_ADDR                 remote_bd_addr;     /* Remote BD address            */
} tSDP_DISC_REC;

typedef struct {
    UINT32          mem_size;                   /* Memory size of the DB        */
    UINT32          mem_free;                   /* Memory still available       */
    tSDP_DISC_REC   *p_first_rec;               /* Addr of first record in DB   */
    UINT16          num_uuid_filters;           /* Number of UUIds to filter    */
    tSDP_UUID       uuid_filters[SDP_MAX_UUID_FILTERS]; /* UUIDs to filter      */
    UINT16          num_attr_filters;           /* Number of attribute filters  */
    UINT16          attr_filters[SDP_MAX_ATTR_FILTERS]; /* Attributes to filter */
    UINT8           *p_free_mem;                /* Pointer to free memory       */
#if (SDP_RAW_DATA_INCLUDED == TRUE)
    UINT8           *raw_data;                  /* Received record from server. allocated/released by client  */
    UINT32          raw_size;                   /* size of raw_data */
    UINT32          raw_used;                   /* length of raw_data used */
#endif
} tSDP_DISCOVERY_DB;

/* This structure is used to add protocol lists and find protocol elements */
typedef struct {
    UINT16      protocol_uuid;
    UINT16      num_params;
    UINT16      params[SDP_MAX_PROTOCOL_PARAMS];
} tSDP_PROTOCOL_ELEM;

typedef struct {
    UINT16              num_elems;
    tSDP_PROTOCOL_ELEM  list_elem[SDP_MAX_LIST_ELEMS];
} tSDP_PROTO_LIST_ELEM;

/* Device Identification (DI) data structure
*/
/* Used to set the DI record */
typedef struct t_sdp_di_record {
    UINT16       vendor;
    UINT16       vendor_id_source;
    UINT16       product;
    UINT16       version;
    BOOLEAN      primary_record;
    char         client_executable_url[SDP_MAX_ATTR_LEN];   /* optional */
    char         service_description[SDP_MAX_ATTR_LEN];     /* optional */
    char         documentation_url[SDP_MAX_ATTR_LEN];       /* optional */
} tSDP_DI_RECORD;

/* Used to get the DI record */
typedef struct t_sdp_di_get_record {
    UINT16          spec_id;
    tSDP_DI_RECORD  rec;
} tSDP_DI_GET_RECORD;


/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/* API into the SDP layer for service discovery. */

/*******************************************************************************
**
** Function         SDP_InitDiscoveryDb
**
** Description      This function is called to initialize a discovery database.
**
** Returns          TRUE if successful, FALSE if one or more parameters are bad
**
*******************************************************************************/
extern BOOLEAN SDP_InitDiscoveryDb (tSDP_DISCOVERY_DB *p_db, UINT32 len,
                                    UINT16 num_uuid,
                                    tSDP_UUID *p_uuid_list,
                                    UINT16 num_attr,
                                    UINT16 *p_attr_list);

/*******************************************************************************
**
** Function         SDP_CancelServiceSearch
**
** Description      This function cancels an active query to an SDP server.
**
** Returns          TRUE if discovery cancelled, FALSE if a matching activity is not found.
**
*******************************************************************************/
extern BOOLEAN SDP_CancelServiceSearch (tSDP_DISCOVERY_DB *p_db);

/*******************************************************************************
**
** Function         SDP_ServiceSearchRequest
**
** Description      This function queries an SDP server for information.
**
** Returns          TRUE if discovery started, FALSE if failed.
**
*******************************************************************************/
extern BOOLEAN SDP_ServiceSearchRequest (UINT8 *p_bd_addr,
        tSDP_DISCOVERY_DB *p_db,
        tSDP_DISC_CMPL_CB *p_cb);


/*******************************************************************************
**
** Function         SDP_ServiceSearchAttributeRequest
**
** Description      This function queries an SDP server for information.
**
**                  The difference between this API function and the function
**                  SDP_ServiceSearchRequest is that this one does a
**                  combined ServiceSearchAttributeRequest SDP function.
**
** Returns          TRUE if discovery started, FALSE if failed.
**
*******************************************************************************/
extern BOOLEAN SDP_ServiceSearchAttributeRequest (UINT8 *p_bd_addr,
        tSDP_DISCOVERY_DB *p_db,
        tSDP_DISC_CMPL_CB *p_cb);

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
extern BOOLEAN SDP_ServiceSearchAttributeRequest2 (UINT8 *p_bd_addr,
        tSDP_DISCOVERY_DB *p_db,
        tSDP_DISC_CMPL_CB2 *p_cb, void *user_data);

/* API of utilities to find data in the local discovery database */

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
extern tSDP_DISC_REC *SDP_FindAttributeInDb (tSDP_DISCOVERY_DB *p_db,
        UINT16 attr_id,
        tSDP_DISC_REC *p_start_rec);


/*******************************************************************************
**
** Function         SDP_FindAttributeInRec
**
** Description      This function searches an SDP discovery record for a
**                  specific attribute.
**
** Returns          Pointer to matching attribute entry, or NULL
**
*******************************************************************************/
extern tSDP_DISC_ATTR *SDP_FindAttributeInRec (tSDP_DISC_REC *p_rec,
        UINT16 attr_id);


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
extern tSDP_DISC_REC *SDP_FindServiceInDb (tSDP_DISCOVERY_DB *p_db,
        UINT16 service_uuid,
        tSDP_DISC_REC *p_start_rec);


/*******************************************************************************
**
** Function         SDP_FindServiceUUIDInDb
**
** Description      This function queries an SDP database for a specific service.
**                  If the p_start_rec pointer is NULL, it looks from the beginning
**                  of the database, else it continues from the next record after
**                  p_start_rec.
**
** NOTE             the only difference between this function and the previous
**                  function "SDP_FindServiceInDb()" is that this function takes
**                  a tBT_UUID input.
**
** Returns          Pointer to record containing service class, or NULL
**
*******************************************************************************/
extern tSDP_DISC_REC *SDP_FindServiceUUIDInDb (tSDP_DISCOVERY_DB *p_db,
        tBT_UUID *p_uuid,
        tSDP_DISC_REC *p_start_rec);

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
extern BOOLEAN SDP_FindServiceUUIDInRec_128bit(tSDP_DISC_REC *p_rec, tBT_UUID *p_uuid);

/*******************************************************************************
**
** Function         SDP_FindServiceInDb_128bit
**
** Description      This function queries an SDP database for a specific service.
**                  If the p_start_rec pointer is NULL, it looks from the beginning
**                  of the database, else it continues from the next record after
**                  p_start_rec.
**
** Returns          Pointer to record containing service class, or NULL
**
*******************************************************************************/
extern tSDP_DISC_REC *SDP_FindServiceInDb_128bit(tSDP_DISCOVERY_DB *p_db,
        tSDP_DISC_REC *p_start_rec);

/*******************************************************************************
**
** Function         SDP_FindProtocolListElemInRec
**
** Description      This function looks at a specific discovery record for a
**                  protocol list element.
**
** Returns          TRUE if found, FALSE if not
**                  If found, the passed protocol list element is filled in.
**
*******************************************************************************/
extern BOOLEAN SDP_FindProtocolListElemInRec (tSDP_DISC_REC *p_rec,
        UINT16 layer_uuid,
        tSDP_PROTOCOL_ELEM *p_elem);


/*******************************************************************************
**
** Function         SDP_FindAddProtoListsElemInRec
**
** Description      This function looks at a specific discovery record for a
**                  protocol list element.
**
** Returns          TRUE if found, FALSE if not
**                  If found, the passed protocol list element is filled in.
**
*******************************************************************************/
extern BOOLEAN SDP_FindAddProtoListsElemInRec (tSDP_DISC_REC *p_rec,
        UINT16 layer_uuid,
        tSDP_PROTOCOL_ELEM *p_elem);


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
extern BOOLEAN SDP_FindProfileVersionInRec (tSDP_DISC_REC *p_rec,
        UINT16 profile_uuid,
        UINT16 *p_version);


/* API into SDP for local service database updates */

/*******************************************************************************
**
** Function         SDP_CreateRecord
**
** Description      This function is called to create a record in the database.
**                  This would be through the SDP database maintenance API. The
**                  record is created empty, teh application should then call
**                  "add_attribute" to add the record's attributes.
**
** Returns          Record handle if OK, else 0.
**
*******************************************************************************/
extern UINT32 SDP_CreateRecord (void);


/*******************************************************************************
**
** Function         SDP_DeleteRecord
**
** Description      This function is called to add a record (or all records)
**                  from the database. This would be through the SDP database
**                  maintenance API.
**
**                  If a record handle of 0 is passed, all records are deleted.
**
** Returns          TRUE if succeeded, else FALSE
**
*******************************************************************************/
extern BOOLEAN SDP_DeleteRecord (UINT32 handle);


/*******************************************************************************
**
** Function         SDP_ReadRecord
**
** Description      This function is called to get the raw data of the record
**                  with the given handle from the database.
**
** Returns          -1, if the record is not found.
**                  Otherwise, the offset (0 or 1) to start of data in p_data.
**
**                  The size of data copied into p_data is in *p_data_len.
**
*******************************************************************************/
extern INT32 SDP_ReadRecord(UINT32 handle, UINT8 *p_data, INT32 *p_data_len);

/*******************************************************************************
**
** Function         SDP_AddAttribute
**
** Description      This function is called to add an attribute to a record.
**                  This would be through the SDP database maintenance API.
**                  If the attribute already exists in the record, it is replaced
**                  with the new value.
**
** NOTE             Attribute values must be passed as a Big Endian stream.
**
** Returns          TRUE if added OK, else FALSE
**
*******************************************************************************/
extern BOOLEAN SDP_AddAttribute (UINT32 handle, UINT16 attr_id,
                                 UINT8 attr_type, UINT32 attr_len,
                                 UINT8 *p_val);


/*******************************************************************************
**
** Function         SDP_AddSequence
**
** Description      This function is called to add a sequence to a record.
**                  This would be through the SDP database maintenance API.
**                  If the sequence already exists in the record, it is replaced
**                  with the new sequence.
**
** NOTE             Element values must be passed as a Big Endian stream.
**
** Returns          TRUE if added OK, else FALSE
**
*******************************************************************************/
extern BOOLEAN SDP_AddSequence (UINT32 handle,  UINT16 attr_id,
                                UINT16 num_elem, UINT8 type[],
                                UINT8 len[], UINT8 *p_val[]);


/*******************************************************************************
**
** Function         SDP_AddUuidSequence
**
** Description      This function is called to add a UUID sequence to a record.
**                  This would be through the SDP database maintenance API.
**                  If the sequence already exists in the record, it is replaced
**                  with the new sequence.
**
** Returns          TRUE if added OK, else FALSE
**
*******************************************************************************/
extern BOOLEAN SDP_AddUuidSequence (UINT32 handle,  UINT16 attr_id,
                                    UINT16 num_uuids, UINT16 *p_uuids);


/*******************************************************************************
**
** Function         SDP_AddProtocolList
**
** Description      This function is called to add a protocol descriptor list to
**                  a record. This would be through the SDP database maintenance API.
**                  If the protocol list already exists in the record, it is replaced
**                  with the new list.
**
** Returns          TRUE if added OK, else FALSE
**
*******************************************************************************/
extern BOOLEAN SDP_AddProtocolList (UINT32 handle, UINT16 num_elem,
                                    tSDP_PROTOCOL_ELEM *p_elem_list);


/*******************************************************************************
**
** Function         SDP_AddAdditionProtoLists
**
** Description      This function is called to add a protocol descriptor list to
**                  a record. This would be through the SDP database maintenance API.
**                  If the protocol list already exists in the record, it is replaced
**                  with the new list.
**
** Returns          TRUE if added OK, else FALSE
**
*******************************************************************************/
extern BOOLEAN SDP_AddAdditionProtoLists (UINT32 handle, UINT16 num_elem,
        tSDP_PROTO_LIST_ELEM *p_proto_list);


/*******************************************************************************
**
** Function         SDP_AddProfileDescriptorList
**
** Description      This function is called to add a profile descriptor list to
**                  a record. This would be through the SDP database maintenance API.
**                  If the version already exists in the record, it is replaced
**                  with the new one.
**
** Returns          TRUE if added OK, else FALSE
**
*******************************************************************************/
extern BOOLEAN SDP_AddProfileDescriptorList (UINT32 handle,
        UINT16 profile_uuid,
        UINT16 version);


/*******************************************************************************
**
** Function         SDP_AddLanguageBaseAttrIDList
**
** Description      This function is called to add a language base attr list to
**                  a record. This would be through the SDP database maintenance API.
**                  If the version already exists in the record, it is replaced
**                  with the new one.
**
** Returns          TRUE if added OK, else FALSE
**
*******************************************************************************/
extern BOOLEAN SDP_AddLanguageBaseAttrIDList (UINT32 handle,
        UINT16 lang, UINT16 char_enc,
        UINT16 base_id);


/*******************************************************************************
**
** Function         SDP_AddServiceClassIdList
**
** Description      This function is called to add a service list to a record.
**                  This would be through the SDP database maintenance API.
**                  If the service list already exists in the record, it is replaced
**                  with the new list.
**
** Returns          TRUE if added OK, else FALSE
**
*******************************************************************************/
extern BOOLEAN SDP_AddServiceClassIdList (UINT32 handle,
        UINT16 num_services,
        UINT16 *p_service_uuids);


/*******************************************************************************
**
** Function         SDP_DeleteAttribute
**
** Description      This function is called to delete an attribute from a record.
**                  This would be through the SDP database maintenance API.
**
** Returns          TRUE if deleted OK, else FALSE if not found
**
*******************************************************************************/
extern BOOLEAN SDP_DeleteAttribute (UINT32 handle, UINT16 attr_id);


/* Device Identification APIs */

/*******************************************************************************
**
** Function         SDP_SetLocalDiRecord
**
** Description      This function adds a DI record to the local SDP database.
**
** Returns          Returns SDP_SUCCESS if record added successfully, else error
**
*******************************************************************************/
extern UINT16 SDP_SetLocalDiRecord (tSDP_DI_RECORD *device_info,
                                    UINT32 *p_handle);

/*******************************************************************************
**
** Function         SDP_DiDiscover
**
** Description      This function queries a remote device for DI information.
**
** Returns          SDP_SUCCESS if query started successfully, else error
**
*******************************************************************************/
extern UINT16 SDP_DiDiscover (BD_ADDR remote_device,
                              tSDP_DISCOVERY_DB *p_db, UINT32 len,
                              tSDP_DISC_CMPL_CB *p_cb);


/*******************************************************************************
**
** Function         SDP_GetNumDiRecords
**
** Description      Searches specified database for DI records
**
** Returns          number of DI records found
**
*******************************************************************************/
extern UINT8  SDP_GetNumDiRecords (tSDP_DISCOVERY_DB *p_db);


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
extern UINT16 SDP_GetDiRecord (UINT8 getRecordIndex,
                               tSDP_DI_GET_RECORD *device_info,
                               tSDP_DISCOVERY_DB *p_db);


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
extern UINT8 SDP_SetTraceLevel (UINT8 new_level);

/*******************************************************************************
**
** Function         SDP_ConnOpen
**
** Description      This function creates a connection to the SDP server on the
**                  given device.
**
** Returns          0, if failed to initiate connection. Otherwise, the handle.
**
*******************************************************************************/
UINT32 SDP_ConnOpen (UINT8 *p_bd_addr, tSDP_DISC_RES_CB *p_rcb,
                     tSDP_DISC_CMPL_CB *p_cb);

/*******************************************************************************
**
** Function         SDP_WriteData
**
** Description      This function sends data to the connected SDP server.
**
** Returns          TRUE if data is sent, FALSE if failed.
**
*******************************************************************************/
BOOLEAN SDP_WriteData (UINT32 handle, BT_HDR  *p_msg);

/*******************************************************************************
**
** Function         SDP_ConnClose
**
** Description      This function is called to close a SDP connection.
**
** Parameters:      handle      - Handle of the connection returned by SDP_ConnOpen
**
** Returns          TRUE if connection is closed, FALSE if failed to find the handle.
**
*******************************************************************************/
BOOLEAN SDP_ConnClose (UINT32 handle);

/*******************************************************************************
**
** Function         SDP_FindServiceUUIDInRec
**
** Description      This function is called to read the service UUID within a record
**                  if there is any.
**
** Parameters:      p_rec      - pointer to a SDP record.
**
** Returns          TRUE if found, otherwise FALSE.
**
*******************************************************************************/
BOOLEAN SDP_FindServiceUUIDInRec(tSDP_DISC_REC *p_rec, tBT_UUID *p_uuid);

#ifdef __cplusplus
}
#endif

#endif  ///SDP_INCLUDED == TRUE

#endif  /* SDP_API_H */
