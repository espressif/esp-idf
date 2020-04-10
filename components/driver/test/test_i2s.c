/**
 * I2S test environment UT_T1_I2S:
 * We use internal signals instead of external wiring, but please keep the following IO connections, or connect nothing to prevent the signal from being disturbed.
 * connect GPIO15 and GPIO19, GPIO25(ESP32)/GPIO17(ESP32-S2) and GPIO26, GPIO21 and GPIO22(ESP32)/GPIO20(ESP32-S2)
 * Please do not connect GPIO32(ESP32) any pull-up resistors externally, it will be used to test i2s adc function.
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "unity.h"
#include "math.h"

#define SAMPLE_RATE     (36000)
#define SAMPLE_BITS     (16)
#define MASTER_BCK_IO 15
#define SLAVE_BCK_IO 19
#define SLAVE_WS_IO 26
#define DATA_IN_IO 21

#if CONFIG_IDF_TARGET_ESP32
#define MASTER_WS_IO 25
#define DATA_OUT_IO 22
#define ADC1_CHANNEL_4_IO 32
#elif CONFIG_IDF_TARGET_ESP32S2
#define MASTER_WS_IO 28
#define DATA_OUT_IO 20
#endif

#define PERCENT_DIFF 0.0001

#define I2S_TEST_MODE_SLAVE_TO_MAXTER 0
#define I2S_TEST_MODE_MASTER_TO_SLAVE 1
#define I2S_TEST_MODE_LOOPBACK        2

// mode: 0, master rx, slave tx. mode: 1, master tx, slave rx. mode: 2, master tx rx loopback
// Since ESP32-S2 has only one I2S, only loop back test can be tested.
static void i2s_test_io_config(int mode)
{
    // Connect internal signals using IO matrix.
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[MASTER_BCK_IO], PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[MASTER_WS_IO], PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[DATA_OUT_IO], PIN_FUNC_GPIO);

    gpio_set_direction(MASTER_BCK_IO, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(MASTER_WS_IO, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(DATA_OUT_IO, GPIO_MODE_INPUT_OUTPUT);

    switch (mode) {
#if SOC_I2S_NUM > 1
        case I2S_TEST_MODE_SLAVE_TO_MAXTER: {
            gpio_matrix_out(MASTER_BCK_IO, I2S0I_BCK_OUT_IDX, 0, 0);
            gpio_matrix_in(MASTER_BCK_IO, I2S1O_BCK_IN_IDX, 0);

            gpio_matrix_out(MASTER_WS_IO, I2S0I_WS_OUT_IDX, 0, 0);
            gpio_matrix_in(MASTER_WS_IO, I2S1O_WS_IN_IDX, 0);

            gpio_matrix_out(DATA_OUT_IO, I2S1O_DATA_OUT23_IDX, 0, 0);
            gpio_matrix_in(DATA_OUT_IO, I2S0I_DATA_IN15_IDX, 0);  
        }
        break;

        case I2S_TEST_MODE_MASTER_TO_SLAVE: {
            gpio_matrix_out(MASTER_BCK_IO, I2S0O_BCK_OUT_IDX, 0, 0);
            gpio_matrix_in(MASTER_BCK_IO, I2S1I_BCK_IN_IDX, 0);

            gpio_matrix_out(MASTER_WS_IO, I2S0O_WS_OUT_IDX, 0, 0);
            gpio_matrix_in(MASTER_WS_IO, I2S1I_WS_IN_IDX, 0);

            gpio_matrix_out(DATA_OUT_IO, I2S0O_DATA_OUT23_IDX, 0, 0);
            gpio_matrix_in(DATA_OUT_IO, I2S1I_DATA_IN15_IDX, 0);  
        }
        break;
#endif
        case I2S_TEST_MODE_LOOPBACK: {
            gpio_matrix_out(DATA_OUT_IO, I2S0O_DATA_OUT23_IDX, 0, 0);
            gpio_matrix_in(DATA_OUT_IO, I2S0I_DATA_IN15_IDX, 0); 
        }
        break;

        default: {
            TEST_FAIL_MESSAGE("error: mode not supported");
        }
        break;
    }
}


/**
 * i2s initialize test
 * 1. i2s_driver_install
 * 2. i2s_set_pin
 */
TEST_CASE("I2S basic driver install, uninstall, set pin test", "[i2s]")
{
    // dac, adc  i2s
    i2s_config_t  i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_buf_count = 6,
        .dma_buf_len = 60,
        .use_apll = 0,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1 ,
    };

#if CONFIG_IDF_TARGET_ESP32
    //install and start i2s driver
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL));
    //for internal DAC, this will enable both of the internal channels
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, NULL));
    //stop & destroy i2s driver
    TEST_ESP_OK(i2s_driver_uninstall(I2S_NUM_0));
