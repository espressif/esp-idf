系统
====

:link_to_translation:`en:[English]`

默认 LibC 已从 Newlib 更改为 PicolibC
--------------------------------------

自 ESP-IDF v6.0 起，构建时默认使用的 LibC 已更改为 PicolibC。

.. note::

    PicolibC 是 Newlib 的一个分支，重新实现了 stdio，其目标是减少内存消耗。

虽然二进制文件大小减小，且 I/O 操作时的栈消耗会降低，但在大多数情况下，应用程序的行为不会发生变化。

.. warning::

    **重大变更：** 无法像使用 Newlib 时那样，为特定任务重新定义 ``stdin``、 ``stdout`` 和 ``stderr``。这些流是全局的，并在所有任务之间共享。这是符合 POSIX 标准的行为。

:ref:`CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY` 是默认启用的，通过提供全局 ``stdin``、 ``stdout``、 ``stderr`` 的线程本地副本以及 ``getreent()`` 实现，为 Newlib 提供有限兼容性。如果使用 Newlib 头文件构建的库操作 ``struct reent`` 的内部字段，可能会导致任务栈损坏。请注意，只有 Newlib 库本身才应该操作 ``struct reent`` 字段。

如果你没有链接使用 Newlib 头文件构建的外部库，可以禁用 :ref:`CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY` 来节省少量内存。

Newlib 仍然在 ESP-IDF 工具链中维护。如需切换使用 Newlib，可在 menuconfig 中通过 :ref:`CONFIG_LIBC` 选项选择 LIBC_NEWLIB。

Newlib 与 Picolibc 对比
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

以下示例说明切换到 Picolibc 原因：

.. code-block:: c

    FILE *f = fopen("/dev/console", "w");
    for (int i = 0; i < 10; i++)
    {
        fprintf(f, "hello world %s\n", "🤖");
        fprintf(f, "%.1000f\n", 3.141592653589793);
        fprintf(f, "%1000d\n", 42);
    }

该示例代码使用 Newlib 和 Picolibc 分别编译，并在 ESP32-C3 上对比结果：

.. list-table:: Newlib 与 Picolibc 对比
   :header-rows: 1
   :widths: 30 20 20 20

   * - 指标
     - Newlib
     - Picolibc
     - 差异
   * - 二进制大小（字节）
     - 280,128
     - 224,656
     - 19.80%
   * - 栈使用（字节）
     - 1,748
     - 802
     - 54.12%
   * - 堆使用（字节）
     - 1,652
     - 376
     - 77.24%
   * - 性能（CPU 周期）
     - 278,232,026
     - 279,823,800
     - 0.59%

.. note::

    即使启用 :ref:`CONFIG_LIBC_NEWLIB_NANO_FORMAT` 选项（禁用浮点格式化），使用 Picolibc 的应用程序仍比 Newlib 小 6%（224,592 v.s. 239,888 字节）。

Xtensa
------

Xtensa 特殊寄存器头文件已更新，使用新的命名约定。旧的 ``specreg.h`` 头文件现已被弃用，并将在未来版本中移除。

寄存器名称已更新为使用 ``XT_REG_`` 前缀。请使用新的 ``xt_specreg.h`` 文件。

电源管理
--------

在旧版本的 ESP-IDF 中，使用 :cpp:func:`esp_sleep_get_wakeup_cause` API 获取芯片从睡眠中唤醒的原因，然而，该函数在多个唤醒源同时激活的情况下，只会返回其中一个唤醒源，可能导致用户遗漏其他同时发生的唤醒条件。

自 v6.0 版本起，ESP-IDF 新增 :cpp:func:`esp_sleep_get_wakeup_causes` API，此函数返回一个 bitmap，表示所有触发唤醒的唤醒源。每一位对应 :cpp:type:`esp_sleep_wakeup_cause_t` 枚举中的一个值（例如 ESP_SLEEP_WAKEUP_TIMER、ESP_SLEEP_WAKEUP_EXT1 等），用户可以通过按位与操作判断是否被对应源唤醒。

