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

#include <esp_http_server.h>
#include "esp_httpd_priv.h"

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

struct sock_db *httpd_sess_get(struct httpd_data *hd, int sockfd)
{
    if (hd == NULL) {
        return NULL;
    }

    /* Check if called inside a request handler, and the
     * session sockfd in use is same as the parameter */
    if ((hd->hd_req_aux.sd) && (hd->hd_req_aux.sd->fd == sockfd)) {
        /* Just return the pointer to the sock_db
         * corresponding to the request */
        return hd->hd_req_aux.sd;
    }

    int i;
    for (i = 0; i < hd->config.max_open_sockets; i++) {
        if (hd->hd_sd[i].fd == sockfd) {
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

            /* Call user-defined session opening function */
            if (hd->config.open_fn) {
                esp_err_t ret = hd->config.open_fn(hd, hd->hd_sd[i].fd);
                if (ret != ESP_OK) {
                    httpd_sess_delete(hd, hd->hd_sd[i].fd);
                    ESP_LOGD(TAG, LOG_FMT("open_fn failed for fd = %d"), newfd);
                    return ret;
                }
            }
            return ESP_OK;
        }
    }
    ESP_LOGD(TAG, LOG_FMT("unable to launch session for fd = %d"), newfd);
    return ESP_FAIL;
}

void httpd_sess_free_ctx(void *ctx, httpd_free_ctx_fn_t free_fn)
{
    if (ctx) {
        if (free_fn) {
            free_fn(ctx);
        } else {
            free(ctx);
        }
    }
}

void *httpd_sess_get_ctx(httpd_handle_t handle, int sockfd)
{
    struct sock_db *sd = httpd_sess_get(handle, sockfd);
    if (sd == NULL) {
        return NULL;
    }

    /* Check if the function has been called from inside a
     * request handler, in which case fetch the context from
     * the httpd_req_t structure */
    struct httpd_data *hd = (struct httpd_data *) handle;
    if (hd->hd_req_aux.sd == sd) {
        return hd->hd_req.sess_ctx;
    }

    return sd->ctx;
}

void httpd_sess_set_ctx(httpd_handle_t handle, int sockfd, void *ctx, httpd_free_ctx_fn_t free_fn)
{
    struct sock_db *sd = httpd_sess_get(handle, sockfd);
    if (sd == NULL) {
        return;
    }

    /* Check if the function has been called from inside a
     * request handler, in which case set the context inside
     * the httpd_req_t structure */
    struct httpd_data *hd = (struct httpd_data *) handle;
    if (hd->hd_req_aux.sd == sd) {
        if (hd->hd_req.sess_ctx != ctx) {
            /* Don't free previous context if it is in sockdb
             * as it will be freed inside httpd_req_cleanup() */
            if (sd->ctx != hd->hd_req.sess_ctx) {
                /* Free previous context */
                httpd_sess_free_ctx(hd->hd_req.sess_ctx, hd->hd_req.free_ctx);
            }
            hd->hd_req.sess_ctx = ctx;
        }
        hd->hd_req.free_ctx = free_fn;
        return;
    }

    /* Else set the context inside the sock_db structure */
    if (sd->ctx != ctx) {
        /* Free previous context */
        httpd_sess_free_ctx(sd->ctx, sd->free_ctx);
        sd->ctx = ctx;
    }
    sd->free_ctx = free_fn;
}

void *httpd_sess_get_transport_ctx(httpd_handle_t handle, int sockfd)
{
    struct sock_db *sd = httpd_sess_get(handle, sockfd);
    if (sd == NULL) {
        return NULL;
    }

    return sd->transport_ctx;
}

