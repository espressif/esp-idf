/*
 * SPDX-FileCopyrightText: 2018-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <errno.h>
#include <esp_log.h>
#include <esp_err.h>
#include <assert.h>
#include <netinet/tcp.h>

#include <esp_http_server.h>
#include "esp_httpd_priv.h"
#include "ctrl_sock.h"
#if CONFIG_HTTPD_QUEUE_WORK_BLOCKING
#include "freertos/semphr.h"
#endif

#if defined(CONFIG_LWIP_MAX_SOCKETS)
#define HTTPD_MAX_SOCKETS CONFIG_LWIP_MAX_SOCKETS
#else
/* LwIP component is not included into the build, use a default value */
#define HTTPD_MAX_SOCKETS 15
#endif

static const int DEFAULT_KEEP_ALIVE_IDLE = 5;
static const int DEFAULT_KEEP_ALIVE_INTERVAL= 5;
static const int DEFAULT_KEEP_ALIVE_COUNT= 3;

typedef struct {
    fd_set *fdset;
    struct httpd_data *hd;
} process_session_context_t;

static const char *TAG = "httpd";

#ifdef CONFIG_HTTPD_ENABLE_EVENTS
ESP_EVENT_DEFINE_BASE(ESP_HTTP_SERVER_EVENT);

void esp_http_server_dispatch_event(int32_t event_id, const void* event_data, size_t event_data_size)
{
    esp_err_t err = esp_event_post(ESP_HTTP_SERVER_EVENT, event_id, event_data, event_data_size, ESP_HTTP_SERVER_EVENT_POST_TIMEOUT);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to post esp_http_server event: %s", esp_err_to_name(err));
    }
}
#endif // CONFIG_HTTPD_ENABLE_EVENTS

static esp_err_t httpd_accept_conn(struct httpd_data *hd, int listen_fd)
{
    /* If no space is available for new session, close the least recently used one */
    if (hd->config.lru_purge_enable == true) {
        if (!httpd_is_sess_available(hd)) {
            /* Queue asynchronous closure of the least recently used session */
#if CONFIG_HTTPD_QUEUE_WORK_BLOCKING
            /* In case of blocking mode, close the least recently used session directly */
            return httpd_sess_close_lru_direct(hd);
#else
            return httpd_sess_close_lru(hd);
#endif /* CONFIG_HTTPD_QUEUE_WORK_BLOCKING */
            /* Returning from this allows the main server thread to process
             * the queued asynchronous control message for closing LRU session.
             * Since connection request hasn't been addressed yet using accept()
             * therefore httpd_accept_conn() will be called again, but this time
             * with space available for one session
             */
        }
    }

    struct sockaddr_storage addr_from;
    socklen_t addr_from_len = sizeof(addr_from);
    int new_fd = accept(listen_fd, (struct sockaddr *)&addr_from, &addr_from_len);
    if (new_fd < 0) {
        ESP_LOGE(TAG, LOG_FMT("error in accept (%d)"), errno);
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, LOG_FMT("newfd = %d"), new_fd);

    struct timeval tv;
    /* Set recv timeout of this fd as per config */
    tv.tv_sec = hd->config.recv_wait_timeout;
    tv.tv_usec = 0;
    if (setsockopt(new_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv)) < 0) {
        ESP_LOGE(TAG, LOG_FMT("error in setsockopt SO_RCVTIMEO (%d)"), errno);
        goto exit;
    }

    /* Set send timeout of this fd as per config */
    tv.tv_sec = hd->config.send_wait_timeout;
    tv.tv_usec = 0;
    if (setsockopt(new_fd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, sizeof(tv)) < 0) {
        ESP_LOGE(TAG, LOG_FMT("error in setsockopt SO_SNDTIMEO (%d)"), errno);
        goto exit;
    }

    if (hd->config.keep_alive_enable) {
        int keep_alive_enable = 1;
        int keep_alive_idle = hd->config.keep_alive_idle ? hd->config.keep_alive_idle : DEFAULT_KEEP_ALIVE_IDLE;
        int keep_alive_interval = hd->config.keep_alive_interval ? hd->config.keep_alive_interval : DEFAULT_KEEP_ALIVE_INTERVAL;
        int keep_alive_count = hd->config.keep_alive_count ? hd->config.keep_alive_count : DEFAULT_KEEP_ALIVE_COUNT;
        ESP_LOGD(TAG, "Enable TCP keep alive. idle: %d, interval: %d, count: %d", keep_alive_idle, keep_alive_interval, keep_alive_count);

        if (setsockopt(new_fd, SOL_SOCKET, SO_KEEPALIVE, &keep_alive_enable, sizeof(keep_alive_enable)) < 0) {
            ESP_LOGE(TAG, LOG_FMT("error in setsockopt SO_KEEPALIVE (%d)"), errno);
            goto exit;
        }
#ifndef __APPLE__
        if (setsockopt(new_fd, IPPROTO_TCP, TCP_KEEPIDLE, &keep_alive_idle, sizeof(keep_alive_idle)) < 0) {
            ESP_LOGE(TAG, LOG_FMT("error in setsockopt TCP_KEEPIDLE (%d)"), errno);
            goto exit;
        }
        if (setsockopt(new_fd, IPPROTO_TCP, TCP_KEEPINTVL, &keep_alive_interval, sizeof(keep_alive_interval)) < 0) {
            ESP_LOGE(TAG, LOG_FMT("error in setsockopt TCP_KEEPINTVL (%d)"), errno);
            goto exit;
        }
        if (setsockopt(new_fd, IPPROTO_TCP, TCP_KEEPCNT, &keep_alive_count, sizeof(keep_alive_count)) < 0) {
            ESP_LOGE(TAG, LOG_FMT("error in setsockopt TCP_KEEPCNT (%d)"), errno);
            goto exit;
        }
#else // __APPLE__
        if (setsockopt(new_fd, IPPROTO_TCP, TCP_KEEPALIVE, &keep_alive_idle, sizeof(keep_alive_idle)) < 0) {
            ESP_LOGE(TAG, LOG_FMT("error in setsockopt TCP_KEEPALIVE (%d)"), errno);
            goto exit;
        }
#endif // __APPLE__
    }
    if (ESP_OK != httpd_sess_new(hd, new_fd)) {
        ESP_LOGE(TAG, LOG_FMT("session creation failed"));
        goto exit;
    }
    ESP_LOGD(TAG, LOG_FMT("complete"));
    hd->http_server_state = HTTP_SERVER_EVENT_ON_CONNECTED;
    esp_http_server_dispatch_event(HTTP_SERVER_EVENT_ON_CONNECTED, &new_fd, sizeof(int));
    return ESP_OK;
