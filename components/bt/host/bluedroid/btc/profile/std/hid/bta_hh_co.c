#include <string.h>
#include "btc_hh.h"
#include "bta/utl.h"
#include "osi/allocator.h"
#if HID_HOST_INCLUDED == TRUE

void bta_hh_co_data_pkt_free(tBTA_HH_DATA_PKT *pkt)
{
    if (pkt == NULL || pkt->p_buf == NULL) {
        return;
    }

    if (pkt->buf_type == BTA_HH_DATA_BUF_BT_HDR) {
        utl_freebuf((void **)&pkt->p_buf);
    } else {
        osi_free(pkt->p_buf);
    }
    pkt->p_buf = NULL;
}

void bta_hh_co_data_linked_pkt_free(pkt_linked_item_t *item)
{
    if (item == NULL) {
        return;
    }

    bta_hh_co_data_pkt_free((tBTA_HH_DATA_PKT *)item->data);
    osi_free(item);
}

UINT8 *bta_hh_co_data_pkt_get_payload(tBTA_HH_DATA_PKT *pkt)
{
    BT_HDR *hdr;

    if (pkt == NULL || pkt->p_buf == NULL || pkt->len == 0) {
        return NULL;
    }

    if (pkt->buf_type == BTA_HH_DATA_BUF_BT_HDR) {
        hdr = (BT_HDR *)pkt->p_buf;
        return hdr->data + hdr->offset;
    }

    return (UINT8 *)pkt->p_buf;
}

/*******************************************************************************
 *
 * Function      bta_hh_co_open
 *
 * Description   When connection is opened, this call-out function is executed
 *               by HH to do platform specific initialization.
 *
 * Returns       TRUE if platform specific initialization succeeds.
 ******************************************************************************/
