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

#include <stdio.h>
#include <string.h>

#include "rom/ets_sys.h"
#include "rom/gpio.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/gpio_reg.h"
#include "soc/dport_reg.h"
#include "soc/emac_ex_reg.h"
#include "soc/emac_reg_v2.h"
#include "soc/soc.h"

#include "tcpip_adapter.h"
#include "sdkconfig.h"

#include "esp_task_wdt.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_eth.h"
#include "esp_intr_alloc.h"

#include "emac_common.h"
#include "emac_desc.h"

#include "freertos/xtensa_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"

#include "lwip/err.h"

#define EMAC_EVT_QNUM 200
#define EMAC_SIG_MAX 50

static struct emac_config_data emac_config;

static uint8_t emac_dma_rx_chain_buf[32 * DMA_RX_BUF_NUM];
static uint8_t emac_dma_tx_chain_buf[32 * DMA_TX_BUF_NUM];
static uint8_t emac_dma_rx_buf[DMA_RX_BUF_SIZE * DMA_RX_BUF_NUM];
static uint8_t emac_dma_tx_buf[DMA_TX_BUF_SIZE * DMA_TX_BUF_NUM];

static SemaphoreHandle_t emac_g_sem;
static portMUX_TYPE g_emac_mux = portMUX_INITIALIZER_UNLOCKED;
static xTaskHandle emac_task_hdl;
static xQueueHandle emac_xqueue;
static uint8_t emac_sig_cnt[EMAC_SIG_MAX] = {0};
static TimerHandle_t emac_timer = NULL;
static SemaphoreHandle_t emac_rx_xMutex = NULL;
static SemaphoreHandle_t emac_tx_xMutex = NULL;
static const char *TAG = "emac";
static bool pause_send = false;

static esp_err_t emac_ioctl(emac_sig_t sig, emac_par_t par);
esp_err_t emac_post(emac_sig_t sig, emac_par_t par);

static void emac_macaddr_init(void)
{
    esp_efuse_read_mac(&(emac_config.macaddr[0]));
    emac_config.macaddr[5] = emac_config.macaddr[5] + 3;
}

void esp_eth_get_mac(uint8_t mac[6])
{
    memcpy(mac, &(emac_config.macaddr[0]), 6);
}

static void emac_setup_tx_desc(struct dma_extended_desc *tx_desc , uint32_t size)
{
    tx_desc->basic.desc1 = size & 0xfff;
    tx_desc->basic.desc0 = EMAC_DESC_INT_COMPL | EMAC_DESC_LAST_SEGMENT | EMAC_DESC_FIRST_SEGMENT | EMAC_DESC_SECOND_ADDR_CHAIN;
    tx_desc->basic.desc0 = EMAC_DESC_TX_OWN | EMAC_DESC_INT_COMPL | EMAC_DESC_LAST_SEGMENT | EMAC_DESC_FIRST_SEGMENT | EMAC_DESC_SECOND_ADDR_CHAIN;
}

static void emac_clean_tx_desc(struct dma_extended_desc *tx_desc)
{
    tx_desc->basic.desc1 = 0;
    tx_desc->basic.desc0 = 0;
}

static void emac_clean_rx_desc(struct dma_extended_desc *rx_desc , uint32_t buf_ptr)
{
    if (buf_ptr != 0) {
        rx_desc->basic.desc2 = buf_ptr;
    }
    rx_desc->basic.desc1 = EMAC_DESC_RX_SECOND_ADDR_CHAIN | DMA_RX_BUF_SIZE;
    rx_desc->basic.desc0 = EMAC_DESC_RX_OWN;
}

static void emac_set_tx_base_reg(void)
{
    REG_WRITE(EMAC_DMATXBASEADDR_REG, (uint32_t)(emac_config.dma_etx));
}

static void emac_set_rx_base_reg(void)
{
    REG_WRITE(EMAC_DMARXBASEADDR_REG, (uint32_t)(emac_config.dma_erx));
}

static void emac_reset_dma_chain(void)
{
    emac_config.cnt_tx = 0;
    emac_config.cur_tx = 0;
    emac_config.dirty_tx = 0;

    emac_config.cnt_rx = 0;
    emac_config.cur_rx = 0;
    emac_config.dirty_rx = 0;
}

