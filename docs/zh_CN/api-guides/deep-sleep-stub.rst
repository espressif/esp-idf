Deep-sleep 唤醒存根
===================

:link_to_translation:`en:[English]`

简介
----

与 Light-sleep 和 Modem-sleep 模式相比，Deep-sleep 模式的唤醒时间要长得多，因为在这种情况下 ROM 和 RAM 都被关闭，而 CPU 需要更多时间进行 SPI 引导。不过 {IDF_TARGET_NAME} 支持在退出 Deep-sleep 模式时运行“Deep-sleep 唤醒存根”，此功能在芯片被唤醒后，在任何正常初始化、引导加载程序或 ESP-IDF 代码运行之前会立即运行。

具体来说，从 Deep-sleep 模式中唤醒后，{IDF_TARGET_NAME} 开始部分初始化，然后使用 CRC 对 RTC 快速内存进行验证，如果验证通过，则执行唤醒存根代码。

由于 {IDF_TARGET_NAME} 刚刚从 Deep-sleep 模式唤醒，大多数外设处于复位状态，SPI flash 也未被映射，因此，唤醒存根代码只能调用在 ROM 中实现或加载到 RTC 快速内存中的函数，后者在 Deep-sleep 期间保留内容。

综上所述，通过在应用程序中调用唤醒存根功能，可以在从 Deep-sleep 模式中唤醒时快速运行一些代码，而无需等待整个启动过程。但是，存根大小受 RTC 快速内存大小的限制。

.. only:: SOC_RTC_SLOW_MEM_SUPPORTED

    {IDF_TARGET_NAME} 支持 RTC 快速内存和 RTC 慢速内存。唤醒存根代码应加载到 RTC 快速内存中，代码使用的数据应存储到 RTC 快速内存或 RTC 慢速内存中。

.. only:: not SOC_RTC_SLOW_MEM_SUPPORTED

    {IDF_TARGET_NAME} 仅支持 RTC 快速内存，唤醒存根代码及其使用的数据应加载到 RTC 快速内存中。

接下来介绍如何在应用程序中实现唤醒存根代码。

唤醒存根的实现
--------------

调用函数 :cpp:func:`esp_wake_deep_sleep()` 可在 esp-idf 中实现唤醒存根。每当 SoC 从 Deep-sleep 中唤醒时，都会执行此函数。此函数与默认函数 :cpp:func:`esp_default_wake_deep_sleep()` 弱链接，因此如果应用程序包含名为 ``esp_wake_deep_sleep()`` 的函数，则会覆盖 esp-idf 中的默认函数。

请注意，如果只想调用 Deep-sleep 功能，则不必在应用程序中实现 :cpp:func:`esp_wake_deep_sleep()` 函数，只有当希望在 SoC 唤醒后立即做一些特殊行为时才需要用到该函数。

在开发自定义的唤醒存根时，首先应调用默认函数 :cpp:func:`esp_default_wake_deep_sleep()`。

此外，如果想在运行时切换不同的唤醒存根，可以调用函数 :cpp:func:`esp_set_deep_sleep_wake_stub()`。

参照以下步骤，可以在应用程序中实现唤醒存根函数：

.. list::

    - 将唤醒存根代码加载到 RTC 快速内存中
    :SOC_RTC_SLOW_MEM_SUPPORTED: - 将数据加载到 RTC 内存中
    :not SOC_RTC_SLOW_MEM_SUPPORTED: - 将数据加载到 RTC 快速内存中

将唤醒存根代码加载到 RTC 快速内存中
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

唤醒存根代码只能调用存放在 ROM 中或加载到 RTC 快速内存中的函数，其他所有 RAM 位置都未初始化，且包含随机数据。虽然唤醒存根代码可以使用其他 RAM 区域进行临时存储，但这些区域的内容在回到 Deep-sleep 模式或启动 esp-idf 时将被覆盖。

唤醒存根代码是主 esp-idf 应用程序的一部分。在 esp-idf 正常运行期间，函数可以像常规程序一样调用唤醒存根代码或访问 RTC 内存。

唤醒存根代码必须驻留在 RTC 快速内存中，这可以通过两种方式实现。

- 使用 ``RTC_IRAM_ATTR`` 属性将 :cpp:func:`esp_wake_deep_sleep()` 函数放到 RTC 快速内存中：

.. code:: c

    void RTC_IRAM_ATTR esp_wake_deep_sleep(void) {
        esp_default_wake_deep_sleep();
        // Add additional functionality here
    }

第一种方法适用于简短的代码段或包含“常规”代码和 "RTC" 代码的源文件。

- 将函数 :cpp:func:`esp_wake_deep_sleep()` 放到任何名字以 ``rtc_wake_stub`` 开头的源文件中。以 ``rtc_wake_stub*`` 为名的文件中的内容会由链接器自动放入 RTC 快速内存中。

在 RTC 快速内存中编写较长的代码段时，建议使用第二种方法。

