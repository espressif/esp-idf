Running Applications on Host
============================

.. note::
    Running IDF applications on host is currently still an experimental feature, thus there is no guarantee for API stability. However, user feedback via the `ESP-IDF GitHub repository <https://github.com/espressif/esp-idf>`_ or the `ESP32 forum <https://esp32.com/>`_ is highly welcome, and may help influence the future of design of the IDF host-based applications.

This document provides an overview of the methods to run IDF applications on Linux, and what type of IDF applications can typically be run on Linux.

Introduction
---------------------------

Typically, an IDF application is built (cross-compiled) on a host machine, uploaded (i.e., flashed) to an ESP chip for execution, and monitored by the host machine via a UART/USB port. However, execution of an IDF application on an ESP chip (hence forth referred to as "running on target") can be limiting in various development/usage/testing scenarios.

Therefore, it is possible for an IDF application to be built and executed entirely within the same Linux host machine (hence forth referred to as "running on host"). Running ESP-IDF applications on host has several advantages:

- No need to upload to a target.
- Faster execution on a host machine, compared to running on an ESP chip.
- No requirements for any specific hardware, except the host machine itself.
- Easier automation and setup for software testing.
- Large number of tools for code and runtime analysis (e.g. Valgrind).

A large number of IDF components depend on chip-specific hardware. These hardware dependencies must be mocked or simulated when running on host. ESP-IDF currently supports the following mocking and simulation approaches:

1. Using the `FreeRTOS POSIX/Linux simulator <https://www.freertos.org/FreeRTOS-simulator-for-Linux.html>`_ that simulates FreeRTOS scheduling. On top of this simulation, other APIs are also simulated or implemented when running on host.
2. Using `CMock <https://www.throwtheswitch.org/cmock>`_ to mock all dependencies and run the code in complete isolation.

In principle, it is possible to mix both approaches (POSIX/Linux simulator and mocking using CMock), but this has not been done yet in ESP-IDF. Note that despite the name, the FreeRTOS POSIX/Linux simulator currently also works on MacOS. Running IDF applications on host machines is often used for testing. However, simulating the environment and mocking dependencies does not fully represent the target device. Thus, testing on the target device is still necessary, though with a different focus that usually puts more weight on integration and system testing.

.. note::
    Another possibility to run applications on the host is to use the QEMU simulator. However, QEMU development for IDF applications is currently work in progress and has not been documented yet.

CMock-Based Approach
^^^^^^^^^^^^^^^^^^^^

This approach uses the `CMock <https://www.throwtheswitch.org/cmock>`_ framework to solve the problem of missing hardware and software dependencies. CMock-based applications running on the host machine have the added advantage that they usually only compile the necessary code, i.e., the (mostly mocked) dependencies instead of the entire system. For a general introduction to Mocks and how to configure and use them in ESP-IDF, please refer to :ref:`mocks`.


POSIX/Linux Simulator Approach
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The `FreeRTOS POSIX/Linux simulator <https://www.freertos.org/FreeRTOS-simulator-for-Linux.html>`_ is available on ESP-IDF as a preview target already. It is the base for the Linux target which is already available as a preview. Using this simulator, IDF components can be implemented on the host to make them available to IDF applications when running on host. Currently, only a limited number of components are ready to be built on Linux. Furthermore the functionality of each component ported to Linux may also be limited or different compared to the functionality when building that component for a chip target. For more information if the desired components are supported on Linux, please refer to :ref:`component-linux-mock-support`.

Requirements
------------

.. include:: inc/linux-host-requirements.rst

If any mocks are used, then ``Ruby`` is required, too.

Build and Run
-------------

To build the application on Linux, the target has to be set to ``linux`` and then it can be built and run:

.. code-block:: bash

  idf.py --preview set-target linux
  idf.py build
  idf.py monitor

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
   * - driver
     - Yes
     - No
   * - esp_common
     - No
     - Yes
   * - esp_event
     - Yes
     - Yes
   * - esp_hw_support
     - Yes
     - Yes
   * - esp_partition
     - Yes
     - No
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
     - No
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
     - No
   * - log
     - No
     - Yes
   * - lwip
     - Yes
     - No
   * - soc
     - No
     - Yes
   * - spi_flash
     - Yes
     - No
   * - tcp_transport
     - Yes
     - No
