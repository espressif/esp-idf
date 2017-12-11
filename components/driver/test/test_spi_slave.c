/*
 Tests for the spi_slave device driver
*/

#include <string.h>
#include "unity.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "esp_log.h"

#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22

static const char MASTER_TAG[] = "test_master";
static const char SLAVE_TAG[] = "test_slave";

#define MASTER_SEND {0x93, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0xaa, 0xcc, 0xff, 0xee, 0x55, 0x77, 0x88, 0x43}
#define SLAVE_SEND { 0xaa, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10, 0x13, 0x57, 0x9b, 0xdf, 0x24, 0x68, 0xac, 0xe0 }

static inline void int_connect( uint32_t gpio, uint32_t sigo, uint32_t sigi ) 
{
    gpio_matrix_out( gpio, sigo, false, false );
    gpio_matrix_in( gpio, sigi, false );
}

static void master_init_nodma( spi_device_handle_t* spi)
{
    esp_err_t ret;
    spi_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO, 
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };
    spi_device_interface_config_t devcfg={
        .clock_speed_hz=4*1000*1000,            //currently only up to 4MHz for internel connect
        .mode=0,                                //SPI mode 0
        .spics_io_num=PIN_NUM_CS,               //CS pin
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
        .pre_cb=NULL,  
        .cs_ena_posttrans=1,
    };
    //Initialize the SPI bus
    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 0);
    TEST_ASSERT(ret==ESP_OK);
    //Attach the LCD to the SPI bus
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, spi);
    TEST_ASSERT(ret==ESP_OK);
}

static void slave_init()
{
    //Configuration for the SPI bus
    spi_bus_config_t buscfg={
        .mosi_io_num=PIN_NUM_MOSI,
        .miso_io_num=PIN_NUM_MISO,
        .sclk_io_num=PIN_NUM_CLK
    };
    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg={
        .mode=0,
        .spics_io_num=PIN_NUM_CS,
        .queue_size=3,
        .flags=0,
    };
    //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(PIN_NUM_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_CLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_CS, GPIO_PULLUP_ONLY);
    //Initialize SPI slave interface
    TEST_ESP_OK( spi_slave_initialize(VSPI_HOST, &buscfg, &slvcfg, 2) );
}

TEST_CASE("test slave startup","[spi]")
{
    uint8_t master_txbuf[320]=MASTER_SEND;
    uint8_t master_rxbuf[320];
    uint8_t slave_txbuf[320]=SLAVE_SEND;
    uint8_t slave_rxbuf[320];

    spi_device_handle_t spi;
    //initial master
    master_init_nodma( &spi );
    //initial slave
    slave_init();

    //do internal connection
    int_connect( PIN_NUM_MOSI,  HSPID_OUT_IDX,   VSPIQ_IN_IDX );
    int_connect( PIN_NUM_MISO,  VSPIQ_OUT_IDX,   HSPID_IN_IDX );
    int_connect( PIN_NUM_CS,    HSPICS0_OUT_IDX, VSPICS0_IN_IDX );
    int_connect( PIN_NUM_CLK,   HSPICLK_OUT_IDX, VSPICLK_IN_IDX );

    for ( int i = 0; i < 3; i ++ ) {
        //slave send
        spi_slave_transaction_t slave_t;
        spi_slave_transaction_t* out;
        memset(&slave_t, 0, sizeof(spi_slave_transaction_t));
        slave_t.length=8*32;
        slave_t.tx_buffer=slave_txbuf+2*i;
        slave_t.rx_buffer=slave_rxbuf;
        TEST_ESP_OK( spi_slave_queue_trans( VSPI_HOST, &slave_t, portMAX_DELAY ) );

        //send
        spi_transaction_t t = {};
        t.length = 32*(i+1);
        if ( t.length != 0 ) {
            t.tx_buffer = master_txbuf+i;  
            t.rx_buffer = master_rxbuf+i;
        }
        spi_device_transmit( spi, (spi_transaction_t*)&t );

        //wait for end
        TEST_ESP_OK( spi_slave_get_trans_result( VSPI_HOST, &out, portMAX_DELAY ) );

        //show result
        ESP_LOGI(SLAVE_TAG, "trans_len: %d", slave_t.trans_len);
        ESP_LOG_BUFFER_HEX( "master tx", t.tx_buffer, t.length/8 );
        ESP_LOG_BUFFER_HEX( "master rx", t.rx_buffer, t.length/8 );
        ESP_LOG_BUFFER_HEX( "slave tx", slave_t.tx_buffer, (slave_t.trans_len+7)/8);
        ESP_LOG_BUFFER_HEX( "slave rx", slave_t.rx_buffer, (slave_t.trans_len+7)/8);

        TEST_ASSERT_EQUAL_HEX8_ARRAY( t.tx_buffer, slave_t.rx_buffer, t.length/8 );
        TEST_ASSERT_EQUAL_HEX8_ARRAY( slave_t.tx_buffer, t.rx_buffer, t.length/8 );

        TEST_ASSERT_EQUAL( t.length, slave_t.trans_len );

        //clean
        memset( master_rxbuf, 0x66, sizeof(master_rxbuf));
        memset( slave_rxbuf, 0x66, sizeof(slave_rxbuf));
    }
    
    TEST_ASSERT(spi_slave_free(VSPI_HOST) == ESP_OK);
  
    TEST_ASSERT(spi_bus_remove_device(spi) == ESP_OK);
    TEST_ASSERT(spi_bus_free(HSPI_HOST) == ESP_OK);

    ESP_LOGI(MASTER_TAG, "test passed.");
}
