/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ROM_ETS_SYS_H_
#define _ROM_ETS_SYS_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup ets_sys_apis, ets system related apis
  * @brief ets system apis
  */

/** @addtogroup ets_sys_apis
  * @{
  */

/************************************************************************
  *                                NOTE
  *   Many functions in this header files can't be run in FreeRTOS.
  *   Please see the comment of the Functions.
  *   There are also some functions that doesn't work on FreeRTOS
  *   without listed in the header, such as:
  *   xtos functions start with "_xtos_" in ld file.
  *
  ***********************************************************************
  */

/** \defgroup ets_apis, Espressif Task Scheduler related apis
  * @brief ets apis
  */

/** @addtogroup ets_apis
  * @{
  */

typedef enum {
    ETS_OK     = 0, /**< return successful in ets*/
    ETS_FAILED = 1  /**< return failed in ets*/
} ETS_STATUS;

typedef ETS_STATUS ets_status_t;

typedef uint32_t ETSSignal;
typedef uint32_t ETSParam;

typedef struct ETSEventTag ETSEvent;    /**< Event transmit/receive in ets*/

struct ETSEventTag {
    ETSSignal sig;  /**< Event signal, in same task, different Event with different signal*/
    ETSParam  par;  /**< Event parameter, sometimes without usage, then will be set as 0*/
};

typedef void (*ETSTask)(ETSEvent *e);       /**< Type of the Task processor*/
typedef void (* ets_idle_cb_t)(void *arg);  /**< Type of the system idle callback*/

/**
  * @brief  Start the Espressif Task Scheduler, which is an infinite loop. Please do not add code after it.
  *
  * @param  none
  *
  * @return none
  */
void ets_run(void);

/**
  * @brief  Set the Idle callback, when Tasks are processed, will call the callback before CPU goto sleep.
  *
  * @param  ets_idle_cb_t func : The callback function.
  *
  * @param  void *arg : Argument of the callback.
  *
  * @return None
  */
void ets_set_idle_cb(ets_idle_cb_t func, void *arg);

/**
  * @brief  Init a task with processor, priority, queue to receive Event, queue length.
  *
  * @param  ETSTask task : The task processor.
  *
  * @param  uint8_t prio : Task priority, 0-31, bigger num with high priority, one priority with one task.
  *
  * @param  ETSEvent *queue : Queue belongs to the task, task always receives Events, Queue is circular used.
  *
  * @param  uint8_t qlen : Queue length.
  *
  * @return None
  */
void ets_task(ETSTask task, uint8_t prio, ETSEvent *queue, uint8_t qlen);

/**
  * @brief  Post an event to an Task.
  *
  * @param  uint8_t prio : Priority of the Task.
  *
  * @param  ETSSignal sig : Event signal.
  *
  * @param  ETSParam  par : Event parameter
  *
  * @return ETS_OK     : post successful
  * @return ETS_FAILED : post failed
  */
ETS_STATUS ets_post(uint8_t prio, ETSSignal sig, ETSParam par);

/**
  * @}
  */

/** \defgroup ets_boot_apis, Boot routing related apis
  * @brief ets boot apis
  */

/** @addtogroup ets_apis
  * @{
  */

extern const char *const exc_cause_table[40];   ///**< exception cause that defined by the core.*/

/**
  * @brief  Set Pro cpu Entry code, code can be called in PRO CPU when booting is not completed.
  *         When Pro CPU booting is completed, Pro CPU will call the Entry code if not NULL.
  *
  * @param  uint32_t start : the PRO Entry code address value in uint32_t
  *
  * @return None
  */
void ets_set_user_start(uint32_t start);

void ets_set_appcpu_boot_addr(uint32_t start);

/**
  * @}
  */

/** \defgroup ets_printf_apis, ets_printf related apis used in ets
  * @brief ets printf apis
  */

/** @addtogroup ets_printf_apis
  * @{
  */

/**
  * @brief  Printf the strings to uart or other devices, similar with printf, simple than printf.
  *         Can not print float point data format, or longlong data format.
  *         So we maybe only use this in ROM.
  *
  * @param  const char *fmt : See printf.
  *
  * @param  ... : See printf.
  *
  * @return int : the length printed to the output device.
  */
int ets_printf(const char *fmt, ...);

/**
  * @brief Get the uart channel of ets_printf(uart_tx_one_char).
  *
  * @return uint8_t uart channel used by ets_printf(uart_tx_one_char).
  */
uint8_t ets_get_printf_channel(void);

/**
  * @brief  Output a char to uart, which uart to output(which is in uart module in ROM) is not in scope of the function.
  *         Can not print float point data format, or longlong data format
  *
  * @param  char c : char to output.
  *
  * @return None
  */
void ets_write_char_uart(char c);

/**
  * @brief  Ets_printf have two output functions： putc1 and putc2, both of which will be called if need output.
  *         To install putc1, which is defaulted installed as ets_write_char_uart in none silent boot mode, as NULL in silent mode.
  *
  * @param  void (*)(char) p: Output function to install.
  *
  * @return None
  */
void ets_install_putc1(void (*p)(char c));

/**
  * @brief  Ets_printf have two output functions： putc1 and putc2, both of which will be called if need output.
  *         To install putc2, which is defaulted installed as NULL.
  *
  * @param  void (*)(char) p: Output function to install.
  *
  * @return None
  */
void ets_install_putc2(void (*p)(char c));

/**
  * @brief  Install putc1 as ets_write_char_uart.
  *         In silent boot mode(to void interfere the UART attached MCU), we can call this function, after booting ok.
  *
  * @param  None
  *
  * @return None
  */
void ets_install_uart_printf(void);

#define ETS_PRINTF(...) ets_printf(...)

#define ETS_ASSERT(v) do { \
    if (!(v)) {             \
        ets_printf("%s %u \n", __FILE__, __LINE__); \
        while (1) {};   \
    }                   \
} while (0);

/**
  * @}
  */

/** \defgroup ets_timer_apis, ets_timer related apis used in ets
  * @brief ets timer apis
  */

/** @addtogroup ets_timer_apis
  * @{
  */
typedef void ETSTimerFunc(void *timer_arg);/**< timer handler*/

typedef struct _ETSTIMER_ {
    struct _ETSTIMER_    *timer_next;   /**< timer linker*/
    uint32_t              timer_expire; /**< abstract time when timer expire*/
    uint32_t              timer_period; /**< timer period, 0 means timer is not periodic repeated*/
    ETSTimerFunc         *timer_func;   /**< timer handler*/
    void                 *timer_arg;    /**< timer handler argument*/
} ETSTimer;

/**
  * @brief  Init ets timer, this timer range is 640 us to 429496 ms
  *         In FreeRTOS, please call FreeRTOS apis, never call this api.
  *
  * @param  None
  *
  * @return None
  */
void ets_timer_init(void);

/**
  * @brief  In FreeRTOS, please call FreeRTOS apis, never call this api.
  *
  * @param  None
  *
  * @return None
  */
void ets_timer_deinit(void);

/**
  * @brief  Arm an ets timer, this timer range is 640 us to 429496 ms.
  *         In FreeRTOS, please call FreeRTOS apis, never call this api.
  *
  * @param  ETSTimer *timer : Timer struct pointer.
  *
  * @param  uint32_t tmout : Timer value in ms, range is 1 to 429496.
  *
  * @param  bool repeat : Timer is periodic repeated.
  *
  * @return None
  */
void ets_timer_arm(ETSTimer *timer, uint32_t tmout, bool repeat);

/**
  * @brief  Arm an ets timer, this timer range is 640 us to 429496 ms.
  *         In FreeRTOS, please call FreeRTOS apis, never call this api.
  *
  * @param  ETSTimer *timer : Timer struct pointer.
  *
  * @param  uint32_t tmout : Timer value in us, range is 1 to 429496729.
  *
  * @param  bool repeat : Timer is periodic repeated.
  *
  * @return None
  */
void ets_timer_arm_us(ETSTimer *ptimer, uint32_t us, bool repeat);

/**
  * @brief  Disarm an ets timer.
  *         In FreeRTOS, please call FreeRTOS apis, never call this api.
  *
  * @param  ETSTimer *timer : Timer struct pointer.
  *
  * @return None
  */
void ets_timer_disarm(ETSTimer *timer);

/**
  * @brief  Set timer callback and argument.
  *         In FreeRTOS, please call FreeRTOS apis, never call this api.
  *
  * @param  ETSTimer *timer : Timer struct pointer.
  *
  * @param  ETSTimerFunc *pfunction : Timer callback.
  *
  * @param  void *parg : Timer callback argument.
  *
  * @return None
  */
void ets_timer_setfn(ETSTimer *ptimer, ETSTimerFunc *pfunction, void *parg);

/**
  * @brief  Unset timer callback and argument to NULL.
  *         In FreeRTOS, please call FreeRTOS apis, never call this api.
  *
  * @param  ETSTimer *timer : Timer struct pointer.
  *
  * @return None
  */
void ets_timer_done(ETSTimer *ptimer);

/**
  * @brief  CPU do while loop for some time.
  *         In FreeRTOS task, please call FreeRTOS apis.
  *
  * @param  uint32_t us : Delay time in us.
  *
  * @return None
  */
void ets_delay_us(uint32_t us);

/**
  * @brief  Set the real CPU ticks per us to the ets, so that ets_delay_us will be accurate.
  *         Call this function when CPU frequency is changed.
  *
  * @param  uint32_t ticks_per_us : CPU ticks per us.
  *
  * @return None
  */
void ets_update_cpu_frequency(uint32_t ticks_per_us);

/**
  * @brief  Set the real CPU ticks per us to the ets, so that ets_delay_us will be accurate.
  *
  * @note This function only sets the tick rate for the current CPU. It is located in ROM,
  *       so the deep sleep stub can use it even if IRAM is not initialized yet.
  *
  * @param  uint32_t ticks_per_us : CPU ticks per us.
  *
  * @return None
  */
void ets_update_cpu_frequency_rom(uint32_t ticks_per_us);

