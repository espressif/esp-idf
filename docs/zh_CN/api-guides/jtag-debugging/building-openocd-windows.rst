********************************
Windows 环境下从源码编译 OpenOCD
********************************

:link_to_translation:`en:[English]`

.. note::

    本文介绍了如何从 OpenOCD 源文件构建二进制文件。如果你想要更快速地构建，也可以从 `乐鑫 GitHub <https://github.com/espressif/openocd-esp32/releases>`_ 直接下载 OpenOCD 的预构建二进制文件，而无需自己编译（详细信息，请参阅 :ref:`jtag-debugging-setup-openocd`）。

.. note::

    本文涉及的命令行操作均在装有 MINGW32 子系统的 MSYS2 shell 环境中进行了验证。


安装依赖的软件包
================

安装编译 OpenOCD 所需的软件包：

.. code-block:: bash

    pacman -S --noconfirm --needed autoconf automake git make \
    mingw-w64-i686-gcc \
    mingw-w64-i686-toolchain \
    mingw-w64-i686-libtool \
    mingw-w64-i686-pkg-config \
    mingw-w64-cross-winpthreads-git \
    p7zip


下载 OpenOCD 源码
=================

支持 {IDF_TARGET_NAME} 的 OpenOCD 源码可以从乐鑫官方 GitHub 获取，网址为 https://github.com/espressif/openocd-esp32。你可以在 Git 中使用以下命令来拉取源代码：

.. code-block:: bash

    cd ~/esp
    git clone --recursive https://github.com/espressif/openocd-esp32.git

克隆后的源代码保存在 ``~/esp/openocd-esp32`` 目录下。


下载 libusb
================

构建 OpenOCD 需使用 libusb 库。请执行以下命令来下载特定版本的 libusb，并将其解压至当前目录。

.. code-block:: bash

    wget https://github.com/libusb/libusb/releases/download/v1.0.22/libusb-1.0.22.7z
    7z x -olibusb ./libusb-1.0.22.7z

现在需要导出以下变量，以便将 libusb 库与 OpenOCD 构建相关联。

.. code-block:: bash

    export CPPFLAGS="$CPPFLAGS -I${PWD}/libusb/include/libusb-1.0"
    export LDFLAGS="$LDFLAGS -L${PWD}/libusb/MinGW32/.libs/dll"


构建 OpenOCD
============

配置和构建 OpenOCD，请参考以下命令：

.. code-block:: bash

    cd ~/esp/openocd-esp32
    export CPPFLAGS="$CPPFLAGS -D__USE_MINGW_ANSI_STDIO=1 -Wno-error"; export CFLAGS="$CFLAGS -Wno-error"
    ./bootstrap
    ./configure --disable-doxygen-pdf --enable-ftdi --enable-jlink --enable-ulink --build=i686-w64-mingw32 --host=i686-w64-mingw32
    make
    cp ../libusb/MinGW32/dll/libusb-1.0.dll ./src
    cp /opt/i686-w64-mingw32/bin/libwinpthread-1.dll ./src


构建完成后，OpenOCD 的二进制文件将被保存于 ``~/esp/openocd-esp32/src/`` 目录下。

你也可以调用 ``make install``，将其复制到指定位置。

- 你可以在配置 OpenOCD 时指定这一位置，也可以在调用 ``make install`` 前设置 ``export DESTDIR="/custom/install/dir"``。
- 如果你已经安装过其他开发平台的 OpenOCD，请跳过此步骤，否则原来的 OpenOCD 可能会被覆盖。

.. note::

    * 如果发生错误，请解决后再次尝试编译，直到 ``make`` 成功为止。
    * 如果 OpenOCD 存在子模块问题，请 ``cd`` 到 ``openocd-esp32`` 目录，并输入 ``git submodule update --init`` 命令。
    * 如果 ``./configure`` 成功运行，JTAG 被使能的信息会被打印在 ``OpenOCD configuration summary`` 下面。
    * 如果你的设备信息未显示在日志中，请根据 ``../openocd-esp32/doc/INSTALL.txt`` 文中的描述使用 ``./configure`` 启用它。
    * 有关编译 OpenOCD 的详细信息，请参阅 ``openocd-esp32/README.Windows``。
    * 请记得将 `libusb-1.0.dll` 和 `libwinpthread-1.dll` 从 ``~/esp/openocd-esp32/src`` 复制到 `OOCD_INSTALLDIR/bin`。

一旦 ``make`` 过程完成，OpenOCD 的可执行文件会被保存到 ``~/esp/openocd-esp32/src/openocd`` 目录下。


完整编译过程
==============

OpenOCD 编译过程中所调用的所有命令都已包含在以下代码片段中，你可以将其复制到 shell 脚本中，以便快速执行:

.. code-block:: bash

    pacman -S --noconfirm --needed autoconf automake git make mingw-w64-i686-gcc mingw-w64-i686-toolchain mingw-w64-i686-libtool mingw-w64-i686-pkg-config mingw-w64-cross-winpthreads-git p7zip
    cd ~/esp
    git clone --recursive https://github.com/espressif/openocd-esp32.git

    wget https://github.com/libusb/libusb/releases/download/v1.0.22/libusb-1.0.22.7z
    7z x -olibusb ./libusb-1.0.22.7z
    export CPPFLAGS="$CPPFLAGS -I${PWD}/libusb/include/libusb-1.0"; export LDFLAGS="$LDFLAGS -L${PWD}/libusb/MinGW32/.libs/dll"

    export CPPFLAGS="$CPPFLAGS -D__USE_MINGW_ANSI_STDIO=1 -Wno-error"; export CFLAGS="$CFLAGS -Wno-error"
    cd ~/esp/openocd-esp32
    ./bootstrap
    ./configure --disable-doxygen-pdf --enable-ftdi --enable-jlink --enable-ulink --build=i686-w64-mingw32 --host=i686-w64-mingw32
    make
    cp ../libusb/MinGW32/dll/libusb-1.0.dll ./src
    cp /opt/i686-w64-mingw32/bin/libwinpthread-1.dll ./src

    # # optional
    # export DESTDIR="$PWD"
    # make install
    # cp ./src/libusb-1.0.dll $DESTDIR/mingw32/bin
    # cp ./src/libwinpthread-1.dll $DESTDIR/mingw32/bin


下一步
======

想要进一步配置调试环境，请前往 :ref:`jtag-debugging-configuring-target` 章节。
