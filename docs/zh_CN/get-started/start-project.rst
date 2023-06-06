{IDF_TARGET_FEATURES:default="[NEEDS TO BE UPDATED]", esp32="WiFi/BT/BLE, silicon revision 1, 2 MB external flash", esp32s2="WiFi, silicon revision 0, 2 MB external flash", esp32s3="This is esp32s3 chip with 2 CPU core(s), WiFi/BLE, silicon revision 0, 2 MB external flash", esp32c2="WiFi/BLE, silicon revision 0, 2 MB embedded flash", esp32c3="WiFi/BLE, silicon revision 0, 2 MB external flash", esp32c6="WiFi/BLE, 802.15.4 (Zigbee/Thread), silicon revision v0.0, 2 MB external flash", esp32h2="BLE, 802.15.4 (Zigbee/Thread), silicon revision v0.1, 2 MB external flash"}

{IDF_TARGET_HEAP_SIZE:default="[NEEDS TO BE UPDATED]", esp32="298968", esp32s2="253900", esp32s3="390684", esp32c2="203888", esp32c3="337332", esp32c6="473816", esp32h2="268256"}

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

请运行以下命令，将刚刚生成的二进制文件烧录至您的 {IDF_TARGET_NAME} 开发板：

.. code-block:: bash

    idf.py -p PORT flash

请将 PORT 替换为 {IDF_TARGET_NAME} 开发板的串口名称。如果 ``PORT`` 未经定义，:ref:`idf.py` 将尝试使用可用的串口自动连接。

更多有关 idf.py 参数的详情，请见 :ref:`idf.py`。

.. note::

    勾选 ``flash`` 选项将自动编译并烧录工程，因此无需再运行 ``idf.py build``。

若在烧录过程中遇到问题，请前往 :doc:`flashing-troubleshooting` 或 :doc:`establish-serial-connection` 获取更多详细信息。

常规操作
~~~~~~~~~~~~~~~~

在烧录过程中，您会看到类似如下的输出日志：

.. only:: esp32

    .. code-block:: none

        ...
        esptool.py --chip esp32 -p /dev/ttyUSB0 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size 2MB 0x8000 partition_table/partition-table.bin 0x1000 bootloader/bootloader.bin 0x10000 hello_world.bin
        esptool.py v3.0-dev
        Serial port /dev/ttyUSB0
        Connecting........_
        Chip is ESP32D0WDQ6 (revision 0)
        Features: WiFi, BT, Dual Core, Coding Scheme None
        Crystal is 40MHz
        MAC: 24:0a:c4:05:b9:14
        Uploading stub...
        Running stub...
        Stub running...
        Changing baud rate to 460800
        Changed.
        Configuring flash size...
        Compressed 3072 bytes to 103...
        Writing at 0x00008000... (100 %)
        Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.0 seconds (effective 5962.8 kbit/s)...
        Hash of data verified.
        Compressed 26096 bytes to 15408...
        Writing at 0x00001000... (100 %)
        Wrote 26096 bytes (15408 compressed) at 0x00001000 in 0.4 seconds (effective 546.7 kbit/s)...
        Hash of data verified.
        Compressed 147104 bytes to 77364...
        Writing at 0x00010000... (20 %)
        Writing at 0x00014000... (40 %)
        Writing at 0x00018000... (60 %)
        Writing at 0x0001c000... (80 %)
        Writing at 0x00020000... (100 %)
        Wrote 147104 bytes (77364 compressed) at 0x00010000 in 1.9 seconds (effective 615.5 kbit/s)...
        Hash of data verified.

        Leaving...
        Hard resetting via RTS pin...
        Done

