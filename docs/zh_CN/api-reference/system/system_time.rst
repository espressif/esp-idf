系统时间
=========
:link_to_translation:`en:[English]`

{IDF_TARGET_RTC_CLK_FRE:default="未更新", esp32="150 kHz", esp32s2="90 kHz", esp32s3="136 kHz", esp32c3="136 kHz", esp32c2="136 kHz"}
{IDF_TARGET_INT_OSC_FRE:default="未更新", esp32="8.5 MHz", esp32s2="8.5 MHz", esp32s3="17.5 MHz", esp32c3="17.5 MHz", esp32c2="17.5 MHz"}
{IDF_TARGET_INT_OSC_FRE_DIVIDED:default="未更新", esp32="~33 kHz", esp32s2="~33 kHz", esp32s3="~68 kHz", esp32c3="~68 kHz", esp32c2="~68 kHz"}
{IDF_TARGET_EXT_CRYSTAL_PIN:default="未更新", esp32="32K_XP 和 32K_XN", esp32s2="XTAL_32K_P 和 XTAL_32K_N", esp32s3="XTAL_32K_P 和 XTAL_32K_N", esp32c3="XTAL_32K_P 和 XTAL_32K_N"}
{IDF_TARGET_EXT_OSC_PIN:default="未更新", esp32="32K_XN", esp32s2="XTAL_32K_P", esp32s3="XTAL_32K_P", esp32c3="XTAL_32K_P", esp32c2="GPIO0"}
{IDF_TARGET_HARDWARE_DESIGN_URL:default="未更新", esp32="`ESP32 硬件设计指南 <https://www.espressif.com/sites/default/files/documentation/esp32_hardware_design_guidelines_cn.pdf#page=10>`_", esp32s2="`ESP32-S2 硬件设计指南 <https://www.espressif.com/sites/default/files/documentation/esp32-s2_hardware_design_guidelines_cn.pdf#page=10>`_", esp32s3="`ESP32-S3 硬件设计指南 <https://www.espressif.com/sites/default/files/documentation/esp32-s3_hardware_design_guidelines_cn.pdf#page=12>`_", esp32c3="`ESP32-C3 硬件设计指南 <https://www.espressif.com/sites/default/files/documentation/esp32-c3_hardware_design_guidelines_cn.pdf#page=9>`_"}


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

默认时钟源的时间精度最高，建议使用该配置。此外，用户也可以通过配置选项 :ref:`CONFIG_NEWLIB_TIME_SYSCALL` 来选择其他时钟源。


.. _rtc-clock-source-choice:

RTC 定时器时钟源
-----------------

RTC 定时器有以下时钟源：

.. list::

    - ``内置 {IDF_TARGET_RTC_CLK_FRE} RC 振荡器`` （默认）：Deep-sleep 模式下电流消耗最低，不依赖任何外部元件。但由于温度波动会影响该时钟源的频率稳定性，在 Deep-sleep 和 Light-sleep 模式下都有可能发生时间偏移。

    :not esp32c2: - ``外置 32 kHz 晶振``：需要将一个 32 kHz 晶振连接到 {IDF_TARGET_EXT_CRYSTAL_PIN} 管脚。频率稳定性更高，但在 Deep-sleep 模式下电流消耗略高（比默认模式高 1 μA）。
    
    - ``管脚 {IDF_TARGET_EXT_OSC_PIN} 外置 32 kHz 振荡器``：允许使用由外部电路产生的 32 kHz 时钟。外部时钟信号必须连接到管脚 {IDF_TARGET_EXT_OSC_PIN}。正弦波信号的振幅应小于 1.2 V，方波信号的振幅应小于 1 V。正常模式下，电压范围应为 0.1 < Vcm < 0.5 xVamp，其中 Vamp 代表信号振幅。使用此时钟源时，管脚 {IDF_TARGET_EXT_OSC_PIN} 无法用作 GPIO 管脚。
    
    - ``内置 {IDF_TARGET_INT_OSC_FRE} 振荡器的 256 分频时钟 ({IDF_TARGET_INT_OSC_FRE_DIVIDED})``：频率稳定性优于 ``内置 {IDF_TARGET_RTC_CLK_FRE} RC 振荡器``，同样无需外部元件，但 Deep-sleep 模式下电流消耗更高（比默认模式高 5 μA）。

