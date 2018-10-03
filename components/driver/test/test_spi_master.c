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
#include "driver/spi_slave.h"
#include "soc/dport_reg.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "soc/spi_periph.h"
#include "freertos/ringbuf.h"
#include "soc/gpio_periph.h"

const static char TAG[] = "test_spi";

#define SPI_BUS_TEST_DEFAULT_CONFIG() {\
        .miso_io_num=PIN_NUM_MISO, \
        .mosi_io_num=PIN_NUM_MOSI,\
        .sclk_io_num=PIN_NUM_CLK,\
        .quadwp_io_num=-1,\
        .quadhd_io_num=-1\
}

#define SPI_DEVICE_TEST_DEFAULT_CONFIG()    {\
    .clock_speed_hz=10*1000*1000,\
    .mode=0,\
    .spics_io_num=PIN_NUM_CS,\
    .queue_size=16,\
    .pre_cb=NULL,  \
    .cs_ena_pretrans = 0,\
    .cs_ena_posttrans = 0,\
    .input_delay_ns = 62.5,\
}

#define FUNC_SPI    1
#define FUNC_GPIO   2

void gpio_output_sel(uint32_t gpio_num, int func, uint32_t signal_idx)
{
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[gpio_num], func);
    GPIO.func_out_sel_cfg[gpio_num].func_sel=signal_idx;
}

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
    t.addr=0xA00000000000000FL;
    t.cmd=0x55;

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

