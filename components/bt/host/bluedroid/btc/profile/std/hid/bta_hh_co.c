#include "btc_hh.h"
#include "osi/allocator.h"
#include "string.h"
#if HID_HOST_INCLUDED == TRUE

/*******************************************************************************
 *
 * Function      bta_hh_co_open
 *
 * Description   When connection is opened, this call-out function is executed
 *               by HH to do platform specific initialization.
 *
 * Returns       void.
 ******************************************************************************/
void bta_hh_co_open(uint8_t dev_handle, uint8_t sub_class, tBTA_HH_ATTR_MASK attr_mask, uint8_t app_id)
{
    uint32_t i;
    btc_hh_device_t *p_dev = NULL;

    if (dev_handle == BTA_HH_INVALID_HANDLE) {
        APPL_TRACE_WARNING("%s: Oops, dev_handle (%d) is invalid...", __func__, dev_handle);
        return;
    }

    for (i = 0; i < BTC_HH_MAX_HID; i++) {
        p_dev = &btc_hh_cb.devices[i];
        if (p_dev->dev_status != ESP_HIDH_CONN_STATE_UNKNOWN && p_dev->dev_handle == dev_handle) {
            // We found a device with the same handle. Must be a device reconnected.
            APPL_TRACE_WARNING("%s: Found an existing device with the same handle dev_status=%d, "
                               "dev_handle=0x%2x, attr_mask=0x%04x, sub_class=0x%02x, app_id=%d",
                               __func__, p_dev->dev_status, dev_handle, p_dev->attr_mask, p_dev->sub_class,
                               p_dev->app_id);
            break;
        }
        p_dev = NULL;
    }

    if (p_dev == NULL) {
        // Did not find a device reconnection case. Find an empty slot now.
        for (i = 0; i < BTC_HH_MAX_HID; i++) {
            if (btc_hh_cb.devices[i].dev_status == ESP_HIDH_CONN_STATE_UNKNOWN) {
                p_dev = &btc_hh_cb.devices[i];
                p_dev->dev_handle = dev_handle;
                p_dev->attr_mask = attr_mask;
                p_dev->sub_class = sub_class;
                p_dev->app_id = app_id;
                p_dev->local_vup = false;

                btc_hh_cb.device_num++;
                break;
            }
        }
    }

    if (p_dev == NULL) {
        APPL_TRACE_ERROR("%s: Error: too many HID devices are connected", __func__);
        return;
    }

    p_dev->dev_status = ESP_HIDH_CONN_STATE_CONNECTED;
    APPL_TRACE_DEBUG("%s: Return device status %d", __func__, p_dev->dev_status);
}

/*******************************************************************************
 *
 * Function      bta_hh_co_close
 *
 * Description   When connection is closed, this call-out function is executed
 *               by HH to do platform specific finalization.
 *
 * Parameters    dev_handle  - device handle
 *                  app_id      - application id
 *
 * Returns          void.
 ******************************************************************************/
void bta_hh_co_close(uint8_t dev_handle, uint8_t app_id)
{
    uint32_t i;
    btc_hh_device_t *p_dev = NULL;

    APPL_TRACE_WARNING("%s: dev_handle = %d, app_id = %d", __func__, dev_handle, app_id);
    if (dev_handle == BTA_HH_INVALID_HANDLE) {
        APPL_TRACE_WARNING("%s: Oops, dev_handle (%d) is invalid...", __func__, dev_handle);
        return;
    }

    for (i = 0; i < BTC_HH_MAX_HID; i++) {
        p_dev = &btc_hh_cb.devices[i];
        if (p_dev->dev_status != ESP_HIDH_CONN_STATE_UNKNOWN && p_dev->dev_handle == dev_handle) {
            APPL_TRACE_WARNING("%s: Found an existing device with the same handle "
                               "dev_status = %d, dev_handle =%d",
                               __func__, p_dev->dev_status, p_dev->dev_handle);
            break;
        }
    }
}

/*******************************************************************************
 *
 * Function         bta_hh_co_data
 *
 * Description      This function is executed by BTA when HID host receive a
 *                  data report on interrupt channel.
 *
 * Parameters       dev_handle  - device handle
 *                  *p_rpt      - pointer to the report data
 *                  len         - length of report data
 *                  mode        - Hid host Protocol Mode
 *                  sub_clas    - Device Subclass
 *                  app_id      - application id
 *
 * Returns          void
 ******************************************************************************/
void bta_hh_co_data(UINT8 dev_handle, UINT8 *p_rpt, UINT16 len, tBTA_HH_PROTO_MODE mode, UINT8 sub_class, UINT8 ctry_code,
               BD_ADDR peer_addr, UINT8 app_id)
{
    btc_msg_t msg;
    tBTA_HH p_data;
    BT_HDR *p_buf = NULL;
    bt_status_t status;
    tBTA_HH_STATUS ret = BTA_HH_OK;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_HH;
    msg.act = BTA_HH_DATA_IND_EVT;

    APPL_TRACE_DEBUG("%s: dev_handle = %d, subclass = 0x%02X, mode = %d, "
                     "ctry_code = %d, app_id = %d",
                     __func__, dev_handle, sub_class, mode, ctry_code, app_id);

    do {
        if ((p_buf = osi_malloc(sizeof(BT_HDR) + len)) == NULL) {
            APPL_TRACE_ERROR("%s malloc failed!", __func__);
            ret = BTA_HH_ERR_NO_RES;
            break;
        }
        p_buf->offset = 0;
        p_buf->len = len;
        p_buf->event = 0;
        p_buf->layer_specific = dev_handle;
        memcpy(p_buf->data, p_rpt, len);
    } while (0);

    p_data.int_data.status = ret;
    p_data.int_data.handle = dev_handle;
    p_data.int_data.p_data = p_buf;
    p_data.int_data.proto_mode = mode;
    status = btc_transfer_context(&msg, &p_data, sizeof(tBTA_HH), NULL, NULL);
    assert(status == BT_STATUS_SUCCESS);
}

#endif /* HID_HOST_INCLUDED == TRUE */
