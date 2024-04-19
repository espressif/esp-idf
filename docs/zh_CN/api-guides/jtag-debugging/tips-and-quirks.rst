注意事项和补充内容
------------------

:link_to_translation:`en:[English]`

本节提供了本指南中各部分提到的一些注意事项和补充内容。

.. _jtag-debugging-tip-breakpoints:

可用的断点和观察点
^^^^^^^^^^^^^^^^^^

{IDF_TARGET_NAME} 调试器支持 {IDF_TARGET_SOC_CPU_BREAKPOINTS_NUM} 个硬件断点和 64 个软件断点。硬件断点是由 {IDF_TARGET_NAME} 芯片内部的逻辑电路实现的，能够设置在代码的任何位置：flash 或者 IRAM 的代码区域。除此以外，OpenOCD 实现了两种软件断点：flash 断点（最多 32 个）和 IRAM 断点（最多 32 个）。目前 GDB 无法在 flash 中设置软件断点，因此除非解决此限制，否则这些断点只能由 OpenOCD 模拟为硬件断点（详细信息可以参阅 :ref:`下文 <jtag-debugging-tip-where-breakpoints>`）。{IDF_TARGET_NAME} 还支持 {IDF_TARGET_SOC_CPU_WATCHPOINTS_NUM} 个观察点，所以可以观察 {IDF_TARGET_SOC_CPU_WATCHPOINTS_NUM} 个变量的变化或者通过 GDB 命令 ``watch myVariable`` 来读取变量的值。请注意 menuconfig 中的 :ref:`CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK` 选项会使用最后一个观察点，如果你想在 OpenOCD 或者 GDB 中再次尝试使用这个观察点，可能不会得到预期的结果。详情请查看 menuconfig 中的帮助文档。


.. _jtag-debugging-tip-where-breakpoints:

关于断点的补充知识
^^^^^^^^^^^^^^^^^^

使用软件 flash 模拟部分硬件断点的意思就是当使用 GDB 命令 ``hb myFunction`` 给某个函数设置硬件断点时，如果该函数位于 flash 中，并且此时还有可用的硬件断点，那调试器就会使用硬件断点，否则就使用 32 个软件 flash 断点中的一个来模拟。这个规则同样适用于 ``b myFunction`` 之类的命令，在这种情况下，GDB 会自己决定该使用哪种类型的断点。如果 ``myFunction`` 位于可写区域 (IRAM)，那就会使用软件 IRAM 断点，否则就会像处理 ``hb`` 命令一样使用硬件断点或者软件 flash 断点。


.. _jtag-debugging-tip-flash-mappings:

flash 映射 vs 软件 flash 断点
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

为了在 flash 中设置或者清除软件断点，OpenOCD 需要知道它们在 flash 中的地址。为了完成从 {IDF_TARGET_NAME} 的地址空间到 flash 地址的转换，OpenOCD 使用 flash 中程序代码区域的映射。这些映射被保存在程序映像的头部，位于二进制数据（代码段和数据段）之前，并且特定于写入 flash 的每一个应用程序的映像。因此，为了支持软件 flash 断点，OpenOCD 需要知道待调试的应用程序映像在 flash 中的位置。默认情况下，OpenOCD 会在 0x8000 处读取分区表并使用第一个找到的应用程序映像的映射，但是也可能会存在无法工作的情况，比如分区表不在标准的 flash 位置，甚至可能有多个映像：一个出厂映像和两个 OTA 映像，你可能想要调试其中的任意一个。为了涵盖所有可能的调试情况，OpenOCD 支持特殊的命令，用于指定待调试的应用程序映像在 flash 中的具体位置。该命令具有以下格式：

``esp appimage_offset <offset>``

偏移量应为十六进制格式，如果要恢复默认行为，可以将偏移地址设置为 ``-1`` 。

.. note::

    由于 GDB 在连接 OpenOCD 时仅仅请求一次内存映射，所以可以在 TCL 配置文件中指定该命令，或者通过命令行传递给 OpenOCD。对于后者，命令行示例如下：

    .. highlight:: bash

    .. include:: {IDF_TARGET_PATH_NAME}.inc
        :start-after: run-openocd-appimage-offset
        :end-before: ---

    另外还可以通过 OpenOCD 的 telnet 会话执行该命令，然后再连接 GDB， 不过这种方式似乎没有那么便捷。

