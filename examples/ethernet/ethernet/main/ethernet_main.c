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

static const char *TAG = "eth_demo";
static int debug = 0;

#define PIN_PHY_POWER 17
#define PIN_SMI_MDC   23
#define PIN_SMI_MDIO  18

#if (CONFIG_PHY_TLK110)
#include "tlk110_phy.h"
void dump_tlk110_registers();
#define DEFAULT_PHY_CONFIG (AUTO_MDIX_ENABLE|AUTO_NEGOTIATION_ENABLE|AN_1|AN_0|LED_CFG)

void phy_tlk110_check_phy_init(void)
{
    if (debug)
        dump_tlk110_registers();
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
        if (debug)
            ESP_LOGI(TAG, "phy_tlk110_get_speed_mode(100)");
        return ETH_SPEED_MODE_100M;
    } else {
        if (debug)
            ESP_LOGI(TAG, "phy_tlk110_get_speed_mode(10)");
        return ETH_SPEED_MODE_10M;
    }
}

eth_duplex_mode_t phy_tlk110_get_duplex_mode(void)
{
    if((esp_eth_smi_read(PHY_STATUS_REG) & DUPLEX_STATUS ) == DUPLEX_STATUS) {
        if (debug)
            ESP_LOGI(TAG, "phy_tlk110_get_duplex_mode(FULL)");
        return ETH_MDOE_FULLDUPLEX;
    } else {
        if (debug)
            ESP_LOGI(TAG, "phy_tlk110_get_duplex_mode(HALF)");
        return ETH_MODE_HALFDUPLEX;
    }
}

bool phy_tlk110_check_phy_link_status(void)
{
    if ((esp_eth_smi_read(BASIC_MODE_STATUS_REG) & LINK_STATUS) == LINK_STATUS) {
        if (debug)
            ESP_LOGI(TAG, "phy_tlk110_check_phy_link_status(UP)");
        return true;
    } else {
        if (debug)
            ESP_LOGI(TAG, "phy_tlk110_check_phy_link_status(DOWN)");
        return false;
    }
}

bool phy_tlk110_get_partner_pause_enable(void)
{
    if((esp_eth_smi_read(PHY_LINK_PARTNER_ABILITY_REG) & PARTNER_PAUSE) == PARTNER_PAUSE) {
        if (debug)
            ESP_LOGI(TAG, "phy_tlk110_get_partner_pause_enable(TRUE)");
        return true;
    } else {
        if (debug)
            ESP_LOGI(TAG, "phy_tlk110_get_partner_pause_enable(FALSE)");
        return false;
    }
}

void phy_enable_flow_ctrl(void)
{
    uint32_t data = 0;
    data = esp_eth_smi_read(AUTO_NEG_ADVERTISEMENT_REG);
    esp_eth_smi_write(AUTO_NEG_ADVERTISEMENT_REG,data|ASM_DIR|PAUSE);
}

void phy_tlk110_power_enable(bool enable)
{
    gpio_pad_select_gpio(PIN_PHY_POWER);
    gpio_set_direction(PIN_PHY_POWER,GPIO_MODE_OUTPUT);
    if(enable == true) {
        gpio_set_level(PIN_PHY_POWER, 1);
    } else {
        gpio_set_level(PIN_PHY_POWER, 0);
    }
}

void phy_tlk110_init(void)
{
    if (debug) {
        ESP_LOGI(TAG, "phy_tlk110_init()");
        dump_tlk110_registers();
    }
    esp_eth_smi_write(PHY_RESET_CONTROL_REG, SOFTWARE_RESET);

    while (esp_eth_smi_read(PHY_IDENTIFIER_REG) != OUI_MSB_21TO6_DEF) {
    }

    esp_eth_smi_write(SOFTWARE_STRAP_CONTROL_REG, DEFAULT_PHY_CONFIG |SW_STRAP_CONFIG_DONE);

    ets_delay_us(300);

    //if config.flow_ctrl_enable == true ,enable this
    phy_enable_flow_ctrl();
}

