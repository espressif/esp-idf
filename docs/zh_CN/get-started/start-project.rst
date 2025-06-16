{IDF_TARGET_FEATURES:default="[NEEDS TO BE UPDATED]", esp32="WiFi/BT/BLE, silicon revision 1, 2 MB external flash", esp32s2="WiFi, silicon revision 0, 2 MB external flash", esp32s3="This is esp32s3 chip with 2 CPU core(s), WiFi/BLE, silicon revision 0, 2 MB external flash", esp32c2="WiFi/BLE, silicon revision 0, 2 MB embedded flash", esp32c3="WiFi/BLE, silicon revision 0, 2 MB external flash", esp32c6="WiFi/BLE, 802.15.4 (Zigbee/Thread), silicon revision v0.0, 2 MB external flash", esp32h2="BLE, 802.15.4 (Zigbee/Thread), silicon revision v0.1, 2 MB external flash", esp32p4="silicon revision v0.0, 2 MB external flash", esp32c5="WiFi/BLE, 802.15.4 (Zigbee/Thread), silicon revision v1.0, 2 MB external flash"}

{IDF_TARGET_HEAP_SIZE:default="[NEEDS TO BE UPDATED]", esp32="298968", esp32s2="253900", esp32s3="390684", esp32c2="203888", esp32c3="337332", esp32c6="473816", esp32h2="268256", esp32p4="618848", esp32c5="348724"}

编译工程
=========================

请使用以下命令，编译烧录工程：

.. code-block:: batch

    idf.py build

运行以上命令可以编译应用程序和所有 ESP-IDF 组件，接着生成引导加载程序、分区表和应用程序二进制文件。

.. code-block:: none

    $ idf.py build
    Running cmake in directory /path/to/hello_world/build
    Executing "cmake -G Ninja --warn-uninitialized /path/to/hello_world"...
    Warn about uninitialized values.
    -- Found Git: /usr/bin/git (found version "2.17.0")
    -- Building empty aws_iot component due to configuration
    -- Component names: ...
    -- Component paths: ...

    ... (more lines of build system output)

    [527/527] Generating hello_world.bin
    esptool.py v2.3.1

    Project build complete. To flash, run this command:
    ../../../components/esptool_py/esptool/esptool.py -p (PORT) -b 921600 write_flash --flash_mode dio --flash_size detect --flash_freq 40m 0x10000 build/hello_world.bin  build 0x1000 build/bootloader/bootloader.bin 0x8000 build/partition_table/partition-table.bin
    or run 'idf.py -p PORT flash'

如果一切正常，编译完成后将生成 .bin 文件。


烧录到设备
=============================

请运行以下命令，将刚刚生成的二进制文件烧录至 {IDF_TARGET_NAME} 开发板：

.. code-block:: bash

    idf.py -p PORT flash

请将 PORT 替换为 {IDF_TARGET_NAME} 开发板的串口名称。如果 ``PORT`` 未经定义，:ref:`idf.py` 将尝试使用可用的串口自动连接。

更多有关 idf.py 参数的详情，请见 :ref:`idf.py`。

.. note::

    勾选 ``flash`` 选项将自动编译并烧录工程，因此无需再运行 ``idf.py build``。

若在烧录过程中遇到问题，请参考下文中的“其他提示”。也可以前往 :doc:`flashing-troubleshooting` 或 :doc:`establish-serial-connection` 获取更多详细信息。

常规操作
~~~~~~~~~~~~~~~~

在烧录过程中，会看到类似如下的输出日志：

.. include:: {IDF_TARGET_PATH_NAME}_output_log.inc
   :start-after: output_log

如果一切顺利，烧录完成后，开发板将会复位，应用程序 "hello_world" 开始运行。

如果希望使用 Eclipse 或是 VS Code IDE，而非 ``idf.py``，请参考 `Eclipse Plugin <https://github.com/espressif/idf-eclipse-plugin/blob/master/README_CN.md>`_，以及 `VSCode Extension <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md>`_。

监视输出
===============

可以使用 ``idf.py -p PORT monitor`` 命令，监视 “hello_world” 工程的运行情况。注意，不要忘记将 PORT 替换为自己的串口名称。

运行该命令后，:doc:`IDF 监视器 <../api-guides/tools/idf-monitor>` 应用程序将启动。

.. code-block:: bash

    $ idf.py -p <PORT> monitor
    Running idf_monitor in directory [...]/esp/hello_world/build
    Executing "python [...]/esp-idf/tools/idf_monitor.py -b 115200 [...]/esp/hello_world/build/hello_world.elf"...
    --- idf_monitor on <PORT> 115200 ---
    --- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
    ets Jun  8 2016 00:22:57

    rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
    ets Jun  8 2016 00:22:57
    ...

此时，就可以在启动日志和诊断日志之后，看到打印的 “Hello world!” 了。

.. code-block:: none

        ...
        Hello world!
        Restarting in 10 seconds...
        This is {IDF_TARGET_PATH_NAME} chip with {IDF_TARGET_SOC_CPU_CORES_NUM} CPU core(s), {IDF_TARGET_FEATURES}
    Minimum free heap size: {IDF_TARGET_HEAP_SIZE} bytes
        Restarting in 9 seconds...
        Restarting in 8 seconds...
        Restarting in 7 seconds...

