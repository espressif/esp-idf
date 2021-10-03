/* OpenThread Command Line Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_check.h"
#include "esp_log.h"
#include "esp_ot_iperf.h"
#include "iperf.h"
#include "openthread/cli.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TAG "ot-iperf"

void esp_ot_process_iperf(void *aContext, uint8_t aArgsLength, char *aArgs[])
{
    (void)(aContext);
    (void)(aArgsLength);
    iperf_cfg_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    // set iperf default flag: tcp server
    IPERF_FLAG_SET(cfg.flag, IPERF_FLAG_TCP);
    IPERF_FLAG_SET(cfg.flag, IPERF_FLAG_SERVER);
    cfg.time = IPERF_DEFAULT_TIME;
    cfg.type = IPERF_IP_TYPE_IPV4;
    if (aArgsLength == 0) {
        otCliOutputFormat("---iperf parameter---\n");
        otCliOutputFormat("-V                  :     use IPV6 address\n");
        otCliOutputFormat("-s                  :     server mode, only receive\n");
        otCliOutputFormat("-u                  :     upd mode\n");
        otCliOutputFormat("-c <addr>           :     client mode, only transmit\n");
        otCliOutputFormat("-i <interval>       :     seconds between periodic bandwidth reports\n");
        otCliOutputFormat("-t <time>           :     time in seconds to transmit for (default 30 secs)\n");
        otCliOutputFormat("-p <port>           :     server port to listen on/connect to\n");
        otCliOutputFormat("-l <len_send_buf>   :     the lenth of send buffer\n");
        otCliOutputFormat("---example---\n");
        otCliOutputFormat("create a tcp server :     iperf -s -i 3 -p 5001 -t 60\n");
        otCliOutputFormat("create a udp client :     iperf -c <addr> -u -i 3 -t 60 -p 5001 -l 512\n");
    } else {
        for (int i = 0; i < aArgsLength; i++) {
            if (strcmp(aArgs[i], "-c") == 0) {
                IPERF_FLAG_SET(cfg.flag, IPERF_FLAG_CLIENT);
                IPERF_FLAG_CLR(cfg.flag, IPERF_FLAG_SERVER);
                i++;
                cfg.destination_ip6 = aArgs[i];
                otCliOutputFormat("ip:%s\n", cfg.destination_ip6);
            } else if (strcmp(aArgs[i], "-s") == 0) {
                IPERF_FLAG_SET(cfg.flag, IPERF_FLAG_SERVER);
                IPERF_FLAG_CLR(cfg.flag, IPERF_FLAG_CLIENT);
            } else if (strcmp(aArgs[i], "-V") == 0) {
                cfg.type = IPERF_IP_TYPE_IPV6;
            } else if (strcmp(aArgs[i], "-u") == 0) {
                IPERF_FLAG_SET(cfg.flag, IPERF_FLAG_UDP);
                IPERF_FLAG_CLR(cfg.flag, IPERF_FLAG_TCP);
            } else if (strcmp(aArgs[i], "-p") == 0) {
                i++;
                if (cfg.flag & IPERF_FLAG_SERVER) {
                    cfg.sport = atoi(aArgs[i]);
                    cfg.dport = IPERF_DEFAULT_PORT;
                } else {
                    cfg.sport = IPERF_DEFAULT_PORT;
                    cfg.dport = atoi(aArgs[i]);
                }
                otCliOutputFormat("dp:%d\n", cfg.dport);
                otCliOutputFormat("sp:%d\n", cfg.sport);
            } else if (strcmp(aArgs[i], "-i") == 0) {
                i++;
                if (atoi(aArgs[i]) < 0) {
                    cfg.interval = IPERF_DEFAULT_INTERVAL;
                } else {
                    cfg.interval = atoi(aArgs[i]);
                }
                otCliOutputFormat("i:%d\n", cfg.interval);
            } else if (strcmp(aArgs[i], "-t") == 0) {
                i++;
                cfg.time = atoi(aArgs[i]);
                if (cfg.time <= cfg.interval) {
                    cfg.time = cfg.interval;
                }
                otCliOutputFormat("t:%d\n", cfg.time);
            } else if (strcmp(aArgs[i], "-l") == 0) {
                i++;
                if (atoi(aArgs[i]) <= 0) {
                    ESP_LOGE(TAG, "Invalid arguments.");
                } else {
                    cfg.len_send_buf = atoi(aArgs[i]);
                }
            } else if (strcmp(aArgs[i], "-a") == 0) {
                iperf_stop();
                return;
            }
        }
        iperf_start(&cfg);
    }
}
