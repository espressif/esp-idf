ESP-BLE-AUDIO
=============

ESP-BLE-AUDIO provides APIs for Bluetooth LE Audio, enabling time-synchronized audio over BLE using the Bluetooth Core Specification LE Audio architecture. Applications can implement unicast (one-to-one) and broadcast audio scenarios, including hearing aids, headsets, speakers, and broadcast sources/sinks.

The LE Audio stack is built on the **Bluetooth Core** and the **LC3** codec. The **generic audio framework** includes the Common Audio Profile (CAP), Common Audio Service (CAS), and the profiles and services below. **Use case specific profiles** (HAP, GMAP, TMAP, PBP) sit on top of the generic framework. LE Audio runs over the LE Isochronous Channels (CIS/BIS) supported by :doc:`esp-ble-iso`.

.. warning::

    This is a **preview release**. The ESP-BLE-AUDIO APIs, data structures, and configuration parameters are subject to change in future releases. Breaking changes — such as renamed or restructured types, modified function signatures, or removed fields — may be introduced without prior notice.

**Generic audio framework**

* **Common Audio Profile (CAP)** — Top-level profile for coordinating audio procedures across single or multiple devices.
* **Common Audio Service (CAS)** — GATT service companion to CAP; used for coordinated set member announcement.
* **Basic Audio Profile (BAP)** — Unicast and broadcast stream setup, codec configuration, and stream control.
* **Published Audio Capabilities Service (PACS)** — Advertising and negotiating audio capabilities.
* **Audio Stream Control Service (ASCS)** — Exposes sink/source audio endpoints on a BAP Unicast Server; used by the Unicast Client to configure and control streams.
* **Broadcast Audio Scan Service (BASS)** — Allows a Broadcast Assistant to offload BIS scanning from a low-power Scan Delegator.
* **Volume Control Profile (VCP)** — Volume control (VCS, VOCS, AICS).
* **Volume Offset Control Service (VOCS)** — Per-output volume offset.
* **Audio Input Control Service (AICS)** — Audio input (microphone) state and gain.
* **Microphone Control Profile (MICP)** — Microphone mute and gain (MICS).
* **Coordinated Set Identification Profile (CSIP)** — Set member identification for coordinated sets (e.g., left/right earbuds).
* **Media Control Profile (MCP)** — Media control (MCS).
* **Media Control Service (MCS)** — Media control service.
* **Call Control Profile (CCP)** — Call/telephony control (TBS).
* **Telephone Bearer Service (TBS)** — Telephony bearer and in-call control.
* **Media Proxy** — Media proxy service (Zephyr implementation component; not a Bluetooth SIG specification).

**Use case specific profiles**

* **Hearing Access Profile (HAP)** — Hearing aid presets and presets control.
* **Gaming Audio Profile (GMAP)** — Gaming audio (e.g. Unicast Game Gateway/Terminal, Broadcast Game Sender/Receiver).
* **Telephony and Media Audio Profile (TMAP)** — Interoperability configurations for telephony and media audio use cases.
* **Public Broadcast Profile (PBP)** — Discovery and subscription to public broadcast streams.

Application Examples
--------------------

* **BAP (Basic Audio Profile)**

  * :example:`bluetooth/esp_ble_audio/bap/broadcast_sink` demonstrates how to act as a BAP Broadcast Sink that synchronizes to a broadcast source and receives BIS audio streams.
  * :example:`bluetooth/esp_ble_audio/bap/broadcast_source` demonstrates how to act as a BAP Broadcast Source that creates a BIG and sends broadcast audio over BIS.
  * :example:`bluetooth/esp_ble_audio/bap/unicast_client` demonstrates how to discover and connect to a unicast server and establish BAP unicast streams.
  * :example:`bluetooth/esp_ble_audio/bap/unicast_server` demonstrates how to advertise and accept unicast connections and serve BAP unicast streams.