原先的 :cpp:func:`esp_sleep_get_wakeup_cause()` 函数现已标记为已弃用，建议用户迁移至新接口。未来版本中，该函数可能会被移除。用户可按以下示例进行迁移：

旧代码：

.. code-block:: c

    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause == ESP_SLEEP_WAKEUP_EXT1) {
        handle_ext1_wakeup();
    }

现在需要修改成：

.. code-block:: c

    uint32_t causes = esp_sleep_get_wakeup_causes();
    if (causes & BIT(ESP_SLEEP_WAKEUP_EXT1)) {
        handle_ext1_wakeup();
    }
    if (causes & BIT(ESP_SLEEP_WAKEUP_TIMER)) {
        handle_timer_wakeup();
    }

.. _gpio_wakeup_api_changes:

GPIO 唤醒 API 变更
^^^^^^^^^^^^^^^^^^

以下 API 和类型已被移除，并替换为支持 Deep Sleep 和 Light Sleep（当外设电源域掉电时）的新 API：

**已移除的 API：**

- :func:`esp_deep_sleep_enable_gpio_wakeup` - 请使用 :func:`esp_sleep_enable_gpio_wakeup_on_hp_periph_powerdown` 替代
- :func:`gpio_deep_sleep_wakeup_enable` - 请使用 :func:`gpio_wakeup_enable_on_hp_periph_powerdown_sleep` 替代
- :func:`gpio_deep_sleep_wakeup_disable` - 请使用 :func:`gpio_wakeup_disable_on_hp_periph_powerdown_sleep` 替代

**已移除的类型：**

- :cpp:type:`esp_deepsleep_gpio_wake_up_mode_t` - 请使用 :cpp:type:`esp_sleep_gpio_wake_up_mode_t` 替代

**已移除的宏：**

- ``GPIO_IS_DEEP_SLEEP_WAKEUP_VALID_GPIO()`` - 请使用 ``GPIO_IS_HP_PERIPH_PD_WAKEUP_VALID_IO()`` 替代

**迁移示例：**

旧代码：

.. code-block:: c

    #include "esp_sleep.h"
    #include "driver/gpio.h"

    // 为深度睡眠启用 GPIO 唤醒
    esp_deep_sleep_enable_gpio_wakeup(BIT(GPIO_NUM_0), ESP_GPIO_WAKEUP_GPIO_LOW);

    // 或使用 GPIO 驱动 API
    gpio_deep_sleep_wakeup_enable(GPIO_NUM_0, GPIO_INTR_LOW_LEVEL);

    // 检查 GPIO 是否可用于深度睡眠唤醒
    if (GPIO_IS_DEEP_SLEEP_WAKEUP_VALID_GPIO(GPIO_NUM_0)) {
        // ...
    }

新代码：

.. code-block:: c

    #include "esp_sleep.h"
    #include "driver/gpio.h"

    // 为深度睡眠或轻量睡眠（当外设电源域掉电时）启用 GPIO 唤醒
    esp_sleep_enable_gpio_wakeup_on_hp_periph_powerdown(BIT(GPIO_NUM_0), ESP_GPIO_WAKEUP_GPIO_LOW);

    // 或使用 GPIO 驱动 API
    gpio_wakeup_enable_on_hp_periph_powerdown_sleep(GPIO_NUM_0, GPIO_INTR_LOW_LEVEL);

    // 检查 GPIO 是否可用于外设电源域掉电睡眠唤醒
    if (GPIO_IS_HP_PERIPH_PD_WAKEUP_VALID_IO(GPIO_NUM_0)) {
        // ...
    }

**注意：** 新的 API 同时支持 Deep Sleep 和 Light Sleep 模式（当在 menuconfig 中启用 ``PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP`` 时）。

引导加载程序
------------

已移除使用无优化等级 (-O0, `CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_NONE`) 编译引导加载程序的选项。在大多数芯片上，使用 -O0 编译引导加载程序已无法成功，因为 IRAM 段会溢出。对于调试目的，推荐使用 -Og (:ref:`CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_DEBUG<CONFIG_BOOTLOADER_COMPILER_OPTIMIZATION_DEBUG>`) 优化等级，它在优化效果与可调试性之间提供了良好的平衡。