static void emac_init_dma_chain(void)
{
    int i;
    uint32_t dma_phy;
    struct dma_extended_desc *p = NULL;

    //init tx chain
    emac_config.dma_etx = (struct dma_extended_desc *)(&emac_dma_tx_chain_buf[0]);
    emac_config.cnt_tx = 0;
    emac_config.cur_tx = 0;
    emac_config.dirty_tx = 0;

    dma_phy = (uint32_t)(emac_config.dma_etx);
    p = emac_config.dma_etx;

    for (i = 0; i < (DMA_TX_BUF_NUM - 1); i++ ) {
        dma_phy += sizeof(struct dma_extended_desc);
        emac_clean_tx_desc(p);
        p->basic.desc3 = dma_phy;
        p->basic.desc2 = (uint32_t)(&emac_dma_tx_buf[0]) + (i * DMA_TX_BUF_SIZE);
        p++;
    }
    p->basic.desc3 = (uint32_t)(emac_config.dma_etx);
    p->basic.desc2 = (uint32_t)(&emac_dma_tx_buf[0]) + (i * DMA_TX_BUF_SIZE);

    //init desc0 desc1
    emac_clean_tx_desc(p);

    //init rx chain
    emac_config.dma_erx = (struct dma_extended_desc *)(&emac_dma_rx_chain_buf[0]);
    emac_config.cnt_rx = 0;
    emac_config.cur_rx = 0;
    emac_config.dirty_rx = 0;

    dma_phy = (uint32_t)(emac_config.dma_erx);
    p = emac_config.dma_erx;

    for (i = 0; i < (DMA_RX_BUF_NUM - 1); i++ ) {
        dma_phy += sizeof(struct dma_extended_desc);
        emac_clean_rx_desc(p, (uint32_t)(&emac_dma_rx_buf[0]) + (i * DMA_RX_BUF_SIZE));
        p->basic.desc3 = dma_phy;
        p++;
    }

    emac_clean_rx_desc(p, (uint32_t)(&emac_dma_rx_buf[0]) + (i * DMA_RX_BUF_SIZE));
    p->basic.desc3 = (uint32_t)(emac_config.dma_erx);
}

void esp_eth_smi_write(uint32_t reg_num, uint16_t value)
{
    uint32_t phy_num = emac_config.phy_addr;

    while (REG_GET_BIT(EMAC_GMACGMIIADDR_REG, EMAC_GMIIBUSY) == 1 ) {
    }

    REG_WRITE(EMAC_GMACGMIIDATA_REG, value);
    REG_WRITE(EMAC_GMACGMIIADDR_REG, 0x3 | ((reg_num & 0x1f) << 6) | ((phy_num & 0x1f) << 11) | ((0x3) << 2));

    while (REG_GET_BIT(EMAC_GMACGMIIADDR_REG, EMAC_GMIIBUSY) == 1 ) {
    }
}

uint16_t esp_eth_smi_read(uint32_t reg_num)
{
    uint32_t phy_num = emac_config.phy_addr;
    uint16_t value = 0;

    while (REG_GET_BIT(EMAC_GMACGMIIADDR_REG, EMAC_GMIIBUSY) == 1 ) {
    }

    REG_WRITE(EMAC_GMACGMIIADDR_REG, 0x1 | ((reg_num & 0x1f) << 6) | ((phy_num & 0x1f) << 11) | (0x3 << 2));
    while (REG_GET_BIT(EMAC_GMACGMIIADDR_REG, EMAC_GMIIBUSY) == 1 ) {
    }
    value = (REG_READ(EMAC_GMACGMIIDATA_REG) & 0xffff);

    return value;
}

static void emac_set_user_config_data(eth_config_t *config )
{
    emac_config.phy_addr = config->phy_addr;
    emac_config.mac_mode = config->mac_mode;
    emac_config.phy_init = config->phy_init;
    emac_config.emac_tcpip_input = config->tcpip_input;
    emac_config.emac_gpio_config = config->gpio_config;
    emac_config.emac_phy_check_link = config->phy_check_link;
    emac_config.emac_phy_check_init = config->phy_check_init;
    emac_config.emac_phy_get_speed_mode = config->phy_get_speed_mode;
    emac_config.emac_phy_get_duplex_mode = config->phy_get_duplex_mode;
#if DMA_RX_BUF_NUM > 9
    emac_config.emac_flow_ctrl_enable = config->flow_ctrl_enable;
#else 
    if(config->flow_ctrl_enable == true) {
        ESP_LOGE(TAG, "eth flow ctrl init err!!! Please run make menuconfig and make sure DMA_RX_BUF_NUM > 9 .");
    }
    emac_config.emac_flow_ctrl_enable = false;
#endif
    emac_config.emac_phy_get_partner_pause_enable = config->phy_get_partner_pause_enable;
}

