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

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t emac_sig_t;
typedef uint32_t emac_par_t;

typedef struct {
    emac_sig_t sig;
    emac_par_t par;
} emac_event_t;

enum emac_mode {
    EMAC_MODE_RMII = 0,
    EMAC_MDOE_MII,
};

enum emac_runtime_status {
    EMAC_RUNTIME_NOT_INIT = 0,
    EMAC_RUNTIME_INIT,
    EMAC_RUNTIME_START,
    EMAC_RUNTIME_STOP,
};

enum {
    SIG_EMAC_TX_DONE,
    SIG_EMAC_RX_DONE,
    SIG_EMAC_TX,
    SIG_EMAC_START,
    SIG_EMAC_STOP,
    SIG_EMAC_MAX
};

typedef void (*emac_phy_fun)(void);
typedef esp_err_t (*emac_tcpip_input_fun)(void *buffer, uint16_t len, void *eb);
typedef void (*emac_gpio_config_func)(void);

struct emac_config_data {
    unsigned int  phy_addr;
    enum emac_mode mac_mode;
    struct dma_extended_desc *dma_etx;
    unsigned int cur_tx;
    unsigned int dirty_tx;
    signed int cnt_tx;
    struct dma_extended_desc *dma_erx;
    unsigned int cur_rx;
    unsigned int dirty_rx;
    signed int cnt_rx;
    unsigned int rx_need_poll;
    bool phy_link_up;
    enum emac_runtime_status emac_status;
    uint8_t macaddr[6];
    emac_phy_fun phy_init;
    emac_tcpip_input_fun emac_tcpip_input;
    emac_gpio_config_func emac_gpio_config;
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
#if CONFIG_ETHERNET
#define DMA_RX_BUF_NUM CONFIG_DMA_RX_BUF_NUM
#define DMA_TX_BUF_NUM CONFIG_DMA_TX_BUF_NUM
#else
#define DMA_RX_BUF_NUM 1
#define DMA_TX_BUF_NUM 1
#endif
#define DMA_RX_BUF_SIZE 1600
#define DMA_TX_BUF_SIZE 1600

//lwip err
#define ERR_OK   0
#define ERR_MEM -1
#define ERR_IF  -16

#define EMAC_CMD_OK 0
#define EMAC_CMD_FAIL -1

#ifdef __cplusplus
}
#endif

#endif
