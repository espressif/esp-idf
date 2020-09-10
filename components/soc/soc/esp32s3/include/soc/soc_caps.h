// The long term plan is to have a single soc_caps.h for all peripherals.
// During the refactoring and multichip support development process, we
// separate these information into periph_caps.h for each peripheral and
// include them here to avoid developing conflicts.

#pragma once

/*-------------------------- COMMON CAPS ---------------------------------------*/
#define SOC_TWAI_SUPPORTED 1
#define SOC_GDMA_SUPPORTED 1
#define SOC_CPU_CORES_NUM 2
#define SOC_CACHE_SUPPORT_WRAP    1

/*-------------------------- ADC CAPS ----------------------------------------*/
#include "adc_caps.h"

/*-------------------------- BROWNOUT CAPS -----------------------------------*/
#include "brownout_caps.h"

/*-------------------------- CPU CAPS ----------------------------------------*/
#include "cpu_caps.h"

/*-------------------------- DAC CAPS ----------------------------------------*/
#include "dac_caps.h"

/*-------------------------- GDMA CAPS ---------------------------------------*/
#include "gdma_caps.h"

/*-------------------------- GPIO CAPS ---------------------------------------*/
#include "gpio_caps.h"

/*-------------------------- I2C CAPS ----------------------------------------*/
#include "i2c_caps.h"

/*-------------------------- I2S CAPS ----------------------------------------*/
#include "i2s_caps.h"

/*-------------------------- LEDC CAPS ---------------------------------------*/
#include "ledc_caps.h"

/*-------------------------- MPU CAPS ----------------------------------------*/
#include "mpu_caps.h"

/*-------------------------- PCNT CAPS ---------------------------------------*/
#include "pcnt_caps.h"

/*-------------------------- RMT CAPS ----------------------------------------*/
#include "rmt_caps.h"

/*-------------------------- RTCIO CAPS --------------------------------------*/
#include "rtc_io_caps.h"

/*-------------------------- SIGMA DELTA CAPS --------------------------------*/
#include "sigmadelta_caps.h"

/*-------------------------- SPI CAPS ----------------------------------------*/
#include "spi_caps.h"

/*-------------------------- SYS TIMER CAPS ----------------------------------*/
#include "systimer_caps.h"

/*-------------------------- TIMER GROUP CAPS --------------------------------*/
#include "timer_group_caps.h"

/*-------------------------- TOUCH SENSOR CAPS -------------------------------*/
#include "touch_sensor_caps.h"

/*-------------------------- TWAI CAPS ---------------------------------------*/
#include "twai_caps.h"

/*-------------------------- UART CAPS ---------------------------------------*/
#include "uart_caps.h"

// Attention: These fixed DMA channels are temporarily workaround before we have a centralized DMA controller API to help alloc the channel dynamically
// Remove them when GDMA driver API is ready
#define SOC_GDMA_M2M_DMA_CHANNEL  (0)
#define SOC_GDMA_SPI2_DMA_CHANNEL (1)
#define SOC_GDMA_SPI3_DMA_CHANNEL (2)
#define SOC_GDMA_SHA_DMA_CHANNEL  (3)
