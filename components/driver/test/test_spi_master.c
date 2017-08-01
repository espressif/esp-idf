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
#include "soc/dport_reg.h"
#include "soc/spi_reg.h"
#include "soc/spi_struct.h"
#include "esp_heap_caps.h"


static void check_spi_pre_n_for(int clk, int pre, int n)
{
    esp_err_t ret;
    spi_device_handle_t handle;

    spi_device_interface_config_t devcfg={
        .command_bits=0,
        .address_bits=0,
        .dummy_bits=0,
        .clock_speed_hz=clk,
        .duty_cycle_pos=128,
        .mode=0,
        .spics_io_num=21,
        .queue_size=3
    };
    char sendbuf[16]="";
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &handle);
    TEST_ASSERT(ret==ESP_OK);

    t.length=16*8;
    t.tx_buffer=sendbuf;
    ret=spi_device_transmit(handle, &t);

    printf("Checking clk rate %dHz. expect pre %d n %d, got pre %d n %d\n", clk, pre, n, SPI2.clock.clkdiv_pre+1, SPI2.clock.clkcnt_n+1);

    TEST_ASSERT(SPI2.clock.clkcnt_n+1==n);
    TEST_ASSERT(SPI2.clock.clkdiv_pre+1==pre);

    ret=spi_bus_remove_device(handle);
    TEST_ASSERT(ret==ESP_OK);
}


TEST_CASE("SPI Master clockdiv calculation routines", "[spi]")
{
    spi_bus_config_t buscfg={
        .mosi_io_num=4,
        .miso_io_num=26,
        .sclk_io_num=25,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };
    esp_err_t ret;
    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    TEST_ASSERT(ret==ESP_OK);

    check_spi_pre_n_for(26000000, 1, 3);
    check_spi_pre_n_for(20000000, 1, 4);
    check_spi_pre_n_for(8000000, 1, 10);
    check_spi_pre_n_for(800000, 2, 50);
    check_spi_pre_n_for(100000, 16, 50);
    check_spi_pre_n_for(333333, 4, 60);
    check_spi_pre_n_for(900000, 2, 44);
    check_spi_pre_n_for(1, 8192, 64); //Actually should generate the minimum clock speed, 152Hz
    check_spi_pre_n_for(26000000, 1, 3);

    ret=spi_bus_free(HSPI_HOST);
    TEST_ASSERT(ret==ESP_OK);
}

static spi_device_handle_t setup_spi_bus(int clkspeed, bool dma) {
    spi_bus_config_t buscfg={
        .mosi_io_num=4,
        .miso_io_num=26,
        .sclk_io_num=25,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz=4096*3
    };
    spi_device_interface_config_t devcfg={
        .command_bits=0,
        .address_bits=0,
        .dummy_bits=0,
        .clock_speed_hz=clkspeed,
        .duty_cycle_pos=128,
        .mode=0,
        .spics_io_num=21,
        .queue_size=3,
    };
    esp_err_t ret;
    spi_device_handle_t handle;
    printf("THIS TEST NEEDS A JUMPER BETWEEN IO4 AND IO26\n");

    ret=spi_bus_initialize(HSPI_HOST, &buscfg, dma?1:0);
    TEST_ASSERT(ret==ESP_OK);
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &handle);
    TEST_ASSERT(ret==ESP_OK);
    printf("Bus/dev inited.\n");
    return handle;
}

static void spi_test(spi_device_handle_t handle, int num_bytes) {
    esp_err_t ret;
    int x;
    srand(num_bytes);
    char *sendbuf=heap_caps_malloc(num_bytes, MALLOC_CAP_DMA);
    char *recvbuf=heap_caps_malloc(num_bytes, MALLOC_CAP_DMA);
    for (x=0; x<num_bytes; x++) {
        sendbuf[x]=rand()&0xff;
        recvbuf[x]=0x55;
    }

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length=num_bytes*8;
    t.tx_buffer=sendbuf;
    t.rx_buffer=recvbuf;
    t.address=0xA00000000000000FL;
    t.command=0x55;

    printf("Transmitting %d bytes...\n", num_bytes);
    ret=spi_device_transmit(handle, &t);
    TEST_ASSERT(ret==ESP_OK);

    srand(num_bytes);
    for (x=0; x<num_bytes; x++) {
        if (sendbuf[x]!=(rand()&0xff)) {
            printf("Huh? Sendbuf corrupted at byte %d\n", x);
            TEST_ASSERT(0);
        }
        if (sendbuf[x]!=recvbuf[x]) break;
    }
    if (x!=num_bytes) {
        int from=x-16;
        if (from<0) from=0;
        printf("Error at %d! Sent vs recved: (starting from %d)\n" , x, from);
        for (int i=0; i<32; i++) {
            if (i+from<num_bytes) printf("%02X ", sendbuf[from+i]);
        }
        printf("\n");
        for (int i=0; i<32; i++) {
            if (i+from<num_bytes) printf("%02X ", recvbuf[from+i]);
        }
        printf("\n");
//        TEST_ASSERT(0);
    }

    printf("Success!\n");

    free(sendbuf);
    free(recvbuf);
}