.. only:: SOC_RTC_SLOW_MEM_SUPPORTED

    将唤醒存根数据加载到 RTC 内存中
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    RTC 内存必须包含唤醒存根代码使用的只读数据。除非是从 Deep-sleep 中唤醒，其他所有 SoC 重新启动时，RTC 内存中的数据会被初始化。从 Deep-sleep 中唤醒时，将保留进入睡眠前存在的数据。唤醒存根代码使用的数据必须驻留在 RTC 内存（RTC 快速内存或 RTC 慢速内存）中。

    有两种方法可以指定此数据：

    - 使用 ``RTC_DATA_ATTR`` 和 ``RTC_RODATA_ATTR`` 属性分别指定可写和只读数据。

    .. code:: c

        RTC_DATA_ATTR int wake_count;

        void RTC_IRAM_ATTR esp_wake_deep_sleep(void) {
            esp_default_wake_deep_sleep();
            static RTC_RODATA_ATTR const char fmt_str[] = "Wake count %d\n";
            esp_rom_printf(fmt_str, wake_count++);
        }

    这些数据被存放在 RTC 内存区域中，可以通过名为 :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_RTCDATA_IN_FAST_MEM` 的 menuconfig 选项进行配置，且此选项允许为 ULP 程序保留慢速内存区域。在默认选项中，这些数据被放入 RTC 慢速内存中，一旦启用上述选项，标记有 ``RTC_DATA_ATTR`` 和 ``RTC_RODATA_ATTR`` 的数据将被放入 RTC 快速内存中。此选项依赖于 :ref:`CONFIG_FREERTOS_UNICORE` 选项，因为 RTC 快速内存只能由 PRO_CPU 访问。

    .. only:: esp32

        此选项依赖于 :ref:`CONFIG_FREERTOS_UNICORE` 选项，因为只有 PRO_CPU 才能访问 RTC 快速内存。

        ``RTC_FAST_ATTR`` 和 ``RTC_SLOW_ATTR`` 属性可分别用于指定被强制放入 RTC 快速内存和 RTC 慢速内存中的数据。对标记为 ``RTC_FAST_ATTR`` 的数据的任何访问都仅由 PRO_CPU 允许。

    .. only:: esp32s2 or esp32s3

        ``RTC_FAST_ATTR`` 和 ``RTC_SLOW_ATTR`` 属性分别可用于指定被强制放入 RTC 快速内存和 RTC 慢速内存中的数据。


.. only:: not SOC_RTC_SLOW_MEM_SUPPORTED

    将唤醒存根数据加载到 RTC 快速内存中
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    唤醒存根代码使用的数据必须驻留在 RTC 快速内存中。

    有两种方法可以指定此数据：

    - 使用 ``RTC_DATA_ATTR`` 和 ``RTC_RODATA_ATTR`` 属性分别指定可写和只读数据。

    .. code:: c

        RTC_DATA_ATTR int wake_count;

        void RTC_IRAM_ATTR esp_wake_deep_sleep(void) {
            esp_default_wake_deep_sleep();
            static RTC_RODATA_ATTR const char fmt_str[] = "Wake count %d\n";
            esp_rom_printf(fmt_str, wake_count++);
        }

    ``RTC_FAST_ATTR`` 和 ``RTC_SLOW_ATTR`` 属性可分别用于指定将被强制放入 RTC 快速内存和 RTC 慢速内存中的数据。但 {IDF_TARGET_NAME} 仅支持 RTC 快速内存，因此上述两个属性都将映射到 RTC 快速内存中。

然而，以这种方式使用的任何字符串常量都必须被声明为数组，且使用 ``RTC_RODATA_ATTR`` 进行标记，如上文例子所示。

- 将数据放到任何名字以 ``rtc_wake_stub`` 开头的源文件中，如示例源文件 :example_file:`system/deep_sleep_wake_stub/main/rtc_wake_stub_example.c`。

.. code:: c

    if (s_count >= s_max_count) {
        // Reset s_count
        s_count = 0;

        // Set the default wake stub.
        // There is a default version of this function provided in esp-idf.
        esp_default_wake_deep_sleep();

        // Return from the wake stub function to continue
        // booting the firmware.
        return;
    }

在包含字符串或更复杂的代码段时，建议使用第二种方法。

启用 Kconfig 选项 :ref:`CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` 可以减少唤醒时间。更多信息请参阅 :doc:`从 Deep-sleep 模式快速启动 <bootloader>`。

上述所有函数在 :component_file:`esp_hw_support/include/esp_sleep.h` 中声明。

应用示例
---------------

.. only:: SOC_RTC_FAST_MEM_SUPPORTED

    - :example:`system/deep_sleep_wake_stub` 演示如何使用 {IDF_TARGET_NAME} 上的深度睡眠唤醒存根，以便在唤醒后立即执行一些任务（唤醒存根代码），然后再返回睡眠状态。
