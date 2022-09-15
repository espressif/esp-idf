/* This file is from test_uart.c, but mainly about RS485 */


#include <string.h>
#include <sys/param.h>
#include "unity.h"
#include "test_utils.h"             // unity_send_signal
#include "driver/uart.h"            // for the uart driver access
#include "esp_log.h"
#include "esp_system.h"             // for uint32_t esp_random()

#define UART_NUM1       (UART_NUM_1)
#define UART_BAUD_RATE  (115200 * 10)
#define BUF_SIZE        (512)
#define UART1_RX_PIN    (22)
#define UART1_TX_PIN    (23)

// RTS for RS485 Half-Duplex Mode manages DE/~RE
#define UART1_RTS_PIN   (18)

// Number of packets to be send during test
#define PACKETS_NUMBER  (30)

// Wait timeout for uart driver
#define PACKET_READ_TICS        (2000 / portTICK_PERIOD_MS)
// This is for workarond to avoid master-slave syncronization issues
// when slave gets the "Master_started" signal with delay ~2-3 seconds
#define TEST_ALLOW_PROC_FAIL    (10)
#define TEST_CHECK_PROC_FAIL(fails, threshold) TEST_ASSERT((fails * 100 / PACKETS_NUMBER) <= threshold)

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2)
//No runners

static const char *TAG = "rs485_test";

// The table for fast CRC16 calculation
static const uint8_t crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
    0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80,
    0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00,
    0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81,
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1,
    0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81,
    0x40
};

static const uint8_t crc_low[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4,
    0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB,
    0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE,
    0xDF, 0x1F, 0xDD,
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2,
    0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7,
    0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E,
    0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B,
    0x2A, 0xEA, 0xEE,
    0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27,
    0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2,
    0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD,
    0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8,
    0xB9, 0x79, 0xBB,
    0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4,
    0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94,
    0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59,
    0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D,
    0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80,
    0x40
};


// Calculate buffer checksum using tables
// The checksum CRC16 algorithm is specific
// for Modbus standard and uses polynomial value = 0xA001
static uint16_t get_buffer_crc16( uint8_t * frame_ptr, uint16_t length )
{
    TEST_ASSERT( frame_ptr != NULL);

    uint8_t crc_hi_byte = 0xFF;
    uint8_t crc_low_byte = 0xFF;
    int index;

    while ( length-- )
    {
        index = crc_low_byte ^ *(frame_ptr++);
        crc_low_byte = crc_hi_byte ^ crc_hi[index];
        crc_hi_byte = crc_low[index];
    }
    return ((crc_hi_byte << 8) | crc_low_byte);
}

// Fill the buffer with random numbers and apply CRC16 at the end
static uint16_t buffer_fill_random(uint8_t *buffer, size_t length)
{
    TEST_ASSERT( buffer != NULL);
    // Packet is too short
    if (length < 4) {
        return 0;
    }
    for (int i = 0; i < length; i += 4) {
        uint32_t random = esp_random();
        memcpy(buffer + i, &random, MIN(length - i, 4));
    }
    // Get checksum of the buffer
    uint16_t crc = get_buffer_crc16((uint8_t*)buffer, (length - 2));
    // Apply checksum bytes into packet
    buffer[length - 2] = (uint8_t)(crc & 0xFF);         // Set Low byte CRC
    buffer[length - 1] = (uint8_t)(crc >> 8);           // Set High byte CRC
    return crc;
}

static void rs485_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };
    ESP_LOGI(TAG, "RS485 port initialization...");
    TEST_ESP_OK(uart_wait_tx_idle_polling(UART_NUM1));
    // Configure UART1 parameters
    TEST_ESP_OK(uart_param_config(UART_NUM1, &uart_config));
    // Set UART1 pins
    TEST_ESP_OK(uart_set_pin(UART_NUM1, UART1_TX_PIN, UART1_RX_PIN, UART1_RTS_PIN, UART_PIN_NO_CHANGE));
    // Install UART driver (we don't need an event queue here)
    TEST_ESP_OK(uart_driver_install(UART_NUM1, BUF_SIZE * 2, 0, 0, NULL, 0));
    // Setup rs485 half duplex mode
    TEST_ESP_OK(uart_set_mode(UART_NUM1, UART_MODE_RS485_HALF_DUPLEX));
}

