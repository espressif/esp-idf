==================
ESP-LyraP-CAM v1.1
==================

:link_to_translation:`zh_CN:[中文]`

This user guide provides information on the ESP-LyraP-CAM extension board.

This extension board cannot be bought separately and is usually sold together with other Espressif development boards (e.g., ESP32-S2-Kaluga-1), which will be referred to as *main boards* below.

Currently, ESP-LyraP-CAM v1.1 is sold as part of the :doc:`user-guide-esp32-s2-kaluga-1-kit`.

The ESP-LyraP-CAM extends the functionality of your main board by adding a camera.

.. Image of v1.0 is used as there are no visual changes

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-cam-v1.0-3d.png
    :align: center
    :alt: ESP-LyraP-CAM
    :figclass: align-center

    ESP-LyraP-CAM

The document consists of the following major sections:

- `Overview`_: Provides an overview and hardware/software setup instructions to get started.
- `Hardware reference`_: Provides more detailed information about the ESP-LyraP-CAM's hardware.
- `Hardware Revision Details`_: Covers revision history, known issues, and links to user guides for previous versions of the ESP-LyraP-CAM.
- `Related Documents`_: Gives links to related documentation.


Overview
========

This extension board adds a camera to your main board.


Description of Components
-------------------------

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-cam-v1.1-layout-front.png
    :align: center
    :alt: ESP-LyraP-CAM - front and back
    :figclass: align-center

    ESP-LyraP-CAM - front and back

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Key Component
     - Description
   * - Main Board Camera Header
     - Mount onto main board's Camera Header
   * - Power ON LED
     - Red LED is on if the power supply voltage is correct
   * - Camera Module Connector
     - Supports OV2640 and OV3660 camera modules; this extension board is supplied with an OV2640 camera module
   * - Power Regulators
     - LDO Regulators converting 3.3 V to 2.8 V and 1.5 V


Start Application Development
-----------------------------

Before powering up your ESP-LyraP-CAM, please make sure that it is in good condition with no obvious signs of damage.


Required Hardware
^^^^^^^^^^^^^^^^^

- Board with a female Camera Header (e.g., ESP32-S2-Kaluga-1)
- ESP-LyraP-CAM extension board
- Computer running Windows, Linux, or macOS


Hardware Setup
^^^^^^^^^^^^^^

Insert the ESP-LyraP-CAM extension board into your board's female Camera Header.


Software Setup
^^^^^^^^^^^^^^

See Section :ref:`user-guide-esp32-s2-kaluga-1-kit-software-setup` of the ESP32-S2-Kaluga-1 kit user guide.


Hardware Reference
==================

Block Diagram
-------------

A block diagram below shows the components of the ESP-LyraP-CAM and their interconnections.

.. Image of v1.0 is used as there are no visual changes

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-cam-v1.0-block-diagram.png
    :align: center
    :alt: ESP-LyraP-CAM block diagram
    :figclass: align-center

    ESP-LyraP-CAM block diagram


Hardware Revision Details
=========================

ESP-LyraP-CAM v1.1
------------------

* Silk screen updated
* No actual hardware updates


ESP-LyraP-CAM v1.0
------------------

:doc:`Initial release <user-guide-esp-lyrap-cam-v1.0>`


Related Documents
=================

- `ESP-LyraP-CAM Schematic <https://dl.espressif.com/dl/schematics/ESP-LyraP-CAM_V1.1_SCH_20200511A.pdf>`_ (PDF)
- `ESP-LyraP-CAM PCB Layout <https://dl.espressif.com/dl/schematics/ESP-LyraP-CAM_V1.1_PCB_20200511AA.pdf>`_ (PDF)

For other design documentation for the board, please contact us at sales@espressif.com.
