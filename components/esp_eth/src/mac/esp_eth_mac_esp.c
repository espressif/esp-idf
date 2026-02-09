/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include <stdarg.h>
#include <inttypes.h>
#include "esp_private/periph_ctrl.h"
#include "esp_private/sleep_retention.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_pm.h"
#include "esp_mac.h"
#include "esp_cpu.h"
#include "esp_heap_caps.h"
#include "esp_intr_alloc.h"
#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp_clock_output.h"
#endif // CONFIG_IDF_TARGET_ESP32
#include "hal/clk_tree_ll.h"
#include "esp_private/esp_clk.h"
#include "esp_private/esp_clk_tree_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "hal/emac_hal.h"
#include "soc/soc.h"
#include "hal/emac_periph.h"
#include "clk_ctrl_os.h"
#include "sdkconfig.h"
#include "esp_rom_sys.h"
#include "esp_private/eth_mac_esp_dma.h"
#include "esp_private/eth_mac_esp_gpio.h"

static const char *TAG = "esp.emac";

#define PHY_OPERATION_TIMEOUT_US        (1000)
#define MAC_STOP_TIMEOUT_US             (2500) // this is absolute maximum for 10Mbps, it is 10 times faster for 100Mbps
#define FLOW_CONTROL_LOW_WATER_MARK     (CONFIG_ETH_DMA_RX_BUFFER_NUM / 3)
#define FLOW_CONTROL_HIGH_WATER_MARK    (FLOW_CONTROL_LOW_WATER_MARK * 2)

#define EMAC_ALLOW_INTR_PRIORITY_MASK   ESP_INTR_FLAG_LOWMED

#define RMII_CLK_HZ                     (50000000)
#define RMII_10M_SPEED_RX_TX_CLK_DIV    (19)
#define RMII_100M_SPEED_RX_TX_CLK_DIV   (1)

#define EMAC_MULTI_REG_MUTEX_TIMEOUT_MS (100)

#define EMAC_USE_RETENTION_LINK (CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_EMAC_SUPPORT_SLEEP_RETENTION)

typedef struct {
    esp_eth_mac_t parent;
    esp_eth_mediator_t *eth;
    emac_hal_context_t hal;
    intr_handle_t intr_hdl;
    TaskHandle_t rx_task_hdl;
    emac_esp_dma_handle_t emac_dma_hndl;
    uint32_t sw_reset_timeout_ms;
    uint32_t frames_remain;
    uint32_t free_rx_descriptor;
    uint32_t flow_control_high_water_mark;
    uint32_t flow_control_low_water_mark;
    bool flow_ctrl_enabled; // indicates whether the user want to do flow control
    bool do_flow_ctrl;  // indicates whether we need to do software flow control
    bool use_pll;  // Only use (A/M)PLL in EMAC_DATA_INTERFACE_RMII && EMAC_CLK_OUT
    SemaphoreHandle_t multi_reg_mutex; // lock for multiple register access
    int32_t mdc_freq_hz;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
    eth_mac_dma_burst_len_t dma_burst_len;

    // ---- Chip specifics ----
#ifdef CONFIG_IDF_TARGET_ESP32
    esp_clock_output_mapping_handle_t rmii_clk_hdl; // we use the esp_clock_output driver to output a pre-configured APLL clock as the RMII reference clock
#endif
#ifdef SOC_EMAC_IEEE1588V2_SUPPORTED
    ts_target_exceed_cb_from_isr_t ts_target_exceed_cb_from_isr;
#endif
} emac_esp32_t;

static esp_err_t emac_esp_alloc_driver_obj(const eth_mac_config_t *config, emac_esp32_t **emac_out_hdl);
static void emac_esp_free_driver_obj(emac_esp32_t *emac);
static esp_err_t emac_esp32_start(esp_eth_mac_t *mac);
static esp_err_t emac_esp32_stop(esp_eth_mac_t *mac);

#if EMAC_USE_RETENTION_LINK
static esp_err_t emac_create_sleep_retention_link_cb(void *arg)
{
    esp_err_t err = sleep_retention_entries_create(emac_reg_retention_info.entry_array,
                                                   emac_reg_retention_info.array_size,
                                                   REGDMA_LINK_PRI_EMAC, emac_reg_retention_info.module_id);
    return err;
}

// TODO rename to emac_esp
static esp_err_t emac_create_retention_module(emac_esp32_t *emac)
{
    esp_err_t ret = ESP_OK;
    sleep_retention_module_t module = emac_reg_retention_info.module_id;
    if (sleep_retention_is_module_inited(module) && !sleep_retention_is_module_created(module)) {
        if ((ret = sleep_retention_module_allocate(module)) != ESP_OK) {
            ESP_LOGW(TAG, "create retention link failed on EMAC, power domain won't be turned off during sleep");
        }
    }
    return ret;
}

static void emac_esp_enable_emac_start_on_wakeup(emac_esp32_t *emac)
{
    void *emac_start_link;
    for (int i = 0; i < EMAC_REGDMA_LINK_EMAC_START_CNT; i++) {
        emac_start_link = sleep_retention_find_link_by_id(emac_reg_retention_info.entry_array[EMAC_REGDMA_LINK_EMAC_START_BEGIN + i].config.id);
        regdma_link_set_skip_flag(emac_start_link, 1, 0);
    }
}

static void emac_esp_disable_emac_start_on_wakeup(emac_esp32_t *emac)
{
    void *emac_start_link;
    for (int i = 0; i < EMAC_REGDMA_LINK_EMAC_START_CNT; i++) {
        emac_start_link = sleep_retention_find_link_by_id(emac_reg_retention_info.entry_array[EMAC_REGDMA_LINK_EMAC_START_BEGIN + i].config.id);
        regdma_link_set_skip_flag(emac_start_link, 1, 1);
    }
}

static void emac_free_retention_module(emac_esp32_t *emac)
{
    sleep_retention_module_t module = emac_reg_retention_info.module_id;
    if (sleep_retention_is_module_created(module)) {
        sleep_retention_module_free(module);
    }
}
#endif // EMAC_USE_RETENTION_LINK

