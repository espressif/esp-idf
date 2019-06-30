/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __BT_SDP_H__
#define __BT_SDP_H__

#include <stdint.h>
// #include "bluetooth.h"
#include "common/bt_defs.h"

#define SDP_OPP_SUPPORTED_FORMATS_MAX_LENGTH 15

/**
 * These events are handled by the state machine
 */
typedef enum {
    SDP_TYPE_RAW,        // Used to carry raw SDP search data for unknown UUIDs
    SDP_TYPE_MAP_MAS,    // Message Access Profile - Server
    SDP_TYPE_MAP_MNS,    // Message Access Profile - Client (Notification Server)
    SDP_TYPE_PBAP_PSE,   // Phone Book Profile - Server
    SDP_TYPE_PBAP_PCE,   // Phone Book Profile - Client
    SDP_TYPE_OPP_SERVER, // Object Push Profile
    SDP_TYPE_SAP_SERVER  // SIM Access Profile
} bluetooth_sdp_types;

typedef struct _bluetooth_sdp_hdr {
    bluetooth_sdp_types type;
    bt_uuid_t   uuid;
    uint32_t    service_name_length;
    char       *service_name;
    int32_t     rfcomm_channel_number;
    int32_t     l2cap_psm;
    int32_t     profile_version;
} bluetooth_sdp_hdr;

/**
 * Some signals need additional pointers, hence we introduce a
 * generic way to handle these pointers.
 */
typedef struct _bluetooth_sdp_hdr_overlay {
    bluetooth_sdp_types type;
    bt_uuid_t   uuid;
    uint32_t    service_name_length;
    char       *service_name;
    int32_t     rfcomm_channel_number;
    int32_t     l2cap_psm;
    int32_t     profile_version;

    // User pointers, only used for some signals - see bluetooth_sdp_ops_record
    int         user1_ptr_len;
    uint8_t    *user1_ptr;
    int         user2_ptr_len;
    uint8_t    *user2_ptr;
} bluetooth_sdp_hdr_overlay;

typedef struct _bluetooth_sdp_mas_record {
    bluetooth_sdp_hdr_overlay hdr;
    uint32_t    mas_instance_id;
    uint32_t    supported_features;
    uint32_t    supported_message_types;
} bluetooth_sdp_mas_record;

typedef struct _bluetooth_sdp_mns_record {
    bluetooth_sdp_hdr_overlay hdr;
    uint32_t    supported_features;
} bluetooth_sdp_mns_record;

typedef struct _bluetooth_sdp_pse_record {
    bluetooth_sdp_hdr_overlay hdr;
    uint32_t    supported_features;
    uint32_t    supported_repositories;
} bluetooth_sdp_pse_record;

typedef struct _bluetooth_sdp_pce_record {
    bluetooth_sdp_hdr_overlay hdr;
} bluetooth_sdp_pce_record;

typedef struct _bluetooth_sdp_ops_record {
    bluetooth_sdp_hdr_overlay hdr;
    int         supported_formats_list_len;
    uint8_t     supported_formats_list[SDP_OPP_SUPPORTED_FORMATS_MAX_LENGTH];
} bluetooth_sdp_ops_record;

typedef struct _bluetooth_sdp_sap_record {
    bluetooth_sdp_hdr_overlay hdr;
} bluetooth_sdp_sap_record;

typedef union {
    bluetooth_sdp_hdr_overlay   hdr;
    bluetooth_sdp_mas_record    mas;
    bluetooth_sdp_mns_record    mns;
    bluetooth_sdp_pse_record    pse;
    bluetooth_sdp_pce_record    pce;
    bluetooth_sdp_ops_record    ops;
    bluetooth_sdp_sap_record    sap;
} bluetooth_sdp_record;

#endif /* __BT_SDP_H__ */
