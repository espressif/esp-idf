// The long term plan is to have a single soc_caps.h for each peripheral.
// During the refactoring and multichip support development process, we
// seperate these information into periph_caps.h for each peripheral and
// include them here.

#pragma once

#define SOC_CPU_CORES_NUM 1
#define SOC_GDMA_SUPPORTED 1

// There are 3 DMA channels on ESP32-C3
// Attention: These fixed DMA channels are temporarily workaround before we have a centralized DMA controller API to help alloc the channel dynamically
// Remove them when GDMA driver API is ready
#define SOC_GDMA_AES_DMA_CHANNEL    (0)
#define SOC_GDMA_SHA_DMA_CHANNEL    (1)
#define SOC_GDMA_SPI2_DMA_CHANNEL   (2)
#define SOC_GDMA_ADC_DMA_CHANNEL    (0)

//NOTE: The CHx number should be consistent with the selected DMA channel above
#define SOC_GDMA_SPI2_INTR_SOURCE   ETS_DMA_CH2_INTR_SOURCE
//On C3, there is only 1 GPSPI controller (GPSPI2)
#define SOC_GDMA_SPI3_DMA_CHANNEL   SOC_GDMA_SPI2_DMA_CHANNEL

#define SOC_GDMA_ADC_INTR_SOURCE    ETS_DMA_CH0_INTR_SOURCE

#include "rmt_caps.h"
#include "dac_caps.h"
#include "i2c_caps.h"
#include "mpu_caps.h"
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
#include "spi_caps.h"
#include "uart_caps.h"

/*-------------------------- TOUCH SENSOR CAPS -------------------------------*/
#define SOC_TOUCH_SENSOR_NUM            (0)    /*! No touch sensors on ESP32-C3 */


#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM) ((PERIPH_NUM==0)? 5 : 1)
#define SOC_ADC_MAX_CHANNEL_NUM         (10)

/**
 * Check if adc support digital controller (DMA) mode.
 * @value
 *      - 1 : support;
 *      - 0 : not support;
 */
#define SOC_ADC_SUPPORT_DMA_MODE(PERIPH_NUM) 1

/*--------------------------- SHA CAPS ---------------------------------------*/

/* Max amount of bytes in a single DMA operation is 4095,
   for SHA this means that the biggest safe amount of bytes is
   31 blocks of 128 bytes = 3968
*/
#define SOC_SHA_DMA_MAX_BUFFER_SIZE     (3968)
#define SOC_SHA_SUPPORT_DMA             (1)

/* The SHA engine is able to resume hashing from a user */
#define SOC_SHA_SUPPORT_RESUME          (1)

/* Has a centralized DMA, which is shared with all peripherals */
#define SOC_SHA_GENERAL_DMA             (1)

/* Supported HW algorithms */
#define SOC_SHA_SUPPORT_SHA1            (1)
#define SOC_SHA_SUPPORT_SHA224          (1)
#define SOC_SHA_SUPPORT_SHA256          (1)


/*--------------------------- RSA CAPS ---------------------------------------*/
#define SOC_RSA_MAX_BIT_LEN    (3072)



/*-------------------------- AES CAPS -----------------------------------------*/
#define SOC_AES_SUPPORT_DMA     (1)

/* Has a centralized DMA, which is shared with all peripherals */
#define SOC_AES_GENERAL_DMA     (1)

#define SOC_AES_SUPPORT_AES_128 (1)
#define SOC_AES_SUPPORT_AES_256 (1)

/*-------------------------- ADC CAPS -------------------------------*/
#define SOC_ADC_PERIPH_NUM              (2)
#define SOC_ADC_PATT_LEN_MAX            (16)

#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM) ((PERIPH_NUM==0)? 5 : 1)
#define SOC_ADC_MAX_CHANNEL_NUM         (10)

/**
 * Check if adc support digital controller (DMA) mode.
 * @value
 *      - 1 : support;
 *      - 0 : not support;
 */
#define SOC_ADC_SUPPORT_DMA_MODE(PERIPH_NUM) 1

/*-------------------------- APB BACKUP DMA CAPS -------------------------------*/
#define SOC_APB_BACKUP_DMA              (1)

/*-------------------------- WI-FI HARDWARE TSF CAPS -------------------------------*/
#define SOC_WIFI_HW_TSF                 (1)

/*-------------------------- COEXISTENCE HARDWARE PTI CAPS -------------------------------*/
#define SOC_COEX_HW_PTI                 (1)
