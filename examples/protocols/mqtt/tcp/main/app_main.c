/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "driver/uart.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "cJSON.h"

#define TXD_PIN (GPIO_NUM_15)
#define RXD_PIN (GPIO_NUM_2)
#define STX 0XF7
#define SWITCHID 0X03
#define CURTAINSWITCHID 0X06
#define CONTROLCOMMAND 0X41
#define STATUSCOMMAND 0XC1
#define ALLSTATUSCOMMAND 0X81
#define CMD_LEN 0x01
#define SETONE_LEN 8
#define SETALL_LEN 16
#define SWITCHCOMMAND_LEN (8)

static const char *TAG = "MQTT_EXAMPLE";
static const int RX_BUF_SIZE = 1024;
static uint8_t switchcmd_template[SWITCHCOMMAND_LEN];

void init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}
void curtainuartcmd(uint8_t subid, uint8_t cmd)
{
   // MDF_LOGI("Curtain operation");
 
    switchcmd_template[0] = STX;             //stx
    switchcmd_template[1] = SWITCHID; //switch id
    switchcmd_template[2] = subid;           //subid
    switchcmd_template[3] = CONTROLCOMMAND;  //command
    switchcmd_template[4] = CMD_LEN;         //length 0X01
    switchcmd_template[5] = cmd;      
           //data or switch command on/off/percentage
    switchcmd_template[6] = switchcmd_template[0] ^
                            switchcmd_template[1] ^
                            switchcmd_template[2] ^
                            switchcmd_template[3] ^
                            switchcmd_template[4] ^
                            switchcmd_template[5]; //xor
    switchcmd_template[7] = switchcmd_template[0] +
                            switchcmd_template[1] +
                            switchcmd_template[2] +
                            switchcmd_template[3] +
                            switchcmd_template[4] +
                            switchcmd_template[5] +
                            switchcmd_template[6]; //addsum

printf("command:");
    for(int i=0;i<=7;i++)
    {                   
     //uart_write_bytes(UART_NUM_1, (const char *)switchcmd_template[i], SWITCHCOMMAND_LEN);
     printf("%d,",switchcmd_template[i]);

    

        }
    uart_write_bytes(UART_NUM_1, (const char *)switchcmd_template, SWITCHCOMMAND_LEN);
    

   
   
}
static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id=0;
    //gpio_set_direction(GPIO_NUM_2,GPIO_MODE_OUTPUT);
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client,"esp32/led",0);
        esp_mqtt_client_publish(client, "esp32/led", "", 0, 1, 0);
        
        
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        
        ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        
        printf("DATA=%.*s\r\n",event->data_len,event->data);
        
        


        cJSON *root2 = cJSON_Parse(event->data);
	if (cJSON_GetObjectItem(root2, "sub_id")) {
		int sub_id = cJSON_GetObjectItem(root2,"sub_id")->valueint;
		
       
        
        
	
	if (cJSON_GetObjectItem(root2, "cmd")) {
		int cmd = cJSON_GetObjectItem(root2,"cmd")->valueint;
		
       curtainuartcmd(sub_id,cmd);
    } 
    }
	
        
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_BROKER_URL,
    };
#if CONFIG_BROKER_URL_FROM_STDIN
    char line[128];

    if (strcmp(mqtt_cfg.uri, "FROM_STDIN") == 0) {
        int count = 0;
        printf("Please enter url of mqtt broker\n");
        while (count < 128) {
            int c = fgetc(stdin);
            if (c == '\n') {
                line[count] = '\0';
                break;
            } else if (c > 0 && c < 127) {
                line[count] = c;
                ++count;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        mqtt_cfg.uri = line;
        printf("Broker url: %s\n", line);
    } else {
        ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
        abort();
    }
#endif /*CONFIG_BROKER_URL_FROM_STDIN*/ 

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void app_main(void)
{
     
    init();
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    
    ESP_ERROR_CHECK(example_connect());
    
    mqtt_app_start();
    

}