static esp_err_t emac_esp32_lock_multi_reg(emac_esp32_t *emac)
{
    return xSemaphoreTake(emac->multi_reg_mutex, pdMS_TO_TICKS(EMAC_MULTI_REG_MUTEX_TIMEOUT_MS)) == pdTRUE ? ESP_OK : ESP_ERR_TIMEOUT;
}

static esp_err_t emac_esp32_unlock_multi_reg(emac_esp32_t *emac)
{
    return xSemaphoreGive(emac->multi_reg_mutex) == pdTRUE ? ESP_OK : ESP_FAIL;
}

static esp_err_t emac_esp32_set_mediator(esp_eth_mac_t *mac, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(eth, ESP_ERR_INVALID_ARG, err, TAG, "can't set mac's mediator to null");
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    emac->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_write_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t reg_value)
{
    esp_err_t ret = ESP_OK;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    ESP_GOTO_ON_FALSE(!emac_hal_is_mii_busy(&emac->hal), ESP_ERR_INVALID_STATE, err, TAG, "phy is busy");
    emac_hal_set_phy_data(&emac->hal, reg_value);
    emac_hal_set_phy_cmd(&emac->hal, phy_addr, phy_reg, true);
    /* polling the busy flag */
    uint32_t to = 0;
    bool busy = true;
    do {
        esp_rom_delay_us(100);
        busy = emac_hal_is_mii_busy(&emac->hal);
        to += 100;
    } while (busy && to < PHY_OPERATION_TIMEOUT_US);
    ESP_GOTO_ON_FALSE(!busy, ESP_ERR_TIMEOUT, err, TAG, "phy is busy");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_read_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t *reg_value)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(reg_value, ESP_ERR_INVALID_ARG, err, TAG, "can't set reg_value to null");
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    ESP_GOTO_ON_FALSE(!emac_hal_is_mii_busy(&emac->hal), ESP_ERR_INVALID_STATE, err, TAG, "phy is busy");
    emac_hal_set_phy_cmd(&emac->hal, phy_addr, phy_reg, false);
    /* polling the busy flag */
    uint32_t to = 0;
    bool busy = true;
    do {
        esp_rom_delay_us(100);
        busy = emac_hal_is_mii_busy(&emac->hal);
        to += 100;
    } while (busy && to < PHY_OPERATION_TIMEOUT_US);
    ESP_GOTO_ON_FALSE(!busy, ESP_ERR_TIMEOUT, err, TAG, "phy is busy");
    /* Store value */
    *reg_value = emac_hal_get_phy_data(&emac->hal);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_set_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "can't set mac addr to null");
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    emac_hal_set_address(&emac->hal, addr);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_get_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "can't set mac addr to null");
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    emac_hal_get_address(&emac->hal, addr);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_add_mac_filter(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    ESP_RETURN_ON_ERROR(emac_esp32_lock_multi_reg(emac), TAG, "failed to lock multiple register access");
    ESP_GOTO_ON_ERROR(emac_hal_add_addr_da_filter_auto(&emac->hal, addr), err, TAG, "failed to add MAC filter");
err:
    emac_esp32_unlock_multi_reg(emac);
    return ret;
}

static esp_err_t emac_esp32_rm_mac_filter(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    ESP_RETURN_ON_ERROR(emac_esp32_lock_multi_reg(emac), TAG, "failed to lock multiple register access");
    ESP_GOTO_ON_ERROR(emac_hal_rm_addr_da_filter_auto(&emac->hal, addr), err, TAG, "failed to remove MAC filter");
err:
    emac_esp32_unlock_multi_reg(emac);
    return ret;
}

