// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
for forensic examination of the state of the CPU. When this interrupt 
handler is not called and the watchdog times out a second time, it will 
reset the SoC.

This uses the TIMERG1 WDT.
*/


/**
  * @brief  Initialize the interrupt watchdog. This is called in the init code if
  *         the interrupt watchdog is enabled in menuconfig.
  *
  */
void esp_int_wdt_init();


/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif