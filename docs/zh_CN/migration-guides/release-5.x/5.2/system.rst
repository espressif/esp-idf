系统
====

:link_to_translation:`en:[English]`

FreeRTOS
--------

升级 IDF FreeRTOS
^^^^^^^^^^^^^^^^^^

IDF FreeRTOS 内核（通过 FreeRTOS 实现的双核 SMP ）已升级，现基于 Vanilla FreeRTOS v10.5.1。伴随升级，IDF FreeRTOS 的设计和实现也发生了重大变化。因此，用户应注意以下关于内核行为和 API 的变化：

- 通过 :ref:`CONFIG_FREERTOS_UNICORE` 选项启用单核模式时，内核行为将与 Vanilla FreeRTOS 完全相同（详见 :ref:`freertos-idf-single-core`）。
- 对于由 IDF FreeRTOS 添加的与 SMP 相关的 API，xCoreID 参数的检查将更加严格。为 ``xCoreID`` 参数提供超出范围的值现在将触发断言。
- 为了保证命名一致性，以下与 SMP 相关的 API 现已被弃用并已重新更名：

    - 弃用 ``xTaskGetAffinity()``，更名为 :cpp:func:`xTaskGetCoreID`。
    - 弃用 ``xTaskGetIdleTaskHandleForCPU()``，更名为 :cpp:func:`xTaskGetIdleTaskHandleForCore`。
    - 弃用 ``xTaskGetCurrentTaskHandleForCPU()``，更名为 :cpp:func:`xTaskGetCurrentTaskHandleForCore`。

任务快照
^^^^^^^^

由于缺乏从用户代码中使用该 API 的实际方法（必须先暂停调度器才能调用该 API），任务快照 API 已被设置为私有。

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    Xtensa
    ------

    几个 Xtensa 头文件的旧版包含路径已被弃用：

    - 弃用 ``#include "freertos/xtensa_api.h"``，请改用 ``#include "xtensa_api.h"``。
    - 弃用 ``#include "freertos/xtensa_context.h"``，请改用 ``#include "xtensa_context.h"``。
    - 弃用 ``#include "freertos/xtensa_timer.h"``，请改用 ``#include "xtensa_timer.h"``。


紧急处理程序行为
----------------

在配置选项 :ref:`CONFIG_ESP_SYSTEM_PANIC` 中，是否可选择 ``CONFIG_ESP_SYSTEM_PANIC_GDBSTUB`` 取决于是否在构建中包含了 ``esp_gdbstub`` 组件。使用 ``set(COMPONENTS main)`` 来缩减这类构建的组件列表时，必须将 ``esp_gdbstub`` 组件添加到这个组件列表中，以使 ``CONFIG_ESP_SYSTEM_PANIC_GDBSTUB`` 选项可用。
