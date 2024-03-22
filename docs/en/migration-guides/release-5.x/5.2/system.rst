System
======

:link_to_translation:`zh_CN:[中文]`

FreeRTOS
--------

IDF FreeRTOS Upgrade
^^^^^^^^^^^^^^^^^^^^

The IDF FreeRTOS kernel (which is a dual-core SMP implementation of FreeRTOS) has been upgraded to be based on Vanilla FreeRTOS v10.5.1. With this upgrade, the design and implementation of IDF FreeRTOS has also been changed significantly. As a result, users should take note of the following changes to kernel behavior and API:

- When enabling single-core mode via the :ref:`CONFIG_FREERTOS_UNICORE` option, the kernel's behavior will now be identical to Vanilla FreeRTOS (see :ref:`freertos-idf-single-core` for more details).
- For SMP related APIs that were added by IDF FreeRTOS, checks on ``xCoreID`` arguments are now stricter. Providing out of range values for ``xCoreID`` arguments will now trigger an assert.
- The following SMP related APIs are now deprecated and replaced due to naming consistency reasons:

    - ``xTaskGetAffinity()`` is deprecated, call :cpp:func:`xTaskGetCoreID` instead.
    - ``xTaskGetIdleTaskHandleForCPU()`` is deprecated, call :cpp:func:`xTaskGetIdleTaskHandleForCore` instead.
    - ``xTaskGetCurrentTaskHandleForCPU()`` is deprecated, call :cpp:func:`xTaskGetCurrentTaskHandleForCore` instead.

Task Snapshot
^^^^^^^^^^^^^

The Task Snapshot API has been made private due to a lack of a practical way for the API to be used from user code (the scheduler must be halted before the API can be called).

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    Xtensa
    ------

    A number of legacy include paths for Xtensa headers have been deprecated:

    - ``#include "freertos/xtensa_api.h"`` is deprecated, please use ``#include "xtensa_api.h"`` instead.
    - ``#include "freertos/xtensa_context.h"`` is deprecated, please use ``#include "xtensa_context.h"`` instead.
    - ``#include "freertos/xtensa_timer.h"`` is deprecated, please use ``#include "xtensa_timer.h"`` instead.


Panic Handler Behavior
----------------------

The choice ``CONFIG_ESP_SYSTEM_PANIC_GDBSTUB`` in the configuration option :ref:`CONFIG_ESP_SYSTEM_PANIC` has been made dependent on whether the ``esp_gdbstub`` component is included in the build. When trimming the list of components in the build using ``set(COMPONENTS main)``, ``esp_gdbstub`` component has to be added to this list of components to make the ``CONFIG_ESP_SYSTEM_PANIC_GDBSTUB`` option available.
