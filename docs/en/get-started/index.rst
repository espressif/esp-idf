***********
Get Started
***********

:link_to_translation:`zh_CN:[中文]`

.. Please keep README.md in sync with these instructions.

This document is intended to help you set up the software development environment for the hardware based on the {IDF_TARGET_NAME} chip by Espressif. After that, a simple example will show you how to use ESP-IDF (Espressif IoT Development Framework) for menu configuration, then for building and flashing firmware onto an {IDF_TARGET_NAME} board.

.. include-build-file:: inc/version-note.inc

Introduction
============

{IDF_TARGET_NAME} is a system on a chip that integrates the following features:

.. only:: esp32

    * Wi-Fi (2.4 GHz band)
    * Bluetooth
    * Dual high performance Xtensa® 32-bit LX6 CPU cores
    * Ultra Low Power co-processor
    * Multiple peripherals

.. only:: esp32s2

    * Wi-Fi (2.4 GHz band)
    * High performance single core Xtensa® 32-bit LX7 CPU
    * Ultra Low Power co-processor running either RISC-V or FSM core
    * Multiple peripherals
    * Built-in security hardware
    * USB OTG interface

.. only:: esp32s3

    * Wi-Fi (2.4 GHz band)
    * Bluetooth Low Energy
    * Dual high performance Xtensa® 32-bit LX7 CPU cores
    * Ultra Low Power co-processor running either RISC-V or FSM core
    * Multiple peripherals
    * Built-in security hardware
    * USB OTG interface
    * USB Serial/JTAG Controller

.. only:: esp32c3

    * Wi-Fi (2.4 GHz band)
    * Bluetooth Low Energy
    * High performance 32-bit RISC-V single-core processor
    * Multiple peripherals
    * Built-in security hardware

.. only:: esp32c2

    * Wi-Fi (2.4 GHz band)
    * Bluetooth Low Energy
    * High performance 32-bit RISC-V single-core processor
    * Multiple peripherals
    * Intended for simple, high-volume IoT applications

.. only:: esp32c6

    * Wi-Fi 6 (2.4 GHz band)
    * Bluetooth Low Energy
    * 802.15.4 Thread/Zigbee
    * High performance 32-bit RISC-V single-core processor
    * Multiple peripherals
    * Built-in security hardware

.. only:: esp32h2

    * Bluetooth Low Energy
    * 802.15.4 Thread/Zigbee
    * High performance 32-bit RISC-V single-core processor
    * Multiple peripherals
    * Built-in security hardware

Powered by 40 nm technology, {IDF_TARGET_NAME} provides a robust, highly integrated platform, which helps meet the continuous demands for efficient power usage, compact design, security, high performance, and reliability.

Espressif provides basic hardware and software resources to help application developers realize their ideas using the {IDF_TARGET_NAME} series hardware. The software development framework by Espressif is intended for development of Internet-of-Things (IoT) applications with Wi-Fi, Bluetooth, power management and several other system features.

What You Need
=============

Hardware
~~~~~~~~

* An **{IDF_TARGET_NAME}** board.
* **USB cable** - USB A / micro USB B.
* **Computer** running Windows, Linux, or macOS.

.. note:: Currently, some of the development boards are using USB Type C connectors. Be sure you have the correct cable to connect your board!

If you have one of {IDF_TARGET_NAME} official development boards listed below, you can click on the link to learn more about the hardware.

.. only:: esp32

    .. toctree::
        :maxdepth: 1

        ESP32-DevKitC <../hw-reference/esp32/get-started-devkitc>
        ESP-WROVER-KIT <../hw-reference/esp32/get-started-wrover-kit>
        ESP32-PICO-KIT <../hw-reference/esp32/get-started-pico-kit>
        ESP32-Ethernet-Kit <../hw-reference/esp32/get-started-ethernet-kit>
        ESP32-DevKit-S(-R) <../hw-reference/esp32/user-guide-devkits-r-v1.1>
        ESP32-PICO-KIT-1 <../hw-reference/esp32/get-started-pico-kit-1>
        ESP32-PICO-DevKitM-2 <../hw-reference/esp32/get-started-pico-devkitm-2>
        ESP32-DevKitM-1 <../hw-reference/esp32/user-guide-devkitm-1>

.. only:: esp32s2

    .. toctree::
        :maxdepth: 1

        ESP32-S2-Saola-1 <../hw-reference/esp32s2/user-guide-saola-1-v1.2>
        ESP32-S2-DevKitM-1 <../hw-reference/esp32s2/user-guide-devkitm-1-v1>
        ESP32-S2-DevKitC-1 <../hw-reference/esp32s2/user-guide-s2-devkitc-1>
        ESP32-S2-Kaluga-Kit <../hw-reference/esp32s2/user-guide-esp32-s2-kaluga-1-kit>

.. only:: esp32c3

    .. toctree::
        :maxdepth: 1

        ESP32-C3-DevKitM-1 <../hw-reference/esp32c3/user-guide-devkitm-1>
        ESP32-C3-DevKitC-02 <../hw-reference/esp32c3/user-guide-devkitc-02>


.. only:: esp32s3

    .. toctree::
        :maxdepth: 1

        ESP32-S3-DevKitC-1 <../hw-reference/esp32s3/user-guide-devkitc-1>
        ESP32-S3-DevKitM-1 <../hw-reference/esp32s3/user-guide-devkitm-1>

.. only:: esp32c2

    .. toctree::
        :maxdepth: 1

        ESP8684-DevKitM-1 <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp8684/esp8684-devkitm-1/index.html>

.. only:: esp32c6

    .. toctree::
        :maxdepth: 1

        ESP32-C6-DevKitC-1 <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitc-1/index.html>
        ESP32-C6-DevKitM-1 <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitm-1/index.html>

.. _get-started-get-prerequisites:

Software
~~~~~~~~

To start using ESP-IDF on **{IDF_TARGET_NAME}**, install the following software:

    * **Toolchain** to compile code for {IDF_TARGET_NAME}
    * **Build tools** - CMake and Ninja to build a full **Application** for {IDF_TARGET_NAME}
    * **ESP-IDF** that essentially contains API (software libraries and source code) for {IDF_TARGET_NAME} and scripts to operate the **Toolchain**

.. figure:: ../../_static/what-you-need.png
    :align: center
    :alt: Development of applications for {IDF_TARGET_NAME}
    :figclass: align-center

.. _get-started-step-by-step:
.. _get-started-how-to-get-esp-idf:

Installation
============

To install all the required software, we offer some different ways to facilitate this task. Choose from one of the available options.

IDE
~~~

.. note:: We highly recommend installing the ESP-IDF through your favorite IDE.

* `Eclipse Plugin <https://github.com/espressif/idf-eclipse-plugin/blob/master/README.md>`_
* `VSCode Extension <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md>`_

Manual Installation
~~~~~~~~~~~~~~~~~~~

For the manual procedure, please select according to your operating system.

.. toctree::
    :maxdepth: 1

    Windows Installer <windows-setup>
    Linux and macOS <linux-macos-setup>

Build Your First Project
========================

If you already have the ESP-IDF installed and are not using an IDE, you can build your first project from the command line following the :ref:`Start a Project on Windows <get-started-windows-first-steps>` or :ref:`Start a Project on Linux and macOS <get-started-linux-macos-first-steps>`.

.. _Stable version: https://docs.espressif.com/projects/esp-idf/en/stable/

Uninstall ESP-IDF
=================

If you want to remove ESP-IDF, please follow :ref:`idf-tools-uninstall`.
