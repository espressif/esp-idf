/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
/*#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "freertos/queue.h"*

 * This is an example which echos any data it receives on configured UART back to the sender,
 * with hardware flow control turned off. It does not use UART driver event queue.
 *
 * - Port: configured UART
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below (See Kconfig)
 */


/*
#define ECHO_TEST_TXD (CONFIG_EXAMPLE_UART_TXD)
#define ECHO_TEST_RXD (CONFIG_EXAMPLE_UART_RXD)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_PORT_NUM      (CONFIG_EXAMPLE_UART_PORT_NUM)
#define ECHO_UART_BAUD_RATE     (CONFIG_EXAMPLE_UART_BAUD_RATE)
#define ECHO_TASK_STACK_SIZE    (CONFIG_EXAMPLE_TASK_STACK_SIZE)

#define BUF_SIZE (1024)

static void echo_task(void *arg)
{
    // Configure parameters of an UART driver,
     // communication pins and install the driver
    uart_config_t uart_config = {
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        // Write data back to the UART
        uart_write_bytes(ECHO_UART_PORT_NUM, (const char *) data, len);
    }
}

void app_main(void)
{
    xTaskCreate(echo_task, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
}
*/


/****************** YOUTUBE++++***************/
/*
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "freertos/queue.h"

#define UART_NUM  UART_NUM_2
#define TXD_PIN		(GPIO_NUM_17)
#define RXD_PIN		(GPIO_NUM_16)
#define BUF_SIZE	(1024)
#define RD_BUF_SIZE	(BLF_SIZE)
#define QUEUE_SIZE	(20)

static QueueHandle_t uart2_queue;

static void uartT_task(void *p){
uart_event_t event;
uint8_t *dtemp=(uint8_t *)malloc(RD_BUF_SIZE);

}

void app_main(void)
{
    //esp_log_level_set(TAG, ESP_LOG_INFO);

     Configure parameters of an UART driver,
      communication pins and install the driver
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    //Install UART driver, and get the queue.
    uart_driver_install(UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart2_queue, 0);
    uart_param_config(UART_NUM, &uart_config);

    //Set UART pins (using UART0 default pins ie no changes.)
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    //Tasks
    xtaskCreate(uartT_task, "uart_T",1024,NULL );
    xtaskCreate(uartR_task, "uart_R",1024,NULL );


}*/

/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

static const int RX_BUF_SIZE = 1024;

#define TXD0_PIN (GPIO_NUM_1)
#define RXD0_PIN (GPIO_NUM_3)

#define TXD2_PIN (GPIO_NUM_17)
#define RXD2_PIN (GPIO_NUM_16)

#define CO 1
#define SO2 2
#define NO2 2
#define O2 2
#define NH3 1
#define H2S 1
#define HF 1
#define CL2 2
#define O3 2

//#define QA false
//#define ACTIVE true

//uint8_t
char petition[] = {0xFF,0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};// Petition to get a single result
char measure[8]={0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};// Space for the response
char setConfig[] = {0xFF, 0x01, 0x78, 0x04, 0x00, 0x00, 0x00, 0x00, 0x83};//QA config
char response[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

typedef enum {
	QA,
	ACTIVE
}bool_t;

void setAs(bool_t mode);

void setAs(bool_t active){

	if(active){
	    setConfig[3] =0x03; //ACTIVE
	    setConfig[8] =0x84;
	}

}

void init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 9600,//9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    /*const uart_config_t uart_config2 = {
            .baud_rate = 9600,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .source_clk = UART_SCLK_APB,
        };*/

	/////////////////////////DEVICE IN UART 0 //////////////////////
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, TXD0_PIN, RXD0_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

	/////////////////////////DEVICE IN UART 2 //////////////////////
    // We won't use a buffer for sending data.
   /* uart_driver_install(UART_NUM_2, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_2, &uart_config2);
    uart_set_pin(UART_NUM_2, TXD0_PIN, RXD0_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);*/
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
	        	  ESP_LOGI(ZE03, "Calibration Successfully, UART: %d", UART_NUM);
	          }
	          else{
	        	  ESP_LOGI(ZE03, "Wrong Calibration, UART: %d", UART_NUM);
	          }
	     }
	    retries++;
	    }while(flag == 0);

	    free(data);
}

void calibration(void){
	/////// CONFIG    ZE03+++++++++++++++++++++++
	    static const char *ZE03_UART0 = "ZE03_UART0 Config";
	    cal_func(ZE03_UART0, UART_NUM_0);
	    static const char *ZE03_UART2 = "ZE03_UART2 Config";
	    cal_func(ZE03_UART2, UART_NUM_1);

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

/////////////////////////DEVICE IN UART 2 //////////////////////
static void tx2_task(void *arg)
{
    static const char *TX_TASK_TAG = "TX2_TASK";
    tx_func(TX_TASK_TAG, UART_NUM_1);

}


static void rx2_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX2_TASK";
    rx_func(RX_TASK_TAG, UART_NUM_1);

}



void app_main(void)
{
	printf("INIT\n");
    init();
    printf("CALIBRATION\n");
    //calibration();
    printf("TASKS\n");
    /*xTaskCreate(rx0_task, "uart_rx0_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(tx0_task, "uart_tx0_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);*/
    /*xTaskCreate(rx2_task, "uart_rx2_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(tx2_task, "uart_tx2_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);*/
}

