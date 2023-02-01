/* Async Request Handlers HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/freertos.h"
#include "freertos/semphr.h"
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>

/* An example that demonstrates multiple
   long running http requests running in parallel.

   In this example, multuple long http request can run at
   the same time. (uri: /long)

   While these long requests are running, the server can still
   respond to other incomings synchronous requests. (uri: /quick)
 */

static const char *TAG = "example";

// Async reqeusts are queued here while they wait to
// be processed by the workers
static QueueHandle_t asyncReqQueue;

// Track the number of free workers at any given time
static SemaphoreHandle_t workerReadyCount;

// Each worker has its own thread
static TaskHandle_t workerHandles[CONFIG_EXAMPLE_MAX_ASYNC_REQUESTS];

typedef esp_err_t (*httpd_req_handler_t)(httpd_req_t *req);

typedef struct {
    httpd_req_t* req;
    httpd_req_handler_t handler;
} httpd_async_req_t;


static bool is_on_async_worker_thread()
{
    // is our handle one of the known async handles?
    TaskHandle_t handle = xTaskGetCurrentTaskHandle();
    for (int i = 0; i < CONFIG_EXAMPLE_MAX_ASYNC_REQUESTS; i++) {
        if(workerHandles[i] == handle) {
            return true;
        }
    }
    return false;
}


// Submit an HTTP req to the async worker queue
static esp_err_t submit_async_req(httpd_req_t *req, httpd_req_handler_t handler)
{
    httpd_async_req_t asyncReq = {
        .req = req,
        .handler = handler,
    };

    // How should we handle resource exhastion? 
    // In this example, we immediately respond with an http error if 
    // no workers are available. If that is undesireable, you can instead
    // set ticks to > 0, which will increase the chance of a request being
    // handled. But your server will be less responsive overall.
    int ticks = 0;

    // counting semaphore - this will block until greater than 0.
    // At that point, we know 1 or more asyncReqTaskWorkers are available.
    if (xSemaphoreTake(workerReadyCount, ticks)) {

        // If a worker is available, then this queue should already
        // have space available / extremely soon. We wait up to 100ms
        // just to be safe.
        if (xQueueSend(asyncReqQueue, &asyncReq, pdMS_TO_TICKS(100))){
            return ESP_OK;
        } else {
            ESP_LOGE(TAG, "worker queue is full");
            return ESP_FAIL;
        }
    } else {
        // No workers are available
        ESP_LOGE(TAG, "No workers are available");
        return ESP_FAIL;
    }
}


/* A long running HTTP GET handler */
static esp_err_t long_async_handler(httpd_req_t *req)
{
    // This handler is first invoked on the httpd thread.
    // In order to free the httpd thread to handle other requests,
    // we must resubmit our request to be handled on an async worker thread.
    if (is_on_async_worker_thread() == false) {
        // submit 
        if (submit_async_req(req, long_async_handler) != ESP_OK) {
            httpd_resp_set_status(req, "503 Busy");
            httpd_resp_sendstr(req, "<div> no workers available. server busy.</div>");
            return ESP_OK;
        }
        // This signals to the server to not purge the socket for this 
        // request until httpd_resp_sendxxx() completes the request
        return ESP_ERR_NOT_FINISHED;
    }

    // track the number of long requests
    static uint8_t reqCount = 0;
    reqCount++;

    // send a request count
    char s[100];
    snprintf(s, sizeof(s), "<div>req: %u</div>\n", reqCount);
    httpd_resp_sendstr_chunk(req, s);

    // then every second, send a "tick"
    for (int i = 0; i < 60; i++) {

        // This delay makes this a "long running task".
        // In a real application, this may be a long calculation,
        // or some IO dependent code for instance.
        vTaskDelay(pdMS_TO_TICKS(1000));

        // send a tick
        snprintf(s, sizeof(s), "<div>%u</div>\n", i);
        httpd_resp_sendstr_chunk(req, s);
    }

    // send "complete"
    httpd_resp_sendstr_chunk(req, NULL);

    return ESP_OK;
}

void async_req_worker_task()
{
    ESP_LOGI(TAG, "starting async req task worker");

    while(true) {

        // counting semaphore - this signals that a worker
        // is ready to accept work
        xSemaphoreGive(workerReadyCount);

        // wait for a request
        httpd_async_req_t asyncReq;
        if (xQueueReceive(asyncReqQueue, &asyncReq, portMAX_DELAY)) {

            ESP_LOGI(TAG, "invoking %s", asyncReq.req->uri);

            // call the handler
            asyncReq.handler(asyncReq.req);

            // mark complete
            httpd_req_async_handler_complete(asyncReq.req);
        }
    }

    ESP_LOGW(TAG, "worker stopped");
    vTaskDelete(NULL);
}

static void start_async_req_workers()
{

    // counting semaphore keeps track of available workers
    workerReadyCount = xSemaphoreCreateCounting(
        CONFIG_EXAMPLE_MAX_ASYNC_REQUESTS,  // Max Count
        0); // Initial Count
    if (workerReadyCount == NULL) {
        ESP_LOGE(TAG, "Failed to create workers countingSemaphore");
        return;
    }

    // create queue
    asyncReqQueue = xQueueCreate(1, sizeof(httpd_async_req_t));
    if (asyncReqQueue == NULL){
        ESP_LOGE(TAG, "Failed to create asyncReqQueue");
        return;
    }

    for (int i = 0; i < CONFIG_EXAMPLE_MAX_ASYNC_REQUESTS; i++) {

        // task to repeatedly xfer data in
        bool success = xTaskCreate(async_req_worker_task, "asyncReqWorker",
                                    5*1024, // stack size
                                    (void *)0, // argument
                                    20, // priority
                                    &workerHandles[i]);

        if (!success) {
            ESP_LOGE(TAG, "Failed to start asyncReqWorker");
            continue;
        }
    }
}


/* A quick HTTP GET handler, which does not
   use any asynchronous features */
static esp_err_t quick_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "uri: /quick");
    char s[100];
    snprintf(s, sizeof(s), "random: %u\n", rand());
    httpd_resp_sendstr(req, s);
    return ESP_OK;
}

static esp_err_t index_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "uri: /");
    const char* html = "<div><a href=\"/long\">long</a></div>"
        "<div><a href=\"/quick\">quick</a></div>";
    httpd_resp_sendstr(req, html);
    return ESP_OK;
}

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // It is advisable that httpd_config_t->max_open_sockets > MAX_ASYNC_REQUESTS
    // Why? This leaves at least one socket still available to handle
    // quick synchronous requests. Otherwise, all the sockets will
    // get taken by the long async handlers, and your server will no
    // longer be responsive.
    config.max_open_sockets = CONFIG_EXAMPLE_MAX_ASYNC_REQUESTS + 1;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGI(TAG, "Error starting server!");
        return NULL;
    }

    const httpd_uri_t indexUri = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = index_handler,
    };

    const httpd_uri_t longUri = {
        .uri       = "/long",
        .method    = HTTP_GET,
        .handler   = long_async_handler,
    };

    const httpd_uri_t quickUri = {
        .uri       = "/quick",
        .method    = HTTP_GET,
        .handler   = quick_handler,
    };

    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &indexUri);
    httpd_register_uri_handler(server, &longUri);
    httpd_register_uri_handler(server, &quickUri);

    return server;
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

    // start workers
    start_async_req_workers();

    /* Start the server for the first time */
    server = start_webserver();
}
