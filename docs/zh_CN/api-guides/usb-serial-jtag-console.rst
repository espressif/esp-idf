**********************************
USB 串行/JTAG 控制器控制台
**********************************

:link_to_translation:`en:[English]`

ESP 芯片通常使用 UART 实现串口，并可以通过外部 USB-UART 桥接芯片，连接到主机/PC 上的串口控制台仿真器。然而，在具有 USB 串行/JTAG 控制器的 ESP 芯片上，控制器的 CDC-ACM 部分即可实现串口，该串口直接连接到主机/PC，不需要外部 USB-UART 桥接芯片。

拥有 USB 串行/JTAG 控制器时，{IDF_TARGET_NAME} 支持以下功能：

* 双向串行控制台，可与 :doc:`IDF 监视器 <tools/idf-monitor>` 或其他串行监视器一起使用。
* 使用 ``esptool.py`` 和 ``idf.py flash`` 烧录。
* 使用 OpenOCD 等工具进行 JTAG 调试，同时进行串行操作。

.. note::

  USB 串行/JTAG 控制器是一个 USB 设备，其功能固定，完全由硬件实现。也就是说，即便重新配置，它也只能执行串口和 JTAG 调试功能。这不同于某些 ESP 芯片中的 USB OTG 控制器，后者经配置，可以执行多种 USB 设备功能。

硬件要求
=====================

{IDF_TARGET_USB_DP_GPIO:default="未更新！",esp32c3="19",esp32s3="20", esp32c6="13", esp32h2="27", esp32p4="25/27", esp32c5="14", esp32c61="13"}
{IDF_TARGET_USB_DM_GPIO:default="未更新！",esp32c3="18",esp32s3="19", esp32c6="12", esp32h2="26", esp32p4="24/26", esp32c5="13", esp32c61="12"}

将 {IDF_TARGET_NAME} 连接到 USB 端口，连接方式如下：

.. list-table::
    :header-rows: 1
    :widths: 40 60
    :align: center

    * - GPIO
      - USB
    * - {IDF_TARGET_USB_DP_GPIO}
      - D+（绿）
    * - {IDF_TARGET_USB_DM_GPIO}
      - D-（白）
    * - GND
      - GND（黑）
    * - 5V（或由外部提供）
      - +5V（红）

部分开发板可能会为 USB 串行/JTAG 控制器提供 USB 连接器，此时无需额外连接。

软件配置
======================

在 :ref:`CONFIG_ESP_CONSOLE_UART` 选项中选择 ``CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG``，可以将 USB 串行/JTAG 控制器用作串口。选择该选项后，即可正常构建和烧录项目。

另外，也可以通过 ``usb_serial_jtag`` 端口访问输出，但请确保在 :ref:`CONFIG_ESP_CONSOLE_SECONDARY` 选项中选择了 ``CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG``。

.. warning::

    除输出外，如果还想用控制台输入或使用交互式解释器 (REPL)，请选择 ``CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG`` 选项。

上传应用程序
=========================

USB 串行/JTAG 控制器可以自动将 {IDF_TARGET_NAME} 置于下载模式。用户只需正常烧录芯片，但需要在系统上指定 USB 串行/JTAG 控制器端口：``idf.py flash -p PORT``，``PORT`` 是正确的端口名称。

.. note::

    USB 串行/JTAG 控制器的串行端口通常：

    - 在 Linux 系统上显示为 ``/dev/ttyACM*``
    - 在 MAC 系统上显示为 ``/dev/cu*``
    - 在 Windows 设备管理器上显示为 ``COM*`` 端口

限制
===========

{IDF_TARGET_BOOT_PIN:default = "未更新！", esp32c3 = "GPIO9", esp32s3 = "GPIO0", esp32c6 = "GPIO9"}

USB 串行/JTAG 控制台功能存在一些限制，这些限制的影响程度各不相同，具体取决于开发中的应用程序类型，以及开发的工作流程。

{IDF_TARGET_BOOT_PIN:default = "未更新！", esp32c3 = "GPIO9", esp32s3 = "GPIO0", esp32c6 = "GPIO9"}

重新配置 USB 管脚
-----------------------

如果在意外情况下，应用程序重新配置了 USB 外设管脚，或禁用了 USB 串行/JTAG 控制器，设备会从系统消失。应用程序中的问题修复后，需要将 {IDF_TARGET_BOOT_PIN} 拉低，并重置芯片，手动将 {IDF_TARGET_NAME} 置于下载模式。

