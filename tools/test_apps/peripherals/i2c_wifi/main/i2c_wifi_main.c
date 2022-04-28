/* I2C-WIFI - Test*/

#include <string.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_timer.h"
#include "driver/i2c.h"

#define WAIT_TIMER_PERIOD               (1 * 1000)

#define I2C_SDA_GPIO                    CONFIG_TEST_I2C_SDA_NUM
#define I2C_SCL_GPIO                    CONFIG_TEST_I2C_SCL_NUM
#define I2C_SLAVE_ADDR                  CONFIG_TEST_I2C_SLAVE_ADDR

#if CONFIG_TEST_I2C_MASTER_MODE
#define I2C_TEST_WIFI_AP_ENABLE         CONFIG_TEST_I2C_WIFI_AP_ENABLE
#define I2C_CLK_FREQUENCY               CONFIG_TEST_I2C_MASTER_FREQUENCY
#define I2C_MASTER_NUM_PORT             CONFIG_TEST_I2C_PORT_NUM
#define I2C_MASTER_TX_BUF_DISABLE       0
#define I2C_MASTER_RX_BUF_DISABLE       0
#define I2C_ACK_CHECK_EN                0x1
#define I2C_ACK_CHECK_DIS               0x0
#else
#define I2C_SLAVE_NUM_PORT              CONFIG_TEST_I2C_PORT_NUM
#define I2C_SLAVE_TX_BUF_LEN            256
#define I2C_SLAVE_RX_BUF_LEN            256
#endif

/* The tests use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define TEST_WIFI_SSID "mywifissid"
*/
static const char* TAG = "I2C-wifi test";

#if I2C_TEST_WIFI_AP_ENABLE
#define TEST_ESP_WIFI_SSID      CONFIG_TEST_WIFI_SSID
#define TEST_ESP_WIFI_PASS      CONFIG_TEST_WIFI_PASSWORD
#define TEST_ESP_WIFI_CHANNEL   CONFIG_TEST_WIFI_CHANNEL
#define TEST_MAX_STA_CONN       CONFIG_TEST_MAX_STA_CONN


static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = TEST_ESP_WIFI_SSID,
            .ssid_len = strlen(TEST_ESP_WIFI_SSID),
            .channel = TEST_ESP_WIFI_CHANNEL,
            .password = TEST_ESP_WIFI_PASS,
            .max_connection = TEST_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(TEST_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d", TEST_ESP_WIFI_SSID, TEST_ESP_WIFI_PASS, TEST_ESP_WIFI_CHANNEL);
}

#endif //I2C_TEST_WIFI_AP_ENABLE

#if CONFIG_TEST_I2C_MASTER_MODE

static esp_timer_handle_t wait_timer;
static uint32_t timer_count = 1000 * 1000;

static esp_err_t i2c_master_init(void)
{
    i2c_port_t i2c_master_port = I2C_MASTER_NUM_PORT;

    i2c_config_t conf_master = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL_GPIO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_CLK_FREQUENCY,
    };

    esp_err_t err = i2c_param_config(i2c_master_port, &conf_master);
    if (err != ESP_OK) {
        return err;
    }

    return i2c_driver_install(i2c_master_port, conf_master.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

// i2c master write
static esp_err_t i2c_master_write_to_slave(uint8_t *data, uint32_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_SLAVE_ADDR << 1) | I2C_MASTER_WRITE, I2C_ACK_CHECK_DIS);
    i2c_master_write(cmd, data, size, I2C_ACK_CHECK_DIS);

    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM_PORT, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

static esp_err_t i2c_data_write(void)
{
    uint8_t data[] = {0x6a, 0x1b, 0x05, 0x1f, 0x1f, 0x08, 0x01, 0x20, 0x19, 0x03, 0x27};

    esp_err_t ret = i2c_master_write_to_slave(data, sizeof(data));
    if(ret != ESP_OK) {
        ESP_LOGW(TAG,"I2C timeout");
    }

    return ret;
}

static void wait_timer_callback(void *arg)
{
    // i2c write
    i2c_data_write();
    /* For local test, please remove this part and give long time testing. */
    timer_count--;
    if (timer_count == 0) {
        esp_timer_stop(wait_timer);
        return;
    }

    // restart timer
    esp_timer_start_once(wait_timer, WAIT_TIMER_PERIOD);
}

static const esp_timer_create_args_t wait_timer_args = {
    .callback = &wait_timer_callback,
    .arg = NULL,
    .name = "wait_timer",
};

#else

static esp_err_t i2c_slave_init(void)
{
    i2c_port_t i2c_slave_port = I2C_SLAVE_NUM_PORT;

    i2c_config_t conf_slave = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = I2C_SDA_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL_GPIO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = I2C_SLAVE_ADDR,

    };

    i2c_param_config(i2c_slave_port, &conf_slave);
    return i2c_driver_install(i2c_slave_port, conf_slave.mode, I2C_SLAVE_RX_BUF_LEN, I2C_SLAVE_TX_BUF_LEN, 0);
}

static void i2c_data_read(void *arg)
{
    printf("slave mode.read thread start...\r\n");
    while (1) {
        uint8_t data[11] = {0};
        i2c_slave_read_buffer(I2C_SLAVE_NUM_PORT, data, sizeof(data), 1000 / portTICK_PERIOD_MS);
        ESP_LOG_BUFFER_HEX(TAG, data, sizeof(data));
        vTaskDelay(1);
    }
}
#endif

// i2c init function
static esp_err_t i2c_init(void)
{
    esp_err_t err = ESP_OK;
#if CONFIG_TEST_I2C_MASTER_MODE
    err = i2c_master_init();
#else
    err = i2c_slave_init();
#endif
    return err;
}

void app_main(void)
{
    esp_err_t ret = i2c_init();

#if I2C_TEST_WIFI_AP_ENABLE
    //Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    wifi_init_softap();
#endif

#if CONFIG_TEST_I2C_MASTER_MODE
    ret = esp_timer_create(&wait_timer_args, &wait_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_timer_create fail!");
    }

    ret = esp_timer_start_once(wait_timer, WAIT_TIMER_PERIOD);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_timer_start_once fail!");
    }
#else
    xTaskCreatePinnedToCore(i2c_data_read, "i2c_data_read", (3 * 1024), NULL, 1, NULL, 1);
#endif
    ESP_ERROR_CHECK(ret);
    printf("I2C-WIFI test success\n");
}