static void emac_enable_intr()
{
    REG_WRITE(EMAC_DMAINTERRUPT_EN_REG, EMAC_INTR_ENABLE_BIT);
}

static void emac_disable_intr()
{
    REG_WRITE(EMAC_DMAINTERRUPT_EN_REG, 0);
}

static esp_err_t emac_verify_args(void)
{
    esp_err_t ret = ESP_OK;

    if (emac_config.phy_addr > PHY31) {
        ESP_LOGE(TAG, "phy addr err");
        ret = ESP_FAIL;
    }

    if (emac_config.mac_mode != ETH_MODE_RMII) {
        ESP_LOGE(TAG, "mac mode err,now only support RMII");
        ret = ESP_FAIL;
    }

    if (emac_config.phy_init == NULL) {
        ESP_LOGE(TAG, "phy_init func is null");
        ret = ESP_FAIL;
    }

    if (emac_config.emac_tcpip_input == NULL) {
        ESP_LOGE(TAG, "tcpip_input func is null");
        ret = ESP_FAIL;
    }

    if (emac_config.emac_gpio_config == NULL) {
        ESP_LOGE(TAG, "gpio config func is null");
        ret = ESP_FAIL;
    }

    if (emac_config.emac_phy_check_link == NULL) {
        ESP_LOGE(TAG, "phy check link func is null");
        ret = ESP_FAIL;
    }

    if (emac_config.emac_phy_check_init == NULL) {
        ESP_LOGE(TAG, "phy check init func is null");
        ret = ESP_FAIL;
    }

    if (emac_config.emac_phy_get_speed_mode == NULL) {
        ESP_LOGE(TAG, "phy get speed mode func is null");
        ret = ESP_FAIL;
    }

    if (emac_config.emac_phy_get_duplex_mode == NULL) {
        ESP_LOGE(TAG, "phy get duplex mode func is null");
        ret = ESP_FAIL;
    }

    if (emac_config.emac_flow_ctrl_enable == true && emac_config.emac_phy_get_partner_pause_enable == NULL) {
        ESP_LOGE(TAG, "phy get partner pause enable func is null");
        ret = ESP_FAIL;
    }

    return ret;
}

//TODO for mac filter
void emac_set_mac_addr(void)
{
}

//TODO
void emac_check_mac_addr(void)
{
}

static void emac_process_tx(void)
{
    uint32_t cur_tx_desc = emac_read_tx_cur_reg();

    if (emac_config.emac_status == EMAC_RUNTIME_STOP) {
        return;
    }

    xSemaphoreTakeRecursive( emac_tx_xMutex, ( TickType_t ) portMAX_DELAY );

    while (((uint32_t) & (emac_config.dma_etx[emac_config.dirty_tx].basic.desc0) != cur_tx_desc)) {
        emac_clean_tx_desc(&(emac_config.dma_etx[emac_config.dirty_tx]));
        emac_config.dirty_tx = (emac_config.dirty_tx + 1) % DMA_TX_BUF_NUM;
        emac_config.cnt_tx --;

        if (emac_config.cnt_tx < 0) {
            ESP_LOGE(TAG, "emac tx chain err");
        }
        cur_tx_desc = emac_read_tx_cur_reg();
    }

    xSemaphoreGiveRecursive( emac_tx_xMutex );
}

void esp_eth_free_rx_buf(void *buf)
{
    xSemaphoreTakeRecursive( emac_rx_xMutex, ( TickType_t ) portMAX_DELAY );

    emac_clean_rx_desc(&(emac_config.dma_erx[emac_config.cur_rx]), (uint32_t) buf);
    emac_config.cur_rx = (emac_config.cur_rx + 1) % DMA_RX_BUF_NUM;
    emac_config.cnt_rx--;
    if (emac_config.cnt_rx < 0) {
        ESP_LOGE(TAG, "emac rx buf err!!\n");
    }
    emac_poll_rx_cmd();

    xSemaphoreGiveRecursive( emac_rx_xMutex );

    if (emac_config.emac_flow_ctrl_partner_support == true) {
        portENTER_CRITICAL(&g_emac_mux);
        if (pause_send == true && emac_config.cnt_rx < FLOW_CONTROL_LOW_WATERMARK) {
            emac_send_pause_zero_frame_enable();
            pause_send = false;
        }
        portEXIT_CRITICAL(&g_emac_mux);
    }
}