#endif

    // normal  i2s
    i2s_pin_config_t pin_config = {
        .bck_io_num = MASTER_BCK_IO,
        .ws_io_num = MASTER_WS_IO,
        .data_out_num = DATA_OUT_IO,
        .data_in_num = -1
    };
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &pin_config));
    TEST_ESP_OK(i2s_driver_uninstall(I2S_NUM_0));

    //error param test
    TEST_ASSERT(i2s_driver_install(I2S_NUM_MAX, &i2s_config, 0, NULL) == ESP_ERR_INVALID_ARG);
    TEST_ASSERT(i2s_driver_install(I2S_NUM_0, NULL, 0, NULL) == ESP_ERR_INVALID_ARG);
    i2s_config.dma_buf_count = 1;
    TEST_ASSERT(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) == ESP_ERR_INVALID_ARG);
    i2s_config.dma_buf_count = 129;
    TEST_ASSERT(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) == ESP_ERR_INVALID_ARG);
    TEST_ESP_OK(i2s_driver_uninstall(I2S_NUM_0));
}

TEST_CASE("I2S Loopback test(master tx and rx)", "[i2s]")
{
    // master driver installed and send data
    i2s_config_t master_i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_buf_count = 6,
        .dma_buf_len = 100,
        .use_apll = 0,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1 ,
    };
    i2s_pin_config_t master_pin_config = {
        .bck_io_num = MASTER_BCK_IO,
        .ws_io_num = MASTER_WS_IO,
        .data_out_num = DATA_OUT_IO,
        .data_in_num = DATA_IN_IO
    };
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &master_i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &master_pin_config));
    i2s_test_io_config(I2S_TEST_MODE_LOOPBACK);
    printf("\r\nheap size: %d\n", esp_get_free_heap_size());

    uint8_t* data_wr = (uint8_t*)malloc(sizeof(uint8_t)*400);
    size_t i2s_bytes_write = 0;
    size_t bytes_read = 0;
    int length = 0;
    uint8_t *i2s_read_buff = (uint8_t*)malloc(sizeof(uint8_t)*10000);

    for(int i=0; i<100; i++) {
        data_wr[i] = i+1;
    }
    int flag=0; // break loop flag
    int end_position = 0;
    // write data to slave
    i2s_write(I2S_NUM_0, data_wr, sizeof(uint8_t)*400, &i2s_bytes_write, 1000 / portTICK_PERIOD_MS);
    while(!flag){
        if (length >= 10000 - 500) {
            break;
        }
        i2s_read(I2S_NUM_0, i2s_read_buff + length, sizeof(uint8_t)*500, &bytes_read, 1000/portMAX_DELAY);
        if(bytes_read>0) {
            printf("read data size: %d\n", bytes_read);
            for(int i=length; i<length + bytes_read; i++) {
                if(i2s_read_buff[i] == 100) {
                    flag=1;
                    end_position = i;
                    break;
                }
            }
        }
        length = length + bytes_read;
    }
    // test the read data right or not
    for(int i=end_position-99; i<=end_position; i++) {
        TEST_ASSERT_EQUAL_UINT8((i-end_position+100), *(i2s_read_buff + i));
    }
    free(data_wr);
    free(i2s_read_buff);
    i2s_driver_uninstall(I2S_NUM_0);
}

#if !DISABLED_FOR_TARGETS(ESP32S2)
/* ESP32S2 has only single I2S port and hence following test cases are not applicable */
TEST_CASE("I2S adc test", "[i2s]")
{
    // init I2S ADC
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN,
        .sample_rate =  SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .intr_alloc_flags = 0,
        .dma_buf_count = 2,
        .dma_buf_len = 1024,
        .use_apll = 0,
     };
    // install and start I2S driver
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    // init ADC pad
    i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_4);
    // enable adc sampling, ADC_WIDTH_BIT_12, ADC_ATTEN_DB_11 hard-coded in adc_i2s_mode_init
    i2s_adc_enable(I2S_NUM_0);
    // init read buffer
    uint16_t* i2sReadBuffer = (uint16_t*)calloc(1024, sizeof(uint16_t));
    size_t bytesRead;

    for (int loop = 0; loop < 10; loop++) {
        for (int level = 0; level <= 1; level++) {
            if (level == 0) {
                gpio_set_pull_mode(ADC1_CHANNEL_4_IO, GPIO_PULLDOWN_ONLY);
            } else {
                gpio_set_pull_mode(ADC1_CHANNEL_4_IO, GPIO_PULLUP_ONLY);
            }
            vTaskDelay(200 / portTICK_RATE_MS);
            // read data from adc, will block until buffer is full
            i2s_read(I2S_NUM_0, (void*)i2sReadBuffer, 1024 * sizeof(uint16_t), &bytesRead, portMAX_DELAY);

            // calc average
            int64_t adcSumValue = 0;
            for (size_t i = 0; i < 1024; i++) {
                adcSumValue += i2sReadBuffer[i] & 0xfff;
            }
            int adcAvgValue = adcSumValue / 1024;
            printf("adc average val: %d\n", adcAvgValue);

            if (level == 0) {
                TEST_ASSERT_LESS_THAN(100, adcAvgValue);
            } else {
                TEST_ASSERT_GREATER_THAN(4000, adcAvgValue);
            }
        }
    }
    i2s_adc_disable(I2S_NUM_0);
    free(i2sReadBuffer);
    i2s_driver_uninstall(I2S_NUM_0);
}

