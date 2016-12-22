/* ethernet Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_err.h"
#include "esp_event_loop.h"
#include "esp_event.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_eth.h"

#include "rom/ets_sys.h"
#include "rom/gpio.h"

#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"

#include "tcpip_adapter.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "tlk110_phy.h"

static const char *TAG = "eth_demo";

#define DEFAULT_PHY_CONFIG (AUTO_MDIX_ENABLE|AUTO_NEGOTIATION_ENABLE|AN_1|AN_0|LED_CFG)

void phy_tlk110_check_phy_init(void)
{
    while((esp_eth_smi_read(BASIC_MODE_STATUS_REG) & AUTO_NEGOTIATION_COMPLETE ) != AUTO_NEGOTIATION_COMPLETE)
    {};
    while((esp_eth_smi_read(PHY_STATUS_REG) & AUTO_NEGTIATION_STATUS ) != AUTO_NEGTIATION_STATUS)
    {};
    while((esp_eth_smi_read(CABLE_DIAGNOSTIC_CONTROL_REG) & DIAGNOSTIC_DONE ) != DIAGNOSTIC_DONE)
    {};
}

eth_speed_mode_t phy_tlk110_get_speed_mode(void)
{
    if((esp_eth_smi_read(PHY_STATUS_REG) & SPEED_STATUS ) != SPEED_STATUS) {
        return ETH_SPEED_MODE_100M;
    } else {
        return ETH_SPEED_MODE_10M;
    }   
}

eth_duplex_mode_t phy_tlk110_get_duplex_mode(void)
{
    if((esp_eth_smi_read(PHY_STATUS_REG) & DUPLEX_STATUS ) == DUPLEX_STATUS) {
        return ETH_MDOE_FULLDUPLEX;
    } else {
        return ETH_MODE_HALFDUPLEX;
    }   
}

bool phy_tlk110_check_phy_link_status(void)
{
    return ((esp_eth_smi_read(BASIC_MODE_STATUS_REG) & LINK_STATUS) == LINK_STATUS );
}

void phy_tlk110_init(void)
{
    esp_eth_smi_write(PHY_RESET_CONTROL_REG, SOFTWARE_RESET);

    while (esp_eth_smi_read(PHY_IDENTIFIER_REG) != OUI_MSB_21TO6_DEF) {
    }

    esp_eth_smi_write(SOFTWARE_STAP_CONTROL_REG, DEFAULT_PHY_CONFIG |SW_STRAP_CONFIG_DONE);
    ets_delay_us(300);
}

void eth_gpio_config_rmii(void)
{
    //txd0 to gpio19 ,can not change
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO19_U, FUNC_GPIO19_EMAC_TXD0);
    //tx_en to gpio21 ,can not change
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO21_U, FUNC_GPIO21_EMAC_TX_EN);
    //txd1 to gpio22 , can not change
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO22_U, FUNC_GPIO22_EMAC_TXD1);
    //rxd0 to gpio25 , can not change
    gpio_set_direction(25, GPIO_MODE_INPUT);
    //rxd1 to gpio26 ,can not change
    gpio_set_direction(26, GPIO_MODE_INPUT);
    //rmii clk  ,can not change
    gpio_set_direction(0, GPIO_MODE_INPUT);

    //mdc to gpio4
    gpio_matrix_out(4, EMAC_MDC_O_IDX, 0, 0);
    //mdio to gpio2
    gpio_matrix_out(2, EMAC_MDO_O_IDX, 0, 0);
    gpio_matrix_in(2, EMAC_MDI_I_IDX, 0);
}

void eth_task(void *pvParameter)
{
    tcpip_adapter_ip_info_t ip;
    memset(&ip, 0, sizeof(tcpip_adapter_ip_info_t));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    while (1) {

        vTaskDelay(2000 / portTICK_PERIOD_MS);

        if (tcpip_adapter_get_ip_info(ESP_IF_ETH, &ip) == 0) {
            ESP_LOGI(TAG, "\n~~~~~~~~~~~\n");
            ESP_LOGI(TAG, "ETHIP:"IPSTR, IP2STR(&ip.ip));
            ESP_LOGI(TAG, "ETHPMASK:"IPSTR, IP2STR(&ip.netmask));
            ESP_LOGI(TAG, "ETHPGW:"IPSTR, IP2STR(&ip.gw));
            ESP_LOGI(TAG, "\n~~~~~~~~~~~\n");
        }
    }
}

void app_main()
{
    esp_err_t ret = ESP_OK;
    tcpip_adapter_init();
    esp_event_loop_init(NULL, NULL);

    eth_config_t config;
    config.phy_addr = PHY31;
    config.mac_mode = ETH_MODE_RMII;
    config.phy_init = phy_tlk110_init;
    config.gpio_config = eth_gpio_config_rmii;
    config.tcpip_input = tcpip_adapter_eth_input;
    config.phy_check_init = phy_tlk110_check_phy_init;
    config.phy_check_link = phy_tlk110_check_phy_link_status;
    config.phy_get_speed_mode = phy_tlk110_get_speed_mode;
    config.phy_get_duplex_mode = phy_tlk110_get_duplex_mode;

    ret = esp_eth_init(&config);

    if(ret == ESP_OK) {
        esp_eth_enable();
        xTaskCreate(eth_task, "eth_task", 2048, NULL, (tskIDLE_PRIORITY + 2), NULL);
    }

}
