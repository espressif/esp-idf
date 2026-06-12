Feature Support Status
======================

:link_to_translation:`zh_CN:[中文]`

This page tracks the support status of Bluetooth LE Audio features in ESP-IDF — the Generic Audio Framework profiles and services provided by ESP-BLE-AUDIO, and the isochronous transport provided by ESP-BLE-ISO.

.. note::

   The Bluetooth LE Audio features marked as supported below are currently a **preview**: their APIs and behavior are provisional and may change in future releases.

The table below lists the Bluetooth LE Audio profiles and services currently supported in ESP-IDF.

.. list-table::
    :header-rows: 1
    :widths: 28 12 60

    * - Profile / Service
      - Supported
      - Notes
    * - LE Isochronous Channels (CIS / BIS)
      - Yes
      - Direct ISO access via :doc:`ESP-BLE-ISO <../../api-reference/bluetooth/esp-ble-iso>`.
    * - BAP
      - Yes
      - All six BAP roles: Unicast Client, Unicast Server, Broadcast Source, Broadcast Sink, Broadcast Assistant, Scan Delegator.
    * - PACS
      - Yes
      - Used by BAP Unicast Server and Broadcast Sink.
    * - ASCS
      - Yes
      - Used by BAP Unicast Server.
    * - BASS
      - Yes
      - Used by BAP Scan Delegator and Broadcast Assistant.
    * - CAP
      - Yes
      - All three CAP roles: Acceptor, Initiator, Commander.
    * - CAS
      - Yes
      - Mandatory service on CAP Acceptors.
    * - CSIP / CSIS
      - Yes
      - Set Member and Set Coordinator roles.
    * - VCP / VCS
      - Yes
      - Volume Renderer and Volume Controller roles.
    * - VOCS
      - Yes
      - Per-output volume offset control; included in VCS as an optional sub-service.
    * - AICS
      - Yes
      - Audio input control; included in VCS and MICS as an optional sub-service.
    * - MICP / MICS
      - Yes
      - Microphone Device and Microphone Controller roles.
    * - MCP / MCS
      - Partial
      - Media Control Server and Media Control Client roles are supported. OTP/OTS-based media object transfer is not currently supported.
    * - CCP / TBS
      - Yes
      - Call Control Server and Call Control Client roles, including GTBS and TBS.
    * - HAP / HAS
      - Yes
      - Hearing Aid and Hearing Aid Unicast Client roles, including preset read/write via HAS.
    * - TMAP / TMAS
      - Yes
      - All six TMAP roles: CG, CT, UMS, UMR, BMS, BMR.
    * - GMAP / GMAS
      - Yes
      - All four GMAP roles: UGG, UGT, BGS, BGR.
    * - PBP
      - Yes
      - Public Broadcast Source and Public Broadcast Sink roles.
    * - OTP / OTS
      - No
      - Object Transfer Profile/Service (used by MCP/MCS for media object transfer) is not currently supported.

.. note::

   For the profile and service definitions, see :doc:`Bluetooth LE Audio Standard <ble-audio-introduction>`.

   For general Bluetooth Low Energy feature support, see :doc:`Major Feature Support Status <../ble/ble-feature-support-status>`.
