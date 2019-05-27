#include "sdkconfig.h"
#include "bootloader_clock.h"

int bootloader_clock_get_rated_freq_mhz()
{
#ifndef CONFIG_HARDWARE_IS_FPGA
    #warning "FIXME this needs to be filled in for real hardware"
#endif
    return 999;
}


