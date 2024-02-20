/* Persistent Sockets Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include "esp_netif.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"
#include "esp_check.h"

#include <esp_http_server.h>

/* An example to demonstrate persistent sockets, with context maintained across
 * multiple requests on that socket.
 */

static const char *TAG = "example";

/* Function to free context */
static void adder_free_func(void *ctx)
{
    ESP_LOGI(TAG, "/adder Free Context function called");
    free(ctx);
}

/* This handler keeps accumulating data that is posted to it into a per
 * socket/session context. And returns the result.
 */
static esp_err_t adder_post_handler(httpd_req_t *req)
{
    /* Log total visitors */
    unsigned *visitors = (unsigned *)req->user_ctx;
    ESP_LOGI(TAG, "/adder visitor count = %d", ++(*visitors));

    char buf[10];
    char outbuf[50];
    int  ret;

    /* Read data received in the request */
    ret = httpd_req_recv(req, buf, sizeof(buf));
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    buf[ret] = '\0';
    int val = atoi(buf);
    ESP_LOGI(TAG, "/adder handler read %d", val);

    /* Create session's context if not already available */
    if (! req->sess_ctx) {
        ESP_LOGI(TAG, "/adder allocating new session");
        req->sess_ctx = malloc(sizeof(int));
        ESP_RETURN_ON_FALSE(req->sess_ctx, ESP_ERR_NO_MEM, TAG, "Failed to allocate sess_ctx");
        req->free_ctx = adder_free_func;
        *(int *)req->sess_ctx = 0;
    }

    /* Add the received data to the context */
    int *adder = (int *)req->sess_ctx;
    *adder += val;

    /* Respond with the accumulated value */
    snprintf(outbuf, sizeof(outbuf),"%d", *adder);
    httpd_resp_send(req, outbuf, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* This handler gets the present value of the accumulator */
static esp_err_t adder_get_handler(httpd_req_t *req)
{
    /* Log total visitors */
    unsigned *visitors = (unsigned *)req->user_ctx;
    ESP_LOGI(TAG, "/adder visitor count = %d", ++(*visitors));

    char outbuf[50];

    /* Create session's context if not already available */
    if (! req->sess_ctx) {
        ESP_LOGI(TAG, "/adder GET allocating new session");
        req->sess_ctx = malloc(sizeof(int));
        ESP_RETURN_ON_FALSE(req->sess_ctx, ESP_ERR_NO_MEM, TAG, "Failed to allocate sess_ctx");
        req->free_ctx = adder_free_func;
        *(int *)req->sess_ctx = 0;
    }
    ESP_LOGI(TAG, "/adder GET handler send %d", *(int *)req->sess_ctx);

    /* Respond with the accumulated value */
    snprintf(outbuf, sizeof(outbuf),"%d", *((int *)req->sess_ctx));
    httpd_resp_send(req, outbuf, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

#if CONFIG_EXAMPLE_SESSION_CTX_HANDLERS
// login and logout handler are created to test the server functionality to delete the older sess_ctx if it is changed from another handler.
// login handler creates a new sess_ctx
static esp_err_t login_handler(httpd_req_t *req)
{
    /* Log total visitors */
    unsigned *visitors = (unsigned *)req->user_ctx;
    ESP_LOGI(TAG, "/login visitor count = %d", ++(*visitors));

    char outbuf[50];

    /* Create session's context if not already available */
    if (! req->sess_ctx) {
        ESP_LOGI(TAG, "/login GET allocating new session");
        req->sess_ctx = malloc(sizeof(int));
        if (!req->sess_ctx) {
            return ESP_ERR_NO_MEM;
        }
        *(int *)req->sess_ctx = 1;
    }
    ESP_LOGI(TAG, "/login GET handler send %d", *(int *)req->sess_ctx);

    /* Respond with the accumulated value */
    snprintf(outbuf, sizeof(outbuf),"%d", *((int *)req->sess_ctx));
    httpd_resp_send(req, outbuf, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// This handler sets sess_ctx to NULL.
static esp_err_t logout_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Logging out");
    // Setting sess_ctx to NULL here. This is done to test the server functionality to free the older sesss_ctx if it is changed by some handler.
    req->sess_ctx = NULL;
    char outbuf[50];
    snprintf(outbuf, sizeof(outbuf),"%d", 1);
    httpd_resp_send(req, outbuf, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
#endif // CONFIG_EXAMPLE_SESSION_CTX_HANDLERS

/* This handler resets the value of the accumulator */
static esp_err_t adder_put_handler(httpd_req_t *req)
{
    /* Log total visitors */
    unsigned *visitors = (unsigned *)req->user_ctx;
    ESP_LOGI(TAG, "/adder visitor count = %d", ++(*visitors));

    char buf[10];
    char outbuf[50];
    int  ret;

    /* Read data received in the request */
    ret = httpd_req_recv(req, buf, sizeof(buf));
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    buf[ret] = '\0';
    int val = atoi(buf);
    ESP_LOGI(TAG, "/adder PUT handler read %d", val);

    /* Create session's context if not already available */
    if (! req->sess_ctx) {
        ESP_LOGI(TAG, "/adder PUT allocating new session");
        req->sess_ctx = malloc(sizeof(int));
        ESP_RETURN_ON_FALSE(req->sess_ctx, ESP_ERR_NO_MEM, TAG, "Failed to allocate sess_ctx");
        req->free_ctx = adder_free_func;
    }
    *(int *)req->sess_ctx = val;

    /* Respond with the reset value */
    snprintf(outbuf, sizeof(outbuf),"%d", *((int *)req->sess_ctx));
    httpd_resp_send(req, outbuf, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Maintain a variable which stores the number of times
 * the "/adder" URI has been visited */
static unsigned visitors = 0;

static const httpd_uri_t adder_post = {
    .uri      = "/adder",
    .method   = HTTP_POST,
    .handler  = adder_post_handler,
    .user_ctx = &visitors
};

static const httpd_uri_t adder_get = {
    .uri      = "/adder",
    .method   = HTTP_GET,
    .handler  = adder_get_handler,
    .user_ctx = &visitors
};

#if CONFIG_EXAMPLE_SESSION_CTX_HANDLERS
static const httpd_uri_t login = {
    .uri      = "/login",
    .method   = HTTP_GET,
    .handler  = login_handler,
    .user_ctx = &visitors
};

static const httpd_uri_t logout = {
    .uri      = "/logout",
    .method   = HTTP_GET,
    .handler  = logout_handler,
    .user_ctx = &visitors
};
#endif // CONFIG_EXAMPLE_SESSION_CTX_HANDLERS

static const httpd_uri_t adder_put = {
    .uri      = "/adder",
    .method   = HTTP_PUT,
    .handler  = adder_put_handler,
    .user_ctx = &visitors
};

static httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    httpd_handle_t server;

    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &adder_get);
#if CONFIG_EXAMPLE_SESSION_CTX_HANDLERS
        httpd_register_uri_handler(server, &login);
        httpd_register_uri_handler(server, &logout);
#endif // CONFIG_EXAMPLE_SESSION_CTX_HANDLERS
        httpd_register_uri_handler(server, &adder_put);
        httpd_register_uri_handler(server, &adder_post);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_stop(server);
}


static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop http server");
        }
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}


void app_main(void)
{
    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    /* Register event handlers to stop the server when Wi-Fi or Ethernet is disconnected,
     * and re-start it upon connection.
     */
#ifdef CONFIG_EXAMPLE_CONNECT_WIFI
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
#endif // CONFIG_EXAMPLE_CONNECT_WIFI
#ifdef CONFIG_EXAMPLE_CONNECT_ETHERNET
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &disconnect_handler, &server));
#endif // CONFIG_EXAMPLE_CONNECT_ETHERNET

    /* Start the server for the first time */
    server = start_webserver();
}
