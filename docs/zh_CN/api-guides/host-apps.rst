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

注意，尽管名称中包含 POSIX/Linux，但目前的 FreeRTOS POSIX/Linux 模拟器也支持在 macOS 系统中运行。在主机上运行 ESP-IDF 应用程序通常用于测试，但模拟环境和模拟依赖文件并不能完全代表目标设备。因此，仍然需要在目标设备上测试，此时测试的侧重点通常在集成和系统测试上。

.. note::

    在主机上运行应用程序的另一方法是使用 QEMU 模拟器，但 ESP-IDF 的 QEMU 模拟器仍在开发中，尚未发布相关文档。

基于 CMock 的模拟
^^^^^^^^^^^^^^^^^^^^^^^^^^^

该方法使用 `CMock <https://www.throwtheswitch.org/cmock>`_ 框架解决缺少硬件和软件依赖文件的问题。在主机上运行基于 CMock 的应用程序具备一大优势：在主机上运行的应用程序通常只编译必要代码，即模拟了最主要代码的依赖文件，而非编译整个系统。有关 Mocks 的总体介绍及其在 ESP-IDF 的配置和使用，请参阅 :ref:`mocks`。


POSIX/Linux 模拟器的模拟
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF 已支持使用 `FreeRTOS POSIX/Linux 模拟器 <https://www.freertos.org/FreeRTOS-simulator-for-Linux.html>`_ 预览应用程序在目标芯片上的运行效果。使用该模拟器可以在主机上运行 ESP-IDF 组件，并使这类组件可用于在主机上运行的 ESP-IDF 应用程序。目前，只有一部分组件可以在 Linux 上构建。此外，各组件移植到 Linux 上后，其功能可能也会受到限制，或与在芯片目标上构建该组件的功能有所不同。有关所需组件在 Linux 上是否受支持的更多信息，请参阅 :ref:`component-linux-mock-support`。

注意，该模拟器在控制和中断线程时大量依赖于 POSIX 信号和信号处理程序。因此，它具有以下 *限制*：

.. list::
    - 避免使用不是 *async-signal-safe* 的函数，例如 ``printf()``。特别是，在多个优先级不同的任务中调用这些函数可能会导致崩溃和死锁。
    - 不是由 FreeRTOS API 函数创建的线程，禁止从中调用任何 FreeRTOS 原语。
    - 在 FreeRTOS 模拟器中，如果一个任务使用了像 ``select()`` 这样的原生阻塞或等待机制，模拟器可能会错误地将这些任务视为处于 *就绪状态*，然后尝试调度它们执行。实际上，这些任务可能仍然处于阻塞状态。FreeRTOS 对于那些使用了 FreeRTOS API 而被阻塞的任务，调度器只能识别出 *等待状态*。
    - 当一个模拟的 FreeRTOS 任务调用可能被信号中断的 API 时，这些 API 将持续接收模拟的 FreeRTOS 时钟中断。因此，调用这些 API 的代码应设计为能够处理潜在的中断信号，或者通过链接器进行 API 的包装处理。

由于测试和开发过程会受到这些限制影响，我们期望寻找到更好的解决方案用于在主机上运行 ESP-IDF 应用程序。

此外，请注意，如果您使用的是 ESP-IDF 中的 FreeRTOS 模拟组件（``tools/mocks/freertos``），这些限制不会影响程序运行。但是，该模拟组件也无法执行任何调度。

.. only:: not esp32p4 and not esp32h4

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

问题排查
---------------

应用程序是为主机编译的，因而可以用主机上的所有工具进行调试。例如，在 Linux 上可以使用 `GDB <https://man7.org/linux/man-pages/man1/gdb.1.html>`_ 和 `Valgrind <https://linux.die.net/man/1/valgrind>`_。在没有连接调试器的情况下，定制了分段错误和中止信号的处理程序，为用户打印出更多信息，并提高与 ESP-IDF 工具的兼容性。

.. note::

  以下功能绝不是应用程序在调试器中运行的替代方案。它们仅用于提供一些补充信息，例如，当在 CI/CD 系统中运行测试时，只收集应用程序日志。在大多数情况下，要追踪实际问题，请使用调试器复现问题。调试器会更加方便，例如，您无需将地址转换为行号。

分段错误
^^^^^^^^^^^^^^^

在 Linux 上，应用程序遇到分段错误时，会打印错误信息和基本的回溯。这些信息可以用于查找源代码中发生问题的行号。以下是 Hello-World 应用程序中发生分段错误的示例：

.. code-block::

  ...
  Hello world!
  ERROR: Segmentation Fault, here's your backtrace:
  path/to/esp-idf/examples/get-started/hello_world/build/hello_world.elf(+0x2d1b)[0x55d3f636ad1b]
  /lib/x86_64-linux-gnu/libc.so.6(+0x3c050)[0x7f49f0e00050]
  path/to/esp-idf/examples/get-started/hello_world/build/hello_world.elf(+0x6198)[0x55d3f636e198]
  path/to/esp-idf/examples/get-started/hello_world/build/hello_world.elf(+0x5909)[0x55d3f636d909]
  path/to/esp-idf/examples/get-started/hello_world/build/hello_world.elf(+0x2c93)[0x55d3f636ac93]
  path/to/esp-idf/examples/get-started/hello_world/build/hello_world.elf(+0x484e)[0x55d3f636c84e]
  /lib/x86_64-linux-gnu/libc.so.6(+0x89134)[0x7f49f0e4d134]
  /lib/x86_64-linux-gnu/libc.so.6(+0x1097dc)[0x7f49f0ecd7dc]

注意，这些地址 (``+0x...``) 是相对的二进制地址，仍然需要转换为源代码行号（见下文）。

另外，回溯信息是由信号处理程序生成的，从回溯信息的第三行开始，才是问题发生的的堆栈帧，而最上面的两个堆栈帧不是导致错误的代码部分所以不重要。

.. code-block::

  path/to/esp-idf/examples/get-started/hello_world/build/hello_world.elf(+0x6198)[0x55d3f636e198]

要检索源代码中的实际行，需要使用文件名和相对地址（如，此例中的 ``+0x6198``）来调用工具 ``addr2line``：

.. code-block::

  $ addr2line -e path/to/esp-idf/examples/get-started/hello_world/build/hello_world.elf +0x6198
  path/to/esp-idf/components/esp_hw_support/port/linux/chip_info.c:13

现在，请使用主机上可用的详细调试工具进一步追踪问题。
有关 ``addr2line`` 及其调用方法的更多信息，请参见 `addr2line 手册页 <https://linux.die.net/man/1/addr2line>`_。

异常中止
^^^^^^^^^^^^

一旦调用了 ``abort()``，将打印以下行：

.. code-block::

  ERROR: Aborted

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
   * - esp_app_format
     - 否
     - 是
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
