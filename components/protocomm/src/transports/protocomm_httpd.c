/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_err.h>
#include <inttypes.h>
#include "esp_random.h"

#include <esp_http_server.h>

#include <protocomm.h>
#include <protocomm_httpd.h>

#include "protocomm_priv.h"

static const char *TAG = "protocomm_httpd";
static protocomm_t *pc_httpd; /* The global protocomm instance for HTTPD */
static bool pc_ext_httpd_handle_provided = false;
/* The socket session id, which is basically just the socket number */
static uint32_t sock_session_id = PROTOCOMM_NO_SESSION_ID;
/* Cookie session id, which is a random number passed through HTTP cookies */
static uint32_t cookie_session_id = PROTOCOMM_NO_SESSION_ID;

#define MAX_REQ_BODY_LEN 4096

static void protocomm_httpd_session_close(void *ctx)
{
    /* When a socket session closes, we just reset the sock_session_id value.
     * Thereafter, only cookie_session_id would get used to check if the subsequent
     * request is for the same session.
     */
    if (sock_session_id != PROTOCOMM_NO_SESSION_ID) {
        ESP_LOGW(TAG, "Resetting socket session id as socket %" PRId32 "was closed", sock_session_id);
        sock_session_id = PROTOCOMM_NO_SESSION_ID;
    }
}

