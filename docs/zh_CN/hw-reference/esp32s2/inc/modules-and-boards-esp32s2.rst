.. _esp32s2-modules:

模组
====

ESP32-S2 系列模组集成了晶振、天线匹配电路等重要组件，可直接集成到终端产品中。如果再结合一些其他组件，例如编程接口、自举电阻和排针，您就可以体验 ESP32-S2 的强大功能了。

下表总结了上述模组的主要特点，详细信息见后文。

===================  ============  ============  ============  =====  ===============
模组                  芯片           Flash (MB)    PSRAM (MB)   天线    尺寸 (mm)
===================  ============  ============  ============  =====  ===============
ESP32-S2-WROOM-32    ESP32-S2       2             N/A          MIFA    16 x 23 x 3
===================  ============  ============  ============  =====  ===============

* MIFA - 蛇形倒 F 天线
* U.FL - U.FL / IPEX 天线连接器

开发板
======

.. _esp-modules-and-boards-esp32s2-kaluga-1-kit:

ESP32-S2-Kaluga-1 套件 v1.3
------------------------------

ESP32-S2-Kaluga-1 是一款来自乐鑫的开发套件，包含 1 个主板和若干个扩展板，主要用于为用户提供基于 ESP32-S2 的人机交互应用开发工具。

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-s2-kaluga-1-v1.2-3d.png
    :align: center
    :width: 2631px
    :height: 1966px
    :scale: 25%
    :alt: ESP32-S2-Kaluga-1
    :figclass: align-center

    ESP32-S2-Kaluga-1（点击放大）


相关文档
^^^^^^^^

* :doc:`esp32s2/user-guide-esp32-s2-kaluga-1-kit`

较早版本
^^^^^^^^

* :ref:`esp32s2-modules-and-boards-previous-esp32s2-kaluga-1-kit-v1.2`

.. _FTDI 虚拟 COM 端口驱动: http://www.ftdichip.com/Drivers/VCP.htm
.. _乐鑫产品订购信息: https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_cn.pdf