void dump_tlk110_registers()
{
    ESP_LOGI(TAG, "TLK110 Registers:");
    ESP_LOGI(TAG, "BMCR     0x%04x", esp_eth_smi_read(0x0));
    ESP_LOGI(TAG, "BMSR     0x%04x", esp_eth_smi_read(0x1));
    ESP_LOGI(TAG, "PHYIDR1  0x%04x", esp_eth_smi_read(0x2));
    ESP_LOGI(TAG, "PHYIDR2  0x%04x", esp_eth_smi_read(0x3));
    ESP_LOGI(TAG, "ANAR     0x%04x", esp_eth_smi_read(0x4));
    ESP_LOGI(TAG, "ANLPAR   0x%04x", esp_eth_smi_read(0x5));
    ESP_LOGI(TAG, "ANER     0x%04x", esp_eth_smi_read(0x6));
    ESP_LOGI(TAG, "ANNPTR   0x%04x", esp_eth_smi_read(0x7));
    ESP_LOGI(TAG, "ANLNPTR  0x%04x", esp_eth_smi_read(0x8));
    ESP_LOGI(TAG, "SWSCR1   0x%04x", esp_eth_smi_read(0x9));
    ESP_LOGI(TAG, "SWSCR2   0x%04x", esp_eth_smi_read(0xa));
    ESP_LOGI(TAG, "SWSCR3   0x%04x", esp_eth_smi_read(0xb));
    ESP_LOGI(TAG, "REGCR    0x%04x", esp_eth_smi_read(0xd));
    ESP_LOGI(TAG, "ADDAR    0x%04x", esp_eth_smi_read(0xe));
    ESP_LOGI(TAG, "PHYSTS   0x%04x", esp_eth_smi_read(0x10));
    ESP_LOGI(TAG, "PHYSCR   0x%04x", esp_eth_smi_read(0x11));
    ESP_LOGI(TAG, "MISR1    0x%04x", esp_eth_smi_read(0x12));
    ESP_LOGI(TAG, "MISR2    0x%04x", esp_eth_smi_read(0x13));
    ESP_LOGI(TAG, "FCSCR    0x%04x", esp_eth_smi_read(0x14));
    ESP_LOGI(TAG, "RECR     0x%04x", esp_eth_smi_read(0x15));
    ESP_LOGI(TAG, "BISCR    0x%04x", esp_eth_smi_read(0x16));
    ESP_LOGI(TAG, "RBR      0x%04x", esp_eth_smi_read(0x17));
    ESP_LOGI(TAG, "LEDCR    0x%04x", esp_eth_smi_read(0x18));
    ESP_LOGI(TAG, "PHYCR    0x%04x", esp_eth_smi_read(0x19));
    ESP_LOGI(TAG, "10BTSCR  0x%04x", esp_eth_smi_read(0x1a));
    ESP_LOGI(TAG, "BICSR1   0x%04x", esp_eth_smi_read(0x1b));
    ESP_LOGI(TAG, "BICSR2   0x%04x", esp_eth_smi_read(0x1c));
    ESP_LOGI(TAG, "CDCR     0x%04x", esp_eth_smi_read(0x1e));
    ESP_LOGI(TAG, "TRXCPSR  0x%04x", esp_eth_smi_read(0x42));
    ESP_LOGI(TAG, "PWRBOCR  0x%04x", esp_eth_smi_read(0xae));
    ESP_LOGI(TAG, "VRCR     0x%04x", esp_eth_smi_read(0xD0));
    ESP_LOGI(TAG, "ALCDRR1  0x%04x", esp_eth_smi_read(0x155));
    ESP_LOGI(TAG, "CDSCR1   0x%04x", esp_eth_smi_read(0x170));
    ESP_LOGI(TAG, "CDSCR2   0x%04x", esp_eth_smi_read(0x171));
}
#endif

#if (CONFIG_PHY_LAN8720)
#include "lan8720_phy.h"
void dump_lan8720_registers();

void phy_lan8720_check_phy_init(void)
{
    if (debug)
        ESP_LOGI(TAG, "phy_lan8720_check_phy_init()");

    while((esp_eth_smi_read(BASIC_MODE_STATUS_REG) & AUTO_NEGOTIATION_COMPLETE ) != AUTO_NEGOTIATION_COMPLETE)
    {};
    while((esp_eth_smi_read(PHY_SPECIAL_CONTROL_STATUS_REG) & AUTO_NEGOTIATION_DONE ) != AUTO_NEGOTIATION_DONE)
    {};
}

eth_speed_mode_t phy_lan8720_get_speed_mode(void)
{
    if(esp_eth_smi_read(PHY_SPECIAL_CONTROL_STATUS_REG) & SPEED_INDICATION_100T ) {
        if (debug)
            ESP_LOGI(TAG, "phy_lan8720_get_speed_mode(100)");
        return ETH_SPEED_MODE_100M;
    } else {
        if (debug)
            ESP_LOGI(TAG, "phy_lan8720_get_speed_mode(10)");
        return ETH_SPEED_MODE_10M;
    }
}

