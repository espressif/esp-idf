Running ESP-IDF Applications on Host
====================================

:link_to_translation:`zh_CN:[中文]`

.. note::

    Running ESP-IDF applications on host is currently still an experimental feature, thus there is no guarantee for API stability. However, user feedback via the `ESP-IDF GitHub repository <https://github.com/espressif/esp-idf>`_ or the `ESP32 forum <https://esp32.com/>`_ is highly welcome, and may help influence the future of design of the ESP-IDF host-based applications.

This document provides an overview of the methods to run ESP-IDF applications on Linux, and what type of ESP-IDF applications can typically be run on Linux.

Introduction
------------

Typically, an ESP-IDF application is built (cross-compiled) on a host machine, uploaded (i.e., flashed) to an ESP chip for execution, and monitored by the host machine via a UART/USB port. However, execution of an ESP-IDF application on an ESP chip can be limiting in various development/usage/testing scenarios.

Therefore, it is possible for an ESP-IDF application to be built and executed entirely within the same Linux host machine (henceforth referred to as "running on host"). Running ESP-IDF applications on host has several advantages:

- No need to upload to a target.
- Faster execution on a host machine, compared to running on an ESP chip.
- No requirements for any specific hardware, except the host machine itself.
- Easier automation and setup for software testing.
- Large number of tools for code and runtime analysis, e.g., Valgrind.

A large number of ESP-IDF components depend on chip-specific hardware. These hardware dependencies must be mocked or simulated when running on host. ESP-IDF currently supports the following mocking and simulation approaches:

1. Using the `FreeRTOS POSIX/Linux simulator <https://www.freertos.org/FreeRTOS-simulator-for-Linux.html>`_ that simulates FreeRTOS scheduling. On top of this simulation, other APIs are also simulated or implemented when running on host.
2. Using `CMock <https://www.throwtheswitch.org/cmock>`_ to mock all dependencies and run the code in complete isolation.

Note that despite the name, the FreeRTOS POSIX/Linux simulator currently also works on macOS. Running ESP-IDF applications on host machines is often used for testing. However, simulating the environment and mocking dependencies does not fully represent the target device. Thus, testing on the target device is still necessary, though with a different focus that usually puts more weight on integration and system testing.

.. note::

    Another possibility to run applications on the host is to use the QEMU simulator. However, QEMU development for ESP-IDF applications is still a work in progress and has not been documented yet.

CMock-Based Approach
^^^^^^^^^^^^^^^^^^^^

This approach uses the `CMock <https://www.throwtheswitch.org/cmock>`_ framework to solve the problem of missing hardware and software dependencies. CMock-based applications running on the host machine have the added advantage that they usually only compile the necessary code, i.e., the (mostly mocked) dependencies instead of the entire system. For a general introduction to Mocks and how to configure and use them in ESP-IDF, please refer to :ref:`mocks`.


POSIX/Linux Simulator Approach
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The `FreeRTOS POSIX/Linux simulator <https://www.freertos.org/FreeRTOS-simulator-for-Linux.html>`_ is available on ESP-IDF as a preview target already. This simulator allows ESP-IDF components to be implemented on the host, making them accessible to ESP-IDF applications when running on host. Currently, only a limited number of components are ready to be built on Linux. Furthermore, the functionality of each component ported to Linux may also be limited or different compared to the functionality when building that component for a chip target. For more information about whether the desired components are supported on Linux, please refer to :ref:`component-linux-mock-support`.

Note that this simulator relies heavily on POSIX signals and signal handlers to control and interrupt threads. Hence, it has the following *limitations*:

.. list::
    - Functions that are not *async-signal-safe*, e.g. ``printf()``, should be avoided. In particular, calling them from different tasks with different priority can lead to crashes and deadlocks.
    - Calling any FreeRTOS primitives from threads not created by FreeRTOS API functions is forbidden.
    - FreeRTOS tasks using any native blocking/waiting mechanism (e.g., ``select()``), may be perceived as *ready* by the simulated FreeRTOS scheduler and therefore may be scheduled, even though they are actually blocked. This is because the simulated FreeRTOS scheduler only recognizes tasks blocked on any FreeRTOS API as *waiting*.
    - APIs that may be interrupted by signals will continually receive the signals simulating FreeRTOS tick interrupts when invoked from a running simulated FreeRTOS task. Consequently, code that calls these APIs should be designed to handle potential interrupting signals or the API needs to be wrapped by the linker.

Since these limitations are not very practical, in particular for testing and development, we are currently evaluating if we can find a better solution for running ESP-IDF applications on the host machine.

Note furthermore that if you use the ESP-IDF FreeRTOS mock component (``tools/mocks/freertos``), these limitations do not apply. But that mock component will not do any scheduling, either.

