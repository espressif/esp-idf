/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include "sysinit/sysinit.h"
#include "nimble/hci_common.h"
#if CONFIG_BT_NIMBLE_ENABLED
#include "host/ble_hs.h"
#endif //CONFIG_BT_NIMBLE_ENABLED
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "esp_nimble_hci.h"
#include "esp_nimble_mem.h"
#include "bt_osi_mem.h"
#if CONFIG_BT_CONTROLLER_ENABLED
#include "esp_bt.h"
#endif
#include "freertos/semphr.h"
#include "esp_compiler.h"
#include "soc/soc_caps.h"
#include "bt_common.h"
#include "hci_log/bt_hci_log.h"

#define NIMBLE_VHCI_TIMEOUT_MS  2000
#define BLE_HCI_EVENT_HDR_LEN               (2)
#define BLE_HCI_CMD_HDR_LEN                 (3)

static ble_hci_trans_rx_cmd_fn *ble_hci_rx_cmd_hs_cb;
static void *ble_hci_rx_cmd_hs_arg;

static ble_hci_trans_rx_acl_fn *ble_hci_rx_acl_hs_cb;
static void *ble_hci_rx_acl_hs_arg;

/*
 * The MBUF payload size must accommodate the HCI data header size plus the
 * maximum ACL data packet length. The ACL block size is the size of the
 * mbufs we will allocate.
 */
#define ACL_BLOCK_SIZE  OS_ALIGN(MYNEWT_VAL(BLE_TRANSPORT_ACL_SIZE) \
        + BLE_MBUF_MEMBLOCK_OVERHEAD \
        + BLE_HCI_DATA_HDR_SZ, OS_ALIGNMENT)



static SemaphoreHandle_t vhci_send_sem;
const static char *TAG = "NimBLE";

int os_msys_buf_alloc(void);
void os_msys_buf_free(void);
extern uint8_t ble_hs_enabled_state;

void ble_hci_trans_cfg_hs(ble_hci_trans_rx_cmd_fn *cmd_cb,
                          void *cmd_arg,
                          ble_hci_trans_rx_acl_fn *acl_cb,
                          void *acl_arg)
{
    ble_hci_rx_cmd_hs_cb = cmd_cb;
    ble_hci_rx_cmd_hs_arg = cmd_arg;
    ble_hci_rx_acl_hs_cb = acl_cb;
    ble_hci_rx_acl_hs_arg = acl_arg;
}

void esp_vhci_host_send_packet_wrapper(uint8_t *data, uint16_t len)
{
#if (BT_HCI_LOG_INCLUDED == TRUE)
    bt_hci_log_record_hci_data(data[0], &data[1], len - 1);
#endif
    esp_vhci_host_send_packet(data, len);
}

int ble_hci_trans_hs_cmd_tx(uint8_t *cmd)
{
    uint16_t len;
    uint8_t rc = 0;

    assert(cmd != NULL);
    *cmd = BLE_HCI_UART_H4_CMD;
    len = BLE_HCI_CMD_HDR_LEN + cmd[3] + 1;
    if (!esp_vhci_host_check_send_available()) {
        ESP_LOGD(TAG, "Controller not ready to receive packets");
    }

    if (xSemaphoreTake(vhci_send_sem, NIMBLE_VHCI_TIMEOUT_MS / portTICK_PERIOD_MS) == pdTRUE) {
        esp_vhci_host_send_packet_wrapper(cmd, len);
    } else {
        rc = BLE_HS_ETIMEOUT_HCI;
    }

    ble_transport_free(cmd);
    return rc;
}

int ble_hci_trans_ll_evt_tx(uint8_t *hci_ev)
{
    int rc = ESP_FAIL;

    rc = ble_transport_to_hs_evt((void *)hci_ev);
    return rc;
}

int ble_hci_trans_hs_acl_tx(struct os_mbuf *om)
{
    uint16_t len = 0;
    uint8_t data[MYNEWT_VAL(BLE_TRANSPORT_ACL_SIZE) + 1], rc = 0;
    /* If this packet is zero length, just free it */
    if (OS_MBUF_PKTLEN(om) == 0) {
        os_mbuf_free_chain(om);
        return 0;
    }
    data[0] = BLE_HCI_UART_H4_ACL;
    len++;

    if (!esp_vhci_host_check_send_available()) {
        ESP_LOGD(TAG, "Controller not ready to receive packets");
    }

    os_mbuf_copydata(om, 0, OS_MBUF_PKTLEN(om), &data[1]);
    len += OS_MBUF_PKTLEN(om);

    if (xSemaphoreTake(vhci_send_sem, NIMBLE_VHCI_TIMEOUT_MS / portTICK_PERIOD_MS) == pdTRUE) {
        esp_vhci_host_send_packet_wrapper(data, len);
    } else {
        rc = BLE_HS_ETIMEOUT_HCI;
    }

    os_mbuf_free_chain(om);

    return rc;
}

int ble_hci_trans_ll_acl_tx(struct os_mbuf *om)
{
    int rc = ESP_FAIL;

    rc = ble_transport_to_hs_acl((void *)om);
    return rc;
}



int ble_hci_trans_reset(void)
{
    /* No work to do.  All allocated buffers are owned by the host or
     * controller, and they will get freed by their owners.
     */
    return 0;
}


static void ble_hci_rx_acl(uint8_t *data, uint16_t len)
{
    struct os_mbuf *m = NULL;
    int rc;
    int sr;
    if (len < BLE_HCI_DATA_HDR_SZ || len > MYNEWT_VAL(BLE_TRANSPORT_ACL_SIZE)) {
        return;
    }

    do {
        m = ble_transport_alloc_acl_from_ll();

        if (!m) {
            ESP_LOGD(TAG,"Failed to allocate buffer, retrying ");
	    /* Give some time to free buffer and try again */
	    vTaskDelay(1);
	}
    }while(!m);

    if ((rc = os_mbuf_append(m, data, len)) != 0) {
        ESP_LOGE(TAG, "%s failed to os_mbuf_append; rc = %d", __func__, rc);
        os_mbuf_free_chain(m);
        return;
    }
    OS_ENTER_CRITICAL(sr);
    ble_transport_to_hs_acl(m);
    OS_EXIT_CRITICAL(sr);
}

/*
 * @brief: BT controller callback function, used to notify the upper layer that
 *         controller is ready to receive command
 */
static void controller_rcv_pkt_ready(void)
{
    if (vhci_send_sem) {
        xSemaphoreGive(vhci_send_sem);
    }
}

static void dummy_controller_rcv_pkt_ready(void)
{
  /* Dummy function */
}

void bt_record_hci_data(uint8_t *data, uint16_t len)
{
#if (BT_HCI_LOG_INCLUDED == TRUE)
    if ((data[0] == BLE_HCI_UART_H4_EVT) && (data[1] == BLE_HCI_EVCODE_LE_META) && ((data[3] ==  BLE_HCI_LE_SUBEV_ADV_RPT) || (data[3] == BLE_HCI_LE_SUBEV_DIRECT_ADV_RPT)
        || (data[3] == BLE_HCI_LE_SUBEV_EXT_ADV_RPT) || (data[3] == BLE_HCI_LE_SUBEV_PERIODIC_ADV_RPT))) {
        bt_hci_log_record_hci_adv(HCI_LOG_DATA_TYPE_ADV, &data[2], len - 2);
    } else {
        uint8_t data_type = ((data[0] == 2) ? HCI_LOG_DATA_TYPE_C2H_ACL : data[0]);
        bt_hci_log_record_hci_data(data_type, &data[1], len - 1);
    }
#endif // (BT_HCI_LOG_INCLUDED == TRUE)
}

static int dummy_host_rcv_pkt(uint8_t *data, uint16_t len)
{
    /* Dummy function */
    return 0;
}

/*
 * @brief: BT controller callback function, to transfer data packet to the host
 */
static int host_rcv_pkt(uint8_t *data, uint16_t len)
{
    bt_record_hci_data(data, len);

    if(!ble_hs_enabled_state) {
        /* If host is not enabled, drop the packet */
        ESP_LOGE(TAG, "Host not enabled. Dropping the packet!");
        return 0;
    }

    if (data[0] == BLE_HCI_UART_H4_EVT) {
        uint8_t *evbuf;
        int totlen;
        int rc;

        totlen = BLE_HCI_EVENT_HDR_LEN + data[2];
        assert(totlen <= UINT8_MAX + BLE_HCI_EVENT_HDR_LEN);

        if (totlen > MYNEWT_VAL(BLE_TRANSPORT_EVT_SIZE)) {
            ESP_LOGE(TAG, "Received HCI data length at host (%d) exceeds maximum configured HCI event buffer size (%d).",
                     totlen, MYNEWT_VAL(BLE_TRANSPORT_EVT_SIZE));
            ble_hs_sched_reset(BLE_HS_ECONTROLLER);
            return 0;
        }

        if (data[1] == BLE_HCI_EVCODE_HW_ERROR) {
            assert(0);
        }

        /* Allocate LE Advertising Report Event from lo pool only */
        if ((data[1] == BLE_HCI_EVCODE_LE_META) &&
                (data[3] == BLE_HCI_LE_SUBEV_ADV_RPT || data[3] == BLE_HCI_LE_SUBEV_EXT_ADV_RPT)) {
            evbuf = ble_transport_alloc_evt(1);
            /* Skip advertising report if we're out of memory */
            if (!evbuf) {
                return 0;
            }
        } else {
            evbuf = ble_transport_alloc_evt(0);
            assert(evbuf != NULL);
        }

        memset(evbuf, 0, sizeof *evbuf);
        memcpy(evbuf, &data[1], totlen);

        rc = ble_hci_trans_ll_evt_tx(evbuf);
        assert(rc == 0);
    } else if (data[0] == BLE_HCI_UART_H4_ACL) {
        ble_hci_rx_acl(data + 1, len - 1);
    }
    return 0;
}

static const esp_vhci_host_callback_t vhci_host_cb = {
    .notify_host_send_available = controller_rcv_pkt_ready,
    .notify_host_recv = host_rcv_pkt,
};

static const esp_vhci_host_callback_t dummy_vhci_host_cb = {
    .notify_host_send_available = dummy_controller_rcv_pkt_ready,
    .notify_host_recv = dummy_host_rcv_pkt,
};


extern void ble_transport_init(void);
extern esp_err_t ble_buf_alloc(void);
extern void ble_buf_free(void);
esp_err_t esp_nimble_hci_init(void)
{
    esp_err_t ret;

    ret = ble_buf_alloc();
    if (ret != ESP_OK) {
        goto err;
    }
    if ((ret = esp_vhci_host_register_callback(&vhci_host_cb)) != ESP_OK) {
        goto err;
    }

    ble_transport_init();

    vhci_send_sem = xSemaphoreCreateBinary();
    if (vhci_send_sem == NULL) {
        ret = ESP_ERR_NO_MEM;
        goto err;
    }

    xSemaphoreGive(vhci_send_sem);

#if MYNEWT_VAL(BLE_QUEUE_CONG_CHECK)
    ble_adv_list_init();
#endif

    return ret;
err:
    ble_buf_free();
    return ret;

}

extern void ble_transport_deinit(void);
esp_err_t esp_nimble_hci_deinit(void)
{
    if (vhci_send_sem) {
        /* Dummy take & give semaphore before deleting */
        xSemaphoreTake(vhci_send_sem, portMAX_DELAY);
        xSemaphoreGive(vhci_send_sem);
        vSemaphoreDelete(vhci_send_sem);
        vhci_send_sem = NULL;
    }
    ble_transport_deinit();

    esp_vhci_host_register_callback(&dummy_vhci_host_cb);

    ble_buf_free();

#if MYNEWT_VAL(BLE_QUEUE_CONG_CHECK)
    ble_adv_list_deinit();
#endif

    return ESP_OK;
}
