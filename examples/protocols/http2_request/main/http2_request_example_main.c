/* HTTP2 GET Example using nghttp2
 
   Contacts http2.golang.org and executes the GET/PUT requests. A thin API
   wrapper on top of nghttp2, to properly demonstrate the interactions.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

 */
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "lwip/apps/sntp.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_netif.h"

#include "sh2lib.h"


/* The HTTP/2 server to connect to */
#define HTTP2_SERVER_URI  "https://http2.golang.org"
/* A GET request that keeps streaming current time every second */
#define HTTP2_STREAMING_GET_PATH  "/clockstream"
/* A PUT request that echoes whatever we had sent to it */
#define HTTP2_PUT_PATH            "/ECHO"


int handle_get_response(struct sh2lib_handle *handle, const char *data, size_t len, int flags)
{
    if (len) {
        printf("[get-response] %.*s\n", len, data);
    }
    if (flags == DATA_RECV_FRAME_COMPLETE) {
        printf("[get-response] Frame fully received\n");
    }
    if (flags == DATA_RECV_RST_STREAM) {
        printf("[get-response] Stream Closed\n");
    }
    return 0;
}

int handle_echo_response(struct sh2lib_handle *handle, const char *data, size_t len, int flags)
{
    if (len) {
        printf("[echo-response] %.*s\n", len, data);
    }
    if (flags == DATA_RECV_FRAME_COMPLETE) {
        printf("[echo-response] Frame fully received\n");
    }
    if (flags == DATA_RECV_RST_STREAM) {
        printf("[echo-response] Stream Closed\n");
    }
    return 0;
}

int send_put_data(struct sh2lib_handle *handle, char *buf, size_t length, uint32_t *data_flags)
{
#define DATA_TO_SEND "Hello World"
    int copylen = strlen(DATA_TO_SEND);
    if (copylen < length) {
        printf("[data-prvd] Sending %d bytes\n", copylen);
        memcpy(buf, DATA_TO_SEND, copylen);
    } else {
        copylen = 0;
    }

    (*data_flags) |= NGHTTP2_DATA_FLAG_EOF;
    return copylen;
}

static void set_time(void)
{
    struct timeval tv = {
        .tv_sec = 1509449941,
    };
    struct timezone tz = {
        0, 0
    };
    settimeofday(&tv, &tz);

    /* Start SNTP service */
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_init();
}

static void http2_task(void *args)
{
    /* Set current time: proper system time is required for TLS based
     * certificate verification.
     */
    set_time();

    /* HTTP2: one connection multiple requests. Do the TLS/TCP connection first */
    printf("Connecting to server\n");
    struct sh2lib_handle hd;
    if (sh2lib_connect(&hd, HTTP2_SERVER_URI) != 0) {
        printf("Failed to connect\n");
        vTaskDelete(NULL);
        return;
    }
    printf("Connection done\n");

    /* HTTP GET  */
    sh2lib_do_get(&hd, HTTP2_STREAMING_GET_PATH, handle_get_response);

    /* HTTP PUT  */
    sh2lib_do_put(&hd, HTTP2_PUT_PATH, send_put_data, handle_echo_response);

    while (1) {
        /* Process HTTP2 send/receive */
        if (sh2lib_execute(&hd) < 0) {
            printf("Error in send/receive\n");
            break;
        }
        vTaskDelay(2);
    }

    sh2lib_free(&hd);
    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    esp_netif_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    xTaskCreate(&http2_task, "http2_task", (1024 * 32), NULL, 5, NULL);
}
