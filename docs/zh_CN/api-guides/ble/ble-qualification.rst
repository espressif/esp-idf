蓝牙\ :sup:`®` SIG 认证
================================

:link_to_translation:`en:[English]`

控制器 (Controller)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

参考下表可知乐鑫各芯片上最新的低功耗蓝牙控制器认证信息。 有关乐鑫模组的认证信息，请参阅 `SIG Qualification Workspace <https://qualification.bluetooth.com/MyProjects/ListingsSearch>`__ 。

.. table::
    :widths: 50 25 25

    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+
    |.. centered:: 芯片名称                                                 |.. centered:: Design Number /              |.. centered:: 协议版本 [2]_               |
    |                                                                       |.. centered:: Qualified Design ID [1]_     |                                          |
    +=======================================================================+===========================================+==========================================+
    |ESP32                                                                  |.. centered:: |141661|                     |.. centered:: 5.0                         |
    |（低功耗蓝牙模式）                                                     |                                           |                                          |
    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+
    |ESP32                                                                  |.. centered:: |147845|                     |.. centered:: 4.2                         |
    |（双模模式: 经典蓝牙 & 低功耗蓝牙）                                    |                                           |                                          |
    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+
    |ESP32-C2 (ESP8684)                                                     |.. centered:: |194009|                     |.. centered:: 5.3                         |
    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+
    |ESP32-C3                                                               |.. centered:: |239440|                     |.. centered:: 5.4                         |
    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+
    |ESP32-C6                                                               |.. centered:: |Q335877|                    |.. centered:: 6.0                         |
    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+
    |ESP32-S3                                                               |.. centered:: |239440|                     |.. centered:: 5.4                         |
    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+
    |ESP32-H2                                                               |.. centered:: |Q331318|                    |.. centered:: 6.0                         |
    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+


主机 (Host)
^^^^^^^^^^^^^^^

参考下表可知最新的低功耗蓝牙主机的认证信息。

.. list-table::
    :width: 100%
    :widths: auto
    :header-rows: 1

    * - .. centered:: 主机 (Host)
      - .. centered:: Design Number / Qualified Design ID [1]_
      - .. centered:: 协议版本 [2]_
    * - ESP-Bluedroid
      - .. centered:: `198312 <https://qualification.bluetooth.com/ListingDetails/165785>`__
      - .. centered:: 5.3
    * - ESP-NimBLE
      - .. centered:: `141499 <https://qualification.bluetooth.com/ListingDetails/97856>`__
      - .. centered:: 5.1

.. |141661| replace:: `141661 <https://qualification.bluetooth.com/ListingDetails/98048>`__
.. |147845| replace:: `147845 <https://qualification.bluetooth.com/ListingDetails/105426>`__
.. |239440| replace:: `239440 <https://qualification.bluetooth.com/ListingDetails/212759>`__
.. |194009| replace:: `194009 <https://qualification.bluetooth.com/ListingDetails/160725>`__
.. |199258| replace:: `199258 <https://qualification.bluetooth.com/ListingDetails/166887>`__
.. |198312| replace:: `198312 <https://qualification.bluetooth.com/ListingDetails/165785>`__
.. |141499| replace:: `141499 <https://qualification.bluetooth.com/ListingDetails/97856>`__
.. |Q331318| replace:: `Q331318 <https://qualification.bluetooth.com/ListingDetails/257081>`__
.. |Q335877| replace:: `Q335877 <https://qualification.bluetooth.com/ListingDetails/262779>`__



.. [1]
   自 2024 年 7 月 1 日起，蓝牙 SIG 新认证的设计识别编号已从 Qualified Deign ID (QDID) 更改为 `Design Number (DN) <https://qualification.support.bluetooth.com/hc/en-us/articles/26704417298573-What-do-I-need-to-know-about-the-new-Qualification-Program-Reference-Document-QPRD-v3#:~:text=The%20identifying%20number%20for%20a%20Design%20has%20changed%20from%20Qualified%20Design%20ID%20(QDID)%20to%20Design%20Number%20(DN)>`__。
   请登录 `Bluetooth SIG 官方网站 <https://www.bluetooth.com/>`__ 以获取合格产品的详细信息，例如设计详情、TCRL 版本和 ICS 详情（通过的测试用例）等。

.. [2]
  蓝牙协议中的某些功能是可选的，因此通过某个协议版本的认证并不意味着支持该版本协议中规定的所有功能
  请参阅 :doc:`主要功能支持状态 <ble-feature-support-status>` 以获取各个芯片当前支持的低功耗蓝牙功能。
