系统时间
=========

:link_to_translation:`en:[English]`

概述
------

{IDF_TARGET_NAME} 使用两种硬件时钟源建立和保持系统时间。根据应用目的及对系统时间的精度要求，既可以仅使用其中一种时钟源，也可以同时使用两种时钟源。这两种硬件时钟源为：

- **RTC 定时器**：RTC 定时器在任何睡眠模式下及在任何复位后均可保持系统时间（上电复位除外，因为上电复位会重置 RTC 定时器）。时钟频率偏差取决于 `RTC 定时器时钟源`_，该偏差只会在睡眠模式下影响时间精度。睡眠模式下，时间分辨率为 6.667 μs。

- **高分辨率定时器**：高分辨率定时器在睡眠模式下及在复位后不可用，但其时间精度更高。该定时器使用 APB_CLK 时钟源（通常为 80 MHz），时钟频率偏差小于 ±10 ppm，时间分辨率为 1 μs。

可供选择的硬件时钟源组合如下所示：

- RTC 和高分辨率定时器（默认）
- RTC
- 高分辨率定时器
- 无

默认时钟源的时间精度最高，建议使用该配置。此外，你可以通过配置选项 :ref:`CONFIG_LIBC_TIME_SYSCALL` 来选择其他时钟源。


.. _rtc-clock-source-choice:

RTC 定时器时钟源
-----------------

RTC 定时器有以下时钟源：

.. list::

    - ``内置 90～150 kHz（频率取决于芯片型号）RC 振荡器`` （默认）：Deep-sleep 模式下电流消耗最低，不依赖任何外部元件。但由于温度波动会影响该时钟源的频率稳定性，在 Deep-sleep 和 Light-sleep 模式下都有可能发生时间偏移。

    :not esp32c2: - ``外部 32 kHz 无源晶振``：需要将一个 32 kHz 晶振连接到外部晶振管脚。频率稳定性更高，但在 Deep-sleep 模式下电流消耗略高（比默认模式高 1 μA）。管脚连接的相关信息请参考 `技术规格书 <{IDF_TARGET_DATASHEET_CN_URL}>`__。

    - ``外部 32 kHz 有源晶振``：允许使用由外部电路产生的 32 kHz 时钟。外部时钟信号必须连接到外部振荡器管脚。输入振幅必须保证数字电路能够正确识别电平高低。测试结果表明，1.7 V 为高低电平的分界点，因此需确保输入波形的峰值大于 1.7 V，谷值小于 1.7 V。管脚连接的相关信息请参考 `技术规格书 <{IDF_TARGET_DATASHEET_CN_URL}>`__。

    :esp32 or esp32s2 or esp32s3 or esp32c2 or esp32c3: - ``内置 8.5～17.5 MHz 振荡器（频率取决于芯片型号）的 256 分频时钟``：频率稳定性优于 ``内置 90～150 kHz RC 振荡器``，同样无需外部元件，但 Deep-sleep 模式下电流消耗更高（比默认模式高 5 μA）。

时钟源的选择取决于系统时间精度要求和睡眠模式下的功耗要求。要修改 RTC 时钟源，请在项目配置中设置 :ref:`CONFIG_RTC_CLK_SRC`。

想要了解外部无源晶振和有源晶振的更多布线要求，请参考 `硬件设计指南 <https://docs.espressif.com/projects/esp-hardware-design-guidelines/zh_CN/latest/{IDF_TARGET_PATH_NAME}>`_。

选择 RTC 定时器时钟源
^^^^^^^^^^^^^^^^^^^^^

在以下情况下使用默认的 **内部 RC 振荡器**：

- 对于电池供电设备或能量受限的设备，最小化睡眠电流消耗。
- 睡眠模式下不需要高时间精度，并且可以接受每天几秒的时间漂移。
- 优先考虑最小化外部元件和硬件复杂度。
- 工作温度相对稳定（减少频率变化）。
- 未使用低功耗蓝牙的节能模式，或仅用于传统广播/扫描而不建立连接。

在以下情况下使用 **外部 32 kHz 有源晶振** 或 **外部 32 kHz 无源晶振**：

