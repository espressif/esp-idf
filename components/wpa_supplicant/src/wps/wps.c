/*
 * Wi-Fi Protected Setup
 * Copyright (c) 2007-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
#include <string.h>

#include "utils/includes.h"
#include "rsn_supp/wpa.h"
#include "utils/common.h"
#include "common/eapol_common.h"
#include "utils/wpa_debug.h"
#include "common/ieee802_11_defs.h"

#include "wps/wps_i.h"
#include "wps/wps_dev_attr.h"

#include "eap_peer/eap_defs.h"
#include "eap_peer/eap_common.h"


/**
 * wps_process_msg - Process a WPS message
 * @wps: WPS Registration protocol data from wps_init()
 * @op_code: Message OP Code
 * @msg: Message data
 * Returns: Processing result
 *
 * This function is used to process WPS messages with OP Codes WSC_ACK,
 * WSC_NACK, WSC_MSG, and WSC_Done. The caller (e.g., EAP server/peer) is
 * responsible for reassembling the messages before calling this function.
 * Response to this message is built by calling wps_get_msg().
 */
enum wps_process_res wps_process_msg(struct wps_data *wps,
                     enum wsc_op_code op_code,
                     const struct wpabuf *msg)
{
    if (wps->registrar)
        return wps_registrar_process_msg(wps, op_code, msg);
    else
        return wps_enrollee_process_msg(wps, op_code, msg);
}


/**
 * wps_get_msg - Build a WPS message
 * @wps: WPS Registration protocol data from wps_init()
 * @op_code: Buffer for returning message OP Code
 * Returns: The generated WPS message or %NULL on failure
 *
 * This function is used to build a response to a message processed by calling
 * wps_process_msg(). The caller is responsible for freeing the buffer.
 */
struct wpabuf * wps_get_msg(struct wps_data *wps, enum wsc_op_code *op_code)
{
    if (wps->registrar)
        return wps_registrar_get_msg(wps, op_code);
    else
        return wps_enrollee_get_msg(wps, op_code);
}


/**
 * wps_is_selected_pbc_registrar - Check whether WPS IE indicates active PBC
 * @msg: WPS IE contents from Beacon or Probe Response frame
 * Returns: 1 if PBC Registrar is active, 0 if not
 */
int wps_is_selected_pbc_registrar(const struct wpabuf *msg)
{
    struct wps_parse_attr *attr = (struct wps_parse_attr *)os_zalloc(sizeof(struct wps_parse_attr));

    if (!attr)
        return 0;

    /*
     * In theory, this could also verify that attr.sel_reg_config_methods
     * includes WPS_CONFIG_PUSHBUTTON, but some deployed AP implementations
     * do not set Selected Registrar Config Methods attribute properly, so
     * it is safer to just use Device Password ID here.
     */

    if (wps_parse_msg(msg, attr) < 0 ||
        !attr->selected_registrar || *attr->selected_registrar == 0 ||
        !attr->dev_password_id ||
        WPA_GET_BE16(attr->dev_password_id) != DEV_PW_PUSHBUTTON) {
            os_free(attr);
            return 0;
    }


#ifdef CONFIG_WPS_STRICT
    if (!attr->sel_reg_config_methods ||
        !(WPA_GET_BE16(attr->sel_reg_config_methods) &
        WPS_CONFIG_PUSHBUTTON)) {
            os_free(attr);
            return 0;
    }
#endif /* CONFIG_WPS_STRICT */

    os_free(attr);
    return 1;
}

#ifdef CONFIG_WPS_PIN

static int is_selected_pin_registrar(struct wps_parse_attr *attr)
{
    /*
     * In theory, this could also verify that attr.sel_reg_config_methods
     * includes WPS_CONFIG_LABEL, WPS_CONFIG_DISPLAY, or WPS_CONFIG_KEYPAD,
     * but some deployed AP implementations do not set Selected Registrar
     * Config Methods attribute properly, so it is safer to just use
     * Device Password ID here.
     */

    if (!attr->selected_registrar || *attr->selected_registrar == 0) {
        return 0;
    }
    if (attr->dev_password_id != NULL &&
        WPA_GET_BE16(attr->dev_password_id) == DEV_PW_PUSHBUTTON) {
        return 0;
    }
#ifdef CONFIG_WPS_STRICT
    if (!attr->sel_reg_config_methods ||
        !(WPA_GET_BE16(attr->sel_reg_config_methods) &
        (WPS_CONFIG_LABEL | WPS_CONFIG_DISPLAY | WPS_CONFIG_KEYPAD)))
        return 0;
#endif /* CONFIG_WPS_STRICT */

    return 1;
}