exit:
    close(new_fd);
    return ESP_FAIL;
}

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
#if CONFIG_HTTPD_QUEUE_WORK_BLOCKING
    // Semaphore is acquired here and released after work function is executed.
    if (xSemaphoreTake(hd->ctrl_sock_semaphore, portMAX_DELAY) == pdTRUE) {
#endif
        int ret = cs_send_to_ctrl_sock(hd->msg_fd, hd->config.ctrl_port, &msg, sizeof(msg));
        if (ret < 0) {
            ESP_LOGW(TAG, LOG_FMT("failed to queue work"));
#if CONFIG_HTTPD_QUEUE_WORK_BLOCKING
            xSemaphoreGive(hd->ctrl_sock_semaphore);
#endif
            return ESP_FAIL;
        }
        return ESP_OK;
#if CONFIG_HTTPD_QUEUE_WORK_BLOCKING
    }
    ESP_LOGE(TAG, "Unable to acquire semaphore");
    return ESP_FAIL;
#endif
}

esp_err_t httpd_get_client_list(httpd_handle_t handle, size_t *fds, int *client_fds)
{
    struct httpd_data *hd = (struct httpd_data *) handle;
    if (hd == NULL || fds == NULL || *fds == 0 || client_fds == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    size_t max_fds = *fds;
    *fds = 0;
    for (int i = 0; i < hd->config.max_open_sockets; ++i) {
        if (hd->hd_sd[i].fd != -1) {
            if (*fds < max_fds) {
                client_fds[(*fds)++] = hd->hd_sd[i].fd;
            } else {
                return ESP_ERR_INVALID_ARG;
            }
        }
    }
    return ESP_OK;
}

void *httpd_get_global_user_ctx(httpd_handle_t handle)
{
    return ((struct httpd_data *)handle)->config.global_user_ctx;
}

void *httpd_get_global_transport_ctx(httpd_handle_t handle)
{
    return ((struct httpd_data *)handle)->config.global_transport_ctx;
}


static void httpd_process_ctrl_msg(struct httpd_data *hd)
{
    struct httpd_ctrl_data msg;
    int ret = recv(hd->ctrl_fd, &msg, sizeof(msg), 0);
    if (ret <= 0) {
        ESP_LOGW(TAG, LOG_FMT("error in recv (%d)"), errno);
#if CONFIG_HTTPD_QUEUE_WORK_BLOCKING
        xSemaphoreGive(hd->ctrl_sock_semaphore);
#endif
        return;
    }
    if (ret != sizeof(msg)) {
        ESP_LOGW(TAG, LOG_FMT("incomplete msg"));
#if CONFIG_HTTPD_QUEUE_WORK_BLOCKING
        xSemaphoreGive(hd->ctrl_sock_semaphore);
#endif
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
#if CONFIG_HTTPD_QUEUE_WORK_BLOCKING
    xSemaphoreGive(hd->ctrl_sock_semaphore);
#endif
}

// Called for each session from httpd_server
static int httpd_process_session(struct sock_db *session, void *context)
{
    if ((!session) || (!context)) {
        return 0;
    }

    if (session->fd < 0) {
        return 1;
    }

    // session is busy in an async task, do not process here.
    if (session->for_async_req) {
        return 1;
    }

    process_session_context_t *ctx = (process_session_context_t *)context;
    int fd = session->fd;

    if (FD_ISSET(fd, ctx->fdset) || httpd_sess_pending(ctx->hd, session)) {
        ESP_LOGD(TAG, LOG_FMT("processing socket %d"), fd);
        if (httpd_sess_process(ctx->hd, session) != ESP_OK) {
            httpd_sess_delete(ctx->hd, session); // Delete session
        }
    }
    return 1;
}

/* Manage in-coming connection or data requests */
static esp_err_t httpd_server(struct httpd_data *hd)
{
    fd_set read_set;
    FD_ZERO(&read_set);
    if (hd->config.lru_purge_enable || httpd_is_sess_available(hd)) {
        /* Only listen for new connections if server has capacity to
         * handle more (or when LRU purge is enabled, in which case
         * older connections will be closed) */
        FD_SET(hd->listen_fd, &read_set);
    }
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
        httpd_sess_delete_invalid(hd);
        return ESP_OK;
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
    process_session_context_t context = {
        .fdset = &read_set,
        .hd = hd
    };
    httpd_sess_enum(hd, httpd_process_session, &context);

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
    httpd_sess_close_all(hd);
    close(hd->listen_fd);
    hd->hd_td.status = THREAD_STOPPED;
    httpd_os_thread_delete();
}

static esp_err_t httpd_server_init(struct httpd_data *hd)
{
#if CONFIG_LWIP_IPV6
    int fd = socket(PF_INET6, SOCK_STREAM, 0);
#else
    int fd = socket(PF_INET, SOCK_STREAM, 0);
#endif
    if (fd < 0) {
        ESP_LOGE(TAG, LOG_FMT("error in socket (%d)"), errno);
        return ESP_FAIL;
    }
#if CONFIG_LWIP_IPV6
    struct in6_addr inaddr_any = IN6ADDR_ANY_INIT;
    struct sockaddr_in6 serv_addr = {
        .sin6_family  = PF_INET6,
        .sin6_addr    = inaddr_any,
        .sin6_port    = htons(hd->config.server_port)
    };
#else
    struct sockaddr_in serv_addr = {
        .sin_family   = PF_INET,
        .sin_addr     = {
            .s_addr = htonl(INADDR_ANY)
        },
        .sin_port     = htons(hd->config.server_port)
    };
#endif
    /* Enable SO_REUSEADDR to allow binding to the same
     * address and port when restarting the server */
    int enable = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
        /* This will fail if CONFIG_LWIP_SO_REUSE is not enabled. But
         * it does not affect the normal working of the HTTP Server */
        ESP_LOGW(TAG, LOG_FMT("error in setsockopt SO_REUSEADDR (%d)"), errno);
    }

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
    if (!hd) {
        ESP_LOGE(TAG, LOG_FMT("Failed to allocate memory for HTTP server instance"));
        return NULL;
    }
    hd->hd_calls = calloc(config->max_uri_handlers, sizeof(httpd_uri_t *));
    if (!hd->hd_calls) {
        ESP_LOGE(TAG, LOG_FMT("Failed to allocate memory for HTTP URI handlers"));
        free(hd);
        return NULL;
    }
    hd->hd_sd = calloc(config->max_open_sockets, sizeof(struct sock_db));
    if (!hd->hd_sd) {
        ESP_LOGE(TAG, LOG_FMT("Failed to allocate memory for HTTP session data"));
        free(hd->hd_calls);
        free(hd);
        return NULL;
    }
    struct httpd_req_aux *ra = &hd->hd_req_aux;
    ra->resp_hdrs = calloc(config->max_resp_headers, sizeof(struct resp_hdr));
    if (!ra->resp_hdrs) {
        ESP_LOGE(TAG, LOG_FMT("Failed to allocate memory for HTTP response headers"));
        free(hd->hd_sd);
        free(hd->hd_calls);
        free(hd);
        return NULL;
    }
    hd->err_handler_fns = calloc(HTTPD_ERR_CODE_MAX, sizeof(httpd_err_handler_func_t));
    if (!hd->err_handler_fns) {
        ESP_LOGE(TAG, LOG_FMT("Failed to allocate memory for HTTP error handlers"));
        free(ra->resp_hdrs);
        free(hd->hd_sd);
        free(hd->hd_calls);
        free(hd);
        return NULL;
    }
    /* Save the configuration for this instance */
    hd->config = *config;
    return hd;
}

static void httpd_delete(struct httpd_data *hd)
{
    struct httpd_req_aux *ra = &hd->hd_req_aux;
    /* Free memory of httpd instance data */
    free(hd->err_handler_fns);
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

    /* Sanity check about whether LWIP is configured for providing the
     * maximum number of open sockets sufficient for the server. Though,
     * this check doesn't guarantee that many sockets will actually be
     * available at runtime as other processes may use up some sockets.
     * Note that server also uses 3 sockets for its internal use :
     *     1) listening for new TCP connections
     *     2) for sending control messages over UDP
     *     3) for receiving control messages over UDP
     * So the total number of required sockets is max_open_sockets + 3
     */
    if (HTTPD_MAX_SOCKETS < config->max_open_sockets + 3) {
        ESP_LOGE(TAG, "Config option max_open_sockets is too large (max allowed %d, 3 sockets used by HTTP server internally)\n\t"
                 "Either decrease this or configure LWIP_MAX_SOCKETS to a larger value",
                 HTTPD_MAX_SOCKETS - 3);
        return ESP_ERR_INVALID_ARG;
    }

    struct httpd_data *hd = httpd_create(config);
    if (hd == NULL) {
        /* Failed to allocate memory */
        return ESP_ERR_HTTPD_ALLOC_MEM;
    }
#if CONFIG_HTTPD_QUEUE_WORK_BLOCKING
    /* Using a Counting Semaphore with count equals CONFIG_LWIP_UDP_RECVMBOX_SIZE
     * as the number of UDP messages which can be stored is equal to UDP mailbox size.
     * Using this, we can make sure that the work function is always received by the ctrl socket.
     */
    hd->ctrl_sock_semaphore = xSemaphoreCreateCounting(CONFIG_LWIP_UDP_RECVMBOX_SIZE, CONFIG_LWIP_UDP_RECVMBOX_SIZE);
    if (hd->ctrl_sock_semaphore == NULL) {
        ESP_LOGE(TAG, "Failed to create Semaphore");
        httpd_delete(hd);
        return ESP_ERR_HTTPD_ALLOC_MEM;
    }
#endif

    if (httpd_server_init(hd) != ESP_OK) {
        httpd_delete(hd);
        return ESP_FAIL;
    }

    httpd_sess_init(hd);
    if (httpd_os_thread_create(&hd->hd_td.handle, "httpd",
                               hd->config.stack_size,
                               hd->config.task_priority,
                               httpd_thread, hd,
                               hd->config.core_id,
                               hd->config.task_caps) != ESP_OK) {
        /* Failed to launch task */
        httpd_delete(hd);
        return ESP_ERR_HTTPD_TASK;
    }

    *handle = (httpd_handle_t)hd;
    hd->http_server_state = HTTP_SERVER_EVENT_START;
    esp_http_server_dispatch_event(HTTP_SERVER_EVENT_START, NULL, 0);

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
    int ret = 0;
    if ((ret = cs_send_to_ctrl_sock(hd->msg_fd, hd->config.ctrl_port, &msg, sizeof(msg))) < 0) {
        ESP_LOGE(TAG, "Failed to send shutdown signal err=%d", ret);
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, LOG_FMT("sent control msg to stop server"));
    while (hd->hd_td.status != THREAD_STOPPED) {
        httpd_os_thread_sleep(100);
    }

    /* Release global user context, if not NULL */
    if (hd->config.global_user_ctx) {
        if (hd->config.global_user_ctx_free_fn) {
            hd->config.global_user_ctx_free_fn(hd->config.global_user_ctx);
        } else {
            free(hd->config.global_user_ctx);
        }
        hd->config.global_user_ctx = NULL;
    }

    /* Release global transport context, if not NULL */
    if (hd->config.global_transport_ctx) {
        if (hd->config.global_transport_ctx_free_fn) {
            hd->config.global_transport_ctx_free_fn(hd->config.global_transport_ctx);
        } else {
            free(hd->config.global_transport_ctx);
        }
        hd->config.global_transport_ctx = NULL;
    }

    ESP_LOGD(TAG, LOG_FMT("server stopped"));
#if CONFIG_HTTPD_QUEUE_WORK_BLOCKING
    vSemaphoreDelete(hd->ctrl_sock_semaphore);
#endif
    httpd_delete(hd);
    esp_http_server_dispatch_event(HTTP_SERVER_EVENT_STOP, NULL, 0);
    return ESP_OK;
}

esp_http_server_event_id_t httpd_get_server_state(httpd_handle_t handle)
{
    struct httpd_data *hd = (struct httpd_data *) handle;
    if (hd == NULL) {
        return HTTP_SERVER_EVENT_ERROR;
    }
    return hd->http_server_state;
}
