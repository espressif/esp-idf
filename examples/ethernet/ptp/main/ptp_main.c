/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_eth.h"
#include "esp_netif.h"
#include "ethernet_init.h"
#include "esp_vfs_l2tap.h"
#include "driver/gpio.h"
#include "ptpd.h"

#include "esp_eth_time.h"
#include "esp_eth_mac_esp.h"

#define ETH_IF_KEY "ETH_0"

static const char *TAG = "ptp_example";

#if CONFIG_EXAMPLE_PTP_PULSE_CALLBACK
static struct timespec s_next_time;
static bool s_gpio_level;
#endif //CONFIG_EXAMPLE_PTP_PULSE_CALLBACK

void init_ethernet_and_netif(void)
{
    uint8_t eth_port_cnt;
    esp_eth_handle_t *eth_handles;

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(ethernet_init_all(&eth_handles, &eth_port_cnt));

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_vfs_l2tap_intf_register(NULL));

    esp_netif_inherent_config_t esp_netif_base_config = ESP_NETIF_INHERENT_DEFAULT_ETH();
    esp_netif_config_t esp_netif_config = {
        .base = &esp_netif_base_config,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH
    };
    char if_key_str[10];
    char if_desc_str[10];
    char num_str[3];
    for (int i = 0; i < eth_port_cnt; i++) {
        itoa(i, num_str, 10);
        strcat(strcpy(if_key_str, "ETH_"), num_str);
        strcat(strcpy(if_desc_str, "eth"), num_str);
        esp_netif_base_config.if_key = if_key_str;
        esp_netif_base_config.if_desc = if_desc_str;
        esp_netif_base_config.route_prio -= i*5;
        esp_netif_t *eth_netif = esp_netif_new(&esp_netif_config);

        // attach Ethernet driver to TCP/IP stack
        ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handles[i])));
    }

    for (int i = 0; i < eth_port_cnt; i++) {
        ESP_ERROR_CHECK(esp_eth_start(eth_handles[i]));
    }
}

#if CONFIG_EXAMPLE_PTP_PULSE_CALLBACK
IRAM_ATTR bool ts_callback(esp_eth_mediator_t *eth, void *user_args)
{
    gpio_set_level(CONFIG_EXAMPLE_PTP_PULSE_GPIO, s_gpio_level ^= 1);

    // Set the next target time
    struct timespec interval = {
        .tv_sec = 0,
        .tv_nsec = CONFIG_EXAMPLE_PTP_PULSE_WIDTH_NS
    };
    timespecadd(&s_next_time, &interval, &s_next_time);

    struct timespec curr_time;
    esp_eth_clock_gettime(CLOCK_PTP_SYSTEM, &curr_time);
    // check the next time is in the future
    if (timespeccmp(&s_next_time, &curr_time, >)) {
        esp_eth_clock_set_target_time(CLOCK_PTP_SYSTEM, &s_next_time);
    }

    return false;
}
#endif //CONFIG_EXAMPLE_PTP_PULSE_CALLBACK

void app_main(void)
{
    ESP_LOGI(TAG, "Starting PTP example");
    init_ethernet_and_netif();

    int pid = ptpd_start(ETH_IF_KEY);

#if CONFIG_EXAMPLE_PTP_PULSE_EMAC_PPS
    (void)pid; // suppress compiler warning about unused variable
    esp_eth_handle_t eth_handle = esp_netif_get_io_driver(esp_netif_get_handle_from_ifkey(ETH_IF_KEY));
    esp_eth_mac_t *mac;
    ESP_ERROR_CHECK(esp_eth_get_mac_instance(eth_handle, &mac));
    ESP_ERROR_CHECK(esp_eth_mac_set_pps_out_gpio(mac, CONFIG_EXAMPLE_PTP_PULSE_GPIO));
    ESP_ERROR_CHECK(esp_eth_mac_set_pps_out_freq(mac, CONFIG_EXAMPLE_PTP_PPS_FREQ_HZ));
#elif CONFIG_EXAMPLE_PTP_PULSE_CALLBACK
    struct timespec cur_time;
    // wait for the clock to be available
    while (esp_eth_clock_gettime(CLOCK_PTP_SYSTEM, &cur_time) == -1) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    // register callback function which will toggle output pin
    esp_eth_clock_register_target_cb(CLOCK_PTP_SYSTEM, ts_callback);

    // initialize output pin
    gpio_config_t gpio_out_cfg = {
        .pin_bit_mask = (1ULL << CONFIG_EXAMPLE_PTP_PULSE_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&gpio_out_cfg);
    gpio_set_level(CONFIG_EXAMPLE_PTP_PULSE_GPIO, 0);

    bool first_pass = true;
    bool clock_source_valid = false;
    bool clock_source_valid_last = false;
    int32_t clock_source_valid_cnt = 0;
    while (1) {
        struct ptpd_status_s ptp_status;
        // if valid PTP status
        if (ptpd_status(pid, &ptp_status) == 0) {
            if (ptp_status.clock_source_valid) {
                clock_source_valid_cnt++;
            } else {
                clock_source_valid_cnt = 0;
            }
        } else {
            clock_source_valid_cnt = 0;
        }
        // consider the source valid only after n consequent intervals to be sure clock was synced
        if (clock_source_valid_cnt > 2) {
            clock_source_valid = true;
        } else {
            clock_source_valid = false;
        }
        // source validity changed => resync the pulse for ptp slave OR when the first pass to PTP master
        // starts generating its pulses
        if ((clock_source_valid == true && clock_source_valid_last == false) || first_pass) {
            first_pass = false;
            // get the most recent (now synced) time
            esp_eth_clock_gettime(CLOCK_PTP_SYSTEM, &cur_time);
            // compute the next target time
            s_next_time.tv_sec = 1;
            timespecadd(&s_next_time, &cur_time, &s_next_time);
            s_next_time.tv_nsec = CONFIG_EXAMPLE_PTP_PULSE_WIDTH_NS;
            ESP_LOGI(TAG, "Starting Pulse train");
            ESP_LOGI(TAG, "curr time: %llu.%09lu", cur_time.tv_sec, cur_time.tv_nsec);
            ESP_LOGI(TAG, "next time: %llu.%09lu", s_next_time.tv_sec, s_next_time.tv_nsec);
            s_gpio_level = 0;
            gpio_set_level(CONFIG_EXAMPLE_PTP_PULSE_GPIO, s_gpio_level);
            esp_eth_clock_set_target_time(CLOCK_PTP_SYSTEM, &s_next_time);
        }
        clock_source_valid_last = clock_source_valid;
    }
#endif //CONFIG_EXAMPLE_PTP_PULSE_EMAC_PPS
}
