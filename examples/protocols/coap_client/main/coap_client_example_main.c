/* CoAP client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"

#include "nvs_flash.h"

#include "protocol_examples_common.h"

#include "coap.h"

#define COAP_DEFAULT_TIME_SEC 5

/* Set this to 9 to get verbose logging from within libcoap */
#define COAP_LOGGING_LEVEL 0

/* The examples use uri "coap://californium.eclipse.org" that
   you can set via the project configuration (idf.py menuconfig)

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define COAP_DEFAULT_DEMO_URI "coap://californium.eclipse.org"
*/
#define COAP_DEFAULT_DEMO_URI CONFIG_TARGET_DOMAIN_URI

const static char *TAG = "CoAP_client";

static int resp_wait = 1;
static coap_optlist_t *optlist = NULL;
static int wait_ms;

static void message_handler(coap_context_t *ctx, coap_session_t *session,
              coap_pdu_t *sent, coap_pdu_t *received,
                const coap_tid_t id)
{
    unsigned char* data = NULL;
    size_t data_len;
    coap_pdu_t *pdu = NULL;
    coap_opt_t *block_opt;
    coap_opt_iterator_t opt_iter;
    unsigned char buf[4];
    coap_optlist_t *option;
    coap_tid_t tid;

    if (COAP_RESPONSE_CLASS(received->code) == 2) {
        /* Need to see if blocked response */
        block_opt = coap_check_option(received, COAP_OPTION_BLOCK2, &opt_iter);
        if (block_opt) {
            uint16_t blktype = opt_iter.type;

            if (coap_opt_block_num(block_opt) == 0) {
                printf("Received:\n");
            }
            if (coap_get_data(received, &data_len, &data)) {
                printf("%.*s", (int)data_len, data);
            }
            if (COAP_OPT_BLOCK_MORE(block_opt)) {
                /* more bit is set */

                /* create pdu with request for next block */
                pdu = coap_new_pdu(session);
                if (!pdu) {
                     ESP_LOGE(TAG, "coap_new_pdu() failed");
                     goto clean_up;
                }
                pdu->type = COAP_MESSAGE_CON;
                pdu->tid = coap_new_message_id(session);
                pdu->code = COAP_REQUEST_GET;

                /* add URI components from optlist */
                for (option = optlist; option; option = option->next ) {
                    switch (option->number) {
                    case COAP_OPTION_URI_HOST :
                    case COAP_OPTION_URI_PORT :
                    case COAP_OPTION_URI_PATH :
                    case COAP_OPTION_URI_QUERY :
                        coap_add_option(pdu, option->number, option->length,
                                  option->data);
                        break;
                    default:
                        ;     /* skip other options */
                    }
                }

                /* finally add updated block option from response, clear M bit */
                /* blocknr = (blocknr & 0xfffffff7) + 0x10; */
                coap_add_option(pdu,
                                blktype,
                                coap_encode_var_safe(buf, sizeof(buf),
                                     ((coap_opt_block_num(block_opt) + 1) << 4) |
                                      COAP_OPT_BLOCK_SZX(block_opt)), buf);

                tid = coap_send(session, pdu);

                if (tid != COAP_INVALID_TID) {
                    resp_wait = 1;
                    wait_ms = COAP_DEFAULT_TIME_SEC * 1000;
                    return;
                }
            }
            printf("\n");
        } else {
            if (coap_get_data(received, &data_len, &data)) {
                printf("Received: %.*s\n", (int)data_len, data);
            }
        }
    }
clean_up:
    resp_wait = 0;
}

