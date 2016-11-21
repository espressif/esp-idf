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

#ifndef __ESP_TASK_WDT_H
#define __ESP_TASK_WDT_H

#ifdef __cplusplus
extern "C" {
#endif


/** \defgroup Watchdog_APIs Watchdog APIs
  * @brief Watchdog APIs
  */

/** @addtogroup Watchdog_APIs
  * @{
  */

/*
This routine enables a more general-purpose task watchdog: tasks can individually
feed the watchdog and the watchdog will bark if one or more tasks haven't fed the
watchdog within the specified time. Optionally, the idle tasks can also configured
to feed the watchdog in a similar fashion, to detect CPU starvation.

This uses the TIMERG0 WDT.
*/


/**
  * @brief  Initialize the task watchdog. This is called in the init code, if the
  *         task watchdog is enabled in menuconfig.
  *
  */
void esp_task_wdt_init();

/**
  * @brief  Feed the watchdog. After the first feeding session, the watchdog will expect the calling
  *         task to keep feeding the watchdog until task_wdt_delete() is called.
  *
  */

void esp_task_wdt_feed();


/**
  * @brief  Delete the watchdog for the current task.
  *
  */
void esp_task_wdt_delete();

/**
  * @}
  */


#ifdef __cplusplus
}
#endif



#endif