/**
  * @brief  Get the real CPU ticks per us to the ets.
  *         This function do not return real CPU ticks per us, just the record in ets. It can be used to check with the real CPU frequency.
  *
  * @param  None
  *
  * @return uint32_t : CPU ticks per us record in ets.
  */
uint32_t ets_get_cpu_frequency(void);

/**
  * @brief  Get xtal_freq value, If value not stored in RTC_STORE5, than store.
  *
  * @param  None
  *
  * @return uint32_t : if stored in efuse(not 0)
  *                         clock = ets_efuse_get_xtal_freq() * 1000000;
  *                    else if analog_8M in efuse
  *                         clock = ets_get_xtal_scale() * 625 / 16 * ets_efuse_get_8M_clock();
  *                    else clock = 40M.
  */
uint32_t ets_get_xtal_freq(void);

/**
  * @brief  Get the apb divior by xtal frequency.
  *         When any types of reset happen, the default value is 2.
  *
  * @param  None
  *
  * @return uint32_t : 1 or 2.
  */
uint32_t ets_get_xtal_div(void);

/**
  * @brief  Get apb_freq value, If value not stored in RTC_STORE5, than store.
  *
  * @param  None
  *
  * @return uint32_t : if rtc store the value (RTC_STORE5 high 16 bits and low 16 bits with same value), read from rtc register.
  *                         clock = (REG_READ(RTC_STORE5) & 0xffff) << 12;
  *                    else store ets_get_detected_xtal_freq() in.
  */
uint32_t ets_get_apb_freq(void);

/**
  * @}
  */

/** \defgroup ets_intr_apis, ets interrupt configure related apis
  * @brief ets intr apis
  */

/** @addtogroup ets_intr_apis
  * @{
  */

typedef void (* ets_isr_t)(void *);/**< interrupt handler type*/

/**
  * @brief  Attach a interrupt handler to a CPU interrupt number.
  *         This function equals to _xtos_set_interrupt_handler_arg(i, func, arg).
  *         In FreeRTOS, please call FreeRTOS apis, never call this api.
  *
  * @param  int i : CPU interrupt number.
  *
  * @param  ets_isr_t func : Interrupt handler.
  *
  * @param  void *arg : argument of the handler.
  *
  * @return None
  */
void ets_isr_attach(int i, ets_isr_t func, void *arg);

/**
  * @brief  Mask the interrupts which show in mask bits.
  *         This function equals to _xtos_ints_off(mask).
  *         In FreeRTOS, please call FreeRTOS apis, never call this api.
  *
  * @param  uint32_t mask : BIT(i) means mask CPU interrupt number i.
  *
  * @return None
  */
void ets_isr_mask(uint32_t mask);

/**
  * @brief  Unmask the interrupts which show in mask bits.
  *         This function equals to _xtos_ints_on(mask).
  *         In FreeRTOS, please call FreeRTOS apis, never call this api.
  *
  * @param  uint32_t mask : BIT(i) means mask CPU interrupt number i.
  *
  * @return None
  */
void ets_isr_unmask(uint32_t unmask);

/**
  * @brief  Lock the interrupt to level 2.
  *         This function direct set the CPU registers.
  *         In FreeRTOS, please call FreeRTOS apis, never call this api.
  *
  * @param  None
  *
  * @return None
  */
void ets_intr_lock(void);

/**
  * @brief  Unlock the interrupt to level 0.
  *         This function direct set the CPU registers.
  *         In FreeRTOS, please call FreeRTOS apis, never call this api.
  *
  * @param  None
  *
  * @return None
  */
void ets_intr_unlock(void);

/**
  * @brief  Unlock the interrupt to level 0, and CPU will go into power save mode(wait interrupt).
  *         This function direct set the CPU registers.
  *         In FreeRTOS, please call FreeRTOS apis, never call this api.
  *
  * @param  None
  *
  * @return None
  */
void ets_waiti0(void);

/**
  * @brief  Attach an CPU interrupt to a hardware source.
  *         We have 4 steps to use an interrupt:
  *         1.Attach hardware interrupt source to CPU.  intr_matrix_set(0, ETS_WIFI_MAC_INTR_SOURCE, ETS_WMAC_INUM);
  *         2.Set interrupt handler.                    xt_set_interrupt_handler(ETS_WMAC_INUM, func, NULL);
  *         3.Enable interrupt for CPU.                 xt_ints_on(1 << ETS_WMAC_INUM);
  *         4.Enable interrupt in the module.
  *
  * @param  int cpu_no : The CPU which the interrupt number belongs.
  *
  * @param  uint32_t model_num : The interrupt hardware source number, please see the interrupt hardware source table.
  *
  * @param  uint32_t intr_num : The interrupt number CPU, please see the interrupt cpu using table.
  *
  * @return None
  */
void intr_matrix_set(int cpu_no, uint32_t model_num, uint32_t intr_num);

/**
  * @}
  */

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#define ETS_MEM_BAR() asm volatile ( "" : : : "memory" )

typedef enum {
    OK = 0,
    FAIL,
    PENDING,
    BUSY,
    CANCEL,
} STATUS;

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _ROM_ETS_SYS_H_ */