- 需要在 Deep-sleep 和 Light-sleep 模式下保持准确的时间。
- 睡眠电流略有增加（通常为几微安）仍在功耗预算范围内。
- 设计可以接受添加外部元件。
- 工作温度可能变化较大。
- 启用低功耗蓝牙的节能模式，且需要建立连接。

某些芯片可能提供 **内部振荡器** 选项，这是内部 RC 振荡器和外部 32 kHz 有源晶振/无源晶振选项之间的折中方案。它不需要外部元件，同时提供比内部 RC 振荡器更好的频率稳定性，但代价是睡眠电流消耗更高。如果使用低功耗蓝牙，请确认内部振荡器是否满足低功耗蓝牙睡眠时钟精度要求（见下文）。

在大多数设计中，睡眠电流的略微增加是换取 RTC 定时器频率稳定性显著提升和睡眠期间时间漂移减少的合理权衡。

**低功耗蓝牙** 要求睡眠时钟精度在 500 PPM 以内。RTC 时钟源可能无法满足此要求，导致连接建立失败、意外超时或与非乐鑫设备不兼容。

.. only:: SOC_BLE_SUPPORTED

    有关低功耗蓝牙时钟配置和功耗的详细比较，请参考 :doc:`低功耗蓝牙的低功耗模式 </api-guides/low-power-mode/low-power-mode-ble>`。


获取当前时间
--------------

要获取当前时间，请使用 POSIX 函数 ``gettimeofday()``。此外，也可以使用以下标准 C 库函数来获取时间并对其进行操作：

.. code-block:: bash

    gettimeofday
    time
    asctime
    clock
    ctime
    difftime
    gmtime
    localtime
    mktime
    strftime
    adjtime*

如需立即更新当前时间，并暂停平滑时间校正，请使用 POSIX 函数 ``settimeofday()``。

若要求时间的分辨率为 1 s，请使用以下代码片段：

.. code-block:: c

    time_t now;
    char strftime_buf[64];
    struct tm timeinfo;

    time(&now);
    // 将时区设置为中国标准时间
    setenv("TZ", "CST-8", 1);
    tzset();

    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time in Shanghai is: %s", strftime_buf);

若要求时间的分辨率为 1 μs，请使用以下代码片段：

.. code-block:: c

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;

.. _system-time-sntp-sync:

SNTP 时间同步
---------------

要设置当前时间，可以使用 POSIX 函数 ``settimeofday()`` 和 ``adjtime()``。lwIP 中的 SNTP 库会在收到 NTP 服务器的响应报文后，调用这两个函数以更新当前的系统时间。当然，用户可以在 lwIP SNTP 库之外独立地使用这两个函数。

包括 SNTP 函数在内的一些 lwIP API 并非线程安全，因此建议在与 SNTP 模块交互时使用 :doc:`esp_netif component <../network/esp_netif>`。

要初始化特定的 SNTP 服务器并启动 SNTP 服务，只需创建有特定服务器名称的默认 SNTP 服务器配置，然后调用 :cpp:func:`esp_netif_sntp_init()` 注册该服务器并启动 SNTP 服务。

.. code-block:: c

    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);

一旦收到 SNTP 服务器的响应，此代码会自动执行时间同步。有时等待时间同步很有意义，调用 :cpp:func:`esp_netif_sntp_sync_wait()` 可实现此目的。应用也可以订阅时间同步完成时发布的事件（事件类型 ``NETIF_SNTP_EVENT``，事件 ID ``NETIF_SNTP_TIME_SYNC``）。事件数据是指向 :cpp:type:`esp_netif_sntp_time_sync_t` 的指针，其中包含已同步的 ``timeval``：

.. code-block:: c

    if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000)) != ESP_OK) {
        printf("Failed to update system time within 10s timeout");
    }

要配置多个 NTP 服务器（或使用更高级的设置，例如 DHCP 提供的 NTP 服务器），请参考 :doc:`esp_netif <../network/esp_netif>` 文档 :ref:`esp_netif-sntp-api` 中的详细说明。

lwIP SNTP 库可在下列任一同步模式下工作：