/**
 * wps_is_selected_pin_registrar - Check whether WPS IE indicates active PIN
 * @msg: WPS IE contents from Beacon or Probe Response frame
 * Returns: 1 if PIN Registrar is active, 0 if not
 */
int wps_is_selected_pin_registrar(const struct wpabuf *msg)
{
    struct wps_parse_attr *attr;
    int ret;

    attr = (struct wps_parse_attr *)os_zalloc(sizeof(struct wps_parse_attr));
    if (attr == NULL)
        return -99;

    if (wps_parse_msg(msg, attr) < 0) {
        os_free(attr);
        return 0;
    }

    ret = is_selected_pin_registrar(attr);
    os_free(attr);

    return ret;
}
#endif

/**
 * wps_is_addr_authorized - Check whether WPS IE authorizes MAC address
 * @msg: WPS IE contents from Beacon or Probe Response frame
 * @addr: MAC address to search for
 * @ver1_compat: Whether to use version 1 compatibility mode
 * Returns: 2 if the specified address is explicit authorized, 1 if address is
 * authorized (broadcast), 0 if not
 */
int wps_is_addr_authorized(const struct wpabuf *msg, const u8 *addr,
               int ver1_compat)
{
    struct wps_parse_attr *attr;
    int ret = 0;
    unsigned int i;
    const u8 *pos;
    const u8 bcast[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    attr = (struct wps_parse_attr *)os_zalloc(sizeof(struct wps_parse_attr));
    if (attr == NULL) {
        ret = 0;
        goto _out;
    }

    if (wps_parse_msg(msg, attr) < 0) {
        ret = 0;
        goto _out;
    }

    if (!attr->version2 && ver1_compat) {
        /*
         * Version 1.0 AP - AuthorizedMACs not used, so revert back to
         * old mechanism of using SelectedRegistrar.
         */
#ifdef CONFIG_WPS_PIN

        ret = is_selected_pin_registrar(attr);
        goto _out;
#endif
    }

    if (!attr->authorized_macs) {
        ret = 0;
        goto _out;
    }

    pos = attr->authorized_macs;
    for (i = 0; i < attr->authorized_macs_len / ETH_ALEN; i++) {
        if (os_memcmp(pos, addr, ETH_ALEN) == 0) {
            ret = 2;
            goto _out;
        }
        if (os_memcmp(pos, bcast, ETH_ALEN) == 0) {
            ret = 1;
            goto _out;
        }
        pos += ETH_ALEN;
    }
_out:
    if (attr)
        os_free(attr);

    return ret;
}


/**
 * wps_ap_priority_compar - Prioritize WPS IE from two APs
 * @wps_a: WPS IE contents from Beacon or Probe Response frame
 * @wps_b: WPS IE contents from Beacon or Probe Response frame
 * Returns: 1 if wps_b is considered more likely selection for WPS
 * provisioning, -1 if wps_a is considered more like, or 0 if no preference
 */
int wps_ap_priority_compar(const struct wpabuf *wps_a,
                           const struct wpabuf *wps_b)
{
    struct wps_parse_attr *attr = NULL;
    int sel_a, sel_b;
    int ret = 0; /* No preference */

    attr = os_zalloc(sizeof(*attr));

    if (!attr)
	    return ret;

    if (wps_a == NULL || wps_parse_msg(wps_a, attr) < 0) {
        ret = 1;
        goto exit;
    }
    sel_a = attr->selected_registrar && *(attr->selected_registrar) != 0;

    if (wps_b == NULL || wps_parse_msg(wps_b, attr) < 0) {
        ret = -1;
        goto exit;
    }
    sel_b = attr->selected_registrar && *(attr->selected_registrar) != 0;

    if (sel_a && !sel_b) {
        ret = -1;
        goto exit;
    }
    if (!sel_a && sel_b) {
        ret = 1;
        goto exit;
    }

exit:
    os_free(attr);
    return ret;
}


/**
 * wps_get_uuid_e - Get UUID-E from WPS IE
 * @msg: WPS IE contents from Beacon or Probe Response frame
 * Returns: Pointer to UUID-E or %NULL if not included
 *
 * The returned pointer is to the msg contents and it remains valid only as
 * long as the msg buffer is valid.
 */
const u8 * wps_get_uuid_e(const struct wpabuf *msg)
{
    struct wps_parse_attr *attr;
    const u8 *uuid_e;

    attr = (struct wps_parse_attr *)os_zalloc(sizeof(struct wps_parse_attr));
    if (attr == NULL)
        return NULL;

    if (wps_parse_msg(msg, attr) < 0) {
        uuid_e = NULL;
    } else {
        uuid_e = attr->uuid_e;
    }
    os_free(attr);
    return uuid_e;
}


/**
 * wps_is_20 - Check whether WPS attributes claim support for WPS 2.0
 */
int wps_is_20(const struct wpabuf *msg)
{
    struct wps_parse_attr *attr;
    int ret;

    attr = (struct wps_parse_attr *)os_zalloc(sizeof(struct wps_parse_attr));
    if (attr == NULL)
        return 0;

    if (msg == NULL || wps_parse_msg(msg, attr) < 0) {
        ret = 0;
    } else {
        ret = (attr->version2 != NULL);
    }
    os_free(attr);
    return ret;
}


/**
 * wps_build_assoc_req_ie - Build WPS IE for (Re)Association Request
 * @req_type: Value for Request Type attribute
 * Returns: WPS IE or %NULL on failure
 *
 * The caller is responsible for freeing the buffer.
 */
struct wpabuf * wps_build_assoc_req_ie(enum wps_request_type req_type)
{
    struct wpabuf *ie;
    u8 *len;

    wpa_printf(MSG_DEBUG,  "WPS: Building WPS IE for (Re)Association "
           "Request");
    ie = wpabuf_alloc(100);
    if (ie == NULL)
        return NULL;

    wpabuf_put_u8(ie, WLAN_EID_VENDOR_SPECIFIC);
    len = wpabuf_put(ie, 1);
    wpabuf_put_be32(ie, WPS_DEV_OUI_WFA);

    if (wps_build_version(ie) ||
        wps_build_req_type(ie, req_type) ||
        wps_build_wfa_ext(ie, 0, NULL, 0)) {
        wpabuf_free(ie);
        return NULL;
    }

    *len = wpabuf_len(ie) - 2;

    return ie;
}


/**
 * wps_build_assoc_resp_ie - Build WPS IE for (Re)Association Response
 * Returns: WPS IE or %NULL on failure
 *
 * The caller is responsible for freeing the buffer.
 */
struct wpabuf * wps_build_assoc_resp_ie(void)
{
    struct wpabuf *ie;
    u8 *len;

    wpa_printf(MSG_DEBUG,  "WPS: Building WPS IE for (Re)Association "
           "Response");
    ie = wpabuf_alloc(100);
    if (ie == NULL)
        return NULL;

    wpabuf_put_u8(ie, WLAN_EID_VENDOR_SPECIFIC);
    len = wpabuf_put(ie, 1);
    wpabuf_put_be32(ie, WPS_DEV_OUI_WFA);

    if (wps_build_version(ie) ||
        wps_build_resp_type(ie, WPS_RESP_AP) ||
        wps_build_wfa_ext(ie, 0, NULL, 0)) {
        wpabuf_free(ie);
        return NULL;
    }

    *len = wpabuf_len(ie) - 2;

    return ie;
}


/**
 * wps_build_probe_req_ie - Build WPS IE for Probe Request
 * @pw_id: Password ID (DEV_PW_PUSHBUTTON for active PBC and DEV_PW_DEFAULT for
 * most other use cases)
 * @dev: Device attributes
 * @uuid: Own UUID
 * @req_type: Value for Request Type attribute
 * @num_req_dev_types: Number of requested device types
 * @req_dev_types: Requested device types (8 * num_req_dev_types octets) or
 *    %NULL if none
 * Returns: WPS IE or %NULL on failure
 *
 * The caller is responsible for freeing the buffer.
 */
struct wpabuf * wps_build_probe_req_ie(u16 pw_id, struct wps_device_data *dev,
                       const u8 *uuid,
                       enum wps_request_type req_type,
                       unsigned int num_req_dev_types,
                       const u8 *req_dev_types)
{
    struct wpabuf *ie;

    wpa_printf(MSG_DEBUG,  "WPS: Building WPS IE for Probe Request\n");

    ie = wpabuf_alloc(400);
    if (ie == NULL) {
        wpa_printf(MSG_ERROR, "WPS: ie alloc failed.");
        return NULL;
    }

    if (wps_build_version(ie) ||
        wps_build_req_type(ie, req_type) ||
        wps_build_config_methods(ie, dev->config_methods) ||
        wps_build_uuid_e(ie, uuid) ||
        wps_build_primary_dev_type(dev, ie) ||
        wps_build_rf_bands(dev, ie) ||
        wps_build_assoc_state(NULL, ie) ||
        wps_build_config_error(ie, WPS_CFG_NO_ERROR) ||
        wps_build_dev_password_id(ie, pw_id) ||
#ifdef CONFIG_WPS2
        wps_build_manufacturer(dev, ie) ||
        wps_build_model_name(dev, ie) ||
        wps_build_model_number(dev, ie) ||
        wps_build_dev_name(dev, ie) ||
        wps_build_wfa_ext(ie, req_type == WPS_REQ_ENROLLEE, NULL, 0) ||
#endif /* CONFIG_WPS2 */
        wps_build_req_dev_type(dev, ie, num_req_dev_types, req_dev_types)
        ||
        wps_build_secondary_dev_type(dev, ie)
        ) {
        wpabuf_free(ie);
        return NULL;
    }

#ifndef CONFIG_WPS2
    if (dev->p2p && wps_build_dev_name(dev, ie)) {
        wpabuf_free(ie);
        return NULL;
    }
#endif /* CONFIG_WPS2 */

    return wps_ie_encapsulate(ie);
}

#ifdef CONFIG_WPS_UPNP

void wps_free_pending_msgs(struct upnp_pending_message *msgs)
{
    struct upnp_pending_message *p, *prev;
    p = msgs;
    while (p) {
        prev = p;
        p = p->next;
        wpabuf_free(prev->msg);
        os_free(prev);
    }
}

#endif

int wps_attr_text(struct wpabuf *data, char *buf, char *end)
{
    struct wps_parse_attr *attr;
    char *pos = buf;
    int ret;

    attr = (struct wps_parse_attr *)os_zalloc(sizeof(struct wps_parse_attr));
    if (attr == NULL)
        return -99;

    if (wps_parse_msg(data, attr) < 0) {
        ret = -1;
        goto _out;
    }

    if (attr->wps_state) {
        if (*attr->wps_state == WPS_STATE_NOT_CONFIGURED)
            ret = snprintf(pos, end - pos,
                      "wps_state=unconfigured\n");
        else if (*attr->wps_state == WPS_STATE_CONFIGURED)
            ret = snprintf(pos, end - pos,
                      "wps_state=configured\n");
        else
            ret = 0;
        if (ret < 0 || ret >= end - pos) {
            ret = pos - buf;
            goto _out;
        }
        pos += ret;
    }

    if (attr->ap_setup_locked && *attr->ap_setup_locked) {
        ret = snprintf(pos, end - pos,
                  "wps_ap_setup_locked=1\n");
        if (ret < 0 || ret >= end - pos) {
            ret = pos - buf;
            goto _out;
        }
        pos += ret;
    }

    if (attr->selected_registrar && *attr->selected_registrar) {
        ret = snprintf(pos, end - pos,
                  "wps_selected_registrar=1\n");
        if (ret < 0 || ret >= end - pos) {
            ret = pos - buf;
            goto _out;
        }
        pos += ret;
    }

    if (attr->dev_password_id) {
        ret = snprintf(pos, end - pos,
                  "wps_device_password_id=%u\n",
                  WPA_GET_BE16(attr->dev_password_id));
        if (ret < 0 || ret >= end - pos) {
            ret = pos - buf;
            goto _out;
        }
        pos += ret;
    }

    if (attr->sel_reg_config_methods) {
        ret = snprintf(pos, end - pos,
                  "wps_selected_registrar_config_methods="
                  "0x%04x\n",
                  WPA_GET_BE16(attr->sel_reg_config_methods));
        if (ret < 0 || ret >= end - pos) {
            ret = pos - buf;
            goto _out;
        }
        pos += ret;
    }

    if (attr->primary_dev_type) {
        char devtype[WPS_DEV_TYPE_BUFSIZE];
        ret = snprintf(pos, end - pos,
                  "wps_primary_device_type=%s\n",
                  wps_dev_type_bin2str(attr->primary_dev_type,
                               devtype,
                               sizeof(devtype)));
        if (ret < 0 || ret >= end - pos) {
            ret = pos - buf;
            goto _out;
        }
        pos += ret;
    }

    if (attr->dev_name) {
        char *str = (char *)os_malloc(attr->dev_name_len + 1);
        size_t i;
        if (str == NULL) {
            ret = pos - buf;
            goto _out;
        }
        for (i = 0; i < attr->dev_name_len; i++) {
            if (attr->dev_name[i] < 32)
                str[i] = '_';
            else
                str[i] = attr->dev_name[i];
        }
        str[i] = '\0';
        ret = snprintf(pos, end - pos, "wps_device_name=%s\n", str);
        os_free(str);
        if (ret < 0 || ret >= end - pos) {
            ret = pos - buf;
            goto _out;
        }
        pos += ret;
    }

    if (attr->config_methods) {
        ret = snprintf(pos, end - pos,
                  "wps_config_methods=0x%04x\n",
                  WPA_GET_BE16(attr->config_methods));
        if (ret < 0 || ret >= end - pos) {
            ret = pos - buf;
            goto _out;
        }
        pos += ret;
    }

    ret = pos - buf;
_out:
    if (attr)
        os_free(attr);
    return ret;
}