static uint32_t IRAM_ATTR emac_get_rxbuf_count_in_intr(void)
{
    uint32_t cnt = 0;
    uint32_t cur_rx_desc = emac_read_rx_cur_reg();
    struct dma_extended_desc *cur_desc = (struct dma_extended_desc *)cur_rx_desc;

    while (cur_desc->basic.desc0 == EMAC_DESC_RX_OWN) {
        cnt++;
        cur_desc = (struct dma_extended_desc *)cur_desc->basic.desc3;
    }
    return cnt;
}

#if CONFIG_EMAC_L2_TO_L3_RX_BUF_MODE
static void emac_process_rx(void)
{
    if (emac_config.emac_status == EMAC_RUNTIME_STOP) {
        return;
    }
    uint32_t cur_rx_desc = emac_read_rx_cur_reg();

    while (((uint32_t) & (emac_config.dma_erx[emac_config.dirty_rx].basic.desc0) != cur_rx_desc)) {
        //copy data to lwip
        emac_config.emac_tcpip_input((void *)(emac_config.dma_erx[emac_config.dirty_rx].basic.desc2),
                                     (((emac_config.dma_erx[emac_config.dirty_rx].basic.desc0) >> EMAC_DESC_FRAME_LENGTH_S) & EMAC_DESC_FRAME_LENGTH) , NULL);

        emac_clean_rx_desc(&(emac_config.dma_erx[emac_config.dirty_rx]), (emac_config.dma_erx[emac_config.dirty_rx].basic.desc2));
        emac_config.dirty_rx = (emac_config.dirty_rx + 1) % DMA_RX_BUF_NUM;

        //if open this ,one intr can do many intrs ?
        cur_rx_desc = emac_read_rx_cur_reg();
    }

    emac_enable_rx_intr();
}

static void emac_process_rx_unavail(void)
{
    if (emac_config.emac_status == EMAC_RUNTIME_STOP) {
        return;
    }

    uint32_t dirty_cnt = 0;
    while (dirty_cnt < DMA_RX_BUF_NUM) {

        if (emac_config.dma_erx[emac_config.dirty_rx].basic.desc0 == EMAC_DESC_RX_OWN) {
            break;
        }

        dirty_cnt ++;
        //copy data to lwip
        emac_config.emac_tcpip_input((void *)(emac_config.dma_erx[emac_config.dirty_rx].basic.desc2),
                                     (((emac_config.dma_erx[emac_config.dirty_rx].basic.desc0) >> EMAC_DESC_FRAME_LENGTH_S) & EMAC_DESC_FRAME_LENGTH) , NULL);

        emac_clean_rx_desc(&(emac_config.dma_erx[emac_config.dirty_rx]), (emac_config.dma_erx[emac_config.dirty_rx].basic.desc2));
        emac_config.dirty_rx = (emac_config.dirty_rx + 1) % DMA_RX_BUF_NUM;
    }
    emac_enable_rx_intr();
    emac_enable_rx_unavail_intr();
    emac_poll_rx_cmd();
}

#else
static void emac_process_rx_unavail(void)
{
    if (emac_config.emac_status == EMAC_RUNTIME_STOP) {
        return;
    }

    xSemaphoreTakeRecursive( emac_rx_xMutex, ( TickType_t ) portMAX_DELAY );

    while (emac_config.cnt_rx < DMA_RX_BUF_NUM) {

        //copy data to lwip
        emac_config.emac_tcpip_input((void *)(emac_config.dma_erx[emac_config.dirty_rx].basic.desc2),
                                     (((emac_config.dma_erx[emac_config.dirty_rx].basic.desc0) >> EMAC_DESC_FRAME_LENGTH_S) & EMAC_DESC_FRAME_LENGTH) , NULL);
        emac_config.cnt_rx++;
        if (emac_config.cnt_rx > DMA_RX_BUF_NUM) {
            ESP_LOGE(TAG, "emac rx unavail buf err !!\n");
        }
        emac_config.dirty_rx = (emac_config.dirty_rx + 1) % DMA_RX_BUF_NUM;
    }
    emac_enable_rx_intr();
    emac_enable_rx_unavail_intr();
    xSemaphoreGiveRecursive( emac_rx_xMutex );
}

