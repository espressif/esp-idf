Migrate FreeRTOS to ESP-IDF 5.0
==================================

Legacy API and Data Types
-------------------------

Previously, the `configENABLE_BACKWARD_COMPATIBILITY` option was set by default, thus allowed pre FreeRTOS v8.0.0 function names and data types to be used. The `configENABLE_BACKWARD_COMPATIBILITY` is now disabled by default, thus legacy FreeRTOS names/types are no longer supportd by default. Users should either:

- Update their code to remove usage of legacy FreeRTOS names/types
- Enable the :ref:`CONFIG_FREERTOS_ENABLE_BACKWARD_COMPATIBILITY` to explicitly allow the usage of legacy names/types

Tasks Snapshot
--------------

The header ``task_snapshot.h`` has been removed from ``freertos/task.h``. ESP-IDF developers should include ``"freertos/task_snapshot.h``` in case they need tasks snapshot API.


FreeRTOS Asserts
----------------
Previously FreeRTOS asserts were configured separately from the rest of the system using the `FREERTOS_ASSERT` kconfig option. This option has now been removed and the configuration is now done through `COMPILER_OPTIMIZATION_ASSERTION_LEVEL`.
