/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "btc_sdp.h"
#include "btc/btc_manage.h"
#include "btc/btc_task.h"
#include "bta/bta_sdp_api.h"
#include "bta/bta_sys.h"
#include "bta/utl.h"
#include "bt_sdp.h"
#include "osi/mutex.h"
#include "osi/allocator.h"
#include "esp_sdp_api.h"

#if (defined BTC_SDP_COMMON_INCLUDED && BTC_SDP_COMMON_INCLUDED == TRUE)

typedef enum {
    SDP_RECORD_FREE = 0,
    SDP_RECORD_ALLOCED,
} sdp_state_t;

typedef struct {
    uint8_t di         : 1;
    uint8_t primary_di : 1;
    uint8_t reserved   : 6;
} sdp_flag_t;

typedef struct {
    uint8_t state;
    sdp_flag_t flag;
    int sdp_handle;
    esp_bt_uuid_t uuid;
    void* record_data;
} sdp_slot_t;

typedef struct {
    bool search_allowed;
    sdp_slot_t *sdp_slots[SDP_MAX_RECORDS];
    osi_mutex_t sdp_slot_mutex;
} sdp_local_param_t;

#if SDP_DYNAMIC_MEMORY == FALSE
static sdp_local_param_t sdp_local_param;
#else
static sdp_local_param_t *sdp_local_param_ptr;
#define sdp_local_param (*sdp_local_param_ptr)
#endif

#if SDP_DYNAMIC_MEMORY == FALSE
#define is_sdp_init() (sdp_local_param.sdp_slot_mutex != NULL)
#else
#define is_sdp_init() (&sdp_local_param != NULL && sdp_local_param.sdp_slot_mutex != NULL)
#endif

static void btc_sdp_cleanup(void)
{
#if SDP_DYNAMIC_MEMORY == TRUE
    if (sdp_local_param_ptr) {
#endif
        if (sdp_local_param.sdp_slot_mutex) {
            osi_mutex_free(&sdp_local_param.sdp_slot_mutex);
            sdp_local_param.sdp_slot_mutex = NULL;
        }
#if SDP_DYNAMIC_MEMORY == TRUE
        osi_free(sdp_local_param_ptr);
        sdp_local_param_ptr = NULL;
    }
#endif
}

static inline void btc_sdp_cb_to_app(esp_sdp_cb_event_t event, esp_sdp_cb_param_t *param)
{
    esp_sdp_cb_t btc_sdp_cb = (esp_sdp_cb_t)btc_profile_cb_get(BTC_PID_SDP);
    if (btc_sdp_cb) {
        btc_sdp_cb(event, param);
    }
}

static int get_sdp_record_size(bluetooth_sdp_record* in_record)
{
    bluetooth_sdp_record *record = in_record;
    int records_size = 0;

    switch (record->hdr.type) {
    case SDP_TYPE_DIP_SERVER:
        records_size = sizeof(bluetooth_sdp_record);
        break;

    default:
        records_size += sizeof(bluetooth_sdp_record);
        records_size += record->hdr.service_name_length;
        if (record->hdr.service_name_length > 0) {
            records_size++; /* + '\0' termination of string */
        }
        break;
    }

    return records_size;
}

static void set_sdp_slot_info(int id, int sdp_handle, esp_bt_uuid_t *uuid)
{
    sdp_slot_t *slot = NULL;

    BTC_TRACE_DEBUG("%s() id=%d to sdp_handle=0x%08x", __func__, id, sdp_handle);

    if(id >= SDP_MAX_RECORDS) {
        BTC_TRACE_ERROR("%s() failed - id %d is invalid", __func__, id);
        return;
    }

    osi_mutex_lock(&sdp_local_param.sdp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);

    do {
        slot = sdp_local_param.sdp_slots[id];
        if (slot == NULL) {
            BTC_TRACE_ERROR("%s() id = %d ", __func__, id);
            break;
        }

        if (slot->state != SDP_RECORD_ALLOCED) {
            BTC_TRACE_ERROR("%s() failed - state for id %d is state = %d expected %d", __func__, id,
                            sdp_local_param.sdp_slots[id]->state, SDP_RECORD_ALLOCED);
            break;
        }
        slot->sdp_handle = sdp_handle;
        slot->record_data = NULL;
        if (uuid) {
            memcpy(&slot->uuid, uuid, sizeof(esp_bt_uuid_t));
        } else {
            memset(&slot->uuid, 0, sizeof(esp_bt_uuid_t));
        }
    } while (0);

    osi_mutex_unlock(&sdp_local_param.sdp_slot_mutex);
}

static void get_sdp_slot_info(int id, int *sdp_handle, esp_bt_uuid_t *uuid, sdp_flag_t *flag)
{
    sdp_slot_t *slot = NULL;

    if(id >= SDP_MAX_RECORDS) {
        BTC_TRACE_ERROR("%s() failed - id %d is invalid", __func__, id);
        return;
    }

    osi_mutex_lock(&sdp_local_param.sdp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);

    do {
        slot = sdp_local_param.sdp_slots[id];
        if (slot == NULL) {
            break;
        }

        if (slot->state != SDP_RECORD_ALLOCED) {
            BTC_TRACE_ERROR("%s() failed - state for id %d is state = %d expected %d", __func__, id,
                            sdp_local_param.sdp_slots[id]->state, SDP_RECORD_ALLOCED);
            break;
        }

        if (sdp_handle) {
            *sdp_handle = slot->sdp_handle;
        }

        if (uuid) {
            memcpy(uuid, &slot->uuid, sizeof(esp_bt_uuid_t));
        }

        if (flag) {
            *flag = slot->flag;
        }
    } while (0);

    osi_mutex_unlock(&sdp_local_param.sdp_slot_mutex);
}

static int get_sdp_slot_id_by_handle(int handle)
{
    sdp_slot_t *slot = NULL;

    osi_mutex_lock(&sdp_local_param.sdp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);

    for (int i = 0; i < SDP_MAX_RECORDS; i++) {
        slot = sdp_local_param.sdp_slots[i];
        if ((slot != NULL) && (slot->sdp_handle == handle)) {
            osi_mutex_unlock(&sdp_local_param.sdp_slot_mutex);
            return i;
        }
    }

    osi_mutex_unlock(&sdp_local_param.sdp_slot_mutex);
    return -1;
}

static bluetooth_sdp_record *start_create_sdp(int id)
{
    sdp_slot_t *slot = NULL;
    bluetooth_sdp_record* record_data = NULL;

    if(id >= SDP_MAX_RECORDS) {
        BTC_TRACE_ERROR("%s() failed - id %d is invalid", __func__, id);
        return NULL;
    }

    osi_mutex_lock(&sdp_local_param.sdp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);

    do {
        slot = sdp_local_param.sdp_slots[id];
        if (slot == NULL) {
            BTC_TRACE_ERROR("%s() id = %d ", __func__, id);
            break;
        }

        if (slot->state != SDP_RECORD_ALLOCED) {
            BTC_TRACE_ERROR("%s() failed - state for id %d is state = %d expected %d", __func__, id,
                            sdp_local_param.sdp_slots[id]->state, SDP_RECORD_ALLOCED);
            break;
        }
        record_data = slot->record_data;
    } while (0);

    osi_mutex_unlock(&sdp_local_param.sdp_slot_mutex);

    return record_data;
}

