/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * On Xtensa unicore, portYIELD_WITHIN_API() only pends a yield and returns.
 * vTaskDelete(NULL) must not come back to the deleted task: under Clang -O2
 * that continuation can fall off the task entry and panic with
 * IllegalInstruction. See https://github.com/espressif/esp-idf/issues/18460
 *
 * The child body is only vTaskDelete(), matching the original repro. Extra
 * code after vTaskDelete() (a flag store or busy-loop) is enough work that
 * the deferred yield interrupt often wins, so those checks miss the bug.
 *
 * app_main then returns so main_task also self-deletes (same kernel path).
 * A lower-priority heartbeat is created after PASS and must keep printing
 * if that second delete did not panic.
 */
#include <array>
#include <cstdio>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace {

StaticTask_t s_self_tcb;
StaticTask_t s_heartbeat_tcb;
std::array<StackType_t, 4096> s_self_stack;
std::array<StackType_t, 4096> s_heartbeat_stack;

void self_delete_must_not_return(void *arg)
{
    (void)arg;
    vTaskDelete(nullptr);
}

void heartbeat(void *arg)
{
    (void)arg;
    for (;;) {
        std::printf("task_self_delete: still running\n");
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

} // namespace

extern "C" void app_main(void)
{
    TaskHandle_t self_del = xTaskCreateStatic(self_delete_must_not_return,
                                              "self_del",
                                              s_self_stack.size(),
                                              nullptr,
                                              configMAX_PRIORITIES - 1,
                                              s_self_stack.data(),
                                              &s_self_tcb);
    if (self_del == nullptr) {
        std::printf("task_self_delete: FAIL create\n");
        return;
    }

    /* Child was higher priority, so it has already run. Surviving to here
     * means it did not fall off its entry after vTaskDelete(). */
    std::printf("task_self_delete: PASS\n");

    TaskHandle_t hb = xTaskCreateStatic(heartbeat,
                                        "hb",
                                        s_heartbeat_stack.size(),
                                        nullptr,
                                        tskIDLE_PRIORITY + 1,
                                        s_heartbeat_stack.data(),
                                        &s_heartbeat_tcb);
    if (hb == nullptr) {
        std::printf("task_self_delete: FAIL create\n");
        return;
    }
}