static void emac_process_rx(void)
{
    if (emac_config.emac_status == EMAC_RUNTIME_STOP) {
        return;
    }

    uint32_t cur_rx_desc = emac_read_rx_cur_reg();

    xSemaphoreTakeRecursive( emac_rx_xMutex, ( TickType_t ) portMAX_DELAY );

    if (((uint32_t) & (emac_config.dma_erx[emac_config.dirty_rx].basic.desc0) != cur_rx_desc)) {

        while (((uint32_t) & (emac_config.dma_erx[emac_config.dirty_rx].basic.desc0) != cur_rx_desc) && emac_config.cnt_rx < DMA_RX_BUF_NUM ) {
            //copy data to lwip
            emac_config.emac_tcpip_input((void *)(emac_config.dma_erx[emac_config.dirty_rx].basic.desc2),
                                         (((emac_config.dma_erx[emac_config.dirty_rx].basic.desc0) >> EMAC_DESC_FRAME_LENGTH_S) & EMAC_DESC_FRAME_LENGTH) , NULL);

            emac_config.cnt_rx++;

            if (emac_config.cnt_rx > DMA_RX_BUF_NUM ) {
                ESP_LOGE(TAG, "emac rx buf err!!\n");
            }
            emac_config.dirty_rx = (emac_config.dirty_rx + 1) % DMA_RX_BUF_NUM;

            cur_rx_desc = emac_read_rx_cur_reg();
        }
    } else {
        if (emac_config.cnt_rx < DMA_RX_BUF_NUM) {
            if ((emac_config.dma_erx[emac_config.dirty_rx].basic.desc0 & EMAC_DESC_RX_OWN) == 0) {
                while (emac_config.cnt_rx < DMA_RX_BUF_NUM) {

                    //copy data to lwip
                    emac_config.emac_tcpip_input((void *)(emac_config.dma_erx[emac_config.dirty_rx].basic.desc2),
                                                 (((emac_config.dma_erx[emac_config.dirty_rx].basic.desc0) >> EMAC_DESC_FRAME_LENGTH_S) & EMAC_DESC_FRAME_LENGTH) , NULL);
                    emac_config.cnt_rx++;
                    if (emac_config.cnt_rx > DMA_RX_BUF_NUM) {
                        ESP_LOGE(TAG, "emac rx buf err!!!\n");
                    }

                    emac_config.dirty_rx = (emac_config.dirty_rx + 1) % DMA_RX_BUF_NUM;
                }
            }
        }
    }
    emac_enable_rx_intr();
    xSemaphoreGiveRecursive( emac_rx_xMutex );
}
#endif

//TODO other events need to do something
static void IRAM_ATTR emac_process_intr(void *arg)
{
    uint32_t event;
    event = REG_READ(EMAC_DMASTATUS_REG);

    //clr intrs
    REG_WRITE(EMAC_DMASTATUS_REG, event);

    if (event & EMAC_RECV_INT) {
        emac_disable_rx_intr();
        if (emac_config.emac_flow_ctrl_partner_support == true) {
            if (emac_get_rxbuf_count_in_intr() < FLOW_CONTROL_HIGH_WATERMARK && pause_send == false ) {
                pause_send = true;
                emac_send_pause_frame_enable();
            }
        }
        emac_post(SIG_EMAC_RX_DONE, 0);
    }

    if (event & EMAC_RECV_BUF_UNAVAIL) {
        emac_disable_rx_unavail_intr();
        emac_post(SIG_EMAC_RX_UNAVAIL, 0);
    }

    if (event & EMAC_TRANS_INT) {
        emac_post(SIG_EMAC_TX_DONE, 0);
    }
}

static void emac_set_macaddr_reg(void)
{
    REG_SET_FIELD(EMAC_GMACADDR0HIGH_REG, EMAC_MAC_ADDRESS0_HI, (emac_config.macaddr[0] << 8) | (emac_config.macaddr[1]));
    REG_WRITE(EMAC_GMACADDR0LOW_REG, (emac_config.macaddr[2] << 24) |  (emac_config.macaddr[3] << 16) | (emac_config.macaddr[4] << 8) | (emac_config.macaddr[5]));
}

