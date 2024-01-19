=====================
ESP-LyraP-TouchA v1.1
=====================

:link_to_translation:`zh_CN:[中文]`

This user guide provides information on the ESP-LyraP-TouchA extension board.

This board cannot be bought separately and is usually sold together with other Espressif development boards (e.g., ESP32-S2-Kaluga-1), which will be referred to as *main boards* below.

Currently, ESP-LyraP-TouchA v1.1 is sold as part of the following kits:

* :doc:`user-guide-esp32-s2-kaluga-1-kit`
* :doc:`user-guide-esp32-s2-kaluga-1-kit-v1.2`

The ESP-LyraP-TouchA extends the functionality of your main board by adding touch buttons.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-toucha-v1.1-3d.png
    :align: center
    :alt: ESP-LyraP-TouchA
    :figclass: align-center

    ESP-LyraP-TouchA

The document consists of the following major sections:

- `Overview`_: Provides an overview and hardware setup instructions.
- `Hardware reference`_: Provides more detailed information about the ESP-LyraP-TouchA's hardware.
- `Hardware Revision Details`_: Covers revision history, known issues, and links to user guides for previous versions of the ESP-LyraP-TouchA.
- `Related Documents`_: Gives links to related documentation.


Overview
========

The ESP-LyraP-TouchA has six touch buttons and is mainly designed for audio applications. However, the touch buttons can also be used for any other purposes.


Description of Components
-------------------------

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-toucha-v1.1-layout-front.png
    :align: center
    :alt: ESP-LyraP-TouchA
    :figclass: align-center

    ESP-LyraP-TouchA

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Key Component
     - Description
   * - Main Board Touch FPC Connector
     - Connect to main board's Touch FPC Connector.
   * - Touchpad
     - Capacitive touch electrode.
   * - Guard Ring
     - Connected to a touch sensor, the guard ring triggers an interrupt if wet (Water rejection). It indicates that the sensor array is also wet and most (if not all) touchpads are unusable due to the false detection of touches. After receiving this interrupt, the user might consider disabling all the touch sensors via software.


Start Application Development
-----------------------------

Before powering up your ESP-LyraP-TouchA, please make sure that it is in good condition with no obvious signs of damage.


Required Hardware
^^^^^^^^^^^^^^^^^

- Board with a Touch FPC connector  (e.g., ESP32-S2-Kaluga-1)
- ESP-LyraP-TouchA extension board
- FPC cable
- Computer running Windows, Linux, or macOS


Hardware Setup
^^^^^^^^^^^^^^

Connect the two FPC connectors with the FPC cable.


Software Setup
^^^^^^^^^^^^^^

See Section :ref:`user-guide-esp32-s2-kaluga-1-kit-software-setup` of the ESP32-S2-Kaluga-1 kit user guide.


Hardware Reference
==================

Block Diagram
-------------

A block diagram below shows the components of ESP-LyraP-TouchA and their interconnections.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-toucha-v1.1-block-diagram.png
    :align: center
    :alt: ESP-LyraP-TouchA-v1.1 block diagram
    :figclass: align-center

    ESP-LyraP-TouchA-v1.1 block diagram


Hardware Revision Details
=========================

No previous versions available.


Related Documents
=================

- `ESP-LyraP-TouchA Schematic <https://dl.espressif.com/dl/schematics/ESP-LyraP-TouchA_V1.1_SCH_20200325A.pdf>`_ (PDF)
- `ESP-LyraP-TouchA PCB Layout <https://dl.espressif.com/dl/schematics/ESP-LyraP-TouchA_V1.1_PCB_20200325AA.pdf>`_ (PDF)

For other design documentation for the board, please contact us at sales@espressif.com.