eth_duplex_mode_t phy_lan8720_get_duplex_mode(void)
{
    if(esp_eth_smi_read(PHY_SPECIAL_CONTROL_STATUS_REG) & DUPLEX_INDICATION_FULL ) {
        if (debug)
            ESP_LOGI(TAG, "phy_lan8720_get_duplex_mode(FULL)");
        return ETH_MDOE_FULLDUPLEX;
    } else {
        if (debug)
            ESP_LOGI(TAG, "phy_lan8720_get_duplex_mode(HALF)");
        return ETH_MODE_HALFDUPLEX;
    }
}

bool phy_lan8720_check_phy_link_status(void)
{
    if ((esp_eth_smi_read(BASIC_MODE_STATUS_REG) & LINK_STATUS) == LINK_STATUS) {
        if (debug)
            ESP_LOGI(TAG, "phy_lan8720_check_phy_link_status(UP)");
        return true;
    } else {
        if (debug)
            ESP_LOGI(TAG, "phy_lan8720_check_phy_link_status(DOWN)");
        return false;
    }
}

bool phy_lan8720_get_partner_pause_enable(void)
{
    if((esp_eth_smi_read(PHY_LINK_PARTNER_ABILITY_REG) & PARTNER_PAUSE) == PARTNER_PAUSE) {
        if (debug)
            ESP_LOGI(TAG, "phy_lan8720_get_partner_pause_enable(TRUE)");
        return true;
    } else {
        if (debug)
            ESP_LOGI(TAG, "phy_lan8720_get_partner_pause_enable(FALSE)");
        return false;
    }
}

void phy_enable_flow_ctrl(void)
{
    uint32_t data = 0;
    data = esp_eth_smi_read(AUTO_NEG_ADVERTISEMENT_REG);
    esp_eth_smi_write(AUTO_NEG_ADVERTISEMENT_REG,data|ASM_DIR|PAUSE);
}

void phy_lan8720_power_enable(bool enable)
{
    gpio_pad_select_gpio(PIN_PHY_POWER);
    gpio_set_direction(PIN_PHY_POWER,GPIO_MODE_OUTPUT);
    if(enable == true) {
        gpio_set_level(PIN_PHY_POWER, 1);
        if (debug)
            ESP_LOGI(TAG, "phy_lan8720_power_enable(TRUE)");
    } else {
        gpio_set_level(PIN_PHY_POWER, 0);
        if (debug)
            ESP_LOGI(TAG, "power_enable(FALSE)");
    }
}

void phy_lan8720_init(void)
{
    if (debug) {
        ESP_LOGI(TAG, "phy_lan8720_init()");
        dump_lan8720_registers();
    }

    esp_eth_smi_write(BASIC_MODE_CONTROL_REG, SOFTWARE_RESET);

    while (esp_eth_smi_read(PHY_IDENTIFIER_REG) != OUI_MSB_21TO6_DEF) {
    }

    ets_delay_us(300);

    //if config.flow_ctrl_enable == true ,enable this
    phy_enable_flow_ctrl();
}

void dump_lan8720_registers()
{
    ESP_LOGI(TAG, "LAN8720 Registers:");
    ESP_LOGI(TAG, "BCR    0x%04x", esp_eth_smi_read(0x0));
    ESP_LOGI(TAG, "BSR    0x%04x", esp_eth_smi_read(0x1));
    ESP_LOGI(TAG, "PHY1   0x%04x", esp_eth_smi_read(0x2));
    ESP_LOGI(TAG, "PHY2   0x%04x", esp_eth_smi_read(0x3));
    ESP_LOGI(TAG, "ANAR   0x%04x", esp_eth_smi_read(0x4));
    ESP_LOGI(TAG, "ANLPAR 0x%04x", esp_eth_smi_read(0x5));
    ESP_LOGI(TAG, "ANER   0x%04x", esp_eth_smi_read(0x6));
    ESP_LOGI(TAG, "MCSR   0x%04x", esp_eth_smi_read(0x17));
    ESP_LOGI(TAG, "SM     0x%04x", esp_eth_smi_read(0x18));
    ESP_LOGI(TAG, "SECR   0x%04x", esp_eth_smi_read(0x26));
    ESP_LOGI(TAG, "CSIR   0x%04x", esp_eth_smi_read(0x27));
    ESP_LOGI(TAG, "ISR    0x%04x", esp_eth_smi_read(0x29));
    ESP_LOGI(TAG, "IMR    0x%04x", esp_eth_smi_read(0x30));
    ESP_LOGI(TAG, "PSCSR  0x%04x", esp_eth_smi_read(0x31));
}
#endif // CONFIG_PHY_LAN8720

