ESP32-S2 Beta Preview Support
=============================

The current master branch of ESP-IDF (pre-V4.1) contains preview support for ESP32-S2 Beta.

The ESP32-S2 Beta chip is not the same as the final ESP32-S2, the Beta chip is a limited engineering sample and as a result not all features are available.

Software support is still being developed ahead of the final ESP32-S2 release (see :ref:`esp32s2-limitations` and :ref:`esp32s2-roadmap` sections below).

Documentation
^^^^^^^^^^^^^

Except where it specifically mentions ESP32-S2, this version of the ESP-IDF Programming Guide is written for ESP32 only. Most software APIs and components are the same or very similar for ESP32-S2, but differences may not be documented yet.

Setting up for ESP32-S2 Beta
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To start with, follow the :doc:`/get-started/index` guide to configure the CMake-based build system for ESP32.

When running ``install.sh`` or ``install.bat`` as part of the Get Started guide, the ESP32-S2 toolchain will be automatically downloaded and added to the IDF Tools directory. If you had already run ``install.sh`` or ``install.bat`` before updating to a version with ESP32-S2 support, you may need to re-run it.

The toolchain tuple is `xtensa-esp32s2-elf-`, for example the GCC compiler for this target is named `xtensa-esp32s2-elf-gcc`.

Manual toolchain URLs
---------------------

If not using ``install.sh`` or ``install.bat``, you can download the manual toolchain manually from here:

- `Windows <https://dl.espressif.com/dl/xtensa-esp32s2-elf-gcc8_2_0-esp-2019r2-win32.zip>`_
- `macOS <https://dl.espressif.com/dl/xtensa-esp32s2-elf-gcc8_2_0-esp-2019r2-macos.tar.gz>`_
- `Linux 64-bit x86 <https://dl.espressif.com/dl/xtensa-esp32s2-elf-gcc8_2_0-esp-2019r2-linux-amd64.tar.gz>`_
- `Linux 32-bit x86 <https://dl.espressif.com/dl/xtensa-esp32s2-elf-gcc8_2_0-esp-2019r2-linux-i686.tar.gz>`_

If installing the toolchain manually, unpack it somewhere and add the ``bin`` subdirectory to your `PATH`.

Building a project for ESP32-S2 Beta
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. important:: Not all ESP-IDF examples support ESP32-S2 Beta.

   For some examples this is because required hardware is not included in ESP32-S2 so it cannot be supported. For some examples this is because the example has not yet been updated for ESP32-S2.

   If building an example, please check the example CMakeLists.txt file for the clause ``SUPPORTED_TARGETS esp32``. If this is present, the example won't work on ESP32-S2.

.. highlight:: bash

Before building an ESP-IDF project, run the following command to switch the target to ESP32-S2 Beta::

  idf.py set-target esp32s2beta

.. important:: Running ``set-target`` will clear the project configuration and create a new empty ``sdkconfig`` file for the project. The old configuration will be saved in ``sdkconfig.old``.

Then configure the project::

  idf.py menuconfig

.. important:: There are 3 versions of ESP32-S2 Beta engineering sample available with different Wi-Fi PHYs. They can be identified by the marking on the chip itself. The project must be configured to match the chip via the project setting "ESP32-S2 Beta chip version" (``CONFIG_ESP32S2_BETA_CHIP_VERSION``). This can be found under Component Config -> ESP32S2-specific. If the project setting doesn't match the chip then Wi-Fi performance may be very poor, or the chip may crash when initializing Wi-Fi.

Build and flash the project as usual, for example::

  idf.py -p PORT flash monitor

Consult the :doc:`/get-started/index` guide for more details on building and flashing an ESP-IDF project.

.. note:: It is possible to set the project target back to ESP32 by running ``idf.py set-target esp32``. This also clears the project configuration.

.. _esp32s2-limitations:

Limitations
^^^^^^^^^^^

ESP32-S2 Beta support is currently a preview and does not support all features. Espressif firmware developers are working hard on the remaining support, so please update the ``master`` branch regularly to receive the latest code.

- The integrated USB OTG peripheral is not supported
- Documentation not updated for ESP32-S2 in most places
- Peripheral drivers are a work in progress
- No power saving modes
- No hardware security features or cryptographic accelerator support
- Time-of-Flight (TOF) measurements with normal Wi-Fi packets is not supported

.. _esp32s2-roadmap:

Roadmap
^^^^^^^

- ESP-IDF V4.1 will support ESP32-S2 Beta chip as a preview with some limitations.
- ESP-IDF V4.2 will support ESP32-S2.

Support for ESP32-S2 Beta will be removed from ESP-IDF once ESP32-S2 is available.


Issues
^^^^^^

If you find issues which are not mentioned in :ref:`esp32s2-limitations`, please `open an issue on GitHub <https://github.com/espressif/esp-idf/issues>`_.

Please make sure to mention that you are using the ESP32-S2 Beta preview support and give your exact version of ESP-IDF.