TEST_CASE("I2S write and read test(master tx and slave rx)", "[i2s]")
{
    // master driver installed and send data
    i2s_config_t master_i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_buf_count = 6,
        .dma_buf_len = 100,
        .use_apll = 0,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1 ,
    };
    i2s_pin_config_t master_pin_config = {
        .bck_io_num = MASTER_BCK_IO,
        .ws_io_num = MASTER_WS_IO,
        .data_out_num = DATA_OUT_IO,
        .data_in_num = -1
    };
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &master_i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &master_pin_config));
    i2s_test_io_config(I2S_TEST_MODE_MASTER_TO_SLAVE);
    printf("\r\nheap size: %d\n", esp_get_free_heap_size());

    i2s_config_t slave_i2s_config = {
        .mode = I2S_MODE_SLAVE | I2S_MODE_RX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_buf_count = 6,
        .dma_buf_len = 100,
        .use_apll = 0,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1 ,
    };
    i2s_pin_config_t slave_pin_config = {
        .bck_io_num = SLAVE_BCK_IO,
        .ws_io_num = SLAVE_WS_IO,
        .data_out_num = -1,
        .data_in_num = DATA_IN_IO,
    };
    // slave driver installed and receive data
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_1, &slave_i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_1, &slave_pin_config));
    i2s_test_io_config(I2S_TEST_MODE_MASTER_TO_SLAVE);
    printf("\r\nheap size: %d\n", esp_get_free_heap_size());

    uint8_t* data_wr = (uint8_t*)malloc(sizeof(uint8_t)*400);
    size_t i2s_bytes_write = 0;
    size_t bytes_read = 0;
    int length = 0;
    uint8_t *i2s_read_buff = (uint8_t*)malloc(sizeof(uint8_t)*10000);

    for(int i=0; i<100; i++) {
        data_wr[i] = i+1;
    }
    int flag=0; // break loop flag
    int end_position = 0;
    // write data to slave
    i2s_write(I2S_NUM_0, data_wr, sizeof(uint8_t)*400, &i2s_bytes_write, 1000 / portTICK_PERIOD_MS);
    while(!flag){
        i2s_read(I2S_NUM_1, i2s_read_buff + length, sizeof(uint8_t)*500, &bytes_read, 1000/portMAX_DELAY);
        if(bytes_read>0) {
            printf("read data size: %d\n", bytes_read);
            for(int i=length; i<length + bytes_read; i++) {
                if(i2s_read_buff[i] == 100) {
                    flag=1;
                    end_position = i;
                    break;
                }
            }
        }
        length = length + bytes_read;
    }
    // test the readed data right or not
    for(int i=end_position-99; i<=end_position; i++) {
        TEST_ASSERT_EQUAL_UINT8((i-end_position+100), *(i2s_read_buff + i));
    }
    free(data_wr);
    free(i2s_read_buff);
    i2s_driver_uninstall(I2S_NUM_0);
    i2s_driver_uninstall(I2S_NUM_1);
}