static esp_err_t emac_esp32_set_link(esp_eth_mac_t *mac, eth_link_t link)
{
    esp_err_t ret = ESP_OK;
    switch (link) {
    case ETH_LINK_UP:
        emac_esp32_start(mac);
        ESP_LOGD(TAG, "emac started");
        break;
    case ETH_LINK_DOWN:
        emac_esp32_stop(mac);
        ESP_LOGD(TAG, "emac stopped");
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "unknown link status");
        break;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_set_speed(esp_eth_mac_t *mac, eth_speed_t speed)
{
    esp_err_t ret = ESP_ERR_INVALID_ARG;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    if (speed >= ETH_SPEED_10M && speed < ETH_SPEED_MAX) {
#ifdef CONFIG_IDF_TARGET_ESP32P4
        // Set RMII clk_rx/clk_tx divider to get 25MHz for 100mbps mode or 2.5MHz for 10mbps mode
        if (emac_hal_get_phy_intf(&emac->hal) == EMAC_DATA_INTERFACE_RMII) {
            if (speed == ETH_SPEED_10M) {
                PERIPH_RCC_ATOMIC() {
                    emac_hal_clock_rmii_rx_tx_div(&emac->hal, RMII_10M_SPEED_RX_TX_CLK_DIV);
                }
            } else {
                PERIPH_RCC_ATOMIC() {
                    emac_hal_clock_rmii_rx_tx_div(&emac->hal, RMII_100M_SPEED_RX_TX_CLK_DIV);
                }
            }
        }
#endif
        emac_hal_set_speed(&emac->hal, speed);
        ESP_LOGD(TAG, "working in %iMbps", speed == ETH_SPEED_10M ? 10 : 100);
        return ESP_OK;
    }
    return ret;
}

static esp_err_t emac_esp32_set_duplex(esp_eth_mac_t *mac, eth_duplex_t duplex)
{
    esp_err_t ret = ESP_ERR_INVALID_ARG;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    if (duplex == ETH_DUPLEX_HALF || duplex == ETH_DUPLEX_FULL) {
        emac_hal_set_duplex(&emac->hal, duplex);
        ESP_LOGD(TAG, "working in %s duplex", duplex == ETH_DUPLEX_HALF ? "half" : "full");
        return ESP_OK;
    }
    return ret;
}

static esp_err_t emac_esp32_set_promiscuous(esp_eth_mac_t *mac, bool enable)
{
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    emac_hal_set_promiscuous(&emac->hal, enable);
    return ESP_OK;
}

static esp_err_t emac_esp32_set_all_multicast(esp_eth_mac_t *mac, bool enable)
{
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    emac_hal_pass_all_multicast_enable(&emac->hal, enable);
    return ESP_OK;
}

static esp_err_t emac_esp32_enable_flow_ctrl(esp_eth_mac_t *mac, bool enable)
{
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    emac->flow_ctrl_enabled = enable;
    return ESP_OK;
}

static esp_err_t emac_esp32_set_peer_pause_ability(esp_eth_mac_t *mac, uint32_t ability)
{
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    // we want to enable flow control, and peer does support pause function
    // then configure the MAC layer to enable flow control feature
    if (emac->flow_ctrl_enabled && ability) {
        emac_hal_enable_flow_ctrl(&emac->hal, true);
        emac->do_flow_ctrl = true;
    } else {
        emac_hal_enable_flow_ctrl(&emac->hal, false);
        emac->do_flow_ctrl = false;
        ESP_LOGD(TAG, "Flow control not enabled for the link");
    }
    return ESP_OK;
}

static void emac_esp_dump_hal_registers(emac_esp32_t *emac)
{
    ESP_LOG_BUFFER_HEXDUMP("DMA", emac->hal.dma_regs, sizeof(emac_dma_dev_t), ESP_LOG_INFO);
    ESP_LOG_BUFFER_HEXDUMP("MAC", emac->hal.mac_regs, sizeof(emac_mac_dev_t), ESP_LOG_INFO);
#ifdef SOC_EMAC_IEEE1588V2_SUPPORTED
    ESP_LOG_BUFFER_HEXDUMP("PTP", emac->hal.ptp_regs, sizeof(emac_ptp_dev_t), ESP_LOG_INFO);
#endif // SOC_EMAC_IEEE1588V2_SUPPORTED
}

esp_err_t emac_esp_custom_ioctl(esp_eth_mac_t *mac, int cmd, void *data)
{
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);

    switch (cmd)
    {
    case ETH_MAC_ESP_CMD_SET_TDES0_CFG_BITS:
        ESP_RETURN_ON_FALSE(data != NULL, ESP_ERR_INVALID_ARG, TAG, "cannot set DMA tx desc flag to null");
        emac_esp_dma_set_tdes0_ctrl_bits(emac->emac_dma_hndl, *(uint32_t *)data);
        break;
    case ETH_MAC_ESP_CMD_CLEAR_TDES0_CFG_BITS:
        ESP_RETURN_ON_FALSE(data != NULL, ESP_ERR_INVALID_ARG, TAG, "cannot clear DMA tx desc flag with null");
        emac_esp_dma_clear_tdes0_ctrl_bits(emac->emac_dma_hndl, *(uint32_t *)data);
        break;
    case ETH_MAC_ESP_CMD_DUMP_REGS:
        emac_esp_dump_hal_registers(emac);
        break;
    default:
        ESP_RETURN_ON_ERROR(ESP_ERR_INVALID_ARG, TAG, "unknown io command: %i", cmd);
    }
    return ESP_OK;
}

