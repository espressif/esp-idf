// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _EMAC_COMMON_H_
#define _EMAC_COMMON_H_

#include <stdint.h>

#include "esp_err.h"
#include "emac_dev.h"
#include "esp_eth.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t emac_sig_t;
typedef uint32_t emac_par_t;

typedef struct {
    emac_sig_t sig;
    emac_par_t par;
} emac_event_t;

enum emac_runtime_status {
    EMAC_RUNTIME_NOT_INIT = 0,
    EMAC_RUNTIME_INIT,
    EMAC_RUNTIME_START,
    EMAC_RUNTIME_STOP,
};

enum {
    SIG_EMAC_RX_UNAVAIL,
    SIG_EMAC_TX_DONE,
    SIG_EMAC_RX_DONE,
    SIG_EMAC_START,
    SIG_EMAC_STOP,
    SIG_EMAC_CHECK_LINK,
    SIG_EMAC_MAX
};

struct emac_config_data {
    eth_phy_base_t phy_addr;
    eth_mode_t mac_mode;
    struct dma_extended_desc *dma_etx;
    uint32_t cur_tx;
    uint32_t dirty_tx;
    int32_t cnt_tx;
    struct dma_extended_desc *dma_erx;
    uint32_t cur_rx;
    uint32_t dirty_rx;
    int32_t cnt_rx;
    uint32_t rx_need_poll;
    bool phy_link_up;
    enum emac_runtime_status emac_status;
    uint8_t macaddr[6];
    eth_phy_func phy_init;
    eth_tcpip_input_func emac_tcpip_input;
    eth_gpio_config_func emac_gpio_config;
    eth_phy_check_link_func emac_phy_check_link;
    eth_phy_check_init_func emac_phy_check_init;
    eth_phy_get_speed_mode_func emac_phy_get_speed_mode;
    eth_phy_get_duplex_mode_func emac_phy_get_duplex_mode;
    bool emac_flow_ctrl_enable;
    bool emac_flow_ctrl_partner_support;
    eth_phy_get_partner_pause_enable_func  emac_phy_get_partner_pause_enable;
    eth_phy_power_enable_func  emac_phy_power_enable;
};

enum emac_post_type {
    EMAC_POST_ASYNC,
    EMAC_POST_SYNC,
};

struct emac_post_cmd {
    void *cmd;
    enum emac_post_type post_type;
};

struct emac_tx_cmd {
    uint8_t *buf;
    uint16_t size;
    int8_t err;
};

struct emac_open_cmd {
    int8_t err;
};

struct emac_close_cmd {
    int8_t err;
};

#define DMA_RX_BUF_NUM CONFIG_DMA_RX_BUF_NUM
#define DMA_TX_BUF_NUM CONFIG_DMA_TX_BUF_NUM
#define EMAC_TASK_PRIORITY CONFIG_EMAC_TASK_PRIORITY

#define DMA_RX_BUF_SIZE 1600
#define DMA_TX_BUF_SIZE 1600

//rest buf num
#define FLOW_CONTROL_HIGH_WATERMARK 3
//used buf num
#define FLOW_CONTROL_LOW_WATERMARK  6

#define PHY_LINK_CHECK_NUM  5

#define EMAC_CMD_OK 0
#define EMAC_CMD_FAIL -1

#ifdef __cplusplus
}
#endif

#endif