TEST_CASE("spi bus setting with different pin configs", "[spi]")
{
    spi_bus_config_t cfg;
    uint32_t flags_o;
    uint32_t flags_expected;

    ESP_LOGI(TAG, "test 6 iomux output pins...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_NATIVE_PINS | SPICOMMON_BUSFLAG_QUAD;
    cfg = (spi_bus_config_t){.mosi_io_num = HSPI_IOMUX_PIN_NUM_MOSI, .miso_io_num = HSPI_IOMUX_PIN_NUM_MISO, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = HSPI_IOMUX_PIN_NUM_HD, .quadwp_io_num = HSPI_IOMUX_PIN_NUM_WP,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ESP_OK(spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );
    TEST_ESP_OK(spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );

    ESP_LOGI(TAG, "test 4 iomux output pins...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_NATIVE_PINS | SPICOMMON_BUSFLAG_DUAL;
    cfg = (spi_bus_config_t){.mosi_io_num = HSPI_IOMUX_PIN_NUM_MOSI, .miso_io_num = HSPI_IOMUX_PIN_NUM_MISO, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ESP_OK(spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );
    TEST_ESP_OK(spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );

    ESP_LOGI(TAG, "test 6 output pins...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_QUAD;
    //swap MOSI and MISO
    cfg = (spi_bus_config_t){.mosi_io_num = HSPI_IOMUX_PIN_NUM_MISO, .miso_io_num = HSPI_IOMUX_PIN_NUM_MOSI, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = HSPI_IOMUX_PIN_NUM_HD, .quadwp_io_num = HSPI_IOMUX_PIN_NUM_WP,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ESP_OK(spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );
    TEST_ESP_OK(spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );

    ESP_LOGI(TAG, "test 4 output pins...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO |  SPICOMMON_BUSFLAG_DUAL;
    //swap MOSI and MISO
    cfg = (spi_bus_config_t){.mosi_io_num = HSPI_IOMUX_PIN_NUM_MISO, .miso_io_num = HSPI_IOMUX_PIN_NUM_MOSI, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ESP_OK(spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );
    TEST_ESP_OK(spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );

    ESP_LOGI(TAG, "test master 5 output pins and MOSI on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_WPHD;
    cfg = (spi_bus_config_t){.mosi_io_num = HSPI_IOMUX_PIN_NUM_MOSI, .miso_io_num = 34, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = HSPI_IOMUX_PIN_NUM_HD, .quadwp_io_num = HSPI_IOMUX_PIN_NUM_WP,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ESP_OK(spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );

    ESP_LOGI(TAG, "test slave 5 output pins and MISO on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_WPHD;
    cfg = (spi_bus_config_t){.mosi_io_num = 34, .miso_io_num = HSPI_IOMUX_PIN_NUM_MISO, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = HSPI_IOMUX_PIN_NUM_HD, .quadwp_io_num = HSPI_IOMUX_PIN_NUM_WP,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ESP_OK(spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );

    ESP_LOGI(TAG, "test master 3 output pins and MOSI on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO;

    cfg = (spi_bus_config_t){.mosi_io_num = HSPI_IOMUX_PIN_NUM_MOSI, .miso_io_num = 34, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ESP_OK(spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );

    ESP_LOGI(TAG, "test slave 3 output pins and MISO on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MISO;
    cfg = (spi_bus_config_t){.mosi_io_num = 34, .miso_io_num = HSPI_IOMUX_PIN_NUM_MISO, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ESP_OK(spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    TEST_ASSERT_EQUAL_HEX32( flags_expected, flags_o );

    ESP_LOGI(TAG, "check native flag for 6 output pins...");
    flags_expected = SPICOMMON_BUSFLAG_NATIVE_PINS;
    //swap MOSI and MISO
    cfg = (spi_bus_config_t){.mosi_io_num = HSPI_IOMUX_PIN_NUM_MISO, .miso_io_num = HSPI_IOMUX_PIN_NUM_MOSI, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = HSPI_IOMUX_PIN_NUM_HD, .quadwp_io_num = HSPI_IOMUX_PIN_NUM_WP,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));

    ESP_LOGI(TAG, "check native flag for 4 output pins...");
    flags_expected = SPICOMMON_BUSFLAG_NATIVE_PINS;
    //swap MOSI and MISO
    cfg = (spi_bus_config_t){.mosi_io_num = HSPI_IOMUX_PIN_NUM_MISO, .miso_io_num = HSPI_IOMUX_PIN_NUM_MOSI, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));

    ESP_LOGI(TAG, "check dual flag for master 5 output pins and MISO/MOSI on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_DUAL;
    cfg = (spi_bus_config_t){.mosi_io_num = HSPI_IOMUX_PIN_NUM_MOSI, .miso_io_num = 34, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = HSPI_IOMUX_PIN_NUM_HD, .quadwp_io_num = HSPI_IOMUX_PIN_NUM_WP,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    cfg = (spi_bus_config_t){.mosi_io_num = 34, .miso_io_num = HSPI_IOMUX_PIN_NUM_MISO, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = HSPI_IOMUX_PIN_NUM_HD, .quadwp_io_num = HSPI_IOMUX_PIN_NUM_WP,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));

    ESP_LOGI(TAG, "check dual flag for master 3 output pins and MISO/MOSI on input-only pin...");
    flags_expected = SPICOMMON_BUSFLAG_DUAL;
    cfg = (spi_bus_config_t){.mosi_io_num = HSPI_IOMUX_PIN_NUM_MOSI, .miso_io_num = 34, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    cfg = (spi_bus_config_t){.mosi_io_num = 34, .miso_io_num = HSPI_IOMUX_PIN_NUM_MISO, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = -1, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));

    ESP_LOGI(TAG, "check sclk flag...");
    flags_expected = SPICOMMON_BUSFLAG_SCLK;
    cfg = (spi_bus_config_t){.mosi_io_num = HSPI_IOMUX_PIN_NUM_MOSI, .miso_io_num = HSPI_IOMUX_PIN_NUM_MISO, .sclk_io_num = -1, .quadhd_io_num = HSPI_IOMUX_PIN_NUM_HD, .quadwp_io_num = HSPI_IOMUX_PIN_NUM_WP,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));

    ESP_LOGI(TAG, "check mosi flag...");
    flags_expected = SPICOMMON_BUSFLAG_MOSI;
    cfg = (spi_bus_config_t){.mosi_io_num = -1, .miso_io_num = HSPI_IOMUX_PIN_NUM_MISO, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = HSPI_IOMUX_PIN_NUM_HD, .quadwp_io_num = HSPI_IOMUX_PIN_NUM_WP,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));

    ESP_LOGI(TAG, "check miso flag...");
    flags_expected = SPICOMMON_BUSFLAG_MISO;
    cfg = (spi_bus_config_t){.mosi_io_num = HSPI_IOMUX_PIN_NUM_MOSI, .miso_io_num = -1, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = HSPI_IOMUX_PIN_NUM_HD, .quadwp_io_num = HSPI_IOMUX_PIN_NUM_WP,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));

    ESP_LOGI(TAG, "check quad flag...");
    flags_expected = SPICOMMON_BUSFLAG_QUAD;
    cfg = (spi_bus_config_t){.mosi_io_num = HSPI_IOMUX_PIN_NUM_MOSI, .miso_io_num = HSPI_IOMUX_PIN_NUM_MISO, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = -1, .quadwp_io_num = HSPI_IOMUX_PIN_NUM_WP,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));
    cfg = (spi_bus_config_t){.mosi_io_num = HSPI_IOMUX_PIN_NUM_MOSI, .miso_io_num = HSPI_IOMUX_PIN_NUM_MISO, .sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK, .quadhd_io_num = HSPI_IOMUX_PIN_NUM_HD, .quadwp_io_num = -1,
        .max_transfer_sz = 8, .flags = flags_expected};
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_MASTER, &flags_o));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, spicommon_bus_initialize_io(HSPI_HOST, &cfg, 0, flags_expected|SPICOMMON_BUSFLAG_SLAVE, &flags_o));
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
        .rx_buffer = NULL,
        .tx_data = {0x04, 0x00}
    };

    //initialize for first host
    host = 1;

    TEST_ASSERT(spi_bus_initialize(host, &bus_config, dma) == ESP_OK);
    TEST_ASSERT(spi_bus_add_device(host, &device_config, &spi) == ESP_OK);

    printf("before first xmit\n");
    TEST_ASSERT(spi_device_transmit(spi, &transaction) == ESP_OK);
    printf("after first xmit\n");

    TEST_ASSERT(spi_bus_remove_device(spi) == ESP_OK);
    TEST_ASSERT(spi_bus_free(host) == ESP_OK);

    //for second host and failed before
    host = 2;

    TEST_ASSERT(spi_bus_initialize(host, &bus_config, dma) == ESP_OK);
    TEST_ASSERT(spi_bus_add_device(host, &device_config, &spi) == ESP_OK);

    printf("before second xmit\n");
    // the original version (bit mis-written) stucks here.
    TEST_ASSERT(spi_device_transmit(spi, &transaction) == ESP_OK);
    // test case success when see this.
    printf("after second xmit\n");

    TEST_ASSERT(spi_bus_remove_device(spi) == ESP_OK);
    TEST_ASSERT(spi_bus_free(host) == ESP_OK);
}

IRAM_ATTR  static uint32_t data_iram[320];
DRAM_ATTR  static uint32_t data_dram[320]={0};
//force to place in code area.
static const uint32_t data_drom[320] = {0};

#if 1 //HSPI
#define PIN_NUM_MISO HSPI_IOMUX_PIN_NUM_MISO
#define PIN_NUM_MOSI HSPI_IOMUX_PIN_NUM_MOSI
#define PIN_NUM_CLK  HSPI_IOMUX_PIN_NUM_CLK
#define PIN_NUM_CS   HSPI_IOMUX_PIN_NUM_CS
#elif 1 //VSPI
#define PIN_NUM_MISO VSPI_IOMUX_PIN_NUM_MISO
#define PIN_NUM_MOSI VSPI_IOMUX_PIN_NUM_MOSI
#define PIN_NUM_CLK  VSPI_IOMUX_PIN_NUM_CLK
#define PIN_NUM_CS   VSPI_IOMUX_PIN_NUM_CS
#endif

#define PIN_NUM_DC   21
#define PIN_NUM_RST  18
#define PIN_NUM_BCKL 5

TEST_CASE("SPI Master DMA test, TX and RX in different regions", "[spi]")
{
    uint32_t data_rxdram[320];

    esp_err_t ret;
    spi_device_handle_t spi;
    spi_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO,
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };
    spi_device_interface_config_t devcfg={
        .clock_speed_hz=10000000,               //Clock out at 10 MHz
        .mode=0,                                //SPI mode 0
        .spics_io_num=PIN_NUM_CS,               //CS pin
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
        .pre_cb=NULL,  //Specify pre-transfer callback to handle D/C line
    };
    //Initialize the SPI bus
    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    TEST_ASSERT(ret==ESP_OK);
    //Attach the LCD to the SPI bus
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    TEST_ASSERT(ret==ESP_OK);

    static spi_transaction_t trans[6];
    int x;

    printf("iram: %p, dram: %p, drom: %p\n", data_iram, data_dram, data_drom);

    memset(trans, 0, 6*sizeof(spi_transaction_t));

    trans[0].length = 320*8,
    trans[0].tx_buffer = data_iram;
    trans[0].rx_buffer = data_rxdram;

    trans[1].length = 320*8,
    trans[1].tx_buffer = data_dram;
    trans[1].rx_buffer = data_rxdram;

    trans[2].length = 320*8,
    trans[2].tx_buffer = data_drom;
    trans[2].rx_buffer = data_rxdram;

    trans[3].length = 320*8,
    trans[3].tx_buffer = data_drom;
    trans[3].rx_buffer = data_iram;

    trans[4].length = 320*8,
    trans[4].rxlength = 8*4;
    trans[4].tx_buffer = data_drom;
    trans[4].flags = SPI_TRANS_USE_RXDATA;

    trans[5].length = 8*4;
    trans[5].flags = SPI_TRANS_USE_RXDATA | SPI_TRANS_USE_TXDATA;

    //Queue all transactions.
    for (x=0; x<6; x++) {
        ret=spi_device_queue_trans(spi,&trans[x], portMAX_DELAY);
        TEST_ASSERT(ret==ESP_OK);
    }

    for (x=0; x<6; x++) {
        spi_transaction_t* ptr;
        ret=spi_device_get_trans_result(spi,&ptr, portMAX_DELAY);
        TEST_ASSERT(ret==ESP_OK);
        TEST_ASSERT(ptr = trans+x);
    }

    TEST_ASSERT(spi_bus_remove_device(spi) == ESP_OK);
    TEST_ASSERT(spi_bus_free(HSPI_HOST) == ESP_OK);
}

//this part tests 3 DMA issues in master mode, full-duplex in IDF2.1
// 1. RX buffer not aligned (start and end)
// 2. not setting rx_buffer
// 3. setting rx_length != length
TEST_CASE("SPI Master DMA test: length, start, not aligned", "[spi]")
{
    uint8_t tx_buf[320]={0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0xaa, 0xcc, 0xff, 0xee, 0x55, 0x77, 0x88, 0x43};
    uint8_t rx_buf[320];

    esp_err_t ret;
    spi_device_handle_t spi;
    spi_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MOSI,
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };
    spi_device_interface_config_t devcfg={
        .clock_speed_hz=10*1000*1000,               //Clock out at 10 MHz
        .mode=0,                                //SPI mode 0
        .spics_io_num=PIN_NUM_CS,               //CS pin
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
        .pre_cb=NULL,
    };
    //Initialize the SPI bus
    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    TEST_ASSERT(ret==ESP_OK);
    //Attach the LCD to the SPI bus
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    TEST_ASSERT(ret==ESP_OK);

    //connect MOSI to two devices breaks the output, fix it.
    gpio_output_sel(buscfg.mosi_io_num, FUNC_GPIO, HSPID_OUT_IDX);

    memset(rx_buf, 0x66, 320);

    for ( int i = 0; i < 8; i ++ ) {
        memset( rx_buf, 0x66, sizeof(rx_buf));

        spi_transaction_t t = {};
        t.length = 8*(i+1);
        t.rxlength = 0;
        t.tx_buffer = tx_buf+2*i;
        t.rx_buffer = rx_buf + i;

        if ( i == 1 ) {
            //test set no start
            t.rx_buffer = NULL;
        } else if ( i == 2 ) {
            //test rx length != tx_length
            t.rxlength = t.length - 8;
        }
        spi_device_transmit( spi, &t );

        for( int i = 0; i < 16; i ++ ) {
            printf("%02X ", rx_buf[i]);
        }
        printf("\n");

        if ( i == 1 ) {
            // no rx, skip check
        } else if ( i == 2 ) {
            //test rx length = tx length-1
            TEST_ASSERT_EQUAL_HEX8_ARRAY(t.tx_buffer, t.rx_buffer, t.length/8-1 );
        } else {
            //normal check
            TEST_ASSERT_EQUAL_HEX8_ARRAY(t.tx_buffer, t.rx_buffer, t.length/8 );
        }
    }

    TEST_ASSERT(spi_bus_remove_device(spi) == ESP_OK);
    TEST_ASSERT(spi_bus_free(HSPI_HOST) == ESP_OK);
}

static const char MASTER_TAG[] = "test_master";
static const char SLAVE_TAG[] = "test_slave";
DRAM_ATTR static uint8_t master_send[] = {0x93, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0xaa, 0xcc, 0xff, 0xee, 0x55, 0x77, 0x88, 0x43};
DRAM_ATTR static uint8_t slave_send[] = { 0xaa, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10, 0x13, 0x57, 0x9b, 0xdf, 0x24, 0x68, 0xac, 0xe0 };


static void master_deinit(spi_device_handle_t spi)
{
    TEST_ESP_OK( spi_bus_remove_device(spi) );
    TEST_ESP_OK( spi_bus_free(HSPI_HOST) );
}

#define SPI_SLAVE_TEST_DEFAULT_CONFIG() {\
        .mode=0,\
        .spics_io_num=PIN_NUM_CS,\
        .queue_size=3,\
        .flags=0,\
}

static void slave_pull_up(const spi_bus_config_t* cfg, int spics_io_num)
{
    gpio_set_pull_mode(cfg->mosi_io_num, GPIO_PULLUP_ENABLE);
    gpio_set_pull_mode(cfg->sclk_io_num, GPIO_PULLUP_ENABLE);
    gpio_set_pull_mode(spics_io_num, GPIO_PULLUP_ENABLE);
}

typedef struct {
    uint32_t len;
    uint8_t *start;
} slave_txdata_t;

typedef struct {
    uint32_t len;
    uint8_t* tx_start;
    uint8_t data[1];
} slave_rxdata_t;

typedef struct {
    spi_host_device_t spi;
    RingbufHandle_t data_received;
    QueueHandle_t data_to_send;
} spi_slave_task_context_t;

esp_err_t init_slave_context(spi_slave_task_context_t *context)
{
    context->data_to_send = xQueueCreate( 16, sizeof( slave_txdata_t ));
    if ( context->data_to_send == NULL ) {
        return ESP_ERR_NO_MEM;
    }
    context->data_received = xRingbufferCreate( 1024, RINGBUF_TYPE_NOSPLIT );
    if ( context->data_received == NULL ) {
        return ESP_ERR_NO_MEM;
    }
    context->spi=VSPI_HOST;
    return ESP_OK;
}

void deinit_slave_context(spi_slave_task_context_t *context)
{
    TEST_ASSERT( context->data_to_send != NULL );
    vQueueDelete( context->data_to_send );
    context->data_to_send = NULL;
    TEST_ASSERT( context->data_received != NULL );
    vRingbufferDelete( context->data_received );
    context->data_received = NULL;
}

/* The task requires a queue and a ringbuf, which should be initialized before task starts.
   Send ``slave_txdata_t`` to the queue to make the task send data;
   the task returns data got to the ringbuf, which should have sufficient size.
*/
static void task_slave(void* arg)
{
    spi_slave_task_context_t* context = (spi_slave_task_context_t*) arg;
    QueueHandle_t queue = context->data_to_send;
    RingbufHandle_t ringbuf = context->data_received;
    uint8_t recvbuf[320+8];
    slave_txdata_t txdata;

    ESP_LOGI( SLAVE_TAG, "slave up" );
    //never quit, but blocked by the queue, waiting to be killed, when no more send from main task.
    while( 1 ) {
        xQueueReceive( queue, &txdata, portMAX_DELAY );

        ESP_LOGI( "test", "to send: %p", txdata.start );
        spi_slave_transaction_t t = {};
        t.length = txdata.len;
        t.tx_buffer = txdata.start;
        t.rx_buffer = recvbuf+8;
        //loop until trans_len != 0 to skip glitches
        memset(recvbuf, 0x66, sizeof(recvbuf));
        do {
            TEST_ESP_OK( spi_slave_transmit( context->spi, &t, portMAX_DELAY ) );
        } while ( t.trans_len == 0 );
        memcpy(recvbuf, &t.trans_len, sizeof(uint32_t));
        *(uint8_t**)(recvbuf+4) = txdata.start;
        ESP_LOGI( SLAVE_TAG, "received: %d", t.trans_len );
        xRingbufferSend( ringbuf, recvbuf, 8+(t.trans_len+7)/8, portMAX_DELAY );
    }
}

#define TEST_SPI_HOST   HSPI_HOST
#define TEST_SLAVE_HOST VSPI_HOST

static uint8_t bitswap(uint8_t in)
{
    uint8_t out = 0;
    for (int i = 0; i < 8; i++) {
        out = out >> 1;
        if (in&0x80) out |= 0x80;
        in = in << 1;
    }
    return out;
}

void test_cmd_addr(spi_slave_task_context_t *slave_context, bool lsb_first)
{
    spi_device_handle_t spi;

    ESP_LOGI(MASTER_TAG, ">>>>>>>>> TEST %s FIRST <<<<<<<<<<<", lsb_first?"LSB":"MSB");

    //initial master, mode 0, 1MHz
    spi_bus_config_t buscfg=SPI_BUS_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, 1));
    spi_device_interface_config_t devcfg=SPI_DEVICE_TEST_DEFAULT_CONFIG();
    devcfg.clock_speed_hz = 1*1000*1000;
    if (lsb_first) devcfg.flags |= SPI_DEVICE_BIT_LSBFIRST;
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &spi));

    //connecting pins to two peripherals breaks the output, fix it.
    gpio_output_sel(buscfg.mosi_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spid_out);
    gpio_output_sel(buscfg.miso_io_num, FUNC_GPIO, spi_periph_signal[TEST_SLAVE_HOST].spiq_out);
    gpio_output_sel(devcfg.spics_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spics_out[0]);
    gpio_output_sel(buscfg.sclk_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spiclk_out);

    for (int i= 0; i < 8; i++) {
        //prepare slave tx data
        slave_txdata_t slave_txdata = (slave_txdata_t) {
            .start = slave_send,
            .len = 256,
        };
        xQueueSend(slave_context->data_to_send, &slave_txdata, portMAX_DELAY);

        vTaskDelay(50);
        //prepare master tx data
        int cmd_bits = (i+1)*2;
        int addr_bits = 56-8*i;
        int round_up = (cmd_bits+addr_bits+7)/8*8;
        addr_bits = round_up - cmd_bits;

        spi_transaction_ext_t trans = (spi_transaction_ext_t) {
            .base = {
                .flags = SPI_TRANS_VARIABLE_CMD | SPI_TRANS_VARIABLE_ADDR,
                .addr = 0x456789abcdef0123,
                .cmd = 0xcdef,
            },
            .command_bits = cmd_bits,
            .address_bits = addr_bits,
        };

        ESP_LOGI( MASTER_TAG, "===== test%d =====", i );
        ESP_LOGI(MASTER_TAG, "cmd_bits： %d, addr_bits: %d", cmd_bits, addr_bits);
        TEST_ESP_OK(spi_device_transmit(spi, (spi_transaction_t*)&trans));
        //wait for both master and slave end

        size_t rcv_len;
        slave_rxdata_t *rcv_data = xRingbufferReceive(slave_context->data_received, &rcv_len, portMAX_DELAY);
        rcv_len-=8;
        uint8_t *buffer = rcv_data->data;

        ESP_LOGI(SLAVE_TAG, "trans_len: %d", rcv_len);
        TEST_ASSERT_EQUAL(rcv_len, (rcv_data->len+7)/8);
        TEST_ASSERT_EQUAL(rcv_data->len, cmd_bits+addr_bits);
        ESP_LOG_BUFFER_HEX("slave rx", buffer, rcv_len);

        uint16_t cmd_expected = trans.base.cmd & (BIT(cmd_bits) - 1);
        uint64_t addr_expected = trans.base.addr & ((1ULL<<addr_bits) - 1);

        uint8_t *data_ptr = buffer;
        uint16_t cmd_got = *(uint16_t*)data_ptr;
        data_ptr += cmd_bits/8;
        cmd_got = __builtin_bswap16(cmd_got);
        cmd_got = cmd_got >> (16-cmd_bits);
        int remain_bits = cmd_bits % 8;

        uint64_t addr_got = *(uint64_t*)data_ptr;
        data_ptr += 8;
        addr_got = __builtin_bswap64(addr_got);
        addr_got = (addr_got << remain_bits);
        addr_got |= (*data_ptr >> (8-remain_bits));
        addr_got = addr_got >> (64-addr_bits);

        if (lsb_first) {
            cmd_got = __builtin_bswap16(cmd_got);
            addr_got = __builtin_bswap64(addr_got);

            uint8_t *swap_ptr = (uint8_t*)&cmd_got;
            swap_ptr[0] = bitswap(swap_ptr[0]);
            swap_ptr[1] = bitswap(swap_ptr[1]);
            cmd_got = cmd_got >> (16-cmd_bits);

            swap_ptr = (uint8_t*)&addr_got;
            for (int j = 0; j < 8; j++) swap_ptr[j] = bitswap(swap_ptr[j]);
            addr_got = addr_got >> (64-addr_bits);
        }

        ESP_LOGI(SLAVE_TAG, "cmd_got: %04X, addr_got: %08X%08X", cmd_got, (uint32_t)(addr_got>>32), (uint32_t)addr_got);

        TEST_ASSERT_EQUAL_HEX16(cmd_expected, cmd_got);
        if (addr_bits > 0) {
            TEST_ASSERT_EQUAL_HEX32(addr_expected, addr_got);
            TEST_ASSERT_EQUAL_HEX32(addr_expected >> 8, addr_got >> 8);
        }

        //clean
        vRingbufferReturnItem(slave_context->data_received, buffer);
    }

    TEST_ASSERT(spi_bus_remove_device(spi) == ESP_OK);
    TEST_ASSERT(spi_bus_free(TEST_SPI_HOST) == ESP_OK);
}

TEST_CASE("SPI master variable cmd & addr test","[spi]")
{
    spi_slave_task_context_t slave_context = {};
    esp_err_t err = init_slave_context( &slave_context );
    TEST_ASSERT( err == ESP_OK );
    TaskHandle_t handle_slave;
    xTaskCreate( task_slave, "spi_slave", 4096, &slave_context, 0, &handle_slave);

    //initial slave, mode 0, no dma
    int dma_chan = 0;
    int slave_mode = 0;
    spi_bus_config_t slv_buscfg=SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_slave_interface_config_t slvcfg=SPI_SLAVE_TEST_DEFAULT_CONFIG();
    slvcfg.mode = slave_mode;
    //Initialize SPI slave interface
    TEST_ESP_OK( spi_slave_initialize(TEST_SLAVE_HOST, &slv_buscfg, &slvcfg, dma_chan) );

    test_cmd_addr(&slave_context, false);
    test_cmd_addr(&slave_context, true);

    vTaskDelete( handle_slave );
    handle_slave = 0;

    deinit_slave_context(&slave_context);

    TEST_ASSERT(spi_slave_free(TEST_SLAVE_HOST) == ESP_OK);

    ESP_LOGI(MASTER_TAG, "test passed.");
}

/********************************************************************************
 *      Test Timing By Internal Connections
 ********************************************************************************/
typedef enum {
    FULL_DUPLEX = 0,
    HALF_DUPLEX_MISO = 1,
    HALF_DUPLEX_MOSI = 2,
} spi_dup_t;

static int timing_speed_array[]={/**/
    SPI_MASTER_FREQ_8M ,
    SPI_MASTER_FREQ_9M ,
    SPI_MASTER_FREQ_10M,
    SPI_MASTER_FREQ_11M,
    SPI_MASTER_FREQ_13M,
    SPI_MASTER_FREQ_16M,
    SPI_MASTER_FREQ_20M,
    SPI_MASTER_FREQ_26M,
    SPI_MASTER_FREQ_40M,
    SPI_MASTER_FREQ_80M,
};

typedef struct {
    uint8_t master_rxbuf[320];
    spi_transaction_t master_trans[16];
    TaskHandle_t handle_slave;
    spi_slave_task_context_t slave_context;
    slave_txdata_t slave_trans[16];
} timing_context_t;

void master_print_data(spi_transaction_t *t, spi_dup_t dup)
{
    if (t->tx_buffer) {
        ESP_LOG_BUFFER_HEX( "master tx", t->tx_buffer, t->length/8 );
    } else {
        ESP_LOGI( "master tx", "no data" );
    }

    int rxlength;
    if (dup!=HALF_DUPLEX_MISO) {
        rxlength = t->length/8;
    } else {
        rxlength = t->rxlength/8;
    }
    if (t->rx_buffer) {
        ESP_LOG_BUFFER_HEX( "master rx", t->rx_buffer, rxlength );
    } else {
        ESP_LOGI( "master rx", "no data" );
    }
}

void slave_print_data(slave_rxdata_t *t)
{
    int rcv_len = (t->len+7)/8;
    ESP_LOGI(SLAVE_TAG, "trans_len: %d", t->len);
    ESP_LOG_BUFFER_HEX( "slave tx", t->tx_start, rcv_len);
    ESP_LOG_BUFFER_HEX( "slave rx", t->data, rcv_len);
}

esp_err_t check_data(spi_transaction_t *t, spi_dup_t dup, slave_rxdata_t *slave_t)
{
    int length;
    if (dup!=HALF_DUPLEX_MISO) {
        length = t->length;
    } else {
        length = t->rxlength;
    }
    TEST_ASSERT(length!=0);

    //currently the rcv_len can be in range of [t->length-1, t->length+3]
    uint32_t rcv_len = slave_t->len;
    TEST_ASSERT(rcv_len >= length-1 && rcv_len <= length+3);

    //the timing speed is temporarily only for master
    if (dup!=HALF_DUPLEX_MISO) {
//        TEST_ASSERT_EQUAL_HEX8_ARRAY(t->tx_buffer, slave_t->data, (t->length+7)/8);
    }
    if (dup!=HALF_DUPLEX_MOSI) {
        TEST_ASSERT_EQUAL_HEX8_ARRAY(slave_t->tx_start, t->rx_buffer, (length+7)/8);
    }
    return ESP_OK;
}

static void timing_init_transactions(spi_dup_t dup, timing_context_t* context)
{
    spi_transaction_t* trans = context->master_trans;
    uint8_t *rx_buf_ptr = context->master_rxbuf;
    if (dup==HALF_DUPLEX_MISO) {
        for (int i = 0; i < 8; i++ ) {
            trans[i] = (spi_transaction_t) {
                .flags = 0,
                .rxlength = 8*(i*2+1),
                .rx_buffer = rx_buf_ptr,
            };
            rx_buf_ptr += ((context->master_trans[i].rxlength + 31)/8)&(~3);
        }
    } else if (dup==HALF_DUPLEX_MOSI) {
        for (int i = 0; i < 8; i++ ) {
            trans[i] = (spi_transaction_t) {
                .flags = 0,
                .length = 8*(i*2+1),
                .tx_buffer = master_send+i,
            };
        }
    } else {
        for (int i = 0; i < 8; i++ ) {
            trans[i] = (spi_transaction_t) {
                .flags = 0,
                .length = 8*(i*2+1),
                .tx_buffer = master_send+i,
                .rx_buffer = rx_buf_ptr,
            };
            rx_buf_ptr += ((context->master_trans[i].length + 31)/8)&(~3);
        }
    }
    //prepare slave tx data
    for (int i = 0; i < 8; i ++) {
        context->slave_trans[i] = (slave_txdata_t) {
            .start = slave_send + 4*(i%3),
            .len = 256,
        };
    }
}

typedef struct {
    const char cfg_name[30];
    /*The test work till the frequency below,
     *set the frequency to higher and remove checks in the driver to know how fast the system can run.
     */
    int freq_limit;
    spi_dup_t dup;
    bool master_iomux;
    bool slave_iomux;
    int slave_tv_ns;
} test_timing_config_t;

#define ESP_SPI_SLAVE_TV    (12.5*3)
#define GPIO_DELAY          (12.5*2)
#define SAMPLE_DELAY        12.5

#define TV_INT_CONNECT_GPIO     (ESP_SPI_SLAVE_TV+GPIO_DELAY)
#define TV_INT_CONNECT          (ESP_SPI_SLAVE_TV)
#define TV_WITH_ESP_SLAVE_GPIO  (ESP_SPI_SLAVE_TV+SAMPLE_DELAY+GPIO_DELAY)
#define TV_WITH_ESP_SLAVE       (ESP_SPI_SLAVE_TV+SAMPLE_DELAY)

//currently ESP32 slave only supports up to 20MHz, but 40MHz on the same board
#define ESP_SPI_SLAVE_MAX_FREQ      SPI_MASTER_FREQ_20M
#define ESP_SPI_SLAVE_MAX_FREQ_SYNC SPI_MASTER_FREQ_40M


static test_timing_config_t timing_master_conf_t[] = {/**/
    { .cfg_name = "FULL_DUP, MASTER IOMUX",
      .freq_limit = SPI_MASTER_FREQ_13M,
      .dup = FULL_DUPLEX,
      .master_iomux = true,
      .slave_iomux = false,
      .slave_tv_ns = TV_INT_CONNECT_GPIO,
    },
    { .cfg_name = "FULL_DUP, SLAVE IOMUX",
      .freq_limit = SPI_MASTER_FREQ_13M,
      .dup = FULL_DUPLEX,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
    },
    { .cfg_name = "FULL_DUP, BOTH GPIO",
      .freq_limit = SPI_MASTER_FREQ_10M,
      .dup = FULL_DUPLEX,
      .master_iomux = false,
      .slave_iomux = false,
      .slave_tv_ns = TV_INT_CONNECT_GPIO,
    },
    { .cfg_name = "HALF_DUP, MASTER IOMUX",
      .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      .dup = HALF_DUPLEX_MISO,
      .master_iomux = true,
      .slave_iomux = false,
      .slave_tv_ns = TV_INT_CONNECT_GPIO,
    },
    { .cfg_name = "HALF_DUP, SLAVE IOMUX",
      .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      .dup = HALF_DUPLEX_MISO,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
    },
    { .cfg_name = "HALF_DUP, BOTH GPIO",
      .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      .dup = HALF_DUPLEX_MISO,
      .master_iomux = false,
      .slave_iomux = false,
      .slave_tv_ns = TV_INT_CONNECT_GPIO,
    },
    { .cfg_name = "MOSI_DUP, MASTER IOMUX",
      .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      .dup = HALF_DUPLEX_MOSI,
      .master_iomux = true,
      .slave_iomux = false,
      .slave_tv_ns = TV_INT_CONNECT_GPIO,
    },
    { .cfg_name = "MOSI_DUP, SLAVE IOMUX",
      .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      .dup = HALF_DUPLEX_MOSI,
      .master_iomux = false,
      .slave_iomux = true,
      .slave_tv_ns = TV_INT_CONNECT,
    },
    { .cfg_name = "MOSI_DUP, BOTH GPIO",
      .freq_limit = ESP_SPI_SLAVE_MAX_FREQ_SYNC,
      .dup = HALF_DUPLEX_MOSI,
      .master_iomux = false,
      .slave_iomux = false,
      .slave_tv_ns = TV_INT_CONNECT_GPIO,
    },
};

//this case currently only checks master read
TEST_CASE("test timing_master","[spi][timeout=120]")
{
    timing_context_t context;

    //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    //slave_pull_up(&slv_buscfg, slvcfg.spics_io_num);

    context.slave_context = (spi_slave_task_context_t){};
    esp_err_t err = init_slave_context( &context.slave_context );
    TEST_ASSERT( err == ESP_OK );

    xTaskCreate( task_slave, "spi_slave", 4096, &context.slave_context, 0, &context.handle_slave);

    const int test_size = sizeof(timing_master_conf_t)/sizeof(test_timing_config_t);
    for (int i = 0; i < test_size; i++) {
        test_timing_config_t* conf = &timing_master_conf_t[i];

        spi_device_handle_t spi;

        timing_init_transactions(conf->dup, &context);

        ESP_LOGI(MASTER_TAG, "****************** %s ***************", conf->cfg_name);
        for (int j=0; j<sizeof(timing_speed_array)/sizeof(int); j++ ) {
            if (timing_speed_array[j] > conf->freq_limit) break;
            ESP_LOGI(MASTER_TAG, "======> %dk", timing_speed_array[j]/1000);

            //master config
            const int master_mode = 0;
            spi_bus_config_t buscfg=SPI_BUS_TEST_DEFAULT_CONFIG();
            spi_device_interface_config_t devcfg=SPI_DEVICE_TEST_DEFAULT_CONFIG();
            devcfg.mode = master_mode;
            if (conf->dup==HALF_DUPLEX_MISO||conf->dup==HALF_DUPLEX_MOSI) {
                devcfg.cs_ena_pretrans = 20;
                devcfg.flags |= SPI_DEVICE_HALFDUPLEX;
            } else {
                devcfg.cs_ena_pretrans = 1;
            }
            devcfg.cs_ena_posttrans = 20;
            devcfg.input_delay_ns = conf->slave_tv_ns;
            devcfg.clock_speed_hz = timing_speed_array[j];

            //slave config
            int slave_mode = 0;
            spi_slave_interface_config_t slvcfg=SPI_SLAVE_TEST_DEFAULT_CONFIG();
            slvcfg.mode = slave_mode;

            //pin config & initialize
            //we can't have two sets of iomux pins on the same pins
            assert(!conf->master_iomux || !conf->slave_iomux);
            if (conf->slave_iomux) {
                //only in this case, use VSPI iomux pins
                buscfg.miso_io_num = VSPI_IOMUX_PIN_NUM_MISO;
                buscfg.mosi_io_num = VSPI_IOMUX_PIN_NUM_MOSI;
                buscfg.sclk_io_num = VSPI_IOMUX_PIN_NUM_CLK;
                devcfg.spics_io_num = VSPI_IOMUX_PIN_NUM_CS;
                slvcfg.spics_io_num = VSPI_IOMUX_PIN_NUM_CS;
            } else {
                buscfg.miso_io_num = HSPI_IOMUX_PIN_NUM_MISO;
                buscfg.mosi_io_num = HSPI_IOMUX_PIN_NUM_MOSI;
                buscfg.sclk_io_num = HSPI_IOMUX_PIN_NUM_CLK;
                devcfg.spics_io_num = HSPI_IOMUX_PIN_NUM_CS;
                slvcfg.spics_io_num = HSPI_IOMUX_PIN_NUM_CS;
            }
            slave_pull_up(&buscfg, slvcfg.spics_io_num);

            //this does nothing, but avoid the driver from using iomux pins if required
            buscfg.quadhd_io_num = (!conf->master_iomux && !conf->slave_iomux? VSPI_IOMUX_PIN_NUM_MISO: -1);
            TEST_ESP_OK(spi_bus_initialize(HSPI_HOST, &buscfg, 0));
            TEST_ESP_OK(spi_bus_add_device(HSPI_HOST, &devcfg, &spi));
            //slave automatically use iomux pins if pins are on VSPI_* pins
            buscfg.quadhd_io_num = -1;
            TEST_ESP_OK( spi_slave_initialize(VSPI_HOST, &buscfg, &slvcfg, 0) );

            //initialize master and slave on the same pins break some of the output configs, fix them
            if (conf->master_iomux) {
                gpio_output_sel(buscfg.mosi_io_num, FUNC_SPI, HSPID_OUT_IDX);
                gpio_output_sel(buscfg.miso_io_num, FUNC_GPIO, VSPIQ_OUT_IDX);
                gpio_output_sel(devcfg.spics_io_num, FUNC_SPI, HSPICS0_OUT_IDX);
                gpio_output_sel(buscfg.sclk_io_num, FUNC_SPI, HSPICLK_OUT_IDX);
            } else if (conf->slave_iomux) {
                gpio_output_sel(buscfg.mosi_io_num, FUNC_GPIO, HSPID_OUT_IDX);
                gpio_output_sel(buscfg.miso_io_num, FUNC_SPI, VSPIQ_OUT_IDX);
                gpio_output_sel(devcfg.spics_io_num, FUNC_GPIO, HSPICS0_OUT_IDX);
                gpio_output_sel(buscfg.sclk_io_num, FUNC_GPIO, HSPICLK_OUT_IDX);
            } else {
                gpio_output_sel(buscfg.mosi_io_num, FUNC_GPIO, HSPID_OUT_IDX);
                gpio_output_sel(buscfg.miso_io_num, FUNC_GPIO, VSPIQ_OUT_IDX);
                gpio_output_sel(devcfg.spics_io_num, FUNC_GPIO, HSPICS0_OUT_IDX);
                gpio_output_sel(buscfg.sclk_io_num, FUNC_GPIO, HSPICLK_OUT_IDX);
            }

            //clear master receive buffer
            memset(context.master_rxbuf, 0x66, sizeof(context.master_rxbuf));

            //prepare slave tx data
            for (int k = 0; k < 8; k ++) xQueueSend( context.slave_context.data_to_send, &context.slave_trans[k], portMAX_DELAY );

            for( int k= 0; k < 8; k ++ ) {
                //wait for both master and slave end
                ESP_LOGI( MASTER_TAG, "=> test%d", k );
                //send master tx data
                vTaskDelay(9);

                spi_transaction_t *t = &context.master_trans[k];
                TEST_ESP_OK (spi_device_transmit( spi, t) );
                master_print_data(t, conf->dup);

                size_t rcv_len;
                slave_rxdata_t *rcv_data = xRingbufferReceive( context.slave_context.data_received, &rcv_len, portMAX_DELAY );
                slave_print_data(rcv_data);

                //check result
                TEST_ESP_OK(check_data(t, conf->dup, rcv_data));
                //clean
                vRingbufferReturnItem(context.slave_context.data_received, rcv_data);
            }
            master_deinit(spi);
            TEST_ASSERT(spi_slave_free(VSPI_HOST) == ESP_OK);
        }
    }

    vTaskDelete( context.handle_slave );
    context.handle_slave = 0;

    deinit_slave_context(&context.slave_context);

    ESP_LOGI(MASTER_TAG, "test passed.");
}

/********************************************************************************
 *      Test SPI transaction interval
 ********************************************************************************/
#define RECORD_TIME_PREPARE() uint32_t __t1, __t2
#define RECORD_TIME_START()   do {__t1 = xthal_get_ccount();}while(0)
#define RECORD_TIME_END(p_time) do{__t2 = xthal_get_ccount(); *p_time = (__t2-__t1)/240;}while(0)

static void speed_setup(spi_device_handle_t* spi, bool use_dma)
{
    esp_err_t ret;
    spi_bus_config_t buscfg=SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_device_interface_config_t devcfg=SPI_DEVICE_TEST_DEFAULT_CONFIG();
    devcfg.queue_size=8;       //We want to be able to queue 7 transactions at a time

    //Initialize the SPI bus and the device to test
    ret=spi_bus_initialize(HSPI_HOST, &buscfg, (use_dma?1:0));
    TEST_ASSERT(ret==ESP_OK);
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, spi);
    TEST_ASSERT(ret==ESP_OK);
}

