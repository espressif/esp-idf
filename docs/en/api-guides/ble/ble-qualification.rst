Bluetooth\ :sup:`®` SIG Qualification
======================================

:link_to_translation:`zh_CN:[中文]`

Controller
^^^^^^^^^^^

The table below shows the latest qualification for Espressif Bluetooth LE Controller on each chip. For the qualification of Espressif modules, please check the `SIG Qualification Workspace <https://qualification.bluetooth.com/MyProjects/ListingsSearch>`__.

.. table::
    :widths: 70 20 10

    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+
    | .. centered:: Chip Name                                               |.. centered:: Design Number /              |.. centered:: Specification               |
    |                                                                       |.. centered:: Qualified Design ID [1]_     |.. centered:: Version [2]_                |
    +=======================================================================+===========================================+==========================================+
    |ESP32                                                                  |                                           |                                          |
    |                                                                       |.. centered:: |141661|                     |.. centered:: 5.0                         |
    |(Bluetooth LE Mode)                                                    |                                           |                                          |
    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+
    |ESP32                                                                  |                                           |                                          |
    |                                                                       |.. centered:: |147845|                     |.. centered:: 4.2                         |
    |(Dual Mode: Bluetooth Classic & Bluetooth LE)                          |                                           |                                          |
    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+
    |ESP32-C2 (ESP8684)                                                     |.. centered:: |194009|                     |.. centered:: 5.3                         |
    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+
    |ESP32-C3                                                               |.. centered:: |239440|                     |.. centered:: 5.4                         |
    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+
    |ESP32-C5                                                               |.. centered:: |Q331318|                    |.. centered:: 6.0                         |
    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+
    |ESP32-C6                                                               |.. centered:: |Q335877|                    |.. centered:: 6.0                         |
    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+
    |ESP32-C61                                                              |.. centered:: |Q331318|                    |.. centered:: 6.0                         |
    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+
    |ESP32-S3                                                               |.. centered:: |239440|                     |.. centered:: 5.4                         |
    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+
    |ESP32-H2                                                               |.. centered:: |Q331318|                    |.. centered:: 6.0                         |
    +-----------------------------------------------------------------------+-------------------------------------------+------------------------------------------+


Host
^^^^^^

The table below shows the latest qualification for Espressif Bluetooth LE Host.

.. list-table::
    :width: 100%
    :widths: auto
    :header-rows: 1

    * - .. centered:: Host
      - .. centered:: Design Number / Qualified Design ID [1]_
      - .. centered:: Specification Version [2]_
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
    Since 1 July 2024, the identifying number for a new qualified design has changed from Qualified Design ID (QDID) to `Design Number (DN) <https://qualification.support.bluetooth.com/hc/en-us/articles/26704417298573-What-do-I-need-to-know-about-the-new-Qualification-Program-Reference-Document-QPRD-v3#:~:text=The%20identifying%20number%20for%20a%20Design%20has%20changed%20from%20Qualified%20Design%20ID%20(QDID)%20to%20Design%20Number%20(DN)>`_.
    Please log in to the `Bluetooth SIG website <https://www.bluetooth.com/>`__ to view Qualified Product Details, such as Design Details, TCRL Version, and ICS Details (passed cases) and etc.

.. [2]
    Some features of the Bluetooth Core Specification are optional. Therefore, passing the certification for a specific specification version does not necessarily mean supporting all the features specified in that version.
    Please refer to :doc:`Major Feature Support Status <ble-feature-support-status>` for the supported Bluetooth LE features on each chip.
