/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_INT_WDT_H
#define __ESP_INT_WDT_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Watchdog_APIs
  * @{
  */

/*
This routine enables a watchdog to catch instances of processes disabling
interrupts for too long, or code within interrupt handlers taking too long.
It does this by setting up a watchdog which gets fed from the FreeRTOS
task switch interrupt. When this watchdog times out, initially it will call
a high-level interrupt routine that will panic FreeRTOS in order to allow
for forensic examination of the state of the both CPUs. When this interrupt
handler is not called and the watchdog times out a second time, it will
reset the SoC.

This uses the TIMERG1 WDT.
*/


/**
  * @brief  Initialize the non-CPU-specific parts of interrupt watchdog.
  *         This is called in the init code if the interrupt watchdog
  *         is enabled in menuconfig.
  *
  */
void esp_int_wdt_init(void);

/**
  * @brief  Enable the interrupt watchdog on the current CPU. This is called
  *         in the init code by both CPUs if the interrupt watchdog is enabled
  *         in menuconfig.
  *
  */
void esp_int_wdt_cpu_init(void);



/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif
