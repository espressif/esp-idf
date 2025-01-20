/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_check.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_clk_tree.h"
#include "esp_private/esp_clk_tree_common.h"
#include "mipi_dsi_priv.h"

static const char *TAG = "lcd.dsi.bus";

#define MIPI_DSI_DEFAULT_TIMEOUT_CLOCK_FREQ_MHZ 10
// TxClkEsc frequency must be configured between 2 and 20 MHz
#define MIPI_DSI_DEFAULT_ESCAPE_CLOCK_FREQ_MHZ  18

esp_err_t esp_lcd_new_dsi_bus(const esp_lcd_dsi_bus_config_t *bus_config, esp_lcd_dsi_bus_handle_t *ret_bus)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(bus_config && ret_bus, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(bus_config->num_data_lanes <= MIPI_DSI_LL_MAX_DATA_LANES,
                        ESP_ERR_INVALID_ARG, TAG, "invalid number of data lanes %d", bus_config->num_data_lanes);
    ESP_RETURN_ON_FALSE(bus_config->lane_bit_rate_mbps >= MIPI_DSI_LL_MIN_PHY_MBPS &&
                        bus_config->lane_bit_rate_mbps <= MIPI_DSI_LL_MAX_PHY_MBPS, ESP_ERR_INVALID_ARG, TAG,
                        "invalid lane bit rate %"PRIu32, bus_config->lane_bit_rate_mbps);

    // we don't use an bus allocator here, because different DSI bus uses different PHY.
    // And each PHY has its own associated PINs, which is not changeable.
    // So user HAS TO specify the bus ID by themselves, according to their PCB design.
    int bus_id = bus_config->bus_id;
    ESP_RETURN_ON_FALSE(bus_id >= 0 && bus_id < MIPI_DSI_LL_NUM_BUS, ESP_ERR_INVALID_ARG, TAG, "invalid bus ID %d", bus_id);
    esp_lcd_dsi_bus_t *dsi_bus = heap_caps_calloc(1, sizeof(esp_lcd_dsi_bus_t), DSI_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(dsi_bus, ESP_ERR_NO_MEM, TAG, "no memory for DSI bus");
    dsi_bus->bus_id = bus_id;

    // Enable the APB clock for accessing the DSI host and bridge registers
    DSI_RCC_ATOMIC() {
        mipi_dsi_ll_enable_bus_clock(bus_id, true);
        mipi_dsi_ll_reset_register(bus_id);
    }

    // if the clock source is not assigned, fallback to the default clock source
    mipi_dsi_phy_clock_source_t phy_clk_src = bus_config->phy_clk_src;
    if (phy_clk_src == 0) {
        phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT;
    }
    ESP_GOTO_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)phy_clk_src, true), err, TAG, "clock source enable failed");
    // enable the clock source for DSI PHY
    DSI_CLOCK_SRC_ATOMIC() {
        // set clock source for DSI PHY
        mipi_dsi_ll_set_phy_clock_source(bus_id, phy_clk_src);
        // the configuration clock is used for all modes except the shutdown mode
        mipi_dsi_ll_enable_phy_config_clock(bus_id, true);
        // enable the clock for generating the serial clock
        mipi_dsi_ll_enable_phy_reference_clock(bus_id, true);
    }

#if CONFIG_PM_ENABLE
    // When MIPI DSI is working, we don't expect the clock source would be turned off
    esp_pm_lock_type_t pm_lock_type = ESP_PM_NO_LIGHT_SLEEP;
    ret  = esp_pm_lock_create(pm_lock_type, 0, "dsi_phy", &dsi_bus->pm_lock);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "create PM lock failed");
    // before we configure the PLL, we want the clock source to be stable
    esp_pm_lock_acquire(dsi_bus->pm_lock);
