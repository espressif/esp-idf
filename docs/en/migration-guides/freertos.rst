Migrate FreeRTOS to ESP-IDF 5.0
==================================

Tasks Snapshot
--------------

The header ``task_snapshot.h`` has been removed from ``freertos/task.h``. ESP-IDF developers should include ``"freertos/task_snapshot.h``` in case they need tasks snapshot API.


FreeRTOS Asserts
----------------
Previously FreeRTOS asserts were configured separately from the rest of the system using the `FREERTOS_ASSERT` kconfig option. This option has now been removed and the configuration is now done through `COMPILER_OPTIMIZATION_ASSERTION_LEVEL`.
