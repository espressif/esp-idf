#include "soc/efuse_reg.h"
#include "bootloader_clock.h"

int bootloader_clock_get_rated_freq_mhz()
{
    //Check if ESP32 is rated for a CPU frequency of 160MHz only
    if (REG_GET_BIT(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_CPU_FREQ_RATED) &&
        REG_GET_BIT(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_CPU_FREQ_LOW)) {
        return 160;
    }
    return 240;
}

