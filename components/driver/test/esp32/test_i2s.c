/**
 * I2S test environment UT_T1_I2S:
 * connect GPIO18 and GPIO19, GPIO25 and GPIO26, GPIO21 and GPIO22
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "unity.h"
#include "math.h"

#define SAMPLE_RATE     (36000)
#define SAMPLE_BITS     (16)
#define MASTER_BCK_IO 18
#define MASTER_WS_IO 25
#define SLAVE_BCK_IO 19
#define SLAVE_WS_IO 26
#define DATA_IN_IO 21
#define DATA_OUT_IO 22
#define PERCENT_DIFF 0.0001

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
        .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
        .dma_buf_count = 6,
        .dma_buf_len = 60,
        .use_apll = 0,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1 ,
    };

    //install and start i2s driver
    TEST_ESP_OK(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL));
    //for internal DAC, this will enable both of the internal channels
    TEST_ESP_OK(i2s_set_pin(I2S_NUM_0, NULL));
    //stop & destroy i2s driver
    TEST_ESP_OK(i2s_driver_uninstall(I2S_NUM_0));

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

TEST_CASE("I2S write and read test(master tx and slave rx)", "[i2s][test_env=UT_T1_I2S]")
{
    // master driver installed and send data
    i2s_config_t master_i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
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
    printf("\r\nheap size: %d\n", esp_get_free_heap_size());

    i2s_config_t slave_i2s_config = {
        .mode = I2S_MODE_SLAVE | I2S_MODE_RX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
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
        TEST_ASSERT(*(i2s_read_buff + i) == (i-end_position+100));
    }
    free(data_wr);
    free(i2s_read_buff);
    i2s_driver_uninstall(I2S_NUM_0);
    i2s_driver_uninstall(I2S_NUM_1);
}

TEST_CASE("I2S write and read test(master rx and slave tx)", "[i2s][test_env=UT_T1_I2S]")
{
    // master driver installed and send data
    i2s_config_t master_i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
        .dma_buf_count = 6,
        .dma_buf_len = 100,
        .use_apll = 0,
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
    printf("\r\nheap size: %d\n", esp_get_free_heap_size());

    i2s_config_t slave_i2s_config = {
        .mode = I2S_MODE_SLAVE | I2S_MODE_TX,                                  // Only RX
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           //2-channels
        .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
        .dma_buf_count = 6,
        .dma_buf_len = 100,
        .use_apll = 0,
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
        TEST_ASSERT(*(i2s_read_buff + i) == (i-end_position+100));
    }
    free(data_wr);
    free(i2s_read_buff);
    i2s_driver_uninstall(I2S_NUM_0);
    i2s_driver_uninstall(I2S_NUM_1);
}

TEST_CASE("I2S memory leaking test", "[i2s]")
{
    i2s_config_t master_i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = SAMPLE_BITS,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
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
        .communication_format = I2S_COMM_FORMAT_I2S,
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