static void coap_example_task(void *p)
{
    struct hostent *hp;
    struct ip4_addr *ip4_addr;

    coap_address_t    dst_addr, src_addr;
    static coap_uri_t uri;
    const char*       server_uri = COAP_DEFAULT_DEMO_URI;
    char* phostname = NULL;

    coap_set_log_level(COAP_LOGGING_LEVEL);
    while (1) {
#define BUFSIZE 40
        unsigned char _buf[BUFSIZE];
        unsigned char *buf;
        size_t buflen;
        int res;
        coap_context_t *ctx = NULL;
        coap_session_t *session = NULL;
        coap_pdu_t *request = NULL;

        optlist = NULL;
        if (coap_split_uri((const uint8_t *)server_uri, strlen(server_uri), &uri) == -1) {
            ESP_LOGE(TAG, "CoAP server uri error");
            break;
        }

        if ((uri.scheme==COAP_URI_SCHEME_COAPS && !coap_dtls_is_supported()) ||
            (uri.scheme==COAP_URI_SCHEME_COAPS_TCP && !coap_tls_is_supported())) {
            ESP_LOGE(TAG, "CoAP server uri scheme error");
            break;
        }

        phostname = (char *)calloc(1, uri.host.length + 1);

        if (phostname == NULL) {
            ESP_LOGE(TAG, "calloc failed");
            continue;
        }

        memcpy(phostname, uri.host.s, uri.host.length);
        hp = gethostbyname(phostname);
        free(phostname);

        if (hp == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            free(phostname);
            continue;
        }

        /* Code to print the resolved IP.

           Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
        ip4_addr = (struct ip4_addr *)hp->h_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*ip4_addr));

        coap_address_init(&src_addr);
        src_addr.addr.sin.sin_family      = AF_INET;
        src_addr.addr.sin.sin_port        = htons(0);
        src_addr.addr.sin.sin_addr.s_addr = INADDR_ANY;

        if (uri.path.length) {
            buflen = BUFSIZE;
            buf = _buf;
            res = coap_split_path(uri.path.s, uri.path.length, buf, &buflen);

            while (res--) {
                coap_insert_optlist(&optlist,
                    coap_new_optlist(COAP_OPTION_URI_PATH,
                    coap_opt_length(buf),
                    coap_opt_value(buf)));

                buf += coap_opt_size(buf);
            }
        }

        if (uri.query.length) {
            buflen = BUFSIZE;
            buf = _buf;
            res = coap_split_query(uri.query.s, uri.query.length, buf, &buflen);

            while (res--) {
                coap_insert_optlist(&optlist,
                    coap_new_optlist(COAP_OPTION_URI_QUERY,
                    coap_opt_length(buf),
                    coap_opt_value(buf)));

                buf += coap_opt_size(buf);
            }
        }

        ctx = coap_new_context(NULL);
        if (!ctx) {
           ESP_LOGE(TAG, "coap_new_context() failed");
           goto clean_up;
        }

        coap_address_init(&dst_addr);
        dst_addr.addr.sin.sin_family      = AF_INET;
        dst_addr.addr.sin.sin_port        = htons(uri.port);
        dst_addr.addr.sin.sin_addr.s_addr = ip4_addr->addr;

        session = coap_new_client_session(ctx, &src_addr, &dst_addr,
           uri.scheme==COAP_URI_SCHEME_COAP_TCP ? COAP_PROTO_TCP :
           uri.scheme==COAP_URI_SCHEME_COAPS_TCP ? COAP_PROTO_TLS :
           uri.scheme==COAP_URI_SCHEME_COAPS ? COAP_PROTO_DTLS : COAP_PROTO_UDP);
        if (!session) {
           ESP_LOGE(TAG, "coap_new_client_session() failed");
           goto clean_up;
        }

        coap_register_response_handler(ctx, message_handler);

        request = coap_new_pdu(session);
        if (!request) {
           ESP_LOGE(TAG, "coap_new_pdu() failed");
           goto clean_up;
        }
        request->type = COAP_MESSAGE_CON;
        request->tid = coap_new_message_id(session);
        request->code = COAP_REQUEST_GET;
        coap_add_optlist_pdu(request, &optlist);

        resp_wait = 1;
        coap_send(session, request);

        wait_ms = COAP_DEFAULT_TIME_SEC * 1000;

        while (resp_wait) {
            int result = coap_run_once(ctx, wait_ms > 1000 ? 1000 : wait_ms);
            if (result >= 0) {
               if (result >= wait_ms) {
                  ESP_LOGE(TAG, "select timeout");
                  break;
               } else {
                  wait_ms -= result;
               } 
            }
        }
clean_up:
        if (optlist) {
            coap_delete_optlist(optlist);
            optlist = NULL;
        }
        if (session) coap_session_release(session);
        if (ctx) coap_free_context(ctx);
        coap_cleanup();
        /* Only send the request off once */
        break;
    }

    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    xTaskCreate(coap_example_task, "coap", 5 * 1024, NULL, 5, NULL);
}
