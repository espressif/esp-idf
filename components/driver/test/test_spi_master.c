/*
 Tests for the spi_master device driver
*/

#include <esp_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "driver/spi_master.h"



TEST_CASE("SPI Master test", "[spi]")
{
    spi_bus_config_t buscfg={
        .mosi_io_num=4,
        .miso_io_num=16,
        .sclk_io_num=25,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };
    spi_device_interface_config_t devcfg={
        .command_bits=8,
        .address_bits=64,
        .dummy_bits=0,
        .clock_speed_hz=8000,
        .duty_cycle_pos=128,
        .mode=0,
        .spics_io_num=21,
        .queue_size=3
    };

    esp_err_t ret;
    spi_device_handle_t handle;
    printf("THIS TEST NEEDS A JUMPER BETWEEN IO4 AND IO16\n");

    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    TEST_ASSERT(ret==ESP_OK);
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &handle);
    TEST_ASSERT(ret==ESP_OK);
    printf("Bus/dev inited.\n");
    spi_transaction_t t;
    char sendbuf[16]="Hello World!";
    char recvbuf[16]="UUUUUUUUUUUUUUU";
    memset(&t, 0, sizeof(t));

    t.length=16*8;
    t.tx_buffer=sendbuf;
    t.rx_buffer=recvbuf;
    t.address=0xA00000000000000FL;
    t.command=0x55;
    printf("Transmit...\n");
    ret=spi_device_transmit(handle, &t);
    TEST_ASSERT(ret==ESP_OK);
    printf("Send vs recv:\n");
    for (int x=0; x<16; x++) printf("%02X ", (int)sendbuf[x]);
    printf("<sent\n");
    for (int x=0; x<16; x++) printf("%02X ", (int)recvbuf[x]);
    printf("<recv\n");

    ret=spi_bus_remove_device(handle);
    TEST_ASSERT(ret==ESP_OK);
    ret=spi_bus_free(HSPI_HOST);
    TEST_ASSERT(ret==ESP_OK);

    TEST_ASSERT_EQUAL_INT8_ARRAY(sendbuf, recvbuf, 16);

}