时间管理
--------

- 已弃用的头文件 ``{IDF_TARGET_NAME}/rtc.h`` 已被移除，请改用替代头文件 ``esp_rtc_time.h``。
- 已弃用的 ``RTC_CLK_SRC_INT_RC32K`` 配置项已被移除，内部 RC32K 时钟在极端温度下不稳定，请不要使用该时钟作为 RTC 低速时钟的时钟源。

硬件支持
--------

已弃用的头文件 ``soc_memory_types.h`` 已被移除，请改用替代头文件 ``esp_memory_utils.h``。

已弃用的头文件 ``intr_types.h`` 已被移除，请改用替代头文件 ``esp_intr_types.h``。

已弃用的头文件 ``esp_private/interrupt_deprecated.h`` （此前通过 ``riscv/interrupt.h`` 头文件提供）已被移除。相关已弃用的函数不再可用，请改用非弃用版本。

头文件 ``esp_fault.h`` 已从 ``esp_hw_support`` 组件移动到 ``esp_common`` 组件。如果应用程序在此更改后遇到构建错误，请在 ``CMakeLists.txt`` 中将 ``esp_common`` 添加到组件的 ``REQUIRES`` 或 ``PRIV_REQUIRES`` 列表中。

ROM 头文件
-----------

已从 ``ets_sys.h`` ROM 头文件中移除了已弃用的 ``STATUS`` 类型。请改用 ``ETS_STATUS``。

App 追踪
----------

配置更改
^^^^^^^^^^

应用程序跟踪配置菜单已移动。之前位于 ``Component config`` > ``Application Level Tracing``，现在位于 ``Component config`` > ``ESP Trace Configuration``。

此前，应用程序跟踪功能会在配置目标传输方式时自动启用。现在必须在配置任何目标传输方式前，通过选择跟踪传输方式显式启用应用程序跟踪功能。

如需启用应用程序跟踪，请在 menuconfig 中依次进入 ``Component config`` > ``ESP Trace Configuration`` > ``Trace transport`` 并选择 ``ESP-IDF apptrace``。之后，可以在 ``Component config`` > ``ESP Trace Configuration`` > ``Application Level Tracing`` 中进行配置。

如果要在独立模式下使用 apptrace 而不使用库（例如禁用 SEGGER SystemView 时），需要在 sdkconfig 文件中设置以下配置：

.. code-block:: none

    CONFIG_ESP_TRACE_ENABLE=y
    CONFIG_ESP_TRACE_LIB_NONE=y
    CONFIG_ESP_TRACE_TRANSPORT_APPTRACE=y

这些配置也可以按上述说明通过 menuconfig 进行设置。

已移除额外数据缓冲选项。不再支持 ``CONFIG_APPTRACE_PENDING_DATA_SIZE_MAX`` 配置项。

已移除弃用的 ``ESP_APPTRACE_DEST_TRAX`` 枚举值。请改用 ``ESP_APPTRACE_DEST_JTAG``。

组件依赖更改
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

应用程序现在需要 ``esp_trace`` 组件，而不是 ``app_trace``。请更新组件的 ``CMakeLists.txt`` 文件以反映此更改。

``app_trace`` 组件现在是 ``esp_trace`` 的子组件，将在需要时自动包含。

初始化流程更改
^^^^^^^^^^^^^^^^^^^

对于运行时配置覆盖，提供了新的回调系统。详细信息请参见 :doc:`应用程序跟踪文档 <../../../api-guides/app_trace>` 中关于 ``esp_apptrace_get_user_params()`` 和 ``esp_trace_get_user_params()`` 的说明。

API 更改
^^^^^^^^^^^

所有 apptrace API 中的目标参数已被移除。

默认目标现在在 menuconfig 中的 ``Component config`` > ``ESP Trace Configuration`` > ``Application Level Tracing`` 下配置，并且可以在运行时通过提供带有自定义跟踪配置的回调来更改。