static void emac_check_phy_init(void)
{
    emac_config.emac_phy_check_init();
    if (emac_config.emac_phy_get_duplex_mode() == ETH_MDOE_FULLDUPLEX) {
        REG_SET_BIT(EMAC_GMACCONFIG_REG, EMAC_GMACDUPLEX);
    } else {
        REG_CLR_BIT(EMAC_GMACCONFIG_REG, EMAC_GMACDUPLEX);
    }
    if (emac_config.emac_phy_get_speed_mode() == ETH_SPEED_MODE_100M) {
        REG_SET_BIT(EMAC_GMACCONFIG_REG, EMAC_GMACFESPEED);
    } else {
        REG_CLR_BIT(EMAC_GMACCONFIG_REG, EMAC_GMACFESPEED);
    }
    if (emac_config.emac_flow_ctrl_enable == true) {
        if (emac_config.emac_phy_get_partner_pause_enable() == true && emac_config.emac_phy_get_duplex_mode() == ETH_MDOE_FULLDUPLEX) {
            emac_enable_flowctrl();
            emac_config.emac_flow_ctrl_partner_support = true;
        } else {
            emac_disable_flowctrl();
            emac_config.emac_flow_ctrl_partner_support = false;
        }
    } else {
        emac_disable_flowctrl();
        emac_config.emac_flow_ctrl_partner_support = false;
    }
    emac_mac_enable_txrx();
}
static void emac_process_link_updown(bool link_status)
{
    system_event_t evt;
    uint8_t i = 0;

    emac_config.phy_link_up = link_status;

    if (link_status == true) {
        emac_check_phy_init();
        ESP_LOGI(TAG, "eth link_up!!!");
        emac_enable_dma_tx();
        emac_enable_dma_rx();
        for (i = 0; i < PHY_LINK_CHECK_NUM; i++) {
            emac_check_phy_init();
        }

        evt.event_id = SYSTEM_EVENT_ETH_CONNECTED;
    } else {
        ESP_LOGI(TAG, "eth link_down!!!");
        emac_disable_dma_tx();
        emac_disable_dma_rx();
        evt.event_id = SYSTEM_EVENT_ETH_DISCONNECTED;
    }

    esp_event_send(&evt);
}

static void emac_hw_init(void)
{
    //init chain
    emac_init_dma_chain();

    //get hw features TODO

    //ipc TODO
}

esp_err_t esp_eth_tx(uint8_t *buf, uint16_t size)
{
    esp_err_t ret = ESP_OK;

    if (emac_config.emac_status != EMAC_RUNTIME_START || emac_config.emac_status == EMAC_RUNTIME_NOT_INIT) {
        ESP_LOGI(TAG, "tx netif close");
        ret = ERR_IF;
        return ret;
    }

    xSemaphoreTakeRecursive( emac_tx_xMutex, ( TickType_t ) portMAX_DELAY );
    if (emac_config.cnt_tx == DMA_TX_BUF_NUM - 1) {
        ESP_LOGD(TAG, "tx buf full");
        ret = ERR_MEM;
        goto _exit;
    }

    memcpy((uint8_t *)(emac_config.dma_etx[emac_config.cur_tx].basic.desc2), (uint8_t *)buf, size);

    emac_setup_tx_desc(&(emac_config.dma_etx[emac_config.cur_tx]), size);

    emac_config.cnt_tx ++;
    emac_config.cur_tx = (emac_config.cur_tx + 1) % DMA_TX_BUF_NUM ;

    emac_poll_tx_cmd();

_exit:

    xSemaphoreGiveRecursive( emac_tx_xMutex );
    return ret;
}

static void emac_init_default_data(void)
{
    memset((uint8_t *)&emac_config, 0, sizeof(struct emac_config_data));
}

void emac_process_link_check(void)
{
    if (emac_config.emac_status != EMAC_RUNTIME_START ||
            emac_config.emac_status == EMAC_RUNTIME_NOT_INIT) {
        return;
    }

    if (emac_config.emac_phy_check_link() == true ) {
        if (emac_config.phy_link_up == false) {
            emac_process_link_updown(true);
        }
    } else {
        if (emac_config.phy_link_up == true) {
            emac_process_link_updown(false);
        }
    }
}

void emac_link_check_func(void *pv_parameters)
{
    emac_post(SIG_EMAC_CHECK_LINK, 0);
}

static bool emac_link_check_timer_init(void)
{
    emac_timer = xTimerCreate("emac_timer", (2000 / portTICK_PERIOD_MS),
                              pdTRUE, (void *)rand(), emac_link_check_func);
    if (emac_timer == NULL) {
        return false;
    } else {
        return true;
    }
}

static bool emac_link_check_timer_start(void)
{
    if (xTimerStart(emac_timer, portMAX_DELAY) != pdPASS) {
        return false;
    } else {
        return true;
    }
}

static bool emac_link_check_timer_stop(void)
{
    if (xTimerStop(emac_timer, portMAX_DELAY) != pdPASS) {
        return false;
    } else {
        return true;
    }
}

static bool emac_link_check_timer_delete(void)
{
    xTimerDelete(emac_timer, portMAX_DELAY);
    emac_timer = NULL;
    return true;
}

