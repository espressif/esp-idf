/******************************************************************************
 *
 *  Copyright (C) 2016 The Android Open Source Project
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
 *  This file contains the HID Device API entry points
 *
 ******************************************************************************/
//#include <errno.h>
//#include <hardware/bluetooth.h>
//#include <hardware/bt_hd.h>
#include "stack/hidd_api.h"
#include "esp_hidd_api.h"
#include "hid_int.h"
#include "osi/allocator.h"
#include "stack/btm_api.h"
#include "stack/btu.h"
#include "stack/hiddefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if (HID_DEV_INCLUDED == TRUE)

#if HID_DYNAMIC_MEMORY == FALSE
tHID_DEV_CTB hd_cb;
#else
tHID_DEV_CTB *hidd_cb_ptr = NULL;
#endif

/*******************************************************************************
 *
 * Function         HID_DevInit
 *
 * Description      Initializes control block
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
tHID_STATUS HID_DevInit(void)
{
#if (HID_DYNAMIC_MEMORY)
    if (!hidd_cb_ptr) {
        hidd_cb_ptr = (tHID_DEV_CTB *)osi_malloc(sizeof(tHID_DEV_CTB));
        if (!hidd_cb_ptr) {
            return HID_ERR_NO_RESOURCES;
        }
    }
#endif /* #if (HID_DYNAMIC_MEMORY) */
    memset(&hd_cb, 0, sizeof(tHID_DEV_CTB));
#if defined(HIDD_INITIAL_TRACE_LEVEL)
    hd_cb.trace_level = HIDD_INITIAL_TRACE_LEVEL;
#else
    hd_cb.trace_level = BT_TRACE_LEVEL_NONE;
#endif
    return HID_SUCCESS;
}

/*******************************************************************************
 *
 * Function         HID_DevDeinit
 *
 * Description      Deinitializes control block
 *
 * Returns          void
 *
 ******************************************************************************/
void HID_DevDeinit(void)
{
#if (HID_DYNAMIC_MEMORY)
    if (hidd_cb_ptr) {
        osi_free(hidd_cb_ptr);
        hidd_cb_ptr = NULL;
    }
#endif /* #if (HID_DYNAMIC_MEMORY) */
}

/*******************************************************************************
 *
 * Function         HID_DevSetTraceLevel
 *
 * Description      This function sets the trace level for HID Dev. If called
 *                  with
 *                  a value of 0xFF, it simply reads the current trace level.
 *
 * Returns          the new (current) trace level
 *
 ******************************************************************************/
uint8_t HID_DevSetTraceLevel(uint8_t new_level)
{
    if (new_level != 0xFF) {
        hd_cb.trace_level = new_level;
    }

    return (hd_cb.trace_level);
}

/*******************************************************************************
 *
 * Function         HID_DevRegister
 *
 * Description      Registers HID device with lower layers
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
tHID_STATUS HID_DevRegister(tHID_DEV_HOST_CALLBACK *host_cback)
{
    tHID_STATUS st;
    HIDD_TRACE_API("%s", __func__);

    if (hd_cb.reg_flag) {
        return HID_ERR_ALREADY_REGISTERED;
    }

    if (host_cback == NULL) {
        return HID_ERR_INVALID_PARAM;
    }
    /* Register with L2CAP */
    if ((st = hidd_conn_reg()) != HID_SUCCESS) {
        return st;
    }

    hd_cb.callback = host_cback;
    hd_cb.reg_flag = TRUE;

    if (hd_cb.pending_data) {
        osi_free(hd_cb.pending_data);
        hd_cb.pending_data = NULL;
    }
    return (HID_SUCCESS);
}

