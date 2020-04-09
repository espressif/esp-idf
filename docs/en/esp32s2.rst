ESP32-S2 Preview Support
========================

The current master branch of ESP-IDF (pre-v4.2) contains preview support for ESP32-S2.

..note: Support for the ESP32-S2beta chip is included in ESP-IDF v4.1 as a preview only. In ESP-IDF v4.2 (this version) it has been replaced by ESP32-S2 (non-beta) chip support.

Software support and documentation for ESP32-S2 chip is still being developed. See :ref:`esp32s2-limitations` section below.

Documentation
^^^^^^^^^^^^^

You are viewing the version of the ESP-IDF Programming Guide that describes ESP32-S2 support. However, as this version is still under development then there may be some changes from ESP32 that are not yet correctly documented here. If you find documentation that needs updating, please use the "Provide feedback about this document" link at the bottom of the page to tell us about it.

Setting up for ESP32-S2
^^^^^^^^^^^^^^^^^^^^^^^

To start with, follow the :doc:`/get-started/index` guide to configure the CMake-based build system for ESP32.

When running ``install.sh`` or ``install.bat`` as part of the Get Started guide, the ESP32-S2 toolchain will be automatically downloaded and added to the IDF Tools directory. If you had already run ``install.sh`` or ``install.bat`` before updating to a version with ESP32-S2 support, you may need to re-run it.

The toolchain tuple is `xtensa-esp32s2-elf-`, for example the GCC compiler for this target is named `xtensa-esp32s2-elf-gcc`.

Manual toolchain URLs
---------------------

If not using ``install.sh`` or ``install.bat``, you can download the toolchain manually. Download links can be found in the :ref:`idf-tools-list`, under the name ``xtensa-esp32s2-elf``.

If installing the toolchain manually, unpack it somewhere and add the ``bin`` subdirectory to your `PATH`.

Building a project for ESP32-S2
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. important:: Not all ESP-IDF examples support ESP32-S2.

   For some examples this is because required hardware is not included in ESP32-S2 so it cannot be supported. For some examples this is because the example has not yet been updated for ESP32-S2.

   If building an example, please check the README file for the ``Supported Targets`` table. If no ESP32-S2 listed, the example won't work on ESP32-S2.

.. highlight:: bash

Before building an ESP-IDF project, run the following command to switch the target to ESP32-S2::

  idf.py set-target esp32s2

.. important:: Running ``set-target`` will clear the project configuration and create a new empty ``sdkconfig`` file for the project. The old configuration will be saved in ``sdkconfig.old``.

See :ref:`selecting-idf-target` for more details about switching between different targets (esp32, esp32s2).

Then configure the project::

  idf.py menuconfig

Build and flash the project as usual, for example::

  idf.py -p PORT flash monitor

Consult the :doc:`/get-started/index` guide for more details on building and flashing an ESP-IDF project.

.. note:: It is possible to set the project target back to ESP32 by running ``idf.py set-target esp32``. This also clears the project configuration.

.. _esp32s2-limitations:

Limitations
^^^^^^^^^^^

ESP32-S2 support is currently a preview and does not support all features. Espressif firmware developers are working hard on the remaining support, so please update the ``master`` branch regularly to receive the latest code.

- The integrated USB OTG peripheral is not supported
- Documentation not updated for ESP32-S2 in most places
- Peripheral drivers and power saving modes are a work in progress
- No hardware security features or cryptographic accelerator support
- Time-of-Flight (TOF) measurements with normal Wi-Fi packets is not supported

The up-to-date list of features supported on ESP32-S2 is maintained at `in this forum post <https://esp32.com/viewtopic.php?f=10&p=56361>`_.

Issues
^^^^^^

If you find issues which are not mentioned in :ref:`esp32s2-limitations`, please `open an issue on GitHub <https://github.com/espressif/esp-idf/issues>`_.

Please make sure to mention that you are using the ESP32-S2 chip and give your exact version of ESP-IDF.
