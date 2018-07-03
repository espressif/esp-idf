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

#include <string.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <errno.h>
#include <esp_log.h>
#include <esp_err.h>
#include <assert.h>

#include <http_server.h>
#include "httpd_priv.h"
#include "ctrl_sock.h"

static const char *TAG = "httpd";

static esp_err_t httpd_accept_conn(struct httpd_data *hd, int listen_fd)
{
    /* If no space is available for new session, close the least recently used one */
    if (hd->config.lru_purge_enable == true) {
        if (!httpd_is_sess_available(hd)) {
            /* Queue asynchronous closure of the least recently used session */
            return httpd_sess_close_lru(hd);
            /* Returning from this allowes the main server thread to process
             * the queued asynchronous control message for closing LRU session.
             * Since connection request hasn't been addressed yet using accept()
             * therefore httpd_accept_conn() will be called again, but this time
             * with space available for one session
             */
       }
    }

    struct sockaddr_in addr_from;
    socklen_t addr_from_len = sizeof(addr_from);
    int new_fd = accept(listen_fd, (struct sockaddr *)&addr_from, &addr_from_len);
    if (new_fd < 0) {
        ESP_LOGW(TAG, LOG_FMT("error in accept (%d)"), errno);
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, LOG_FMT("newfd = %d"), new_fd);

    struct timeval tv;
    /* Set recv timeout of this fd as per config */
    tv.tv_sec = hd->config.recv_wait_timeout;
    tv.tv_usec = 0;
    setsockopt(new_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    /* Set send timeout of this fd as per config */
    tv.tv_sec = hd->config.send_wait_timeout;
    tv.tv_usec = 0;
    setsockopt(new_fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));

    if (httpd_sess_new(hd, new_fd)) {
        ESP_LOGW(TAG, LOG_FMT("no slots left for launching new session"));
        close(new_fd);
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, LOG_FMT("complete"));
    return ESP_OK;
}

struct httpd_ctrl_data {
    enum httpd_ctrl_msg {
        HTTPD_CTRL_SHUTDOWN,
        HTTPD_CTRL_WORK,
    } hc_msg;
    httpd_work_fn_t hc_work;
    void *hc_work_arg;
};

