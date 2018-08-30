#include <stdlib.h>
#include <stdbool.h>

#include <esp_log.h>
#include <esp_system.h>
#include <http_server.h>

#include "tests.h"

static const char *TAG="TESTS";

int pre_start_mem, post_stop_mem, post_stop_min_mem;
bool basic_sanity = true;

/********************* Basic Handlers Start *******************/

esp_err_t hello_get_handler(httpd_req_t *req)
{
#define STR "Hello World!"
    ESP_LOGI(TAG, "Free Stack for server task: %d", uxTaskGetStackHighWaterMark(NULL));
    httpd_resp_send(req, STR, strlen(STR));
    return ESP_OK;
#undef STR
}

esp_err_t hello_type_get_handler(httpd_req_t *req)
{
#define STR "Hello World!"
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, STR, strlen(STR));
    return ESP_OK;
#undef STR
}

esp_err_t hello_status_get_handler(httpd_req_t *req)
{
#define STR "Hello World!"
    httpd_resp_set_status(req, HTTPD_500);
    httpd_resp_send(req, STR, strlen(STR));
    return ESP_OK;
#undef STR
}

esp_err_t echo_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "/echo handler read content length %d", req->content_len);

    char*  buf = malloc(req->content_len + 1);
    size_t off = 0;
    int    ret;

    if (!buf) {
        return ESP_FAIL;
    }

    while (off < req->content_len) {
        /* Read data received in the request */
        ret = httpd_req_recv(req, buf + off, req->content_len - off);
        if (ret < 0) {
            free (buf);
            return ESP_FAIL;
        }
        off += ret;
        ESP_LOGI(TAG, "/echo handler recv length %d", ret);
    }
    buf[off] = '\0';

    if (req->content_len < 128) {
        ESP_LOGI(TAG, "/echo handler read %s", buf);
    }

    /* Search for Custom header field */
    char*  req_hdr = 0;
    size_t hdr_len = httpd_req_get_hdr_value_len(req, "Custom");
    if (hdr_len) {
        /* Read Custom header value */
        req_hdr = malloc(hdr_len + 1);
        if (req_hdr) {
            httpd_req_get_hdr_value_str(req, "Custom", req_hdr, hdr_len + 1);

            /* Set as additional header for response packet */
            httpd_resp_set_hdr(req, "Custom", req_hdr);
        }
    }
    httpd_resp_send(req, buf, req->content_len);
    free (req_hdr);
    free (buf);
    return ESP_OK;
}

void adder_free_func(void *ctx)
{
    ESP_LOGI(TAG, "Custom Free Context function called");
    free(ctx);
}

/* Create a context, keep incrementing value in the context, by whatever was
 * received. Return the result
 */
esp_err_t adder_post_handler(httpd_req_t *req)
{
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

    if (! req->sess_ctx) {
        ESP_LOGI(TAG, "/adder allocating new session");
        req->sess_ctx = malloc(sizeof(int));
        req->free_ctx = adder_free_func;
        *(int *)req->sess_ctx = 0;
    }
    int *adder = (int *)req->sess_ctx;
    *adder += val;

    snprintf(outbuf, sizeof(outbuf),"%d", *adder);
    httpd_resp_send(req, outbuf, strlen(outbuf));
    return ESP_OK;
}

esp_err_t leftover_data_post_handler(httpd_req_t *req)
{
    /* Only echo the first 10 bytes of the request, leaving the rest of the
     * request data as is.
     */
    char buf[11];
    int  ret;

    /* Read data received in the request */
    ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret < 0) {
        return ESP_FAIL;
    }

    buf[ret] = '\0';
    ESP_LOGI(TAG, "leftover data handler read %s", buf);
    httpd_resp_send(req, buf, strlen(buf));
    return ESP_OK;
}