void eth_gpio_config_rmii(void)
{
    //crs_dv to gpio27 ,can not change (default so not needed but physical signal must be connected)
    //PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO27_U, FUNC_GPIO27_EMAC_RX_DV);

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

    //mdc to gpio23
    gpio_matrix_out(PIN_SMI_MDC, EMAC_MDC_O_IDX, 0, 0);
    //mdio to gpio18
    gpio_matrix_out(PIN_SMI_MDIO, EMAC_MDO_O_IDX, 0, 0);
    gpio_matrix_in(PIN_SMI_MDIO, EMAC_MDI_I_IDX, 0);
}

void eth_task(void *pvParameter)
{
    tcpip_adapter_ip_info_t ip;
    memset(&ip, 0, sizeof(tcpip_adapter_ip_info_t));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    while (1) {

        vTaskDelay(2000 / portTICK_PERIOD_MS);

        if (tcpip_adapter_get_ip_info(ESP_IF_ETH, &ip) == 0) {
#if (CONFIG_PHY_TLK110)
            if (debug)
                dump_tlk110_registers();
#endif
#if (CONFIG_PHY_LAN8720)
            if (debug)
                dump_lan8720_registers();
#endif
            ESP_LOGI(TAG, "~~~~~~~~~~~");
            ESP_LOGI(TAG, "ETHIP:"IPSTR, IP2STR(&ip.ip));
            ESP_LOGI(TAG, "ETHPMASK:"IPSTR, IP2STR(&ip.netmask));
            ESP_LOGI(TAG, "ETHPGW:"IPSTR, IP2STR(&ip.gw));
            ESP_LOGI(TAG, "~~~~~~~~~~~");
        }
    }
}

void app_main()
{
    esp_err_t ret = ESP_OK;
    tcpip_adapter_init();
    esp_event_loop_init(NULL, NULL);

#if (CONFIG_PHY_TLK110)
    eth_config_t config;
//    config.phy_addr = PHY31;
    config.phy_addr = PHY1; // tlk eval board
    config.mac_mode = ETH_MODE_RMII;
    config.phy_init = phy_tlk110_init;
    config.gpio_config = eth_gpio_config_rmii;
    config.tcpip_input = tcpip_adapter_eth_input;
    config.phy_check_init = phy_tlk110_check_phy_init;
    config.phy_check_link = phy_tlk110_check_phy_link_status;
    config.phy_get_speed_mode = phy_tlk110_get_speed_mode;
    config.phy_get_duplex_mode = phy_tlk110_get_duplex_mode;
    //Only FULLDUPLEX mode support flow ctrl now!
    config.flow_ctrl_enable = true;
    config.phy_get_partner_pause_enable = phy_tlk110_get_partner_pause_enable;
    config.phy_power_enable = phy_tlk110_power_enable;
#endif
#if (CONFIG_PHY_LAN8720)
    eth_config_t config;
    //Only FULLDUPLEX mode support flow ctrl now!
    config.flow_ctrl_enable = true;
    config.phy_addr = PHY0;
    config.mac_mode = ETH_MODE_RMII;
    config.gpio_config = eth_gpio_config_rmii;
    config.tcpip_input = tcpip_adapter_eth_input;
    config.phy_init = phy_lan8720_init;
    config.phy_check_init = phy_lan8720_check_phy_init;
    config.phy_check_link = phy_lan8720_check_phy_link_status;
    config.phy_get_speed_mode = phy_lan8720_get_speed_mode;
    config.phy_get_duplex_mode = phy_lan8720_get_duplex_mode;
    config.phy_get_partner_pause_enable = phy_lan8720_get_partner_pause_enable;
    config.phy_power_enable = phy_lan8720_power_enable;
#endif

    ret = esp_eth_init(&config);

    if(ret == ESP_OK) {
        esp_eth_enable();
        xTaskCreate(eth_task, "eth_task", 2048, NULL, (tskIDLE_PRIORITY + 2), NULL);
    }

}
