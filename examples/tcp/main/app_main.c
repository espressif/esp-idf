/* MQTT (over TCP) Example

This example code is in the Public Domain (or CC0 licensed, at your option.)

Unless required by applicable law or agreed to in writing, this
software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied.
*/

/////////////////////////////////        MQTT     ///////////////////////////////////////

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

/////////////////////////////////        UART     ///////////////////////////////////////

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "driver/uart.h"
#include "driver/gpio.h"

/////////////////////////////////        ADC     ///////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

/////////////////////////////////        CLOCK     ///////////////////////////////////////
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_sntp.h"

/////////////////////////////////        MQTT     ///////////////////////////////////////
#if !UART0_ACTIVE
static const char *TAG = "MQTT_EXAMPLE"; //Variable used for debug
#endif

/////////////////////////////////        UART     ///////////////////////////////////////
static const int RX_BUF_SIZE = 1024;

/////////////////////////////////        DATA TO BE UPDATED     ///////////////////////////////////////
char *ciudad="Epitacio Huerta, Michoacan";
char *dispositivo="ESP32";
#define UART0_ACTIVE 1 //if 0, UART0 cannot be used
//END DATA TO BE UPDATED


#if UART0_ACTIVE
#define TXD0_PIN (GPIO_NUM_1)//Used for debug, GPIO for UART0
#define RXD0_PIN (GPIO_NUM_3)
#endif

#define TXD2_PIN (GPIO_NUM_17)  //GPIO for UART2
#define RXD2_PIN (GPIO_NUM_16)

/////////////////////////////////        ADC     ///////////////////////////////////////
//This code for ADC1 was extracted from examples\peripherals\adc\single_read\adc
#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   120          //Multisampling

static esp_adc_cal_characteristics_t *adc_chars;
#if CONFIG_IDF_TARGET_ESP32
static const adc_channel_t channel = ADC_CHANNEL_6;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
#elif CONFIG_IDF_TARGET_ESP32S2
static const adc_channel_t channel = ADC_CHANNEL_6;     // GPIO7 if ADC1, GPIO17 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_13;
#endif
/*
 * ADC_ATTEN_DB_0: gives full scale with voltage=1.1V
 * ADC_ATTEN_DB_2_5: up Voltage =1.5V
 * ADC_ATTEN_DB_6: up Voltage=2.2V
 * ADC_ATTEN_DB_11: up Voltage=3.9V
 * */

//I selected this attenuation because ZE25 give us an output from 0.4 to 2V
static const adc_atten_t atten = ADC_ATTEN_DB_6;
static const adc_unit_t unit = ADC_UNIT_1;

/////////////////////////////////        FOR UART     ///////////////////////////////////////
//uint8_t
char petition[] = {0xFF,0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};// Petition to get a single result
char measure[8]={0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};// Space for the response
char setConfig[] = {0xFF, 0x01, 0x78, 0x04, 0x00, 0x00, 0x00, 0x00, 0x83};//QA config
char response[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//Sensor configuration for read
typedef enum {
	QA,
	ACTIVE
}bool_t;

typedef enum {
	SO2,
	NO2
}sensor_t;

/////////////////////////////////        ADC     ///////////////////////////////////////
uint8_t output_data=0;
int     read_raw;
esp_err_t r;

gpio_num_t adc_gpio_num, dac_gpio_num;

void ReadPPM(uint8_t* param, esp_mqtt_client_handle_t client, sensor_t type_sensor);
void ftoa(float n, char* res, int afterpoint);
void setAs(bool_t mode);
void setup_sensor(void);

/////////////////////////////////        CLOCK     ///////////////////////////////////////
//The code for GETDATE was taken from examples\protocols\sntp as base.
static void obtain_time(void);
static void initialize_sntp(void);

static void obtain_time(void)
{
    initialize_sntp();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
#if !UART0_ACTIVE
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
#endif
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    time(&now);
    localtime_r(&now, &timeinfo);
}
void time_sync_notification_cb(struct timeval *tv)
{
#if !UART0_ACTIVE
    ESP_LOGI(TAG, "Notification of a time synchronization event");
#endif
}

static void initialize_sntp(void)
{
#if !UART0_ACTIVE
    ESP_LOGI(TAG, "Initializing SNTP");
#endif
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();
}


void getDate(esp_mqtt_client_handle_t client){

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2016 - 1900)) {
#if !UART0_ACTIVE
        ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
#endif
        obtain_time();
        // update 'now' variable with current time
        time(&now);
    }

    char strftime_buf[64];

    // Set timezone to Central Time and print local time
    setenv("TZ", "CST6CDT,M3.2.0,M11.1.0", 1); //Currently is set as Central Time.
    tzset();
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    //ESP_LOGI(TAG, "The current date/time in New York is: %s", strftime_buf);

    esp_mqtt_client_publish(client, "SMART-Air-Qro/Est-Epitacio/Fecha", strftime_buf, 0, 1, 0);
}