static void emac_start(void *param)
{
    struct emac_post_cmd  *post_cmd = (struct emac_post_cmd *)param;
    struct emac_open_cmd *cmd = (struct emac_open_cmd *)(post_cmd->cmd);

    ESP_LOGI(TAG , "emac start !!!\n");
    cmd->err = EMAC_CMD_OK;
    emac_enable_clk(true);

    emac_reset();
    emac_macaddr_init();

    emac_check_mac_addr();

    emac_set_mac_addr();
    emac_set_macaddr_reg();

    emac_set_tx_base_reg();
    emac_set_rx_base_reg();

    emac_mac_init();

    emac_config.phy_init();

    //ptp TODO

    emac_enable_intr();

    emac_config.emac_status = EMAC_RUNTIME_START;

    system_event_t evt;
    evt.event_id = SYSTEM_EVENT_ETH_START;
    esp_event_send(&evt);

    //set a timer to check link up status
    if (emac_link_check_timer_init() == true) {
        if (emac_link_check_timer_start() != true) {
            cmd->err = EMAC_CMD_FAIL;
            emac_link_check_timer_delete();
        }
    } else {
        cmd->err = EMAC_CMD_FAIL;
    }

    if (post_cmd->post_type == EMAC_POST_SYNC) {
        xSemaphoreGive(emac_g_sem);
    }

    ESP_LOGI(TAG, "emac start success !!!");
}

esp_err_t esp_eth_enable(void)
{
    struct emac_post_cmd post_cmd;
    struct emac_open_cmd open_cmd;

    post_cmd.cmd = (void *)(&open_cmd);
    open_cmd.err = EMAC_CMD_OK;

    if (emac_config.emac_status == EMAC_RUNTIME_START) {
        open_cmd.err = EMAC_CMD_OK;
        return open_cmd.err;
    }

    if (emac_config.emac_status != EMAC_RUNTIME_NOT_INIT) {
        if (emac_ioctl(SIG_EMAC_START, (emac_par_t)(&post_cmd)) != 0) {
            open_cmd.err = EMAC_CMD_FAIL;
        }
    } else {
        open_cmd.err = EMAC_CMD_FAIL;
    }
    return open_cmd.err;
}

static void emac_stop(void *param)
{
    struct emac_post_cmd  *post_cmd = (struct emac_post_cmd *)param;
    ESP_LOGI(TAG, "emac stop");

    emac_link_check_timer_stop();
    emac_link_check_timer_delete();

    emac_process_link_updown(false);

    emac_disable_intr();
    emac_reset_dma_chain();
    emac_reset();
    emac_enable_clk(false);

    emac_config.emac_status = EMAC_RUNTIME_STOP;
    system_event_t evt;
    evt.event_id = SYSTEM_EVENT_ETH_STOP;
    esp_event_send(&evt);

    if (post_cmd->post_type == EMAC_POST_SYNC) {
        xSemaphoreGive(emac_g_sem);
    }

    ESP_LOGI(TAG, "emac stop success !!!");
}

esp_err_t esp_eth_disable(void)
{
    struct emac_post_cmd post_cmd;
    struct emac_close_cmd close_cmd;

    post_cmd.cmd = (void *)(&close_cmd);
    close_cmd.err = EMAC_CMD_OK;

    if (emac_config.emac_status == EMAC_RUNTIME_STOP) {
        close_cmd.err = EMAC_CMD_OK;
        return close_cmd.err;
    }

    if (emac_config.emac_status == EMAC_RUNTIME_START) {
        if (emac_ioctl(SIG_EMAC_STOP, (emac_par_t)(&post_cmd)) != 0) {
            close_cmd.err = EMAC_CMD_FAIL;
        }
    } else {
        close_cmd.err = EMAC_CMD_FAIL;
    }
    return close_cmd.err;
}

static esp_err_t emac_ioctl(emac_sig_t sig, emac_par_t par)
{
    esp_err_t ret = ESP_OK;
    struct emac_post_cmd  *post_cmd = (struct emac_post_cmd *)par;
    xTaskHandle task_hdl = xTaskGetCurrentTaskHandle();

    if (emac_task_hdl != task_hdl) {
        post_cmd->post_type = EMAC_POST_SYNC;
        if (emac_post(sig, par) != ESP_OK) {
            ret = ESP_FAIL;
            return ret;
        };

        if (xSemaphoreTake(emac_g_sem, portMAX_DELAY) == pdTRUE) {
            return ret;
        }
    } else {
        post_cmd->post_type = EMAC_POST_ASYNC;
        switch (sig) {
        case SIG_EMAC_RX_DONE:
            emac_process_rx();
            break;
        case SIG_EMAC_TX_DONE:
            emac_process_tx();
            break;
        case SIG_EMAC_START:
            emac_start((void *)par);
            break;
        case SIG_EMAC_STOP:
            emac_stop((void *)par);
            break;
        default:
            ESP_LOGE(TAG, "unexpect sig %d", sig);
            break;
        }
    }

    return ret;
}