esp_err_t httpd_queue_work(httpd_handle_t handle, httpd_work_fn_t work, void *arg)
{
    if (handle == NULL || work == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    struct httpd_data *hd = (struct httpd_data *) handle;
    struct httpd_ctrl_data msg = {
        .hc_msg = HTTPD_CTRL_WORK,
        .hc_work = work,
        .hc_work_arg = arg,
    };

    int ret = cs_send_to_ctrl_sock(hd->msg_fd, hd->config.ctrl_port, &msg, sizeof(msg));
    if (ret < 0) {
        ESP_LOGW(TAG, LOG_FMT("failed to queue work"));
        return ESP_FAIL;
    }

    return ESP_OK;
}

static void httpd_close_all_sessions(struct httpd_data *hd)
{
    int fd = -1;
    while ((fd = httpd_sess_iterate(hd, fd)) != -1) {
        ESP_LOGD(TAG, LOG_FMT("cleaning up socket %d"), fd);
        httpd_sess_delete(hd, fd);
        close(fd);
    }
}

static void httpd_process_ctrl_msg(struct httpd_data *hd)
{
    struct httpd_ctrl_data msg;
    int ret = recv(hd->ctrl_fd, &msg, sizeof(msg), 0);
    if (ret <= 0) {
        ESP_LOGW(TAG, LOG_FMT("error in recv (%d)"), errno);
        return;
    }
    if (ret != sizeof(msg)) {
        ESP_LOGW(TAG, LOG_FMT("incomplete msg"));
        return;
    }

    switch (msg.hc_msg) {
    case HTTPD_CTRL_WORK:
        if (msg.hc_work) {
            ESP_LOGD(TAG, LOG_FMT("work"));
            (*msg.hc_work)(msg.hc_work_arg);
        }
        break;
    case HTTPD_CTRL_SHUTDOWN:
        ESP_LOGD(TAG, LOG_FMT("shutdown"));
        hd->hd_td.status = THREAD_STOPPING;
        break;
    default:
        break;
    }
}

/* Manage in-coming connection or data requests */
static esp_err_t httpd_server(struct httpd_data *hd)
{
    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(hd->listen_fd, &read_set);
    FD_SET(hd->ctrl_fd, &read_set);

    int tmp_max_fd;
    httpd_sess_set_descriptors(hd, &read_set, &tmp_max_fd);
    int maxfd = MAX(hd->listen_fd, tmp_max_fd);
    tmp_max_fd = maxfd;
    maxfd = MAX(hd->ctrl_fd, tmp_max_fd);

    ESP_LOGD(TAG, LOG_FMT("doing select maxfd+1 = %d"), maxfd + 1);
    int active_cnt = select(maxfd + 1, &read_set, NULL, NULL, NULL);
    if (active_cnt < 0) {
        ESP_LOGE(TAG, LOG_FMT("error in select (%d)"), errno);
        /* Assert, as it's not possible to recover from this point onwards,
         * and there is no way to notify the main thread that server handle
         * has become invalid */
        assert(false);
        return ESP_FAIL;
    }

    /* Case0: Do we have a control message? */
    if (FD_ISSET(hd->ctrl_fd, &read_set)) {
        ESP_LOGD(TAG, LOG_FMT("processing ctrl message"));
        httpd_process_ctrl_msg(hd);
        if (hd->hd_td.status == THREAD_STOPPING) {
            ESP_LOGD(TAG, LOG_FMT("stopping thread"));
            return ESP_FAIL;
        }
    }

    /* Case1: Do we have any activity on the current data
     * sessions? */
    int fd = -1;
    while ((fd = httpd_sess_iterate(hd, fd)) != -1) {
        if (FD_ISSET(fd, &read_set) || (httpd_sess_pending(hd, fd))) {
            ESP_LOGD(TAG, LOG_FMT("processing socket %d"), fd);
            if (httpd_sess_process(hd, fd) != ESP_OK) {
                ESP_LOGD(TAG, LOG_FMT("closing socket %d"), fd);
                close(fd);
                /* Delete session and update fd to that
                 * preceding the one being deleted */
                fd = httpd_sess_delete(hd, fd);
            }
        }
    }

    /* Case2: Do we have any incoming connection requests to
     * process? */
    if (FD_ISSET(hd->listen_fd, &read_set)) {
        ESP_LOGD(TAG, LOG_FMT("processing listen socket %d"), hd->listen_fd);
        if (httpd_accept_conn(hd, hd->listen_fd) != ESP_OK) {
            ESP_LOGW(TAG, LOG_FMT("error accepting new connection"));
        }
    }
    return ESP_OK;
}

/* The main HTTPD thread */
static void httpd_thread(void *arg)
{
    int ret;
    struct httpd_data *hd = (struct httpd_data *) arg;
    hd->hd_td.status = THREAD_RUNNING;

    ESP_LOGD(TAG, LOG_FMT("web server started"));
    while (1) {
        ret = httpd_server(hd);
        if (ret != ESP_OK) {
            break;
        }
    }

    ESP_LOGD(TAG, LOG_FMT("web server exiting"));
    close(hd->msg_fd);
    cs_free_ctrl_sock(hd->ctrl_fd);
    httpd_close_all_sessions(hd);
    close(hd->listen_fd);
    hd->hd_td.status = THREAD_STOPPED;
    httpd_os_thread_delete();
}

static esp_err_t httpd_server_init(struct httpd_data *hd)
{
    int fd = socket(PF_INET6, SOCK_STREAM, 0);
    if (fd < 0) {
        ESP_LOGE(TAG, LOG_FMT("error in socket (%d)"), errno);
        return ESP_FAIL;
    }

    struct in6_addr inaddr_any = IN6ADDR_ANY_INIT;
    struct sockaddr_in6 serv_addr = {
        .sin6_family  = PF_INET6,
        .sin6_addr    = inaddr_any,
        .sin6_port    = htons(hd->config.server_port)
    };

    int ret = bind(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret < 0) {
        ESP_LOGE(TAG, LOG_FMT("error in bind (%d)"), errno);
        close(fd);
        return ESP_FAIL;
    }

    ret = listen(fd, hd->config.backlog_conn);
    if (ret < 0) {
        ESP_LOGE(TAG, LOG_FMT("error in listen (%d)"), errno);
        close(fd);
        return ESP_FAIL;
    }

    int ctrl_fd = cs_create_ctrl_sock(hd->config.ctrl_port);
    if (ctrl_fd < 0) {
        ESP_LOGE(TAG, LOG_FMT("error in creating ctrl socket (%d)"), errno);
        close(fd);
        return ESP_FAIL;
    }

    int msg_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (msg_fd < 0) {
        ESP_LOGE(TAG, LOG_FMT("error in creating msg socket (%d)"), errno);
        close(fd);
        close(ctrl_fd);
        return ESP_FAIL;
    }

    hd->listen_fd = fd;
    hd->ctrl_fd = ctrl_fd;
    hd->msg_fd  = msg_fd;
    return ESP_OK;
}

static struct httpd_data *httpd_create(const httpd_config_t *config)
{
    /* Allocate memory for httpd instance data */
    struct httpd_data *hd = calloc(1, sizeof(struct httpd_data));
    if (hd != NULL) {
        hd->hd_calls = calloc(config->max_uri_handlers, sizeof(httpd_uri_t *));
        if (hd->hd_calls == NULL) {
            free(hd);
            return NULL;
        }
        hd->hd_sd = calloc(config->max_open_sockets, sizeof(struct sock_db));
        if (hd->hd_sd == NULL) {
            free(hd->hd_calls);
            free(hd);
            return NULL;
        }
        struct httpd_req_aux *ra = &hd->hd_req_aux;
        ra->resp_hdrs = calloc(config->max_resp_headers, sizeof(struct resp_hdr));
        if (ra->resp_hdrs == NULL) {
            free(hd->hd_sd);
            free(hd->hd_calls);
            free(hd);
            return NULL;
        }
        /* Save the configuration for this instance */
        hd->config = *config;
    } else {
        ESP_LOGE(TAG, "mem alloc failed");
    }
    return hd;
}

static void httpd_delete(struct httpd_data *hd)
{
    struct httpd_req_aux *ra = &hd->hd_req_aux;
    /* Free memory of httpd instance data */
    free(ra->resp_hdrs);
    free(hd->hd_sd);

    /* Free registered URI handlers */
    httpd_unregister_all_uri_handlers(hd);
    free(hd->hd_calls);
    free(hd);
}

esp_err_t httpd_start(httpd_handle_t *handle, const httpd_config_t *config)
{
    if (handle == NULL || config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    struct httpd_data *hd = httpd_create(config);
    if (hd == NULL) {
        /* Failed to allocate memory */
        return ESP_ERR_HTTPD_ALLOC_MEM;
    }

    if (httpd_server_init(hd) != ESP_OK) {
        httpd_delete(hd);
        return ESP_FAIL;
    }

    httpd_sess_init(hd);
    if (httpd_os_thread_create(&hd->hd_td.handle, "httpd",
                               hd->config.stack_size,
                               hd->config.task_priority,
                               httpd_thread, hd) != ESP_OK) {
        /* Failed to launch task */
        httpd_delete(hd);
        return ESP_ERR_HTTPD_TASK;
    }

    *handle = (httpd_handle_t *)hd;
    return ESP_OK;
}

esp_err_t httpd_stop(httpd_handle_t handle)
{
    struct httpd_data *hd = (struct httpd_data *) handle;
    if (hd == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    struct httpd_ctrl_data msg;
    memset(&msg, 0, sizeof(msg));
    msg.hc_msg = HTTPD_CTRL_SHUTDOWN;
    cs_send_to_ctrl_sock(hd->msg_fd, hd->config.ctrl_port, &msg, sizeof(msg));

    ESP_LOGD(TAG, LOG_FMT("sent control msg to stop server"));
    while (hd->hd_td.status != THREAD_STOPPED) {
        httpd_os_thread_sleep(1000);
    }

    ESP_LOGD(TAG, LOG_FMT("server stopped"));
    httpd_delete(hd);
    return ESP_OK;
}
