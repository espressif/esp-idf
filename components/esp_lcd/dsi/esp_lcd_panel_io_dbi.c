/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_check.h"
#include "esp_lcd_panel_io_interface.h"
#include "esp_lcd_mipi_dsi.h"
#include "mipi_dsi_priv.h"

static const char *TAG = "lcd.dsi.dbi";

typedef struct esp_lcd_dbi_io_t esp_lcd_dbi_io_t;

struct esp_lcd_dbi_io_t {
    esp_lcd_panel_io_t base;      // Base class of generic lcd panel IO
    esp_lcd_dsi_bus_handle_t bus; // DSI bus handle
    uint8_t virtual_channel;      // Virtual channel ID, index from 0
    int lcd_cmd_bits;             // Bit-width of LCD command
    int lcd_param_bits;           // Bit-width of LCD parameter
};

static esp_err_t panel_io_dbi_del(esp_lcd_panel_io_t *io);
static esp_err_t panel_io_dbi_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, const void *param, size_t param_size);
static esp_err_t panel_io_dbi_rx_param(esp_lcd_panel_io_t *io, int lcd_cmd, void *param, size_t param_size);

esp_err_t esp_lcd_new_panel_io_dbi(esp_lcd_dsi_bus_handle_t bus, const esp_lcd_dbi_io_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io)
{
    ESP_RETURN_ON_FALSE(bus && io_config && ret_io, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(io_config->virtual_channel < 4, ESP_ERR_INVALID_ARG, TAG, "invalid virtual channel %d", io_config->virtual_channel);
    mipi_dsi_hal_context_t *hal = &bus->hal;

    esp_lcd_dbi_io_t *dbi_io = heap_caps_calloc(1, sizeof(esp_lcd_dbi_io_t), DSI_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(dbi_io, ESP_ERR_NO_MEM, TAG, "no memory for DBI IO");
    dbi_io->virtual_channel = io_config->virtual_channel;
    dbi_io->bus = bus;

    // Tear Effect is not supported
    mipi_dsi_host_ll_enable_te_ack(hal->host, false);
    // enable command ack, to ensure the reliability and integrity of the data transmission
    mipi_dsi_host_ll_enable_cmd_ack(hal->host, true);
    // using low power mode for sending generic MIPI DSI packets
    mipi_dsi_host_ll_set_gen_short_wr_speed_mode(hal->host, 0, MIPI_DSI_LL_TRANS_SPEED_LP);
    mipi_dsi_host_ll_set_gen_short_wr_speed_mode(hal->host, 1, MIPI_DSI_LL_TRANS_SPEED_LP);
    mipi_dsi_host_ll_set_gen_short_wr_speed_mode(hal->host, 2, MIPI_DSI_LL_TRANS_SPEED_LP);
    mipi_dsi_host_ll_set_gen_long_wr_speed_mode(hal->host, MIPI_DSI_LL_TRANS_SPEED_LP);
    mipi_dsi_host_ll_set_gen_short_rd_speed_mode(hal->host, 0, MIPI_DSI_LL_TRANS_SPEED_LP);
    mipi_dsi_host_ll_set_gen_short_rd_speed_mode(hal->host, 1, MIPI_DSI_LL_TRANS_SPEED_LP);
    mipi_dsi_host_ll_set_gen_short_rd_speed_mode(hal->host, 2, MIPI_DSI_LL_TRANS_SPEED_LP);
    mipi_dsi_host_ll_set_dcs_short_wr_speed_mode(hal->host, 0, MIPI_DSI_LL_TRANS_SPEED_LP);
    mipi_dsi_host_ll_set_dcs_short_wr_speed_mode(hal->host, 1, MIPI_DSI_LL_TRANS_SPEED_LP);
    mipi_dsi_host_ll_set_dcs_long_wr_speed_mode(hal->host, MIPI_DSI_LL_TRANS_SPEED_LP);
    mipi_dsi_host_ll_set_dcs_short_rd_speed_mode(hal->host, 0, MIPI_DSI_LL_TRANS_SPEED_LP);
    mipi_dsi_host_ll_set_mrps_speed_mode(hal->host, MIPI_DSI_LL_TRANS_SPEED_LP);

    dbi_io->base.del = panel_io_dbi_del;
    dbi_io->base.tx_param = panel_io_dbi_tx_param;
    dbi_io->base.rx_param = panel_io_dbi_rx_param;
    dbi_io->lcd_cmd_bits = io_config->lcd_cmd_bits;
    dbi_io->lcd_param_bits = io_config->lcd_param_bits;
    *ret_io = &dbi_io->base;
    return ESP_OK;
}

static esp_err_t panel_io_dbi_del(esp_lcd_panel_io_t *io)
{
    esp_lcd_dbi_io_t *dbi_io = __containerof(io, esp_lcd_dbi_io_t, base);
    free(dbi_io);
    return ESP_OK;
}

static esp_err_t panel_io_dbi_tx_param(esp_lcd_panel_io_t *io, int lcd_cmd, const void *param, size_t param_size)
{
    esp_lcd_dbi_io_t *dbi_io = __containerof(io, esp_lcd_dbi_io_t, base);
    esp_lcd_dsi_bus_handle_t bus = dbi_io->bus;
    mipi_dsi_hal_context_t *hal = &bus->hal;

    mipi_dsi_hal_host_gen_write_dcs_command(hal, dbi_io->virtual_channel, lcd_cmd, dbi_io->lcd_cmd_bits / 8, param, param_size);

    return ESP_OK;
}

static esp_err_t panel_io_dbi_rx_param(esp_lcd_panel_io_t *io, int lcd_cmd, void *param, size_t param_size)
{
    esp_lcd_dbi_io_t *dbi_io = __containerof(io, esp_lcd_dbi_io_t, base);
    esp_lcd_dsi_bus_handle_t bus = dbi_io->bus;
    mipi_dsi_hal_context_t *hal = &bus->hal;

    mipi_dsi_hal_host_gen_read_dcs_command(hal, dbi_io->virtual_channel, lcd_cmd, dbi_io->lcd_cmd_bits / 8, param, param_size);

    return ESP_OK;
}