使用快捷键 ``Ctrl+]``，可退出 ESP-IDF 监视器。

.. only:: esp32 or esp32c2

    如果 ESP-IDF 监视器在烧录后很快发生错误，或打印信息全是乱码（如下），很有可能是因为开发板采用了 26 MHz 晶振，而 ESP-IDF 默认支持大多数开发板使用的 40 MHz 晶振。

    .. figure:: ../../_static/get-started-garbled-output.png
        :align: center
        :alt: 乱码输出
        :figclass: align-center

    此时，可以：

    1. 退出监视器。
    2. 返回 ``menuconfig``。
    3. 进入 ``Component config`` --> ``Hardware Settings`` --> ``Main XTAL Config`` --> ``Main XTAL frequency`` 进行配置，将 :ref:`CONFIG_XTAL_FREQ` 设置为 26 MHz。
    4. 重新 ``编译和烧录`` 应用程序。

    在当前的 ESP-IDF 版本中，{IDF_TARGET_NAME} 支持的主晶振频率如下：

    .. list::

        :SOC_XTAL_SUPPORT_26M: - 26 MHz
        :SOC_XTAL_SUPPORT_32M: - 32 MHz
        :SOC_XTAL_SUPPORT_40M: - 40 MHz

.. note::

    也可以运行以下命令，一次性执行构建、烧录和监视过程：

    ``idf.py -p PORT flash monitor``

此外，

- 请前往 :doc:`IDF 监视器 <../api-guides/tools/idf-monitor>`，了解更多使用 ESP-IDF 监视器的快捷键和其他详情。
- 请前往 :ref:`idf.py`，查看更多 ``idf.py`` 命令和选项。

**恭喜完成 {IDF_TARGET_NAME} 的入门学习！**

现在，可以尝试一些其他 :idf:`examples`，或者直接开发自己的应用程序。

.. 重要::

    一些示例程序不支持 {IDF_TARGET_NAME}，因为 {IDF_TARGET_NAME} 中不包含所需的硬件。

    在编译示例程序前请查看 README 文件中 ``Supported Targets`` 表格。如果表格中包含 {IDF_TARGET_NAME}， 或者不存在这个表格，那么即表示 {IDF_TARGET_NAME} 支持这个示例程序。



其他提示
===============

权限问题
~~~~~~~~~~~~~~~~~~~~~

使用某些 Linux 版本向 {IDF_TARGET_NAME} 烧录固件时，可能会出现类似 ``Could not open port <PORT>: Permission denied: '<PORT>'`` 错误消息。此时可以在 Linux 将用户添加至 :ref:`dialout 组或 uucp 组<linux-dialout-group>` 来解决此类问题。

兼容的 Python 版本
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ESP-IDF 支持 Python 3.9 及以上版本，建议升级操作系统到最新版本从而更新 Python。也可选择从 `sources <https://www.python.org/downloads/>`_ 安装最新版 Python，或使用 Python 管理系统如 `pyenv <https://github.com/pyenv/pyenv>`_ 对版本进行升级管理。

.. only:: esp32 or esp32s2 or esp32s3

    ..
        当在上述行中添加新目标时，请同时更新 windows-start-project.rst 和 linux-macos-start-project.rst 中的列表。


    上手板级支持包
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    可以使用 `板级支持包 (BSP) <https://github.com/espressif/esp-bsp>`_，协助在开发板上的原型开发。仅需要调用几个函数，便可以完成对特定开发板的初始化。

    一般来说，BSP 支持开发板上所有硬件组件。除了管脚定义和初始化功能外，BSP 还附带如传感器、显示器、音频编解码器等外部元件的驱动程序。

    BSP 通过 :doc:`IDF 组件管理器 </api-guides/tools/idf-component-manager>` 发布，可以前往 `乐鑫组件注册表 <https://components.espressif.com>`_ 进行下载。

    .. only:: esp32

        **以下示例演示了如何将 ESP-WROVER-KIT BSP 添加到项目中：**

        .. code-block:: bash

            idf.py add-dependency esp_wrover_kit

    .. only:: esp32s2

        **以下示例演示了如何将 ESP32-S2-Kaluga-Kit BSP 添加到项目中：**

        .. code-block:: bash

            idf.py add-dependency esp32_s2_kaluga_kit

    .. only:: esp32s3

        **以下示例演示了如何将 ESP-BOX BSP 添加到项目中：**

        .. code-block:: bash

            idf.py add-dependency esp-box

    更多有关使用 BSP 的示例，请前往 `BSP 示例文件夹 <https://github.com/espressif/esp-bsp/tree/master/examples>`_。

擦除 flash
~~~~~~~~~~~

ESP-IDF 支持擦除 flash。请运行以下命令，擦除整个 flash：

.. code-block:: bash

    idf.py -p PORT erase-flash

若存在需要擦除的 OTA 数据，请运行以下命令：

.. code-block:: bash

    idf.py -p PORT erase-otadata

擦除 flash 需要一段时间，在擦除过程中，请勿断开设备连接。