/////////////////////////////////        UART     ///////////////////////////////////////
void setAs(bool_t active){

	if(active){
	    setConfig[3] =0x03; //ACTIVE
	    setConfig[8] =0x84;
	}

}

void init_sensor(void) {
#if UART0_ACTIVE
    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
#endif

    const uart_config_t uart_config2 = {
            .baud_rate = 9600,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_APB,
        };

	/////////////////////////DEVICE IN UART 0 //////////////////////
    // We won't use a buffer for sending data.
#if UART0_ACTIVE
    uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, TXD0_PIN, RXD0_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
#endif

	/////////////////////////DEVICE IN UART 2 //////////////////////
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_2, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_2, &uart_config2);
    uart_set_pin(UART_NUM_2, TXD2_PIN, RXD2_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

int sendData(const char* logName,  char* data, int UART_NUM)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_NUM, data, len);
#if !UART0_ACTIVE
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
#endif
    return txBytes;
}

//This function is to setup our sensor to Q&A, we send the command and validate the response.
void cal_func(const char *ZE03, int UART_NUM){
	/////// CONFIG    ZE03+++++++++++++++++++++++
	    setAs(QA);
#if !UART0_ACTIVE
	    esp_log_level_set(ZE03, ESP_LOG_INFO);
#endif
	    sendData(ZE03, setConfig, UART_NUM);			//Send to TX, change to QA.

	    vTaskDelay(2000 / portTICK_PERIOD_MS);  //Wait for response
	    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
	    uint8_t flag=0, retries=0;
	    int rxBytes = uart_read_bytes(UART_NUM, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);

	    do {
	    	if(retries!=0){
	    rxBytes = uart_read_bytes(UART_NUM, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
	    	}

	    if (rxBytes > 0) {
	    		flag=1;
	          data[rxBytes] = 0;
#if !UART0_ACTIVE
	          ESP_LOGI(ZE03, "Read %d bytes: '%s'", rxBytes, data);
	          ESP_LOG_BUFFER_HEXDUMP(ZE03, data, rxBytes, ESP_LOG_INFO);
#endif
	          if ((data[2]==0)||(data[2]==0)){
#if !UART0_ACTIVE
	        	  ESP_LOGI(ZE03, "Setup QA Successfully, UART: %d", UART_NUM);
#endif
	          }
	          else{
#if !UART0_ACTIVE
	        	  ESP_LOGI(ZE03, "Wrong setup QA, UART: %d", UART_NUM);
#endif
	          }
	     }
	    retries++;
	    }while(flag == 0);

	    free(data);
}

//Setup QA configuration in every UART
void setup_sensor(void){
	/////// CONFIG    ZE03+++++++++++++++++++++++
#if UART0_ACTIVE
	    static const char *ZE03_UART0 = "ZE03_UART0 Config";
	    cal_func(ZE03_UART0, UART_NUM_0);
#endif
	    static const char *ZE03_UART2 = "ZE03_UART2 Config";
	    cal_func(ZE03_UART2, UART_NUM_2);

}

/////////////////////////////////        FUNCTIONS FOR TASKS     ///////////////////////////////////////
static void tx_func( const char *TX_TASK_TAG, int UART_NUM)
{
#if !UART0_ACTIVE
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
#endif
    while (1) {
    	sendData(TX_TASK_TAG, petition, UART_NUM); //Send petition for meassure
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

 static void rx_func(  const char *RX_TASK_TAG, int UART_NUM, esp_mqtt_client_handle_t client, sensor_t type_sensor)
{
#if !UART0_ACTIVE
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
#endif
    uint8_t* data = (uint8_t*) calloc((RX_BUF_SIZE+1)/sizeof(uint8_t),sizeof(uint8_t));
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
#if !UART0_ACTIVE
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
#endif
            ReadPPM(data, client, type_sensor);
        }
    }
    free(data);
}


/////////////////////////DEVICE IN UART 0 //////////////////////
#if UART0_ACTIVE
static void tx0_task(void *arg)
{
    static const char *TX_TASK_TAG = "TX0_TASK";
    tx_func(TX_TASK_TAG, UART_NUM_0);

}


static void rx0_task(esp_mqtt_client_handle_t client)
{
    static const char *RX_TASK_TAG = "RX0_TASK";
    rx_func(RX_TASK_TAG, UART_NUM_0, client, NO2);

}
#endif

/////////////////////////DEVICE IN UART 2 //////////////////////
static void tx2_task(void *arg)
{
    static const char *TX_TASK_TAG = "TX2_TASK";
    tx_func(TX_TASK_TAG, UART_NUM_2);

}


static void rx2_task(esp_mqtt_client_handle_t client)
{
    static const char *RX_TASK_TAG = "RX2_TASK";
    rx_func(RX_TASK_TAG, UART_NUM_2, client, SO2);

}

///////////////////////////////////FUNCTIONS FOR ADC///////////////////////////////////


static void check_efuse(void)
{
#if CONFIG_IDF_TARGET_ESP32
    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
#if !UART0_ACTIVE
        printf("eFuse Two Point: Supported\n");
    }
    else {
        printf("eFuse Two Point: NOT supported\n");
#endif
    }

    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
#if !UART0_ACTIVE
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
#endif
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
#if !UART0_ACTIVE
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("Cannot retrieve eFuse Two Point calibration values. Default calibration values will be used.\n");
#endif
    }