static void destroy_spi_bus(spi_device_handle_t handle) {
    esp_err_t ret;
    ret=spi_bus_remove_device(handle);
    TEST_ASSERT(ret==ESP_OK);
    ret=spi_bus_free(HSPI_HOST);
    TEST_ASSERT(ret==ESP_OK);
}


#define TEST_LEN 111

TEST_CASE("SPI Master test", "[spi][ignore]")
{
    printf("Testing bus at 80KHz\n");
    spi_device_handle_t handle=setup_spi_bus(80000, true);
    spi_test(handle, 16); //small
    spi_test(handle, 21); //small, unaligned
    spi_test(handle, 36); //aligned
    spi_test(handle, 128); //aligned
    spi_test(handle, 129); //unaligned
    spi_test(handle, 4096-2); //multiple descs, edge case 1
    spi_test(handle, 4096-1); //multiple descs, edge case 2
    spi_test(handle, 4096*3); //multiple descs

    destroy_spi_bus(handle);

    printf("Testing bus at 80KHz, non-DMA\n");
    handle=setup_spi_bus(80000, false);
    spi_test(handle, 4); //aligned
    spi_test(handle, 16); //small
    spi_test(handle, 21); //small, unaligned

    destroy_spi_bus(handle);


    printf("Testing bus at 26MHz\n");
    handle=setup_spi_bus(20000000, true);

    spi_test(handle, 128); //DMA, aligned
    spi_test(handle, 4096*3); //DMA, multiple descs
    destroy_spi_bus(handle);

    printf("Testing bus at 900KHz\n");
    handle=setup_spi_bus(9000000, true);

    spi_test(handle, 128); //DMA, aligned
    spi_test(handle, 4096*3); //DMA, multiple descs
    destroy_spi_bus(handle);
}


TEST_CASE("SPI Master test, interaction of multiple devs", "[spi][ignore]") {
    esp_err_t ret;
    spi_device_interface_config_t devcfg={
        .command_bits=0,
        .address_bits=0,
        .dummy_bits=0,
        .clock_speed_hz=1000000,
        .duty_cycle_pos=128,
        .mode=0,
        .spics_io_num=23,
        .queue_size=3,
    };
    spi_device_handle_t handle1=setup_spi_bus(80000, true);
    spi_device_handle_t handle2;
    spi_bus_add_device(HSPI_HOST, &devcfg, &handle2);

    printf("Sending to dev 1\n");
    spi_test(handle1, 7);
    printf("Sending to dev 1\n");
    spi_test(handle1, 15);
    printf("Sending to dev 2\n");
    spi_test(handle2, 15);
    printf("Sending to dev 1\n");
    spi_test(handle1, 32);
    printf("Sending to dev 2\n");
    spi_test(handle2, 32);
    printf("Sending to dev 1\n");
    spi_test(handle1, 63);
    printf("Sending to dev 2\n");
    spi_test(handle2, 63);
    printf("Sending to dev 1\n");
    spi_test(handle1, 5000);
    printf("Sending to dev 2\n");
    spi_test(handle2, 5000);


    ret=spi_bus_remove_device(handle2);
    TEST_ASSERT(ret==ESP_OK);
    destroy_spi_bus(handle1);
}

TEST_CASE("SPI Master no response when switch from host1 (HSPI) to host2 (VSPI)", "[spi]")
{
	//spi config
	spi_bus_config_t bus_config;
	spi_device_interface_config_t device_config;
	spi_device_handle_t spi;
	spi_host_device_t host;
	int dma = 1;

	memset(&bus_config, 0, sizeof(spi_bus_config_t));
	memset(&device_config, 0, sizeof(spi_device_interface_config_t));

	bus_config.miso_io_num = -1;
	bus_config.mosi_io_num = 26;
	bus_config.sclk_io_num = 25;
	bus_config.quadwp_io_num = -1;
	bus_config.quadhd_io_num = -1;

	device_config.clock_speed_hz = 50000;
	device_config.mode = 0;
	device_config.spics_io_num = -1;
	device_config.queue_size = 1;
	device_config.flags = SPI_DEVICE_TXBIT_LSBFIRST | SPI_DEVICE_RXBIT_LSBFIRST;

	struct spi_transaction_t transaction = {
		.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA,
		.length = 16,
		.tx_buffer = NULL,
		.rx_buffer = NULL,
		.tx_data = {0x04, 0x00}
	};


	//initialize for first host
	host = 1;

	assert(spi_bus_initialize(host, &bus_config, dma) == ESP_OK);
	assert(spi_bus_add_device(host, &device_config, &spi) == ESP_OK);

	printf("before first xmit\n");
	assert(spi_device_transmit(spi, &transaction) == ESP_OK);
	printf("after first xmit\n");

	assert(spi_bus_remove_device(spi) == ESP_OK);
	assert(spi_bus_free(host) == ESP_OK);


	//for second host and failed before
	host = 2;

	assert(spi_bus_initialize(host, &bus_config, dma) == ESP_OK);
	assert(spi_bus_add_device(host, &device_config, &spi) == ESP_OK);

	printf("before second xmit\n");
	// the original version (bit mis-written) stucks here.
	assert(spi_device_transmit(spi, &transaction) == ESP_OK);
	// test case success when see this.
	printf("after second xmit\n");


}
