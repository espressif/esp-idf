.. _esp32s2-modules:

Modules
=======

This is a family of ESP32-S2-based modules with some integrated key components, including a crystal oscillator and an antenna matching circuit. The modules constitute ready-made solutions for integration into final products. If combined with a few extra components, such as a programming interface, bootstrapping resistors, and pin headers, these modules can also be used for evaluation of ESP32-S2's functionality.

The key characteristics of these modules are summarized in the table below. Some additional details are covered in the following sections.

===================  ============  ===========  =========  ====  ===============
Module               Chip          Flash, MB    PSRAM, MB  Ant.  Dimensions, mm
===================  ============  ===========  =========  ====  ===============
ESP32-S2-WROOM-32    ESP32-S2      2            N/A        MIFA  16 x 23 x 3
===================  ============  ===========  =========  ====  ===============

* MIFA - Meandered Inverted-F Antenna
* U.FL - U.FL / IPEX antenna connector


Development Boards
==================

.. _esp-modules-and-boards-esp32s2-kaluga-1-kit:

ESP32-S2-Kaluga-1 Kit v1.3
--------------------------

The ESP32-S2-Kaluga-1 kit is a development kit by Espressif that consists of one main board and several extension boards. This kit is intended to provide users with tools for development of human-computer interaction applications based on the ESP32-S2 chip.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-s2-kaluga-1-v1.2-3d.png
    :align: center
    :width: 2631px
    :height: 1966px
    :scale: 25%
    :alt: ESP32-S2-Kaluga-1
    :figclass: align-center

    ESP32-S2-Kaluga-1 (click to enlarge)


Documentation
^^^^^^^^^^^^^

* :doc:`esp32s2/user-guide-esp32-s2-kaluga-1-kit`

Previous Versions
^^^^^^^^^^^^^^^^^

* :ref:`esp32s2-modules-and-boards-previous-esp32s2-kaluga-1-kit-v1.2`


.. _FTDI Virtual COM Port Drivers: https://www.ftdichip.com/Drivers/VCP.htm
.. _Espressif Products Ordering Information: https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_en.pdf
