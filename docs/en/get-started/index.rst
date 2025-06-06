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

.. only:: esp32c5

    * Wi-Fi 6 (2.4 and 5 GHz dual-band)
    * Bluetooth Low Energy
    * 802.15.4 Thread/Zigbee
    * High performance 32-bit RISC-V single-core processor
    * Multiple peripherals
    * Built-in security hardware

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

.. only:: esp32p4

    * High performance MCU with RISC-V dual-core processors
    * Powerful image and voice processing capability
    * Single-precision FPU and AI extensions
    * Rich set of peripherals including MIPI, USB, SDIO, and Ethernet
    * Built-in security hardware


.. only:: esp32p4

    Powered by 40 nm technology, {IDF_TARGET_NAME} offers excellent power efficiency, security, and reliability, making it suitable for a wide range of application scenarios and power consumption requirements.


.. only:: not esp32p4

    Powered by 40 nm technology, {IDF_TARGET_NAME} offers excellent power efficiency, RF performance, security, and reliability, making it suitable for a wide range of application scenarios and power consumption requirements.


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

        ESP32-DevKitC <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/index.html>
        ESP32-DevKitM-1 <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitm-1/index.html>
        ESP-WROVER-KIT <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp-wrover-kit/index.html>
        ESP32-PICO-KIT <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-pico-kit/index.html>
        ESP32-Ethernet-Kit <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-ethernet-kit/index.html>
        ESP32-PICO-KIT-1 <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-pico-kit-1/index.html>
        ESP32-PICO-DevKitM-2 <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-pico-devkitm-2/index.html>

.. only:: esp32s2

    .. toctree::
        :maxdepth: 1

        ESP32-S2-Saola-1 <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s2/esp32-s2-saola-1/index.html>
        ESP32-S2-DevKitM-1 <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s2/esp32-s2-devkitm-1/index.html>
        ESP32-S2-DevKitC-1 <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s2/esp32-s2-devkitc-1/index.html>
        ESP32-S2-Kaluga-Kit <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s2/esp32-s2-kaluga-1/index.html>

.. only:: esp32c3

    .. toctree::
        :maxdepth: 1

        ESP32-C3-DevKitM-1 <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32c3/esp32-c3-devkitm-1/index.html>
        ESP32-C3-DevKitC-02 <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32c3/esp32-c3-devkitc-02/index.html>


.. only:: esp32s3

    .. toctree::
        :maxdepth: 1

        ESP32-S3-DevKitC-1 <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/index.html>
        ESP32-S3-DevKitM-1 <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitm-1/index.html>

.. only:: esp32c2

    .. toctree::
        :maxdepth: 1

        ESP8684-DevKitM-1 <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp8684/esp8684-devkitm-1/index.html>

.. only:: esp32c5

    .. toctree::
        :maxdepth: 1

        ESP32-C5-DevKitC-1 <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c5/esp32-c5-devkitc-1/index.html>

.. only:: esp32c6

    .. toctree::
        :maxdepth: 1

        ESP32-C6-DevKitC-1 <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitc-1/index.html>
        ESP32-C6-DevKitM-1 <https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitm-1/index.html>

.. only:: esp32p4

    .. toctree::
        :maxdepth: 1

        ESP32-P4-Function-EV-Board <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-function-ev-board/index.html>
        ESP32-P4-EYE <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-eye/index.html>

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
* `VSCode Extension <https://github.com/espressif/vscode-esp-idf-extension/blob/master/README.md>`_

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