static esp_err_t print_packet_data(const char *str, uint8_t *buffer, uint16_t buffer_size)
{
    TEST_ASSERT( buffer != NULL);
    TEST_ASSERT( str != NULL);

    // Calculate the checksum of the buffer
    uint16_t crc16_calc = get_buffer_crc16(buffer, (buffer_size - 2));
    uint16_t crc16_in = ((uint16_t)(buffer[buffer_size - 1]) << 8) | buffer[buffer_size - 2];
    const char* state_str = (crc16_in != crc16_calc) ? "incorrect " : "correct ";
    // Print an array of data
    printf("%s%s RS485 packet = [ ", str, state_str);
    for (int i = 0; i < buffer_size; i++) {
            printf("0x%.2X ", (uint8_t)buffer[i]);
    }
    printf(" ]\r\n");
    printf("crc_in = 0x%.4X\r\n", (uint16_t)crc16_in);
    printf("crc_calc = 0x%.4X\r\n", (uint16_t)crc16_calc);
    esp_err_t result = (crc16_in != crc16_calc) ? ESP_ERR_INVALID_CRC : ESP_OK;
    return result;
}

// Slave test case for multi device
static void rs485_slave(void)
{
    rs485_init();
    uint8_t* slave_data = (uint8_t*) malloc(BUF_SIZE);
    uint16_t err_count = 0, good_count = 0;
    unity_send_signal("Slave_ready");
    unity_wait_for_signal("Master_started");
    ESP_LOGI(TAG, "Start recieve loop.");
    for(int pack_count = 0; pack_count < PACKETS_NUMBER; pack_count++) {
        //Read slave_data from UART
        int len = uart_read_bytes(UART_NUM1, slave_data, BUF_SIZE, PACKET_READ_TICS);
        //Write slave_data back to UART
        if (len > 2) {
            esp_err_t status = print_packet_data("Received ", slave_data, len);

            // If received packet is correct then send it back
            if (status == ESP_OK) {
                uart_write_bytes(UART_NUM1, (char*)slave_data, len);
                uart_wait_tx_idle_polling(UART_NUM1);
                good_count++;
            } else {
                printf("Incorrect packet received.\r\n");
                err_count++;
            }
        } else {
            printf("Incorrect data packet[%d], data length: %d, received.\r\n", pack_count, len);
            err_count++;
        }
    }
    ESP_LOGI(TAG, "Test completed. Received packets = %d, errors = %d", good_count, err_count);
    // Wait for packet to be sent
    uart_wait_tx_done(UART_NUM1, PACKET_READ_TICS);
    free(slave_data);
    uart_driver_delete(UART_NUM1);
    TEST_CHECK_PROC_FAIL(err_count, TEST_ALLOW_PROC_FAIL);
}

// Master test of multi device test case.
// It forms packet with random data, apply generated CRC16 and sends to slave.
// If response recieved correctly from slave means RS485 channel works.
static void rs485_master(void)
{
    uint16_t err_count = 0, good_count = 0;
    rs485_init();
    uint8_t* master_buffer = (uint8_t*) malloc(BUF_SIZE);
    uint8_t* slave_buffer = (uint8_t*) malloc(BUF_SIZE);
    // The master test case should be synchronized with slave
    unity_wait_for_signal("Slave_ready");
    unity_send_signal("Master_started");
    ESP_LOGI(TAG, "Start recieve loop.");
    for(int i = 0; i < PACKETS_NUMBER; i++) {
        // Form random buffer with CRC16
        buffer_fill_random(master_buffer, BUF_SIZE);
        // Print created packet for debugging
        esp_err_t status = print_packet_data("Send ", master_buffer, BUF_SIZE);
        TEST_ASSERT(status == ESP_OK);
        uart_write_bytes(UART_NUM1, (char*)master_buffer, BUF_SIZE);
        uart_wait_tx_idle_polling(UART_NUM1);
        // Read translated packet from slave
        int len = uart_read_bytes(UART_NUM1, slave_buffer, BUF_SIZE, PACKET_READ_TICS);
        // Check if the received packet is too short
        if (len > 2) {
            // Print received packet and check checksum
            esp_err_t status = print_packet_data("Received ", slave_buffer, len);
            if (status == ESP_OK) {
                good_count++;
                printf("Received: %d\r\n", good_count);
            } else {
                err_count++;
                printf("Errors: %d\r\n", err_count);
            }
        }
        else {
            printf("Incorrect answer from slave.\r\n");
            err_count++;
        }
    }
    uart_wait_tx_done(UART_NUM1, PACKET_READ_TICS);
    // Free the buffer and delete driver at the end
    free(master_buffer);
    free(slave_buffer);
    uart_driver_delete(UART_NUM1);
    ESP_LOGI(TAG, "Test completed. Received packets = %d, errors = %d", good_count, err_count);
    TEST_CHECK_PROC_FAIL(err_count, TEST_ALLOW_PROC_FAIL);
}

/*
 * This multi devices test case verifies RS485 mode of the uart driver and checks
 * correctness of RS485 interface channel communication. It requires
 * RS485 bus driver hardware to be connected to boards.
*/
TEST_CASE_MULTIPLE_DEVICES("RS485 half duplex uart multiple devices test.", "[driver_RS485][test_env=UT_T2_RS485]", rs485_master, rs485_slave);

#endif