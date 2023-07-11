* PSRAM 中的栈：对于不以任何直接或间接方式调用 ROM 中代码的任务，选项 :ref:`CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY` 会取消 :cpp:func:`xTaskCreateStatic` 中的检查，允许任务栈存放在外部 RAM 中。但 **不建议** 使用此选项。
* 在以 80 MHz 时钟速度运行时，外部 RAM 还必须占用 HSPI 或 VSPI 总线。设置 :ref:`CONFIG_SPIRAM_OCCUPY_SPI_HOST` 选项可以选择使用的 SPI 主机。


芯片版本
==============

某些 ESP32 版本存在与外部 RAM 使用相关的问题，这些问题记录在 `ESP32 系列芯⽚勘误表`_ 文档中。ESP-IDF 会以下列特定方式处理上述错误：


ESP32 rev v0.0
--------------
ESP-IDF 并未针对该版芯片的错误提供解决方案，也不能将外部 PSRAM 映射到 ESP32 的主存储器映射中。


ESP32 rev v1.0
--------------
某些机器指令序列对外部内存进行操作时，该版芯片的错误会引发问题，详情请参阅 `ESP32 系列芯⽚勘误表`_ 第 3.2 节。为此，ESP32 GCC 编译器增加了标志 ``-mfix-esp32-psram-cache-issue``，用于过滤这些序列，只输出可以安全执行的代码。请启用 :ref:`CONFIG_SPIRAM_CACHE_WORKAROUND` 选项以使用此方法。

启用此选项后，ESP-IDF 会链接到重新编译且带有额外标志的 Newlib，此外还会执行以下操作：

- 避免使用某些 ROM 函数
- 为 Wi-Fi 栈分配静态内存

ESP32 rev v3.0
--------------

ESP32 rev v3.0 修复了 rev v1.0 中发现的 PSRAM 缓存问题。当 :ref:`CONFIG_ESP32_REV_MIN` 选项设置为 ``rev v3.0`` 时，会禁用与 PSRAM 相关的编译器解决方案。有关 ESP32 v3.0 的更多详情，请参阅 `ESP32 芯⽚版本 v3.0 使⽤指南`_。
