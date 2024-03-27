在主机上运行 ESP-IDF 应用程序
===============================

:link_to_translation:`en:[English]`

.. note::

    在主机上运行 ESP-IDF 应用程序的功能仍处于试验阶段，无法保证 API 的稳定性。欢迎用户通过 `ESP-IDF GitHub 仓库 <https://github.com/espressif/esp-idf>`_ 或 `ESP32 论坛 <https://esp32.com/>`_ 提供反馈意见，助力未来 ESP-IDF 基于主机的应用程序设计。

本文档概述了在 Linux 上运行 ESP-IDF 应用程序的方法，并介绍了可以在 Linux 上运行的常见 ESP-IDF 应用程序类型。

介绍
------------

ESP-IDF 应用程序通常在主机上进行构建（交叉编译），然后上传（即烧录）到 ESP 芯片上运行，并由主机通过 UART/USB 端口监控。然而，在 ESP 芯片上运行 ESP-IDF 应用程序在各种开发/使用/测试场景下可能存在限制。

因此，ESP-IDF 支持在同一台 Linux 主机上构建和运行其应用程序。在主机上运行 ESP-IDF 应用程序具有以下几个优点：

- 无需上传到目标芯片。
- 应用程序在主机上的运行速度比在 ESP 芯片上快。
- 除主机本身外，无特定硬件要求。
- 更易进行软件测试的自动化和设置。
- 提供大量用于代码和运行分析的工具，如 Valgrind。

许多 ESP-IDF 组件依赖支持特定芯片的硬件，因此在主机上运行应用程序时，必须对这些硬件依赖文件进行模拟或仿真。目前，ESP-IDF 支持以下模拟和仿真方法：

1. 使用 `FreeRTOS POSIX/Linux 模拟器 <https://www.freertos.org/FreeRTOS-simulator-for-Linux.html>`_ 可以模拟 FreeRTOS 调度。在此模拟的基础上，在主机上运行应用程序时也会模拟或使用其他 API。
2. 使用 `CMock <https://www.throwtheswitch.org/cmock>`_ 可以模拟所有依赖文件，并在完全独立的情况下运行代码。

原则上，这两种方法（POSIX/Linux 模拟器和使用 CMock 模拟）可以混用，但此功能在 ESP-IDF 中尚未实现。注意，尽管名称中包含 POSIX/Linux，但目前的 FreeRTOS POSIX/Linux 模拟器也支持在 macOS 系统中运行。在主机上运行 ESP-IDF 应用程序通常用于测试，但模拟环境和模拟依赖文件并不能完全代表目标设备。因此，仍然需要在目标设备上测试，此时测试的侧重点通常在集成和系统测试上。

.. note::

    在主机上运行应用程序的另一方法是使用 QEMU 模拟器，但 ESP-IDF 的 QEMU 模拟器仍在开发中，尚未发布相关文档。

基于 CMock 的模拟
^^^^^^^^^^^^^^^^^^^^^^^^^^^

该方法使用 `CMock <https://www.throwtheswitch.org/cmock>`_ 框架解决缺少硬件和软件依赖文件的问题。在主机上运行基于 CMock 的应用程序具备一大优势：在主机上运行的应用程序通常只编译必要代码，即模拟了最主要代码的依赖文件，而非编译整个系统。有关 Mocks 的总体介绍及其在 ESP-IDF 的配置和使用，请参阅 :ref:`mocks`。


POSIX/Linux 模拟器的模拟
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF 已支持使用 `FreeRTOS POSIX/Linux 模拟器 <https://www.freertos.org/FreeRTOS-simulator-for-Linux.html>`_ 预览应用程序在目标芯片上的运行效果。使用该模拟器可以在主机上运行 ESP-IDF 组件，并使这类组件可用于在主机上运行的 ESP-IDF 应用程序。目前，只有一部分组件可以在 Linux 上构建。此外，各组件移植到 Linux 上后，其功能可能也会受到限制，或与在芯片目标上构建该组件的功能有所不同。有关所需组件在 Linux 上是否受支持的更多信息，请参阅 :ref:`component-linux-mock-support`。

.. only:: not esp32p4

    .. note::

        FreeRTOS POSIX/Linux 模拟器支持配置 :ref:`amazon_smp_freertos` 版本，但模拟仍在单核模式下运行。支持 Amazon SMP FreeRTOS 主要是为给 Amazon SMP FreeRTOS 编写的 ESP-IDF 应用程序提供 API 兼容性。

使用模拟器的前提
-----------------

.. include:: inc/linux-host-requirements.rst

如果使用了任意模拟器，则应设置变量 ``Ruby``。

构建和运行
-------------

要在 Linux 上构建并运行应用程序，需要将目标芯片设置为 ``linux``：

.. code-block:: bash

  idf.py --preview set-target linux
  idf.py build
  idf.py monitor

.. _component-linux-mock-support:

组件 Linux/Mock 支持情况概述
-------------------------------------

注意，下表中的“是”并不代表完全实现或模拟，它也可以表示功能的部分实现或模拟。实现或模拟通常只进行到可以提供足够功能、可以构建和运行测试应用程序的程度。

.. list-table::
   :header-rows: 1
   :widths: 20 10 10

   * - 组件
     - 模拟
     - 仿真
   * - cmock
     - 否
     - 是
   * - driver
     - 是
     - 否
   * - esp_common
     - 否
     - 是
   * - esp_event
     - 是
     - 是
   * - esp_http_client
     - 否
     - 是
   * - esp_http_server
     - 否
     - 是
   * - esp_https_server
     - 否
     - 是
   * - esp_hw_support
     - 是
     - 是
   * - esp_netif
     - 是
     - 是
   * - esp_netif_stack
     - 否
     - 是
   * - esp_partition
     - 是
     - 否
   * - esp_rom
     - 否
     - 是
   * - esp_system
     - 否
     - 是
   * - esp_timer
     - 是
     - 否
   * - esp_tls
     - 是
     - 否
   * - fatfs
     - 否
     - 是
   * - freertos
     - 是
     - 是
   * - hal
     - 否
     - 是
   * - heap
     - 否
     - 是
   * - http_parser
     - 是
     - 是
   * - json
     - 否
     - 是
   * - linux
     - 否
     - 是
   * - log
     - 否
     - 是
   * - lwip
     - 是
     - 是
   * - mbedtls
     - 否
     - 是
   * - mqtt
     - 否
     - 是
   * - nvs_flash
     - 否
     - 是
   * - partition_table
     - 否
     - 是
   * - protobuf-c
     - 否
     - 是
   * - pthread
     - 否
     - 是
   * - soc
     - 否
     - 是
   * - spiffs
     - 否
     - 是
   * - spi_flash
     - 是
     - 否
   * - tcp_transport
     - 是
     - 否
   * - unity
     - 否
     - 是