- :cpp:enumerator:`SNTP_SYNC_MODE_IMMED` （默认）：使用 ``settimeofday()``，收到 SNTP 服务器响应后立即更新系统时间。
- :cpp:enumerator:`SNTP_SYNC_MODE_SMOOTH`：使用函数 ``adjtime()`` 逐渐减少时间误差以平滑更新时间。如果 SNTP 响应时间和系统时间之差超过 35 分钟，请立即使用 ``settimeofday()`` 更新系统时间。

如要选择 :cpp:enumerator:`SNTP_SYNC_MODE_SMOOTH` 模式，请将 SNTP 配置结构体中的 :cpp:member:`esp_sntp_config::smooth` 设置为 ``true``，否则将默认使用 :cpp:enumerator:`SNTP_SYNC_MODE_IMMED` 模式。

设置时间同步时的回调函数，请使用配置结构体中的 :cpp:member:`esp_sntp_config::sync_cb` 字段。

添加此初始化代码后，应用程序将定期同步时间。时间同步周期由 :ref:`CONFIG_LWIP_SNTP_UPDATE_DELAY` 设置（默认为一小时）。如需修改，请在项目配置中设置 :ref:`CONFIG_LWIP_SNTP_UPDATE_DELAY`。

如需查看示例代码，请前往 :example:`protocols/sntp` 目录。该目录下的示例展示了如何基于 lwIP SNTP 库实现时间同步。

也可以直接使用 lwIP API，但请务必注意线程安全。线程安全的 API 如下：

- :cpp:func:`sntp_set_time_sync_notification_cb` 用于设置通知时间同步过程的回调函数。
- :cpp:func:`sntp_get_sync_status` 和 :cpp:func:`sntp_set_sync_status` 用于获取/设置时间同步状态。
- :cpp:func:`sntp_set_sync_mode` 用于设置同步模式。
- :cpp:func:`esp_sntp_setoperatingmode` 用于设置首选操作模式。:cpp:enumerator:`ESP_SNTP_OPMODE_POLL` 和 :cpp:func:`esp_sntp_init` 可初始化 SNTP 模块。
- :cpp:func:`esp_sntp_setservername` 用于配置特定 SNTP 服务器。


时区
------

要设置本地时区，请使用以下 POSIX 函数：

1. 调用 ``setenv()``，将 ``TZ`` 环境变量根据设备位置设置为正确的值。时间字符串的格式与 `GNU libc 文档 <https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html>`_ 中描述的相同（但实现方式不同）。
2. 调用 ``tzset()``，为新的时区更新 C 库的运行数据。

完成上述步骤后，请调用标准 C 库函数 ``localtime()``。该函数将返回排除时区偏差和夏令时干扰后的准确本地时间。


2036 年和 2038 年溢出问题
--------------------------------

SNTP/NTP 2036 年溢出问题
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

SNTP/NTP 时间戳为 64 位无符号定点数，其中前 32 位表示整数部分，后 32 位表示小数部分。该 64 位无符号定点数代表从 1900 年 1 月 1 日 00:00 起经过的秒数，因此 SNTP/NTP 时间将在 2036 年溢出。

为了解决这一问题，可以使用整数部分的 MSB（惯例为位 0）来表示 1968 年到 2104 年之间的时间范围（查看 `RFC2030 <https://www.rfc-editor.org/rfc/rfc2030>`_ 了解更多信息），这一惯例将使得 SNTP/NTP 时间戳的生命周期延长。该惯例会在 lwIP 库的 SNTP 模块中实现，因此 ESP-IDF 中 SNTP 相关功能在 2104 年之前能够经受住时间的考验。


Unix 时间 2038 年溢出问题
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Unix 时间（类型 ``time_t``）此前为有符号的 32 位整数，因此将于 2038 年溢出（即 `Y2K38 问题 <https://zh.wikipedia.org/wiki/2038%E5%B9%B4%E9%97%AE%E9%A2%98>`_）。为了解决 Y2K38 问题，ESP-IDF 从 v5.0 版本起开始使用有符号的 64 位整数来表示 ``time_t``，从而将 ``time_t`` 溢出推迟 2920 亿年。


API 参考
-------------

.. include-build-file:: inc/esp_sntp.inc
