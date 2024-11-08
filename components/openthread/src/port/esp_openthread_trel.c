/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_err.h"
#include "esp_netif_types.h"
#include "lwip/ip6_addr.h"
#include "sdkconfig.h"
#include "common/code_utils.hpp"
#include "openthread/error.h"
#include "esp_check.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "mdns.h"
#include "esp_netif_ip_addr.h"
#include "esp_openthread.h"
#include "esp_openthread_border_router.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_radio.h"
#include "esp_openthread_task_queue.h"
#include "lwip/pbuf.h"
#include "lwip/tcpip.h"
#include "lwip/udp.h"
#include "openthread/trel.h"
#include "openthread/platform/diag.h"

static esp_netif_t *s_trel_netif = NULL;
static otPlatTrelCounters s_trel_counters;

#define TREL_MDNS_TYPE "_trel"
#define TREL_MDNS_PROTO "_udp"

typedef struct {
    uint16_t port;
    struct udp_pcb *trel_pcb;
} ot_trel_t;

typedef struct {
    struct pbuf *p;
} ot_trel_recv_task_t;

typedef struct {
    struct udp_pcb *pcb;
    const uint8_t *payload;
    uint16_t length;
    ip_addr_t peer_addr;
    uint16_t peer_port;
} ot_trel_send_task_t;

static ot_trel_t s_ot_trel = {CONFIG_OPENTHREAD_TREL_PORT, NULL};
static bool s_is_service_registered = false;

static void trel_browse_notifier(mdns_result_t *result)
{
    while (result) {
        if (result->addr && result->addr->addr.type == IPADDR_TYPE_V6) {
            otPlatTrelPeerInfo info;
            uint8_t trel_txt[1024] = {0};
            uint16_t trel_txt_len = 0;
            size_t index = 0;
            while (index < result->txt_count) {
                trel_txt[trel_txt_len++] = strlen(result->txt[index].key) + result->txt_value_len[index] + 1;
                memcpy((trel_txt + trel_txt_len), (void *)result->txt[index].key, strlen(result->txt[index].key));
                trel_txt_len += (strlen(result->txt[index].key));
                trel_txt[trel_txt_len++] = '=';
                memcpy((trel_txt + trel_txt_len), (void *)result->txt[index].value, result->txt_value_len[index]);
                trel_txt_len += result->txt_value_len[index];
                index++;
            }
            if (!s_trel_netif) {
                s_trel_netif = result->esp_netif;
            }
            info.mTxtData = trel_txt;
            info.mTxtLength = trel_txt_len;
            info.mSockAddr.mPort = result->port;
            memcpy(info.mSockAddr.mAddress.mFields.m32, result->addr->addr.u_addr.ip6.addr, OT_IP6_ADDRESS_SIZE);
            info.mRemoved = (result->ttl == 0);
            ESP_LOGI(OT_PLAT_LOG_TAG, "%s TREL peer: address: %s, port:%d", info.mRemoved ? "Remove" : "Found", ip6addr_ntoa(((ip6_addr_t*)(&result->addr->addr.u_addr.ip6))), info.mSockAddr.mPort);
            esp_openthread_task_switching_lock_acquire(portMAX_DELAY);
            otPlatTrelHandleDiscoveredPeerInfo(esp_openthread_get_instance(), &info);
            esp_openthread_task_switching_lock_release();
        }
        result = result->next;
    }
}

static void trel_recv_task(void *ctx)
{
    struct pbuf *recv_buf = (struct pbuf *)ctx;
    uint8_t *data_buf = (uint8_t *)recv_buf->payload;
    uint8_t *data_buf_to_free = NULL;
    uint16_t length = recv_buf->len;

    if (recv_buf->next != NULL) {
        data_buf = (uint8_t *)malloc(recv_buf->tot_len);
        if (data_buf != NULL) {
            length = recv_buf->tot_len;
            data_buf_to_free = data_buf;
            pbuf_copy_partial(recv_buf, data_buf, recv_buf->tot_len, 0);
        } else {
            ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to allocate data buf when receiving Thread TREL message");
            ExitNow();
        }
    }
    otPlatTrelHandleReceived(esp_openthread_get_instance(), data_buf, length);

exit:
    if (data_buf_to_free) {
        free(data_buf_to_free);
    }
    pbuf_free(recv_buf);
}

static void handle_trel_udp_recv(void *ctx, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port)
{
    ESP_LOGD(OT_PLAT_LOG_TAG, "Receive from %s:%d", ip6addr_ntoa(&(addr->u_addr.ip6)), port);
    if (esp_openthread_task_queue_post(trel_recv_task, p) != ESP_OK) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to receive OpenThread TREL message");
    }
}

static esp_err_t ot_new_trel(void *ctx)
{
    ot_trel_t *task = (ot_trel_t *)ctx;

    task->trel_pcb = udp_new();
    ESP_RETURN_ON_FALSE(task->trel_pcb != NULL, ESP_ERR_NO_MEM, OT_PLAT_LOG_TAG, "Failed to create a new UDP pcb");
    udp_bind(task->trel_pcb, IP6_ADDR_ANY, task->port);
    udp_recv(task->trel_pcb, handle_trel_udp_recv, NULL);
    return ESP_OK;
}

