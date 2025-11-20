Bluetooth\ :sup:`®` Classic
===============================

:link_to_translation:`zh_CN:[中文]`

**Bluetooth Classic** provides APIs for implementing traditional Bluetooth functionalities, including audio streaming, device communication, and data exchange over the Serial Port Profile. It supports multiple Bluetooth profiles, allowing ESP devices to act as source or sink in scenarios like wireless audio, remote control, and data transmission.

The Bluetooth Classic API provides the following main features:

- Core protocol support (**GAP**, **L2CAP**, and **SDP**)
- Serial data communication (**SPP**)
- High-quality audio streaming (**A2DP**)
- Media playback control (**AVRCP**)
- Hands-free calling support (**HFP**)
- Input device connectivity (**HID** host and device roles)

----

The Bluetooth Classic API in ESP-IDF is organized into the following parts:

**Core Protocols**

- :doc:`Bluetooth GAP <esp_gap_bt>`

  **Generic Access Profile (GAP):** Device discovery, pairing, and security management

- :doc:`Bluetooth L2CAP <esp_l2cap_bt>`

  **Logical Link Control and Adaptation Protocol (L2CAP):** Data multiplexing and channel management

- :doc:`Bluetooth SDP <esp_sdp>`

  **Service Discovery Protocol (SDP):** Discovers remote device services and attributes

**Communication Profile**

- :doc:`Bluetooth SPP <esp_spp>`

  **Serial Port Profile (SPP):** Emulates a serial communication channel over Bluetooth for data exchange

**Audio and Media Profiles**

- :doc:`Bluetooth A2DP <esp_a2dp>`

  **Advanced Audio Distribution Profile (A2DP):** High-quality audio streaming (source and sink)

- :doc:`Bluetooth AVRCP <esp_avrc>`

  **Audio/Video Remote Control Profile (AVRCP):** Media playback control (play, pause, volume)

**Hands-Free Profile (HFP)**

- :doc:`Bluetooth HFP Define <esp_hf_defs>`: Core definitions shared by HFP roles
- :doc:`Bluetooth HFP Client <esp_hf_client>`: Implements the hands-free unit role (e.g., headset, car kit)
- :doc:`Bluetooth HFP AG <esp_hf_ag>`: Implements the audio gateway role (e.g., mobile phone)

**Human Interface Device (HID)**

- :doc:`Bluetooth HID Device <esp_hidd>`: Implements peripheral roles such as keyboard, mouse, or game controller
- :doc:`Bluetooth HID Host <esp_hidh>`: Implements the host role for connecting to remote HID devices

Each part typically includes an **Overview**, **Application Examples**, and **API Reference**, covering purpose, main functionality, sample usage, and detailed API documentation.


.. toctree::
   :maxdepth: 1
   :hidden:

   Bluetooth GAP <esp_gap_bt>
   Bluetooth L2CAP <esp_l2cap_bt>
   Bluetooth SDP <esp_sdp>
   Bluetooth SPP <esp_spp>
   Bluetooth A2DP <esp_a2dp>
   Bluetooth AVRCP <esp_avrc>
   Bluetooth HFP Define <esp_hf_defs>
   Bluetooth HFP Client <esp_hf_client>
   Bluetooth HFP AG <esp_hf_ag>
   Bluetooth HID Device <esp_hidd>
   Bluetooth HID Host <esp_hidh>
