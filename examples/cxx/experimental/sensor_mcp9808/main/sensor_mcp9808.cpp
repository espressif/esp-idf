#include <iostream>
#include "i2c_cxx.hpp"

using namespace std;
using namespace idf;

#define ADDR 0x18
#define I2C_MASTER_NUM I2C_NUM_0   /*!< I2C port number for master dev */
#define I2C_MASTER_SCL_IO    19    /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO    18    /*!< gpio number for I2C master data  */

#define MCP_9808_TEMP_REG 0x05

/**
 * Calculates the temperature of the MCP9808 from the read msb and lsb. Loosely adapted from the MCP9808's datasheet.
 */
float calc_temp(uint8_t msb, uint8_t lsb) {
    float temperature;
    msb &= 0x1F;
    bool sign = msb & 0x10;
    if (sign) {
        msb &= 0x0F;
        temperature = 256 - (msb * 16 + (float) lsb / 16);
    } else {
        temperature = (msb * 16 + (float) lsb / 16);
    }

    return temperature;
}

extern "C" void app_main(void)
{
    try {
        // creating master bus, writing temperature register pointer and reading the value
        shared_ptr<I2CMaster> master(new I2CMaster(I2C_MASTER_NUM, I2C_MASTER_SCL_IO, I2C_MASTER_SDA_IO, 400000));
        master->sync_write(ADDR, {MCP_9808_TEMP_REG});
        vector<uint8_t> data = master->sync_read(ADDR, 2);

        cout << "Current temperature: " << calc_temp(data[0], data[1]) << endl;
    } catch (const I2CException &e) {
        cout << "I2C Exception with error: "  << e.what();
        cout << " (" << e.error<< ")" << endl;
        cout << "Coulnd't read sensor!" << endl;
    }
}