* **CAP (Common Audio Profile)**

  * :example:`bluetooth/esp_ble_audio/cap/acceptor` demonstrates how to act as a CAP Acceptor for unicast and broadcast flows.
  * :example:`bluetooth/esp_ble_audio/cap/initiator` demonstrates how to act as a CAP Initiator for unicast and broadcast flows.

* **TMAP (Telephony and Media Audio Profile)**

  * :example:`bluetooth/esp_ble_audio/tmap/bmr` demonstrates the TMAP Broadcast Media Receiver (BMR) role that receives broadcast audio from a BMS.
  * :example:`bluetooth/esp_ble_audio/tmap/bms` demonstrates the TMAP Broadcast Media Sender (BMS) role that sends broadcast audio.
  * :example:`bluetooth/esp_ble_audio/tmap/central` demonstrates the TMAP Call Gateway (CG) and Unicast Media Sender (UMS) roles.
  * :example:`bluetooth/esp_ble_audio/tmap/peripheral` demonstrates the TMAP Call Terminal (CT) and Unicast Media Receiver (UMR) roles.

API Reference
-------------

ESP-BLE-AUDIO APIs are divided into the following parts:

* `Definitions <ESP-BLE-AUDIO Definitions_>`_

  * `General Definitions`_
  * `LC3 Codec Definitions`_
  * `MCS Definitions`_
  * `BAP LC3 Preset Definitions`_
  * `GMAP LC3 Preset Definitions`_

* `API <ESP-BLE-AUDIO API_>`_

  * `Common API`_
  * `Codec API`_
  * `Common Audio Profile (CAP)`_
  * `Call Control Profile (CCP)`_
  * `Basic Audio Profile (BAP)`_
  * `Published Audio Capabilities (PACS)`_
  * `Gaming Audio Profile (GMAP)`_
  * `Volume Control Profile (VCP)`_
  * `Volume Offset Control Service (VOCS)`_
  * `Audio Input Control Service (AICS)`_
  * `Microphone Control Profile (MICP)`_
  * `Hearing Access Profile (HAP)`_
  * `Coordinated Set Identification Profile (CSIP)`_
  * `Telephone Bearer Service (TBS)`_
  * `Media Control Client (MCC)`_
  * `Media Proxy`_
  * `Telephony and Media Audio Profile (TMAP)`_
  * `Public Broadcast Profile (PBP)`_

ESP-BLE-AUDIO Definitions
-------------------------

This section contains common definitions, constants, and types used across LE Audio.


General Definitions
^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_defs.inc


LC3 Codec Definitions
^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_lc3_defs.inc


MCS Definitions
^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_mcs_defs.inc


BAP LC3 Preset Definitions
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_bap_lc3_preset_defs.inc


GMAP LC3 Preset Definitions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_gmap_lc3_preset_defs.inc


ESP-BLE-AUDIO API
-----------------

This section contains LE Audio APIs: common helpers, codec, Common Audio Profile (CAP), Call Control Profile (CCP), and all profile/service APIs.


Common API
^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_common_api.inc


Codec API
^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_codec_api.inc


Common Audio Profile (CAP)
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_cap_api.inc


Call Control Profile (CCP)
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_ccp_api.inc


Basic Audio Profile (BAP)
^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_bap_api.inc


Published Audio Capabilities (PACS)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_pacs_api.inc


Gaming Audio Profile (GMAP)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_gmap_api.inc


Volume Control Profile (VCP)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_vcp_api.inc


Volume Offset Control Service (VOCS)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_vocs_api.inc


Audio Input Control Service (AICS)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_aics_api.inc


Microphone Control Profile (MICP)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_micp_api.inc


Hearing Access Profile (HAP)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_has_api.inc


Coordinated Set Identification Profile (CSIP)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_csip_api.inc


Telephone Bearer Service (TBS)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_tbs_api.inc


Media Control Client (MCC)
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_mcc_api.inc


Media Proxy
^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_media_proxy_api.inc


Telephony and Media Audio Profile (TMAP)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_tmap_api.inc


Public Broadcast Profile (PBP)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_ble_audio_pbp_api.inc