static esp_err_t emac_esp32_transmit(esp_eth_mac_t *mac, uint8_t *buf, uint32_t length)
{
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    uint32_t sent_len = emac_esp_dma_transmit_frame(emac->emac_dma_hndl, buf, length);
    if (sent_len != length) {
        ESP_LOGD(TAG, "insufficient TX buffer size");
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

static esp_err_t emac_esp32_transmit_ctrl_vargs(esp_eth_mac_t *mac, void *ctrl, uint32_t argc, va_list args)
{
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    uint32_t buf_num = argc / 2;

    emac_esp_dma_transmit_buff_t buff_array[buf_num];

    uint32_t exp_len = 0;
    for (int i = 0; i < buf_num; i++) {
        buff_array[i].buf = va_arg(args, uint8_t *);
        buff_array[i].size = va_arg(args, uint32_t);
        exp_len += buff_array[i].size;
    }

    eth_mac_time_t *ts = (eth_mac_time_t *)ctrl;
    uint32_t sent_len = emac_esp_dma_transmit_frame_ext(emac->emac_dma_hndl, buff_array, buf_num, ts);

    if(sent_len != exp_len) {
        ESP_LOGD(TAG, "insufficient TX buffer size");
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

static esp_err_t emac_esp32_receive(esp_eth_mac_t *mac, uint8_t *buf, uint32_t *length)
{
    esp_err_t ret = ESP_OK;
    uint32_t expected_len = *length;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    ESP_GOTO_ON_FALSE(buf && length, ESP_ERR_INVALID_ARG, err, TAG, "can't set buf and length to null");
    uint32_t receive_len = emac_esp_dma_receive_frame(emac->emac_dma_hndl, buf, expected_len, NULL);
    emac_esp_dma_get_remain_frames(emac->emac_dma_hndl, &emac->frames_remain, &emac->free_rx_descriptor);
    /* we need to check the return value in case the buffer size is not enough */
    ESP_GOTO_ON_FALSE(expected_len >= receive_len, ESP_ERR_INVALID_SIZE, err, TAG, "received buffer longer than expected");
    *length = receive_len;
    return ESP_OK;
err:
    *length = expected_len;
    return ret;
}

static void emac_esp32_rx_task(void *arg)
{
    emac_esp32_t *emac = (emac_esp32_t *)arg;
    uint8_t *buffer = NULL;
    while (1) {
        // block indefinitely until got notification from underlay event
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        do {
            /* set max expected frame len */
            uint32_t frame_len = ETH_MAX_PACKET_SIZE;
            buffer = emac_esp_dma_alloc_recv_buf(emac->emac_dma_hndl, &frame_len);
            /* we have memory to receive the frame of maximal size previously defined */
            if (buffer != NULL) {
#ifdef SOC_EMAC_IEEE1588V2_SUPPORTED
                eth_mac_time_t ts;
                eth_mac_time_t *p_ts = &ts;
#else
                eth_mac_time_t *p_ts = NULL;
#endif
                uint32_t recv_len = emac_esp_dma_receive_frame(emac->emac_dma_hndl, buffer, EMAC_DMA_BUF_SIZE_AUTO, p_ts);
                if (recv_len == 0) {
                    ESP_LOGE(TAG, "frame copy error");
                    free(buffer);
                    /* ensures that interface to EMAC does not get stuck with unprocessed frames */
                    emac_esp_dma_flush_recv_frame(emac->emac_dma_hndl);
                } else if (frame_len > recv_len) {
                    ESP_LOGE(TAG, "received frame was truncated");
                    free(buffer);
                } else {
                    ESP_LOGD(TAG, "receive len= %" PRIu32, recv_len);
                    emac->eth->stack_input_info(emac->eth, buffer, recv_len, (void *)p_ts);
                }
            /* if allocation failed and there is a waiting frame */
            } else if (frame_len) {
                ESP_LOGE(TAG, "no mem for receive buffer");
                /* ensures that interface to EMAC does not get stuck with unprocessed frames */
                emac_esp_dma_flush_recv_frame(emac->emac_dma_hndl);
            }
            emac_esp_dma_get_remain_frames(emac->emac_dma_hndl, &emac->frames_remain, &emac->free_rx_descriptor);
#if CONFIG_ETH_SOFT_FLOW_CONTROL
            // we need to do extra checking of remained frames in case there are no unhandled frames left, but pause frame is still undergoing
            if ((emac->free_rx_descriptor < emac->flow_control_low_water_mark) && emac->do_flow_ctrl && emac->frames_remain) {
                emac_hal_send_pause_frame(&emac->hal, true);
            } else if ((emac->free_rx_descriptor > emac->flow_control_high_water_mark) || !emac->frames_remain) {
                emac_hal_send_pause_frame(&emac->hal, false);
            }
#endif
        } while (emac->frames_remain);
    }
}

static esp_err_t emac_config_pll_clock(emac_esp32_t *emac)
{
    uint32_t expt_freq = RMII_CLK_HZ; // 50 MHz
    uint32_t real_freq = 0;

#if CONFIG_IDF_TARGET_ESP32
    // the RMII reference comes from the APLL
    periph_rtc_apll_acquire();
    emac->use_pll = true;
    esp_err_t ret = periph_rtc_apll_freq_set(expt_freq, &real_freq);
    ESP_RETURN_ON_FALSE(ret != ESP_ERR_INVALID_ARG, ESP_FAIL, TAG, "Set APLL clock coefficients failed");
    if (ret == ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "APLL is occupied already, it is working at %" PRIu32 " Hz", real_freq);
    }
#elif CONFIG_IDF_TARGET_ESP32P4
    // the RMII reference comes from the MPLL
    periph_rtc_mpll_acquire();
    emac->use_pll = true;
    esp_err_t ret = periph_rtc_mpll_freq_set(expt_freq * 2, &real_freq); // cannot set 50MHz at MPLL, the nearest possible freq is 100 MHz
    if (ret == ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "MPLL is occupied already, it is working at %" PRIu32 " Hz", real_freq);
    }
    // Set divider of MPLL clock
    if (real_freq > RMII_CLK_HZ) {
        uint32_t div = real_freq / RMII_CLK_HZ;
        clk_ll_pll_f50m_set_divider(div);
        // compute real RMII CLK frequency
        real_freq /= div;
    }
    // Enable 50MHz MPLL derived clock
    ESP_RETURN_ON_ERROR(esp_clk_tree_enable_src(SOC_MOD_CLK_PLL_F50M, true), TAG, "clock source enable failed");
#endif
    // If the difference of real RMII CLK frequency is not within 50 ppm, i.e. 2500 Hz, the (A/M)PLL is unusable
    ESP_RETURN_ON_FALSE(abs((int)real_freq - (int)expt_freq) <= 2500,
                        ESP_ERR_INVALID_STATE, TAG, "The (A/M)PLL is working at an unusable frequency %" PRIu32 " Hz", real_freq);
    return ESP_OK;
}

static esp_err_t emac_esp32_init(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    esp_eth_mediator_t *eth = emac->eth;

    ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LLINIT, NULL), err, TAG, "lowlevel init failed");
    /* software reset */
    emac_hal_reset(&emac->hal);
    uint32_t to = 0;
    for (to = 0; to < emac->sw_reset_timeout_ms / 10; to++) {
        if (emac_hal_is_reset_done(&emac->hal)) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    ESP_GOTO_ON_FALSE(to < emac->sw_reset_timeout_ms / 10, ESP_ERR_TIMEOUT, err, TAG, "reset timeout");
    /* set smi clock */
    uint32_t csr_freq_hz;
    soc_module_clk_t csr_clk_src = emac_ll_get_csr_clk_src();
    ESP_GOTO_ON_ERROR(esp_clk_tree_src_get_freq_hz(csr_clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &csr_freq_hz), err, TAG, "get CSR frequency failed");
    if (emac->mdc_freq_hz <= 0) {
        emac_hal_set_csr_clock_range(&emac->hal, csr_freq_hz);
    } else {
        emac_hal_find_set_closest_csr_clock_range(&emac->hal, emac->mdc_freq_hz, csr_freq_hz);
    }
    /* init mac registers by default */
    emac_hal_init_mac_default(&emac->hal);
    /* init dma registers with selected EMAC-DMA configuration */
    emac_hal_dma_config_t dma_config = { .dma_burst_len = emac->dma_burst_len };
    emac_hal_init_dma_default(&emac->hal, &dma_config);
    /* get emac address from efuse */
    uint8_t addr[ETH_ADDR_LEN];
    ESP_GOTO_ON_ERROR(esp_read_mac(addr, ESP_MAC_ETH), err, TAG, "fetch ethernet mac address failed");
    /* set MAC address to emac register */
    emac_hal_set_address(&emac->hal, addr);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(emac->pm_lock);
#endif
#if EMAC_USE_RETENTION_LINK
    emac_create_retention_module(emac);
#endif // EMAC_USE_RETENTION_LINK
    return ESP_OK;

err:
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ret;
}

static esp_err_t emac_esp32_deinit(esp_eth_mac_t *mac)
{
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    esp_eth_mediator_t *eth = emac->eth;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(emac->pm_lock);
#endif
#if EMAC_USE_RETENTION_LINK
    emac_free_retention_module(emac);
#endif // EMAC_USE_RETENTION_LINK
    emac_hal_stop(&emac->hal);
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ESP_OK;
}

static esp_err_t emac_esp32_start(esp_eth_mac_t *mac)
{
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    /* reset descriptor chain */
    emac_esp_dma_reset(emac->emac_dma_hndl);
#if EMAC_USE_RETENTION_LINK
    emac_esp_enable_emac_start_on_wakeup(emac);
#endif // EMAC_USE_RETENTION_LINK
    emac_hal_start(&emac->hal);
    return ESP_OK;
}

static esp_err_t emac_esp32_stop(esp_eth_mac_t *mac)
{
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    esp_err_t ret = ESP_OK;
    int32_t to = 0;
#if EMAC_USE_RETENTION_LINK
    emac_esp_disable_emac_start_on_wakeup(emac);
#endif // EMAC_USE_RETENTION_LINK
    do {
        if ((ret = emac_hal_stop(&emac->hal)) == ESP_OK) {
            break;
        }
        to += 25;
        esp_rom_delay_us(25);
    } while (to < MAC_STOP_TIMEOUT_US);
    return ret;
}

static esp_err_t emac_esp32_del(esp_eth_mac_t *mac)
{
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    emac_esp_free_driver_obj(emac);
    emac_esp_gpio_deinit_all();
    // disable bus clock
    PERIPH_RCC_ATOMIC() {
        emac_ll_enable_bus_clock(0, false);
    }
    return ESP_OK;
}

// To achieve a better performance, we put the ISR always in IRAM
IRAM_ATTR void emac_isr_default_handler(void *args)
{
    emac_hal_context_t *hal = (emac_hal_context_t *)args;
    uint32_t intr_stat = emac_hal_get_intr_status(hal);
    emac_hal_clear_corresponding_intr(hal, intr_stat);
    emac_esp32_t *emac = __containerof(hal, emac_esp32_t, hal);
    bool high_task_woken = false;

#if SOC_EMAC_IEEE1588V2_SUPPORTED && EMAC_LL_CONFIG_ENABLE_MAC_INTR_MASK & EMAC_LL_MAC_INTR_TIME_STAMP_ENABLE
    if (intr_stat & EMAC_LL_DMA_TIMESTAMP_TRIGGER_INTR) {
        uint32_t ts_stat = emac_hal_get_ts_status(hal);
        if (ts_stat & EMAC_LL_TS_TARGET_TIME_REACHED) {
            if (emac->ts_target_exceed_cb_from_isr) {
                bool ts_high_task_woken = emac->ts_target_exceed_cb_from_isr(emac->eth, NULL);
                high_task_woken |= ts_high_task_woken;
            }
        }
    }
#endif // SOC_EMAC_IEEE1588V2_SUPPORTED

#if EMAC_LL_CONFIG_ENABLE_INTR_MASK & EMAC_LL_INTR_RECEIVE_ENABLE
    if (intr_stat & EMAC_LL_DMA_RECEIVE_FINISH_INTR) {
        BaseType_t rx_high_task_woken = pdFALSE;
        /* notify receive task */
        vTaskNotifyGiveFromISR(emac->rx_task_hdl, &rx_high_task_woken);
        high_task_woken |= (bool)rx_high_task_woken;
    }
#endif // EMAC_LL_CONFIG_ENABLE_INTR_MASK & EMAC_LL_INTR_RECEIVE_ENABLE

    if (high_task_woken) {
        portYIELD_FROM_ISR();
    }
}

static void emac_esp_free_driver_obj(emac_esp32_t *emac)
{
    if (emac) {
        if (emac->rx_task_hdl) {
            vTaskDelete(emac->rx_task_hdl);
        }
        if (emac->intr_hdl) {
            esp_intr_free(emac->intr_hdl);
        }

        if (emac->use_pll) {
#if CONFIG_IDF_TARGET_ESP32
            periph_rtc_apll_release();
#elif CONFIG_IDF_TARGET_ESP32P4
            periph_rtc_mpll_release();
#endif
        }
#ifdef CONFIG_IDF_TARGET_ESP32
        if (emac->rmii_clk_hdl) {
            esp_clock_output_stop(emac->rmii_clk_hdl);
        }
#endif // CONFIG_IDF_TARGET_ESP32

        if (emac->multi_reg_mutex) {
            vSemaphoreDelete(emac->multi_reg_mutex);
        }

#ifdef CONFIG_PM_ENABLE
        if (emac->pm_lock) {
            esp_pm_lock_delete(emac->pm_lock);
        }
#endif // CONFIG_PM_ENABLE
#if EMAC_USE_RETENTION_LINK
    sleep_retention_module_t module = emac_reg_retention_info.module_id;
    if (sleep_retention_is_module_inited(module)) {
        sleep_retention_module_deinit(module);
    }
#endif // EMAC_USE_RETENTION_LINK

        emac_esp_del_dma(emac->emac_dma_hndl);

        free(emac);
    }
}

static esp_err_t emac_esp_alloc_driver_obj(const eth_mac_config_t *config, emac_esp32_t **emac_out_hdl)
{
    esp_err_t ret = ESP_OK;
    emac_esp32_t *emac = NULL;
    if (config->flags & ETH_MAC_FLAG_WORK_WITH_CACHE_DISABLE) {
        emac = heap_caps_calloc(1, sizeof(emac_esp32_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    } else {
        emac = calloc(1, sizeof(emac_esp32_t));
    }
    ESP_GOTO_ON_FALSE(emac, ESP_ERR_NO_MEM, err, TAG, "no mem for esp emac object");

    /* alloc PM lock */
#ifdef CONFIG_PM_ENABLE
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "emac_esp32", &emac->pm_lock), err, TAG, "create pm lock failed");
#endif

#if EMAC_USE_RETENTION_LINK
    sleep_retention_module_t module = emac_reg_retention_info.module_id;
    sleep_retention_module_init_param_t init_param = {
        .cbs = {
            .create = {
                .handle = emac_create_sleep_retention_link_cb,
                .arg = (void *)emac
            },
        },
        .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_SYSTEM)
    };
    if (sleep_retention_module_init(module, &init_param) != ESP_OK) {
        // even though the sleep retention module init failed, EMAC driver should still work, so just warning here
        ESP_LOGW(TAG, "init sleep retention failed on EMAC, power domain may be turned off during sleep");
    }
#endif // EMAC_USE_RETENTION_LINK

    ESP_GOTO_ON_ERROR(emac_esp_new_dma(NULL, &emac->emac_dma_hndl), err, TAG, "create EMAC DMA object failed");

    emac->multi_reg_mutex = xSemaphoreCreateMutex();
    ESP_GOTO_ON_FALSE(emac->multi_reg_mutex, ESP_ERR_NO_MEM, err, TAG, "failed to create multiple register access mutex");

    /* create rx task */
    BaseType_t core_num = tskNO_AFFINITY;
    if (config->flags & ETH_MAC_FLAG_PIN_TO_CORE) {
        core_num = esp_cpu_get_core_id();
    }
    BaseType_t xReturned = xTaskCreatePinnedToCore(emac_esp32_rx_task, "emac_rx", config->rx_task_stack_size, emac,
                                                   config->rx_task_prio, &emac->rx_task_hdl, core_num);
    ESP_GOTO_ON_FALSE(xReturned == pdPASS, ESP_FAIL, err, TAG, "create emac_rx task failed");

err:
    *emac_out_hdl = emac;
    return ret;
}