void httpd_sess_set_transport_ctx(httpd_handle_t handle, int sockfd, void *ctx, httpd_free_ctx_fn_t free_fn)
{
    struct sock_db *sd = httpd_sess_get(handle, sockfd);
    if (sd == NULL) {
        return;
    }

    if (sd->transport_ctx != ctx) {
        /* Free previous transport context */
        httpd_sess_free_ctx(sd->transport_ctx, sd->free_transport_ctx);
        sd->transport_ctx = ctx;
    }
    sd->free_transport_ctx = free_fn;
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

/** Check if a FD is valid */
static int fd_is_valid(int fd)
{
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

static inline uint64_t httpd_sess_get_lru_counter(void)
{
    static uint64_t lru_counter = 0;
    return ++lru_counter;
}

void httpd_sess_delete_invalid(struct httpd_data *hd)
{
    for (int i = 0; i < hd->config.max_open_sockets; i++) {
        if (hd->hd_sd[i].fd != -1 && !fd_is_valid(hd->hd_sd[i].fd)) {
            ESP_LOGW(TAG, LOG_FMT("Closing invalid socket %d"), hd->hd_sd[i].fd);
            httpd_sess_delete(hd, hd->hd_sd[i].fd);
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
            /* global close handler */
            if (hd->config.close_fn) {
                hd->config.close_fn(hd, fd);
            }

            /* release 'user' context */
            if (hd->hd_sd[i].ctx) {
                if (hd->hd_sd[i].free_ctx) {
                    hd->hd_sd[i].free_ctx(hd->hd_sd[i].ctx);
                } else {
                    free(hd->hd_sd[i].ctx);
                }
                hd->hd_sd[i].ctx = NULL;
                hd->hd_sd[i].free_ctx = NULL;
            }

            /* release 'transport' context */
            if (hd->hd_sd[i].transport_ctx) {
                if (hd->hd_sd[i].free_transport_ctx) {
                    hd->hd_sd[i].free_transport_ctx(hd->hd_sd[i].transport_ctx);
                } else {
                    free(hd->hd_sd[i].transport_ctx);
                }
                hd->hd_sd[i].transport_ctx = NULL;
                hd->hd_sd[i].free_transport_ctx = NULL;
            }

            /* mark session slot as available */
            hd->hd_sd[i].fd = -1;
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

    if (sd->pending_fn) {
        // test if there's any data to be read (besides read() function, which is handled by select() in the main httpd loop)
        // this should check e.g. for the SSL data buffer
        if (sd->pending_fn(hd, fd) > 0) {
            return true;
        }
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
    sd->lru_counter = httpd_sess_get_lru_counter();
    return ESP_OK;
}

esp_err_t httpd_sess_update_lru_counter(httpd_handle_t handle, int sockfd)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Search for the socket database entry */
    struct httpd_data *hd = (struct httpd_data *) handle;
    int i;
    for (i = 0; i < hd->config.max_open_sockets; i++) {
        if (hd->hd_sd[i].fd == sockfd) {
            hd->hd_sd[i].lru_counter = httpd_sess_get_lru_counter();
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t httpd_sess_close_lru(struct httpd_data *hd)
{
    uint64_t lru_counter = UINT64_MAX;
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
        if (hd->hd_sd[i].lru_counter < lru_counter) {
            lru_counter = hd->hd_sd[i].lru_counter;
            lru_fd = hd->hd_sd[i].fd;
        }
    }
    ESP_LOGD(TAG, LOG_FMT("fd = %d"), lru_fd);
    struct sock_db *sd = httpd_sess_get(hd, lru_fd);
    sd->lru_socket = true;
    return httpd_sess_trigger_close(hd, lru_fd);
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
        if (sock_db->lru_counter == 0 && !sock_db->lru_socket) {
            ESP_LOGD(TAG, "Skipping session close for %d as it seems to be a race condition", sock_db->fd);
            return;
        }
        int fd = sock_db->fd;
        sock_db->lru_socket = false;
        struct httpd_data *hd = (struct httpd_data *) sock_db->handle;
        httpd_sess_delete(hd, fd);
        close(fd);
    }
}

esp_err_t httpd_sess_trigger_close(httpd_handle_t handle, int sockfd)
{
    struct sock_db *sock_db = httpd_sess_get(handle, sockfd);
    if (sock_db) {
        return httpd_queue_work(handle, httpd_sess_close, sock_db);
    }

    return ESP_ERR_NOT_FOUND;
}