/* Deep copy all content of in_records into out_records.
 * out_records must point to a chunk of memory large enough to contain all
 * the data. Use getSdpRecordsSize() to calculate the needed size. */
static void copy_sdp_record_common(bluetooth_sdp_record* in_record, bluetooth_sdp_record* out_record)
{
    uint8_t *free_ptr = (uint8_t *)(out_record + 1); /* set pointer to after the last entry */

    memcpy(out_record, in_record, sizeof(bluetooth_sdp_record));

    if (in_record->hdr.service_name == NULL || in_record->hdr.service_name_length == 0) {
        out_record->hdr.service_name = NULL;
        out_record->hdr.service_name_length = 0;
    } else {
        out_record->hdr.service_name = (char *)free_ptr; // Update service_name pointer
        // Copy string
        memcpy(free_ptr, in_record->hdr.service_name, in_record->hdr.service_name_length);
        free_ptr += in_record->hdr.service_name_length;
        *(free_ptr) = '\0'; // Set '\0' termination of string
        free_ptr++;
    }
}

static void copy_sdp_record(bluetooth_sdp_record* in_record, bluetooth_sdp_record* out_record)
{
    switch (in_record->hdr.type) {
    case SDP_TYPE_DIP_SERVER:
        memcpy(out_record, in_record, sizeof(bluetooth_sdp_record));
        break;

    default:
        copy_sdp_record_common(in_record, out_record);
        break;
    }
}

static bool check_if_primary_di_record(bluetooth_sdp_record* record)
{
    bool ret = false;

    if (record->hdr.type == SDP_TYPE_DIP_SERVER) {
        bluetooth_sdp_dip_record *di_record = (bluetooth_sdp_dip_record *)record;
        ret = di_record->primary_record;
    }

    return ret;
}

static bool check_if_di_record(bluetooth_sdp_record* record)
{
    return record->hdr.type == SDP_TYPE_DIP_SERVER ? true : false;
}

