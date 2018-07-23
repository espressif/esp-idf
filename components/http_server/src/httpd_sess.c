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
#include <esp_log.h>
#include <esp_err.h>

#include <http_server.h>
#include "httpd_priv.h"

static const char *TAG = "httpd_sess";

bool httpd_is_sess_available(struct httpd_data *hd)
{
    int i;
    for (i = 0; i < hd->config.max_open_sockets; i++) {
        if (hd->hd_sd[i].fd == -1) {
            return true;
        }
    }
    return false;
}

static struct sock_db *httpd_sess_get(struct httpd_data *hd, int newfd)
{
    int i;
    for (i = 0; i < hd->config.max_open_sockets; i++) {
        if (hd->hd_sd[i].fd == newfd) {
            return &hd->hd_sd[i];
        }
    }
    return NULL;
}

esp_err_t httpd_sess_new(struct httpd_data *hd, int newfd)
{
    ESP_LOGD(TAG, LOG_FMT("fd = %d"), newfd);

    if (httpd_sess_get(hd, newfd)) {
        ESP_LOGE(TAG, LOG_FMT("session already exists with fd = %d"), newfd);
        return ESP_FAIL;
    }

    int i;
    for (i = 0; i < hd->config.max_open_sockets; i++) {
        if (hd->hd_sd[i].fd == -1) {
            memset(&hd->hd_sd[i], 0, sizeof(hd->hd_sd[i]));
            hd->hd_sd[i].fd = newfd;
            hd->hd_sd[i].handle = (httpd_handle_t) hd;
            hd->hd_sd[i].send_fn = httpd_default_send;
            hd->hd_sd[i].recv_fn = httpd_default_recv;
            return ESP_OK;
        }
    }
    ESP_LOGD(TAG, LOG_FMT("unable to launch session for fd = %d"), newfd);
    return ESP_FAIL;
}

void *httpd_sess_get_ctx(httpd_handle_t handle, int sockfd)
{
    if (handle == NULL) {
        return NULL;
    }

    struct httpd_data *hd = (struct httpd_data *) handle;
    struct sock_db    *sd = httpd_sess_get(hd, sockfd);
    if (sd == NULL) {
        return NULL;
    }

    return sd->ctx;
}

void httpd_sess_set_descriptors(struct httpd_data *hd,
                                fd_set *fdset, int *maxfd)
{
    int i;
    *maxfd = -1;
    for (i = 0; i < hd->config.max_open_sockets; i++) {
        if (hd->hd_sd[i].fd != -1) {
            FD_SET(hd->hd_sd[i].fd, fdset);
            if (hd->hd_sd[i].fd > *maxfd) {
                *maxfd = hd->hd_sd[i].fd;
            }
        }
    }
}

int httpd_sess_delete(struct httpd_data *hd, int fd)
{
    ESP_LOGD(TAG, LOG_FMT("fd = %d"), fd);
    int i;
    int pre_sess_fd = -1;
    for (i = 0; i < hd->config.max_open_sockets; i++) {
        if (hd->hd_sd[i].fd == fd) {
            hd->hd_sd[i].fd = -1;
            if (hd->hd_sd[i].ctx) {
                if (hd->hd_sd[i].free_ctx) {
                    hd->hd_sd[i].free_ctx(hd->hd_sd[i].ctx);
                } else {
                    free(hd->hd_sd[i].ctx);
                }
                hd->hd_sd[i].ctx = NULL;
                hd->hd_sd[i].free_ctx = NULL;
            }
            break;
        } else if (hd->hd_sd[i].fd != -1) {
            /* Return the fd just preceding the one being
             * deleted so that iterator can continue from
             * the correct fd */
            pre_sess_fd = hd->hd_sd[i].fd;
        }
    }
    return pre_sess_fd;
}

void httpd_sess_init(struct httpd_data *hd)
{
    int i;
    for (i = 0; i < hd->config.max_open_sockets; i++) {
        hd->hd_sd[i].fd = -1;
        hd->hd_sd[i].ctx = NULL;
    }
}

bool httpd_sess_pending(struct httpd_data *hd, int fd)
{
    struct sock_db *sd = httpd_sess_get(hd, fd);
    if (! sd) {
        return ESP_FAIL;
    }

    return (sd->pending_len != 0);
}

/* This MUST return ESP_OK on successful execution. If any other
 * value is returned, everything related to this socket will be
 * cleaned up and the socket will be closed.
 */
esp_err_t httpd_sess_process(struct httpd_data *hd, int newfd)
{
    struct sock_db *sd = httpd_sess_get(hd, newfd);
    if (! sd) {
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, LOG_FMT("httpd_req_new"));
    if (httpd_req_new(hd, sd) != ESP_OK) {
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, LOG_FMT("httpd_req_delete"));
    if (httpd_req_delete(hd) != ESP_OK) {
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, LOG_FMT("success"));
    sd->timestamp = httpd_os_get_timestamp();
    return ESP_OK;
}

esp_err_t httpd_sess_update_timestamp(httpd_handle_t handle, int sockfd)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Search for the socket database entry */
    struct httpd_data *hd = (struct httpd_data *) handle;
    int i;
    for (i = 0; i < hd->config.max_open_sockets; i++) {
        if (hd->hd_sd[i].fd == sockfd) {
            hd->hd_sd[i].timestamp = httpd_os_get_timestamp();
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t httpd_sess_close_lru(struct httpd_data *hd)
{
    int64_t timestamp = INT64_MAX;
    int lru_fd = -1;
    int i;
    for (i = 0; i < hd->config.max_open_sockets; i++) {
        /* If a descriptor is -1, there is no need to close any session.
         * So, we can return from here, without finding the Least Recently Used
         * session
         */
        if (hd->hd_sd[i].fd == -1) {
            return ESP_OK;
        }
        if (hd->hd_sd[i].timestamp < timestamp) {
            timestamp = hd->hd_sd[i].timestamp;
            lru_fd = hd->hd_sd[i].fd;
        }
    }
    ESP_LOGD(TAG, LOG_FMT("fd = %d"), lru_fd);
    return httpd_trigger_sess_close(hd, lru_fd);
}

int httpd_sess_iterate(struct httpd_data *hd, int start_fd)
{
    int start_index = 0;
    int i;

    if (start_fd != -1) {
        /* Take our index to where this fd is stored */
        for (i = 0; i < hd->config.max_open_sockets; i++) {
            if (hd->hd_sd[i].fd == start_fd) {
                start_index = i + 1;
                break;
            }
        }
    }

    for (i = start_index; i < hd->config.max_open_sockets; i++) {
        if (hd->hd_sd[i].fd != -1) {
            return hd->hd_sd[i].fd;
        }
    }
    return -1;
}

static void httpd_sess_close(void *arg)
{
    struct sock_db *sock_db = (struct sock_db *)arg;
    if (sock_db) {
        int fd = sock_db->fd;
        struct httpd_data *hd = (struct httpd_data *) sock_db->handle;
        httpd_sess_delete(hd, fd);
        close(fd);
    }
}

esp_err_t httpd_trigger_sess_close(httpd_handle_t handle, int sockfd)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    struct httpd_data *hd = (struct httpd_data *) handle;
    struct sock_db *sock_db = httpd_sess_get(hd, sockfd);
    if (sock_db) {
        return httpd_queue_work(handle, httpd_sess_close, sock_db);
    }

    return ESP_ERR_NOT_FOUND;
}
