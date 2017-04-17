/* PPPoS Client Example with GSM (tested with Telit GL865-DUAL-V3)

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "driver/uart.h"

#include "netif/ppp/pppos.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/pppapi.h"

/* The examples use simple GSM configuration that you can set via
   'make menuconfig'.
 */
#define BUF_SIZE (1024)
const char *PPP_User = CONFIG_GSM_INTERNET_USER;
const char *PPP_Pass = CONFIG_GSM_INTERNET_PASSWORD;
const char *PPP_ApnATReq = "AT+CGDCONT=1,\"IP\",\"" \
                           CONFIG_GSM_APN \
                           "\"";

/* Pins used for serial communication with GSM module */
#define UART1_TX_PIN CONFIG_UART1_TX_PIN
#define UART1_RX_PIN CONFIG_UART1_RX_PIN
#define UART1_RTS_PIN CONFIG_UART1_RTS_PIN
#define UART1_CTS_PIN CONFIG_UART1_CTS_PIN

/* UART */
int uart_num = UART_NUM_1;

/* The PPP control block */
ppp_pcb *ppp;

/* The PPP IP interface */
struct netif ppp_netif;

static const char *TAG = "example";

typedef struct {
    char *cmd;
    uint16_t cmdSize;
    char *cmdResponseOnOk;
    uint32_t timeoutMs;
} GSM_Cmd;

#define GSM_OK_Str "OK"

GSM_Cmd GSM_MGR_InitCmds[] = {
    {
        .cmd = "AT\r",
        .cmdSize = sizeof("AT\r") - 1,
        .cmdResponseOnOk = GSM_OK_Str,
        .timeoutMs = 3000,
    },
    {
        .cmd = "ATE0\r",
        .cmdSize = sizeof("ATE0\r") - 1,
        .cmdResponseOnOk = GSM_OK_Str,
        .timeoutMs = 3000,
    },
    {
        .cmd = "AT+CPIN?\r",
        .cmdSize = sizeof("AT+CPIN?\r") - 1,
        .cmdResponseOnOk = "CPIN: READY",
        .timeoutMs = 3000,
    },
    {
        //AT+CGDCONT=1,"IP","apn"
        .cmd = "AT+CGDCONT=1,\"IP\",\"playmetric\"\r",
        .cmdSize = sizeof("AT+CGDCONT=1,\"IP\",\"playmetric\"\r") - 1,
        .cmdResponseOnOk = GSM_OK_Str,
        .timeoutMs = 3000,
    },
    {
        .cmd = "ATDT*99***1#\r",
        .cmdSize = sizeof("ATDT*99***1#\r") - 1,
        .cmdResponseOnOk = "CONNECT",
        .timeoutMs = 30000,
    }
};

#define GSM_MGR_InitCmdsSize  (sizeof(GSM_MGR_InitCmds)/sizeof(GSM_Cmd))

