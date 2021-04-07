====================
ESP-LyraT-8311A v1.2
====================

:link_to_translation:`zh_CN:[中文]`

This user guide provides information on the ESP-LyraT-8311A extension board.

This board cannot be bought separately and is usually sold together with other Espressif development boards (e.g., ESP32-S2-Kaluga-1), which will be referred to as *main boards* below.

Currently, ESP-LyraT-8311A v1.2 is sold as part of the :doc:`user-guide-esp32-s2-kaluga-1-kit-v1.2`.

The ESP-LyraT-8311A extends the functionality of your main board by adding sound processing functionality:

- Audio playback/recording
- Processing of audio signals
- Programmable buttons for easy control

This extension board can be used in many ways. The applications might include voice user interface, voice control, voice authorization, recording and playback of sound, etc.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrat-8311a-v1.2-3d.png
    :align: center
    :width: 2545px
    :height: 1786px
    :scale: 30%
    :alt: ESP-LyraT-8311A
    :figclass: align-center

    ESP-LyraT-8311A (click to enlarge)

The document consists of the following major sections:

- `Overview`_: Provides an overview and hardware setup instructions.
- `Hardware reference`_: Provides more detailed information about the ESP-LyraT-8311A's hardware.
- `Hardware Revision Details`_: Covers revision history, known issues, and links to user guides for previous versions of the ESP-LyraT-8311A.
- `Related Documents`_: Gives links to related documentation.


Overview
========

The ESP-LyraT-8311A is mainly designed for audio applications. However, you can use your creativity to come up with any other use cases.

Description of Components
-------------------------

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrat-8311a-v1.2-layout-front.png
    :align: center
    :width: 934px
    :height: 565px
    :scale: 70%
    :alt: ESP-LyraT-8311A - front
    :figclass: align-center

    ESP-LyraT-8311A - front (click to enlarge)

The description of components starts from the top right corner and then goes clockwise.

**Reserved** means that the functionality is available, but the current version of the kit does not use it.


.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Key Component
     - Description
   * - Extension Header
     - Male Extension Header on the flip side is for mounting onto main board's Extension Header; Female Extension Header is for mounting other boards that have a Male Extension Header
   * - Function Button
     - This board has six programmable buttons
   * - Microphone
     - Supports Electret and MEMS microphones; this extension board is supplied with an electret microphone
   * - Headphone Connector
     - 6.3 mm (1/8") stereo headphone connector
   * - Speaker Connector
     - Connect an external speaker to the 2-pin connector
   * - PA
     - 3 W Audio signal amplifier for the external speaker
   * - External Mic-Matrix Connector
     - (Reserved) FPC connector for external Mic-Matrix (microphone boards)
   * - ADC
     - (Reserved) high-performance ADC/ES7243: 1 channel for microphone, 1 channel for acoustic echo cancellation (AEC) function
   * - Mono Audio Codec
     - ES8311 audio ADC and DAC; it can convert the analog signal picked up by the microphone or convert digital signal to play it back through a speaker or headphones



Start Application Development
-----------------------------

Before powering up your ESP-LyraT-8311A, please make sure that it is in good condition with no obvious signs of damage.


Required Hardware
^^^^^^^^^^^^^^^^^

- Board with a female Extension Header (e.g., ESP32-S2-Kaluga-1)
- ESP-LyraT-8311A extension board
- Four mounting bolts (for stable mounting)
- Computer running Windows, Linux, or macOS


Hardware Setup
^^^^^^^^^^^^^^

To mount your ESP-LyraT-8311A onto the board with a female Extension Header:

1. Install the four mounting bolts onto the board with a female Extension Header
2. Align the ESP-LyraT-8311A with the bolts and Extension Header and insert it carefully


Software Setup
^^^^^^^^^^^^^^

Depending on your application, see:

* `ESP-ADF Getting Started Guide <https://docs.espressif.com/projects/esp-adf/en/latest/get-started/index.html#about-esp-adf>`_ if you develop with ESP-ADF (Espressif Audio Development Framework).
* Section :ref:`user-guide-esp32-s2-kaluga-1-kit-v1.2-software-setup` of the ESP32-S2-Kaluga-1 kit user guide if you develop directly with ESP-IDF (Espressif IOT Development Framework).


Hardware Reference
==================


Block Diagram
-------------

A block diagram below shows the components of ESP-LyraT-8311A and their interconnections.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrat-8311a-v1.2-block-diagram.png
    :align: center
    :alt: ESP-LyraT-8311A block diagram
    :figclass: align-center

    ESP-LyraT-8311A block diagram


Hardware Revision Details
=========================

No previous versions available.


Related Documents
=================

- `ESP-LyraT-8311A Schematic <https://dl.espressif.com/dl/schematics/ESP-LyraT-8311A_V1_2_SCH_20200421A.pdf>`_ (PDF)
- `ESP-LyraT-8311A PCB Layout <https://dl.espressif.com/dl/schematics/ESP-LyraT-8311A_V1_2_PCB_20200324AA.pdf>`_ (PDF)

For other design documentation for the board, please contact us at sales@espressif.com.