static esp_err_t emac_esp_config_data_interface(const eth_esp32_emac_config_t *esp32_emac_config, emac_esp32_t *emac)
{
    esp_err_t ret = ESP_OK;
    switch (esp32_emac_config->interface) {
    case EMAC_DATA_INTERFACE_MII: {
        /* MII interface GPIO initialization */
#if SOC_EMAC_MII_USE_GPIO_MATRIX
        ESP_GOTO_ON_ERROR(emac_esp_gpio_matrix_init_mii(&esp32_emac_config->emac_dataif_gpio.mii), err, TAG, "failed to initialize EMAC MII GPIO Matrix");
#else
        eth_mac_mii_gpio_config_t *mii_data_gpio = NULL;
#if SOC_EMAC_USE_MULTI_IO_MUX
        mii_data_gpio = &esp32_emac_config->emac_dataif_gpio.mii;
#endif // SOC_EMAC_USE_MULTI_IO_MUX
        ESP_GOTO_ON_ERROR(emac_esp_iomux_init_mii(mii_data_gpio), err, TAG, "invalid EMAC MII data plane GPIO");
#endif // SOC_EMAC_MII_USE_GPIO_MATRIX
        /* Enable MII clock */
        PERIPH_RCC_ATOMIC() {
            emac_hal_clock_enable_mii(&emac->hal);
        }
        break;
    }
    case EMAC_DATA_INTERFACE_RMII: {
        /* RMII interface GPIO initialization */
        const eth_mac_rmii_gpio_config_t *rmii_data_gpio = NULL;
#if SOC_EMAC_USE_MULTI_IO_MUX
        rmii_data_gpio = &esp32_emac_config->emac_dataif_gpio.rmii;
#endif // SOC_EMAC_USE_MULTI_IO_MUX
        ESP_GOTO_ON_ERROR(emac_esp_iomux_init_rmii(rmii_data_gpio), err, TAG, "invalid EMAC RMII data plane GPIO");
        /* If ref_clk is configured as input */
        if (esp32_emac_config->clock_config.rmii.clock_mode == EMAC_CLK_EXT_IN) {
            ESP_GOTO_ON_ERROR(emac_esp_iomux_rmii_clk_input(esp32_emac_config->clock_config.rmii.clock_gpio), err, TAG, "invalid EMAC RMII clock input GPIO");
            PERIPH_RCC_ATOMIC() {
                emac_hal_clock_enable_rmii_input(&emac->hal);
            }
        } else if (esp32_emac_config->clock_config.rmii.clock_mode == EMAC_CLK_OUT) {
            ESP_GOTO_ON_ERROR(emac_config_pll_clock(emac), err, TAG, "Configure (A/M)PLL for RMII failed");
#if CONFIG_IDF_TARGET_ESP32P4
            /* Output RMII clock is routed back to input externally */
            ESP_GOTO_ON_FALSE(esp32_emac_config->clock_config_out_in.rmii.clock_mode == EMAC_CLK_EXT_IN && esp32_emac_config->clock_config_out_in.rmii.clock_gpio >= 0,
                              ESP_ERR_INVALID_ARG, err, TAG, "invalid EMAC input of output clock mode");
            ESP_GOTO_ON_ERROR(emac_esp_iomux_rmii_clk_input(esp32_emac_config->clock_config_out_in.rmii.clock_gpio), err, TAG, "invalid EMAC RMII clock input GPIO");
            PERIPH_RCC_ATOMIC() {
                emac_hal_clock_enable_rmii_input(&emac->hal);
            }
#elif CONFIG_IDF_TARGET_ESP32
            // we can also use the IOMUX to route the APLL clock to GPIO_0
            if (esp32_emac_config->clock_config.rmii.clock_gpio == 0) {
                ESP_GOTO_ON_ERROR(esp_clock_output_start(CLKOUT_SIG_APLL, 0, &emac->rmii_clk_hdl),
                                  err, TAG, "start APLL clock output failed");
            } else
#endif
            {
                ESP_GOTO_ON_ERROR(emac_esp_iomux_rmii_clk_ouput(esp32_emac_config->clock_config.rmii.clock_gpio), err, TAG, "invalid EMAC RMII clock output GPIO");
            }
            /* Enable RMII Output clock */
            PERIPH_RCC_ATOMIC() {
                emac_hal_clock_enable_rmii_output(&emac->hal);
            }
        } else {
            ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "invalid EMAC clock mode");
        }
        break;
    }
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "invalid EMAC Data Interface:%i", esp32_emac_config->interface);
    }