void emac_task(void *pv)
{
    emac_event_t e;

    for (;;) {
        if (xQueueReceive(emac_xqueue, &e, (portTickType)portMAX_DELAY) == pdTRUE) {
            portENTER_CRITICAL(&g_emac_mux);
            emac_sig_cnt[e.sig]--;
            portEXIT_CRITICAL(&g_emac_mux);
            switch (e.sig) {
            case SIG_EMAC_RX_DONE:
                emac_process_rx();
                break;
            case SIG_EMAC_RX_UNAVAIL:
                emac_process_rx_unavail();
                break;
            case SIG_EMAC_TX_DONE:
                emac_process_tx();
                break;
            case SIG_EMAC_START:
                emac_start((void *)e.par);
                break;
            case SIG_EMAC_STOP:
                emac_stop((void *)e.par);
                break;
            case SIG_EMAC_CHECK_LINK:
                emac_process_link_check();
                break;
            default:
                ESP_LOGE(TAG, "unexpect sig %d", e.sig);
                break;
            }
        }
    }
}

esp_err_t IRAM_ATTR emac_post(emac_sig_t sig, emac_par_t par)
{
    if (sig <= SIG_EMAC_RX_DONE) {
        if (emac_sig_cnt[sig]) {
            return ESP_OK;
        } else {
            emac_sig_cnt[sig]++;
            emac_event_t evt;
            signed portBASE_TYPE ret;
            evt.sig = sig;
            evt.par = par;
            portBASE_TYPE tmp;

            ret = xQueueSendFromISR(emac_xqueue, &evt, &tmp);

            if (tmp != pdFALSE) {
                portYIELD_FROM_ISR();
            }

            if (ret != pdPASS) {
                return ESP_FAIL;
            }
        }
    } else {
        portENTER_CRITICAL(&g_emac_mux);
        emac_sig_cnt[sig]++;
        portEXIT_CRITICAL(&g_emac_mux);
        emac_event_t evt;
        evt.sig = sig;
        evt.par = par;

        if (xQueueSend(emac_xqueue, &evt, 10 / portTICK_PERIOD_MS) != pdTRUE) {
            return ESP_FAIL;
        }
    }

    return ESP_OK;
}

esp_err_t esp_eth_init(eth_config_t *config)
{
    esp_err_t ret = ESP_OK;
#if !CONFIG_ETHERNET
    ESP_LOGI(TAG, "eth driver init fail,please make menuconfig and enable ethernet .");
    ret = ESP_FAIL;
    goto _exit;
#endif

    emac_init_default_data();

    if (config != NULL ) {
        emac_set_user_config_data(config);
    }

    ret = emac_verify_args();

    if (ret != ESP_OK) {
        goto _exit;
    }

    //before set emac reg must enable clk
    emac_enable_clk(true);
    REG_SET_FIELD(EMAC_EX_PHYINF_CONF_REG, EMAC_EX_PHY_INTF_SEL, EMAC_EX_PHY_INTF_RMII);

    emac_dma_init();
    if (emac_config.mac_mode == ETH_MODE_RMII) {
        emac_set_clk_rmii();
    } else {
        emac_set_clk_mii();
    }

    emac_config.emac_gpio_config();

    ESP_LOGI(TAG, "mac version %04xa", emac_read_mac_version());
    emac_hw_init();

    //watchdog  TODO

    //init task for emac
    emac_g_sem = xSemaphoreCreateBinary();
    emac_rx_xMutex = xSemaphoreCreateRecursiveMutex();
    emac_tx_xMutex = xSemaphoreCreateRecursiveMutex();
    emac_xqueue = xQueueCreate(EMAC_EVT_QNUM, sizeof(emac_event_t));
    xTaskCreate(emac_task, "emacT", 2048, NULL, EMAC_TASK_PRIORITY, &emac_task_hdl);

    emac_enable_clk(false);
    esp_intr_alloc(ETS_ETH_MAC_INTR_SOURCE, 0, emac_process_intr, NULL, NULL);

    emac_config.emac_status = EMAC_RUNTIME_INIT;

_exit:
    return ret;
}