/*******************************************************************************
 *
 * Function         HID_DevDeregister
 *
 * Description      Deregisters HID device with lower layers
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
tHID_STATUS HID_DevDeregister(void)
{
    HIDD_TRACE_API("%s", __func__);

    if (!hd_cb.reg_flag)
        return (HID_ERR_NOT_REGISTERED);
    hidd_conn_dereg();
    hd_cb.reg_flag = FALSE;

    return (HID_SUCCESS);
}

tHID_STATUS HID_DevSetSecurityLevel(uint8_t sec_lvl)
{
    HIDD_TRACE_API("%s", __func__);
    if (!BTM_SetSecurityLevel(FALSE, "", BTM_SEC_SERVICE_HIDD_SEC_CTRL, sec_lvl, HID_PSM_CONTROL, BTM_SEC_PROTO_HID,
                              HIDD_SEC_CHN)) {
        HIDD_TRACE_ERROR("Security Registration 1 failed");
        return (HID_ERR_NO_RESOURCES);
    }
    if (!BTM_SetSecurityLevel(TRUE, "", BTM_SEC_SERVICE_HIDD_SEC_CTRL, sec_lvl, HID_PSM_CONTROL, BTM_SEC_PROTO_HID,
                              HIDD_SEC_CHN)) {
        HIDD_TRACE_ERROR("Security Registration 2 failed");
        return (HID_ERR_NO_RESOURCES);
    }
    if (!BTM_SetSecurityLevel(FALSE, "", BTM_SEC_SERVICE_HIDD_NOSEC_CTRL, BTM_SEC_NONE, HID_PSM_CONTROL,
                              BTM_SEC_PROTO_HID, HIDD_NOSEC_CHN)) {
        HIDD_TRACE_ERROR("Security Registration 3 failed");
        return (HID_ERR_NO_RESOURCES);
    }
    if (!BTM_SetSecurityLevel(TRUE, "", BTM_SEC_SERVICE_HIDD_NOSEC_CTRL, BTM_SEC_NONE, HID_PSM_CONTROL,
                              BTM_SEC_PROTO_HID, HIDD_NOSEC_CHN)) {
        HIDD_TRACE_ERROR("Security Registration 4 failed");
        return (HID_ERR_NO_RESOURCES);
    }
    if (!BTM_SetSecurityLevel(TRUE, "", BTM_SEC_SERVICE_HIDD_INTR, BTM_SEC_NONE, HID_PSM_INTERRUPT, BTM_SEC_PROTO_HID,
                              0)) {
        HIDD_TRACE_ERROR("Security Registration 5 failed");
        return (HID_ERR_NO_RESOURCES);
    }
    if (!BTM_SetSecurityLevel(FALSE, "", BTM_SEC_SERVICE_HIDD_INTR, BTM_SEC_NONE, HID_PSM_INTERRUPT, BTM_SEC_PROTO_HID,
                              0)) {
        HIDD_TRACE_ERROR("Security Registration 6 failed");
        return (HID_ERR_NO_RESOURCES);
    }
    return (HID_SUCCESS);
}

/*******************************************************************************
 *
 * Function         HID_DevAddRecord
 *
 * Description      Creates SDP record for HID device
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
tHID_STATUS HID_DevAddRecord(uint32_t handle, char *p_name, char *p_description, char *p_provider, uint16_t subclass,
                             uint16_t desc_len, uint8_t *p_desc_data)
{
    bool result = TRUE;

    HIDD_TRACE_API("%s", __func__);

    // Service Class ID List
    if (result) {
        uint16_t uuid = UUID_SERVCLASS_HUMAN_INTERFACE;
        result &= SDP_AddServiceClassIdList(handle, 1, &uuid);
    }
    // Protocol Descriptor List
    if (result) {
        tSDP_PROTOCOL_ELEM proto_list[2];
        proto_list[0].protocol_uuid = UUID_PROTOCOL_L2CAP;
        proto_list[0].num_params = 1;
        proto_list[0].params[0] = BT_PSM_HIDC;
        proto_list[1].protocol_uuid = UUID_PROTOCOL_HIDP;
        proto_list[1].num_params = 0;
        result &= SDP_AddProtocolList(handle, 2, proto_list);
    }
    // Language Base Attribute ID List
    if (result) {
        result &=
            SDP_AddLanguageBaseAttrIDList(handle, LANG_ID_CODE_ENGLISH, LANG_ID_CHAR_ENCODE_UTF8, LANGUAGE_BASE_ID);
    }
    // Additional Protocol Descriptor List
    if (result) {
        tSDP_PROTO_LIST_ELEM add_proto_list;
        add_proto_list.num_elems = 2;
        add_proto_list.list_elem[0].protocol_uuid = UUID_PROTOCOL_L2CAP;
        add_proto_list.list_elem[0].num_params = 1;
        add_proto_list.list_elem[0].params[0] = BT_PSM_HIDI;
        add_proto_list.list_elem[1].protocol_uuid = UUID_PROTOCOL_HIDP;
        add_proto_list.list_elem[1].num_params = 0;
        result &= SDP_AddAdditionProtoLists(handle, 1, &add_proto_list);
    }
    // Service Name (O)
    // Service Description (O)
    // Provider Name (O)
    if (result) {
        const char *srv_name = p_name;
        const char *srv_desc = p_description;
        const char *provider_name = p_provider;
        result &= SDP_AddAttribute(handle, ATTR_ID_SERVICE_NAME, TEXT_STR_DESC_TYPE, strlen(srv_name) + 1,
                                   (uint8_t *)srv_name);
        result &= SDP_AddAttribute(handle, ATTR_ID_SERVICE_DESCRIPTION, TEXT_STR_DESC_TYPE, strlen(srv_desc) + 1,
                                   (uint8_t *)srv_desc);
        result &= SDP_AddAttribute(handle, ATTR_ID_PROVIDER_NAME, TEXT_STR_DESC_TYPE, strlen(provider_name) + 1,
                                   (uint8_t *)provider_name);
    }
    // Bluetooth Profile Descriptor List
    if (result) {
        const uint16_t profile_uuid = UUID_SERVCLASS_HUMAN_INTERFACE;
        const uint16_t version = 0x0100;
        result &= SDP_AddProfileDescriptorList(handle, profile_uuid, version);
    }
    // HID Parser Version
    if (result) {
        uint8_t *p;
        const uint16_t rel_num = 0x0100;
        const uint16_t parser_version = 0x0111;
        const uint16_t prof_ver = 0x0100;
        const uint8_t dev_subclass = subclass;
        const uint8_t country_code = 0x21;
        const uint8_t bool_false = 0x00;
        const uint8_t bool_true = 0x01;
        uint16_t temp;
        p = (uint8_t *)&temp;
        UINT16_TO_BE_STREAM(p, rel_num);
        result &= SDP_AddAttribute(handle, ATTR_ID_HID_DEVICE_RELNUM, UINT_DESC_TYPE, 2, (uint8_t *)&temp);
        p = (uint8_t *)&temp;
        UINT16_TO_BE_STREAM(p, parser_version);
        result &= SDP_AddAttribute(handle, ATTR_ID_HID_PARSER_VERSION, UINT_DESC_TYPE, 2, (uint8_t *)&temp);
        result &= SDP_AddAttribute(handle, ATTR_ID_HID_DEVICE_SUBCLASS, UINT_DESC_TYPE, 1, (uint8_t *)&dev_subclass);
        result &= SDP_AddAttribute(handle, ATTR_ID_HID_COUNTRY_CODE, UINT_DESC_TYPE, 1, (uint8_t *)&country_code);
        result &= SDP_AddAttribute(handle, ATTR_ID_HID_VIRTUAL_CABLE, BOOLEAN_DESC_TYPE, 1, (uint8_t *)&bool_true);
        result &= SDP_AddAttribute(handle, ATTR_ID_HID_RECONNECT_INITIATE, BOOLEAN_DESC_TYPE, 1, (uint8_t *)&bool_true);
        {
            static uint8_t cdt = 0x22;
            uint8_t *p_buf;
            uint8_t seq_len = 4 + desc_len;
            p_buf = (uint8_t *)osi_malloc(2048);
            if (p_buf == NULL) {
                HIDD_TRACE_ERROR("%s: Buffer allocation failure for size = 2048 ", __func__);
                return HID_ERR_NOT_REGISTERED;
            }
            p = p_buf;
            UINT8_TO_BE_STREAM(p, (DATA_ELE_SEQ_DESC_TYPE << 3) | SIZE_IN_NEXT_BYTE);
            UINT8_TO_BE_STREAM(p, seq_len);
            UINT8_TO_BE_STREAM(p, (UINT_DESC_TYPE << 3) | SIZE_ONE_BYTE);
            UINT8_TO_BE_STREAM(p, cdt);
            UINT8_TO_BE_STREAM(p, (TEXT_STR_DESC_TYPE << 3) | SIZE_IN_NEXT_BYTE);
            UINT8_TO_BE_STREAM(p, desc_len);
            ARRAY_TO_BE_STREAM(p, p_desc_data, (int)desc_len);
            result &= SDP_AddAttribute(handle, ATTR_ID_HID_DESCRIPTOR_LIST, DATA_ELE_SEQ_DESC_TYPE, p - p_buf, p_buf);
            osi_free(p_buf);
        }
        {
            uint8_t lang_buf[8];
            p = lang_buf;
            uint8_t seq_len = 6;
            uint16_t lang_english = 0x0409;
            UINT8_TO_BE_STREAM(p, (DATA_ELE_SEQ_DESC_TYPE << 3) | SIZE_IN_NEXT_BYTE);
            UINT8_TO_BE_STREAM(p, seq_len);
            UINT8_TO_BE_STREAM(p, (UINT_DESC_TYPE << 3) | SIZE_TWO_BYTES);
            UINT16_TO_BE_STREAM(p, lang_english);
            UINT8_TO_BE_STREAM(p, (UINT_DESC_TYPE << 3) | SIZE_TWO_BYTES);
            UINT16_TO_BE_STREAM(p, LANGUAGE_BASE_ID);
            result &=
                SDP_AddAttribute(handle, ATTR_ID_HID_LANGUAGE_ID_BASE, DATA_ELE_SEQ_DESC_TYPE, p - lang_buf, lang_buf);
        }
        result &= SDP_AddAttribute(handle, ATTR_ID_HID_BATTERY_POWER, BOOLEAN_DESC_TYPE, 1, (uint8_t *)&bool_true);
        result &= SDP_AddAttribute(handle, ATTR_ID_HID_REMOTE_WAKE, BOOLEAN_DESC_TYPE, 1, (uint8_t *)&bool_false);
        result &=
            SDP_AddAttribute(handle, ATTR_ID_HID_NORMALLY_CONNECTABLE, BOOLEAN_DESC_TYPE, 1, (uint8_t *)&bool_true);
        result &= SDP_AddAttribute(handle, ATTR_ID_HID_BOOT_DEVICE, BOOLEAN_DESC_TYPE, 1, (uint8_t *)&bool_true);
        p = (uint8_t *)&temp;
        UINT16_TO_BE_STREAM(p, prof_ver);
        result &= SDP_AddAttribute(handle, ATTR_ID_HID_PROFILE_VERSION, UINT_DESC_TYPE, 2, (uint8_t *)&temp);
    }
    if (result) {
        uint16_t browse_group = UUID_SERVCLASS_PUBLIC_BROWSE_GROUP;
        result &= SDP_AddUuidSequence(handle, ATTR_ID_BROWSE_GROUP_LIST, 1, &browse_group);
    }
    if (!result) {
        HIDD_TRACE_ERROR("%s: failed to complete SDP record", __func__);
        return HID_ERR_NOT_REGISTERED;
    }
    return HID_SUCCESS;
}

/*******************************************************************************
 *
 * Function         HID_DevSendReport
 *
 * Description      Sends report
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
tHID_STATUS HID_DevSendReport(uint8_t channel, uint8_t type, uint8_t id, uint16_t len, uint8_t *p_data)
{
    HIDD_TRACE_VERBOSE("%s: channel=%d type=%d id=%d len=%d", __func__, channel, type, id, len);

    if (channel == HID_CHANNEL_CTRL) {
        return hidd_conn_send_data(HID_CHANNEL_CTRL, HID_TRANS_DATA, type, id, len, p_data);
    }

    if (channel == HID_CHANNEL_INTR && type == HID_PAR_REP_TYPE_INPUT) {
        // on INTR we can only send INPUT
        return hidd_conn_send_data(HID_CHANNEL_INTR, HID_TRANS_DATA, HID_PAR_REP_TYPE_INPUT, id, len, p_data);
    }

    return HID_ERR_INVALID_PARAM;
}

/*******************************************************************************
 *
 * Function         HID_DevVirtualCableUnplug
 *
 * Description      Sends Virtual Cable Unplug
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
tHID_STATUS HID_DevVirtualCableUnplug(void)
{
    HIDD_TRACE_API("%s", __func__);

    return hidd_conn_send_data(HID_CHANNEL_CTRL, HID_TRANS_CONTROL, HID_PAR_CONTROL_VIRTUAL_CABLE_UNPLUG, 0, 0, NULL);
}

/*******************************************************************************
 *
 * Function         HID_DevPlugDevice
 *
 * Description      Establishes virtual cable to given host
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
tHID_STATUS HID_DevPlugDevice(BD_ADDR addr)
{
    hd_cb.device.in_use = TRUE;
    memcpy(hd_cb.device.addr, addr, sizeof(BD_ADDR));

    return HID_SUCCESS;
}

/*******************************************************************************
 *
 * Function         HID_DevUnplugDevice
 *
 * Description      Unplugs virtual cable from given host
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
tHID_STATUS HID_DevUnplugDevice(BD_ADDR addr)
{
    if (!memcmp(hd_cb.device.addr, addr, sizeof(BD_ADDR))) {
        hd_cb.device.in_use = FALSE;
        hd_cb.device.conn.conn_state = HID_CONN_STATE_UNUSED;
        hd_cb.device.conn.ctrl_cid = 0;
        hd_cb.device.conn.intr_cid = 0;
    }
    return HID_SUCCESS;
}

/*******************************************************************************
 *
 * Function         HID_DevConnect
 *
 * Description      Connects to device
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
tHID_STATUS HID_DevConnect(void)
{
    if (!hd_cb.reg_flag) {
        return HID_ERR_NOT_REGISTERED;
    }
    if (!hd_cb.device.in_use) {
        return HID_ERR_INVALID_PARAM;
    }
    if (hd_cb.device.state != HIDD_DEV_NO_CONN) {
        return HID_ERR_ALREADY_CONN;
    }
    return hidd_conn_initiate();
}

/*******************************************************************************
 *
 * Function         HID_DevDisconnect
 *
 * Description      Disconnects from device
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
tHID_STATUS HID_DevDisconnect(void)
{
    if (!hd_cb.reg_flag) {
        return HID_ERR_NOT_REGISTERED;
    }
    if (!hd_cb.device.in_use) {
        return HID_ERR_INVALID_PARAM;
    }
    if (hd_cb.device.state == HIDD_DEV_NO_CONN) {
        return HID_ERR_NO_CONNECTION;
    }
    return hidd_conn_disconnect();
}

/*******************************************************************************
 *
 * Function         HID_DevSetIncomingPolicy
 *
 * Description      Sets policy for incoming connections (allowed/disallowed)
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
tHID_STATUS HID_DevSetIncomingPolicy(bool allow)
{
    hd_cb.allow_incoming = allow;
    return HID_SUCCESS;
}

/*******************************************************************************
 *
 * Function         HID_DevReportError
 *
 * Description      Reports error for Set Report via HANDSHAKE
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
tHID_STATUS HID_DevReportError(uint8_t error)
{
    uint8_t handshake_param;

    HIDD_TRACE_API("%s: error = %d", __func__, error);

    switch (error) {
    case HID_PAR_HANDSHAKE_RSP_SUCCESS:
    case HID_PAR_HANDSHAKE_RSP_NOT_READY:
    case HID_PAR_HANDSHAKE_RSP_ERR_INVALID_REP_ID:
    case HID_PAR_HANDSHAKE_RSP_ERR_UNSUPPORTED_REQ:
    case HID_PAR_HANDSHAKE_RSP_ERR_INVALID_PARAM:
    case HID_PAR_HANDSHAKE_RSP_ERR_UNKNOWN:
    case HID_PAR_HANDSHAKE_RSP_ERR_FATAL:
        handshake_param = error;
        break;
    default:
        handshake_param = HID_PAR_HANDSHAKE_RSP_ERR_UNKNOWN;
        break;
    }

    return hidd_conn_send_data(0, HID_TRANS_HANDSHAKE, handshake_param, 0, 0, NULL);
}

/*******************************************************************************
 *
 * Function         HID_DevGetDevice
 *
 * Description      Returns the BD Address of virtually cabled device
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
tHID_STATUS HID_DevGetDevice(BD_ADDR *addr)
{
    HIDD_TRACE_API("%s", __func__);

    if (hd_cb.device.in_use) {
        memcpy(addr, hd_cb.device.addr, sizeof(BD_ADDR));
    } else {
        return HID_ERR_NOT_REGISTERED;
    }

    return HID_SUCCESS;
}

/*******************************************************************************
 *
 * Function         HID_DevSetIncomingQos
 *
 * Description      Sets Incoming QoS values for Interrupt L2CAP Channel
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
tHID_STATUS HID_DevSetIncomingQos(uint8_t service_type, uint32_t token_rate, uint32_t token_bucket_size,
                                  uint32_t peak_bandwidth, uint32_t latency, uint32_t delay_variation)
{
    HIDD_TRACE_API("%s", __func__);
    hd_cb.use_in_qos = TRUE;
    hd_cb.in_qos.service_type = service_type;
    hd_cb.in_qos.token_rate = token_rate;
    hd_cb.in_qos.token_bucket_size = token_bucket_size;
    hd_cb.in_qos.peak_bandwidth = peak_bandwidth;
    hd_cb.in_qos.latency = latency;
    hd_cb.in_qos.delay_variation = delay_variation;
    return HID_SUCCESS;
}
/*******************************************************************************
 *
 * Function         HID_DevSetOutgoingQos
 *
 * Description      Sets Outgoing QoS values for Interrupt L2CAP Channel
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
tHID_STATUS HID_DevSetOutgoingQos(uint8_t service_type, uint32_t token_rate, uint32_t token_bucket_size,
                                  uint32_t peak_bandwidth, uint32_t latency, uint32_t delay_variation)
{
    HIDD_TRACE_API("%s", __func__);
    hd_cb.l2cap_intr_cfg.qos_present = TRUE;
    hd_cb.l2cap_intr_cfg.qos.service_type = service_type;
    hd_cb.l2cap_intr_cfg.qos.token_rate = token_rate;
    hd_cb.l2cap_intr_cfg.qos.token_bucket_size = token_bucket_size;
    hd_cb.l2cap_intr_cfg.qos.peak_bandwidth = peak_bandwidth;
    hd_cb.l2cap_intr_cfg.qos.latency = latency;
    hd_cb.l2cap_intr_cfg.qos.delay_variation = delay_variation;
    return HID_SUCCESS;
}
#endif