err:
    return ret;
}

#ifdef SOC_EMAC_IEEE1588V2_SUPPORTED
esp_err_t esp_eth_mac_ptp_enable(esp_eth_mac_t *mac, const eth_mac_ptp_config_t *config)
{
    ESP_RETURN_ON_FALSE(mac && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument, can't be NULL");
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    PERIPH_RCC_ATOMIC() {
        emac_hal_clock_enable_ptp(&emac->hal, config->clk_src, true);
    }
    emac_hal_ptp_config_t ptp_config = {
        .upd_method = ETH_PTP_UPDATE_METHOD_FINE,
        .roll = config->roll_type,
        .ptp_clk_src_period_ns = config->clk_src_period_ns,
        .ptp_req_accuracy_ns = config->required_accuracy_ns
    };
    ESP_RETURN_ON_ERROR(emac_hal_ptp_start(&emac->hal, &ptp_config), TAG, "failed to start PTP module");
    emac_esp_dma_ts_enable(emac->emac_dma_hndl, true);
    return ESP_OK;
}

esp_err_t esp_eth_mac_ptp_disable(esp_eth_mac_t *mac)
{
    ESP_RETURN_ON_FALSE(mac, ESP_ERR_INVALID_ARG, TAG, "invalid argument, can't be NULL");
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    ESP_RETURN_ON_ERROR(emac_hal_ptp_stop(&emac->hal), TAG, "failed to stop PTP module");
    emac_esp_dma_ts_enable(emac->emac_dma_hndl, false);
    PERIPH_RCC_ATOMIC() {
        emac_hal_clock_enable_ptp(&emac->hal, 0, false);
    }
    return ESP_OK;
}

esp_err_t esp_eth_mac_set_ptp_time(esp_eth_mac_t *mac, const eth_mac_time_t *time)
{
    ESP_RETURN_ON_FALSE(mac && time, ESP_ERR_INVALID_ARG, TAG, "invalid argument, can't be NULL");
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    ESP_RETURN_ON_ERROR(emac_hal_ptp_set_sys_time(&emac->hal, time->seconds, time->nanoseconds), TAG, "failed to set PTP time");
    return ESP_OK;
}

esp_err_t esp_eth_mac_get_ptp_time(esp_eth_mac_t *mac, eth_mac_time_t *time)
{
    ESP_RETURN_ON_FALSE(mac && time, ESP_ERR_INVALID_ARG, TAG, "invalid argument, can't be NULL");
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    ESP_RETURN_ON_ERROR(emac_hal_ptp_get_sys_time(&emac->hal, &time->seconds, &time->nanoseconds), TAG, "failed to get PTP time");
    return ESP_OK;
}

esp_err_t esp_eth_mac_adj_ptp_freq(esp_eth_mac_t *mac, double scale_factor)
{
    ESP_RETURN_ON_FALSE(mac, ESP_ERR_INVALID_ARG, TAG, "invalid argument, can't be NULL");
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    ESP_RETURN_ON_ERROR(emac_hal_adj_freq_factor(&emac->hal, scale_factor), TAG, "failed to adjust PTP time base by scale factor");
    return ESP_OK;
}

esp_err_t esp_eth_mac_adj_ptp_time(esp_eth_mac_t *mac, int32_t adj_ppb)
{
    ESP_RETURN_ON_FALSE(mac, ESP_ERR_INVALID_ARG, TAG, "invalid argument, can't be NULL");
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    ESP_RETURN_ON_ERROR(emac_hal_ptp_adj_inc(&emac->hal, adj_ppb), TAG, "failed to adjust PTP time base");
    return ESP_OK;
}

esp_err_t esp_eth_mac_set_target_time(esp_eth_mac_t *mac, const eth_mac_time_t *target)
{
    ESP_RETURN_ON_FALSE(mac && target, ESP_ERR_INVALID_ARG, TAG, "invalid argument, can't be NULL");
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    ESP_RETURN_ON_ERROR(emac_hal_ptp_set_target_time(&emac->hal, target->seconds, target->nanoseconds), TAG,
                        "failed to set PTP target time");
    return ESP_OK;
}

esp_err_t esp_eth_mac_set_target_time_cb(esp_eth_mac_t *mac, ts_target_exceed_cb_from_isr_t cb)
{
    ESP_RETURN_ON_FALSE(mac, ESP_ERR_INVALID_ARG, TAG, "invalid argument, can't be NULL");
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    emac->ts_target_exceed_cb_from_isr = cb;
    return ESP_OK;
}

esp_err_t esp_eth_mac_enable_ts4all(esp_eth_mac_t *mac, bool enable)
{
    ESP_RETURN_ON_FALSE(mac, ESP_ERR_INVALID_ARG, TAG, "invalid argument, can't be NULL");
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    ESP_RETURN_ON_ERROR(emac_hal_ptp_enable_ts4all(&emac->hal, enable), TAG, "failed to enable timestamping for all frames");
    return ESP_OK;
}

esp_err_t esp_eth_mac_set_pps_out_gpio(esp_eth_mac_t *mac, int gpio_num)
{
    // The mac argument is unused in implementation but kept for API consistency
    ESP_RETURN_ON_FALSE(mac, ESP_ERR_INVALID_ARG, TAG, "invalid argument, can't be NULL");
    ESP_RETURN_ON_ERROR(emac_esp_gpio_matrix_init_ptp_pps(gpio_num), TAG, "failed to set PPS0 output at GPIO %i", gpio_num);
    return ESP_OK;
}

esp_err_t esp_eth_mac_set_pps_out_freq(esp_eth_mac_t *mac, uint32_t freq_hz)
{
    ESP_RETURN_ON_FALSE(mac, ESP_ERR_INVALID_ARG, TAG, "invalid argument, can't be NULL");
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    ESP_RETURN_ON_ERROR(emac_hal_set_pps0_out_freq(&emac->hal, freq_hz), TAG, "failed to set PPS0 output frequency");
    return ESP_OK;
}
#endif // SOC_EMAC_IEEE1588V2_SUPPORTED

esp_eth_mac_t *esp_eth_mac_new_esp32(const eth_esp32_emac_config_t *esp32_config, const eth_mac_config_t *config)
{
    esp_err_t ret_code = ESP_OK;
    esp_eth_mac_t *ret = NULL;
    emac_esp32_t *emac = NULL;
    ESP_RETURN_ON_FALSE(config, NULL, TAG, "can't set mac config to null");
    if (esp32_config->intr_priority > 0) {
        ESP_RETURN_ON_FALSE(1 << (esp32_config->intr_priority) & EMAC_ALLOW_INTR_PRIORITY_MASK, NULL,
                            TAG, "invalid interrupt priority: %d", esp32_config->intr_priority);
    }

    ret_code = emac_esp_alloc_driver_obj(config, &emac);
    ESP_GOTO_ON_FALSE(ret_code == ESP_OK, NULL, err, TAG, "alloc driver object failed");

    // enable bus clock for the EMAC module, and reset the registers into default state
    // this must be called before HAL layer initialization
    PERIPH_RCC_ATOMIC() {
        emac_ll_enable_bus_clock(0, true);
        emac_ll_reset_register(0);
    }
    /* initialize hal layer driver */
    emac_hal_init(&emac->hal);

    /* alloc interrupt */
    int isr_flags = 0;
    if (esp32_config->intr_priority > 0) {
        isr_flags |= 1 << (esp32_config->intr_priority);
    } else {
        isr_flags |= ESP_INTR_FLAG_LOWMED;
    }
    if (config->flags & ETH_MAC_FLAG_WORK_WITH_CACHE_DISABLE) {
        isr_flags |= ESP_INTR_FLAG_IRAM;
    }
    ret_code = esp_intr_alloc(ETS_ETH_MAC_INTR_SOURCE, isr_flags,
                              emac_isr_default_handler, &emac->hal, &(emac->intr_hdl));
    ESP_GOTO_ON_FALSE(ret_code == ESP_OK, NULL, err, TAG, "alloc emac interrupt failed");
    ret_code = esp_intr_enable(emac->intr_hdl);
    ESP_GOTO_ON_FALSE(ret_code == ESP_OK, NULL, err, TAG, "enable interrupt failed");

    /* init GPIO used by SMI interface */
    ret_code = emac_esp_gpio_init_smi(&esp32_config->smi_gpio);
    ESP_GOTO_ON_FALSE(ret_code == ESP_OK, NULL, err, TAG, "SMI GPIO init failed");
    /* init GPIO and CLK for data interface */
    ret_code = emac_esp_config_data_interface(esp32_config, emac);
    ESP_GOTO_ON_FALSE(ret_code == ESP_OK, NULL, err, TAG, "config emac interface failed");

    emac->dma_burst_len = esp32_config->dma_burst_len;
    emac->sw_reset_timeout_ms = config->sw_reset_timeout_ms;

    emac->mdc_freq_hz = esp32_config->mdc_freq_hz;

    emac->flow_control_high_water_mark = FLOW_CONTROL_HIGH_WATER_MARK;
    emac->flow_control_low_water_mark = FLOW_CONTROL_LOW_WATER_MARK;
    emac->parent.set_mediator = emac_esp32_set_mediator;
    emac->parent.init = emac_esp32_init;
    emac->parent.deinit = emac_esp32_deinit;
    emac->parent.start = emac_esp32_start;
    emac->parent.stop = emac_esp32_stop;
    emac->parent.del = emac_esp32_del;
    emac->parent.write_phy_reg = emac_esp32_write_phy_reg;
    emac->parent.read_phy_reg = emac_esp32_read_phy_reg;
    emac->parent.set_addr = emac_esp32_set_addr;
    emac->parent.get_addr = emac_esp32_get_addr;
    emac->parent.add_mac_filter = emac_esp32_add_mac_filter;
    emac->parent.rm_mac_filter = emac_esp32_rm_mac_filter;
    emac->parent.set_speed = emac_esp32_set_speed;
    emac->parent.set_duplex = emac_esp32_set_duplex;
    emac->parent.set_link = emac_esp32_set_link;
    emac->parent.set_promiscuous = emac_esp32_set_promiscuous;
    emac->parent.set_all_multicast = emac_esp32_set_all_multicast;
    emac->parent.set_peer_pause_ability = emac_esp32_set_peer_pause_ability;
    emac->parent.enable_flow_ctrl = emac_esp32_enable_flow_ctrl;
    emac->parent.transmit = emac_esp32_transmit;
    emac->parent.transmit_ctrl_vargs = emac_esp32_transmit_ctrl_vargs;
    emac->parent.receive = emac_esp32_receive;
    emac->parent.custom_ioctl = emac_esp_custom_ioctl;
#ifdef SOC_EMAC_IEEE1588V2_SUPPORTED
    emac->ts_target_exceed_cb_from_isr = NULL;
#endif // SOC_EMAC_IEEE1588V2_SUPPORTED
    return &(emac->parent);

err:
    emac_esp_free_driver_obj(emac);
    emac_esp_gpio_deinit_all();
    return ret;
}
