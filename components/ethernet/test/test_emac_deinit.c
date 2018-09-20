/**
 * @brief This test has just run in the ESP32_Ethernet_V3 board, which featured
 * in PoE submodule and TLK110 PHY. The 50MHz clock used by MAC and PHY is
 * supplied by external oscillator through GPIO0.
 *
 * @file test_emac_deinit.c
 * @author morris
 * @date 2018-08-24
 */
#include <string.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_err.h"
#include "esp_event_loop.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_eth.h"

#include "rom/gpio.h"

#include "tcpip_adapter.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"

#include "eth_phy/phy_tlk110.h"

#define DEFAULT_ETHERNET_PHY_CONFIG phy_tlk110_default_ethernet_config

static const char *TAG = "eth_test_deinit";

#define PIN_PHY_POWER 17
#define PIN_SMI_MDC 23
#define PIN_SMI_MDIO 18
#define CONFIG_PHY_ADDRESS 31
#define CONFIG_PHY_CLOCK_MODE 0

static void phy_device_power_enable_via_gpio(bool enable)
{
    if (!enable) {
        DEFAULT_ETHERNET_PHY_CONFIG.phy_power_enable(false);
    }

    gpio_pad_select_gpio(PIN_PHY_POWER);
    gpio_set_direction(PIN_PHY_POWER, GPIO_MODE_OUTPUT);
    if (enable == true) {
        gpio_set_level(PIN_PHY_POWER, 1);
        ESP_LOGI(TAG, "power on ethernet phy");
    } else {
        gpio_set_level(PIN_PHY_POWER, 0);
        ESP_LOGI(TAG, "power off ethernet phy");
    }

    vTaskDelay(1); // Allow the power up/down to take effect, min 300us

    if (enable) {
        /* operates the default phy-specific power on function */
        DEFAULT_ETHERNET_PHY_CONFIG.phy_power_enable(true);
    }
}

static void eth_gpio_config_rmii(void)
{
    phy_rmii_configure_data_interface_pins();
    phy_rmii_smi_configure_pins(PIN_SMI_MDC, PIN_SMI_MDIO);
}

TEST_CASE("test emac deinit", "[ethernet][ignore]")
{
    eth_config_t config = DEFAULT_ETHERNET_PHY_CONFIG;
    config.phy_addr = CONFIG_PHY_ADDRESS;
    config.gpio_config = eth_gpio_config_rmii;
    config.tcpip_input = tcpip_adapter_eth_input;
    config.clock_mode = CONFIG_PHY_CLOCK_MODE;
    config.phy_power_enable = phy_device_power_enable_via_gpio;

    ESP_ERROR_CHECK(esp_eth_init(&config));
    ESP_ERROR_CHECK(esp_eth_enable());

    vTaskDelay(2000 / portTICK_RATE_MS);

    ESP_ERROR_CHECK(esp_eth_disable());
    ESP_ERROR_CHECK(esp_eth_deinit());
}
