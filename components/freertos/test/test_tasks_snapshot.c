/*
 Test FreeRTOS support for core dump.
*/

#include <stdio.h>
#include "soc/cpu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task_snapshot.h"
#include "unity.h"
#include "sdkconfig.h"

#define TEST_MAX_TASKS_NUM	32

/* simple test to check that in normal conditions uxTaskGetSnapshotAll does not generate exception */
TEST_CASE("Tasks snapshot", "[freertos]")
{
    TaskSnapshot_t tasks[TEST_MAX_TASKS_NUM];
    UBaseType_t tcb_sz;
#ifndef CONFIG_FREERTOS_UNICORE
    int other_core_id = xPortGetCoreID() == 0 ? 1 : 0;
#endif

    // uxTaskGetSnapshotAll is supposed to be called when all tasks on both CPUs are
    // inactive and can not alter FreeRTOS internal tasks lists, e.g. from panic handler
    unsigned state = portSET_INTERRUPT_MASK_FROM_ISR();
#ifndef CONFIG_FREERTOS_UNICORE
    esp_cpu_stall(other_core_id);
#endif
    UBaseType_t task_num = uxTaskGetSnapshotAll(tasks, TEST_MAX_TASKS_NUM, &tcb_sz);
#ifndef CONFIG_FREERTOS_UNICORE
    esp_cpu_unstall(other_core_id);
#endif
    portCLEAR_INTERRUPT_MASK_FROM_ISR(state);

    printf("Dumped %d tasks. TCB size %d\n", task_num, tcb_sz);
    TEST_ASSERT_NOT_EQUAL(0, task_num);
    TEST_ASSERT_NOT_EQUAL(0, tcb_sz);
}
