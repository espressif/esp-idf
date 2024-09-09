/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <dlfcn.h>
#include <assert.h>
#include <sys/select.h>
#include <errno.h>

/** This module addresses the FreeRTOS simulator's coexistence with Linux system calls from user apps.
 *  It wraps select so that it doesn't block the FreeRTOS task calling it, so that the
 *  scheduler will allow lower priority tasks to run.
 *  Without the wrapper, most components such as ESP-MQTT block lower priority tasks from running at all.
 */
typedef int (*select_func_t)(int fd, fd_set *rfds, fd_set *wfds, fd_set *efds, struct timeval *tval);

int select(int fd, fd_set *rfds, fd_set *wfds, fd_set *efds, struct timeval *tval)
{
    static select_func_t s_real_select = NULL;
    TickType_t end_ticks = portMAX_DELAY;
    fd_set o_rfds, o_wfds, o_efds;

    // Lookup the select symbol
    if (s_real_select == NULL) {
        s_real_select = (select_func_t)dlsym(RTLD_NEXT, "select");
        assert(s_real_select);  // abort() if we cannot locate the symbol
    }

    // Calculate the end_ticks if a timeout is provided
    if (tval != NULL) {
        end_ticks = xTaskGetTickCount() + pdMS_TO_TICKS(tval->tv_sec * 1000 + tval->tv_usec / 1000);
    }

    // Preserve the original FD sets as select call will change them
    if (rfds) {
        o_rfds = *rfds;
    }
    if (wfds) {
        o_wfds = *wfds;
    }
    if (efds) {
        o_efds = *efds;
    }

    while (1) {
        // Restore original FD sets before the select call
        if (rfds) {
            *rfds = o_rfds;
        }
        if (wfds) {
            *wfds = o_wfds;
        }
        if (efds) {
            *efds = o_efds;
        }

        // Call select with a zero timeout to avoid blocking
        struct timeval zero_tv = {0, 0};
        int ret = s_real_select(fd, rfds, wfds, efds, &zero_tv);

        // Return on success
        if (ret > 0) {
            return ret;
        }

        // Return on any error except EINTR
        if (ret == -1 && errno != EINTR) {
            return ret;
        }

        /**
         * Sleep for maximum 10 tick(s) to allow other tasks to run.
         * This can be any value greater than zero.
         * 10 is a good trade-off between CPU time usage and timeout resolution.
         */
        const TickType_t max_sleep_ticks = 10;
        TickType_t sleep_ticks = max_sleep_ticks;

        if (tval != NULL) {
            TickType_t now_ticks = xTaskGetTickCount();
            if (now_ticks >= end_ticks) {
                errno = 0;
                return 0;
            }
            // Sleep for the remaining time or a maximum of 10 tick
            TickType_t remaining_ticks = end_ticks - now_ticks;
            sleep_ticks = (remaining_ticks < max_sleep_ticks) ? remaining_ticks : max_sleep_ticks;
        }

        vTaskDelay(sleep_ticks);
    }
}