void otPlatTrelEnable(otInstance *aInstance, uint16_t *aUdpPort)
{
    *aUdpPort = s_ot_trel.port;
    esp_openthread_task_switching_lock_release();
    esp_err_t err = esp_netif_tcpip_exec(ot_new_trel, &s_ot_trel);
    if (err != ESP_OK) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Fail to create trel udp");
    }
    mdns_browse_new(TREL_MDNS_TYPE, TREL_MDNS_PROTO, trel_browse_notifier);
    esp_openthread_task_switching_lock_acquire(portMAX_DELAY);
}

static void trel_send_task(void *ctx)
{
    err_t err = ERR_OK;
    struct pbuf *send_buf = NULL;
    ot_trel_send_task_t *task = (ot_trel_send_task_t *)ctx;

    task->pcb = s_ot_trel.trel_pcb;
    task->pcb->ttl = UDP_TTL;
    task->pcb->netif_idx = esp_netif_get_netif_impl_index(s_trel_netif);
    task->peer_addr.u_addr.ip6.zone = 0;
    task->peer_addr.type = IPADDR_TYPE_V6;
    task->pcb->flags = (task->pcb->flags & (~UDP_FLAGS_MULTICAST_LOOP));
    task->pcb->local_port = s_ot_trel.port;
    send_buf = pbuf_alloc(PBUF_TRANSPORT, task->length, PBUF_RAM);
    if (send_buf == NULL) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to allocate data buf when sending Thread TREL message");
        ExitNow();
    }
    memcpy(send_buf->payload, task->payload, task->length);
    err = udp_sendto_if(task->pcb, send_buf, &task->peer_addr, task->peer_port, netif_get_by_index(task->pcb->netif_idx));
    if(err != ERR_OK) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Fail to send trel msg to %s:%d %d (%d)", ip6addr_ntoa(&(task->peer_addr.u_addr.ip6)), task->peer_port, task->pcb->netif_idx, err);
    }
exit:
    pbuf_free(send_buf);
    free(task);
}

void otPlatTrelSend(otInstance       *aInstance,
                    const uint8_t    *aUdpPayload,
                    uint16_t          aUdpPayloadLen,
                    const otSockAddr *aDestSockAddr)
{
    if (!s_trel_netif) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "None Thread TREL interface");
        return;
    }
    ot_trel_send_task_t *task = (ot_trel_send_task_t *)malloc(sizeof(ot_trel_send_task_t));
    if (task == NULL) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to allocate buf for Thread TREL");
        return;
    }
    memcpy(task->peer_addr.u_addr.ip6.addr, aDestSockAddr->mAddress.mFields.m32, OT_IP6_ADDRESS_SIZE);
    task->peer_port = aDestSockAddr->mPort;
    ESP_LOGD(OT_PLAT_LOG_TAG, "send trel msg to %s:%d", ip6addr_ntoa(&(task->peer_addr.u_addr.ip6)), task->peer_port);
    task->payload = aUdpPayload;
    task->length = aUdpPayloadLen;
    esp_openthread_task_switching_lock_release();
    tcpip_callback(trel_send_task, task);
    esp_openthread_task_switching_lock_acquire(portMAX_DELAY);
}

void otPlatTrelRegisterService(otInstance *aInstance, uint16_t aPort, const uint8_t *aTxtData, uint8_t aTxtLength)
{
    esp_err_t ret = ESP_OK;

    esp_openthread_task_switching_lock_release();
    if (s_is_service_registered) {
        mdns_service_remove(TREL_MDNS_TYPE, TREL_MDNS_PROTO);
    }
    mdns_service_add(NULL, TREL_MDNS_TYPE, TREL_MDNS_PROTO, aPort, NULL, 0);
    s_is_service_registered = true;
    uint16_t index = 0;
    while (index < aTxtLength) {
        const uint8_t *item_header = aTxtData + index + 1;
        uint8_t item_len = aTxtData[index];

        char key[UINT8_MAX + 1];
        for (uint16_t i = 0; i < item_len; i++) {
            if (item_header[i] == '=') {
                ESP_GOTO_ON_FALSE(i != 0, ESP_FAIL, exit, OT_PLAT_LOG_TAG, "Wrong format of _trel._udp txt key");
                key[i] = '\0';
                uint16_t value_len = item_len - i - 1;
                ESP_GOTO_ON_FALSE(value_len != 0, ESP_FAIL, exit, OT_PLAT_LOG_TAG, "Wrong format of _trel._udp txt value");
                mdns_service_txt_item_set_with_explicit_value_len(TREL_MDNS_TYPE, TREL_MDNS_PROTO, key, (const char *)item_header + i + 1, value_len);
                break;
            }
            key[i] = item_header[i];
        }
        index += item_len + 1;
    }
exit:
    if (ret != ESP_OK) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Registered TREL service with some errors");
    }
    esp_openthread_task_switching_lock_acquire(portMAX_DELAY);
}

void otPlatTrelResetCounters(otInstance *aInstance)
{
    memset(&s_trel_counters, 0, sizeof(otPlatTrelCounters));
}

static void trel_disable_task(void *ctx)
{
    struct udp_pcb *pcb = (struct udp_pcb *)ctx;
    udp_remove(pcb);
}

void otPlatTrelDisable(otInstance *aInstance)
{
    esp_openthread_task_switching_lock_release();
    if (s_ot_trel.trel_pcb) {
        tcpip_callback(trel_disable_task, s_ot_trel.trel_pcb);
    }
    mdns_service_remove(TREL_MDNS_TYPE, TREL_MDNS_PROTO);
    s_is_service_registered = false;
    mdns_browse_delete(TREL_MDNS_TYPE, TREL_MDNS_PROTO);
    esp_openthread_task_switching_lock_acquire(portMAX_DELAY);
    s_ot_trel.trel_pcb = NULL;
}

const otPlatTrelCounters *otPlatTrelGetCounters(otInstance *aInstance)
{
    return &s_trel_counters;
}

// TODO: TZ-1169
OT_TOOL_WEAK otError otPlatRadioSetTransmitPower(otInstance *aInstance, int8_t aPower)
{
    ESP_LOGD(OT_PLAT_LOG_TAG, "Running in TREL mode and not support `otPlatRadioSetTransmitPower`");
    return OT_ERROR_NOT_IMPLEMENTED;
}

OT_TOOL_WEAK otError otPlatRadioGetTransmitPower(otInstance *aInstance, int8_t *aPower)
{
    ESP_LOGD(OT_PLAT_LOG_TAG, "Running in TREL mode and not support `otPlatRadioGetTransmitPower`");
    return OT_ERROR_NOT_IMPLEMENTED;
}

OT_TOOL_WEAK bool otPlatRadioGetPromiscuous(otInstance *aInstance)
{
    ESP_LOGD(OT_PLAT_LOG_TAG, "Running in TREL mode and not support `otPlatRadioGetPromiscuous`");
    return false;
}

OT_TOOL_WEAK otError otPlatRadioSetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t aThreshold)
{
    ESP_LOGD(OT_PLAT_LOG_TAG, "Running in TREL mode and not support `otPlatRadioSetCcaEnergyDetectThreshold`");
    return OT_ERROR_NOT_IMPLEMENTED;
}

OT_TOOL_WEAK otError otPlatRadioGetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t *aThreshold)
{
    ESP_LOGD(OT_PLAT_LOG_TAG, "Running in TREL mode and not support `otPlatRadioGetCcaEnergyDetectThreshold`");
    return OT_ERROR_NOT_IMPLEMENTED;
}

OT_TOOL_WEAK void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64)
{
    ESP_LOGD(OT_PLAT_LOG_TAG, "Running in TREL mode and not support `otPlatRadioGetIeeeEui64`");
}

OT_TOOL_WEAK otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    ESP_LOGD(OT_PLAT_LOG_TAG, "Running in TREL mode and not support `otPlatRadioGetTransmitBuffer`");
    return NULL;
}

#if CONFIG_OPENTHREAD_DIAG

OT_TOOL_WEAK void otPlatDiagSetOutputCallback(otInstance *aInstance, otPlatDiagOutputCallback aCallback, void *aContext)
{
    ESP_LOGD(OT_PLAT_LOG_TAG, "Running in TREL mode and not support `otPlatDiagSetOutputCallback`");
}

OT_TOOL_WEAK void otPlatDiagModeSet(bool mode)
{
    ESP_LOGD(OT_PLAT_LOG_TAG, "Running in TREL mode and not support `otPlatDiagModeSet`");
}

OT_TOOL_WEAK bool otPlatDiagModeGet(void)
{
    ESP_LOGD(OT_PLAT_LOG_TAG, "Running in TREL mode and not support `otPlatDiagModeGet`");
    return false;
}

OT_TOOL_WEAK void otPlatDiagTxPowerSet(int8_t tx_power)
{
    ESP_LOGD(OT_PLAT_LOG_TAG, "Running in TREL mode and not support `otPlatDiagTxPowerSet`");
}

OT_TOOL_WEAK void otPlatDiagChannelSet(uint8_t channel)
{
    ESP_LOGD(OT_PLAT_LOG_TAG, "Running in TREL mode and not support `otPlatDiagChannelSet`");
}

OT_TOOL_WEAK void otPlatDiagAlarmCallback(otInstance *aInstance)
{
    ESP_LOGD(OT_PLAT_LOG_TAG, "Running in TREL mode and not support `otPlatDiagAlarmCallback`");
}

#endif // CONFIG_OPENTHREAD_DIAG

OT_TOOL_WEAK esp_err_t esp_openthread_radio_init(const esp_openthread_platform_config_t *config)
{
    ESP_LOGI(OT_PLAT_LOG_TAG, "Running in TREL mode");
    return ESP_OK;
}

OT_TOOL_WEAK void esp_openthread_radio_deinit(void)
{
    ESP_LOGI(OT_PLAT_LOG_TAG, "Running in TREL mode");
}