TEST_CASE("I2S write and read test(master rx and slave tx)", "[i2s]")
{
    // master driver installed and send data
    i2s_config_t master_i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_buf_count = 6,
        .dma_buf_len = 100,
        .use_apll = 1,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1 ,
    };
    i2s_pin_config_t master_pin_config = {
        .bck_io_num = MASTER_BCK_IO,
        .ws_io_num = MASTER_WS_IO,
        .data_out_num = -1,
        .data_in_num = DATA_IN_IO,
    };
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &master_i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &master_pin_config));
    i2s_test_io_config(I2S_TEST_MODE_SLAVE_TO_MAXTER);
    printf("\r\nheap size: %d\n", esp_get_free_heap_size());

    i2s_config_t slave_i2s_config = {
        .mode = I2S_MODE_SLAVE | I2S_MODE_TX,                                  // Only RX
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           //2-channels
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_buf_count = 6,
        .dma_buf_len = 100,
        .use_apll = 1,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1 ,
    };
    i2s_pin_config_t slave_pin_config = {
        .bck_io_num = SLAVE_BCK_IO,
        .ws_io_num = SLAVE_WS_IO,
        .data_out_num = DATA_OUT_IO,
        .data_in_num = -1
    };
    // slave driver installed and receive data
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_1, &slave_i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_1, &slave_pin_config));
    i2s_test_io_config(I2S_TEST_MODE_SLAVE_TO_MAXTER);

    uint8_t* data_wr = (uint8_t*)malloc(sizeof(uint8_t)*400);
    size_t i2s_bytes_write = 0;
    size_t bytes_read = 0;
    int length = 0;
    uint8_t *i2s_read_buff = (uint8_t*)malloc(sizeof(uint8_t)*100000);

    for(int i=0; i<100; i++) {
        data_wr[i] = i+1;
    }
    // slave write data to master
    i2s_write(I2S_NUM_1, data_wr, sizeof(uint8_t)*400, &i2s_bytes_write, 1000 / portTICK_PERIOD_MS);

    int flag=0; // break loop flag
    int end_position = 0;
    // write data to slave
    while(!flag){
        TEST_ESP_OK(i2s_read(I2S_NUM_0, i2s_read_buff + length, 10000-length, &bytes_read, 1000/portMAX_DELAY));
        if(bytes_read > 0) {
            for(int i=length; i<length+bytes_read; i++) {
                if(i2s_read_buff[i] == 100) {
                    flag=1;
                    end_position = i;
                    break;
                }
            }
        }
        length = length + bytes_read;
    }
    // test the readed data right or not
    for(int i=end_position-99; i<=end_position; i++) {
        TEST_ASSERT_EQUAL_UINT8((i-end_position+100), *(i2s_read_buff + i));
    }
    free(data_wr);
    free(i2s_read_buff);
    i2s_driver_uninstall(I2S_NUM_0);
    i2s_driver_uninstall(I2S_NUM_1);
}
#endif

TEST_CASE("I2S memory leaking test", "[i2s]")
{
    i2s_config_t master_i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_buf_count = 6,
        .dma_buf_len = 100,
        .use_apll = 0,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1 ,
    };
    i2s_pin_config_t master_pin_config = {
        .bck_io_num = MASTER_BCK_IO,
        .ws_io_num = MASTER_WS_IO,
        .data_out_num = -1,
        .data_in_num = DATA_IN_IO
    };

    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &master_i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &master_pin_config));
    i2s_driver_uninstall(I2S_NUM_0);
    int initial_size = esp_get_free_heap_size();

    for(int i=0; i<100; i++) {
        TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &master_i2s_config, 0, NULL));
        TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &master_pin_config));
        i2s_driver_uninstall(I2S_NUM_0);
        TEST_ASSERT(initial_size == esp_get_free_heap_size());
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT(initial_size == esp_get_free_heap_size());
}

/*
 *   The I2S APLL clock variation test used to test the difference between the different sample rates, different bits per sample
 *   and the APLL clock generate for it. The TEST_CASE passes PERCENT_DIFF variation from the provided sample rate in APLL generated clock
 *   The percentage difference calculated as (mod((obtained clock rate - desired clock rate)/(desired clock rate))) * 100.
 */
TEST_CASE("I2S APLL clock variation test", "[i2s]")
{
    i2s_pin_config_t pin_config = {
        .bck_io_num = MASTER_BCK_IO,
        .ws_io_num = MASTER_WS_IO,
        .data_out_num = DATA_OUT_IO,
        .data_in_num = -1
    };

    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_buf_count = 6,
        .dma_buf_len = 60,
        .use_apll = true,
        .intr_alloc_flags = 0,
    };

    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL));
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &pin_config));
    TEST_ESP_OK(i2s_driver_uninstall(I2S_NUM_0));
    int initial_size = esp_get_free_heap_size();

    uint32_t sample_rate_arr[8] = { 10675, 11025, 16000, 22050, 32000, 44100, 48000, 96000 };
    int bits_per_sample_arr[3] = { 16, 24, 32 };

    for (int i = 0; i < (sizeof(sample_rate_arr)/sizeof(sample_rate_arr[0])); i++) {
        for (int j = 0; j < (sizeof(bits_per_sample_arr)/sizeof(bits_per_sample_arr[0])); j++) {
            i2s_config.sample_rate = sample_rate_arr[i];
            i2s_config.bits_per_sample = bits_per_sample_arr[j];

            TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL));
            TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, &pin_config));
            TEST_ASSERT((fabs((i2s_get_clk(I2S_NUM_0) - sample_rate_arr[i]))/(sample_rate_arr[i]))*100 < PERCENT_DIFF);
            TEST_ESP_OK(i2s_driver_uninstall(I2S_NUM_0));
            TEST_ASSERT(initial_size == esp_get_free_heap_size());
        }
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT(initial_size == esp_get_free_heap_size());
}
