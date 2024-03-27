/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include <stdarg.h>
#include <inttypes.h>
#include "esp_private/periph_ctrl.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_eth_driver.h"
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "hal/emac_hal.h"
#include "soc/soc.h"
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

#if CONFIG_IDF_TARGET_ESP32P4
// ESP32P4 EMAC interface clock configuration is shared among other modules in registers
#define EMAC_IF_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define EMAC_IF_RCC_ATOMIC()
#endif

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
    emac_esp_smi_gpio_config_t smi_gpio;
    eth_mac_clock_config_t clock_config;
    uint8_t addr[ETH_ADDR_LEN];
    bool isr_need_yield;
    bool flow_ctrl_enabled; // indicates whether the user want to do flow control
    bool do_flow_ctrl;  // indicates whether we need to do software flow control
    bool use_pll;  // Only use (A/M)PLL in EMAC_DATA_INTERFACE_RMII && EMAC_CLK_OUT
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif
    eth_mac_dma_burst_len_t dma_burst_len;

    // ---- Chip specifics ----
#ifdef CONFIG_IDF_TARGET_ESP32
    esp_clock_output_mapping_handle_t rmii_clk_hdl; // we use the esp_clock_output driver to output a pre-configured APLL clock as the RMII reference clock
#endif

#ifdef CONFIG_IDF_TARGET_ESP32P4
    eth_mac_clock_config_t clock_config_out_in;
    union
    {
        emac_esp_mii_gpio_config_t mii_gpio;
        emac_esp_rmii_gpio_config_t rmii_gpio;
    };
#endif // CONFIG_IDF_TARGET_ESP32P4
} emac_esp32_t;