UART 目标配置已简化：

- 移除：通过 ``CONFIG_APPTRACE_DEST_UARTx=y`` 选择单个 UART
- 新增：通过 ``CONFIG_APPTRACE_DEST_UART_NUM`` 选择 UART 端口

迁移方法，更新你的 sdkconfig 配置：

旧配置：

.. code-block:: none

    CONFIG_APPTRACE_DEST_UART0=y
    # 或
    CONFIG_APPTRACE_DEST_UART1=y

新配置：

.. code-block:: none

    CONFIG_APPTRACE_DEST_UART=y
    CONFIG_APPTRACE_DEST_UART_NUM=0  # 或 1、2，具体取决于目标芯片

SEGGER SystemView
^^^^^^^^^^^^^^^^^

SystemView 组件已迁移至独立仓库。`esp_sysview <https://components.espressif.com/components/espressif/esp_sysview>`_ 现为托管组件。

配置
^^^^

SystemView 配置在 menuconfig 中的位置已变更。仅当项目在 ``idf_component.yml`` 中添加 ``esp_sysview`` 组件依赖，并在 menuconfig 中选择外部库时，相关配置选项才会显示。

在组件清单中添加依赖：

.. code-block:: yaml

    dependencies:
      espressif/esp_sysview: ^1

随后在 menuconfig 中依次选择：``Component config`` > ``ESP Trace Configuration`` > ``Trace library`` > ``External library from component registry``。

之后，可通过 ``Component config`` > ``SEGGER SystemView Configuration`` 查看 SystemView 配置选项。

SystemView 不再拥有独立的传输目标配置。它与应用程序跟踪传输方式（JTAG 或 UART）共享配置。

FreeRTOS
--------

已移除的函数
^^^^^^^^^^^^^^

以下已弃用的 FreeRTOS 函数已在 ESP-IDF v6.0 中移除：

- :cpp:func:`xTaskGetAffinity` – 请使用 :cpp:func:`xTaskGetCoreID` 替代。
- :cpp:func:`xTaskGetIdleTaskHandleForCPU` – 请使用 :cpp:func:`xTaskGetIdleTaskHandleForCore` 替代。
- :cpp:func:`xTaskGetCurrentTaskHandleForCPU` – 请使用 :cpp:func:`xTaskGetCurrentTaskHandleForCore` 替代。

以下兼容性函数已在 ESP-IDF v6.0 中移除。这些函数原本是为了向后兼容旧版本 ESP-IDF 而维护的，因为它们在 FreeRTOS 中已被更改为宏或独立函数。现已移除此兼容性支持。

- :cpp:func:`xQueueGenericReceive` - 请根据具体使用场景选择 :cpp:func:`xQueueReceive`、:cpp:func:`xQueuePeek` 或 :cpp:func:`xQueueSemaphoreTake` 替代。
- :cpp:func:`vTaskDelayUntil` - 请使用 :cpp:func:`xTaskDelayUntil` 替代。
- :cpp:func:`ulTaskNotifyTake` - 请使用宏 ``ulTaskNotifyTake`` 替代。
- :cpp:func:`xTaskNotifyWait` - 请使用宏 ``xTaskNotifyWait`` 替代。

已弃用的函数
^^^^^^^^^^^^^^

函数 :cpp:func:`pxTaskGetStackStart` 已弃用。请使用 :cpp:func:`xTaskGetStackStart` 替代以提高类型安全性。

新增 API
^^^^^^^^^

任务快照 API 已对外公开，以支持 ESP Insights 等外部框架。这些 API 现通过 ``freertos/freertos_debug.h`` 头文件提供，不再使用已弃用的 ``freertos/task_snapshot.h``。

在调度程序运行时安全使用的方案是：调用快照函数前先执行 ``vTaskSuspendAll()`` 暂停所有任务，完成后调用 ``xTaskResumeAll()`` 恢复运行。

内存布局
^^^^^^^^^^^^

