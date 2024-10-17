
#include "sht21.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_check.h"
#include <assert.h>

//==============================================================================
// DEFINES - MACROS
//==============================================================================

#define I2C_ADDRESS 0x40            // i2c address of SHT21
#define I2C_MASTER_RX_BUF_DISABLE 0 // receiving buffer size
#define I2C_MASTER_TX_BUF_DISABLE 0 // sending buffer size
#define I2C_TIMEOUT_MS 200 // max waiting time before issuing an i2c timeout

// See Datasheet Page 9 Table 7
#define SHT21_DELAY_T_MEASUREMENT 85  // max delay for temp meas. in ms
#define SHT21_DELAY_RH_MEASUREMENT 29 // max delay for humidity meas. in ms

// See Datasheet Page 9 Section 5.5
#define SHT21_DELAY_SOFT_RESET 15 // max delay for soft reset

// See Datasheet Page 9 Section 5.7
#define CRC_POLYNOMIAL 0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

#define ER(x) ESP_RETURN_ON_ERROR(x, __FILE__, "")

//==============================================================================
// TYPEDEFS
//==============================================================================

// clang-format off
typedef enum {
    SHT21_CMD_TRIG_T_MEASUREMENT_HM   = 0xE3, // command trig. temp meas. hold master
    SHT21_CMD_TRIG_RH_MEASUREMENT_HM  = 0xE5, // command trig. humidity meas. hold master
    SHT21_CMD_TRIG_T_MEASUREMENT_NHM  = 0xF3, // command trig. temp meas. no hold master
    SHT21_CMD_TRIG_RH_MEASUREMENT_NHM = 0xF5, // command trig. humid. meas. no hold master
    SHT21_CMD_SOFT_RESET              = 0xFE  // command soft reset
} sht21_command_t; /* See Datasheet Page 8 Table 6 */
// clang-format on

//==============================================================================
// STATIC VARIABLES - STATIC PROTOTYPES
//==============================================================================

static i2c_port_t i2c_port;

static TickType_t get_delay_for_measurement(sht21_command_t command);

static esp_err_t read_sensor(uint16_t *dst, sht21_command_t command);

static esp_err_t crc_checksum(uint8_t data_arr[], uint8_t data_len,
                              uint8_t checksum);

//==============================================================================
// GLOBAL FUNCTIONS
//==============================================================================

esp_err_t sht21_init(i2c_port_t i2c_num, gpio_num_t sda_pin, gpio_num_t scl_pin,
                     sht21_i2c_speed_t i2c_speed)
{
    i2c_port = i2c_num;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin,
        .scl_io_num = scl_pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = i2c_speed,
    };

    ER(i2c_param_config(i2c_port, &conf));
    ER(i2c_driver_install(i2c_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE,
                          I2C_MASTER_TX_BUF_DISABLE, 0));
    return ESP_OK;
}

esp_err_t sht21_get_temperature(float *dst)
{
    uint16_t raw_reading;
    ER(read_sensor(&raw_reading, SHT21_CMD_TRIG_T_MEASUREMENT_NHM));
    *dst = -46.85 + 175.72 / 65536 * (float)raw_reading; // Datasheet Page 10
    return ESP_OK;
}

esp_err_t sht21_get_humidity(float *dst)
{
    uint16_t raw_reading;
    ER(read_sensor(&raw_reading, SHT21_CMD_TRIG_RH_MEASUREMENT_NHM));
    *dst = -6.0 + 125.0 / 65536 * (float)raw_reading; // Datasheet Page 10
    return ESP_OK;
}

esp_err_t sht21_soft_reset(void)
{
    i2c_cmd_handle_t write_cmd = i2c_cmd_link_create();
    ER(i2c_master_start(write_cmd));
    ER(i2c_master_write_byte(write_cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE,
                             I2C_MASTER_ACK));
    ER(i2c_master_write_byte(write_cmd, SHT21_CMD_SOFT_RESET, I2C_MASTER_ACK));
    ER(i2c_master_stop(write_cmd));

    esp_err_t err = i2c_master_cmd_begin(i2c_port, write_cmd,
                                         I2C_TIMEOUT_MS / portTICK_RATE_MS);
    i2c_cmd_link_delete(write_cmd);
    ER(err);

    vTaskDelay(SHT21_DELAY_SOFT_RESET / portTICK_PERIOD_MS);
    return ESP_OK;
}

esp_err_t sht21_deinit(void)
{
    ER(i2c_driver_delete(i2c_port));
    return ESP_OK;
}

//==============================================================================
// STATIC FUNCTIONS
//==============================================================================

static TickType_t get_delay_for_measurement(sht21_command_t command)
{
    switch (command)
    {
    case SHT21_CMD_TRIG_T_MEASUREMENT_HM:
    case SHT21_CMD_TRIG_T_MEASUREMENT_NHM:
        return SHT21_DELAY_T_MEASUREMENT;
    case SHT21_CMD_TRIG_RH_MEASUREMENT_HM:
    case SHT21_CMD_TRIG_RH_MEASUREMENT_NHM:
        return SHT21_DELAY_RH_MEASUREMENT;
    default:
        assert(0);
    }
}

static esp_err_t read_sensor(uint16_t *dst, sht21_command_t command)
{
    uint8_t data_arr[2];
    uint8_t checksum;
    esp_err_t err = ESP_OK;

    i2c_cmd_handle_t write_cmd = i2c_cmd_link_create();
    ER(i2c_master_start(write_cmd));
    ER(i2c_master_write_byte(write_cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE,
                             I2C_MASTER_ACK));
    ER(i2c_master_write_byte(write_cmd, command, I2C_MASTER_ACK));
    ER(i2c_master_stop(write_cmd));

    err = i2c_master_cmd_begin(i2c_port, write_cmd,
                               I2C_TIMEOUT_MS / portTICK_RATE_MS);
    i2c_cmd_link_delete(write_cmd);
    ER(err);

    vTaskDelay(get_delay_for_measurement(command) / portTICK_PERIOD_MS);

    i2c_cmd_handle_t read_cmd = i2c_cmd_link_create();
    ER(i2c_master_start(read_cmd));
    ER(i2c_master_write_byte(read_cmd, (I2C_ADDRESS << 1) | I2C_MASTER_READ,
                             I2C_MASTER_ACK));
    ER(i2c_master_read(read_cmd, data_arr, sizeof(data_arr), I2C_MASTER_ACK));
    ER(i2c_master_read_byte(read_cmd, &checksum, I2C_MASTER_NACK));
    ER(i2c_master_stop(read_cmd));

    err = i2c_master_cmd_begin(i2c_port, read_cmd,
                               I2C_TIMEOUT_MS / portTICK_RATE_MS);
    i2c_cmd_link_delete(read_cmd);
    ER(err);

    ER(crc_checksum(data_arr, sizeof(data_arr), checksum));

    // The last two bits are used for transmitting status. See Datasheet Page 8
    *dst = (data_arr[0] << 8) | (data_arr[1] & 0xFC);
    return ESP_OK;
}

// Calculates 8-Bit checksum with given polynomial
static esp_err_t crc_checksum(uint8_t data_arr[], uint8_t data_len,
                              uint8_t checksum)
{
    uint8_t crc = 0;
    for (uint8_t byte_ctr = 0; byte_ctr < data_len; ++byte_ctr)
    {
        crc ^= (data_arr[byte_ctr]);
        for (uint8_t bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ CRC_POLYNOMIAL;
            else
                crc = (crc << 1);
        }
    }

    if (crc != checksum)
        return ESP_ERR_INVALID_CRC;
    else
        return ESP_OK;
}