static esp_err_t esp_emac_alloc_driver_obj(const eth_mac_config_t *config, emac_esp32_t **emac_out_hdl);
static void esp_emac_free_driver_obj(emac_esp32_t *emac);
static esp_err_t emac_esp32_start(esp_eth_mac_t *mac);
static esp_err_t emac_esp32_stop(esp_eth_mac_t *mac);

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
    memcpy(emac->addr, addr, 6);
    emac_hal_set_address(&emac->hal, emac->addr);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_get_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "can't set mac addr to null");
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    memcpy(addr, emac->addr, 6);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_set_link(esp_eth_mac_t *mac, eth_link_t link)
{
    esp_err_t ret = ESP_OK;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    switch (link) {
    case ETH_LINK_UP:
        ESP_GOTO_ON_ERROR(esp_intr_enable(emac->intr_hdl), err, TAG, "enable interrupt failed");
        emac_esp32_start(mac);
        ESP_LOGD(TAG, "emac started");
        break;
    case ETH_LINK_DOWN:
        ESP_GOTO_ON_ERROR(esp_intr_disable(emac->intr_hdl), err, TAG, "disable interrupt failed");
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
                EMAC_IF_RCC_ATOMIC () {
                    emac_hal_clock_rmii_rx_tx_div(&emac->hal, RMII_10M_SPEED_RX_TX_CLK_DIV);
                }
            } else {
                EMAC_IF_RCC_ATOMIC () {
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

static esp_err_t emac_esp32_transmit(esp_eth_mac_t *mac, uint8_t *buf, uint32_t length)
{
    esp_err_t ret = ESP_OK;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    uint32_t sent_len = emac_esp_dma_transmit_frame(emac->emac_dma_hndl, buf, length);
    ESP_GOTO_ON_FALSE(sent_len == length, ESP_ERR_NO_MEM, err, TAG, "insufficient TX buffer size");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_transmit_multiple_bufs(esp_eth_mac_t *mac, uint32_t argc, va_list args)
{
    esp_err_t ret = ESP_OK;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    uint8_t *bufs[argc];
    uint32_t len[argc];
    uint32_t exp_len = 0;
    for (int i = 0; i < argc; i++) {
        bufs[i] = va_arg(args, uint8_t *);
        len[i] = va_arg(args, uint32_t);
        exp_len += len[i];
    }
    uint32_t sent_len = emac_esp_dma_transmit_multiple_buf_frame(emac->emac_dma_hndl, bufs, len, argc);
    ESP_GOTO_ON_FALSE(sent_len == exp_len, ESP_ERR_INVALID_SIZE, err, TAG, "insufficient TX buffer size");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_receive(esp_eth_mac_t *mac, uint8_t *buf, uint32_t *length)
{
    esp_err_t ret = ESP_OK;
    uint32_t expected_len = *length;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    ESP_GOTO_ON_FALSE(buf && length, ESP_ERR_INVALID_ARG, err, TAG, "can't set buf and length to null");
    uint32_t receive_len = emac_esp_dma_receive_frame(emac->emac_dma_hndl, buf, expected_len, &emac->frames_remain, &emac->free_rx_descriptor);
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
                uint32_t recv_len = emac_esp_dma_receive_frame(emac->emac_dma_hndl, buffer, EMAC_HAL_BUF_SIZE_AUTO, &emac->frames_remain, &emac->free_rx_descriptor);
                if (recv_len == 0) {
                    ESP_LOGE(TAG, "frame copy error");
                    free(buffer);
                    /* ensure that interface to EMAC does not get stuck with unprocessed frames */
                    emac_esp_dma_flush_recv_frame(emac->emac_dma_hndl, &emac->frames_remain, &emac->free_rx_descriptor);
                } else if (frame_len > recv_len) {
                    ESP_LOGE(TAG, "received frame was truncated");
                    free(buffer);
                } else {
                    ESP_LOGD(TAG, "receive len= %" PRIu32, recv_len);
                    emac->eth->stack_input(emac->eth, buffer, recv_len);
                }
                /* if allocation failed and there is a waiting frame */
            } else if (frame_len) {
                ESP_LOGE(TAG, "no mem for receive buffer");
                /* ensure that interface to EMAC does not get stuck with unprocessed frames */
                emac_esp_dma_flush_recv_frame(emac->emac_dma_hndl, &emac->frames_remain, &emac->free_rx_descriptor);
            }
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
    vTaskDelete(NULL);
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
        int32_t div = real_freq / RMII_CLK_HZ - 1;
        clk_ll_pll_f50m_set_divider(div);
        // compute real RMII CLK frequency
        real_freq /= div + 1;
    }
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

    /* init gpio used by smi interface */
    emac_esp32_gpio_init_smi(&emac->smi_gpio);
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
    emac_hal_set_csr_clock_range(&emac->hal, esp_clk_apb_freq());
    /* init mac registers by default */
    emac_hal_init_mac_default(&emac->hal);
    /* init dma registers with selected EMAC-DMA configuration */
    emac_hal_dma_config_t dma_config = { .dma_burst_len = emac->dma_burst_len };
    emac_hal_init_dma_default(&emac->hal, &dma_config);
    /* get emac address from efuse */
    ESP_GOTO_ON_ERROR(esp_read_mac(emac->addr, ESP_MAC_ETH), err, TAG, "fetch ethernet mac address failed");
    /* set MAC address to emac register */
    emac_hal_set_address(&emac->hal, emac->addr);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(emac->pm_lock);
#endif
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
    emac_hal_stop(&emac->hal);
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ESP_OK;
}

static esp_err_t emac_esp32_start(esp_eth_mac_t *mac)
{
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    /* reset descriptor chain */
    emac_esp_dma_reset_desc_chain(emac->emac_dma_hndl);
    emac_hal_start(&emac->hal);
    return ESP_OK;
}

static esp_err_t emac_esp32_stop(esp_eth_mac_t *mac)
{
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    esp_err_t ret = ESP_OK;
    int32_t to = 0;
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
    esp_emac_free_driver_obj(emac);
    /// disable bus clock
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

#if EMAC_LL_CONFIG_ENABLE_INTR_MASK & EMAC_LL_INTR_RECEIVE_ENABLE
    if (intr_stat & EMAC_LL_DMA_RECEIVE_FINISH_INTR) {
        emac_esp32_t *emac = __containerof(hal, emac_esp32_t, hal);
        BaseType_t high_task_wakeup = pdFALSE;
        /* notify receive task */
        vTaskNotifyGiveFromISR(emac->rx_task_hdl, &high_task_wakeup);
        if (high_task_wakeup == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    }
#endif
}

static void esp_emac_free_driver_obj(emac_esp32_t *emac)
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

#ifdef CONFIG_PM_ENABLE
        if (emac->pm_lock) {
            esp_pm_lock_delete(emac->pm_lock);
        }
#endif // CONFIG_PM_ENABLE

        emac_esp_del_dma(emac->emac_dma_hndl);

        free(emac);
    }
}

static esp_err_t esp_emac_alloc_driver_obj(const eth_mac_config_t *config, emac_esp32_t **emac_out_hdl)
{
    esp_err_t ret = ESP_OK;
    emac_esp32_t *emac = NULL;
    if (config->flags & ETH_MAC_FLAG_WORK_WITH_CACHE_DISABLE) {
        emac = heap_caps_calloc(1, sizeof(emac_esp32_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    } else {
        emac = calloc(1, sizeof(emac_esp32_t));
    }
    ESP_GOTO_ON_FALSE(emac, ESP_ERR_NO_MEM, err, TAG, "no mem for esp emac object");

    emac_esp_dma_config_t emac_dma_config;
    ESP_GOTO_ON_ERROR(emac_esp_new_dma(&emac_dma_config, &emac->emac_dma_hndl), err, TAG, "create EMAC DMA object failed");

    /* alloc PM lock */
#ifdef CONFIG_PM_ENABLE
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "emac_esp32", &emac->pm_lock), err, TAG, "create pm lock failed");
#endif
    /* create rx task */
    BaseType_t core_num = tskNO_AFFINITY;
    if (config->flags & ETH_MAC_FLAG_PIN_TO_CORE) {
        core_num = esp_cpu_get_core_id();
    }
    BaseType_t xReturned = xTaskCreatePinnedToCore(emac_esp32_rx_task, "emac_rx", config->rx_task_stack_size, emac,
                                                   config->rx_task_prio, &emac->rx_task_hdl, core_num);
    ESP_GOTO_ON_FALSE(xReturned == pdPASS, ESP_FAIL, err, TAG, "create emac_rx task failed");

    *emac_out_hdl = emac;
    return ESP_OK;
err:
    esp_emac_free_driver_obj(emac);
    return ret;
}

static esp_err_t esp_emac_config_data_interface(const eth_esp32_emac_config_t *esp32_emac_config, emac_esp32_t *emac)
{
    esp_err_t ret = ESP_OK;
    switch (esp32_emac_config->interface) {
    case EMAC_DATA_INTERFACE_MII:
        emac->clock_config = esp32_emac_config->clock_config;
        /* MII interface GPIO initialization */
        ESP_GOTO_ON_ERROR(emac_esp_iomux_init_mii(NULL), err, TAG, "invalid EMAC MII data plane GPIO");
        /* Enable MII clock */
        EMAC_IF_RCC_ATOMIC() {
            emac_hal_clock_enable_mii(&emac->hal);
        }
        break;
    case EMAC_DATA_INTERFACE_RMII:
        // by default, the clock mode is selected at compile time (by Kconfig)
        if (esp32_emac_config->clock_config.rmii.clock_mode == EMAC_CLK_DEFAULT) {
#ifdef CONFIG_IDF_TARGET_ESP32
#if CONFIG_ETH_RMII_CLK_INPUT
#if CONFIG_ETH_RMII_CLK_IN_GPIO == 0
            emac->clock_config.rmii.clock_mode = EMAC_CLK_EXT_IN;
            emac->clock_config.rmii.clock_gpio = CONFIG_ETH_RMII_CLK_IN_GPIO;
#else
#error "ESP32 EMAC only support input RMII clock to GPIO0"
#endif // CONFIG_ETH_RMII_CLK_IN_GPIO == 0
#elif CONFIG_ETH_RMII_CLK_OUTPUT
            emac->clock_config.rmii.clock_mode = EMAC_CLK_OUT;
#if CONFIG_ETH_RMII_CLK_OUTPUT_GPIO0
            emac->clock_config.rmii.clock_gpio = 0;
#elif CONFIG_ETH_RMII_CLK_OUT_GPIO
            emac->clock_config.rmii.clock_gpio = CONFIG_ETH_RMII_CLK_OUT_GPIO;
#endif // CONFIG_ETH_RMII_CLK_OUTPUT_GPIO0
#else
#error "Unsupported RMII clock mode"
#endif // CONFIG_ETH_RMII_CLK_INPUT
#else // EMAC_CLK_DEFAULT "not supported for ESP32P4" - this configuration has been kept due to compatibility reasons for ESP32
            ESP_LOGE(TAG, "EMAC_CLK_DEFAULT options is only supported by ESP32");
            return ESP_ERR_INVALID_ARG;
#endif // CONFIG_IDF_TARGET_ESP32
        } else {
            emac->clock_config = esp32_emac_config->clock_config;
#ifdef CONFIG_IDF_TARGET_ESP32P4
            emac->clock_config_out_in = esp32_emac_config->clock_config_out_in;
#endif // CONFIG_IDF_TARGET_ESP32P4
        }
        /* RMII interface GPIO initialization */
#ifdef CONFIG_IDF_TARGET_ESP32
        ESP_GOTO_ON_ERROR(emac_esp_iomux_init_rmii(NULL), err, TAG, "invalid EMAC RMII data plane GPIO");
#else
        ESP_GOTO_ON_ERROR(emac_esp_iomux_init_rmii(&emac->rmii_gpio), err, TAG, "invalid EMAC RMII data plane GPIO");
#endif // CONFIG_IDF_TARGET_ESP32
        /* If ref_clk is configured as input */
        if (emac->clock_config.rmii.clock_mode == EMAC_CLK_EXT_IN) {
            ESP_GOTO_ON_ERROR(emac_esp_iomux_rmii_clk_input(emac->clock_config.rmii.clock_gpio), err, TAG, "invalid EMAC RMII clock input GPIO");
            EMAC_IF_RCC_ATOMIC() {
                emac_hal_clock_enable_rmii_input(&emac->hal);
            }
        } else if (emac->clock_config.rmii.clock_mode == EMAC_CLK_OUT) {
            ESP_GOTO_ON_ERROR(emac_config_pll_clock(emac), err, TAG, "Configure (A/M)PLL for RMII failed");
#if CONFIG_IDF_TARGET_ESP32
            // we can also use the IOMUX to route the APLL clock to specific GPIO
            if (emac->clock_config.rmii.clock_gpio == EMAC_APPL_CLK_OUT_GPIO) {
                ESP_GOTO_ON_ERROR(esp_clock_output_start(CLKOUT_SIG_APLL, EMAC_APPL_CLK_OUT_GPIO, &emac->rmii_clk_hdl),
                                  err, TAG, "start APLL clock output failed");
            }
#elif CONFIG_IDF_TARGET_ESP32P4
            /* Output RMII clock is routed back to input externally */
            ESP_GOTO_ON_FALSE(emac->clock_config_out_in.rmii.clock_mode == EMAC_CLK_EXT_IN && emac->clock_config_out_in.rmii.clock_gpio >= 0,
                              ESP_ERR_INVALID_ARG, err, TAG, "invalid EMAC input of output clock mode");
            ESP_GOTO_ON_ERROR(emac_esp_iomux_rmii_clk_input(emac->clock_config_out_in.rmii.clock_gpio), err, TAG, "invalid EMAC RMII clock input GPIO");
            EMAC_IF_RCC_ATOMIC() {
                emac_hal_clock_enable_rmii_input(&emac->hal);
            }
#endif
            ESP_GOTO_ON_ERROR(emac_esp_iomux_rmii_clk_ouput(emac->clock_config.rmii.clock_gpio), err, TAG, "invalid EMAC RMII clock output GPIO");
            /* Enable RMII Output clock */
            EMAC_IF_RCC_ATOMIC () {
                emac_hal_clock_enable_rmii_output(&emac->hal);
            }
        } else {
            ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "invalid EMAC clock mode");
        }
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "invalid EMAC Data Interface:%i", esp32_emac_config->interface);
    }
err:
    return ret;
}

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

    ret_code = esp_emac_alloc_driver_obj(config, &emac);
    ESP_RETURN_ON_FALSE(ret_code == ESP_OK, NULL, TAG, "alloc driver object failed");

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