int httpd_default_send(int sockfd, const char *buf, unsigned buf_len, int flags);
void generate_async_resp(void *arg)
{
    char buf[250];
    int fd = (int )arg;
#define HTTPD_HDR_STR      "HTTP/1.1 200 OK\r\n"                   \
                           "Content-Type: text/html\r\n"           \
                           "Content-Length: %d\r\n"
#define STR "Hello Double World!"

    ESP_LOGI(TAG, "Executing queued work fd : %d", fd);

    snprintf(buf, sizeof(buf), HTTPD_HDR_STR,
         strlen(STR));
    httpd_default_send(fd, buf, strlen(buf), 0);
    /* Space for sending additional headers based on set_header */
    httpd_default_send(fd, "\r\n", strlen("\r\n"), 0);
    httpd_default_send(fd, STR, strlen(STR), 0);
#undef STR
}

esp_err_t async_get_handler(httpd_req_t *req)
{
#define STR "Hello World!"
    httpd_resp_send(req, STR, strlen(STR));
    /* Also register a HTTPD Work which sends the same data on the same
     * socket again
     */
    int fd = httpd_req_to_sockfd(req);
    if (fd < 0) {
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Queuing work fd : %d", fd);
    httpd_queue_work(req->handle, generate_async_resp, (void *)fd);
    return ESP_OK;
#undef STR
}


httpd_uri_t basic_handlers[] = {
    { .uri      = "/hello/type_html",
      .method   = HTTP_GET,
      .handler  = hello_type_get_handler,
      .user_ctx = NULL,
    },
    { .uri      = "/hello",
      .method   = HTTP_GET,
      .handler  = hello_get_handler,
      .user_ctx = NULL,
    },
    { .uri      = "/hello/status_500",
      .method   = HTTP_GET,
      .handler  = hello_status_get_handler,
      .user_ctx = NULL,
    },
    { .uri      = "/echo",
      .method   = HTTP_POST,
      .handler  = echo_post_handler,
      .user_ctx = NULL,
    },
    { .uri      = "/echo",
      .method   = HTTP_PUT,
      .handler  = echo_post_handler,
      .user_ctx = NULL,
    },
    { .uri      = "/leftover_data",
      .method   = HTTP_POST,
      .handler  = leftover_data_post_handler,
      .user_ctx = NULL,
    },
    { .uri      = "/adder",
      .method   = HTTP_POST,
      .handler  = adder_post_handler,
      .user_ctx = NULL,
    },
    { .uri      = "/async_data",
      .method   = HTTP_GET,
      .handler  = async_get_handler,
      .user_ctx = NULL,
    }
};

int basic_handlers_no = sizeof(basic_handlers)/sizeof(httpd_uri_t);
void register_basic_handlers(httpd_handle_t hd)
{
    int i;
    ESP_LOGI(TAG, "Registering basic handlers");
    ESP_LOGI(TAG, "No of handlers = %d", basic_handlers_no);
    for (i = 0; i < basic_handlers_no; i++) {
        if (httpd_register_uri_handler(hd, &basic_handlers[i]) != ESP_OK) {
            ESP_LOGW(TAG, "register uri failed for %d", i);
            return;
        }
    }
    ESP_LOGI(TAG, "Success");
}

/********************* Basic Handlers End *******************/

esp_err_t my_hello_post_handler(httpd_req_t *req)
{
    char buf[10];
    char outbuf[50];
    int ret;

    ret = httpd_req_recv(req, buf, sizeof(buf));
    if (ret < 0) {
        return ESP_FAIL;
    }

    httpd_resp_set_status(req, HTTPD_404);
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    ESP_LOGI(TAG, "Read %d bytes as:%s:", ret, buf);
    buf[ret] = '\0';
#define STR "my_hello_handler"
    snprintf(outbuf, sizeof(outbuf), STR" %s", buf);
    httpd_resp_send(req, outbuf, strlen(outbuf));
    return ESP_OK;
#undef STR
}


/********************* Test Handler Limit Start *******************/

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
    int i, ret;
    char x[HTTPD_TEST_MAX_URI_HANDLERS+1][num_digits(HTTPD_TEST_MAX_URI_HANDLERS)+1];
    httpd_uri_t uris[HTTPD_TEST_MAX_URI_HANDLERS+1];

    for (i = 0; i < HTTPD_TEST_MAX_URI_HANDLERS + 1; i++) {
        sprintf(x[i],"%d",i);
        uris[i] = handler_limit_uri(x[i]);
    }

    /* Register multiple instances of the same handler for MAX URI Handlers */
    ESP_LOGI(TAG, "Test: Register Max URI handlers: %d...", HTTPD_TEST_MAX_URI_HANDLERS);
    for (i = 0; i < HTTPD_TEST_MAX_URI_HANDLERS; i++) {
        ret = httpd_register_uri_handler(hd, &uris[i]);
        if (ret != ESP_OK) {
            ESP_LOGI(TAG, "Fail");
            goto error_ret;
        }
    }
    ESP_LOGI(TAG, "Success");

    /* Register the MAX URI + 1 Handlers should fail */
    ESP_LOGI(TAG, "Test: Register Max URI + 1 handlers: %d th...", HTTPD_TEST_MAX_URI_HANDLERS +1 );
    ret = httpd_register_uri_handler(hd, &uris[HTTPD_TEST_MAX_URI_HANDLERS]);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Fail");
        goto error_ret;
    }
    ESP_LOGI(TAG, "Success");

    /* Unregister the one of the Handler should pass */
    ESP_LOGI(TAG, "Test: Unregister 0th handler...");
    ret = httpd_unregister_uri_handler(hd, uris[0].uri, uris[0].method);
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "Fail");
        goto error_ret;
    }
    ESP_LOGI(TAG, "Success");

    /* Unregister non added Handler should fail */
    ESP_LOGI(TAG, "Test: Again unregister 0th handler not registered...");
    ret = httpd_unregister_uri_handler(hd, uris[0].uri, uris[0].method);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Fail");
        goto error_ret;
    }
    ESP_LOGI(TAG, "Success");

    /* Register the MAX URI Handler should pass */
    ESP_LOGI(TAG, "Test: Register back 0th handler...");
    ret = httpd_register_uri_handler(hd, &uris[0]);
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "Fail");
        goto error_ret;
    }
    ESP_LOGI(TAG, "Success");

    /* Reregister same instance of handler should fail */
    ESP_LOGI(TAG, "Test: Register 0th handler again after registering...");
    ret = httpd_register_uri_handler(hd, &uris[0]);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Fail");
        goto error_ret;
    }
    ESP_LOGI(TAG, "Success");

    /* Register the MAX URI + 1 Handlers should fail */
    ESP_LOGI(TAG, "Test: Register 1 more handler...");
    ret = httpd_register_uri_handler(hd, &uris[HTTPD_TEST_MAX_URI_HANDLERS]);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Fail");
        goto error_ret;
    }
    ESP_LOGI(TAG, "Success");

    /* Unregister the same handler for MAX URI Handlers */
    ESP_LOGI(TAG, "Test: Unregister all handlers:");
    for (i = 0; i < HTTPD_TEST_MAX_URI_HANDLERS; i++) {
        ret = httpd_unregister_uri_handler(hd, uris[i].uri, uris[i].method);
        if (ret != 0) {
            ESP_LOGI(TAG, "Fail");
            goto error_ret;
        }
    }
    ESP_LOGI(TAG, "Success");