BOOLEAN bta_hh_co_open(UINT8 dev_handle, UINT8 sub_class, tBTA_HH_ATTR_MASK attr_mask, UINT8 app_id)
{
    BOOLEAN is_new_device = TRUE;
    uint8_t old_dev_status = ESP_HIDH_CONN_STATE_UNKNOWN;
    UINT32 i;
    btc_hh_device_t *p_dev = NULL;

    if (dev_handle == BTA_HH_INVALID_HANDLE) {
        APPL_TRACE_WARNING("%s: Oops, dev_handle (%d) is invalid...", __func__, dev_handle);
        return FALSE;
    }

    for (i = 0; i < BTC_HH_MAX_HID; i++) {
        p_dev = &btc_hh_cb.devices[i];
        if (p_dev->dev_status != ESP_HIDH_CONN_STATE_UNKNOWN && p_dev->dev_handle == dev_handle) {
            // We found a device with the same handle. Must be a device reconnected.
            APPL_TRACE_WARNING("%s: Found an existing device with the same handle dev_status=%d, "
                               "dev_handle=0x%2x, attr_mask=0x%04x, sub_class=0x%02x, app_id=%d",
                               __func__, p_dev->dev_status, dev_handle, p_dev->attr_mask, p_dev->sub_class,
                               p_dev->app_id);
            is_new_device = FALSE;
            old_dev_status = p_dev->dev_status;
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
        return FALSE;
    }

    p_dev->dev_status = ESP_HIDH_CONN_STATE_CONNECTED;
    if (!btc_hh_data_path_init(dev_handle)) {
        APPL_TRACE_ERROR("%s: failed to init HID host datapath, dev_handle=%u", __func__, dev_handle);
        if (is_new_device) {
            p_dev->dev_status = ESP_HIDH_CONN_STATE_UNKNOWN;
            p_dev->dev_handle = BTA_HH_INVALID_HANDLE;
            if (btc_hh_cb.device_num) {
                btc_hh_cb.device_num--;
            }
        } else {
            p_dev->dev_status = old_dev_status;
        }
        return FALSE;
    }

    APPL_TRACE_DEBUG("%s: Return device status %d", __func__, p_dev->dev_status);
    return TRUE;
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
void bta_hh_co_close(UINT8 dev_handle, UINT8 app_id)
{
    UINT32 i;
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

    // data path will be cleaned up on CLOSE or UNPLUG event in btc context
}

/*******************************************************************************
 *
 * Function         bta_hh_co_data_hdr
 *
 * Description      Transfer BT_HDR ownership to BTC queue (BR/EDR zero-copy).
 *
 * Returns          void
 ******************************************************************************/
void bta_hh_co_data_hdr(UINT8 dev_handle, BT_HDR *p_hdr, tBTA_HH_PROTO_MODE mode, UINT8 sub_class, UINT8 ctry_code,
                        BD_ADDR peer_addr, UINT8 app_id)
{
    pkt_linked_item_t *linked_pkt = NULL;
    tBTA_HH_DATA_PKT *pkt;

    APPL_TRACE_DEBUG("%s: dev_handle = %d, subclass = 0x%02X, mode = %d, "
                     "ctry_code = %d, app_id = %d",
                     __func__, dev_handle, sub_class, mode, ctry_code, app_id);

    if (p_hdr == NULL || p_hdr->len == 0) {
        utl_freebuf((void **)&p_hdr);
        return;
    }

    linked_pkt = btc_hh_data_dequeue_reusable_linked_pkt(dev_handle);
    if (linked_pkt != NULL) {
        bta_hh_co_data_pkt_free((tBTA_HH_DATA_PKT *)linked_pkt->data);
    } else {
        linked_pkt = (pkt_linked_item_t *)osi_malloc(BT_PKT_LINKED_HDR_SIZE +
                        sizeof(tBTA_HH_DATA_PKT));
        if (linked_pkt == NULL) {
            btc_hh_on_pkt_dropped(dev_handle);
            utl_freebuf((void **)&p_hdr);
            return;
        }
    }

    pkt = (tBTA_HH_DATA_PKT *)linked_pkt->data;
    pkt->dev_handle = dev_handle;
    pkt->len = p_hdr->len;
    pkt->proto_mode = mode;
    pkt->buf_type = BTA_HH_DATA_BUF_BT_HDR;
    pkt->p_buf = p_hdr;

    if (!btc_hh_data_enqueue_linked_pkt(linked_pkt)) {
        btc_hh_on_pkt_dropped(dev_handle);
        bta_hh_co_data_linked_pkt_free(linked_pkt);
    }
}
#endif /* HID_HOST_INCLUDED == TRUE */

#if (BLE_INCLUDED == TRUE && BTA_HH_LE_INCLUDED == TRUE)
/*******************************************************************************
 *
 * Function         bta_hh_le_co_data_owned
 *
 * Description      Transfer raw buffer ownership to BTC queue (BLE zero-copy).
 *
 * Returns          void
 ******************************************************************************/
void bta_hh_le_co_data_owned(UINT8 dev_handle, UINT8 *p_buf, UINT16 len, tBTA_HH_PROTO_MODE mode, UINT8 sub_class,
                             UINT8 ctry_code, BD_ADDR peer_addr, UINT8 app_id)
{
    tBTA_HH_DATA_PKT pkt = {
        .dev_handle = dev_handle,
        .len = len,
        .proto_mode = mode,
        .buf_type = BTA_HH_DATA_BUF_RAW,
        .p_buf = p_buf,
    };

    APPL_TRACE_DEBUG("%s: dev_handle = %d, subclass = 0x%02X, mode = %d, "
                     "ctry_code = %d, app_id = %d",
                     __func__, dev_handle, sub_class, mode, ctry_code, app_id);

    if ((p_buf == NULL) || (len == 0)) {
        osi_free(p_buf);
        return;
    }

    if (!btc_hh_data_enqueue_pkt(&pkt)) {
        btc_hh_on_pkt_dropped(dev_handle);
        osi_free(p_buf);
    }
}

/*******************************************************************************
 *
 * Function         bta_hh_le_co_data
 *
 * Description      Copy ephemeral report data into BTC queue (BLE GATT notify).
 *
 * Returns          void
 ******************************************************************************/
void bta_hh_le_co_data(UINT8 dev_handle, UINT8 *p_rpt, UINT16 len, tBTA_HH_PROTO_MODE mode, UINT8 sub_class,
                       UINT8 ctry_code, BD_ADDR peer_addr, UINT8 app_id)
{
    UINT8 *p_buf;
    tBTA_HH_DATA_PKT pkt;

    APPL_TRACE_DEBUG("%s: dev_handle = %d, subclass = 0x%02X, mode = %d, "
                     "ctry_code = %d, app_id = %d",
                     __func__, dev_handle, sub_class, mode, ctry_code, app_id);

    if ((p_rpt == NULL) || (len == 0)) {
        return;
    }

    p_buf = (UINT8 *)osi_malloc(len);
    if (p_buf == NULL) {
        btc_hh_on_pkt_dropped(dev_handle);
        return;
    }
    memcpy(p_buf, p_rpt, len);

    pkt.dev_handle = dev_handle;
    pkt.len = len;
    pkt.proto_mode = mode;
    pkt.buf_type = BTA_HH_DATA_BUF_RAW;
    pkt.p_buf = p_buf;

    if (!btc_hh_data_enqueue_pkt(&pkt)) {
        btc_hh_on_pkt_dropped(dev_handle);
        osi_free(p_buf);
    }
}
#endif /* (BLE_INCLUDED == TRUE && BTA_HH_LE_INCLUDED == TRUE) */
