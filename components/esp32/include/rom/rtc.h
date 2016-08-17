// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ROM_RTC_H_
#define _ROM_RTC_H_

#include "ets_sys.h"

#include <stdbool.h>
#include <stdint.h>

#include "soc/soc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AWAKE = 0,              //CPU ON
    LIGHT_SLEEP = BIT0,    //CPU waiti, PLL ON.  We don't need explicitly set this mode.
    DEEP_SLEEP  = BIT1     //CPU OFF, PLL OFF, only specific timer could wake up
} SLEEP_MODE;

typedef enum {
    NO_MEAN                =  0,
    POWERON_RESET          =  1,    //1  Vbat power on reset, RTC reset
//    EXT_SYS_RESET        =  2,    //4  External System reset, RTC reset
    SW_RESET               =  3,    //6  Software warm reset
    OWDT_RESET             =  4,    //5  Watch dog reset
    DEEPSLEEP_RESET        =  5,    //2  Deep sleep timer reach reset.
    SDIO_RESET             =  6,    //3  Deep sleep Pbint power on reset [boot]
    TG0WDT_SYS_RESET       =  7,
    TG1WDT_SYS_RESET       =  8,
    RTCWDT_SYS_RESET       =  9,
    INTRUSION_RESET        = 10,
    TGWDT_CPU_RESET        = 11,
    SW_CPU_RESET           = 12,
    RTCWDT_CPU_RESET       = 13,
    EXT_CPU_RESET          = 14,
    RTCWDT_BROWN_OUT_RESET = 15,
    RTCWDT_RTC_RESET = 16
} RESET_REASON;

typedef enum {
    NO_SLEEP        = 0,
    EXT_EVENT0_TRIG = BIT0,
    EXT_EVENT1_TRIG = BIT1,
    GPIO_TRIG       = BIT2,
    TIMER_EXPIRE    = BIT3,
    SDIO_TRIG       = BIT4,
    MAC_TRIG        = BIT5,
    UART0_TRIG      = BIT6,
    UART1_TRIG      = BIT7,
    TOUCH_TRIG      = BIT8,
    SAR_TRIG        = BIT9,
    BT_TRIG         = BIT10
} WAKEUP_REASON;

typedef enum {
    DISEN_WAKEUP       = NO_SLEEP,
    EXT_EVENT0_TRIG_EN = EXT_EVENT0_TRIG,
    EXT_EVENT1_TRIG_EN = EXT_EVENT1_TRIG,
    GPIO_TRIG_EN       = GPIO_TRIG,
    TIMER_EXPIRE_EN    = TIMER_EXPIRE,
    SDIO_TRIG_EN       = SDIO_TRIG,
    MAC_TRIG_EN        = MAC_TRIG,
    UART0_TRIG_EN      = UART0_TRIG,
    UART1_TRIG_EN      = UART1_TRIG,
    TOUCH_TRIG_EN      = TOUCH_TRIG,
    SAR_TRIG_EN        = SAR_TRIG,
    BT_TRIG_EN         = BT_TRIG
} WAKEUP_ENABLE;

typedef enum {
    NO_INT             = 0,
    WAKEUP_INT         = BIT0,
    REJECT_INT         = BIT1,
    SDIO_IDLE_INT      = BIT2,
    RTC_WDT_INT        = BIT3,
    RTC_TIME_VALID_INT = BIT4
} RTC_INT_REASON;

typedef enum {
    DISEN_INT             = 0,
    WAKEUP_INT_EN         = WAKEUP_INT,
    REJECT_INT_EN         = REJECT_INT,
    SDIO_IDLE_INT_EN      = SDIO_IDLE_INT,
    RTC_WDT_INT_EN        = RTC_WDT_INT,
    RTC_TIME_VALID_INT_EN = RTC_TIME_VALID_INT
}RTC_INT_EN;



// Alive memory is a special memory block which could restore data during system
// deep sleep.  power management and wlan profile data may need put into this
// memory area.
// Should create a dram segment in link script.
#define ALIVE_MEMORY_ADDR
#define ALIVE_MEMORY_SIZE       (1024 * 2)

void rtc_hw_init(void);

RESET_REASON rtc_get_reset_reason(int cpu_no);
void software_reset(void);
void software_reset_cpu(int cpu_no);
void rtc_select_apb_bridge(bool sel);
void rtc_set_sleep_mode(SLEEP_MODE mode, uint32_t sleep_sec, uint32_t wakeup_mode);

uint8_t ets_rtc_recovery(void);

#define MAX_DEEPSLEEP_DURATION      (0xffffffff / RTC_CLK_FREQ)  
#define SECOND_TO_RTC_TICK(second) ((second)*RTC_CLK_FREQ)  //32KHz
#define CALIB_VALUE_TO_RTC_TICK(microsecond, clk_mkz, n_rtc,nclk) ((microsecond)*(clk_mkz)*(n_rtc)/(nclk))  //32KHz
#define RTC_TICK_TO_SECOND(tick) ((tick)/RTC_CLK_FREQ )
#define GET_CURRENT_TICK() (READ_PERI_REG(RTC_TIME))
#define SET_WAKEUP_TICK(tick) (WRITE_PERI_REG(RTC_TIMER0, tick))

//#define GET_WAKEUP_CAUSE()              GET_PERI_REG_BITS2(RTC_STATE1, RTC_CNTL_WAKEUP_CAUSE, RTC_CNTL_WAKEUP_CAUSE_S)
#define DISABLE_RTC_INT(int_type)       CLEAR_PERI_REG_MASK(RTC_INT_ENA, int_type)
#define ENABLE_RTC_INT(int_type)        SET_PERI_REG_MASK(RTC_INT_ENA, int_type)
#define CLR_RTC_INT(int_type)           SET_PERI_REG_MASK(RTC_INT_CLR, int_type)
#define GET_RTC_INT_CAUSE()             GET_PERI_REG_BITS(RTC_INT_RAW, RTC_INT_RAW_MSB,RTC_INT_RAW_LSB)

void rtc_register_deepsleep_timer(ETSTimer *timer, uint32_t tmout);
void rtc_disable_deepsleep_timer(void);

void rtc_enter_sleep(void);
void ets_rtc_int_register(void);
void dtm_set_intr_mask(uint32_t mask);
uint32_t dtm_get_intr_mask(void);
void dtm_set_params(uint32_t sleep_mode, uint32_t sleep_tm_ms, uint32_t wakeup_tm_ms, uint32_t sleep_times, uint32_t rxbcn_len);
void save_rxbcn_mactime(uint32_t rxbcn_mactime);
void save_tsf_us(uint32_t tsf_us);

typedef void (* ets_idle_cb_t)(void *arg);

typedef uint32_t (* ETS_GET_MACTIME)(void);
typedef void   (* ETS_WAKEUP_INIT)(void);

void dtm_params_init(ETS_GET_MACTIME get_mactime, ETS_WAKEUP_INIT wakeup_init);

void ets_set_idle_cb(ets_idle_cb_t func, void *arg);

void ets_enter_sleep(void);

void rtc_intr_handler(void *);
 
#define ETS_SLEEP_START(pfunc, parg) ets_set_idle_cb((pfunc), (parg));

#define ETS_SLEEP_END() ets_set_idle_cb(NULL, NULL);

#ifdef __cplusplus
}
#endif

#endif /* _ROM_RTC_H_ */