.. only:: esp32s2

    .. code-block:: none

        ...
        esptool.py --chip esp32s2 -p /dev/ttyUSB0 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size 2MB 0x8000 partition_table/partition-table.bin 0x1000 bootloader/bootloader.bin 0x10000 hello_world.bin
        esptool.py v3.0-dev
        Serial port /dev/ttyUSB0
        Connecting....
        Chip is ESP32-S2
        Features: WiFi
        Crystal is 40MHz
        MAC: 18:fe:34:72:50:e3
        Uploading stub...
        Running stub...
        Stub running...
        Changing baud rate to 460800
        Changed.
        Configuring flash size...
        Compressed 3072 bytes to 103...
        Writing at 0x00008000... (100 %)
        Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.0 seconds (effective 3851.6 kbit/s)...
        Hash of data verified.
        Compressed 22592 bytes to 13483...
        Writing at 0x00001000... (100 %)
        Wrote 22592 bytes (13483 compressed) at 0x00001000 in 0.3 seconds (effective 595.1 kbit/s)...
        Hash of data verified.
        Compressed 140048 bytes to 70298...
        Writing at 0x00010000... (20 %)
        Writing at 0x00014000... (40 %)
        Writing at 0x00018000... (60 %)
        Writing at 0x0001c000... (80 %)
        Writing at 0x00020000... (100 %)
        Wrote 140048 bytes (70298 compressed) at 0x00010000 in 1.7 seconds (effective 662.5 kbit/s)...
        Hash of data verified.

        Leaving...
        Hard resetting via RTS pin...
        Done

.. only:: esp32s3

    .. code-block:: none

        ...
        esptool.py esp32s3 -p /dev/ttyUSB0 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 80m --flash_size 2MB 0x0 bootloader/bootloader.bin 0x10000 hello_world.bin 0x8000 partition_table/partition-table.bin
        esptool.py v3.2-dev
        Serial port /dev/ttyUSB0
        Connecting....
        Chip is ESP32-S3
        Features: WiFi, BLE
        Crystal is 40MHz
        MAC: 7c:df:a1:e0:00:64
        Uploading stub...
        Running stub...
        Stub running...
        Changing baud rate to 460800
        Changed.
        Configuring flash size...
        Flash will be erased from 0x00000000 to 0x00004fff...
        Flash will be erased from 0x00010000 to 0x00039fff...
        Flash will be erased from 0x00008000 to 0x00008fff...
        Compressed 18896 bytes to 11758...
        Writing at 0x00000000... (100 %)
        Wrote 18896 bytes (11758 compressed) at 0x00000000 in 0.5 seconds (effective 279.9 kbit/s)...
        Hash of data verified.
        Compressed 168208 bytes to 88178...
        Writing at 0x00010000... (16 %)
        Writing at 0x0001a80f... (33 %)
        Writing at 0x000201f1... (50 %)
        Writing at 0x00025dcf... (66 %)
        Writing at 0x0002d0be... (83 %)
        Writing at 0x00036c07... (100 %)
        Wrote 168208 bytes (88178 compressed) at 0x00010000 in 2.4 seconds (effective 569.2 kbit/s)...
        Hash of data verified.
        Compressed 3072 bytes to 103...
        Writing at 0x00008000... (100 %)
        Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.1 seconds (effective 478.9 kbit/s)...
        Hash of data verified.

        Leaving...
        Hard resetting via RTS pin...
        Done


.. only:: esp32c2

    .. code-block:: none

        ...
        esptool.py esp32c2 -p /dev/ttyUSB0 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 60m --flash_size 2MB 0x0 bootloader/bootloader.bin 0x10000 hello_world.bin 0x8000 partition_table/partition-table.bin
        esptool.py v3.3.1
        Serial port /dev/ttyUSB0
        Connecting....
        Chip is ESP32-C2 (revision 1)
        Features: Wi-Fi
        Crystal is 40MHz
        MAC: 10:97:bd:f0:e5:0c
        Uploading stub...
        Running stub...
        Stub running...
        Changing baud rate to 460800
        Changed.
        Configuring flash size...
        Flash will be erased from 0x00000000 to 0x00004fff...
        Flash will be erased from 0x00010000 to 0x0002ffff...
        Flash will be erased from 0x00008000 to 0x00008fff...
        Compressed 18192 bytes to 10989...
        Writing at 0x00000000... (100 %)
        Wrote 18192 bytes (10989 compressed) at 0x00000000 in 0.6 seconds (effective 248.5 kbit/s)...
        Hash of data verified.
        Compressed 128640 bytes to 65895...
        Writing at 0x00010000... (20 %)
        Writing at 0x00019539... (40 %)
        Writing at 0x00020bf2... (60 %)
        Writing at 0x00027de1... (80 %)
        Writing at 0x0002f480... (100 %)
        Wrote 128640 bytes (65895 compressed) at 0x00010000 in 1.7 seconds (effective 603.0 kbit/s)...
        Hash of data verified.
        Compressed 3072 bytes to 103...
        Writing at 0x00008000... (100 %)
        Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.1 seconds (effective 360.1 kbit/s)...
        Hash of data verified.

        Leaving...
        Hard resetting via RTS pin...