#endif

    // if the number of data lanes is not assigned, fallback to the maximum number of data lanes
    int num_data_lanes = bus_config->num_data_lanes;
    if (num_data_lanes == 0) {
        num_data_lanes = MIPI_DSI_LL_MAX_DATA_LANES;
    }
    // initialize HAL context
    mipi_dsi_hal_config_t hal_config = {
        .bus_id = bus_id,
        .lane_bit_rate_mbps = bus_config->lane_bit_rate_mbps,
        .num_data_lanes = num_data_lanes,
    };
    mipi_dsi_hal_init(&dsi_bus->hal, &hal_config);
    mipi_dsi_hal_context_t *hal = &dsi_bus->hal;

    // get the frequency of the PHY clock source
    uint32_t phy_clk_src_freq_hz = 0;
    ESP_GOTO_ON_ERROR(esp_clk_tree_src_get_freq_hz(phy_clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED,
                                                   &phy_clk_src_freq_hz), err, TAG, "get phy clock source frequency failed");
    // configure the PHY PLL
    mipi_dsi_hal_configure_phy_pll(hal, phy_clk_src_freq_hz, bus_config->lane_bit_rate_mbps);

    // wait for PHY initialization done
    while (!mipi_dsi_phy_ll_is_pll_locked(hal->host)) {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    while (!mipi_dsi_phy_ll_are_lanes_stopped(hal->host, num_data_lanes)) {
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    // initialize the DSI operation mode: command mode
    mipi_dsi_host_ll_enable_video_mode(hal->host, false);
    // place the clock lane in low power mode, we will switch to high speed mode later when DPI stream is ready
    mipi_dsi_host_ll_set_clock_lane_state(hal->host, MIPI_DSI_LL_CLOCK_LANE_STATE_LP);
    // Set the time that is required by the clock and data lanes to go from high-speed to low-power and from low-power to high-speed
    mipi_dsi_phy_ll_set_switch_time(hal->host, 50, 104, 46, 128);

    // enable CRC reception and ECC reception, error correction, and reporting
    mipi_dsi_host_ll_enable_rx_crc(hal->host, true);
    mipi_dsi_host_ll_enable_rx_ecc(hal->host, true);
    // enable sending the EoTp packet at the end of each transmission for HS mode
    mipi_dsi_host_ll_enable_tx_eotp(hal->host, true, false);

    // Set the divider to get the Time Out clock, clock source is the high-speed byte clock
    mipi_dsi_host_ll_set_timeout_clock_division(hal->host, bus_config->lane_bit_rate_mbps / 8 / MIPI_DSI_DEFAULT_TIMEOUT_CLOCK_FREQ_MHZ);
    // Set the divider to get the TX Escape clock, clock source is the high-speed byte clock
    mipi_dsi_host_ll_set_escape_clock_division(hal->host, bus_config->lane_bit_rate_mbps / 8 / MIPI_DSI_DEFAULT_ESCAPE_CLOCK_FREQ_MHZ);
    // set the timeout intervals to zero, means to disable the timeout mechanism
    mipi_dsi_host_ll_set_timeout_count(hal->host, 0, 0, 0, 0, 0, 0, 0);
    // DSI host will wait indefinitely for a read response from the DSI device
    mipi_dsi_phy_ll_set_max_read_time(hal->host, 6000);
    // set how long the DSI host will wait before sending the next transmission
    mipi_dsi_phy_ll_set_stop_wait_time(hal->host, 0x3F);

    *ret_bus = dsi_bus;
    return ESP_OK;
err:
    if (dsi_bus) {
        esp_lcd_del_dsi_bus(dsi_bus);
    }
    return ret;
}

esp_err_t esp_lcd_del_dsi_bus(esp_lcd_dsi_bus_handle_t bus)
{
    ESP_RETURN_ON_FALSE(bus, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    int bus_id = bus->bus_id;
    // disable the clock source for DSI PHY
    DSI_CLOCK_SRC_ATOMIC() {
        mipi_dsi_ll_enable_phy_reference_clock(bus_id, false);
        mipi_dsi_ll_enable_phy_config_clock(bus_id, false);
    }
    // disable the APB clock for accessing the DSI peripheral registers
    DSI_RCC_ATOMIC() {
        mipi_dsi_ll_enable_bus_clock(bus_id, false);
    }
#if CONFIG_PM_ENABLE
    if (bus->pm_lock) {
        esp_pm_lock_release(bus->pm_lock);
        esp_pm_lock_delete(bus->pm_lock);
    }
#endif
    free(bus);
    return ESP_OK;
}
