/* Iperf Example - iperf implementation

   This example code is in the Public Domain (or CC0 licensed, at your option.)
/
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "iperf.h"
#include "wifi_stats.h"


typedef struct {
    iperf_cfg_t cfg;
    bool finish;
    uint32_t actual_len;
    uint32_t buffer_len;
    uint8_t *buffer;
    uint32_t sockfd;
} iperf_ctrl_t;

static bool s_iperf_is_running = false;
static iperf_ctrl_t s_iperf_ctrl;
static const char *TAG = "iperf";

inline static bool iperf_is_udp_client(void)
{
    return ((s_iperf_ctrl.cfg.flag & IPERF_FLAG_CLIENT) && (s_iperf_ctrl.cfg.flag & IPERF_FLAG_UDP));
}

inline static bool iperf_is_udp_server(void)
{
    return ((s_iperf_ctrl.cfg.flag & IPERF_FLAG_SERVER) && (s_iperf_ctrl.cfg.flag & IPERF_FLAG_UDP));
}

inline static bool iperf_is_tcp_client(void)
{
    return ((s_iperf_ctrl.cfg.flag & IPERF_FLAG_CLIENT) && (s_iperf_ctrl.cfg.flag & IPERF_FLAG_TCP));
}

inline static bool iperf_is_tcp_server(void)
{
    return ((s_iperf_ctrl.cfg.flag & IPERF_FLAG_SERVER) && (s_iperf_ctrl.cfg.flag & IPERF_FLAG_TCP));
}

static int iperf_get_socket_error_code(int sockfd)
{
    return errno;
}

static int iperf_show_socket_error_reason(const char *str, int sockfd)
{
    int err = errno;
    if (err != 0) {
        ESP_LOGW(TAG, "%s error, error code: %d, reason: %s", str, err, strerror(err));
    }

    return err;
}

static void iperf_report_task(void *arg)
{
    uint32_t interval = s_iperf_ctrl.cfg.interval;
    uint32_t time = s_iperf_ctrl.cfg.time;
    TickType_t delay_interval = (interval * 1000) / portTICK_PERIOD_MS;
    uint32_t cur = 0;
    double average = 0;
    double actual_bandwidth = 0;
    int k = 1;
    const double coefficient[3] = {1048576.0, 1024.0, 1.0};
    const char unit[3] = {'M', 'K', '\0'};
    iperf_output_format format = s_iperf_ctrl.cfg.format;

    printf("\n%16s %s\n", "Interval", "Bandwidth");
    while (!s_iperf_ctrl.finish) {
        vTaskDelay(delay_interval);
        actual_bandwidth = (s_iperf_ctrl.actual_len / coefficient[format] * 8) / interval;
        printf("%4" PRIi32 "-%4" PRIi32 " sec       %.2f %cbits/sec\n", cur, cur + interval,
            actual_bandwidth, unit[format]);
        cur += interval;
        average = ((average * (k - 1) / k) + (actual_bandwidth / k));
        k++;
        s_iperf_ctrl.actual_len = 0;
        if (cur >= time) {
            printf("%4d-%4" PRIu32 " sec       %.2f %cbits/sec\n", 0, time,
                average, unit[format]);
            break;
        }
    }

    s_iperf_ctrl.finish = true;
    vTaskDelete(NULL);
}

static esp_err_t iperf_start_report(void)
{
    int ret;

    ret = xTaskCreatePinnedToCore(iperf_report_task, IPERF_REPORT_TASK_NAME, IPERF_REPORT_TASK_STACK, NULL, IPERF_REPORT_TASK_PRIORITY, NULL, portNUM_PROCESSORS - 1);

    if (ret != pdPASS) {
        ESP_LOGE(TAG, "create task %s failed", IPERF_REPORT_TASK_NAME);
        return ESP_FAIL;
    }

    return ESP_OK;
}

static void IRAM_ATTR socket_recv(int recv_socket, struct sockaddr_storage listen_addr, uint8_t type)
{
    bool iperf_recv_start = true;
    uint8_t *buffer;
    int want_recv = 0;
    int actual_recv = 0;
#ifdef CONFIG_LWIP_IPV6
    socklen_t socklen = (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6) ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);
#else
    socklen_t socklen = sizeof(struct sockaddr_in);
#endif
    const char *error_log = (type == IPERF_TRANS_TYPE_TCP) ? "tcp server recv" : "udp server recv";

    buffer = s_iperf_ctrl.buffer;
    want_recv = s_iperf_ctrl.buffer_len;
    while (!s_iperf_ctrl.finish) {
        actual_recv = recvfrom(recv_socket, buffer, want_recv, 0, (struct sockaddr *)&listen_addr, &socklen);
        if (actual_recv < 0) {
            iperf_show_socket_error_reason(error_log, recv_socket);
            s_iperf_ctrl.finish = true;
            break;
        } else {
            if (iperf_recv_start) {
                iperf_start_report();
                iperf_recv_start = false;
            }
            s_iperf_ctrl.actual_len += actual_recv;
        }
    }
}

static void IRAM_ATTR socket_send(int send_socket, struct sockaddr_storage dest_addr, uint8_t type, int bw_lim)
{
    uint8_t *buffer;
    uint32_t *pkt_id_p;
    uint32_t pkt_cnt = 0;
    int actual_send = 0;
    int want_send = 0;
    int period_us = -1;
    int delay_us = 0;
    int64_t prev_time = 0;
    int64_t send_time = 0;
    int err = 0;
#ifdef CONFIG_LWIP_IPV6
    const socklen_t socklen = (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6) ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);
#else
    const socklen_t socklen = sizeof(struct sockaddr_in);
#endif
    const char *error_log = (type == IPERF_TRANS_TYPE_TCP) ? "tcp client send" : "udp client send";

    buffer = s_iperf_ctrl.buffer;
    pkt_id_p = (uint32_t *)s_iperf_ctrl.buffer;
    want_send = s_iperf_ctrl.buffer_len;
    iperf_start_report();

    if (bw_lim > 0) {
        period_us = want_send * 8 / bw_lim;
    }

    while (!s_iperf_ctrl.finish) {
        if (period_us > 0) {
            send_time = esp_timer_get_time();
            if (actual_send > 0){
                // Last packet "send" was successful, check how much off the previous loop duration was to the ideal send period. Result will adjust the
                // next send delay.
                delay_us += period_us + (int32_t)(prev_time - send_time);
            } else {
                // Last packet "send" was not successful. Ideally we should try to catch up the whole previous loop duration (e.g. prev_time - send_time).
                // However, that's not possible since the most probable reason why the send was unsuccessful is the HW was not able to process the packet.
                // Hence, we cannot queue more packets with shorter (or no) delay to catch up since we are already at the performance edge. The best we
                // can do is to reset the send delay (which is probably big negative number) and start all over again.
                delay_us = 0;
            }
            prev_time = send_time;
        }
        *pkt_id_p = htonl(pkt_cnt++); // datagrams need to be sequentially numbered
        actual_send = sendto(send_socket, buffer, want_send, 0, (struct sockaddr *)&dest_addr, socklen);
        if (actual_send != want_send) {
            if (type == IPERF_TRANS_TYPE_UDP) {
                err = iperf_get_socket_error_code(send_socket);
                // ENOMEM is expected under heavy load => do not print it
                if (err != ENOMEM) {
                    iperf_show_socket_error_reason(error_log, send_socket);
                }
            } else if (type == IPERF_TRANS_TYPE_TCP) {
                iperf_show_socket_error_reason(error_log, send_socket);
                break;
            }
        } else {
            s_iperf_ctrl.actual_len += actual_send;
        }
        // The send delay may be negative, it indicates we are trying to catch up and hence to not delay the loop at all.
        if (delay_us > 0) {
            esp_rom_delay_us(delay_us);
        }
    }
}

static esp_err_t iperf_run_tcp_server(void)
{
    int listen_socket = -1;
    int client_socket = -1;
    int opt = 1;
    int err = 0;
    esp_err_t ret = ESP_OK;
    struct sockaddr_in remote_addr;
    struct timeval timeout = { 0 };
    socklen_t addr_len = sizeof(struct sockaddr);
    struct sockaddr_storage listen_addr = { 0 };

    struct sockaddr_in listen_addr4 = { 0 };
#ifdef CONFIG_LWIP_IPV6
    struct sockaddr_in6 listen_addr6 = { 0 };
    ESP_GOTO_ON_FALSE((s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6 || s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4), ESP_FAIL, exit, TAG, "Ivalid AF types");
#else
    ESP_GOTO_ON_FALSE((s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4), ESP_FAIL, exit, TAG, "Invalid AF types");
#endif
#ifdef CONFIG_LWIP_IPV6
    if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6) {
        // The TCP server listen at the address "::", which means all addresses can be listened to.
        inet6_aton("::", &listen_addr6.sin6_addr);
        listen_addr6.sin6_family = AF_INET6;
        listen_addr6.sin6_port = htons(s_iperf_ctrl.cfg.sport);

        listen_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_IPV6);
        ESP_GOTO_ON_FALSE((listen_socket >= 0), ESP_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);

        setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        setsockopt(listen_socket, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));

        ESP_LOGI(TAG, "Socket created");

        err = bind(listen_socket, (struct sockaddr *)&listen_addr6, sizeof(listen_addr6));
        ESP_GOTO_ON_FALSE((err == 0), ESP_FAIL, exit, TAG, "Socket unable to bind: errno %d, IPPROTO: %d", errno, AF_INET6);
        err = listen(listen_socket, 1);
        ESP_GOTO_ON_FALSE((err == 0), ESP_FAIL, exit, TAG, "Error occurred during listen: errno %d", errno);

        memcpy(&listen_addr, &listen_addr6, sizeof(listen_addr6));
    } else
#endif
    if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4) {
        listen_addr4.sin_family = AF_INET;
        listen_addr4.sin_port = htons(s_iperf_ctrl.cfg.sport);
        listen_addr4.sin_addr.s_addr = s_iperf_ctrl.cfg.source_ip4;

        listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        ESP_GOTO_ON_FALSE((listen_socket >= 0), ESP_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);

        setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        ESP_LOGI(TAG, "Socket created");

        err = bind(listen_socket, (struct sockaddr *)&listen_addr4, sizeof(listen_addr4));
        ESP_GOTO_ON_FALSE((err == 0), ESP_FAIL, exit, TAG, "Socket unable to bind: errno %d, IPPROTO: %d", errno, AF_INET);

        err = listen(listen_socket, 5);
        ESP_GOTO_ON_FALSE((err == 0), ESP_FAIL, exit, TAG, "Error occurred during listen: errno %d", errno);
        memcpy(&listen_addr, &listen_addr4, sizeof(listen_addr4));
    }

    timeout.tv_sec = IPERF_SOCKET_RX_TIMEOUT;
    setsockopt(listen_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    client_socket = accept(listen_socket, (struct sockaddr *)&remote_addr, &addr_len);
    ESP_GOTO_ON_FALSE((client_socket >= 0), ESP_FAIL, exit, TAG, "Unable to accept connection: errno %d", errno);
    ESP_LOGI(TAG, "accept: %s,%d\n", inet_ntoa(remote_addr.sin_addr), htons(remote_addr.sin_port));

    timeout.tv_sec = IPERF_SOCKET_RX_TIMEOUT;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

#if CONFIG_ESP_WIFI_ENABLE_WIFI_TX_STATS
    wifi_cmd_clr_tx_statistics(0, NULL);
#endif
#if CONFIG_ESP_WIFI_ENABLE_WIFI_RX_STATS
    wifi_cmd_clr_rx_statistics(0, NULL);
#endif
    socket_recv(client_socket, listen_addr, IPERF_TRANS_TYPE_TCP);
#if CONFIG_ESP_WIFI_ENABLE_WIFI_RX_STATS
    wifi_cmd_get_rx_statistics(0, NULL);
#endif
#if CONFIG_ESP_WIFI_ENABLE_WIFI_TX_STATS
    wifi_cmd_get_tx_statistics(0, NULL);
#endif

exit:
    if (client_socket != -1) {
        close(client_socket);
    }

    if (listen_socket != -1) {
        shutdown(listen_socket, 0);
        close(listen_socket);
        ESP_LOGI(TAG, "TCP Socket server is closed.");
    }
    s_iperf_ctrl.finish = true;
    return ret;
}

static esp_err_t iperf_run_tcp_client(void)
{
    int client_socket = -1;
    int err = 0;
    esp_err_t ret = ESP_OK;
    struct sockaddr_storage dest_addr = { 0 };
    struct sockaddr_in dest_addr4 = { 0 };
    struct timeval timeout = { 0 };
#ifdef CONFIG_LWIP_IPV6
    struct sockaddr_in6 dest_addr6 = { 0 };
    ESP_GOTO_ON_FALSE((s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6 || s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4), ESP_FAIL, exit, TAG, "Ivalid AF types");
#else
    ESP_GOTO_ON_FALSE((s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4), ESP_FAIL, exit, TAG, "Invalid AF types");
#endif
#ifdef CONFIG_LWIP_IPV6
    if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6) {
        client_socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_IPV6);
        ESP_GOTO_ON_FALSE((client_socket >= 0), ESP_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);

        inet6_aton(s_iperf_ctrl.cfg.destination_ip6, &dest_addr6.sin6_addr);
        dest_addr6.sin6_family = AF_INET6;
        dest_addr6.sin6_port = htons(s_iperf_ctrl.cfg.dport);

        err = connect(client_socket, (struct sockaddr *)&dest_addr6, sizeof(struct sockaddr_in6));
        ESP_GOTO_ON_FALSE((err == 0), ESP_FAIL, exit, TAG, "Socket unable to connect: errno %d", errno);
        ESP_LOGI(TAG, "Successfully connected");
        memcpy(&dest_addr, &dest_addr6, sizeof(dest_addr6));
    } else
#endif
    if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4) {
        client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        ESP_GOTO_ON_FALSE((client_socket >= 0), ESP_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);

        dest_addr4.sin_family = AF_INET;
        dest_addr4.sin_port = htons(s_iperf_ctrl.cfg.dport);
        dest_addr4.sin_addr.s_addr = s_iperf_ctrl.cfg.destination_ip4;
        err = connect(client_socket, (struct sockaddr *)&dest_addr4, sizeof(struct sockaddr_in));
        ESP_GOTO_ON_FALSE((err == 0), ESP_FAIL, exit, TAG, "Socket unable to connect: errno %d", errno);
        ESP_LOGI(TAG, "Successfully connected");
        memcpy(&dest_addr, &dest_addr4, sizeof(dest_addr4));
    }
    timeout.tv_sec = IPERF_SOCKET_TCP_TX_TIMEOUT;
    setsockopt(client_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

#if CONFIG_ESP_WIFI_ENABLE_WIFI_RX_STATS
    wifi_cmd_clr_rx_statistics(0, NULL);
#endif
#if CONFIG_ESP_WIFI_ENABLE_WIFI_TX_STATS
    wifi_cmd_clr_tx_statistics(0, NULL);
#endif
    socket_send(client_socket, dest_addr, IPERF_TRANS_TYPE_TCP, s_iperf_ctrl.cfg.bw_lim);
#if CONFIG_ESP_WIFI_ENABLE_WIFI_RX_STATS
    wifi_cmd_get_rx_statistics(0, NULL);
#endif
#if CONFIG_ESP_WIFI_ENABLE_WIFI_TX_STATS
    wifi_cmd_get_tx_statistics(0, NULL);
#endif

exit:
    if (client_socket != -1) {
        shutdown(client_socket, 0);
        close(client_socket);
        ESP_LOGI(TAG, "TCP Socket client is closed.");
    }
    s_iperf_ctrl.finish = true;
    return ret;
}

static esp_err_t iperf_run_udp_server(void)
{
    int listen_socket = -1;
    int opt = 1;
    int err = 0;
    esp_err_t ret = ESP_OK;
    struct timeval timeout = { 0 };
    struct sockaddr_storage listen_addr = { 0 };
    struct sockaddr_in listen_addr4 = { 0 };
#ifdef CONFIG_LWIP_IPV6
    struct sockaddr_in6 listen_addr6 = { 0 };
    ESP_GOTO_ON_FALSE((s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6 || s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4), ESP_FAIL, exit, TAG, "Ivalid AF types");
#else
    ESP_GOTO_ON_FALSE((s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4), ESP_FAIL, exit, TAG, "Ivalid AF types");
#endif
#ifdef CONFIG_LWIP_IPV6
    if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6) {
        // The UDP server listen at the address "::", which means all addresses can be listened to.
        inet6_aton("::", &listen_addr6.sin6_addr);
        listen_addr6.sin6_family = AF_INET6;
        listen_addr6.sin6_port = htons(s_iperf_ctrl.cfg.sport);

        listen_socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
        ESP_GOTO_ON_FALSE((listen_socket >= 0), ESP_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);
        ESP_LOGI(TAG, "Socket created");

        setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        err = bind(listen_socket, (struct sockaddr *)&listen_addr6, sizeof(struct sockaddr_in6));
        ESP_GOTO_ON_FALSE((err == 0), ESP_FAIL, exit, TAG, "Socket unable to bind: errno %d", errno);
        ESP_LOGI(TAG, "Socket bound, port %" PRIu16, listen_addr6.sin6_port);

        memcpy(&listen_addr, &listen_addr6, sizeof(listen_addr6));
    } else
#endif
    if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4) {
        listen_addr4.sin_family = AF_INET;
        listen_addr4.sin_port = htons(s_iperf_ctrl.cfg.sport);
        listen_addr4.sin_addr.s_addr = s_iperf_ctrl.cfg.source_ip4;

        listen_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        ESP_GOTO_ON_FALSE((listen_socket >= 0), ESP_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);
        ESP_LOGI(TAG, "Socket created");

        setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        err = bind(listen_socket, (struct sockaddr *)&listen_addr4, sizeof(struct sockaddr_in));
        ESP_GOTO_ON_FALSE((err == 0), ESP_FAIL, exit, TAG, "Socket unable to bind: errno %d", errno);
        ESP_LOGI(TAG, "Socket bound, port %d", listen_addr4.sin_port);
        memcpy(&listen_addr, &listen_addr4, sizeof(listen_addr4));
    }

    timeout.tv_sec = IPERF_SOCKET_RX_TIMEOUT;
    setsockopt(listen_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#if CONFIG_ESP_WIFI_ENABLE_WIFI_RX_STATS
    wifi_cmd_clr_rx_statistics(0, NULL);
#endif
    socket_recv(listen_socket, listen_addr, IPERF_TRANS_TYPE_UDP);
#if CONFIG_ESP_WIFI_ENABLE_WIFI_RX_STATS
    wifi_cmd_get_rx_statistics(0, NULL);
#endif
exit:
    if (listen_socket != -1) {
        shutdown(listen_socket, 0);
        close(listen_socket);
    }
    ESP_LOGI(TAG, "Udp socket server is closed.");
    s_iperf_ctrl.finish = true;
    return ret;
}

static esp_err_t iperf_run_udp_client(void)
{
    int client_socket = -1;
    int opt = 1;
    esp_err_t ret = ESP_OK;
    struct sockaddr_storage dest_addr = { 0 };
    struct sockaddr_in dest_addr4 = { 0 };
#ifdef CONFIG_LWIP_IPV6
    struct sockaddr_in6 dest_addr6 = { 0 };
    ESP_GOTO_ON_FALSE((s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6 || s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4), ESP_FAIL, exit, TAG, "Ivalid AF types");
#else
    ESP_GOTO_ON_FALSE((s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4), ESP_FAIL, exit, TAG, "Ivalid AF types");
#endif
#ifdef CONFIG_LWIP_IPV6
    if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6) {
        inet6_aton(s_iperf_ctrl.cfg.destination_ip6, &dest_addr6.sin6_addr);
        dest_addr6.sin6_family = AF_INET6;
        dest_addr6.sin6_port = htons(s_iperf_ctrl.cfg.dport);

        client_socket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_IPV6);
        ESP_GOTO_ON_FALSE((client_socket >= 0), ESP_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);
        ESP_LOGI(TAG, "Socket created, sending to %s:%" PRIu16, s_iperf_ctrl.cfg.destination_ip6, s_iperf_ctrl.cfg.dport);

        setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        memcpy(&dest_addr, &dest_addr6, sizeof(dest_addr6));
    } else
#endif
    if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV4) {
        dest_addr4.sin_family = AF_INET;
        dest_addr4.sin_port = htons(s_iperf_ctrl.cfg.dport);
        dest_addr4.sin_addr.s_addr = s_iperf_ctrl.cfg.destination_ip4;

        client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        ESP_GOTO_ON_FALSE((client_socket >= 0), ESP_FAIL, exit, TAG, "Unable to create socket: errno %d", errno);
        ESP_LOGI(TAG, "Socket created, sending to %d.%d.%d.%d:%" PRIu16,
            (uint16_t) s_iperf_ctrl.cfg.destination_ip4 & 0xFF,
            (uint16_t) (s_iperf_ctrl.cfg.destination_ip4 >> 8) & 0xFF,
            (uint16_t) (s_iperf_ctrl.cfg.destination_ip4 >> 16) & 0xFF,
            (uint16_t) (s_iperf_ctrl.cfg.destination_ip4 >> 24) & 0xFF,
            s_iperf_ctrl.cfg.dport);

        setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        memcpy(&dest_addr, &dest_addr4, sizeof(dest_addr4));
    }
#if CONFIG_ESP_WIFI_ENABLE_WIFI_TX_STATS
    wifi_cmd_clr_tx_statistics(0, NULL);
#endif
    socket_send(client_socket, dest_addr, IPERF_TRANS_TYPE_UDP, s_iperf_ctrl.cfg.bw_lim);
#if CONFIG_ESP_WIFI_ENABLE_WIFI_TX_STATS
    wifi_cmd_get_tx_statistics(0, NULL);
#endif
exit:
    if (client_socket != -1) {
        shutdown(client_socket, 0);
        close(client_socket);
    }
    s_iperf_ctrl.finish = true;
    ESP_LOGI(TAG, "UDP Socket client is closed");
    return ret;
}

static void iperf_task_traffic(void *arg)
{
    if (iperf_is_udp_client()) {
        iperf_run_udp_client();
    } else if (iperf_is_udp_server()) {
        iperf_run_udp_server();
    } else if (iperf_is_tcp_client()) {
        iperf_run_tcp_client();
    } else {
        iperf_run_tcp_server();
    }

    if (s_iperf_ctrl.buffer) {
        free(s_iperf_ctrl.buffer);
        s_iperf_ctrl.buffer = NULL;
    }
    ESP_LOGI(TAG, "iperf exit");
    s_iperf_is_running = false;
    vTaskDelete(NULL);
}

static uint32_t iperf_get_buffer_len(void)
{
    if (iperf_is_udp_client()) {
#ifdef CONFIG_LWIP_IPV6
        if (s_iperf_ctrl.cfg.len_send_buf) {
            return s_iperf_ctrl.cfg.len_send_buf;
        } else if (s_iperf_ctrl.cfg.type == IPERF_IP_TYPE_IPV6) {
            return IPERF_DEFAULT_IPV6_UDP_TX_LEN;
        } else {
            return IPERF_DEFAULT_IPV4_UDP_TX_LEN;
        }
#else
        return (s_iperf_ctrl.cfg.len_send_buf == 0 ? IPERF_DEFAULT_IPV4_UDP_TX_LEN : s_iperf_ctrl.cfg.len_send_buf);
#endif
    } else if (iperf_is_udp_server()) {
        return IPERF_DEFAULT_UDP_RX_LEN;
    } else if (iperf_is_tcp_client()) {
        return (s_iperf_ctrl.cfg.len_send_buf == 0 ? IPERF_DEFAULT_TCP_TX_LEN : s_iperf_ctrl.cfg.len_send_buf);
    } else {
        return IPERF_DEFAULT_TCP_RX_LEN;
    }
    return 0;
}

esp_err_t iperf_start(iperf_cfg_t *cfg)
{
    BaseType_t ret;

    if (!cfg) {
        return ESP_FAIL;
    }

    if (s_iperf_is_running) {
        ESP_LOGW(TAG, "iperf is running");
        return ESP_FAIL;
    }

    memset(&s_iperf_ctrl, 0, sizeof(s_iperf_ctrl));
    memcpy(&s_iperf_ctrl.cfg, cfg, sizeof(*cfg));
    s_iperf_is_running = true;
    s_iperf_ctrl.finish = false;
    s_iperf_ctrl.buffer_len = iperf_get_buffer_len();
    s_iperf_ctrl.buffer = (uint8_t *)malloc(s_iperf_ctrl.buffer_len);
    if (!s_iperf_ctrl.buffer) {
        ESP_LOGE(TAG, "create buffer: not enough memory");
        return ESP_FAIL;
    }
    memset(s_iperf_ctrl.buffer, 0, s_iperf_ctrl.buffer_len);
    ret = xTaskCreatePinnedToCore(iperf_task_traffic, IPERF_TRAFFIC_TASK_NAME, IPERF_TRAFFIC_TASK_STACK, NULL, IPERF_TRAFFIC_TASK_PRIORITY, NULL, portNUM_PROCESSORS - 1);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "create task %s failed", IPERF_TRAFFIC_TASK_NAME);
        free(s_iperf_ctrl.buffer);
        s_iperf_ctrl.buffer = NULL;
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t iperf_stop(void)
{
    if (s_iperf_is_running) {
        s_iperf_ctrl.finish = true;
    }

    while (s_iperf_is_running) {
        ESP_LOGI(TAG, "wait current iperf to stop ...");
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
    return ESP_OK;
}