.. only:: not esp32p4 and not esp32h4

    .. note::

        The FreeRTOS POSIX/Linux simulator allows configuring the :ref:`amazon_smp_freertos` version. However, the simulation still runs in single-core mode. The main reason allowing Amazon SMP FreeRTOS is to provide API compatibility with ESP-IDF applications written for Amazon SMP FreeRTOS.

Requirements for Using Mocks
----------------------------

.. include:: inc/linux-host-requirements.rst

If any mocks are used, then ``Ruby`` is required, too.

Build and Run
-------------

To build the application on Linux, the target has to be set to ``linux`` and then it can be built and run:

.. code-block:: bash

  idf.py --preview set-target linux
  idf.py build
  idf.py monitor

Troubleshooting
---------------

Since the applications are compiled for the host, they can be debugged with all the tools available on the host. E.g., this could be `GDB <https://man7.org/linux/man-pages/man1/gdb.1.html>`_ and `Valgrind <https://linux.die.net/man/1/valgrind>`_ on Linux. For cases where no debugger is attached, the segmentation fault and Abort signal handlers are customized to print additional information to the user and to increase compatibility with the ESP-IDF tools.

.. note::

  The following features are by no means a replacement for running the application in a debugger. It is only meant to give some additional information, e.g., if a battery of tests runs on Linux in a CI/CD system where only the application logs are collected. To trace down the actual issue in most cases, you will need to reproduce it with a debugger attached. A debugger is much more convenient too, because, for example, you do not need to convert addresses to line numbers.

Segmentation Faults
^^^^^^^^^^^^^^^^^^^

On Linux, applications prints an error message and a rudimentary backtrace once it encounters a segmentation fault. This information can be used to find the line numbers in the source code where the issue occurred. The following is an example of a segmentation fault in the Hello-World application:

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

Note that the addresses (``+0x...``) are relative binary addresses, which still need to be converted to the source code line numbers (see below).

Note furthermore that the backtrace is created from the signal handler, which means that the two uppermost stack frames are not of interest. Instead, the third line is the uppermost stack frame where the issue occurred:

.. code-block::

  path/to/esp-idf/examples/get-started/hello_world/build/hello_world.elf(+0x6198)[0x55d3f636e198]

To retrieve the actual line in the source code, we need to call the tool ``addr2line`` with the file name and the relative address (in this case ``+0x6198``):

.. code-block::

  $ addr2line -e path/to/esp-idf/examples/get-started/hello_world/build/hello_world.elf +0x6198
  path/to/esp-idf/components/esp_hw_support/port/linux/chip_info.c:13

From here on, you should use elaborate debugging tools available on the host to further trace the issue down.
For more information on ``addr2line`` and how to call it, see the `addr2line man page <https://linux.die.net/man/1/addr2line>`_.

Aborts
^^^^^^

Once ``abort()`` has been called, the following line is printed:

.. code-block::

  ERROR: Aborted

.. _component-linux-mock-support:

Component Linux/Mock Support Overview
-------------------------------------

Note that any "Yes" here does not necessarily mean a full implementation or mocking. It can also mean a partial implementation or mocking of functionality. Usually, the implementation or mocking is done to a point where enough functionality is provided to build and run a test application.

.. list-table::
   :header-rows: 1
   :widths: 20 10 10

   * - Component
     - Mock
     - Simulation
   * - cmock
     - No
     - Yes
   * - driver
     - Yes
     - No
   * - esp_app_format
     - No
     - Yes
   * - esp_common
     - No
     - Yes
   * - esp_event
     - Yes
     - Yes
   * - esp_http_client
     - No
     - Yes
   * - esp_http_server
     - No
     - Yes
   * - esp_https_server
     - No
     - Yes
   * - esp_hw_support
     - Yes
     - Yes
   * - esp_netif
     - Yes
     - Yes
   * - esp_netif_stack
     - No
     - Yes
   * - esp_partition
     - Yes
     - Yes
   * - esp_rom
     - No
     - Yes
   * - esp_system
     - No
     - Yes
   * - esp_timer
     - Yes
     - No
   * - esp_tls
     - Yes
     - Yes
   * - fatfs
     - No
     - Yes
   * - freertos
     - Yes
     - Yes
   * - hal
     - No
     - Yes
   * - heap
     - No
     - Yes
   * - http_parser
     - Yes
     - Yes
   * - json
     - No
     - Yes
   * - linux
     - No
     - Yes
   * - log
     - No
     - Yes
   * - lwip
     - Yes
     - Yes
   * - mbedtls
     - No
     - Yes
   * - mqtt
     - No
     - Yes
   * - nvs_flash
     - No
     - Yes
   * - partition_table
     - No
     - Yes
   * - protobuf-c
     - No
     - Yes
   * - pthread
     - No
     - Yes
   * - soc
     - No
     - Yes
   * - spiffs
     - No
     - Yes
   * - spi_flash
     - Yes
     - No
   * - tcp_transport
     - Yes
     - No
   * - unity
     - No
     - Yes
