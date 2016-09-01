#ifndef __ESP_TASK_H
#define __ESP_TASK_H

/* Notes:
 * 1. Put all task priority and stack size definition in this file
 * 2. If the task priority is less than 10, use ESP_TASK_PRIO_MIN + X style,
 *    otherwise use ESP_TASK_PRIO_MAX + X style
 * 3. If this is a daemon task, the macro prifix is ESP_TASKD_, otherwise
 *    it's ESP_TASK_
 * 4. If the configMAX_PRIORITIES is modified, please make all prority are 
 *    greater than 0
 * 5. Make sure esp_task.h is consistent between wifi lib and idf
 */

#define ESP_TASK_PRIO_MAX (configMAX_PRIORITIES)
#define ESP_TASK_PRIO_MIN (0)

/* Wifi library task */
#define ESP_TASKD_WATCHDOG_PRIO       (ESP_TASK_PRIO_MAX - 1)
#define ESP_TASKD_WATCHDOG_STACK      2048
#define ESP_TASK_WPA2_PRIO            (ESP_TASK_PRIO_MAX - 1)
#define ESP_TASK_WPA2_STACK           2048
#define ESP_TASKD_WIFI_PRIO           (ESP_TASK_PRIO_MAX - 2)
#define ESP_TASKD_WIFI_STACK          8196
#define ESP_TASKD_WIFI_TIMER_PRIO     (ESP_TASK_PRIO_MAX - 3)
#define ESP_TASKD_WIFI_TIMER_STACK    2048
#define ESP_TASK_WPS_PRIO             (ESP_TASK_PRIO_MIN + 2)
#define ESP_TASK_WPS_STACK            2048

/* idf task */
#define ESP_TASKD_EVENT_PRIO          (ESP_TASK_PRIO_MAX - 5)
#define ESP_TASKD_EVENT_STACK         2048
#define ESP_TASK_WIFI_STARTUP_PRIO    (ESP_TASK_PRIO_MAX - 7)
#define ESP_TASK_WIFI_STARTUP_STACK   4096
#define ESP_TASKD_LWIP_PRIO           (ESP_TASK_PRIO_MAX - 7)
#define ESP_TASKD_LWIP_STACK          2048
#define ESP_TASKD_IDLE_PRIO           0
#define ESP_TASKD_IDLE_STACK          configMINIMAL_STACK_SIZE
#define ESP_TASKD_TIMER_PRIO          configTIMER_TASK_PRIORITY
#define ESP_TASKD_TIMER_STACK         configTIMER_TASK_STACK_DEPTH
#define ESP_TASK_NVS_PRIO             "TODO"
#define ESP_TASK_NVS_STACK            "TODO"

/* SSC tasks */
#define ESP_TASKD_SSC_PRIO            (ESP_TASK_PRIO_MAX - 5)
#define ESP_TASKD_SSC_STACK           2048
#define ESP_TASKD_SSC_APP_PRIO        (ESP_TASK_PRIO_MAX - 5)
#define ESP_TASKD_SSC_APP_STACK       8196
#define ESP_TASK_SSC_SOCKET_PRIO      (ESP_TASK_PRIO_MAX - 6)
#define ESP_TASK_SSC_SOCKET_STACK     2048
#define ESP_TASK_SSC_SSL_PRIO         (ESP_TASK_PRIO_MAX - 6)
#define ESP_TASK_SSC_SSL_STACK        2048

/* smart config */
#define ESP_TASK_KISS_PRIO           "TODO"  
#define ESP_TASK_KISS_STACK          "TODO"
#define ESP_TASK_TOUCH_PRIO          "TODO"  
#define ESP_TASK_TOUCH_STACK         "TODO"

#endif