/* PPP status callback example */
static void ppp_status_cb(ppp_pcb *pcb, int err_code, void *ctx)
{
    struct netif *pppif = ppp_netif(pcb);
    LWIP_UNUSED_ARG(ctx);

    switch (err_code) {
    case PPPERR_NONE: {
        ESP_LOGI(TAG, "status_cb: Connected\n");
#if PPP_IPV4_SUPPORT
        ESP_LOGI(TAG, "   our_ipaddr  = %s\n", ipaddr_ntoa(&pppif->ip_addr));
        ESP_LOGI(TAG, "   his_ipaddr  = %s\n", ipaddr_ntoa(&pppif->gw));
        ESP_LOGI(TAG, "   netmask     = %s\n", ipaddr_ntoa(&pppif->netmask));
#endif /* PPP_IPV4_SUPPORT */
#if PPP_IPV6_SUPPORT
        ESP_LOGI(TAG, "   our6_ipaddr = %s\n", ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
#endif /* PPP_IPV6_SUPPORT */
        break;
    }
    case PPPERR_PARAM: {
        ESP_LOGE(TAG, "status_cb: Invalid parameter\n");
        break;
    }
    case PPPERR_OPEN: {
        ESP_LOGE(TAG, "status_cb: Unable to open PPP session\n");
        break;
    }
    case PPPERR_DEVICE: {
        ESP_LOGE(TAG, "status_cb: Invalid I/O device for PPP\n");
        break;
    }
    case PPPERR_ALLOC: {
        ESP_LOGE(TAG, "status_cb: Unable to allocate resources\n");
        break;
    }
    case PPPERR_USER: {
        ESP_LOGE(TAG, "status_cb: User interrupt\n");
        break;
    }
    case PPPERR_CONNECT: {
        ESP_LOGE(TAG, "status_cb: Connection lost\n");
        break;
    }
    case PPPERR_AUTHFAIL: {
        ESP_LOGE(TAG, "status_cb: Failed authentication challenge\n");
        break;
    }
    case PPPERR_PROTOCOL: {
        ESP_LOGE(TAG, "status_cb: Failed to meet protocol\n");
        break;
    }
    case PPPERR_PEERDEAD: {
        ESP_LOGE(TAG, "status_cb: Connection timeout\n");
        break;
    }
    case PPPERR_IDLETIMEOUT: {
        ESP_LOGE(TAG, "status_cb: Idle Timeout\n");
        break;
    }
    case PPPERR_CONNECTTIME: {
        ESP_LOGE(TAG, "status_cb: Max connect time reached\n");
        break;
    }
    case PPPERR_LOOPBACK: {
        ESP_LOGE(TAG, "status_cb: Loopback detected\n");
        break;
    }
    default: {
        ESP_LOGE(TAG, "status_cb: Unknown error code %d\n", err_code);
        break;
    }
    }

    /*
     * This should be in the switch case, this is put outside of the switch
     * case for example readability.
     */

    if (err_code == PPPERR_NONE) {
        return;
    }

    /* ppp_close() was previously called, don't reconnect */
    if (err_code == PPPERR_USER) {
        /* ppp_free(); -- can be called here */
        return;
    }


    /*
     * Try to reconnect in 30 seconds, if you need a modem chatscript you have
     * to do a much better signaling here ;-)
     */
    //ppp_connect(pcb, 30);
    /* OR ppp_listen(pcb); */
}

static u32_t ppp_output_callback(ppp_pcb *pcb, u8_t *data, u32_t len, void *ctx)
{
    ESP_LOGI(TAG, "PPP tx len %d", len);
    return uart_write_bytes(uart_num, (const char *)data, len);
}

static void pppos_client_task()
{
    char *data = (char *) malloc(BUF_SIZE);
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS
    };
    //Configure UART1 parameters
    uart_param_config(uart_num, &uart_config);

    // Configure UART1 pins (as set in example's menuconfig)
    ESP_LOGI(TAG, "Configuring UART1 GPIOs: TX:%d RX:%d RTS:%d CTS: %d",
             UART1_TX_PIN, UART1_RX_PIN, UART1_RTS_PIN, UART1_CTS_PIN);
    uart_set_pin(uart_num, UART1_TX_PIN, UART1_RX_PIN, UART1_RTS_PIN, UART1_CTS_PIN);
    uart_driver_install(uart_num, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0);

    while (1) {
        //init gsm
        int gsmCmdIter = 0;
        while (1) {
            ESP_LOGI(TAG, "%s", GSM_MGR_InitCmds[gsmCmdIter].cmd);
            uart_write_bytes(uart_num, (const char *)GSM_MGR_InitCmds[gsmCmdIter].cmd,
                             GSM_MGR_InitCmds[gsmCmdIter].cmdSize);

            int timeoutCnt = 0;
            while (1) {
                memset(data, 0, BUF_SIZE);
                int len = uart_read_bytes(uart_num, (uint8_t *)data, BUF_SIZE, 500 / portTICK_RATE_MS);
                if (len > 0) {
                    ESP_LOGI(TAG, "%s", data);
                }

                timeoutCnt += 500;
                if (strstr(data, GSM_MGR_InitCmds[gsmCmdIter].cmdResponseOnOk) != NULL) {
                    break;
                }

                if (timeoutCnt > GSM_MGR_InitCmds[gsmCmdIter].timeoutMs) {
                    ESP_LOGE(TAG, "Gsm Init Error");
                    return;
                }
            }
            gsmCmdIter++;

            if (gsmCmdIter >= GSM_MGR_InitCmdsSize) {
                break;
            }
        }

        ESP_LOGI(TAG, "Gsm init end");

        ppp = pppapi_pppos_create(&ppp_netif,
                                  ppp_output_callback, ppp_status_cb, NULL);

        ESP_LOGI(TAG, "After pppapi_pppos_create");

        if (ppp == NULL) {
            ESP_LOGE(TAG, "Error init pppos");
            return;
        }

        pppapi_set_default(ppp);

        ESP_LOGI(TAG, "After pppapi_set_default");

        pppapi_set_auth(ppp, PPPAUTHTYPE_PAP, PPP_User, PPP_Pass);

        ESP_LOGI(TAG, "After pppapi_set_auth");

        pppapi_connect(ppp, 0);

        ESP_LOGI(TAG, "After pppapi_connect");

        while (1) {
            memset(data, 0, BUF_SIZE);
            int len = uart_read_bytes(uart_num, (uint8_t *)data, BUF_SIZE, 10 / portTICK_RATE_MS);
            if (len > 0) {
                ESP_LOGI(TAG, "PPP rx len %d", len);
                pppos_input_tcpip(ppp, (u8_t *)data, len);
            }
        }

    }
}

void app_main()
{
    tcpip_adapter_init();
    xTaskCreate(&pppos_client_task, "pppos_client_task", 2048, NULL, 5, NULL);
}