static int alloc_sdp_slot(bluetooth_sdp_record* in_record)
{
    int i;
    int record_size = get_sdp_record_size(in_record);
    bluetooth_sdp_record *record = NULL;
    sdp_slot_t **slot = NULL;
    bool is_di_record = check_if_di_record(in_record);
    bool is_primary_di_record = check_if_primary_di_record(in_record);
    bool primary_di_record_found = false;

    record = osi_malloc(record_size);
    if (record == NULL) {
        BTC_TRACE_ERROR("%s() osi_malloc failed!", __func__);
        return -1;
    }

    copy_sdp_record(in_record, record);

    osi_mutex_lock(&sdp_local_param.sdp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    // find the primary di record slot
    if (is_di_record && is_primary_di_record) {
        for (i = 0; i < SDP_MAX_RECORDS; i++) {
            slot = &sdp_local_param.sdp_slots[i];
            if ((*slot) && (*slot)->flag.di && (*slot)->flag.primary_di) {
                BTC_TRACE_WARNING("%s() overwrite primary di record!", __func__);
                if ((*slot)->record_data) {
                    osi_free((*slot)->record_data);
                }

                (*slot)->record_data = record;
                primary_di_record_found = true;
                break;
            }
        }
    }

    if (!primary_di_record_found) {
        for (i = 0; i < SDP_MAX_RECORDS; i++) {
            slot = &sdp_local_param.sdp_slots[i];
            if ((*slot) == NULL) {
                if (((*slot) = (sdp_slot_t *)osi_malloc(sizeof(sdp_slot_t))) == NULL) {
                    osi_mutex_unlock(&sdp_local_param.sdp_slot_mutex);
                    BTC_TRACE_ERROR("%s() osi_malloc slot failed!", __func__);
                    osi_free(record);
                    return -1;
                }
                (*slot)->flag.di         = is_di_record;
                (*slot)->flag.primary_di = is_primary_di_record;
                (*slot)->state           = SDP_RECORD_ALLOCED;
                (*slot)->record_data     = record;
                break;
            }
        }
    }
    osi_mutex_unlock(&sdp_local_param.sdp_slot_mutex);
    if(i >= SDP_MAX_RECORDS) {
        BTC_TRACE_ERROR("%s() failed - no more free slots!", __func__);
        osi_free(record);
        return -1;
    }

    return i;
}

static int free_sdp_slot(int id)
{
    int handle = -1;
    bluetooth_sdp_record *record = NULL;
    sdp_slot_t *slot = NULL;

    if(id >= SDP_MAX_RECORDS) {
        APPL_TRACE_ERROR("%s() failed - id %d is invalid", __func__, id);
        return handle;
    }
    slot = sdp_local_param.sdp_slots[id];
    if (slot == NULL) {
        // already freed
        return handle;
    }

    osi_mutex_lock(&sdp_local_param.sdp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    handle = slot->sdp_handle;
    if (slot->state != SDP_RECORD_FREE) {
        /* safe a copy of the pointer, and free after unlock() */
        record = slot->record_data;
    }
    osi_mutex_unlock(&sdp_local_param.sdp_slot_mutex);

    if(record != NULL) {
        osi_free(record);
    } else {
        // Record have already been freed
        handle = -1;
    }
    osi_free(sdp_local_param.sdp_slots[id]);
    sdp_local_param.sdp_slots[id] = NULL;

    return handle;
}

/* Create a raw SDP record based on information stored in a bluetooth_sdp_raw_record */
static int add_raw_sdp(const bluetooth_sdp_raw_record *rec)
{
    tSDP_PROTOCOL_ELEM  protoList [2];
    UINT16              browse = UUID_SERVCLASS_PUBLIC_BROWSE_GROUP;
    bool                status = true;
    // Buffer capable to hold 2, 4 and 16-byte UUIDs
    UINT8               temp[LEN_UUID_128];
    UINT8*              p_temp = temp;
    UINT32              sdp_handle = 0;
    const esp_bt_uuid_t *p_uuid = &rec->hdr.uuid;

    BTC_TRACE_DEBUG("%s(): scn 0x%02x, psm = 0x%04x\n  service name %s", __func__,
            rec->hdr.rfcomm_channel_number, rec->hdr.l2cap_psm, rec->hdr.service_name);

    if ((sdp_handle = SDP_CreateRecord()) == 0) {
        BTC_TRACE_ERROR("%s(): Unable to register raw sdp record", __func__);
        return sdp_handle;
    }

    if (p_uuid->len == ESP_UUID_LEN_16) {
        UINT8_TO_BE_STREAM (p_temp, (UUID_DESC_TYPE << 3) | SIZE_TWO_BYTES);
        UINT16_TO_BE_STREAM (p_temp, p_uuid->uuid.uuid16);
    } else if (p_uuid->len == ESP_UUID_LEN_32) {
        UINT8_TO_BE_STREAM (p_temp, (UUID_DESC_TYPE << 3) | SIZE_FOUR_BYTES);
        UINT32_TO_BE_STREAM (p_temp, p_uuid->uuid.uuid32);
    } else if (p_uuid->len == ESP_UUID_LEN_128) {
        UINT8_TO_BE_STREAM (p_temp, (UUID_DESC_TYPE << 3) | SIZE_SIXTEEN_BYTES);
        ARRAY_TO_BE_STREAM (p_temp, p_uuid->uuid.uuid128, LEN_UUID_128);
    } else {
        SDP_DeleteRecord(sdp_handle);
        sdp_handle = 0;
        return sdp_handle;
    }
    /* add service class */
    status &= SDP_AddAttribute(sdp_handle, ATTR_ID_SERVICE_CLASS_ID_LIST,
                                DATA_ELE_SEQ_DESC_TYPE, (UINT32) (p_temp - temp), temp);

    memset( protoList, 0 , 2*sizeof(tSDP_PROTOCOL_ELEM) );

    /* add protocol list, including RFCOMM scn */
    protoList[0].protocol_uuid = UUID_PROTOCOL_L2CAP;
    protoList[0].num_params = 0;
    if (rec->hdr.rfcomm_channel_number < 0) {
        status &= SDP_AddProtocolList(sdp_handle, 1, protoList);
    } else {
        protoList[1].protocol_uuid = UUID_PROTOCOL_RFCOMM;
        protoList[1].num_params = 1;
        protoList[1].params[0] = rec->hdr.rfcomm_channel_number;
        status &= SDP_AddProtocolList(sdp_handle, 2, protoList);
    }

    /* Add a name entry */
    status &= SDP_AddAttribute(sdp_handle,
                    (UINT16)ATTR_ID_SERVICE_NAME,
                    (UINT8)TEXT_STR_DESC_TYPE,
                    (UINT32)(rec->hdr.service_name_length + 1),
                    (UINT8 *)rec->hdr.service_name);

    /* Add the L2CAP PSM if present */
    if(rec->hdr.l2cap_psm != -1) {
        p_temp = temp;// The macro modifies p_temp, hence rewind.
        UINT16_TO_BE_STREAM(p_temp, rec->hdr.l2cap_psm);
        status &= SDP_AddAttribute(sdp_handle, ATTR_ID_GOEP_L2CAP_PSM,
                UINT_DESC_TYPE, (UINT32)2, temp);
    }

    /* Make the service browsable */
    status &= SDP_AddUuidSequence (sdp_handle, ATTR_ID_BROWSE_GROUP_LIST, 1, &browse);

    if (!status) {
        SDP_DeleteRecord(sdp_handle);
        sdp_handle = 0;
        BTC_TRACE_ERROR("%s() FAILED, status = %d", __func__, status);
    } else {
        if (p_uuid->len == ESP_UUID_LEN_16) {
            bta_sys_add_uuid(p_uuid->uuid.uuid16);
        } else if (p_uuid->len == ESP_UUID_LEN_32) {
            bta_sys_add_uuid_32(p_uuid->uuid.uuid32);
        } else if (p_uuid->len == ESP_UUID_LEN_128) {
            bta_sys_add_uuid_128((UINT8 *)&p_uuid->uuid.uuid128);
        }
        BTC_TRACE_DEBUG("%s():  SDP Registered (handle 0x%08x)", __func__, sdp_handle);
    }

    return sdp_handle;
}

/* Create a MAP MAS SDP record based on information stored in a bluetooth_sdp_mas_record */
static int add_maps_sdp(const bluetooth_sdp_mas_record* rec)
{
    tSDP_PROTOCOL_ELEM  protoList [3];
    UINT16              service = UUID_SERVCLASS_MESSAGE_ACCESS;
    UINT16              browse = UUID_SERVCLASS_PUBLIC_BROWSE_GROUP;
    bool                status = true;
    UINT32              sdp_handle = 0;
    UINT8               temp[4];
    UINT8*              p_temp = temp;

    BTC_TRACE_DEBUG("%s(): MASID = 0x%02x, scn 0x%02x, psm = 0x%04x\n  service name %s", __func__,
            rec->mas_instance_id, rec->hdr.rfcomm_channel_number,
            rec->hdr.l2cap_psm, rec->hdr.service_name);

    BTC_TRACE_DEBUG("  msg_types: 0x%02x, feature_bits: 0x%08x",
            rec->supported_message_types, rec->supported_features);

    if ((sdp_handle = SDP_CreateRecord()) == 0) {
        BTC_TRACE_ERROR("%s() - Unable to register MAPS Service", __func__);
        return sdp_handle;
    }

    /* add service class */
    status &= SDP_AddServiceClassIdList(sdp_handle, 1, &service);
    memset( protoList, 0 , 3*sizeof(tSDP_PROTOCOL_ELEM) );

    /* add protocol list, including RFCOMM scn */
    protoList[0].protocol_uuid = UUID_PROTOCOL_L2CAP;
    protoList[0].num_params = 0;
    protoList[1].protocol_uuid = UUID_PROTOCOL_RFCOMM;
    protoList[1].num_params = 1;
    protoList[1].params[0] = rec->hdr.rfcomm_channel_number;
    protoList[2].protocol_uuid = UUID_PROTOCOL_OBEX;
    protoList[2].num_params = 0;
    status &= SDP_AddProtocolList(sdp_handle, 3, protoList);

    /* Add a name entry */
    status &= SDP_AddAttribute(sdp_handle,
                    (UINT16)ATTR_ID_SERVICE_NAME,
                    (UINT8)TEXT_STR_DESC_TYPE,
                    (UINT32)(rec->hdr.service_name_length + 1),
                    (UINT8 *)rec->hdr.service_name);

    /* Add in the Bluetooth Profile Descriptor List */
    status &= SDP_AddProfileDescriptorList(sdp_handle,
                                     UUID_SERVCLASS_MAP_PROFILE,
                                     rec->hdr.profile_version);

    /* Add MAS instance ID */
    status &= SDP_AddAttribute(sdp_handle, ATTR_ID_MAS_INSTANCE_ID, UINT_DESC_TYPE,
              (UINT32)1, (UINT8*)&rec->mas_instance_id);

    /* Add supported message types */
    status &= SDP_AddAttribute(sdp_handle, ATTR_ID_SUPPORTED_MSG_TYPE, UINT_DESC_TYPE,
              (UINT32)1, (UINT8*)&rec->supported_message_types);

    /* Add supported feature */
    UINT32_TO_BE_STREAM(p_temp, rec->supported_features);
    status &= SDP_AddAttribute(sdp_handle, ATTR_ID_MAP_SUPPORTED_FEATURES,
            UINT_DESC_TYPE, (UINT32)4, temp);

    /* Add the L2CAP PSM if present */
    if(rec->hdr.l2cap_psm != -1) {
        p_temp = temp;// The macro modifies p_temp, hence rewind.
        UINT16_TO_BE_STREAM(p_temp, rec->hdr.l2cap_psm);
        status &= SDP_AddAttribute(sdp_handle, ATTR_ID_GOEP_L2CAP_PSM,
                UINT_DESC_TYPE, (UINT32)2, temp);
    }

    /* Make the service browsable */
    status &= SDP_AddUuidSequence (sdp_handle, ATTR_ID_BROWSE_GROUP_LIST, 1, &browse);

    if (!status) {
        SDP_DeleteRecord(sdp_handle);
        sdp_handle = 0;
        BTC_TRACE_ERROR("%s() FAILED", __func__);
    } else {
        bta_sys_add_uuid(service);  /* UUID_SERVCLASS_MESSAGE_ACCESS */
        BTC_TRACE_DEBUG("%s():  SDP Registered (handle 0x%08x)", __func__, sdp_handle);
    }

    return sdp_handle;
}

/* Create a MAP MNS SDP record based on information stored in a bluetooth_sdp_mns_record */
static int add_mapc_sdp(const bluetooth_sdp_mns_record* rec)
{
    tSDP_PROTOCOL_ELEM  protoList [3];
    UINT16              service = UUID_SERVCLASS_MESSAGE_NOTIFICATION;
    UINT16              browse = UUID_SERVCLASS_PUBLIC_BROWSE_GROUP;
    bool                status = true;
    UINT32              sdp_handle = 0;
    UINT8               temp[4];
    UINT8*              p_temp = temp;

    BTC_TRACE_DEBUG("%s(): scn 0x%02x, psm = 0x%04x\n  service name %s", __func__,
            rec->hdr.rfcomm_channel_number, rec->hdr.l2cap_psm, rec->hdr.service_name);

    BTC_TRACE_DEBUG("  feature_bits: 0x%08x", rec->supported_features);

    if ((sdp_handle = SDP_CreateRecord()) == 0) {
        BTC_TRACE_ERROR("%s(): Unable to register MAP Notification Service", __func__);
        return sdp_handle;
    }

    /* add service class */
    status &= SDP_AddServiceClassIdList(sdp_handle, 1, &service);
    memset( protoList, 0 , 3*sizeof(tSDP_PROTOCOL_ELEM) );

    /* add protocol list, including RFCOMM scn */
    protoList[0].protocol_uuid = UUID_PROTOCOL_L2CAP;
    protoList[0].num_params = 0;
    protoList[1].protocol_uuid = UUID_PROTOCOL_RFCOMM;
    protoList[1].num_params = 1;
    protoList[1].params[0] = rec->hdr.rfcomm_channel_number;
    protoList[2].protocol_uuid = UUID_PROTOCOL_OBEX;
    protoList[2].num_params = 0;
    status &= SDP_AddProtocolList(sdp_handle, 3, protoList);

    /* Add a name entry */
    status &= SDP_AddAttribute(sdp_handle,
                    (UINT16)ATTR_ID_SERVICE_NAME,
                    (UINT8)TEXT_STR_DESC_TYPE,
                    (UINT32)(rec->hdr.service_name_length + 1),
                    (UINT8 *)rec->hdr.service_name);

    /* Add in the Bluetooth Profile Descriptor List */
    status &= SDP_AddProfileDescriptorList(sdp_handle,
                                     UUID_SERVCLASS_MAP_PROFILE,
                                     rec->hdr.profile_version);

    /* Add supported feature */
    UINT32_TO_BE_STREAM(p_temp, rec->supported_features);
    status &= SDP_AddAttribute(sdp_handle, ATTR_ID_MAP_SUPPORTED_FEATURES,
            UINT_DESC_TYPE, (UINT32)4, temp);

    /* Add the L2CAP PSM if present */
    if(rec->hdr.l2cap_psm != -1) {
        p_temp = temp;// The macro modifies p_temp, hence rewind.
        UINT16_TO_BE_STREAM(p_temp, rec->hdr.l2cap_psm);
        status &= SDP_AddAttribute(sdp_handle, ATTR_ID_GOEP_L2CAP_PSM,
                UINT_DESC_TYPE, (UINT32)2, temp);
    }

    /* Make the service browsable */
    status &= SDP_AddUuidSequence (sdp_handle, ATTR_ID_BROWSE_GROUP_LIST, 1, &browse);

    if (!status) {
        SDP_DeleteRecord(sdp_handle);
        sdp_handle = 0;
        BTC_TRACE_ERROR("%s() FAILED", __func__);
    } else {
        bta_sys_add_uuid(service);  /* UUID_SERVCLASS_MESSAGE_ACCESS */
        BTC_TRACE_DEBUG("%s():  SDP Registered (handle 0x%08x)", __func__, sdp_handle);
    }

    return sdp_handle;
}

/* Create a PBAP Server SDP record based on information stored in a bluetooth_sdp_pse_record */
static int add_pbaps_sdp(const bluetooth_sdp_pse_record* rec)
{
    tSDP_PROTOCOL_ELEM  protoList [3];
    UINT16              service = UUID_SERVCLASS_PBAP_PSE;
    UINT16              browse = UUID_SERVCLASS_PUBLIC_BROWSE_GROUP;
    bool                status = true;
    UINT32              sdp_handle = 0;
    UINT8               temp[4];
    UINT8*              p_temp = temp;

    BTC_TRACE_DEBUG("%s(): scn 0x%02x, psm = 0x%04x\n  service name %s", __func__,
            rec->hdr.rfcomm_channel_number, rec->hdr.l2cap_psm, rec->hdr.service_name);

    BTC_TRACE_DEBUG("  supported_repositories: 0x%08x, feature_bits: 0x%08x",
            rec->supported_repositories, rec->supported_features);

    if ((sdp_handle = SDP_CreateRecord()) == 0) {
        BTC_TRACE_ERROR("%s(): Unable to register PBAP Server Service", __func__);
        return sdp_handle;
    }

    /* add service class */
    status &= SDP_AddServiceClassIdList(sdp_handle, 1, &service);
    memset( protoList, 0 , 3*sizeof(tSDP_PROTOCOL_ELEM) );

    /* add protocol list, including RFCOMM scn */
    protoList[0].protocol_uuid = UUID_PROTOCOL_L2CAP;
    protoList[0].num_params = 0;
    protoList[1].protocol_uuid = UUID_PROTOCOL_RFCOMM;
    protoList[1].num_params = 1;
    protoList[1].params[0] = rec->hdr.rfcomm_channel_number;
    protoList[2].protocol_uuid = UUID_PROTOCOL_OBEX;
    protoList[2].num_params = 0;
    status &= SDP_AddProtocolList(sdp_handle, 3, protoList);

    /* Add a name entry */
    status &= SDP_AddAttribute(sdp_handle,
                    (UINT16)ATTR_ID_SERVICE_NAME,
                    (UINT8)TEXT_STR_DESC_TYPE,
                    (UINT32)(rec->hdr.service_name_length + 1),
                    (UINT8 *)rec->hdr.service_name);

    /* Add in the Bluetooth Profile Descriptor List */
    status &= SDP_AddProfileDescriptorList(sdp_handle,
                                     UUID_SERVCLASS_PHONE_ACCESS,
                                     rec->hdr.profile_version);

    /* Add supported repositories 1 byte */
    status &= SDP_AddAttribute(sdp_handle, ATTR_ID_SUPPORTED_REPOSITORIES,
            UINT_DESC_TYPE, (UINT32)1, (UINT8*)&rec->supported_repositories);

    /* Add supported feature 4 bytes*/
    UINT32_TO_BE_STREAM(p_temp, rec->supported_features);
    status &= SDP_AddAttribute(sdp_handle, ATTR_ID_PBAP_SUPPORTED_FEATURES,
            UINT_DESC_TYPE, (UINT32)4, temp);

    /* Add the L2CAP PSM if present */
    if(rec->hdr.l2cap_psm != -1) {
        p_temp = temp;// The macro modifies p_temp, hence rewind.
        UINT16_TO_BE_STREAM(p_temp, rec->hdr.l2cap_psm);
        status &= SDP_AddAttribute(sdp_handle, ATTR_ID_GOEP_L2CAP_PSM,
                UINT_DESC_TYPE, (UINT32)2, temp);
    }

    /* Make the service browsable */
    status &= SDP_AddUuidSequence (sdp_handle, ATTR_ID_BROWSE_GROUP_LIST, 1, &browse);

    if (!status) {
        SDP_DeleteRecord(sdp_handle);
        sdp_handle = 0;
        BTC_TRACE_ERROR("%s() FAILED, status = %d", __func__, status);
    } else {
        bta_sys_add_uuid(service);  /* UUID_SERVCLASS_MESSAGE_ACCESS */
        BTC_TRACE_DEBUG("%s():  SDP Registered (handle 0x%08x)", __func__, sdp_handle);
    }

    return sdp_handle;
}

/* Create a PBAP Client SDP record based on information stored in a bluetooth_sdp_pse_record */
static int add_pbapc_sdp(const bluetooth_sdp_pce_record* rec)
{
    UINT16              service = UUID_SERVCLASS_PBAP_PCE;
    UINT16              browse = UUID_SERVCLASS_PUBLIC_BROWSE_GROUP;
    bool                status = true;
    UINT32              sdp_handle = 0;

    BTC_TRACE_DEBUG("%s(): scn 0x%02x, psm = 0x%04x\n  service name %s", __func__,
            rec->hdr.rfcomm_channel_number, rec->hdr.l2cap_psm, rec->hdr.service_name);

    if ((sdp_handle = SDP_CreateRecord()) == 0) {
        BTC_TRACE_ERROR("%s(): Unable to register PBAP Client Service", __func__);
        return sdp_handle;
    }

    /* add service class */
    status &= SDP_AddServiceClassIdList(sdp_handle, 1, &service);

    /* Add a name entry */
    status &= SDP_AddAttribute(sdp_handle,
                    (UINT16)ATTR_ID_SERVICE_NAME,
                    (UINT8)TEXT_STR_DESC_TYPE,
                    (UINT32)(rec->hdr.service_name_length + 1),
                    (UINT8 *)rec->hdr.service_name);

    /* Add in the Bluetooth Profile Descriptor List */
    status &= SDP_AddProfileDescriptorList(sdp_handle,
                                     UUID_SERVCLASS_PHONE_ACCESS,
                                     rec->hdr.profile_version);

    /* Make the service browsable */
    status &= SDP_AddUuidSequence (sdp_handle, ATTR_ID_BROWSE_GROUP_LIST, 1, &browse);

    if (!status) {
        SDP_DeleteRecord(sdp_handle);
        sdp_handle = 0;
        BTC_TRACE_ERROR("%s() FAILED, status = %d", __func__, status);
    } else {
        bta_sys_add_uuid(service);  /* UUID_SERVCLASS_MESSAGE_ACCESS */
        BTC_TRACE_DEBUG("%s():  SDP Registered (handle 0x%08x)", __func__, sdp_handle);
    }

    return sdp_handle;
}


/* Create a OPP Server SDP record based on information stored in a bluetooth_sdp_ops_record */
static int add_opps_sdp(const bluetooth_sdp_ops_record* rec)
{
    tSDP_PROTOCOL_ELEM  protoList [3];
    UINT16              service = UUID_SERVCLASS_OBEX_OBJECT_PUSH;
    UINT16              browse = UUID_SERVCLASS_PUBLIC_BROWSE_GROUP;
    UINT8               type_len[rec->supported_formats_list_len];
    UINT8               desc_type[rec->supported_formats_list_len];
    UINT8              *type_value[rec->supported_formats_list_len];
    bool                status = true;
    UINT32              sdp_handle = 0;
    UINT8               temp[4];
    UINT8*              p_temp = temp;
    tBTA_UTL_COD        cod;
    int i,j;

    BTC_TRACE_DEBUG("%s(): scn 0x%02x, psm = 0x%04x\n  service name %s", __func__,
            rec->hdr.rfcomm_channel_number, rec->hdr.l2cap_psm, rec->hdr.service_name);

    BTC_TRACE_DEBUG("  supported formats count: %d",
            rec->supported_formats_list_len);

    if ((sdp_handle = SDP_CreateRecord()) == 0) {
        BTC_TRACE_ERROR("%s(): Unable to register Object Push Server Service", __func__);
        return sdp_handle;
    }

    /* add service class */
    status &= SDP_AddServiceClassIdList(sdp_handle, 1, &service);
    memset( protoList, 0 , 3*sizeof(tSDP_PROTOCOL_ELEM) );

    /* add protocol list, including RFCOMM scn */
    protoList[0].protocol_uuid = UUID_PROTOCOL_L2CAP;
    protoList[0].num_params = 0;
    protoList[1].protocol_uuid = UUID_PROTOCOL_RFCOMM;
    protoList[1].num_params = 1;
    protoList[1].params[0] = rec->hdr.rfcomm_channel_number;
    protoList[2].protocol_uuid = UUID_PROTOCOL_OBEX;
    protoList[2].num_params = 0;
    status &= SDP_AddProtocolList(sdp_handle, 3, protoList);

    /* Add a name entry */
    status &= SDP_AddAttribute(sdp_handle,
                    (UINT16)ATTR_ID_SERVICE_NAME,
                    (UINT8)TEXT_STR_DESC_TYPE,
                    (UINT32)(rec->hdr.service_name_length + 1),
                    (UINT8 *)rec->hdr.service_name);

    /* Add in the Bluetooth Profile Descriptor List */
    status &= SDP_AddProfileDescriptorList(sdp_handle,
                                     UUID_SERVCLASS_OBEX_OBJECT_PUSH,
                                     rec->hdr.profile_version);

    /* add sequence for supported types */
    for (i = 0, j = 0; i < rec->supported_formats_list_len; i++) {
        type_value[j] = (UINT8 *) &rec->supported_formats_list[i];
        desc_type[j] = UINT_DESC_TYPE;
        type_len[j++] = 1;
    }

    status &= SDP_AddSequence(sdp_handle, (UINT16) ATTR_ID_SUPPORTED_FORMATS_LIST,
        (UINT8) rec->supported_formats_list_len, desc_type, type_len, type_value);

    /* Add the L2CAP PSM if present */
    if(rec->hdr.l2cap_psm != -1) {
        p_temp = temp;// The macro modifies p_temp, hence rewind.
        UINT16_TO_BE_STREAM(p_temp, rec->hdr.l2cap_psm);
        status &= SDP_AddAttribute(sdp_handle, ATTR_ID_GOEP_L2CAP_PSM,
                UINT_DESC_TYPE, (UINT32)2, temp);
    }

    /* Make the service browsable */
    status &= SDP_AddUuidSequence (sdp_handle, ATTR_ID_BROWSE_GROUP_LIST, 1, &browse);

    if (!status) {
        SDP_DeleteRecord(sdp_handle);
        sdp_handle = 0;
        BTC_TRACE_ERROR("%s() FAILED", __func__);
    } else {
        /* set class of device */
        cod.service = BTM_COD_SERVICE_OBJ_TRANSFER;
        utl_set_device_class(&cod, BTA_UTL_SET_COD_SERVICE_CLASS);

        bta_sys_add_uuid(service);  /* UUID_SERVCLASS_OBEX_OBJECT_PUSH */
        BTC_TRACE_DEBUG("%s():  SDP Registered (handle 0x%08x)", __func__, sdp_handle);
    }

    return sdp_handle;
}

// Create a Sim Access Profile SDP record based on information stored in a bluetooth_sdp_sap_record.
static int add_saps_sdp(const bluetooth_sdp_sap_record* rec)
{
    tSDP_PROTOCOL_ELEM  protoList [2];
    UINT16              services[2];
    UINT16              browse = UUID_SERVCLASS_PUBLIC_BROWSE_GROUP;
    bool                status = true;
    UINT32              sdp_handle = 0;

    BTC_TRACE_DEBUG("%s(): scn 0x%02x, service name %s", __func__,
            rec->hdr.rfcomm_channel_number, rec->hdr.service_name);

    if ((sdp_handle = SDP_CreateRecord()) == 0) {
        BTC_TRACE_ERROR("%s(): Unable to register SAPS Service", __func__);
        return sdp_handle;
    }

    services[0] = UUID_SERVCLASS_SAP;
    services[1] = UUID_SERVCLASS_GENERIC_TELEPHONY;

    // add service class
    status &= SDP_AddServiceClassIdList(sdp_handle, 2, services);
    memset(protoList, 0, 2 * sizeof(tSDP_PROTOCOL_ELEM));

    // add protocol list, including RFCOMM scn
    protoList[0].protocol_uuid = UUID_PROTOCOL_L2CAP;
    protoList[0].num_params = 0;
    protoList[1].protocol_uuid = UUID_PROTOCOL_RFCOMM;
    protoList[1].num_params = 1;
    protoList[1].params[0] = rec->hdr.rfcomm_channel_number;
    status &= SDP_AddProtocolList(sdp_handle, 2, protoList);

    // Add a name entry
    status &= SDP_AddAttribute(sdp_handle,
                    (UINT16)ATTR_ID_SERVICE_NAME,
                    (UINT8)TEXT_STR_DESC_TYPE,
                    (UINT32)(rec->hdr.service_name_length + 1),
                    (UINT8 *)rec->hdr.service_name);

    // Add in the Bluetooth Profile Descriptor List
    status &= SDP_AddProfileDescriptorList(sdp_handle,
            UUID_SERVCLASS_SAP,
            rec->hdr.profile_version);

    // Make the service browsable
    status &= SDP_AddUuidSequence (sdp_handle, ATTR_ID_BROWSE_GROUP_LIST, 1, &browse);

    if (!status) {
        SDP_DeleteRecord(sdp_handle);
        sdp_handle = 0;
        BTC_TRACE_ERROR("%s(): FAILED deleting record", __func__);
    } else {
        bta_sys_add_uuid(UUID_SERVCLASS_SAP);
        BTC_TRACE_DEBUG("%s(): SDP Registered (handle 0x%08x)", __func__, sdp_handle);
    }

    return sdp_handle;
}

static int add_dips_sdp(bluetooth_sdp_dip_record *rec)
{
    UINT32 sdp_handle = 0;
    tBTA_DI_RECORD device_info = {0};

    device_info.vendor           = rec->vendor;
    device_info.vendor_id_source = rec->vendor_id_source;
    device_info.product          = rec->product;
    device_info.version          = rec->version;
    device_info.primary_record   = rec->primary_record;

    BTA_DmSetLocalDiRecord(&device_info, &sdp_handle);

    return sdp_handle;
}

static int btc_handle_create_record_event(int id)
{
    int sdp_handle = 0;
    bluetooth_sdp_record *record = start_create_sdp(id);
    esp_bt_uuid_t service_uuid = {0};

    BTC_TRACE_DEBUG("Sdp Server %s", __func__);

    if (record != NULL) {
        switch (record->hdr.type) {
        case SDP_TYPE_RAW:
            sdp_handle = add_raw_sdp(&record->raw);
            memcpy(&service_uuid, &record->hdr.uuid, sizeof(esp_bt_uuid_t));
            break;
        case SDP_TYPE_MAP_MAS:
            sdp_handle = add_maps_sdp(&record->mas);
            service_uuid.len = ESP_UUID_LEN_16;
            service_uuid.uuid.uuid16 = UUID_SERVCLASS_MESSAGE_ACCESS;
            break;
        case SDP_TYPE_MAP_MNS:
            sdp_handle = add_mapc_sdp(&record->mns);
            service_uuid.len = ESP_UUID_LEN_16;
            service_uuid.uuid.uuid16 = UUID_SERVCLASS_MESSAGE_NOTIFICATION;
            break;
        case SDP_TYPE_PBAP_PSE:
            sdp_handle = add_pbaps_sdp(&record->pse);
            service_uuid.len = ESP_UUID_LEN_16;
            service_uuid.uuid.uuid16 = UUID_SERVCLASS_PBAP_PSE;
            break;
        case SDP_TYPE_PBAP_PCE:
            sdp_handle = add_pbapc_sdp(&record->pce);
            service_uuid.len = ESP_UUID_LEN_16;
            service_uuid.uuid.uuid16 = UUID_SERVCLASS_PBAP_PCE;
            break;
        case SDP_TYPE_OPP_SERVER:
            sdp_handle = add_opps_sdp(&record->ops);
            service_uuid.len = ESP_UUID_LEN_16;
            service_uuid.uuid.uuid16 = UUID_SERVCLASS_OBEX_OBJECT_PUSH;
            break;
        case SDP_TYPE_SAP_SERVER:
            sdp_handle = add_saps_sdp(&record->sap);
            service_uuid.len = ESP_UUID_LEN_16;
            service_uuid.uuid.uuid16 = UUID_SERVCLASS_SAP;
            break;
        case SDP_TYPE_DIP_SERVER:
            sdp_handle = add_dips_sdp(&record->dip);
            break;
        default:
            BTC_TRACE_DEBUG("Record type %d is not supported", record->hdr.type);
            break;
        }

        if(sdp_handle != 0) {
            set_sdp_slot_info(id, sdp_handle, &service_uuid);
            // free the record, since not use it anymore
            osi_free(record);
        } else {
            sdp_handle = -1;
        }
    } else {
        sdp_handle = -1;
    }

    if (sdp_handle == -1) {
        free_sdp_slot(id);
    }

    return sdp_handle;
}

static bool btc_sdp_remove_record_event(int id, int *p_sdp_handle)
{
    BTC_TRACE_DEBUG("Sdp Server %s", __func__);

    bool result = false;
    int sdp_handle = -1;
    sdp_flag_t flag = {0};
    esp_bt_uuid_t service_uuid = {0};

    get_sdp_slot_info(id, &sdp_handle, &service_uuid, &flag);

    if (sdp_handle > 0) {
        if (flag.di && BTA_DmRemoveLocalDiRecord(sdp_handle) == BTA_SUCCESS) {
            result = true;
        } else {
            do {
                result = SDP_DeleteRecord(sdp_handle);
                if (!result) {
                    BTC_TRACE_ERROR("Unable to remove handle 0x%08x", sdp_handle);
                    break;
                }

                if (service_uuid.len == ESP_UUID_LEN_16) {
                    bta_sys_remove_uuid(service_uuid.uuid.uuid16);
                } else if (service_uuid.len == ESP_UUID_LEN_32) {
                    bta_sys_remove_uuid_32(service_uuid.uuid.uuid32);
                } else if (service_uuid.len == ESP_UUID_LEN_128) {
                    bta_sys_remove_uuid_128((UINT8 *)&service_uuid.uuid.uuid128);
                }
            } while (0);
        }

        if (p_sdp_handle) {
            *p_sdp_handle = sdp_handle;
        }
    }

    return result;
}

static void btc_sdp_cb_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    switch (msg->act) {
    case BTA_SDP_SEARCH_COMP_EVT: {
        tBTA_SDP_SEARCH_COMP *src_search_comp = (tBTA_SDP_SEARCH_COMP *)p_src;
        tBTA_SDP_SEARCH_COMP *dest_search_comp = (tBTA_SDP_SEARCH_COMP *)p_dest;
        int record_count = src_search_comp->record_count;

        for (int i = 0; i < record_count; i++) {
            bluetooth_sdp_record *src_record = &src_search_comp->records[i];
            bluetooth_sdp_record *dest_record = &dest_search_comp->records[i];
            // deep copy service name
            uint32_t src_service_name_length = src_record->hdr.service_name_length;
            char *src_service_name = src_record->hdr.service_name;
            dest_record->hdr.service_name_length = 0;
            dest_record->hdr.service_name        = NULL;
            if (src_service_name && src_service_name_length) {
                char *service_name = (char *)osi_malloc(src_service_name_length + 1);
                if (service_name) {
                    memcpy(service_name, src_service_name, src_service_name_length);
                    service_name[src_service_name_length] = '\0';

                    dest_record->hdr.service_name_length = src_service_name_length;
                    dest_record->hdr.service_name        = service_name;
                } else {
                    BTC_TRACE_ERROR("%s malloc service name failed, orig service name:%s", __func__, src_service_name);
                }
            }

            // deep copy user1_ptr fow RAW type
            int src_user1_ptr_len = src_record->hdr.user1_ptr_len;
            uint8_t *src_user1_ptr = src_record->hdr.user1_ptr;
            dest_record->hdr.user1_ptr_len = 0;
            dest_record->hdr.user1_ptr     = NULL;
            if (src_record->hdr.type == SDP_TYPE_RAW && src_user1_ptr && src_user1_ptr_len) {
                uint8_t *user1_ptr = (uint8_t *)osi_malloc(src_user1_ptr_len);
                if (user1_ptr) {
                    memcpy(user1_ptr, src_user1_ptr, src_user1_ptr_len);

                    dest_record->hdr.user1_ptr_len = src_user1_ptr_len;
                    dest_record->hdr.user1_ptr     = user1_ptr;
                } else {
                    BTC_TRACE_ERROR("%s malloc user1_ptr failed", __func__);
                }
            }
        }

        break;
    }
    default:
        break;
    }
}

static void btc_sdp_cb_arg_deep_free(btc_msg_t *msg)
{
    switch (msg->act) {
    case BTA_SDP_SEARCH_COMP_EVT: {
        tBTA_SDP_SEARCH_COMP *search_comp = (tBTA_SDP_SEARCH_COMP *)msg->arg;
        for (size_t i = 0; i < search_comp->record_count; i++) {
            bluetooth_sdp_record *record = &search_comp->records[i];
            if (record->hdr.service_name) {
                osi_free(record->hdr.service_name);
            }

            if (record->hdr.user1_ptr) {
                osi_free(record->hdr.user1_ptr);
            }
        }

        break;
    }
    default:
        break;
    }
}

static void btc_sdp_dm_cback(tBTA_SDP_EVT event, tBTA_SDP* p_data, void* user_data)
{
    btc_msg_t msg;
    bt_status_t status;

    switch (event) {
    case BTA_SDP_CREATE_RECORD_USER_EVT: {
        if (p_data->sdp_create_record.status == BTA_SDP_SUCCESS) {
            p_data->sdp_create_record.handle = btc_handle_create_record_event((int)user_data);
            if (p_data->sdp_create_record.handle < 0) {
                p_data->sdp_create_record.status = BTA_SDP_FAILURE;
            }
        }
    }
    break;
    case BTA_SDP_REMOVE_RECORD_USER_EVT: {
        if (p_data->sdp_remove_record.status == BTA_SDP_SUCCESS) {
            if (btc_sdp_remove_record_event((int)user_data, &p_data->sdp_remove_record.handle) == false) {
                p_data->sdp_remove_record.status = BTA_SDP_FAILURE;
            }
        }
    }
    break;
    default:
        break;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_SDP;
    msg.act = event;

    status = btc_transfer_context(&msg, p_data, sizeof(tBTA_SDP), btc_sdp_cb_arg_deep_copy, btc_sdp_cb_arg_deep_free);

    if (status != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed", __func__);
    }
}

static void btc_sdp_init(void)
{
    esp_sdp_cb_param_t param;
    esp_sdp_status_t ret = ESP_SDP_SUCCESS;

    do {
        if (is_sdp_init()) {
            BTC_TRACE_ERROR("%s SDP has been initiated, shall deinit first!", __func__);
            ret = ESP_SDP_NEED_DEINIT;
            break;
        }

#if SDP_DYNAMIC_MEMORY == TRUE
        if ((sdp_local_param_ptr = (sdp_local_param_t *)osi_malloc(sizeof(sdp_local_param_t))) == NULL) {
            BTC_TRACE_ERROR("%s malloc failed\n", __func__);
            ret = ESP_SDP_NO_RESOURCE;
            break;
        }
#endif
        memset(&sdp_local_param, 0, sizeof(sdp_local_param_t));

        if (osi_mutex_new(&sdp_local_param.sdp_slot_mutex) != 0) {
            BTC_TRACE_ERROR("%s osi_mutex_new failed\n", __func__);
            ret = ESP_SDP_NO_RESOURCE;
            break;
        }

        ret = BTA_SdpEnable(btc_sdp_dm_cback);
        if (ret != ESP_SDP_SUCCESS) {
            BTC_TRACE_ERROR("%s BTA_SdpEnable failed, ret = %d\n", __func__, ret);
            ret = ESP_SDP_FAILURE;
            break;
        }

        sdp_local_param.search_allowed = true;
    } while(0);

    if (ret != ESP_SDP_SUCCESS) {
        btc_sdp_cleanup();
        param.init.status = ret;
        btc_sdp_cb_to_app(ESP_SDP_INIT_EVT, &param);
    }
}

static void btc_sdp_deinit(void)
{
    esp_sdp_cb_param_t param;
    esp_sdp_status_t ret = ESP_SDP_SUCCESS;

    do {
        if (!is_sdp_init()) {
            BTC_TRACE_ERROR("%s SDP has not been initiated, shall init first!", __func__);
            ret = ESP_SDP_NEED_INIT;
            break;
        }

        for(int i = 0; i < SDP_MAX_RECORDS; i++) {
            int sdp_handle = -1;
            get_sdp_slot_info(i, &sdp_handle, NULL, NULL);
            if (sdp_handle > 0) {
                BTA_SdpRemoveRecordByUser((void*)i);
            }
        }
        BTA_SdpDisable();
    } while(0);

    if (ret != ESP_SDP_SUCCESS) {
        param.deinit.status = ret;
        btc_sdp_cb_to_app(ESP_SDP_DEINIT_EVT, &param);
    }
}

static void btc_sdp_create_record(btc_sdp_args_t *arg)
{
    int slot_id;
    esp_sdp_cb_param_t param;
    esp_sdp_status_t ret = ESP_SDP_SUCCESS;

    do {
        if (!is_sdp_init()) {
            BTC_TRACE_ERROR("%s SDP has not been initiated, shall init first!", __func__);
            ret = ESP_SDP_NEED_INIT;
            break;
        }

        slot_id = alloc_sdp_slot(arg->create_record.record);
        if (slot_id < 0) {
            ret = ESP_SDP_FAILURE;
            break;
        }

        BTA_SdpCreateRecordByUser((void *) slot_id);
    } while(0);

    if (ret != ESP_SDP_SUCCESS) {
        param.create_record.status = ret;
        param.create_record.record_handle = -1;
        btc_sdp_cb_to_app(ESP_SDP_CREATE_RECORD_COMP_EVT, &param);
    }
}

static void btc_sdp_remove_record(btc_sdp_args_t *arg)
{
    esp_sdp_cb_param_t param;
    esp_sdp_status_t ret = ESP_SDP_SUCCESS;

    do {
        if (!is_sdp_init()) {
            BTC_TRACE_ERROR("%s SDP has not been initiated, shall init first!", __func__);
            ret = ESP_SDP_NEED_INIT;
            break;
        }

        /* Get the Record handle, and free the slot */
        /* The application layer record_handle is equivalent to the id of the btc layer */
        int slot_id = get_sdp_slot_id_by_handle(arg->remove_record.record_handle);
        if (slot_id < 0) {
            BTC_TRACE_ERROR("%s SDP record with handle %d not found", __func__, arg->remove_record.record_handle);
            ret = ESP_SDP_NO_CREATE_RECORD;
            break;
        }

        BTA_SdpRemoveRecordByUser((void *)slot_id);
    } while(0);

    if (ret != ESP_SDP_SUCCESS) {
        param.create_record.status = ret;
        btc_sdp_cb_to_app(ESP_SDP_REMOVE_RECORD_COMP_EVT, &param);
    }
}

static void btc_sdp_search(btc_sdp_args_t *arg)
{
    esp_sdp_cb_param_t param = {0};
    esp_sdp_status_t ret = ESP_SDP_SUCCESS;

    do {
        if (!is_sdp_init()) {
            BTC_TRACE_ERROR("%s SDP has not been initiated, shall init first!", __func__);
            ret = ESP_SDP_NEED_INIT;
            break;
        }

        if (!sdp_local_param.search_allowed) {
            BTC_TRACE_ERROR("%s SDP search is not allowed!", __func__);
            ret = ESP_SDP_NO_RESOURCE;
            break;
        }

        BTA_SdpSearch(arg->search.bd_addr, &arg->search.sdp_uuid);
        /**
         * ESP_SDP_SEARCH_COMP_EVT will refer service name in BTA sdp database, so it is not allowed to be search until
         * the previous search is completed
         */
        sdp_local_param.search_allowed = false;
    } while(0);

    if (ret != ESP_SDP_SUCCESS) {
        param.search.status = ret;
        btc_sdp_cb_to_app(ESP_SDP_SEARCH_COMP_EVT, &param);
    }
}

void btc_sdp_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    switch (msg->act) {
    case BTC_SDP_ACT_CREATE_RECORD: {
        bluetooth_sdp_record **dst_record = &((btc_sdp_args_t *)p_dest)->create_record.record;
        bluetooth_sdp_record *src_record = ((btc_sdp_args_t *)p_src)->create_record.record;
        bluetooth_sdp_record *record = (bluetooth_sdp_record *)osi_calloc(get_sdp_record_size(src_record));
        if (record) {
            copy_sdp_record(src_record, record);
        } else {
            BTC_TRACE_ERROR("%s %d osi_malloc failed\n", __func__, msg->act);
            break;
        }

        *dst_record = record;
        break;
    }
    default:
        break;
    }
}

void btc_sdp_arg_deep_free(btc_msg_t *msg)
{
    switch (msg->act) {
    case BTC_SDP_ACT_CREATE_RECORD: {
        btc_sdp_args_t *arg = (btc_sdp_args_t *)msg->arg;
        bluetooth_sdp_record *record = arg->create_record.record;
        if (record) {
            osi_free(record);
        }
        break;
    }
    default:
        break;
    }
}

void btc_sdp_call_handler(btc_msg_t *msg)
{
    btc_sdp_args_t *arg = (btc_sdp_args_t *)(msg->arg);

    BTC_TRACE_DEBUG("%s: event = %d\n", __func__, msg->act);

    switch (msg->act) {
    case BTC_SDP_ACT_INIT:
        btc_sdp_init();
        break;
    case BTC_SDP_ACT_DEINIT:
        btc_sdp_deinit();
        break;
    case BTC_SDP_ACT_SEARCH:
        btc_sdp_search(arg);
        break;
    case BTC_SDP_ACT_CREATE_RECORD:
        btc_sdp_create_record(arg);
        break;
    case BTC_SDP_ACT_REMOVE_RECORD:
        btc_sdp_remove_record(arg);
        break;
    default:
        BTC_TRACE_ERROR("%s: Unhandled event (%d)!\n", __func__, msg->act);
        break;
    }
    btc_sdp_arg_deep_free(msg);
}

void btc_sdp_cb_handler(btc_msg_t *msg)
{
    esp_sdp_cb_param_t param = {0};
    uint8_t event = msg->act;
    tBTA_SDP *p_data = (tBTA_SDP *)msg->arg;

    switch(event) {
    case BTA_SDP_ENABLE_EVT:
        param.init.status = p_data->status;
        btc_sdp_cb_to_app(ESP_SDP_INIT_EVT, &param);
        break;
    case BTA_SDP_DISABLE_EVT:
        BTA_SdpCleanup();
        btc_sdp_cleanup();
        param.deinit.status = ESP_SDP_SUCCESS;
        btc_sdp_cb_to_app(ESP_SDP_DEINIT_EVT, &param);
        break;
    case BTA_SDP_SEARCH_COMP_EVT:
        // SDP search completed, now can be searched again
        sdp_local_param.search_allowed = true;

        param.search.status = p_data->sdp_search_comp.status;
        memcpy(param.search.remote_addr, p_data->sdp_search_comp.remote_addr, sizeof(BD_ADDR));
        memcpy(&param.search.sdp_uuid, &p_data->sdp_search_comp.uuid, sizeof(tSDP_UUID));
        param.search.record_count = p_data->sdp_search_comp.record_count;
        param.search.records = (esp_bluetooth_sdp_record_t *)p_data->sdp_search_comp.records;
        btc_sdp_cb_to_app(ESP_SDP_SEARCH_COMP_EVT, &param);
        break;
    case BTA_SDP_CREATE_RECORD_USER_EVT:
        param.create_record.status = p_data->sdp_create_record.status;
        param.create_record.record_handle = p_data->sdp_create_record.handle;
        btc_sdp_cb_to_app(ESP_SDP_CREATE_RECORD_COMP_EVT, &param);
        break;
    case BTA_SDP_REMOVE_RECORD_USER_EVT:
        if (p_data->sdp_remove_record.status == BTA_SDP_SUCCESS) {
            int slot_id = get_sdp_slot_id_by_handle(p_data->sdp_remove_record.handle);
            if (slot_id < 0) {
                p_data->sdp_remove_record.status = ESP_SDP_NO_CREATE_RECORD;
                break;
            } else {
                free_sdp_slot(slot_id);
            }
        }

        param.remove_record.status = p_data->sdp_remove_record.status;
        btc_sdp_cb_to_app(ESP_SDP_REMOVE_RECORD_COMP_EVT, &param);
        break;
    default:
        BTC_TRACE_DEBUG("%s: Unhandled event (%d)!", __func__, msg->act);
        break;
    }

    btc_sdp_cb_arg_deep_free(msg);
}

#endif ///defined BTC_SDP_COMMON_INCLUDED && BTC_SDP_COMMON_INCLUDED == TRUE