- 为了减少 IRAM 的使用，大多数 FreeRTOS 函数的默认存储位置已从 IRAM 更改为 flash。因此，``CONFIG_FREERTOS_PLACE_FUNCTIONS_INTO_FLASH`` 选项已被移除。这项变更可显著节省 IRAM 空间，但可能会对性能造成轻微影响。如果应用对性能有严苛要求，可通过启用新选项 :ref:`CONFIG_FREERTOS_IN_IRAM` 恢复原先配置。
- 启用 ``CONFIG_FREERTOS_IN_IRAM`` 前，建议进行性能测试以评估对具体应用场景的实际影响。flash 和 IRAM 配置的性能差异取决于 flash 缓存效率、API 调用模式和系统负载等因素。
- ``components/freertos/test_apps/freertos/performance/test_freertos_api_performance.c`` 中提供了基准性能测试。该测试测量常用 FreeRTOS API 的执行时长，可帮助开发者根据目标硬件和应用需求评估内存布局方案带来的性能影响。
- 当启用 ``CONFIG_ESP_PANIC_HANDLER_IRAM`` 时，任务快照函数会自动存入 IRAM，确保在系统崩溃处理期间仍可调用。
- 除非启用 ``CONFIG_FREERTOS_PLACE_ISR_FUNCTIONS_INTO_FLASH``，否则 ``vTaskGetSnapshot`` 将始终保留在 IRAM 中，因为该函数被任务看门狗中断处理程序所调用。

已移除的配置选项
^^^^^^^^^^^^^^^^^^

以下隐藏（且始终启用）的配置选项已被移除：

- ``CONFIG_FREERTOS_ENABLE_TASK_SNAPSHOT``
- ``CONFIG_FREERTOS_PLACE_SNAPSHOT_FUNS_INTO_FLASH``

环形缓冲区
----------

内存布局
^^^^^^^^^^

为了减少 IRAM 的使用，`esp_ringbuf` 函数的默认位置已从 IRAM 更改为 flash。因此，``CONFIG_RINGBUF_PLACE_FUNCTIONS_INTO_FLASH`` 选项已被移除。此举可节省大量 IRAM，但可能会对性能造成轻微影响。对于性能要求严苛的应用程序，可通过启用新增的 :ref:`CONFIG_RINGBUF_IN_IRAM` 选项来恢复之前的行为。

Log
---

已移除的函数
^^^^^^^^^^^^^^

以下已弃用的 Log 函数已在 ESP-IDF v6.0 中移除：

- :cpp:func:`esp_log_buffer_hex` – 请使用 :cpp:func:`ESP_LOG_BUFFER_HEX` 替代。
- :cpp:func:`esp_log_buffer_char` – 请使用 :cpp:func:`ESP_LOG_BUFFER_CHAR` 替代。

已移除的头文件
^^^^^^^^^^^^^^^

- ``esp_log_internal.h`` – 请使用 ``esp_log_buffer.h`` 替代。

ESP-Event
---------

不必要的 FreeRTOS 头文件已从 ``esp_event.h`` 中移除。此前依赖这些隐式包含的代码，现在必须显式添加以下头文件：``#include "freertos/queue.h"`` 和 ``#include "freertos/semphr.h"``。

核心转储
--------

二进制数据格式已被弃用。`CONFIG_ESP_COREDUMP_DATA_FORMAT_BIN` 表示该功能已完全删除，不再可用。现在默认的数据格式是 ELF。

CRC 数据完整性检查已被弃用。`ESP_COREDUMP_CHECKSUM_CRC32` 表示该功能已完全删除，不再可用。现在默认的校验和算法为 SHA256。

函数 :cpp:func:`esp_core_dump_partition_and_size_get()` 现在对空白（已擦除）分区返回 `ESP_ERR_NOT_FOUND`，而不是 `ESP_ERR_INVALID_SIZE`。

OTA 更新
-----------

ESP HTTPS OTA 的分段下载功能已移至配置选项下，以便在未使用分段下载时减少内存占用。