#ifdef SOC_EMAC_USE_IO_MUX
    emac->rmii_gpio.tx_en_num = esp32_config->emac_dataif_gpio.rmii.tx_en_num;
    emac->rmii_gpio.txd0_num = esp32_config->emac_dataif_gpio.rmii.txd0_num;
    emac->rmii_gpio.txd1_num = esp32_config->emac_dataif_gpio.rmii.txd1_num;
    emac->rmii_gpio.crs_dv_num = esp32_config->emac_dataif_gpio.rmii.crs_dv_num;
    emac->rmii_gpio.rxd0_num = esp32_config->emac_dataif_gpio.rmii.rxd0_num;
    emac->rmii_gpio.rxd1_num = esp32_config->emac_dataif_gpio.rmii.rxd1_num;
#endif // SOC_EMAC_USE_IO_MUX
    ret_code = esp_emac_config_data_interface(esp32_config, emac);
    ESP_GOTO_ON_FALSE(ret_code == ESP_OK, NULL, err, TAG, "config emac interface failed");

    emac->dma_burst_len = esp32_config->dma_burst_len;
    emac->sw_reset_timeout_ms = config->sw_reset_timeout_ms;
    emac->smi_gpio.mdc_num = esp32_config->smi_mdc_gpio_num;
    emac->smi_gpio.mdio_num = esp32_config->smi_mdio_gpio_num;

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
    emac->parent.set_speed = emac_esp32_set_speed;
    emac->parent.set_duplex = emac_esp32_set_duplex;
    emac->parent.set_link = emac_esp32_set_link;
    emac->parent.set_promiscuous = emac_esp32_set_promiscuous;
    emac->parent.set_peer_pause_ability = emac_esp32_set_peer_pause_ability;
    emac->parent.enable_flow_ctrl = emac_esp32_enable_flow_ctrl;
    emac->parent.transmit = emac_esp32_transmit;
    emac->parent.transmit_vargs = emac_esp32_transmit_multiple_bufs;
    emac->parent.receive = emac_esp32_receive;
    return &(emac->parent);

err:
    esp_emac_free_driver_obj(emac);
    return ret;
}