#else
#error "This example is configured for ESP32/ESP32S2."
#endif
}


static void print_char_val_type(esp_adc_cal_value_t val_type)
{
#if !UART0_ACTIVE
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
#endif
}

void init_adc(void){

    //Check if Two Point or Vref are burned into eFuse
    check_efuse();

    //Configure ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(width);
        adc1_config_channel_atten(channel, atten);
    } else {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }

    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);
}

static void adc_task(esp_mqtt_client_handle_t client)
{
    static const char *ADC_TASK_TAG = "ADC_TASK";
#if !UART0_ACTIVE
    ESP_LOGE(ADC_TASK_TAG,"START CONVERSION\n");
#endif
    while(1) {
        uint32_t adc_reading = 0;
        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            if (unit == ADC_UNIT_1) {
                adc_reading += adc1_get_raw((adc1_channel_t)channel);
            } else {
                int raw;
                adc2_get_raw((adc2_channel_t)channel, width, &raw);
                adc_reading += raw;
            }
        }
        adc_reading /= NO_OF_SAMPLES; //Here a function to convert to ppm needs to apply to adc_reading
        float voltage_reading=0, ppm_reading=0;
        voltage_reading=(2.0/4095)*adc_reading; //Obtain voltage reading considering voltage max=2.0V
        ppm_reading=6.25*voltage_reading-2.5;   //Equation considering that ZE25: 0.4-2 Vout and 0-10ppm

        //Convert adc_reading to voltage in mV
        //Need to verify if this function give us the voltage correctly
        //uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
#if !UART0_ACTIVE
        ESP_LOGE(ADC_TASK_TAG,"Raw: %d, voltage_reading:%f  , ppm_reading:%f\t\n", adc_reading, voltage_reading, ppm_reading);
#endif
        vTaskDelay(pdMS_TO_TICKS(1000));

    	char a[20];
    	ftoa(ppm_reading,a,1);
        esp_mqtt_client_publish(client, "SMART-Air-Qro/Est-Epitacio/Sensores/ZE25/O3", a, 0, 1, 0);
    }
}

