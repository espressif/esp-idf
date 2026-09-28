/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "common/bt_target.h"
#include "stack/sdp_api.h"
#include "bta/bta_api.h"
#include "bta_opp_int.h"

#if BTA_OPP_SERVER_INCLUDED

UINT32 bta_opp_server_create_sdp_record(const tBTA_OPP_SERVER_CFG *cfg)
{
    UINT32 sdp_handle;
    const UINT8 *formats;
    UINT8 formats_len;
    UINT16 service = UUID_SERVCLASS_OBEX_OBJECT_PUSH;
    UINT16 browse = UUID_SERVCLASS_PUBLIC_BROWSE_GROUP;
    tSDP_PROTOCOL_ELEM proto_list[3];
    const char *service_name = cfg->service_name ? cfg->service_name : BTA_OPP_DEFAULT_SERVER_NAME;
    UINT8 default_format = BTA_OPP_DEFAULT_FORMAT;
    UINT8 type_len[15];
    UINT8 desc_type[15];
    UINT8 *type_value[15];
    BOOLEAN status = TRUE;

    if (cfg->supported_formats && cfg->supported_formats_len) {
        formats = cfg->supported_formats;
        formats_len = cfg->supported_formats_len;
    } else {
        formats = &default_format;
        formats_len = 1;
    }

    if (formats_len > 15 || bta_opp_cb.server_scn == 0) {
        return 0;
    }

    sdp_handle = SDP_CreateRecord();
    if (sdp_handle == 0) {
        return 0;
    }

    status &= SDP_AddServiceClassIdList(sdp_handle, 1, &service);
    memset(proto_list, 0, sizeof(proto_list));
    proto_list[0].protocol_uuid = UUID_PROTOCOL_L2CAP;
    proto_list[1].protocol_uuid = UUID_PROTOCOL_RFCOMM;
    proto_list[1].num_params = 1;
    proto_list[1].params[0] = bta_opp_cb.server_scn;
    proto_list[2].protocol_uuid = UUID_PROTOCOL_OBEX;
    status &= SDP_AddProtocolList(sdp_handle, 3, proto_list);

    status &= SDP_AddAttribute(sdp_handle, ATTR_ID_SERVICE_NAME, TEXT_STR_DESC_TYPE,
                               (UINT32)(strlen(service_name) + 1), (UINT8 *)service_name);
    status &= SDP_AddProfileDescriptorList(sdp_handle, UUID_SERVCLASS_OBEX_OBJECT_PUSH, OPP_VERSION);

    for (int i = 0; i < formats_len; i++) {
        type_value[i] = (UINT8 *)&formats[i];
        desc_type[i] = UINT_DESC_TYPE;
        type_len[i] = 1;
    }
    status &= SDP_AddSequence(sdp_handle, ATTR_ID_SUPPORTED_FORMATS_LIST, formats_len, desc_type, type_len, type_value);
    status &= SDP_AddUuidSequence(sdp_handle, ATTR_ID_BROWSE_GROUP_LIST, 1, &browse);

    if (bta_opp_cb.server_l2cap_psm != 0) {
        UINT8 psm_buf[2];
        UINT8 *p_psm = psm_buf;
        UINT16_TO_BE_STREAM(p_psm, bta_opp_cb.server_l2cap_psm);
        status &= SDP_AddAttribute(sdp_handle, ATTR_ID_GOEP_L2CAP_PSM, UINT_DESC_TYPE,
                                 (UINT32)sizeof(psm_buf), psm_buf);
    }

    if (!status) {
        SDP_DeleteRecord(sdp_handle);
        return 0;
    }

    bta_sys_add_uuid(service);
    return sdp_handle;
}

void bta_opp_server_del_sdp_record(void)
{
    if (bta_opp_cb.sdp_handle != 0) {
        SDP_DeleteRecord(bta_opp_cb.sdp_handle);
        bta_opp_cb.sdp_handle = 0;
        bta_sys_remove_uuid(UUID_SERVCLASS_OBEX_OBJECT_PUSH);
    }
}

#endif /* BTA_OPP_SERVER_INCLUDED */