.. only:: esp32c3

    .. code-block:: none

        ...
        esptool.py --chip esp32c3 -p /dev/ttyUSB0 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 80m --flash_size 2MB 0x8000 partition_table/partition-table.bin 0x0 bootloader/bootloader.bin 0x10000 hello_world.bin
        esptool.py v3.0
        Serial port /dev/ttyUSB0
        Connecting....
        Chip is ESP32-C3
        Features: Wi-Fi
        Crystal is 40MHz
        MAC: 7c:df:a1:40:02:a4
        Uploading stub...
        Running stub...
        Stub running...
        Changing baud rate to 460800
        Changed.
        Configuring flash size...
        Compressed 3072 bytes to 103...
        Writing at 0x00008000... (100 %)
        Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.0 seconds (effective 4238.1 kbit/s)...
        Hash of data verified.
        Compressed 18960 bytes to 11311...
        Writing at 0x00000000... (100 %)
        Wrote 18960 bytes (11311 compressed) at 0x00000000 in 0.3 seconds (effective 584.9 kbit/s)...
        Hash of data verified.
        Compressed 145520 bytes to 71984...
        Writing at 0x00010000... (20 %)
        Writing at 0x00014000... (40 %)
        Writing at 0x00018000... (60 %)
        Writing at 0x0001c000... (80 %)
        Writing at 0x00020000... (100 %)
        Wrote 145520 bytes (71984 compressed) at 0x00010000 in 2.3 seconds (effective 504.4 kbit/s)...
        Hash of data verified.

        Leaving...
        Hard resetting via RTS pin...
        Done

.. only:: esp32c6

    .. code-block:: none

        ...
        esptool esp32c6 -p /dev/ttyUSB0 -b 460800 --before=default_reset --after=hard_reset --no-stub write_flash --flash_mode dio --flash_freq 80m --flash_size 2MB 0x0 bootloader/bootloader.bin 0x10000 hello_world.bin 0x8000 partition_table/partition-table.bin
        esptool.py v4.3
        Serial port /dev/ttyUSB0
        Connecting....
        Chip is ESP32-C6 (revision v0.0)
        Features: WiFi 6, BT 5
        Crystal is 40MHz
        MAC: 60:55:f9:f6:01:38
        Changing baud rate to 460800
        Changed.
        Enabling default SPI flash mode...
        Configuring flash size...
        Flash will be erased from 0x00000000 to 0x00004fff...
        Flash will be erased from 0x00010000 to 0x00028fff...
        Flash will be erased from 0x00008000 to 0x00008fff...
        Erasing flash...
        Took 0.17s to erase flash block
        Writing at 0x00000000... (5 %)
        Writing at 0x00000c00... (23 %)
        Writing at 0x00001c00... (47 %)
        Writing at 0x00003000... (76 %)
        Writing at 0x00004000... (100 %)
        Wrote 17408 bytes at 0x00000000 in 0.5 seconds (254.6 kbit/s)...
        Hash of data verified.
        Erasing flash...
        Took 0.85s to erase flash block
        Writing at 0x00010000... (1 %)
        Writing at 0x00014c00... (20 %)
        Writing at 0x00019c00... (40 %)
        Writing at 0x0001ec00... (60 %)
        Writing at 0x00023c00... (80 %)
        Writing at 0x00028c00... (100 %)
        Wrote 102400 bytes at 0x00010000 in 3.2 seconds (253.5 kbit/s)...
        Hash of data verified.
        Erasing flash...
        Took 0.04s to erase flash block
        Writing at 0x00008000... (33 %)
        Writing at 0x00008400... (66 %)
        Writing at 0x00008800... (100 %)
        Wrote 3072 bytes at 0x00008000 in 0.1 seconds (269.0 kbit/s)...
        Hash of data verified.

        Leaving...
        Hard resetting via RTS pin...

