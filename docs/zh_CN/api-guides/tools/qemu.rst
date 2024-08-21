QEMU 模拟器
===========

:link_to_translation:`en:[English]`

乐鑫维护了一个 QEMU 模拟器的 `分支 <https://github.com/espressif/qemu>`_，支持 {IDF_TARGET_NAME}。这个分支实现了对 {IDF_TARGET_NAME} 的 CPU、内存和多个外设的模拟。更多有关 {IDF_TARGET_NAME} 的 QEMU 的信息，请参阅 `QEMU README 文档 <https://github.com/espressif/esp-toolchain-docs/blob/main/qemu/README.md>`_。

:doc:`idf.py <idf-py>` 支持在 QEMU 中运行和调试应用程序，因此不必将应用程序烧录到真实硬件中，也可以便捷地进行测试。

先决条件
--------

要使用 ``idf.py`` 和 QEMU，首先需要安装上述 QEMU 分支。ESP-IDF 为 x86_64、 arm64 版本的 Linux 和 macOS，以及 x86_64 Windows 提供了预构建的二进制文件。在 Linux 和 macOS 中使用预构建的二进制文件之前，请先安装系统依赖项：

- Ubuntu 和 Debian:

.. code-block:: console

    sudo apt-get install -y libgcrypt20 libglib2.0-0 libpixman-1-0 libsdl2-2.0-0 libslirp0

- CentOS:

.. code-block:: console

    sudo yum install -y --enablerepo=powertools libgcrypt glib2 pixman SDL2 libslirp

- Arch:

.. code-block:: console

    sudo pacman -S --needed libgcrypt glib2 pixman sdl2 libslirp

- macOS:

.. code-block:: console

    brew install libgcrypt glib pixman sdl2 libslirp

然后使用以下命令来安装预构建的二进制文件：

.. code-block:: console

    python $IDF_PATH/tools/idf_tools.py install qemu-xtensa qemu-riscv32

安装完 QEMU，请在 ESP-IDF 目录中运行 ``. ./export.sh``，从而确保 QEMU 已被添加到 PATH 中。

如果使用的平台不同，则需要从源代码构建 QEMU。请参考官方 QEMU 文档。

用法
----

运行应用程序
~~~~~~~~~~~~

请使用以下命令，在 QEMU 中运行 IDF 应用程序：

.. code-block:: console

    idf.py qemu monitor

此命令可用于构建应用程序、启动 QEMU 、打开 :doc:`IDF 监视器 <idf-monitor>`，并将 IDF 监视器连接到模拟的 UART 端口。你可以看到应用程序的控制台输出并与之交互。IDF 监视器还能对 panic 回溯以及对 UART 主存转储进行自动解码。

调试
~~~~

请使用以下命令，在 QEMU 中调试应用程序：

.. code-block:: console

    idf.py qemu gdb

此命令能构建应用程序，启用 GDB 服务器，并启动 QEMU，开启交互式 GDB 会话。这样就能像在真实的硬件上一样，通过 GDB 调试应用程序。

如需在 QEMU 中调试的同时查看控制台输出，可使用两个终端。

* 在第一个终端中运行：

  .. code-block:: console

    idf.py qemu --gdb monitor

  此命令可启动 QEMU 和 IDF 监视器，并告知 QEMU 等待 GDB 连接。

* 在第二个终端中运行：

  .. code-block:: console

    idf.py gdb

  此命令可开启交互式 GDB 会话，并将其连接到 QEMU。现在可以调试应用程序，并在第一个终端中看见控制台输出。

也可以在不使用 IDF 监视器的情况下运行 QEMU：

.. code-block:: console

    idf.py qemu

使用该命令不会启动 IDF 监视器，可直接与 QEMU 进程进行交互。按下 Ctrl-A 快捷键，可以在模拟的 UART 控制台和 QEMU 控制台（“QEMU 监视器”）之间切换。例如，想要退出 QEMU，可以使用 Ctrl-A 快捷键，然后键入 ``q`` 并按 Enter 键。还可以使用 QEMU 控制台输入命令，例如可用于检查寄存器和内存。

图形支持
~~~~~~~~

QEMU 支持虚拟帧缓冲设备。帧缓冲设备在真实的 {IDF_TARGET_NAME} 硬件中并不存在，但可以用于在 QEMU 中测试图形应用程序。

请使用以下命令，启动带有虚拟帧缓冲设备的 QEMU：

.. code-block:: console

    idf.py qemu --graphics monitor

当启用 ``--graphics`` 选项时，QEMU 会打开一个额外的窗口，显示帧缓冲内容。

要在应用程序中使用虚拟帧缓冲设备，可以将 `espressif/esp_lcd_qemu_rgb <https://components.espressif.com/components/espressif/esp_lcd_qemu_rgb>`_ 组件添加到项目中。此组件为虚拟帧缓冲设备提供了一个与 esp_lcd 兼容的驱动程序。

eFuse 仿真
~~~~~~~~~~~

QEMU 支持 eFuse 的仿真，可用来测试安全启动和 flash 加密等与安全相关的功能，而无需在真实硬件上执行不可逆操作。

使用 :doc:`idf.py<idf-py>` eFuse 相关命令来编程 eFuse。当这些命令与 ``qemu`` 命令一起运行时，eFuse 会在 QEMU 中编程，并且 ``qemu_efuse.bin`` 文件会更新。例如，

.. code-block:: console

    idf.py qemu efuse-burn FLASH_CRYPT_CNT 1

默认情况下，eFuse 的值从编译文件夹里的 ``qemu_efuse.bin`` 文件中读取和写入。也可以使用 ``--efuse-file`` 选项指定不同的文件。例如，

.. code-block:: console

    idf.py qemu --efuse-file my_efuse.bin efuse-burn FLASH_CRYPT_CNT 1
    idf.py qemu --efuse-file my_efuse.bin monitor