static void speed_deinit(spi_device_handle_t spi)
{
    TEST_ESP_OK( spi_bus_remove_device(spi) );
    TEST_ESP_OK( spi_bus_free(HSPI_HOST) );
}

static void sorted_array_insert(uint32_t* array, int* size, uint32_t item)
{
    int pos;
    for (pos = *size; pos>0; pos--) {
        if (array[pos-1] < item) break;
        array[pos] = array[pos-1];
    }
    array[pos]=item;
    (*size)++;
}

#define TEST_TIMES  11

TEST_CASE("spi_speed","[spi]")
{
    RECORD_TIME_PREPARE();
    uint32_t t_flight;
    //to get rid of the influence of randomly interrupts, we measured the performance by median value
    uint32_t t_flight_sorted[TEST_TIMES];
    int t_flight_num = 0;

    spi_device_handle_t spi;
    const bool use_dma = true;
    WORD_ALIGNED_ATTR spi_transaction_t trans = {
        .length = 1*8,
        .flags = SPI_TRANS_USE_TXDATA,
    };

    //first work with DMA
    speed_setup(&spi, use_dma);

    //first time introduces a device switch, which costs more time. we skip this
    spi_device_transmit(spi, &trans);

    //record flight time by isr, with DMA
    t_flight_num = 0;
    for (int i = 0; i < TEST_TIMES; i++) {
        spi_device_transmit(spi, &trans); // prime the flash cache
        RECORD_TIME_START();
        spi_device_transmit(spi, &trans);
        RECORD_TIME_END(&t_flight);
        sorted_array_insert(t_flight_sorted, &t_flight_num, t_flight);
    }
    TEST_PERFORMANCE_LESS_THAN(SPI_PER_TRANS_NO_POLLING, "%d us", t_flight_sorted[(TEST_TIMES+1)/2]);
    for (int i = 0; i < TEST_TIMES; i++) {
        ESP_LOGI(TAG, "%d", t_flight_sorted[i]);
    }

    speed_deinit(spi);
    speed_setup(&spi, !use_dma);

    //first time introduces a device switch, which costs more time. we skip this
    spi_device_transmit(spi, &trans);

    //record flight time by isr, without DMA
    t_flight_num = 0;
    for (int i = 0; i < TEST_TIMES; i++) {
        RECORD_TIME_START();
        spi_device_transmit(spi, &trans);
        RECORD_TIME_END(&t_flight);
        sorted_array_insert(t_flight_sorted, &t_flight_num, t_flight);
    }
    TEST_PERFORMANCE_LESS_THAN( SPI_PER_TRANS_NO_POLLING_NO_DMA, "%d us", t_flight_sorted[(TEST_TIMES+1)/2]);
    for (int i = 0; i < TEST_TIMES; i++) {
        ESP_LOGI(TAG, "%d", t_flight_sorted[i]);
    }
    speed_deinit(spi);
}