/////////////////////////////////MQTT     ///////////////////////////////////////

static void log_error_if_nonzero(const char *message, int error_code)
{
#if !UART0_ACTIVE
if (error_code != 0) {
ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
}
#endif
}

/*
* @brief Event handler registered to receive MQTT events
*
* This function is called by the MQTT client event loop.
*
* @param handler_args user data registered to the event.
* @param base Event base for the handler(always MQTT Base in this example).
* @param event_id The id for the received event.
* @param event_data The data for the event, esp_mqtt_event_handle_t.
*/
int msg_id;
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
#if !UART0_ACTIVE
ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
#endif
esp_mqtt_event_handle_t event = event_data;
esp_mqtt_client_handle_t client = event->client;
//int msg_id;
switch ((esp_mqtt_event_id_t)event_id) {
case MQTT_EVENT_CONNECTED:
#if !UART0_ACTIVE
ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
//msg_id = esp_mqtt_client_publish(client, "SMART-Air-Qro/EST-Diana/Prueba/Dato", "medida2", 0, 1, 0);
ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
#endif

msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
#if !UART0_ACTIVE
ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
#endif

msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
#if !UART0_ACTIVE
ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
#endif

msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
#if !UART0_ACTIVE
ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
break;
case MQTT_EVENT_DISCONNECTED:
ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
break;

case MQTT_EVENT_SUBSCRIBED:
ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
//msg_id = esp_mqtt_client_publish(client, "SMART-Air-Qro/EST-Diana/Ubicacion", "Epitacio Huerta", 0, 0, 0);
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
printf("DATA=%.*s\r\n", event->data_len, event->data);
break;
case MQTT_EVENT_ERROR:
ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
log_error_if_nonzero("captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
}
#endif
break;
default:
#if !UART0_ACTIVE
ESP_LOGI(TAG, "Other event id:%d", event->event_id);
#endif
break;
}
}

static esp_mqtt_client_handle_t mqtt_app_start(void)
{
esp_mqtt_client_config_t mqtt_cfg = {
.uri = CONFIG_BROKER_URL, .username = "DianaV", .password = "diaz",
};
#if CONFIG_BROKER_URL_FROM_STDIN
char line[128];

if (strcmp(mqtt_cfg.uri, "FROM_STDIN") == 0) {
int count = 0;
#if !UART0_ACTIVE
printf("Please enter url of mqtt broker\n");
#endif
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
#if !UART0_ACTIVE
printf("Broker url: %s\n", line);
#endif
} else {
#if !UART0_ACTIVE
ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
#endif
abort();
}
#endif  CONFIG_BROKER_URL_FROM_STDIN */

esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
//The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
esp_mqtt_client_start(client);

msg_id=esp_mqtt_client_publish(client, "SMART-Air-Qro/Est-Epitacio/Ubicacion", ciudad, 0, 1, 0);
#if !UART0_ACTIVE
ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
#endif
msg_id=esp_mqtt_client_publish(client, "SMART-Air-Qro/Est-Epitacio/Dispositivo", dispositivo, 0, 1, 0);

/*char dispositivo[100];
char *str1="ESP32_";*/
//char *client_id=client->connect_info->client_id;
//mqtt_connect_info_t connect_info=client->connect_info;
/*strcat(strcpy(dispositivo, str1),client_id );
msg_id=esp_mqtt_client_publish(client, "SMART-Air-Qro/Est-Epitacio/Dispositivo", dispositivo, 0, 1, 0);
ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);*/

return client;
}

