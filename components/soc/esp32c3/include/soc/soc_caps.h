// The long term plan is to have a single soc_caps.h for each peripheral.
// During the refactoring and multichip support development process, we
// seperate these information into periph_caps.h for each peripheral and
// include them here.

#pragma once

#define SOC_CPU_CORES_NUM           1
#define SOC_GDMA_SUPPORTED          1
#define SOC_TWAI_SUPPORTED          1
#define SOC_BT_SUPPORTED            1
#define SOC_DIG_SIGN_SUPPORTED      1
#define SOC_HMAC_SUPPORTED          1
#define SOC_ASYNC_MEMCPY_SUPPORTED  1

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
#include "cpu_caps.h"
#include "gpio_caps.h"
#include "ledc_caps.h"
#include "spi_caps.h"
#include "uart_caps.h"
#include "rtc_caps.h"

/*-------------------------- COMMON CAPS ---------------------------------------*/
#define SOC_SUPPORTS_SECURE_DL_MODE     1
#define SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS 3

/*-------------------------- TOUCH SENSOR CAPS -------------------------------*/
#define SOC_TOUCH_SENSOR_NUM            (0)    /*! No touch sensors on ESP32-C3 */

/*-------------------------- TWAI CAPS ---------------------------------------*/
#define SOC_TWAI_BRP_MIN                2
#define SOC_TWAI_BRP_MAX                16384
#define SOC_TWAI_SUPPORTS_RX_STATUS     1

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
#define SOC_SHA_GDMA             (1)

/* Supported HW algorithms */
#define SOC_SHA_SUPPORT_SHA1            (1)
#define SOC_SHA_SUPPORT_SHA224          (1)
#define SOC_SHA_SUPPORT_SHA256          (1)

/*--------------------------- TIMER GROUP CAPS ---------------------------------------*/
#define SOC_TIMER_GROUPS                  (2)
#define SOC_TIMER_GROUP_TIMERS_PER_GROUP  (1)
#define SOC_TIMER_GROUP_COUNTER_BIT_WIDTH (54)
#define SOC_TIMER_GROUP_SUPPORT_XTAL      (1)
#define SOC_TIMER_GROUP_TOTAL_TIMERS (SOC_TIMER_GROUPS * SOC_TIMER_GROUP_TIMERS_PER_GROUP)

/*--------------------------- RMT CAPS ---------------------------------------*/
#define SOC_RMT_GROUPS                  (1)  /*!< One RMT group */
#define SOC_RMT_TX_CANDIDATES_PER_GROUP (2)  /*!< Number of channels that capable of Transmit */
#define SOC_RMT_RX_CANDIDATES_PER_GROUP (2)  /*!< Number of channels that capable of Receive */
#define SOC_RMT_CHANNELS_PER_GROUP      (4)  /*!< Total 4 channels */
#define SOC_RMT_MEM_WORDS_PER_CHANNEL   (48) /*!< Each channel owns 48 words memory (1 word = 4 Bytes) */
#define SOC_RMT_SUPPORT_RX_PINGPONG     (1)  /*!< Support Ping-Pong mode on RX path */
#define SOC_RMT_SUPPORT_RX_DEMODULATION (1)  /*!< Support signal demodulation on RX path (i.e. remove carrier) */
#define SOC_RMT_SUPPORT_TX_LOOP_COUNT   (1)  /*!< Support transmit specified number of cycles in loop mode */
#define SOC_RMT_SUPPORT_TX_SYNCHRO      (1)  /*!< Support coordinate a group of TX channels to start simultaneously */
#define SOC_RMT_SUPPORT_XTAL            (1)  /*!< Support set XTAL clock as the RMT clock source */

/*--------------------------- RSA CAPS ---------------------------------------*/
#define SOC_RSA_MAX_BIT_LEN    (3072)



/*-------------------------- AES CAPS -----------------------------------------*/
#define SOC_AES_SUPPORT_DMA     (1)

/* Has a centralized DMA, which is shared with all peripherals */
#define SOC_AES_GDMA            (1)

#define SOC_AES_SUPPORT_AES_128 (1)
#define SOC_AES_SUPPORT_AES_256 (1)

/*-------------------------- ADC CAPS -------------------------------*/
#define SOC_ADC_PERIPH_NUM                      (2)
#define SOC_ADC_PATT_LEN_MAX                    (16)
#define SOC_ADC_CHANNEL_NUM(PERIPH_NUM)         ((PERIPH_NUM==0)? 5 : 1)
#define SOC_ADC_MAX_CHANNEL_NUM                 (5)
#define SOC_ADC_MAX_BITWIDTH                    (12)
#define SOC_ADC_DIGI_FILTER_NUM                 (2)
#define SOC_ADC_DIGI_MONITOR_NUM                (2)
#define SOC_ADC_HW_CALIBRATION_V1               (1) /*!< support HW offset calibration */
#define SOC_ADC_SUPPORT_DMA_MODE(PERIPH_NUM)    1
//F_sample = F_digi_con / 2 / interval. F_digi_con = 5M for now. 30 <= interva <= 4095
#define SOC_ADC_SAMPLE_FREQ_THRES_HIGH          83333
#define SOC_ADC_SAMPLE_FREQ_THRES_LOW           611

/*-------------------------- APB BACKUP DMA CAPS -------------------------------*/
#define SOC_APB_BACKUP_DMA              (1)

/*-------------------------- WI-FI HARDWARE TSF CAPS -------------------------------*/
#define SOC_WIFI_HW_TSF                 (1)

/*-------------------------- COEXISTENCE HARDWARE PTI CAPS -------------------------------*/
#define SOC_COEX_HW_PTI                 (1)

/*--------------- PHY REGISTER AND MEMORY SIZE CAPS --------------------------*/
#define SOC_PHY_DIG_REGS_MEM_SIZE       (21*4)
#define SOC_MAC_BB_PD_MEM_SIZE          (192*4)

/*--------------- WIFI LIGHT SLEEP CLOCK WIDTH CAPS --------------------------*/
#define SOC_WIFI_LIGHT_SLEEP_CLK_WIDTH  (12)

/*-------------------------- SPI MEM CAPS ---------------------------------------*/
#define SOC_SPI_MEM_SUPPORT_AUTO_WAIT_IDLE                (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND                  (1)
#define SOC_SPI_MEM_SUPPORT_AUTO_RESUME                   (1)
#define SOC_SPI_MEM_SUPPORT_IDLE_INTR                     (1)
#define SOC_SPI_MEM_SUPPORT_SW_SUSPEND                    (1)
#define SOC_SPI_MEM_SUPPORT_CHECK_SUS                     (1)
/*-------------------------- Power Management CAPS ----------------------------*/
#define SOC_PM_SUPPORT_WIFI_WAKEUP      (1)

#define SOC_PM_SUPPORT_BT_WAKEUP        (1)

#define SOC_PM_SUPPORT_CPU_PD           (1)

#define SOC_PM_SUPPORT_WIFI_PD          (1)

#define SOC_PM_SUPPORT_BT_PD            (1)
