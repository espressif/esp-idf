/* Iperf Example - iperf implementation

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "iperf.h"

typedef struct {
    iperf_cfg_t cfg;
    bool finish;
    uint32_t total_len;
    uint32_t buffer_len;
    uint8_t *buffer;
    uint32_t sockfd;
} iperf_ctrl_t;

typedef struct {
    int32_t id;
    uint32_t sec;
    uint32_t usec;
} iperf_udp_pkt_t;

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
    uint32_t last_len = 0;
    uint32_t cur = 0;

    printf("\n%16s %s\n", "Interval", "Bandwidth");
    while (!s_iperf_ctrl.finish) {
        vTaskDelay(delay_interval);
        printf("%4d-%4d sec       %.2f Mbits/sec\n", cur, cur + interval,
               (double)((s_iperf_ctrl.total_len - last_len) * 8) / interval / 1e6);
        cur += interval;
        last_len = s_iperf_ctrl.total_len;
        if (cur >= time) {
            break;
        }
    }

    if (cur != 0) {
        printf("%4d-%4d sec       %.2f Mbits/sec\n", 0, time,
               (double)(s_iperf_ctrl.total_len * 8) / cur / 1e6);
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

static esp_err_t IRAM_ATTR iperf_run_tcp_server(void)
{
    socklen_t addr_len = sizeof(struct sockaddr);
    struct sockaddr_in remote_addr;
    struct sockaddr_in addr;
    int actual_recv = 0;
    int want_recv = 0;
    uint8_t *buffer;
    int listen_socket;
    struct timeval t;
    int sockfd;
    int opt = 1;

    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket < 0) {
        iperf_show_socket_error_reason("tcp server create", listen_socket);
        return ESP_FAIL;
    }

    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(s_iperf_ctrl.cfg.sport);
    addr.sin_addr.s_addr = s_iperf_ctrl.cfg.sip;
    if (bind(listen_socket, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        iperf_show_socket_error_reason("tcp server bind", listen_socket);
        close(listen_socket);
        return ESP_FAIL;
    }

    if (listen(listen_socket, 5) < 0) {
        iperf_show_socket_error_reason("tcp server listen", listen_socket);
        close(listen_socket);
        return ESP_FAIL;
    }

    printf("iperf tcp server create successfully\n");
    buffer = s_iperf_ctrl.buffer;
    want_recv = s_iperf_ctrl.buffer_len;
    while (!s_iperf_ctrl.finish) {

        /*TODO need to change to non-block mode */
        sockfd = accept(listen_socket, (struct sockaddr *)&remote_addr, &addr_len);
        if (sockfd < 0) {
            iperf_show_socket_error_reason("tcp server listen", listen_socket);
            close(listen_socket);
            return ESP_FAIL;
        } else {
            printf("accept: %s,%d\n", inet_ntoa(remote_addr.sin_addr), htons(remote_addr.sin_port));
            iperf_start_report();

            t.tv_sec = IPERF_SOCKET_RX_TIMEOUT;
            setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t));
        }

        while (!s_iperf_ctrl.finish) {
            actual_recv = recv(sockfd, buffer, want_recv, 0);
            if (actual_recv < 0) {
                iperf_show_socket_error_reason("tcp server recv", listen_socket);
                s_iperf_ctrl.finish = true;
                break;
            } else {
                s_iperf_ctrl.total_len += actual_recv;
            }
        }

        close(sockfd);
    }

    s_iperf_ctrl.finish = true;
    close(listen_socket);
    return ESP_OK;
}