.. only:: esp32h2

    .. code-block:: none

        ...
        esptool esp32h2 -p /dev/ttyUSB0 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 48m --flash_size 2MB 0x0 bootloader/bootloader.bin 0x10000 hello_world.bin 0x8000 partition_table/partition-table.bin
        esptool.py v4.6
        Serial port /dev/ttyUSB0
        Connecting....
        Chip is ESP32-H2 (revision v0.1)
        Features: BLE
        Crystal is 32MHz
        MAC: 60:55:f9:f7:3e:93:ff:fe
        Uploading stub...
        Running stub...
        Stub running...
        Changing baud rate to 460800
        Changed.
        Configuring flash size...
        Flash will be erased from 0x00000000 to 0x00005fff...
        Flash will be erased from 0x00010000 to 0x00034fff...
        Flash will be erased from 0x00008000 to 0x00008fff...
        Compressed 20880 bytes to 12788...
        Writing at 0x00000000... (100 %)
        Wrote 20880 bytes (12788 compressed) at 0x00000000 in 0.6 seconds (effective 297.5 kbit/s)...
        Hash of data verified.
        Compressed 149424 bytes to 79574...
        Writing at 0x00010000... (20 %)
        Writing at 0x00019959... (40 %)
        Writing at 0x00020bb5... (60 %)
        Writing at 0x00026d8f... (80 %)
        Writing at 0x0002e60a... (100 %)
        Wrote 149424 bytes (79574 compressed) at 0x00010000 in 2.1 seconds (effective 571.7 kbit/s)...
        Hash of data verified.
        Compressed 3072 bytes to 103...
        Writing at 0x00008000... (100 %)
        Wrote 3072 bytes (103 compressed) at 0x00008000 in 0.0 seconds (effective 539.7 kbit/s)...
        Hash of data verified.

        Leaving...
        Hard resetting via RTS pin...

如果一切顺利，烧录完成后，开发板将会复位，应用程序 "hello_world" 开始运行。

如果您希望使用 Eclipse 或是 VS Code IDE，而非 ``idf.py``，请参考 `Eclipse Plugin <https://github.com/espressif/idf-eclipse-plugin/blob/master/README_CN.md>`_，以及 `VSCode Extension <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md>`_。

监视输出
===============

您可以使用 ``idf.py -p PORT monitor`` 命令，监视 “hello_world” 工程的运行情况。注意，不要忘记将 PORT 替换为您的串口名称。

运行该命令后，:doc:`IDF 监视器 <../api-guides/tools/idf-monitor>` 应用程序将启动：::

    $ idf.py -p <PORT> monitor
    Running idf_monitor in directory [...]/esp/hello_world/build
    Executing "python [...]/esp-idf/tools/idf_monitor.py -b 115200 [...]/esp/hello_world/build/hello_world.elf"...
    --- idf_monitor on <PORT> 115200 ---
    --- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
    ets Jun  8 2016 00:22:57

    rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
    ets Jun  8 2016 00:22:57
    ...

此时，您就可以在启动日志和诊断日志之后，看到打印的 “Hello world!” 了。

.. code-block:: none

        ...
        Hello world!
        Restarting in 10 seconds...
        This is {IDF_TARGET_PATH_NAME} chip with {IDF_TARGET_SOC_CPU_CORES_NUM} CPU core(s), {IDF_TARGET_FEATURES}
    Minimum free heap size: {IDF_TARGET_HEAP_SIZE} bytes
        Restarting in 9 seconds...
        Restarting in 8 seconds...
        Restarting in 7 seconds...