如果应用程序进入 Deep-sleep 模式，USB 串行/JTAG 设备会从系统中消失。

数据缓冲
--------------

对于从 {IDF_TARGET_NAME} 发送到 PC 终端的数据（例如 stdout、日志），{IDF_TARGET_NAME} 会先写入一个小型内部缓冲区。如果该缓冲区达到最大容量（例如没有连接 PC 终端），{IDF_TARGET_NAME} 会进行一次等待，时长约 50 ms。在此期间，PC 终端可以请求数据，但可能会导致应用程序出现非常短暂的暂停。

对于从 PC 终端发送到 {IDF_TARGET_NAME} 的数据（如控制台命令），许多 PC 终端会等待 {IDF_TARGET_NAME} 接收字节后，再允许发送更多数据。这与使用 USB 转串口 (URAT) 桥接芯片不同，URAT 桥接芯片始终接收字节，并将其发送到（可能未在监听）的 {IDF_TARGET_NAME}。

.. note::

    在极少数情况下，从 {IDF_TARGET_NAME} 发送到主机的数据可能会“阻塞”在主机内存中。继续发送数据即可“取消阻塞”，但如果应用程序未继续发送数据，则需要手动将这些数据刷新到主机。能否继续发送数据取决于驱动程序，默认配置的非阻塞驱动程序和 VFS 实现会在换行后自动刷新，而基于中断的阻塞驱动程序会在发送缓冲区变空时自动刷新。

睡眠模式考虑因素
-------------------------

USB 串行/JTAG 控制器及其关联的 USB PHY 均由特定时钟控制（如 APB 和 USB PHY 时钟），并属于特定的电源域（如数字电源域）。因此，对 USB 串行/JTAG 控制器关联的时钟和电源域进行任何更改，如进入另一睡眠模式，都可能影响控制器操作。

Deep-sleep
^^^^^^^^^^

进入 Deep-sleep 模式时，USB 串行/JTAG 控制器及 USB PHY 关闭，导致 USB PHY 的 D+ 线不再变成高电平。因此：

- 进入 Deep-sleep 模式时，USB 串行/JTAG 设备会对主机/PC 显示为断开连接。即使 USB 电缆在物理层面上处于连接状态，仍会显示为断开连接。
- 退出 Deep-sleep 模式时，USB 串行/JTAG 设备会重新连接到主机/PC。

Light-sleep
^^^^^^^^^^^

.. only:: not SOC_USB_SERIAL_JTAG_SUPPORT_LIGHT_SLEEP

进入 Light-sleep 模式时，APB 和 USB PHY 时钟关闭。因此，USB 串行/JTAG 控制器将无法接收或响应任何来自相连主机的 USB 事务，包括周期性的 CDC Data IN 事务。因此：

- 进入 Light-sleep 模式时，USB 串行/JTAG 设备无法响应主机/PC 的 USB CDC 驱动程序。主机/PC 可能会报告 USB 串行/JTAG 设备已断开连接，或连接出错。即使 USB 电缆在物理层面上处于连接状态，仍会显示为断开连接。
- 退出 Light-sleep 模式时，由于 USB PHY 的 D+ 线在 Light-sleep 期间始终处于高电平，可能导致主机/PC 不会重新枚举（即重新连接）USB 串行/JTAG 设备。用户可能需要在物理层面上断开 USB 电缆，并重新连接。

自动与手动进入睡眠模式
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

如果通过 :cpp:func:`esp_light_sleep_start` 或 :cpp:func:`esp_deep_sleep_start` 手动进入睡眠模式，需注意，USB 串行/JTAG 控制器在睡眠期间不工作。ESP-IDF **没有添加任何拒绝进入睡眠的安全检查**，即使 USB 串行/JTAG 控制器已连接，也会进入睡眠模式。如果在连接了 USB 串行/JTAG 控制器的情况下进入睡眠，可以通过拔出并重新插入 USB 电缆来重新建立连接。

如果通过 :cpp:func:`esp_pm_configure` 自动进入睡眠模式，请启用 :ref:`CONFIG_USJ_NO_AUTO_LS_ON_CONNECTION` 选项，该选项支持 {IDF_TARGET_NAME} 自动检测 USB 串行/JTAG 控制器与主机的连接情况，并在连接持续时，阻止程序自动进入睡眠模式。注意，此选项会增加功耗。