static esp_err_t IRAM_ATTR iperf_run_udp_server(void)
{
    socklen_t addr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in addr;
    int actual_recv = 0;
    struct timeval t;
    int want_recv = 0;
    uint8_t *buffer;
    int sockfd;
    int opt = 1;
    bool udp_recv_start = true ;

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        iperf_show_socket_error_reason("udp server create", sockfd);
        return ESP_FAIL;
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(s_iperf_ctrl.cfg.sport);
    addr.sin_addr.s_addr = s_iperf_ctrl.cfg.sip;
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        iperf_show_socket_error_reason("udp server bind", sockfd);
        return ESP_FAIL;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(s_iperf_ctrl.cfg.sport);
    addr.sin_addr.s_addr = s_iperf_ctrl.cfg.sip;

    buffer = s_iperf_ctrl.buffer;
    want_recv = s_iperf_ctrl.buffer_len;
    ESP_LOGI(TAG, "want recv=%d", want_recv);

    t.tv_sec = IPERF_SOCKET_RX_TIMEOUT;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t));

    while (!s_iperf_ctrl.finish) {
        actual_recv = recvfrom(sockfd, buffer, want_recv, 0, (struct sockaddr *)&addr, &addr_len);
        if (actual_recv < 0) {
            iperf_show_socket_error_reason("udp server recv", sockfd);
        } else {
            if(udp_recv_start){
                iperf_start_report();
                udp_recv_start = false;
            }
            s_iperf_ctrl.total_len += actual_recv;
        }
    }

    s_iperf_ctrl.finish = true;
    close(sockfd);
    return ESP_OK;
}

static esp_err_t iperf_run_udp_client(void)
{
    struct sockaddr_in addr;
    iperf_udp_pkt_t *udp;
    int actual_send = 0;
    bool retry = false;
    uint32_t delay = 1;
    int want_send = 0;
    uint8_t *buffer;
    int sockfd;
    int opt = 1;
    int err;
    int id;

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        iperf_show_socket_error_reason("udp client create", sockfd);
        return ESP_FAIL;
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(s_iperf_ctrl.cfg.dport);
    addr.sin_addr.s_addr = s_iperf_ctrl.cfg.dip;

    iperf_start_report();
    buffer = s_iperf_ctrl.buffer;
    udp = (iperf_udp_pkt_t *)buffer;
    want_send = s_iperf_ctrl.buffer_len;
    id = 0;

    while (!s_iperf_ctrl.finish) {
        if (false == retry) {
            id++;
            udp->id = htonl(id);
            delay = 1;
        }

        retry = false;
        actual_send = sendto(sockfd, buffer, want_send, 0, (struct sockaddr *)&addr, sizeof(addr));

        if (actual_send != want_send) {
            err = iperf_get_socket_error_code(sockfd);
            if (err == ENOMEM) {
                vTaskDelay(delay);
                if (delay < IPERF_MAX_DELAY) {
                    delay <<= 1;
                }
                retry = true;
                continue;
            } else {
                ESP_LOGE(TAG, "udp client send abort: err=%d", err);
                break;
            }
        } else {
            s_iperf_ctrl.total_len += actual_send;
        }
    }

    s_iperf_ctrl.finish = true;
    close(sockfd);
    return ESP_OK;
}

static esp_err_t iperf_run_tcp_client(void)
{
    struct sockaddr_in remote_addr;
    int actual_send = 0;
    int want_send = 0;
    uint8_t *buffer;
    int sockfd;

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        iperf_show_socket_error_reason("tcp client create", sockfd);
        return ESP_FAIL;
    }

    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(s_iperf_ctrl.cfg.dport);
    remote_addr.sin_addr.s_addr = s_iperf_ctrl.cfg.dip;
    if (connect(sockfd, (struct sockaddr *)&remote_addr, sizeof(remote_addr)) < 0) {
        iperf_show_socket_error_reason("tcp client connect", sockfd);
        return ESP_FAIL;
    }

    iperf_start_report();
    buffer = s_iperf_ctrl.buffer;
    want_send = s_iperf_ctrl.buffer_len;
    while (!s_iperf_ctrl.finish) {
        actual_send = send(sockfd, buffer, want_send, 0);
        if (actual_send <= 0) {
            iperf_show_socket_error_reason("tcp client send", sockfd);
            break;
        } else {
            s_iperf_ctrl.total_len += actual_send;
        }
    }

    s_iperf_ctrl.finish = true;
    close(sockfd);
    return ESP_OK;
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
        return IPERF_UDP_TX_LEN;
    } else if (iperf_is_udp_server()) {
        return IPERF_UDP_RX_LEN;
    } else if (iperf_is_tcp_client()) {
        return IPERF_TCP_TX_LEN;
    } else {
        return IPERF_TCP_RX_LEN;
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