如果要在 OTA 应用中使用分段下载功能，需要在 menuconfig 中启用组件级配置 :ref:`CONFIG_ESP_HTTPS_OTA_ENABLE_PARTIAL_DOWNLOAD` (``Component config`` > ``ESP HTTPS OTA`` > ``Enable partial HTTP download for OTA``)。

已移除的废弃 API
^^^^^^^^^^^^^^^^^^^^

以下废弃函数已从 ``app_update`` 组件中移除：

- :cpp:func:`esp_ota_get_app_description` – 请使用 :cpp:func:`esp_app_get_description` 代替。
- :cpp:func:`esp_ota_get_app_elf_sha256` – 请使用 :cpp:func:`esp_app_get_elf_sha256` 代替。

这些函数已移至 ``esp_app_format`` 组件。请将包含文件从 ``esp_ota_ops.h`` 更新为 ``esp_app_desc.h``，如有需要请将 ``esp_app_format`` 添加到您的组件依赖项中。

Gcov
----

gcov 组件已移至独立仓库。`esp_gcov <https://components.espressif.com/components/espressif/esp_gcov>`_ 现为托管组件。

组件依赖
^^^^^^^^^^^

使用 gcov 功能的项目现在必须在 ``idf_component.yml`` 清单文件中添加 esp_gcov 组件作为依赖项：

.. code-block:: yaml

    dependencies:
      espressif/esp_gcov: ^1

配置更改
^^^^^^^^^^^

gcov 配置选项已从应用程序级别追踪菜单移至专用的 ``GNU Code Coverage`` 菜单。

``CONFIG_APPTRACE_GCOV_ENABLE`` 选项已重命名为 ``CONFIG_ESP_GCOV_ENABLE``。

头文件更改
^^^^^^^^^^^

对于 gcov 功能，请改用 ``esp_gcov.h`` 头文件替代原有的 ``esp_app_trace.h``。

系统控制台 (STDIO)
----------------------

``esp_vfs_cdcacm.h`` 头文件已移至新组件 ``esp_usb_cdc_romconsole`` 中。若需使用该头文件中的任何函数，现在需要显式添加对 ``esp_usb_cdc_rom_console`` 的 ``REQUIRES`` 依赖项。

LibC
------

:ref:`CONFIG_COMPILER_ASSERT_NDEBUG_EVALUATE` 的默认值已改为 `n`。这意味着当设置了 ``NDEBUG`` 时，断言将不再对断言内的表达式进行求值。此更改将默认行为恢复为与 C 语言标准一致。

ULP
---

LP-Core 在深度睡眠期间遇到异常时，将唤醒主 CPU。此功能默认启用，若不需要此行为，可以通过 :ref:`CONFIG_ULP_TRAP_WAKEUP` Kconfig 配置选项禁用。

堆
----

在 ESP-IDF 旧版本中，无论是否启用内存保护配置， ``MALLOC_CAP_EXEC`` 都是可用的。这意味着，当启用了 ``CONFIG_ESP_SYSTEM_MEMPROT_FEATURE`` 或 ``CONFIG_ESP_SYSTEM_PMP_IDRAM_SPLIT`` 时，如果调用 :cpp:func:`heap_caps_malloc` 并指定 ``MALLOC_CAP_EXEC``，该函数会返回 ``NULL``。

自 ESP-IDF v6.0 起， ``MALLOC_CAP_EXEC`` 的定义改为条件性定义。也就是说，当启用 ``CONFIG_ESP_SYSTEM_MEMPROT`` 时， ``MALLOC_CAP_EXEC`` 将不会被定义。因此，如果代码中仍然使用 ``MALLOC_CAP_EXEC``，会在编译阶段报错。

``esp_common``
----------------

- ``EXT_RAM_ATTR`` 自 v5.0 已弃用，现已移除。请使用 ``EXT_RAM_BSS_ATTR`` 宏将 ``.bss`` 放在 PSRAM 上。
- RTC 相关内存属性 (``RTC_x_ATTR``) 已从没有 RTC 存储的芯片中移除。
