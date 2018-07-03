/* Persistent Sockets Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>

#include <http_server.h>

/* An example to demonstrate persistent sockets, with context maintained across
 * multiple requests on that socket.
 * The examples use simple WiFi configuration that you can set via 'make menuconfig'.
 * If you'd rather not, just change the below entries to strings with
 * the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
 */
#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

static const char *TAG="APP";

/* Function to free context */
void adder_free_func(void *ctx)
{
    ESP_LOGI(TAG, "/adder Free Context function called");
    free(ctx);
}

/* This handler keeps accumulating data that is posted to it into a per
 * socket/session context. And returns the result.
 */
esp_err_t adder_post_handler(httpd_req_t *req)
{
    /* Log total visitors */
    unsigned *visitors = (unsigned *)req->user_ctx;
    ESP_LOGI(TAG, "/adder visitor count = %d", ++(*visitors));

    char buf[10];
    char outbuf[50];
    int  ret;

    /* Read data received in the request */
    ret = httpd_req_recv(req, buf, sizeof(buf));
    if (ret < 0) {
        return ESP_FAIL;
    }

    buf[ret] = '\0';
    int val = atoi(buf);
    ESP_LOGI(TAG, "/adder handler read %d", val);

    /* Create session's context if not already available */
    if (! req->sess_ctx) {
        ESP_LOGI(TAG, "/adder allocating new session");
        req->sess_ctx = malloc(sizeof(int));
        req->free_ctx = adder_free_func;
        *(int *)req->sess_ctx = 0;
    }

    /* Add the received data to the context */
    int *adder = (int *)req->sess_ctx;
    *adder += val;

    /* Respond with the accumulated value */
    snprintf(outbuf, sizeof(outbuf),"%d", *adder);
    httpd_resp_send(req, outbuf, strlen(outbuf));
    return ESP_OK;
}

/* This handler gets the present value of the accumulator */
esp_err_t adder_get_handler(httpd_req_t *req)
{
    /* Log total visitors */
    unsigned *visitors = (unsigned *)req->user_ctx;
    ESP_LOGI(TAG, "/adder visitor count = %d", ++(*visitors));

    char outbuf[50];

    /* Create session's context if not already available */
    if (! req->sess_ctx) {
        ESP_LOGI(TAG, "/adder GET allocating new session");
        req->sess_ctx = malloc(sizeof(int));
        req->free_ctx = adder_free_func;
        *(int *)req->sess_ctx = 0;
    }
    ESP_LOGI(TAG, "/adder GET handler send %d", *(int *)req->sess_ctx);

    /* Respond with the accumulated value */
    snprintf(outbuf, sizeof(outbuf),"%d", *((int *)req->sess_ctx));
    httpd_resp_send(req, outbuf, strlen(outbuf));
    return ESP_OK;
}

/* This handler resets the value of the accumulator */
esp_err_t adder_put_handler(httpd_req_t *req)
{
    /* Log total visitors */
    unsigned *visitors = (unsigned *)req->user_ctx;
    ESP_LOGI(TAG, "/adder visitor count = %d", ++(*visitors));

    char buf[10];
    char outbuf[50];
    int  ret;

    /* Read data received in the request */
    ret = httpd_req_recv(req, buf, sizeof(buf));
    if (ret < 0) {
        return ESP_FAIL;
    }

    buf[ret] = '\0';
    int val = atoi(buf);
    ESP_LOGI(TAG, "/adder PUT handler read %d", val);

    /* Create session's context if not already available */
    if (! req->sess_ctx) {
        ESP_LOGI(TAG, "/adder PUT allocating new session");
        req->sess_ctx = malloc(sizeof(int));
        req->free_ctx = adder_free_func;
    }
    *(int *)req->sess_ctx = val;

    /* Respond with the reset value */
    snprintf(outbuf, sizeof(outbuf),"%d", *((int *)req->sess_ctx));
    httpd_resp_send(req, outbuf, strlen(outbuf));
    return ESP_OK;
}

/* Maintain a variable which stores the number of times
 * the "/adder" URI has been visited */
static unsigned visitors = 0;

httpd_uri_t adder_post = {
    .uri      = "/adder",
    .method   = HTTP_POST,
    .handler  = adder_post_handler,
    .user_ctx = &visitors
};

httpd_uri_t adder_get = {
    .uri      = "/adder",
    .method   = HTTP_GET,
    .handler  = adder_get_handler,
    .user_ctx = &visitors
};

httpd_uri_t adder_put = {
    .uri      = "/adder",
    .method   = HTTP_PUT,
    .handler  = adder_put_handler,
    .user_ctx = &visitors
};

httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: %d", config.server_port);
    httpd_handle_t server;

    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &adder_get);
        httpd_register_uri_handler(server, &adder_put);
        httpd_register_uri_handler(server, &adder_post);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    httpd_handle_t *server = (httpd_handle_t *) ctx;

    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
        ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
        ESP_LOGI(TAG, "Got IP: %s",
                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

        /* Start the web server */
        if (*server == NULL) {
            *server = start_webserver();
        }
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
        ESP_ERROR_CHECK(esp_wifi_connect());

        /* Stop the webserver */
        if (*server) {
            stop_webserver(*server);
            *server = NULL;
        }
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void *arg)
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, arg));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void app_main()
{
    static httpd_handle_t server = NULL;
    ESP_ERROR_CHECK(nvs_flash_init());
    initialise_wifi(&server);
}
