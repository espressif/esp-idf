/* cmd_sniffer example — declarations of command registration functions.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SNIFFER_INTF_WLAN = 0,
} sniffer_intf_t;

typedef enum {
    SNIFFER_WLAN_FILTER_MGMT = 0,
    SNIFFER_WLAN_FILTER_CTRL,
    SNIFFER_WLAN_FILTER_DATA,
    SNIFFER_WLAN_FILTER_MISC,
    SNIFFER_WLAN_FILTER_MPDU,
    SNIFFER_WLAN_FILTER_AMPDU,
    SNIFFER_WLAN_FILTER_MAX
} sniffer_wlan_filter_t;

typedef struct {
    char *filter_name;
    uint32_t filter_val;
} wlan_filter_table_t;

typedef struct {
    sniffer_intf_t interf;
    uint32_t channel;
    uint32_t duration;
    uint32_t filter;
} sniffer_config_t;

void register_sniffer();

#ifdef __cplusplus
}
#endif