.. _jtag-debugging-tip-why-next-works-as-step:

“next” 命令无法跳过子程序的原因
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

当使用 ``next`` 命令单步执行代码时，GDB 会在子程序的前面设置一个断点，这样就可以跳过进入子程序内部的细节。如果 {IDF_TARGET_SOC_CPU_BREAKPOINTS_NUM} 个断点都已经设置好，那么 ``next`` 命令将不起作用。在这种情况下，请删掉其他断点以使其中一个变得可用。当所有断点都已经被使用时，``next`` 命令会像 ``step`` 命令一样工作，调试器就会进入子程序内部。


.. _jtag-debugging-tip-code-options:

OpenOCD 支持的编译时的选项
^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF 有一些针对 OpenOCD 调试功能的选项可以在编译时进行设置：

* :ref:`CONFIG_ESP_DEBUG_OCDAWARE` 默认会被使能。如果程序抛出了不可修复或者未处理的异常，并且此时已经连接上了 JTAG 调试器（即 OpenOCD 正在运行），那么 ESP-IDF 将会进入调试器工作模式。

* :ref:`CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK` 默认没有使能。在所有任务堆栈的末尾设置观察点，从 1 号开始索引。这是调试任务堆栈溢出的最准确的方式。

更多有关设置编译时的选项的信息，请参阅 :ref:`项目配置菜单 <get-started-configure>`。

.. _jtag-debugging-tip-freertos-support:

支持 FreeRTOS
^^^^^^^^^^^^^^^^

OpenOCD 完全支持 ESP-IDF 自带的 FreeRTOS 操作系统，GDB 会将 FreeRTOS 中的任务当做线程。使用 GDB 命令 ``i threads`` 可以查看所有的线程，使用命令 ``thread n`` 可以切换到某个具体任务的堆栈，其中 ``n`` 是线程的编号。检测 FreeRTOS 的功能可以在配置目标时被禁用。更多详细信息，请参阅 :ref:`jtag-debugging-tip-openocd-configure-target`。

GDB 具有 FreeRTOS 支持的 Python 扩展模块。在系统要求满足的情况下，通过 ``idf.py gdb`` 命令，ESP-IDF 会将该模块自动加载到 GDB 中。详细信息请参考 :ref:`jtag-debugging-examples-command-line-08`。

.. only:: esp32

    .. _jtag-debugging-tip-code-flash-voltage:

    在 OpenOCD 的配置文件中设置 SPI flash 的工作电压
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    ESP32 的 MTDI 管脚是用于 JTAG 通信的四个管脚之一，同时也是 ESP32 的 bootstrapping 管脚。上电时，ESP32 会在 MTDI 管脚上采样二进制电平，据此来设置内部的稳压器，用于给外部的 SPI flash 芯片供电。如果上电时 MTDI 管脚上的二进制电平为低电平，则稳压器会被设置为 3.3 V；如果 MTDI 管脚为高电平，则稳压器会被设置为 1.8 V。MTDI 管脚通常需要一个上拉电阻或者直接使能内部的弱下拉电阻（详见 `ESP32 系列芯片技术规格书 <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_cn.pdf>`_ ），具体取决于所使用的 SPI 芯片的类型。但是一旦连接上 JTAG 后，原来用于实现 bootstrapping 功能的上拉或者下拉电阻都会被覆盖掉。

    为了解决这个问题，OpenOCD 的板级配置文件（例如 ESP-WROVER-KIT 开发板的 ``board\esp32-wrover-kit-3.3v.cfg``）提供了 ``ESP32_FLASH_VOLTAGE`` 参数来设置 ``TDO`` 信号线在空闲状态下的二进制电平，这样就可以减少由于 flash 电压不正确而导致的应用程序启动不良的几率。

    查看 JTAG 连接的 ESP32 模组的规格书，检查其 SPI flash 芯片的供电电压值，然后再相应的设置 ``ESP32_FLASH_VOLTAGE``。大多数WROOM模块使用 3.3 V 的 flash 芯片。 早于 ESP32-WROVER-B 的 WROVER 模块使用 1.8 V flash 芯片，而ESP32-WROVER-B和-E模块使用 3.3 V flash 芯片。

    .. _jtag-debugging-tip-optimize-jtag-speed:

.. only:: not esp32

    .. _jtag-debugging-tip-optimize-jtag-speed:

