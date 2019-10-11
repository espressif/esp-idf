#include <stdlib.h>
#include <stdbool.h>

#include <esp_log.h>
#include <esp_system.h>
#include <esp_http_server.h>

#include "tests.h"

static const char *TAG = "TESTS";

static int pre_start_mem, post_stop_mem;

struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
};

/********************* Basic Handlers Start *******************/

static esp_err_t hello_get_handler(httpd_req_t *req)
{
#define STR "Hello World!"
    ESP_LOGI(TAG, "Free Stack for server task: '%d'", uxTaskGetStackHighWaterMark(NULL));
    httpd_resp_send(req, STR, strlen(STR));
    return ESP_OK;
#undef STR
}

/* This handler is intended to check what happens in case of empty values of headers. 
 * Here `Header2` is an empty header and `Header1` and `Header3` will have `Value1` 
 * and `Value3` in them. */
static esp_err_t test_header_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    int buf_len;
    char *buf;

    buf_len = httpd_req_get_hdr_value_len(req, "Header1");
    if (buf_len > 0) {
        buf = malloc(++buf_len);
        if (!buf) {
            ESP_LOGE(TAG, "Failed to allocate memory of %d bytes!", buf_len);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Memory allocation failed");
            return ESP_ERR_NO_MEM;
        }
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Header1", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Header1 content: %s", buf);
            if (strcmp("Value1", buf) != 0) {
                httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Wrong value of Header1 received");
                free(buf);
                return ESP_ERR_INVALID_ARG;
            } else {
                ESP_LOGI(TAG, "Expected value and received value matched for Header1");
            }
        } else {
            ESP_LOGE(TAG, "Error in getting value of Header1");
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Error in getting value of Header1");
            free(buf);
            return ESP_FAIL;
        }
        free(buf);
    } else {
        ESP_LOGE(TAG, "Header1 not found");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Header1 not found");
        return ESP_ERR_NOT_FOUND;
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Header3");
    if (buf_len > 0) {
        buf = malloc(++buf_len);
        if (!buf) {
            ESP_LOGE(TAG, "Failed to allocate memory of %d bytes!", buf_len);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Memory allocation failed");
            return ESP_ERR_NO_MEM;
        }
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Header3", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Header3 content: %s", buf);
            if (strcmp("Value3", buf) != 0) {
                httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Wrong value of Header3 received");
                free(buf);
                return ESP_ERR_INVALID_ARG;
            } else {
                ESP_LOGI(TAG, "Expected value and received value matched for Header3");
            }
        } else {
            ESP_LOGE(TAG, "Error in getting value of Header3");
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Error in getting value of Header3");
            free(buf);
            return ESP_FAIL;
        }
        free(buf);
    } else {
        ESP_LOGE(TAG, "Header3 not found");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Header3 not found");
        return ESP_ERR_NOT_FOUND;
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Header2");
    buf = malloc(++buf_len);
    if (!buf) {
        ESP_LOGE(TAG, "Failed to allocate memory of %d bytes!", buf_len);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Memory allocation failed");
        return ESP_ERR_NO_MEM;
    }
    if (httpd_req_get_hdr_value_str(req, "Header2", buf, buf_len) == ESP_OK) {
        ESP_LOGI(TAG, "Header2 content: %s", buf);
        httpd_resp_send(req, buf, strlen(buf));
    } else {
        ESP_LOGE(TAG, "Header2 not found");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Header2 not found");
        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t hello_type_get_handler(httpd_req_t *req)
{
#define STR "Hello World!"
    httpd_resp_set_type(req, HTTPD_TYPE_TEXT);
    httpd_resp_send(req, STR, strlen(STR));
    return ESP_OK;
#undef STR
}

static esp_err_t hello_status_get_handler(httpd_req_t *req)
{
#define STR "Hello World!"
    httpd_resp_set_status(req, HTTPD_500);
    httpd_resp_send(req, STR, strlen(STR));
    return ESP_OK;
#undef STR
}

static esp_err_t echo_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "/echo handler read content length %d", req->content_len);

    char*  buf = malloc(req->content_len + 1);
    size_t off = 0;
    int    ret;

    if (!buf) {
        ESP_LOGE(TAG, "Failed to allocate memory of %d bytes!", req->content_len + 1);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    while (off < req->content_len) {
        /* Read data received in the request */
        ret = httpd_req_recv(req, buf + off, req->content_len - off);
        if (ret <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                httpd_resp_send_408(req);
            }
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
        if (!req_hdr) {
            ESP_LOGE(TAG, "Failed to allocate memory of %d bytes!", hdr_len + 1);
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        httpd_req_get_hdr_value_str(req, "Custom", req_hdr, hdr_len + 1);

        /* Set as additional header for response packet */
        httpd_resp_set_hdr(req, "Custom", req_hdr);
    }
    httpd_resp_send(req, buf, req->content_len);
    free (req_hdr);
    free (buf);
    return ESP_OK;
}

static void adder_free_func(void *ctx)
{
    ESP_LOGI(TAG, "Custom Free Context function called");
    free(ctx);
}

/* Create a context, keep incrementing value in the context, by whatever was
 * received. Return the result
 */
static esp_err_t adder_post_handler(httpd_req_t *req)
{
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

static esp_err_t leftover_data_post_handler(httpd_req_t *req)
{
    /* Only echo the first 10 bytes of the request, leaving the rest of the
     * request data as is.
     */
    char buf[11];
    int  ret;

    /* Read data received in the request */
    ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    buf[ret] = '\0';
    ESP_LOGI(TAG, "leftover data handler read %s", buf);
    httpd_resp_send(req, buf, strlen(buf));
    return ESP_OK;
}

extern int httpd_default_send(httpd_handle_t hd, int sockfd, const char *buf, unsigned buf_len, int flags);

static void generate_async_resp(void *arg)
{
    char buf[250];
    struct async_resp_arg *resp_arg = (struct async_resp_arg *)arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
#define HTTPD_HDR_STR      "HTTP/1.1 200 OK\r\n"                   \
                           "Content-Type: text/html\r\n"           \
                           "Content-Length: %d\r\n"
#define STR "Hello Double World!"

    ESP_LOGI(TAG, "Executing queued work fd : %d", fd);

    snprintf(buf, sizeof(buf), HTTPD_HDR_STR,
         strlen(STR));
    httpd_default_send(hd, fd, buf, strlen(buf), 0);
    /* Space for sending additional headers based on set_header */
    httpd_default_send(hd, fd, "\r\n", strlen("\r\n"), 0);
    httpd_default_send(hd, fd, STR, strlen(STR), 0);
#undef STR
    free(arg);
}

static esp_err_t async_get_handler(httpd_req_t *req)
{
#define STR "Hello World!"
    httpd_resp_send(req, STR, strlen(STR));
    /* Also register a HTTPD Work which sends the same data on the same
     * socket again
     */
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    if (resp_arg->fd < 0) {
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Queuing work fd : %d", resp_arg->fd);
    httpd_queue_work(req->handle, generate_async_resp, resp_arg);
    return ESP_OK;
#undef STR
}


static const httpd_uri_t basic_handlers[] = {
    { .uri      = "/hello/type_html",
      .method   = HTTP_GET,
      .handler  = hello_type_get_handler,
      .user_ctx = NULL,
    },
    { .uri      = "/test_header",
      .method   = HTTP_GET,
      .handler  = test_header_get_handler,
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

static const int basic_handlers_no = sizeof(basic_handlers)/sizeof(httpd_uri_t);

static void register_basic_handlers(httpd_handle_t hd)
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

static httpd_handle_t test_httpd_start(void)
{
    pre_start_mem = esp_get_free_heap_size();
    httpd_handle_t hd;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    /* Modify this setting to match the number of test URI handlers */
    config.max_uri_handlers  = 9;
    config.server_port = 1234;

    /* This check should be a part of http_server */
    config.max_open_sockets = (CONFIG_LWIP_MAX_SOCKETS - 3);

    if (httpd_start(&hd, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Started HTTP server on port: '%d'", config.server_port);
        ESP_LOGI(TAG, "Max URI handlers: '%d'", config.max_uri_handlers);
        ESP_LOGI(TAG, "Max Open Sessions: '%d'", config.max_open_sockets);
        ESP_LOGI(TAG, "Max Header Length: '%d'", HTTPD_MAX_REQ_HDR_LEN);
        ESP_LOGI(TAG, "Max URI Length: '%d'", HTTPD_MAX_URI_LEN);
        ESP_LOGI(TAG, "Max Stack Size: '%d'", config.stack_size);
        return hd;
    }
    return NULL;
}

static void test_httpd_stop(httpd_handle_t hd)
{
    httpd_stop(hd);
    post_stop_mem = esp_get_free_heap_size();
    ESP_LOGI(TAG, "HTTPD Stop: Current free memory: %d", post_stop_mem);
}

httpd_handle_t start_tests(void)
{
    httpd_handle_t hd = test_httpd_start();
    if (hd) {
        register_basic_handlers(hd);
    }
    return hd;
}

void stop_tests(httpd_handle_t hd)
{
    ESP_LOGI(TAG, "Stopping httpd");
    test_httpd_stop(hd);
}
