//UART
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

//ADC
#include <stdio.h>
#include <stdlib.h>
#include "freertos/queue.h"
#include "driver/adc.h"
#include "driver/dac.h"

//UART
static const int RX_BUF_SIZE = 1024;

#define UART0_ACTIVE 0 //if 0, UART0 cannot be used

#if UART0_ACTIVE
#define TXD0_PIN (GPIO_NUM_1)//Used for debug
#define RXD0_PIN (GPIO_NUM_3)
#endif

#define TXD2_PIN (GPIO_NUM_17)
#define RXD2_PIN (GPIO_NUM_16)

//ADC
#define ADC2_EXAMPLE_CHANNEL    0
/*!< ADC1 channel 7 is GPIO35 */
/*!< ADC2 channel 7 is GPIO27 (ESP32), GPIO18 (ESP32-S2) */

#if CONFIG_IDF_TARGET_ESP32
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
#elif CONFIG_IDF_TARGET_ESP32S2
static const adc_bits_width_t width = ADC_WIDTH_BIT_13;
#endif

//UART
char petition[] = {0xFF,0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};// Petition to get a single result
char measure[8]={0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};// Space for the response
char setConfig[] = {0xFF, 0x01, 0x78, 0x04, 0x00, 0x00, 0x00, 0x00, 0x83};//QA config
char response[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

typedef enum {
	QA,
	ACTIVE
}bool_t;

//ADC
uint8_t output_data=0;
int     read_raw;
esp_err_t r;

gpio_num_t adc_gpio_num, dac_gpio_num;

///////////////////////////////////FUNCTIONS FOR UART///////////////////////////////////

void setAs(bool_t mode);

void setAs(bool_t active){

	if(active){
	    setConfig[3] =0x03; //ACTIVE
	    setConfig[8] =0x84;
	}

}

void init(void) {
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
    uart_set_pin(UART_NUM_0, TXD2_PIN, RXD2_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
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
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    return txBytes;
}

void cal_func(const char *ZE03, int UART_NUM){
	/////// CONFIG    ZE03+++++++++++++++++++++++
	    setAs(QA);
	    esp_log_level_set(ZE03, ESP_LOG_INFO);
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
	          ESP_LOGI(ZE03, "Read %d bytes: '%s'", rxBytes, data);
	          ESP_LOG_BUFFER_HEXDUMP(ZE03, data, rxBytes, ESP_LOG_INFO);
	          if ((data[2]==0)||(data[2]==0)){
	        	  ESP_LOGI(ZE03, "Setup QA Successfully, UART: %d", UART_NUM);
	          }
	          else{
	        	  ESP_LOGI(ZE03, "Wrong setup QA, UART: %d", UART_NUM);
	          }
	     }
	    retries++;
	    }while(flag == 0);

	    free(data);
}

void setup(void){
	/////// CONFIG    ZE03+++++++++++++++++++++++
#if UART0_ACTIVE
	    static const char *ZE03_UART0 = "ZE03_UART0 Config";
	    cal_func(ZE03_UART0, UART_NUM_0);
#endif
	    static const char *ZE03_UART2 = "ZE03_UART2 Config";
	    cal_func(ZE03_UART2, UART_NUM_2);

}

/////////////////////////FUNCTIONS FOR TASKS //////////////////////
static void tx_func( const char *TX_TASK_TAG, int UART_NUM)
{
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    while (1) {
    	sendData(TX_TASK_TAG, petition, UART_NUM); //Send petition for meassure
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

 static void rx_func(  const char *RX_TASK_TAG, int UART_NUM)
{
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM, data, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
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


static void rx0_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX0_TASK";
    rx_func(RX_TASK_TAG, UART_NUM_0);

}
#endif

/////////////////////////DEVICE IN UART 2 //////////////////////
static void tx2_task(void *arg)
{
    static const char *TX_TASK_TAG = "TX2_TASK";
    tx_func(TX_TASK_TAG, UART_NUM_2);

}


static void rx2_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX2_TASK";
    rx_func(RX_TASK_TAG, UART_NUM_2);

}

///////////////////////////////////FUNCTIONS FOR ADC///////////////////////////////////

void init_adc(void){

    static const char *ADC_TASK_TAG = "ADC_TASK";

    r = adc2_pad_get_io_num( ADC2_EXAMPLE_CHANNEL, &adc_gpio_num );
    assert( r == ESP_OK );

    ESP_LOGI(ADC_TASK_TAG, "ADC2 channel %d @ GPIO %d\n", ADC2_EXAMPLE_CHANNEL, adc_gpio_num);

    //be sure to do the init before using adc2.
    ESP_LOGI(ADC_TASK_TAG, "adc2_init...\n");
    adc2_config_channel_atten( ADC2_EXAMPLE_CHANNEL, ADC_ATTEN_11db );

    vTaskDelay(2 * portTICK_PERIOD_MS);
}

static void adc_task(void *arg)
{
    static const char *ADC_TASK_TAG = "ADC_TASK";
    printf("start conversion.\n");
    while(1) {
        r = adc2_get_raw( ADC2_EXAMPLE_CHANNEL, width, &read_raw);
        if ( r == ESP_OK ) {
        	ESP_LOGI(ADC_TASK_TAG,"%d\n", read_raw );
        } else if ( r == ESP_ERR_INVALID_STATE ) {
        	ESP_LOGI(ADC_TASK_TAG,"%s: ADC2 not initialized yet.\n", esp_err_to_name(r));
        } else if ( r == ESP_ERR_TIMEOUT ) {
            //This can not happen in this example. But if WiFi is in use, such error code could be returned.
        	ESP_LOGI(ADC_TASK_TAG,"%s: ADC2 is in use by Wi-Fi.\n", esp_err_to_name(r));
        } else {
        	ESP_LOGI(ADC_TASK_TAG,"%s\n", esp_err_to_name(r));
        }

        vTaskDelay( 5 * portTICK_PERIOD_MS );
    }
}

void app_main(void)
{
	printf("INIT\n");
    init();
    printf("SETUP QA\n");
    setup();
    init_adc();
    printf("TASKS\n");
#if UART0_ACTIVE
    xTaskCreate(rx0_task, "uart_rx0_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(tx0_task, "uart_tx0_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
#endif

    xTaskCreate(rx2_task, "uart_rx2_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(tx2_task, "uart_tx2_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
    xTaskCreate(adc_task, "adc_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
}