优化 JTAG 的速度
^^^^^^^^^^^^^^^^^^^^^

为了实现更高的数据通信速率同时最小化丢包数，建议优化 JTAG 时钟频率的设置，使其达到 JTAG 能稳定运行的最大值。为此，请参考以下建议。

1.  如果 CPU 以 80 MHz 运行，则 JTAG 时钟频率的上限为 20 MHz；如果 CPU 以 160 MHz 或者 240 MHz 运行，则上限为 26 MHz。
2.  根据特定的 JTAG 适配器和连接线缆的长度，你可能需要将 JTAG 的工作频率降低至 20 MHz 或 26 MHz 以下。
3.  在某些特殊情况下，如果你看到 DSR/DIR 错误（并且它并不是由 OpenOCD 试图从一个没有物理存储器映射的地址空间读取数据而导致的），请降低 JTAG 的工作频率。
4.  ESP-WROVER-KIT 能够稳定运行在 20 MHz 或 26 MHz 频率下。


.. _jtag-debugging-tip-debugger-startup-commands:

调试器的启动命令的含义
^^^^^^^^^^^^^^^^^^^^^^^^^^^

在启动时，调试器发出一系列命令来复位芯片并使其在特定的代码行停止运行。这个命令序列（如下所示）支持自定义，用户可以选择在最方便合适的代码行开始调试工作。

* ``set remote hardware-watchpoint-limit {IDF_TARGET_SOC_CPU_WATCHPOINTS_NUM}`` — 限制 GDB 使用芯片支持的硬件观察点数量，{IDF_TARGET_NAME} 支持 {IDF_TARGET_SOC_CPU_WATCHPOINTS_NUM} 个观察点。更多详细信息，请查阅 `GDB 配置远程目标 <https://sourceware.org/gdb/onlinedocs/gdb/Remote-Configuration.html>`_ 。
* ``mon reset halt`` — 复位芯片并使 CPU 停止运行。
* ``maintenance flush register-cache`` — monitor (``mon``) 命令无法通知 GDB 目标状态已经更改，GDB 会假设在 ``mon reset halt`` 之前所有的任务堆栈仍然有效。实际上，复位后目标状态将发生变化。执行 ``maintenance flush register-cache`` 是一种强制 GDB 从目标获取最新状态的方法。
* ``thb app_main`` — 在 ``app_main`` 处插入一个临时的硬件断点，如果有需要，可以将其替换为其他函数名。
* ``c`` — 恢复程序运行，它将会在 ``app_main`` 的断点处停止运行。


.. _jtag-debugging-tip-openocd-configure-target:

根据目标芯片配置 OpenOCD
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

OpenOCD 有很多种配置文件（``*.cfg``），它们位于 OpenOCD 安装目录的 ``share/openocd/scripts`` 子目录中（或者在 OpenOCD 源码目录的 ``tcl/scripts`` 目录中）。本文主要介绍 ``board``，``interface`` 和 ``target`` 这三个目录。

* ``interface`` 包含了例如 ESPProg、J-Link 这些 JTAG 适配器的配置文件。
* ``target`` 包含了目标芯片或者模组的配置文件。
* ``board`` 包含有内置了 JTAG 适配器的开发板的配置文件，这些配置文件会根据实际的 JTAG 适配器和芯片/模组来导入某个具体的 ``interface`` 和 ``target`` 的配置。

{IDF_TARGET_NAME} 可以使用的配置文件如下表所示:

.. include:: {IDF_TARGET_PATH_NAME}.inc
    :start-after: openocd-cfg-files
    :end-before: ---


如果你使用的开发板已经有了一份预定义好的配置文件，你只须将该文件通过 ``-f`` 参数告诉 OpenOCD。

如果你的开发板不在上述列表中，你需要使用多个 ``-f`` 参数来告诉 OpenOCD 你选择的 ``interface`` 和 ``target`` 配置文件。

自定义配置文件
""""""""""""""

OpenOCD 的配置文件是用 TCL 语言编写的, 包含了定制和编写脚本的各种选项。这在非标准调试的场景中非常有用，更多关于 TCL 脚本的内容请参考 `OpenOCD 参考手册`_。

.. _jtag-debugging-tip-openocd-config-vars:

OpenOCD 中的配置变量
""""""""""""""""""""

