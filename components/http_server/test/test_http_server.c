// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <stdbool.h>
#include <esp_system.h>
#include <http_server.h>

#include "unity.h"

int pre_start_mem, post_stop_mem, post_stop_min_mem;
bool basic_sanity = true;

esp_err_t null_func(httpd_req_t *req)
{
    return ESP_OK;
}

httpd_uri_t handler_limit_uri (char* path)
{
    httpd_uri_t uri = {
        .uri      = path,
        .method   = HTTP_GET,
        .handler  = null_func,
        .user_ctx = NULL,
    };
    return uri;
};

static inline unsigned num_digits(unsigned x)
{
    unsigned digits = 1;
    while ((x = x/10) != 0) {
        digits++;
    }
    return digits;
}

#define HTTPD_TEST_MAX_URI_HANDLERS 8

void test_handler_limit(httpd_handle_t hd)
{
    int i;
    char x[HTTPD_TEST_MAX_URI_HANDLERS+1][num_digits(HTTPD_TEST_MAX_URI_HANDLERS)+1];
    httpd_uri_t uris[HTTPD_TEST_MAX_URI_HANDLERS+1];

    for (i = 0; i < HTTPD_TEST_MAX_URI_HANDLERS + 1; i++) {
        sprintf(x[i],"%d",i);
        uris[i] = handler_limit_uri(x[i]);
    }

    /* Register multiple instances of the same handler for MAX URI Handlers */
    for (i = 0; i < HTTPD_TEST_MAX_URI_HANDLERS; i++) {
        TEST_ASSERT(httpd_register_uri_handler(hd, &uris[i]) == ESP_OK);
    }

    /* Register the MAX URI + 1 Handlers should fail */
    TEST_ASSERT(httpd_register_uri_handler(hd, &uris[HTTPD_TEST_MAX_URI_HANDLERS]) != ESP_OK);

    /* Unregister the one of the Handler should pass */
    TEST_ASSERT(httpd_unregister_uri_handler(hd, uris[0].uri, uris[0].method) == ESP_OK);

    /* Unregister non added Handler should fail */
    TEST_ASSERT(httpd_unregister_uri_handler(hd, uris[0].uri, uris[0].method) != ESP_OK);

    /* Register the MAX URI Handler should pass */
    TEST_ASSERT(httpd_register_uri_handler(hd, &uris[0]) == ESP_OK);

    /* Reregister same instance of handler should fail */
    TEST_ASSERT(httpd_register_uri_handler(hd, &uris[0]) != ESP_OK);

    /* Register the MAX URI + 1 Handlers should fail */
    TEST_ASSERT(httpd_register_uri_handler(hd, &uris[HTTPD_TEST_MAX_URI_HANDLERS]) != ESP_OK);

    /* Unregister the same handler for MAX URI Handlers */
    for (i = 0; i < HTTPD_TEST_MAX_URI_HANDLERS; i++) {
        TEST_ASSERT(httpd_unregister_uri_handler(hd, uris[i].uri, uris[i].method) == ESP_OK);
    }
    basic_sanity = false;
}

/********************* Test Handler Limit End *******************/

httpd_handle_t test_httpd_start(uint16_t id)
{
    httpd_handle_t hd;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = HTTPD_TEST_MAX_URI_HANDLERS;
    config.server_port += id;
    config.ctrl_port += id;
    TEST_ASSERT(httpd_start(&hd, &config) == ESP_OK)
    return hd;
}

#define SERVER_INSTANCES 2

/* Currently this only tests for the number of tasks.
 * Heap leakage is not tested as LWIP allocates memory
 * which may not be freed immedietly causing erroneous
 * evaluation. Another test to implement would be the
 * monitoring of open sockets, but LWIP presently provides
 * no such API for getting the number of open sockets.
 */
TEST_CASE("Leak Test", "[HTTP SERVER]")
{
    httpd_handle_t hd[SERVER_INSTANCES];
    unsigned task_count = uxTaskGetNumberOfTasks();
    pre_start_mem = esp_get_free_heap_size();
    bool res = true;

    for (int i = 0; i < SERVER_INSTANCES; i++) {
        hd[i] = test_httpd_start(i);
        vTaskDelay(10);
        if (uxTaskGetNumberOfTasks() != ++task_count) {
            res = false;
        }
    }

    for (int i = 0; i < SERVER_INSTANCES; i++) {
        if (httpd_stop(hd[i]) != ESP_OK) {
            res = false;
        }
        vTaskDelay(10);
        if (uxTaskGetNumberOfTasks() != --task_count) {
            res = false;
        }
    }
    post_stop_mem = esp_get_free_heap_size();
    TEST_ASSERT(res == true);
}

TEST_CASE("Basic Functionality Tests", "[HTTP SERVER]")
{
    httpd_handle_t hd;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    TEST_ASSERT(httpd_start(&hd, &config) == ESP_OK);
    test_handler_limit(hd);
    TEST_ASSERT(httpd_stop(hd) == ESP_OK);
}