error_ret:
    for (; i < HTTPD_TEST_MAX_URI_HANDLERS; i++) {
        httpd_unregister_uri_handler(hd, uris[i].uri, uris[i].method);
    }
    basic_sanity = false;
}

/********************* Test Handler Limit End *******************/

httpd_handle_t test_httpd_start()
{
    pre_start_mem = esp_get_free_heap_size();
    httpd_handle_t hd;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 1234;

    /* This check should be a part of http_server */
    config.max_open_sockets = (CONFIG_LWIP_MAX_SOCKETS - 3);

    if (httpd_start(&hd, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Started HTTP server on port: %d", config.server_port);
        ESP_LOGI(TAG, "Max URI handlers: %d", config.max_uri_handlers);
        ESP_LOGI(TAG, "Max Open Sessions: %d", config.max_open_sockets);
        ESP_LOGI(TAG, "Max Header Length: %d", HTTPD_MAX_REQ_HDR_LEN);
        ESP_LOGI(TAG, "Max URI Length: %d", HTTPD_MAX_URI_LEN);
        ESP_LOGI(TAG, "Max Stack Size: %d", config.stack_size);
        return hd;
    }
    return NULL;
}

httpd_handle_t test_httpd_start_dummy(uint16_t id)
{
    pre_start_mem = esp_get_free_heap_size();
    ESP_LOGI(TAG, "HTTPD Start: Current free memory: %d", pre_start_mem);
    httpd_handle_t hd;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = HTTPD_TEST_MAX_URI_HANDLERS;
    config.server_port += id;
    config.ctrl_port += id;
    if (httpd_start(&hd, &config) == ESP_OK) {
        return hd;
    }
    return NULL;
}

void test_httpd_stop(httpd_handle_t hd)
{
    httpd_stop(hd);
    post_stop_mem = esp_get_free_heap_size();
    ESP_LOGI(TAG, "HTTPD Stop: Current free memory: %d", post_stop_mem);

    // Below function is not available but would be desirable to have
    // post_stop_min_mem = os_get_minimum_free_mem();
    // ESP_LOGI(TAG, "HTTPD Stop: Minimum free memory: %d", post_stop_min_mem);
}

#define SERVER_INSTANCES 10

/* Currently this only tests for the number of tasks.
 * Heap leakage is not tested as LWIP allocates memory
 * which may not be freed immedietly causing erroneous
 * evaluation. Another test to implement would be the
 * monitoring of open sockets, but LWIP presently provides
 * no such API for getting the number of open sockets.
 */
bool leak_test(void)
{
    httpd_handle_t hd[SERVER_INSTANCES];
    bool success = true;

    unsigned task_count = uxTaskGetNumberOfTasks();
    ESP_LOGI(TAG, "Leak Test Started...");
    ESP_LOGI(TAG, "Current free heap : %d", xPortGetFreeHeapSize());
    ESP_LOGI(TAG, "Total tasks running : %d", task_count);

    for (int i = 0; i < SERVER_INSTANCES; i++) {
        ESP_LOGI(TAG, "Starting Server Instance [%d]", i);
        hd[i] = test_httpd_start_dummy(i);
        if (hd[i]) {
            register_basic_handlers(hd[i]);
            task_count++;
        }
        ESP_LOGI(TAG, "Current free heap : %d", xPortGetFreeHeapSize());
        ESP_LOGI(TAG, "Total tasks running : %d", uxTaskGetNumberOfTasks());
        if (uxTaskGetNumberOfTasks() != task_count) {
            ESP_LOGE(TAG, "Task count mismatch");
            success = false;
            break;
        }
    }

    for (int i = 0; i < SERVER_INSTANCES; i++) {
        ESP_LOGI(TAG, "Stopping Server Instance [%d]", i);
        if (hd[i]) {
            httpd_stop(hd[i]);
            task_count--;
        }
        ESP_LOGI(TAG, "Current free heap : %d", xPortGetFreeHeapSize());
        ESP_LOGI(TAG, "Total tasks running : %d", uxTaskGetNumberOfTasks());
        if (uxTaskGetNumberOfTasks() != task_count) {
            ESP_LOGE(TAG, "Task count mismatch");
            success = false;
        }
    }
    if (success) {
        ESP_LOGI(TAG, "Leak Test Passed");
    }
    else {
        ESP_LOGI(TAG, "Leak Test Failed");
    }
    return success;
}

httpd_handle_t start_tests()
{
//     leak_test();
    httpd_handle_t hd = test_httpd_start();
    if (hd) {
//         test_handler_limit(hd);
        register_basic_handlers(hd);
    }
    return hd;
}

void stop_tests(httpd_handle_t hd)
{
    ESP_LOGI(TAG, "Stopping httpd");
    test_httpd_stop(hd);
}