你还可以视情况在导入 ``target`` 配置文件之前，设定如下变量的值。可以写在自定义配置文件中，或者通过命令行传递。

TCL 语言中为变量赋值的语法是:

.. code-block:: tcl

    set VARIABLE_NAME value

在命令行中为变量赋值请参考如下示例（请把 .cfg 配置文件替换成你自己的开发板配置）:

.. code-block:: bash

    openocd -c 'set VARIABLE_NAME value' -f board/esp-xxxxx-kit.cfg

请切记，一定要在导入配置文件之前设置这些变量，否则变量的值将不会生效。为多个变量赋值需要重复多次 ``-c`` 选项。

.. list-table:: 通用的 ESP 相关的 OpenOCD 变量
    :widths: 25 75
    :header-rows: 1

    * - 变量名
      - 描述
    * - ``ESP_RTOS``
      - 设置成 ``none`` 可以关闭 OpenOCD 对 RTOS 的支持，这样的话，你将无法在 GDB 中查看到线程列表。这个功能在调试 FreeRTOS 本身的时候会很有用，可以单步调试调度器的代码。
    * - ``ESP_FLASH_SIZE``
      - 设置成 ``0`` 可以关闭对 flash 断点的支持。
    * - ``ESP_SEMIHOST_BASEDIR``
      - 设置 semihosting 在主机端的默认目录。

.. include:: {IDF_TARGET_PATH_NAME}.inc
    :start-after: openocd-target-specific-config-vars
    :end-before: ---

.. _jtag-debugging-tip-reset-by-debugger:

复位 {IDF_TARGET_NAME}
^^^^^^^^^^^^^^^^^^^^^

通过在 GDB 中输入 ``mon reset`` 或者 ``mon reset halt`` 来复位板子。


.. _jtag-debugging-tip-jtag-pins-reconfigured:

JTAG 管脚是否能用于其他功能
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

    {IDF_TARGET_NAME} 包含一个可用于调试的 USB Serial/JTAG 控制器。 默认情况下，{IDF_TARGET_NAME} JTAG 接口连接到内置的外设 USB Serial/JTAG。 详细信息可参阅 :doc:`配置 {IDF_TARGET_NAME} 内置 JTAG 接口 <../jtag-debugging/configure-builtin-jtag>`。

    如果 USB Serial/JTAG 控制器用于调试，|jtag-gpio-list| 可用于其他功能。

    如果用户通过烧录 eFuse 将 USB JTAG 接口切换为 GPIO，|jtag-gpio-list| 则可用于 JTAG 调试，但用于 JTAG 调试的 |jtag-gpio-list| 不能再用于其他功能。

如果除了 {IDF_TARGET_NAME} 模组和 JTAG 适配器之外的其他硬件也连接到了 JTAG 管脚，那么 JTAG 的操作可能会受到干扰。{IDF_TARGET_NAME} JTAG 使用以下管脚：

.. include:: {IDF_TARGET_PATH_NAME}.inc
    :start-after: jtag-pins
    :end-before: ---

如果用户应用程序更改了 JTAG 管脚的配置，JTAG 通信可能会失败。如果 OpenOCD 正确初始化（检测到芯片全部 CPU 内核），但在程序运行期间失去了同步并报出大量 DTR/DIR 错误，原因可能是应用程序将 JTAG 管脚重新配置为其他功能或者用户忘记将 Vtar 连接到 JTAG 适配器。

.. only:: esp32

    .. highlight:: none

    以下是应用程序进入重新配置 MTDO 作为输入代码后，双核 {IDF_TARGET_NAME} GDB 报告的一系列错误摘录::

        cpu0: xtensa_resume (line 431): DSR (FFFFFFFF) indicates target still busy!
        cpu0: xtensa_resume (line 431): DSR (FFFFFFFF) indicates DIR instruction generated an exception!
        cpu0: xtensa_resume (line 431): DSR (FFFFFFFF) indicates DIR instruction generated an overrun!
        cpu1: xtensa_resume (line 431): DSR (FFFFFFFF) indicates target still busy!
        cpu1: xtensa_resume (line 431): DSR (FFFFFFFF) indicates DIR instruction generated an exception!
        cpu1: xtensa_resume (line 431): DSR (FFFFFFFF) indicates DIR instruction generated an overrun!

.. _jtag-debugging-security-features:

JTAG 与 flash 加密和安全引导
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

默认情况下，开启了 flash 加密和（或者）安全引导后，系统在首次启动时，引导程序会烧写 eFuse 的某个比特，从而将 JTAG 永久关闭。

.. only:: SOC_HMAC_SUPPORTED

    请注意，一旦 JTAG 被永久禁用，就无法重新启用以访问 JTAG。但是我们也提供了暂时禁用 (soft disable) JTAG 的选项。有关如何暂时禁用以及重新启用 JTAG，请参考 :ref:`hmac_for_enabling_jtag`。

Kconfig 配置项 :ref:`CONFIG_SECURE_BOOT_ALLOW_JTAG` 可以改变这个默认行为，使得用户即使开启了安全引导或者 flash 加密，仍会保留 JTAG 的功能。

然而，因为设置 :ref:`软件断点 <jtag-debugging-tip-where-breakpoints>` 的需要，OpenOCD 会尝试自动读写 flash 中的内容，这会带来两个问题：

- 软件断点和 flash 加密是不兼容的，目前 OpenOCD 尚不支持对 flash 中的内容进行加密和解密。
- 如果开启了安全引导功能，设置软件断点会改变被签名的程序的摘要，从而使得签名失效。这也意味着，如果设置了软件断点，系统会在下次重启时的签名验证阶段失败，导致无法启动。

关闭 JTAG 的软件断点功能，可以在启动 OpenOCD 时在命令行额外加一项配置参数 ``-c 'set ESP_FLASH_SIZE 0'``，请参考 :ref:`jtag-debugging-tip-openocd-config-vars`。

.. note::

   同样地，当启用该选项，并且在调试过程中设置了软件断点，引导程序将无法校验通过应用程序的签名。

.. only:: esp32

    JTAG 和 ESP32-WROOM-32 AT 固件兼容性问题
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    ESP32-WROOM 系列模块预装了 AT 固件。该固件将 GPIO12 至 GPIO15 管脚配置为 SPI 从属接口，使得无法使用 JTAG。

    要想使用 JTAG，需要编译新的固件，新的固件不能使用专门用于 JTAG 通信的管脚（GPIO12 至 GPIO15），然后将固件烧录到模组中。请参考 :ref:`jtag-debugging-tip-jtag-pins-reconfigured`。

.. _jtag-debugging-tip-reporting-issues:

报告 OpenOCD/GDB 的问题
^^^^^^^^^^^^^^^^^^^^^^^^^

如果你遇到 OpenOCD 或者 GDB 程序本身的问题，并且在网上没有找到可用的解决方案，请前往 https://github.com/espressif/openocd-esp32/issues 新建一个议题。

1.  请在问题报告中提供你使用的配置的详细信息：

    a. JTAG 适配器类型。
    b. 用于编译和加载正在调试的应用程序的 ESP-IDF 版本号。
    c. 用于调试的操作系统的详细信息。
    d. 操作系统是在本地计算机运行还是在虚拟机上运行？

2.  创建一个能够演示问题的简单示例工程，描述复现该问题的步骤。且这个调试示例不能受到 Wi-Fi 协议栈引入的非确定性行为的影响，这样再次遇到同样问题时，更容易复现。

.. highlight:: bash

3.  在启动命令中添加额外的参数来输出调试日志。

    OpenOCD 端：

    .. include:: {IDF_TARGET_PATH_NAME}.inc
        :start-after: run-openocd-d3
        :end-before: ---

    这种方式会将日志输出到文件，但是它会阻止调试信息打印在终端上。当有大量信息需要输出的时候（比如调试等级提高到 ``-d3``）这是个不错的选择。如果你仍然希望在屏幕上看到调试日志，请改用以下命令：

    .. include:: {IDF_TARGET_PATH_NAME}.inc
        :start-after: run-openocd-d3-tee
        :end-before: ---

    Debugger 端：

    .. include:: {IDF_TARGET_PATH_NAME}.inc
        :start-after: run-gdb-remotelog
        :end-before: ---

    也可以将命令 ``remotelogfile gdb_log.txt`` 添加到 ``gdbinit`` 文件中。


4.  请将 ``openocd_log.txt`` 和 ``gdb_log.txt`` 文件附在你的问题报告中。


.. _OpenOCD 参考手册: https://openocd.org/doc/html/index.html
