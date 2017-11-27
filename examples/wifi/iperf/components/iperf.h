/* Iperf Example - iperf declaration

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef __IPERF_H_
#define __IPERF_H_

#ifdef __cplusplus
extern "C" {
#endif

#define IPERF_FLAG_CLIENT (1)
#define IPERF_FLAG_SERVER (1<<1)
#define IPERF_FLAG_TCP    (1<<2)
#define IPERF_FLAG_UDP    (1<<3)

#define IPERF_DEFAULT_PORT     5001
#define IPERF_DEFAULT_INTERVAL 3
#define IPERF_DEFAULT_TIME     30

#define IPERF_TRAFFIC_TASK_NAME       "iperf_traffic"
#define IPERF_TRAFFIC_TASK_PRIORITY   19
#define IPERF_TRAFFIC_TASK_STACK      4096
#define IPERF_REPORT_TASK_NAME        "iperf_report"
#define IPERF_REPORT_TASK_PRIORITY    10
#define IPERF_REPORT_TASK_STACK       4096
#define IPERF_REPORT_TASK_NAME        "iperf_report"

#define IPERF_UDP_TX_LEN     (1472)
#define IPERF_UDP_RX_LEN     (32<<10)
#define IPERF_TCP_TX_LEN     (32<<10)
#define IPERF_TCP_RX_LEN     (32<<10)

#define IPERF_MAX_DELAY      64

#define IPERF_SOCKET_RX_TIMEOUT      10
#define IPERF_SOCKET_ACCEPT_TIMEOUT  5

typedef struct {
    uint32_t flag;
    uint32_t dip;
    uint32_t sip;
    uint16_t dport;
    uint16_t sport;
    uint32_t interval;
    uint32_t time;
} iperf_cfg_t;

esp_err_t iperf_start(iperf_cfg_t *cfg);
esp_err_t iperf_stop(void);

#ifdef __cplusplus
}
#endif

#endif