/////////////////////////////////        MAIN     ///////////////////////////////////////
void app_main(void)
{
ESP_LOGI(TAG, "[APP] Startup..");
ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

#if !UART0_ACTIVE
esp_log_level_set("*", ESP_LOG_INFO);
esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);
#else
esp_log_level_set("*", ESP_LOG_NONE);
esp_log_level_set("MQTT_CLIENT", ESP_LOG_NONE);
esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_NONE);
esp_log_level_set("TRANSPORT_BASE", ESP_LOG_NONE);
esp_log_level_set("esp-tls", ESP_LOG_NONE);
esp_log_level_set("TRANSPORT", ESP_LOG_NONE);
esp_log_level_set("OUTBOX", ESP_LOG_NONE);
esp_log_level_set("*", ESP_LOG_NONE);
#endif

ESP_ERROR_CHECK(nvs_flash_init());
ESP_ERROR_CHECK(esp_netif_init());
ESP_ERROR_CHECK(esp_event_loop_create_default());

/* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
* Read "Establishing Wi-Fi or Ethernet Connection" section in
* examples/protocols/README.md for more information about this function.
*/

ESP_ERROR_CHECK(example_connect());

esp_mqtt_client_handle_t client=mqtt_app_start();

getDate(client);

init_sensor();
init_adc();

setup_sensor();

#if UART0_ACTIVE
	//This tasks is for UART0, for the NO2 sensor
    xTaskCreate(rx0_task, "uart_rx0_task", 1024*2, client, configMAX_PRIORITIES, NULL);
    xTaskCreate(tx0_task, "uart_tx0_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
#else
printf("TASKS\n");
#endif
	//This tasks is for UART2, for the SO2 sensor. We can change that inside the tasks
    xTaskCreate(rx2_task, "uart_rx2_task", 1024*2, client, configMAX_PRIORITIES, NULL);
    xTaskCreate(tx2_task, "uart_tx2_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
    //This adc task is for O3 sensor
    xTaskCreate(adc_task, "adc_task", 1024*2, client, configMAX_PRIORITIES-1, NULL);
}



/////////////////////////////////        ReadPPM     ///////////////////////////////////////
//All this code is for ReadPPM function. ReadPPM function is used for the extraction of PPM value
//into the response of the sensor. We implemented all this functions to made the ftoa function. In order to
//have a better result than itoa.
///////////// C program for implementation of ftoa() used for ReadPPM function.
#include <math.h>
//#include <stdio.h>

// Reverses a string 'str' of length 'len'
void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}
// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

// Converts a floating-point/double number to a string.
void ftoa(float n, char* res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0) {
        res[i] = '.'; // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

void ReadPPM(uint8_t* param, esp_mqtt_client_handle_t client, sensor_t type_sensor){
	float HighCon=*(param+3);
	float LowCon=*(param+4);
#if !UART0_ACTIVE
	ESP_LOGI("ReadPPM", "HighCon=%f", HighCon);
	ESP_LOGI("ReadPPM", "LowCon=%f", LowCon);
#endif
	float ppm=(HighCon*256+LowCon)*0.1;  //For SO2/NO2/O3, check datasheet for other gas
#if !UART0_ACTIVE
	ESP_LOGI(TAG, "DATO ppm=%f", ppm);
#endif
	char a[20];
	//itoa(ppm,a,10);
	ftoa(ppm,a,2);

	//We publish the value depending of the type of sensor. Remember: UART2:SO2, UART0:NO2
	char string[100];
	if (type_sensor==SO2){
		strcat(strcpy(string, "SMART-Air-Qro/Est-Epitacio/Sensores/ZE03/"),"SO2");
	}else if (type_sensor==NO2){
		strcat(strcpy(string, "SMART-Air-Qro/Est-Epitacio/Sensores/ZE03/"),"NO2");

	}

	msg_id=esp_mqtt_client_publish(client,string, a, 0, 1, 0);
#if !UART0_ACTIVE
	ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
#endif
}
