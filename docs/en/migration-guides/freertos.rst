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

The function :cpp:func:`vTaskGetSnapshot` now returns ``BaseType_t``. Return value shall be ``pdTRUE`` on success and ``pdFALSE`` otherwise.

FreeRTOS Asserts
----------------
Previously FreeRTOS asserts were configured separately from the rest of the system using the `FREERTOS_ASSERT` kconfig option. This option has now been removed and the configuration is now done through `COMPILER_OPTIMIZATION_ASSERTION_LEVEL`.

Port Macro APIs
---------------
The file ``portmacro_deprecated.h`` which was added to maintain backward compatibility for deprecated APIs is removed. Users are advised to use the alternate functions for the deprecated APIs as listed below:

- ``portENTER_CRITICAL_NESTED()`` is removed. Users should use the ``portSET_INTERRUPT_MASK_FROM_ISR()`` macro instead.
- ``portEXIT_CRITICAL_NESTED()`` is removed. Users should use the ``portCLEAR_INTERRUPT_MASK_FROM_ISR()`` macro instead.
- ``vPortCPUInitializeMutex()`` is removed. Users should use the ``spinlock_initialize()`` function instead.
- ``vPortCPUAcquireMutex()`` is removed. Users should use the ``spinlock_acquire()`` function instead.
- ``vPortCPUAcquireMutexTimeout()`` is removed. Users should use the ``spinlock_acquire()`` function instead.
- ``vPortCPUReleaseMutex()`` is removed. Users should use the ``spinlock_release()`` function instead.