static esp_err_t common_post_handler(httpd_req_t *req)
{
    esp_err_t ret;
    uint8_t *outbuf = NULL;
    char *req_body = NULL;
    const char *ep_name = NULL;
    ssize_t outlen;

    int cur_sock_session_id = httpd_req_to_sockfd(req);
    int cur_cookie_session_id = 0;
    char cookie_buf[20] = {0};
    bool same_session = false;

    /* Check if any cookie is available in the received headers */
    if (httpd_req_get_hdr_value_str(req, "Cookie", cookie_buf, sizeof(cookie_buf)) == ESP_OK) {
        ESP_LOGD(TAG, "Received cookie %s", cookie_buf);
        char session_cookie[20] = {0};
        snprintf(session_cookie, sizeof(session_cookie), "session=%" PRIu32, cookie_session_id);
        /* If a cookie is found, check it against the session id. If it matches,
         * it means that this is a continuation of the same session.
         */
        if (strcmp(session_cookie, cookie_buf) == 0) {
            ESP_LOGD(TAG, "Continuing Session %" PRIu32, cookie_session_id);
            /* If we reach here, it means that the client supports cookies and so the
             * socket session id would no more be required for checking.
             */
            sock_session_id = PROTOCOMM_NO_SESSION_ID;
            same_session = true;
        }
    } else if (cur_sock_session_id == sock_session_id) {
        /* If the socket number matches, we assume it to be the same session */
        ESP_LOGD(TAG, "Continuing Socket Session %" PRIu32, sock_session_id);
        same_session = true;
    }
    if (!same_session) {
        /* If the received request is not a continuation of an existing session,
         * first close any existing sessions as applicable.
         */
        if (cookie_session_id != PROTOCOMM_NO_SESSION_ID) {
            ESP_LOGW(TAG, "Closing session with ID: %" PRIu32, cookie_session_id);
            if (pc_httpd->sec && pc_httpd->sec->close_transport_session) {
                ret = pc_httpd->sec->close_transport_session(pc_httpd->sec_inst, cookie_session_id);
                if (ret != ESP_OK) {
                    ESP_LOGW(TAG, "Error closing session with ID: %" PRIu32, cookie_session_id);
                }
            }
            cookie_session_id = PROTOCOMM_NO_SESSION_ID;
            sock_session_id = PROTOCOMM_NO_SESSION_ID;
        }
        /* Initialize new security session. A random number will be assigned to the session */
        cur_cookie_session_id = esp_random();
        ESP_LOGD(TAG, "Creating new session: %u", cur_cookie_session_id);
        if (pc_httpd->sec && pc_httpd->sec->new_transport_session) {
            ret = pc_httpd->sec->new_transport_session(pc_httpd->sec_inst, cur_cookie_session_id);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to launch new session with ID: %u", cur_cookie_session_id);
                ret = ESP_FAIL;
                goto out;
            }
            req->sess_ctx = pc_httpd->sec_inst;
            req->free_ctx = protocomm_httpd_session_close;

        }
        cookie_session_id = cur_cookie_session_id;
        sock_session_id = cur_sock_session_id;
        ESP_LOGD(TAG, "New socket session ID: %" PRId32, sock_session_id);
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
        if (ret <= 0) {
            ret = ESP_FAIL;
            goto out;
        }
        recv_size += ret;
    }

    /* Extract the endpoint name from URI string of type "/ep_name" */
    ep_name = req->uri + 1;

    ret = protocomm_req_handle(pc_httpd, ep_name, cookie_session_id,
                               (uint8_t *)req_body, recv_size, &outbuf, &outlen);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Data handler failed");
        ret = ESP_FAIL;
        goto out;
    }
    /* If this is a new session, send the session id in a cookie */
    if (!same_session) {
        snprintf(cookie_buf, sizeof(cookie_buf), "session=%" PRIu32, cookie_session_id);
        ESP_LOGD(TAG, "Setting cookie %s", cookie_buf);
        httpd_resp_set_hdr(req, "Set-Cookie", cookie_buf);
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

static esp_err_t protocomm_httpd_add_endpoint(const char *ep_name,
                                       protocomm_req_handler_t req_handler,
                                       void *priv_data)
{
    if (pc_httpd == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    ESP_LOGD(TAG, "Adding endpoint : %s", ep_name);

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

    ESP_LOGD(TAG, "Removing endpoint : %s", ep_name);

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

    if (config->ext_handle_provided) {
        if (config->data.handle) {
            pc->priv = config->data.handle;
            pc_ext_httpd_handle_provided = true;
        } else {
            return ESP_ERR_INVALID_ARG;
        }
    } else {
        /* Private data will point to the HTTP server handle */
        pc->priv = calloc(1, sizeof(httpd_handle_t));
        if (!pc->priv) {
            ESP_LOGE(TAG, "Malloc failed for HTTP server handle");
            return ESP_ERR_NO_MEM;
        }

        /* Configure the HTTP server */
        httpd_config_t server_config   = HTTPD_DEFAULT_CONFIG();
        server_config.server_port      = config->data.config.port;
        server_config.stack_size       = config->data.config.stack_size;
        server_config.task_priority    = config->data.config.task_priority;
        server_config.lru_purge_enable = true;
        server_config.max_open_sockets = 1;

        esp_err_t err;
        if ((err = httpd_start((httpd_handle_t *)pc->priv, &server_config)) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to start http server: %s", esp_err_to_name(err));
            free(pc->priv);
            return err;
        }
    }
    pc->add_endpoint    = protocomm_httpd_add_endpoint;
    pc->remove_endpoint = protocomm_httpd_remove_endpoint;
    pc_httpd = pc;
    cookie_session_id = PROTOCOMM_NO_SESSION_ID;
    sock_session_id = PROTOCOMM_NO_SESSION_ID;
    return ESP_OK;
}

esp_err_t protocomm_httpd_stop(protocomm_t *pc)
{
    if ((pc != NULL) && (pc == pc_httpd)) {
        if (!pc_ext_httpd_handle_provided) {
            httpd_handle_t *server_handle = (httpd_handle_t *) pc_httpd->priv;
            esp_err_t ret = httpd_stop(*server_handle);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to stop http server");
                return ret;
            }
            free(server_handle);
        } else {
            pc_ext_httpd_handle_provided = false;
        }
        pc_httpd->priv = NULL;
        pc_httpd = NULL;
        cookie_session_id = PROTOCOMM_NO_SESSION_ID;
        sock_session_id = PROTOCOMM_NO_SESSION_ID;
        return ESP_OK;
    }
    return ESP_ERR_INVALID_ARG;
}
