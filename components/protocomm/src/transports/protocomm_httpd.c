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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_err.h>

#include <http_server.h>

#include <protocomm.h>
#include <protocomm_httpd.h>

#include "protocomm_priv.h"

static const char *TAG = "protocomm_httpd";
static protocomm_t *pc_httpd; /* The global protocomm instance for HTTPD */
static uint32_t session_id = PROTOCOMM_NO_SESSION_ID;

#define MAX_REQ_BODY_LEN 4096

static esp_err_t common_post_handler(httpd_req_t *req)
{
    esp_err_t ret;
    uint8_t *outbuf = NULL;
    char *req_body = NULL;
    const char *ep_name = NULL;
    ssize_t outlen;

    int cur_session_id = httpd_req_to_sockfd(req);

    if (cur_session_id != session_id) {
        /* Initialise new security session */
        if (session_id != PROTOCOMM_NO_SESSION_ID) {
            ESP_LOGV(TAG, "Closing session with ID: %d", session_id);
            /* Presently HTTP server doesn't support callback on socket closure so
             * previous session can only be closed when new session is requested */
            if (pc_httpd->sec && pc_httpd->sec->close_transport_session) {
                ret = pc_httpd->sec->close_transport_session(session_id);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to close session with ID: %d", session_id);
                    ret = ESP_FAIL;
                    goto out;
                }
            }
            session_id = PROTOCOMM_NO_SESSION_ID;
        }
        if (pc_httpd->sec && pc_httpd->sec->new_transport_session) {
            ret = pc_httpd->sec->new_transport_session(cur_session_id);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to launch new session with ID: %d", cur_session_id);
                ret = ESP_FAIL;
                goto out;
            }
        }
        session_id = cur_session_id;
        ESP_LOGV(TAG, "New session with ID: %d", cur_session_id);
    }

    if (req->content_len <= 0) {
        ESP_LOGE(TAG, "Content length not found");
        ret = ESP_FAIL;
        goto out;
    } else if (req->content_len > MAX_REQ_BODY_LEN) {
        ESP_LOGE(TAG, "Request content length should be less than 4kb");
        ret = ESP_FAIL;
        goto out;
    }

    req_body = (char *) malloc(req->content_len);
    if (!req_body) {
        ESP_LOGE(TAG, "Unable to allocate for request length %d", req->content_len);
        ret = ESP_ERR_NO_MEM;
        goto out;
    }

    size_t recv_size = 0;
    while (recv_size < req->content_len) {
        ret = httpd_req_recv(req, req_body + recv_size, req->content_len - recv_size);
        if (ret < 0) {
            ret = ESP_FAIL;
            goto out;
        }
        recv_size += ret;
    }

    /* Extract the endpoint name from URI string of type "/ep_name" */
    ep_name = req->uri + 1;

    ret = protocomm_req_handle(pc_httpd, ep_name, session_id,
                               (uint8_t *)req_body, recv_size, &outbuf, &outlen);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Data handler failed");
        ret = ESP_FAIL;
        goto out;
    }

    ret = httpd_resp_send(req, (char *)outbuf, outlen);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "HTTP send failed");
        ret = ESP_FAIL;
        goto out;
    }
    ret = ESP_OK;
out:
    if (req_body) {
        free(req_body);
    }
    if (outbuf) {
        free(outbuf);
    }
    return ret;
}

esp_err_t protocomm_httpd_add_endpoint(const char *ep_name,
                                       protocomm_req_handler_t req_handler,
                                       void *priv_data)
{
    if (pc_httpd == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    ESP_LOGV(TAG, "Adding endpoint : %s", ep_name);

    /* Construct URI name by prepending '/' to ep_name */
    char* ep_uri = calloc(1, strlen(ep_name) + 2);
    if (!ep_uri) {
        ESP_LOGE(TAG, "Malloc failed for ep uri");
        return ESP_ERR_NO_MEM;
    }

    /* Create URI handler structure */
    sprintf(ep_uri, "/%s", ep_name);
    httpd_uri_t config_handler = {
        .uri      = ep_uri,
        .method   = HTTP_POST,
        .handler  = common_post_handler,
        .user_ctx = NULL
    };

    /* Register URI handler */
    esp_err_t err;
    httpd_handle_t *server = (httpd_handle_t *) pc_httpd->priv;
    if ((err = httpd_register_uri_handler(*server, &config_handler)) != ESP_OK) {
        ESP_LOGE(TAG, "Uri handler register failed: %s", esp_err_to_name(err));
        free(ep_uri);
        return ESP_FAIL;
    }

    free(ep_uri);
    return ESP_OK;
}

static esp_err_t protocomm_httpd_remove_endpoint(const char *ep_name)
{
    if (pc_httpd == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    ESP_LOGV(TAG, "Removing endpoint : %s", ep_name);

    /* Construct URI name by prepending '/' to ep_name */
    char* ep_uri = calloc(1, strlen(ep_name) + 2);
    if (!ep_uri) {
        ESP_LOGE(TAG, "Malloc failed for ep uri");
        return ESP_ERR_NO_MEM;
    }
    sprintf(ep_uri, "/%s", ep_name);

    /* Unregister URI handler */
    esp_err_t err;
    httpd_handle_t *server = (httpd_handle_t *) pc_httpd->priv;
    if ((err = httpd_unregister_uri_handler(*server, ep_uri, HTTP_POST)) != ESP_OK) {
        ESP_LOGE(TAG, "Uri handler de-register failed: %s", esp_err_to_name(err));
        free(ep_uri);
        return ESP_FAIL;
    }

    free(ep_uri);
    return ESP_OK;
}

esp_err_t protocomm_httpd_start(protocomm_t *pc, const protocomm_httpd_config_t *config)
{
    if (!pc || !config) {
        return ESP_ERR_INVALID_ARG;
    }

    if (pc_httpd) {
        if (pc == pc_httpd) {
            ESP_LOGE(TAG, "HTTP server already running for this protocomm instance");
            return ESP_ERR_INVALID_STATE;
        }
        ESP_LOGE(TAG, "HTTP server started for another protocomm instance");
        return ESP_ERR_NOT_SUPPORTED;
    }

    /* Private data will point to the HTTP server handle */
    pc->priv = calloc(1, sizeof(httpd_handle_t));
    if (!pc->priv) {
        ESP_LOGE(TAG, "Malloc failed for HTTP server handle");
        return ESP_ERR_NO_MEM;
    }

    /* Configure the HTTP server */
    httpd_config_t server_config   = HTTPD_DEFAULT_CONFIG();
    server_config.server_port      = config->port;
    server_config.stack_size       = config->stack_size;
    server_config.task_priority    = config->task_priority;
    server_config.lru_purge_enable = true;
    server_config.max_open_sockets = 1;

    esp_err_t err;
    if ((err = httpd_start((httpd_handle_t *)pc->priv, &server_config)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start http server: %s", esp_err_to_name(err));
        free(pc->priv);
        return err;
    }

    pc->add_endpoint    = protocomm_httpd_add_endpoint;
    pc->remove_endpoint = protocomm_httpd_remove_endpoint;
    pc_httpd = pc;
    return ESP_OK;
}

esp_err_t protocomm_httpd_stop(protocomm_t *pc)
{
    if ((pc != NULL) && (pc == pc_httpd)) {
        httpd_handle_t *server_handle = (httpd_handle_t *) pc_httpd->priv;
        httpd_stop(*server_handle);
        free(server_handle);
        pc_httpd->priv = NULL;
        pc_httpd = NULL;
        return ESP_OK;
    }
    return ESP_ERR_INVALID_ARG;
}
