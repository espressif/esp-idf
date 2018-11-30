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


#include <errno.h>
#include <esp_log.h>
#include <esp_err.h>
#include <http_parser.h>

#include <esp_http_server.h>
#include "esp_httpd_priv.h"

static const char *TAG = "httpd_uri";

static int httpd_find_uri_handler(struct httpd_data *hd,
                                  const char* uri,
                                  httpd_method_t method)
{
    for (int i = 0; i < hd->config.max_uri_handlers; i++) {
        if (hd->hd_calls[i]) {
            ESP_LOGD(TAG, LOG_FMT("[%d] = %s"), i, hd->hd_calls[i]->uri);
            if ((hd->hd_calls[i]->method == method) &&          // First match methods
                (strcmp(hd->hd_calls[i]->uri, uri) == 0)) {     // Then match uri strings
                return i;
            }
        }
    }
    return -1;
}

esp_err_t httpd_register_uri_handler(httpd_handle_t handle,
                                     const httpd_uri_t *uri_handler)
{
    if (handle == NULL || uri_handler == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    struct httpd_data *hd = (struct httpd_data *) handle;

    /* Make sure another handler with same URI and method
     * is not already registered
     */
    if (httpd_find_uri_handler(handle, uri_handler->uri,
                               uri_handler->method) != -1) {
        ESP_LOGW(TAG, LOG_FMT("handler %s with method %d already registered"),
                 uri_handler->uri, uri_handler->method);
        return ESP_ERR_HTTPD_HANDLER_EXISTS;
    }

    for (int i = 0; i < hd->config.max_uri_handlers; i++) {
        if (hd->hd_calls[i] == NULL) {
            hd->hd_calls[i] = malloc(sizeof(httpd_uri_t));
            if (hd->hd_calls[i] == NULL) {
                /* Failed to allocate memory */
                return ESP_ERR_HTTPD_ALLOC_MEM;
            }

            /* Copy URI string */
            hd->hd_calls[i]->uri = strdup(uri_handler->uri);
            if (hd->hd_calls[i]->uri == NULL) {
                /* Failed to allocate memory */
                free(hd->hd_calls[i]);
                return ESP_ERR_HTTPD_ALLOC_MEM;
            }

            /* Copy remaining members */
            hd->hd_calls[i]->method   = uri_handler->method;
            hd->hd_calls[i]->handler  = uri_handler->handler;
            hd->hd_calls[i]->user_ctx = uri_handler->user_ctx;
            ESP_LOGD(TAG, LOG_FMT("[%d] installed %s"), i, uri_handler->uri);
            return ESP_OK;
        }
        ESP_LOGD(TAG, LOG_FMT("[%d] exists %s"), i, hd->hd_calls[i]->uri);
    }
    ESP_LOGW(TAG, LOG_FMT("no slots left for registering handler"));
    return ESP_ERR_HTTPD_HANDLERS_FULL;
}

esp_err_t httpd_unregister_uri_handler(httpd_handle_t handle,
                                       const char *uri, httpd_method_t method)
{
    if (handle == NULL || uri == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    struct httpd_data *hd = (struct httpd_data *) handle;
    int i = httpd_find_uri_handler(hd, uri, method);

    if (i != -1) {
        ESP_LOGD(TAG, LOG_FMT("[%d] removing %s"), i, hd->hd_calls[i]->uri);

        free((char*)hd->hd_calls[i]->uri);
        free(hd->hd_calls[i]);
        hd->hd_calls[i] = NULL;
        return ESP_OK;
    }
    ESP_LOGW(TAG, LOG_FMT("handler %s with method %d not found"), uri, method);
    return ESP_ERR_NOT_FOUND;
}

esp_err_t httpd_unregister_uri(httpd_handle_t handle, const char *uri)
{
    if (handle == NULL || uri == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    struct httpd_data *hd = (struct httpd_data *) handle;
    bool found = false;

    for (int i = 0; i < hd->config.max_uri_handlers; i++) {
        if ((hd->hd_calls[i] != NULL) &&
            (strcmp(hd->hd_calls[i]->uri, uri) == 0)) {
            ESP_LOGD(TAG, LOG_FMT("[%d] removing %s"), i, uri);

            free((char*)hd->hd_calls[i]->uri);
            free(hd->hd_calls[i]);
            hd->hd_calls[i] = NULL;
            found = true;
        }
    }
    if (!found) {
        ESP_LOGW(TAG, LOG_FMT("no handler found for URI %s"), uri);
    }
    return (found ? ESP_OK : ESP_ERR_NOT_FOUND);
}

void httpd_unregister_all_uri_handlers(struct httpd_data *hd)
{
    for (unsigned i = 0; i < hd->config.max_uri_handlers; i++) {
        if (hd->hd_calls[i]) {
            ESP_LOGD(TAG, LOG_FMT("[%d] removing %s"), i, hd->hd_calls[i]->uri);

            free((char*)hd->hd_calls[i]->uri);
            free(hd->hd_calls[i]);
        }
    }
}

/* Alternate implmentation of httpd_find_uri_handler()
 * which takes a uri_len field. This is useful when the URI
 * string contains extra parameters that are not to be included
 * while matching with the registered URI_handler strings
 */
static httpd_uri_t* httpd_find_uri_handler2(httpd_err_resp_t *err,
                                            struct httpd_data *hd,
                                            const char *uri, size_t uri_len,
                                            httpd_method_t method)
{
    *err = 0;
    for (int i = 0; i < hd->config.max_uri_handlers; i++) {
        if (hd->hd_calls[i]) {
            ESP_LOGD(TAG, LOG_FMT("[%d] = %s"), i, hd->hd_calls[i]->uri);
            if ((strlen(hd->hd_calls[i]->uri) == uri_len) &&            // First match uri length
                (strncmp(hd->hd_calls[i]->uri, uri, uri_len) == 0))  {  // Then match uri strings
                if (hd->hd_calls[i]->method == method)  {               // Finally match methods
                    return hd->hd_calls[i];
                }
                /* URI found but method not allowed.
                 * If URI IS found later then this
                 * error is to be neglected */
                *err = HTTPD_405_METHOD_NOT_ALLOWED;
            }
        }
    }
    if (*err == 0) {
        *err = HTTPD_404_NOT_FOUND;
    }
    return NULL;
}

esp_err_t httpd_uri(struct httpd_data *hd)
{
    httpd_uri_t            *uri = NULL;
    httpd_req_t            *req = &hd->hd_req;
    struct http_parser_url *res = &hd->hd_req_aux.url_parse_res;

    /* For conveying URI not found/method not allowed */
    httpd_err_resp_t err = 0;

    ESP_LOGD(TAG, LOG_FMT("request for %s with type %d"), req->uri, req->method);
    /* URL parser result contains offset and length of path string */
    if (res->field_set & (1 << UF_PATH)) {
        uri = httpd_find_uri_handler2(&err, hd,
                                      req->uri + res->field_data[UF_PATH].off,
                                      res->field_data[UF_PATH].len,
                                      req->method);
    }

    /* If URI with method not found, respond with error code */
    if (uri == NULL) {
        switch (err) {
            case HTTPD_404_NOT_FOUND:
                ESP_LOGW(TAG, LOG_FMT("URI '%s' not found"), req->uri);
                return httpd_resp_send_err(req, HTTPD_404_NOT_FOUND);
            case HTTPD_405_METHOD_NOT_ALLOWED:
                ESP_LOGW(TAG, LOG_FMT("Method '%d' not allowed for URI '%s'"), req->method, req->uri);
                return httpd_resp_send_err(req, HTTPD_405_METHOD_NOT_ALLOWED);
            default:
                return ESP_FAIL;
        }
    }

    /* Attach user context data (passed during URI registration) into request */
    req->user_ctx = uri->user_ctx;

    /* Invoke handler */
    if (uri->handler(req) != ESP_OK) {
        /* Handler returns error, this socket should be closed */
        ESP_LOGW(TAG, LOG_FMT("uri handler execution failed"));
        return ESP_FAIL;
    }
    return ESP_OK;
}
