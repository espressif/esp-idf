// The long term plan is to have a single soc_caps.h for each peripheral.
// During the refactoring and multichip support development process, we
// seperate these information into periph_caps.h for each peripheral and
// include them here.

#pragma once

#define SOC_CPU_CORES_NUM 1
#define SOC_GDMA_SUPPORTED 1

// Attention: These fixed DMA channels are temporarily workaround before we have a centralized DMA controller API to help alloc the channel dynamically
// Remove them when GDMA driver API is ready
#define SOC_GDMA_M2M_DMA_CHANNEL    (0)
#define SOC_GDMA_SHA_DMA_CHANNEL    (1)
#define SOC_GDMA_SPI2_DMA_CHANNEL   (2)

//NOTE: The CHx number should be consistent with the selected DMA channel above
#define SOC_GDMA_SPI2_INTR_SOURCE   ETS_DMA_CH2_INTR_SOURCE
//On C3, there is only 1 GPSPI controller (GPSPI2)
#define SOC_GDMA_SPI3_DMA_CHANNEL   SOC_GDMA_SPI2_DMA_CHANNEL

#include "rmt_caps.h"
#include "adc_caps.h"
#include "dac_caps.h"
#include "i2c_caps.h"
#include "mpu_caps.h"
#include "rsa_caps.h"
#include "sigmadelta_caps.h"
#include "systimer_caps.h"
#include "uart_caps.h"
#include "brownout_caps.h"
#include "gdma_caps.h"
#include "i2s_caps.h"
#include "rtc_io_caps.h"
#include "soc_caps.h"
#include "timer_group_caps.h"
#include "cpu_caps.h"
#include "gpio_caps.h"
#include "ledc_caps.h"
#include "rmt_caps.h"
#include "sha_caps.h"
#include "spi_caps.h"

/*-------------------------- TOUCH SENSOR CAPS -------------------------------*/
#define SOC_TOUCH_SENSOR_NUM            (0)    /*! No touch sensors on ESP32-C3 */
