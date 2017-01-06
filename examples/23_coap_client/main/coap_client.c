/* CoAP client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "coap_client.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"

#include "nvs_flash.h"
#include <sys/socket.h>

#include "coap_config.h"
#include "resource.h"
#include "coap.h"


static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const static int CONNECTED_BIT = BIT0;

const static char *TAG = "CoAP_demo";

static void message_handler(struct coap_context_t *ctx, const coap_endpoint_t *local_interface, const coap_address_t *remote,
              coap_pdu_t *sent, coap_pdu_t *received,
                const coap_tid_t id)
{
	unsigned char* data = NULL;
	size_t data_len;
	if (COAP_RESPONSE_CLASS(received->hdr->code) == 2) {
		if (coap_get_data(received, &data_len, &data)) {
			printf("Received: %s\n", data);
		}
	}
}

static void coap_demo_thread(void *p)
{
	coap_context_t*   ctx = NULL;
	coap_address_t    dst_addr, src_addr;
	static coap_uri_t uri;
	fd_set            readfds;
	struct timeval    tv;
	int flags, result;
	coap_pdu_t*       request = NULL;
	const char*       server_uri = COAP_DEFAULT_DEMO_URI;
	uint8_t     get_method = 1;

	coap_address_init(&src_addr);
	src_addr.addr.sin.sin_family      = AF_INET;
	src_addr.addr.sin.sin_port        = htons(0);
	src_addr.addr.sin.sin_addr.s_addr = INADDR_ANY;

	ctx = coap_new_context(&src_addr);
	if (ctx) {
		coap_address_init(&dst_addr);
		dst_addr.addr.sin.sin_family      = AF_INET;
		dst_addr.addr.sin.sin_port        = htons(COAP_DEFAULT_PORT);
		dst_addr.addr.sin.sin_addr.s_addr = inet_addr(COAP_DEFAULT_DEMO_ADDR);

		coap_split_uri((const uint8_t *)server_uri, strlen(server_uri), &uri);
		request            = coap_new_pdu();
		if (request){
			request->hdr->type = COAP_MESSAGE_CON;
			request->hdr->id   = coap_new_message_id(ctx);
			request->hdr->code = get_method;
			coap_add_option(request, COAP_OPTION_URI_PATH, uri.path.length, uri.path.s);

			coap_register_response_handler(ctx, message_handler);
			coap_send_confirmed(ctx, ctx->endpoint, &dst_addr, request);

			flags = fcntl(ctx->sockfd, F_GETFL, 0);
			fcntl(ctx->sockfd, F_SETFL, flags|O_NONBLOCK);

			tv.tv_usec = COAP_DEFAULT_TIME_USEC;
			tv.tv_sec = COAP_DEFAULT_TIME_SEC;

			for(;;) {
				FD_ZERO(&readfds);
				FD_CLR( ctx->sockfd, &readfds );
				FD_SET( ctx->sockfd, &readfds );
				result = select( FD_SETSIZE, &readfds, 0, 0, &tv );
				if (result > 0) {
					if (FD_ISSET( ctx->sockfd, &readfds ))
						coap_read(ctx);
				} else if (result < 0) {
					break;
				} else {
					printf("select timeout\n");
				}
			}
		}
		coap_free_context(ctx);
	}

	vTaskDelete(NULL);
}

static void coap_server_init(void)
{
    int ret = pdPASS;
    xTaskHandle coap_handle = NULL;

    ret = xTaskCreate(coap_demo_thread,
                      COAP_DEMO_THREAD_NAME,
                      COAP_DEMO_THREAD_STACK_WORDS,
                      NULL,
                      COAP_DEMO_THREAD_PRORIOTY,
                      &coap_handle);

    if (ret != pdPASS)  {
        ESP_LOGI(TAG, "create thread %s failed", COAP_DEMO_THREAD_NAME);
    }
}

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        coap_server_init();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void wifi_conn_init(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(wifi_event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );

    ESP_ERROR_CHECK( esp_wifi_start() );
}

void app_main(void)
{
    nvs_flash_init();
    wifi_conn_init();
}