时钟源的选择取决于系统时间精度要求和睡眠模式下的功耗要求。要修改 RTC 时钟源，请在项目配置中设置 :ref:`CONFIG_RTC_CLK_SRC`。

.. Need to add esp32c2 hardware design guideline link after it is publsihed.

.. only:: not esp32c2

    想要了解外置晶振或外置振荡器的更多布线要求，请参考 {IDF_TARGET_HARDWARE_DESIGN_URL}。


获取当前时间
--------------

要获取当前时间，请使用 POSIX 函数 ``gettimeofday()``。此外，您也可以使用以下标准 C 库函数来获取时间并对其进行操作：

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

在 lwIP SNTP 库内部调用的函数依赖于系统时间的同步模式。可使用函数 :cpp:func:`sntp_set_sync_mode` 来设置下列同步模式之一。

- :cpp:enumerator:`SNTP_SYNC_MODE_IMMED` （默认）：使用函数 ``settimeofday()`` 后，收到 SNTP 服务器响应时立即更新系统时间。
- :cpp:enumerator:`SNTP_SYNC_MODE_SMOOTH`：使用函数 ``adjtime()`` 后，通过逐渐减小时间误差，平滑地更新时间。如果 SNTP 响应报文中的时间与当前系统时间相差大于 35 分钟，则会通过 ``settimeofday()`` 立即更新系统时间。

lwIP SNTP 库提供了 API 函数，用于设置某个事件的回调函数。您可能需要使用以下函数：

- :cpp:func:`sntp_set_time_sync_notification_cb()`：用于设置回调函数，通知时间同步的过程。
- :cpp:func:`sntp_get_sync_status()` 和 :cpp:func:`sntp_set_sync_status()`：用于获取或设置时间同步状态。

通过 SNTP 开始时间同步，只需调用以下三个函数：

.. code-block:: c

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

添加此初始化代码后，应用程序将定期同步时间。时间同步周期由 :ref:`CONFIG_LWIP_SNTP_UPDATE_DELAY` 设置（默认为一小时）。如需修改，请在项目配置中设置 :ref:`CONFIG_LWIP_SNTP_UPDATE_DELAY`。

如需查看示例代码，请前往 :example:`protocols/sntp` 目录。该目录下的示例展示了如何基于 lwIP SNTP 库实现时间同步。


时区
------

要设置本地时区，请使用以下 POSIX 函数：

1. 调用 ``setenv()``，将 ``TZ`` 环境变量根据设备位置设置为正确的值。时间字符串的格式与 `GNU libc 文档 <https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html>`_ 中描述的相同（但实现方式不同）。
2. 调用 ``tzset()``，为新的时区更新 C 库的运行数据。

完成上述步骤后，请调用标准 C 库函数 ``localtime()``。该函数将返回排除时区偏差和夏令时干扰后的准确本地时间。


64 位 ``time_t``
-----------------

ESP-IDF 默认使用 32 位的 ``time_t`` 类型。为解决 Y2K38 漏洞，您在构建应用程序时可能需要使用 64 位的 ``time_t`` 类型。

目前，完成这一操作需要从头开始构建交叉编译器工具链，具体步骤请参阅 :doc:`/get-started/linux-macos-setup`。要在工具链中启用对 64 位 ``time_t`` 的支持，您需要在构建工具链之前从 ``crosstool-NG/samples/xtensa-esp32-elf/crosstool.config`` 文件中删除 ``--enable-newlib-long-time_t`` 选项。

如需使程序同时兼容 32 位和 64 位的 ``time_t``，可以使用以下方法：

- 在 C 或 C++ 源文件中，如果 ``time_t`` 是 32 位的，编译器会预定义 ``_USE_LONG_TIME_T`` 宏，该宏定义在 ``<sys/types.h>`` 中。
- 在 CMake 文件中，ESP-IDF 构建属性 ``TIME_T_SIZE`` 将被设置为 ``time_t`` 的大小，单位为字节。您可以调用 ``idf_build_get_property(var TIME_T_SIZE)`` 来获取该属性的值，并将其放入 CMake 变量 ``var`` 中。了解更多关于 ``idf_build_get_property`` 的信息，参见 :ref:`ESP-IDF CMake 构建系统 API <cmake_buildsystem_api>`。

注意， ``time_t`` 类型的大小也会影响其他类型的大小，例如 ``struct timeval``、 ``struct stat`` 和 ``struct utimbuf``。


API 参考
-------------

.. include-build-file:: inc/esp_sntp.inc