您可使用快捷键 ``Ctrl+]``，退出 IDF 监视器。

.. only:: esp32 or esp32c2

    如果 IDF 监视器在烧录后很快发生错误，或打印信息全是乱码（如下），很有可能是因为您的开发板采用了 26 MHz 晶振，而 ESP-IDF 默认支持大多数开发板使用的 40 MHz 晶振。

    .. figure:: ../../_static/get-started-garbled-output.png
        :align: center
        :alt: 乱码输出
        :figclass: align-center

    此时，您可以：

    1. 退出监视器。
    2. 返回 ``menuconfig``。
    3. 进入 ``Component config`` --> ``Hardware Settings`` --> ``Main XTAL Config`` --> ``Main XTAL frequency`` 进行配置，将 :ref:`CONFIG_XTAL_FREQ_SEL` 设置为 26 MHz。
    4. 重新 ``编译和烧录`` 应用程序。

    在当前的 ESP-IDF 版本中，{IDF_TARGET_NAME} 支持的主晶振频率如下：

    .. list::

        :SOC_XTAL_SUPPORT_24M: - 24 MHz
        :SOC_XTAL_SUPPORT_26M: - 26 MHz
        :SOC_XTAL_SUPPORT_32M: - 32 MHz
        :SOC_XTAL_SUPPORT_40M: - 40 MHz

.. note::

    您也可以运行以下命令，一次性执行构建、烧录和监视过程：

    ``idf.py -p PORT flash monitor``

此外，

- 请前往 :doc:`IDF 监视器 <../api-guides/tools/idf-monitor>`，了解更多使用 IDF 监视器的快捷键和其他详情。
- 请前往 :ref:`idf.py`，查看更多 ``idf.py`` 命令和选项。

**恭喜，您已完成 {IDF_TARGET_NAME} 的入门学习！**

现在，您可以尝试一些其他 :idf:`examples`，或者直接开发自己的应用程序。

.. 重要::

	一些示例程序不支持 {IDF_TARGET_NAME}，因为 {IDF_TARGET_NAME} 中不包含所需的硬件。

	在编译示例程序前请查看 README 文件中 ``Supported Targets`` 表格。如果表格中包含 {IDF_TARGET_NAME}， 或者不存在这个表格，那么即表示 {IDF_TARGET_NAME} 支持这个示例程序。


其他提示
===============

权限问题 /dev/ttyUSB0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

使用某些 Linux 版本向 {IDF_TARGET_NAME} 烧录固件时，可能会出现 ``Failed to open port /dev/ttyUSB0`` 错误消息。此时可以将用户添加至 :ref:`Linux Dialout 组<linux-dialout-group>`。

兼容的 Python 版本
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ESP-IDF 支持 Python 3.7 及以上版本，建议升级操作系统到最新版本从而更新 Python。也可选择从 `sources <https://www.python.org/downloads/>`_ 安装最新版 Python，或使用 Python 管理系统如 `pyenv <https://github.com/pyenv/pyenv>`_ 对版本进行升级管理。

.. only:: esp32 or esp32s2 or esp32s3

    ..
        当在上述行中添加新目标时，请同时更新 windows-start-project.rst 和 linux-macos-start-project.rst 中的列表。


    上手板级支持包
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    您可以使用 `板级支持包 (BSP) <https://github.com/espressif/esp-bsp>`_，协助您在开发板上的原型开发。仅需要调用几个函数，便可以完成对特定开发板的初始化。

    一般来说，BSP 支持开发板上所有硬件组件。除了管脚定义和初始化功能外，BSP 还附带如传感器、显示器、音频编解码器等外部元件的驱动程序。

    BSP 通过 :doc:`IDF 组件管理器 </api-guides/tools/idf-component-manager>` 发布，您可以前往 `IDF 组件注册器 <https://components.espressif.com>`_ 进行下载。

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
