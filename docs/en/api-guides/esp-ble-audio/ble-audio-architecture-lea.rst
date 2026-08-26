.. _arch-audio:

ESP-BLE-AUDIO
=============

:link_to_translation:`zh_CN:[中文]`

.. |br| raw:: html

   <br/>

.. contents:: Table of Contents

ESP-BLE-AUDIO sits directly on top of ESP-BLE-ISO and implements the Bluetooth LE Audio profiles and services. It is the layer an ESP-IDF Bluetooth LE Audio application actually programs against, through the ``esp_ble_audio_*`` API.

Component Layout
----------------

The component has two distinct kinds of code, and the distinction matters for this document:

- **Open source, covered here.** The public API headers (``api/include/esp_ble_audio_*_api.h``), the GATT *service* adapters under ``host/adapter/bluedroid/profiles`` and ``host/adapter/nimble/profiles``, the Object Transfer Service under ``host/services/ots``, and the initialization orchestration in ``host/common/init.c``.
- **Prebuilt, out of scope.** The ESP-IDF Bluetooth LE Audio profile and controller *logic* -- the unicast and broadcast clients, the volume, microphone, media and call control, set coordination, and the top-level profiles -- ships as a prebuilt per-target library (``lib/lib/<target>/libble_audio.a``, linked with ``add_prebuilt_library``). This document does not describe its internals; it describes the public contract around it and, where it matters, the task a registered callback runs on.

Layering
--------

.. mermaid::

   %%{init: {'flowchart': {'nodeSpacing': 40, 'rankSpacing': 40, 'subGraphTitleMargin': {'top': 6, 'bottom': 14}}}}%%
   flowchart TB
       APP["Application"]
       API["ESP-BLE-AUDIO public API<br/>(esp_ble_audio_*)"]
       subgraph AUDIO["ESP-BLE-AUDIO"]
         direction LR
         LIB["Prebuilt profile and<br/>controller library"]
         SVC["GATT service adapters<br/>(per host)"]
         OTS["Object Transfer Service"]
         LIB ~~~ SVC
         LIB ~~~ OTS
       end
       ISO["ESP-BLE-ISO transport<br/>(GATT, GAP, ISO, L2CAP, ISO task)"]
       APP --> API --> AUDIO --> ISO

.. _arch-audio-svc-ctrl:

Services and Controllers
------------------------

Bluetooth LE Audio functionality divides into two kinds of building block, and the component reflects the split:

- **GATT services** are the attribute tables a peer reads, writes and subscribes to. They are implemented in the open per-host adapters (``host/adapter/*/profiles``), one source file per service, on top of the ESP-BLE-ISO GATT-server layer. Grouped by their profile, as in the Bluetooth LE Audio specification, the set is PACS, ASCS and BASS (BAP), MCS (MCP), TBS (CCP), CSIS (CSIP), MICS (MICP), VCS (VCP), CAS (CAP), TMAS (TMAP) and HAS (HAP).
- **Profile clients and controllers** are the state machines that drive those services and orchestrate streams: the Basic Audio and Common Audio profiles, volume, microphone, media and call control, coordinated-set identification, and the top-level Telephony-and-Media, Gaming, and Public-Broadcast profiles. This logic lives in the prebuilt library; the application reaches it through the matching ``esp_ble_audio_*_api.h`` header.

The GATT services are detailed in :ref:`GATT Services <arch-audio-services>`, and the clients and controllers in :ref:`Clients and Controllers <arch-audio-profiles>`.

Where Profile Callbacks Run
---------------------------

A registered ESP-IDF Bluetooth LE Audio callback runs on whichever task delivered the underlying transport event, so the rules from :ref:`Callback Execution Context <arch-callback-context>` carry over directly:

- A callback driven by a **notification** -- for example an ASCS or volume-state change arriving as a GATT notification -- runs on the ISO task on both hosts.
- A callback driven by a **GATT client read or write completion** -- for example reading a peer's PAC records -- runs on the ISO task on Bluedroid but on the **NimBLE host task** on NimBLE.
- A callback driven by an inbound **attribute write to a local service** -- for example a client configuring an ASE -- runs on the ISO task on Bluedroid but on the **NimBLE host task** on NimBLE.

The consequence is the one that runs through the whole stack: on the ISO task a callback shares the 4 KB stack and must not block the event loop, while on the NimBLE host task it executes inside the host's attribute path. In every case the global ISO lock is held, so profile state stays consistent regardless of which task the callback runs on.

.. _arch-audio-services:

GATT Services
-------------

Bluetooth LE Audio exposes its capabilities and state through a fixed set of GATT services. These are the *server* side of the profiles -- the attribute tables a peer reads, writes and subscribes to -- and, as established in :ref:`Services and Controllers <arch-audio-svc-ctrl>`, they are the open part of ESP-BLE-AUDIO: one source file per service under ``host/adapter/bluedroid/profiles`` and ``host/adapter/nimble/profiles``. The state behind each service, and the command handling for its control points, lives in the prebuilt library; this section covers the services and their adapters, not that logic.

.. _arch-audio-adapter:

Service Adapter Pattern
~~~~~~~~~~~~~~~~~~~~~~~

Every service is realized the same way. The prebuilt library owns the canonical, host-agnostic definition of the service's attribute table (a ``bt_gatt_service``); the open adapter registers that table with the active host's GATT server and then reconciles the handles the host assigns back onto the library's definition, so the library can address its own attributes when it notifies or indicates. The two hosts differ in how the registration is done:

- The **NimBLE** adapter declares the service a second time in NimBLE's native form (a ``ble_gatt_svc_def``), registers it with ``ble_gatts_add_svcs``, checks that its characteristics match the library's definition, and maps the assigned handles back with a ``*_attr_handle_set`` step. Registration is synchronous and single-phase.
- The **Bluedroid** adapter hands the library's ``bt_gatt_service`` to a shared ``bt_le_bluedroid_svc_init`` helper that drives BTA GATTS. Because BTA registers a service in two asynchronous phases, every Bluedroid service adapter exposes a matching ``*_init`` and ``*_start`` pair and coordinates completion with the server semaphore from the GATT-server layer.

Once a service is registered, peer access to its attributes follows the per-host attribute-access path of :ref:`Callback Execution Context <arch-callback-context>`: completed synchronously on the NimBLE host task, or posted to the ISO task on Bluedroid.

.. _arch-audio-deferred-add:

**When a service is added (deferred registration)**

**Mechanism.** Services are not all added to the GATT table during ``esp_ble_audio_common_init``. The build defines ``BLE_AUDIO_SVC_DEFERRED_ADD`` by default, which holds a service back until the application registers the matching role through its ``esp_ble_audio_<profile>_register`` call -- the prebuilt library turns that call into the service's ``bt_le_<service>_init`` registration -- rather than adding it during init.

**Reason.** A firmware image usually has more Bluetooth LE Audio capabilities compiled in than any one application uses: enabling a service's Kconfig role option builds its code in, but the application may never take that role. If every compiled-in service were added at init, a connecting peer would discover and could access attributes for a service that has no backing state behind it yet -- the application has not registered it, so the prebuilt library cannot answer for it meaningfully.

**Impact on the application.** Deferring the add keeps an unused-but-compiled-in capability out of the GATT table until the application opts into it, so a peer only ever discovers services that are fully backed.

Service Reference
~~~~~~~~~~~~~~~~~

.. list-table:: Bluetooth LE Audio GATT services
   :header-rows: 1
   :widths: 10 24 38 11 17

   * - Service
     - Full name
     - Key characteristics
     - Adapter
     - Profile
   * - PACS
     - Published Audio Capabilities Service
     - Sink and Source PAC records, audio locations, available and supported audio contexts.
     - ``pacs.c``
     - BAP
   * - ASCS
     - Audio Stream Control Service
     - Sink and Source Audio Stream Endpoints (ASEs) and the ASE control point.
     - ``ascs.c``
     - BAP
   * - BASS
     - Broadcast Audio Scan Service
     - Broadcast Receive State and the broadcast audio scan control point.
     - ``bass.c``
     - BAP
   * - MCS
     - Media Control Service
     - Media player name and track information, the media control point and related state.
     - ``mcs.c``
     - MCP
   * - TBS
     - Telephone Bearer Service
     - Bearer information, call states and the call control point.
     - ``tbs.c``
     - CCP
   * - CSIS
     - Coordinated Set Identification Service
     - Set Identity Resolving Key (SIRK), set size, set member lock and rank.
     - ``csis.c``
     - CSIP
   * - MICS
     - Microphone Control Service
     - Microphone mute; includes AICS.
     - ``mics.c``
     - MICP
   * - VCS
     - Volume Control Service
     - Volume state, the volume control point and volume flags; includes VOCS and AICS.
     - ``vcs.c``
     - VCP
   * - CAS
     - Common Audio Service
     - Identifies a Common Audio device; may include CSIS.
     - ``cas.c``
     - CAP
   * - TMAS
     - Telephony and Media Audio Service
     - The device's TMAP role.
     - ``tmas.c``
     - TMAP
   * - HAS
     - Hearing Access Service
     - Hearing-aid features, the preset control point and the active preset index.
     - ``has.c``
     - HAP

Each service is configured by the application through its profile's ``esp_ble_audio_*_api.h`` header (for example ``esp_ble_audio_vcp_api.h`` for VCS, ``esp_ble_audio_pacs_api.h`` for PACS); those APIs are part of the client-and-controller reference that follows.

Included Services and Control Points
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Several services are not standalone but are *included* by another service:

- **CAS** includes **CSIS** when the device is a member of a coordinated set, so a peer discovers set membership through the common audio device.
- **VCS** includes zero or more **VOCS** (Volume Offset Control Service) and **AICS** (Audio Input Control Service) instances -- one VOCS per independently offset output, one AICS per audio input.
- **MICS** includes zero or more **AICS** instances, one per audio input.

AICS and VOCS therefore have public API headers (``esp_ble_audio_aics_api.h`` and ``esp_ble_audio_vocs_api.h``) but no standalone adapter file; the including service registers them as part of its own attribute table.

Several services -- ASCS, BASS, MCS, TBS, CSIS, VCS and HAS -- expose a *control point*: a writable characteristic that carries a command and is answered by notifying the affected state characteristic. The command handling lives in the prebuilt library; the adapter only carries the write inward and the resulting notification outward, on the task described in :ref:`Callback Execution Context <arch-callback-context>`.

.. _arch-audio-profiles:

Clients and Controllers
-----------------------

The profiles and their client and controller roles are the part of ESP-BLE-AUDIO that ships in the prebuilt library. This section describes them at the level this document can: their roles, the public API through which an application drives them, and the task their callbacks run on. The internal state machines are out of scope.

A *client* or *controller* role acts on a remote device over a connection -- for example a volume controller changing a renderer's volume -- while a *renderer*, *device*, *member* or *server* role responds locally and is backed by one of the GATT services in the previous section. A single application can hold several roles at once.

Profile Relationships
~~~~~~~~~~~~~~~~~~~~~

Bluetooth LE Audio profiles build on one another. The **Basic Audio Profile (BAP)** is the foundation: it establishes and carries the audio streams, both unicast (over Connected Isochronous Streams) and broadcast (over Broadcast Isochronous Streams). The **control profiles** -- volume, microphone, coordinated-set, media and call control -- manage features and are each backed by a GATT service. The **Common Audio Profile (CAP)** coordinates the others so an operation applies consistently across every member of a set. The **top-level profiles** -- Telephony and Media Audio (TMAP), Gaming Audio (GMAP), Public Broadcast (PBP) and Hearing Access (HAP) -- are defined combinations of the layers below.

.. mermaid::

   flowchart TB
       TOP["Top-level profiles — TMAP, GMAP, PBP, HAP"]
       CAP["CAP — coordinated control across a set"]
       CTRL["Control profiles — VCP, MICP, CSIP, MCP, CCP"]
       BAP["BAP — unicast (CIS) and broadcast (BIS) streams"]
       SVC["Bluetooth LE Audio GATT services"]
       ISO["ESP-BLE-ISO transport"]
       TOP --> CAP
       CAP --> BAP
       CAP --> CTRL
       TOP --> CTRL
       CTRL --> SVC
       BAP --> ISO

Profile Reference
~~~~~~~~~~~~~~~~~

.. list-table:: Bluetooth LE Audio profiles
   :header-rows: 1
   :widths: 10 27 45 18

   * - Profile
     - Full name
     - Roles
     - API header
   * - BAP
     - Basic Audio Profile
     - Unicast client and server, broadcast source and sink, broadcast assistant, scan delegator.
     - ``bap_api.h``
   * - CAP
     - Common Audio Profile
     - Initiator, commander and handover across a coordinated set.
     - ``cap_api.h``
   * - VCP
     - Volume Control Profile
     - Volume controller (client) and volume renderer (server).
     - ``vcp_api.h``
   * - MICP
     - Microphone Control Profile
     - Microphone controller (client) and microphone device (server).
     - ``micp_api.h``
   * - CSIP
     - Coordinated Set Identification Profile
     - Set coordinator (client) and set member (server).
     - ``csip_api.h``
   * - MCP
     - Media Control Profile
     - Media Control Client (client) and the media proxy (server).
     - ``mcc_api.h``, ``media_proxy_api.h``
   * - CCP
     - Call Control Profile
     - Call Control Client (the server side is the Telephone Bearer Service).
     - ``ccp_api.h``
   * - TMAP
     - Telephony and Media Audio Profile
     - Role configuration combining CAP, BAP and the control profiles.
     - ``tmap_api.h``
   * - GMAP
     - Gaming Audio Profile
     - Role configuration for low-latency gaming audio.
     - ``gmap_api.h``
   * - PBP
     - Public Broadcast Profile
     - Public broadcast announcement helpers over BAP broadcast.
     - ``pbp_api.h``

API Shape and Roles
~~~~~~~~~~~~~~~~~~~

Each profile exposes a small, uniform API in its ``esp_ble_audio_<profile>_api.h`` header: the application registers a callback structure for the role it is taking and calls functions to start operations -- discover a peer, configure a stream, set a volume, place a call. Results and state changes then arrive asynchronously through that callback, on the task described in :ref:`Callback Execution Context <arch-callback-context>` -- the ISO task in most cases, the NimBLE host task for GATT read and write completions and for inbound writes to a local service.

Because registration and dispatch are uniform across profiles, adding a role to an application always has the same shape: enable the role's Kconfig option, register its callback structure, and drive it through its functions. The role options also determine which GATT services are compiled in, as the initialization flow below shows.

Object Transfer (Draft)
-----------------------

.. warning::

   Object Transfer is an **early draft and is not yet officially supported**. It builds on the draft L2CAP channel (:ref:`L2CAP <arch-l2cap>`), so it runs only on NimBLE and **not on Bluedroid**, and its API and behavior are provisional and may change. Do not rely on it in production.

The Object Transfer Service (OTS) moves bulk objects -- larger than an ordinary GATT read can carry -- over the credit-based L2CAP channel from :ref:`L2CAP <arch-l2cap>`. In Bluetooth LE Audio it is used by Media Control: a media player exposes objects such as the current track segmentation and group structure, and a client transfers them with OTS, reached through the media API (``esp_ble_audio_mcs_get_ots`` and the Media Control Client).

Unlike the profiles, OTS is open source, under ``host/services/ots``. Its parts are:

- a **server** (``ots.c``) and a **client** (``ots_client.c``);
- the **Object Action Control Point** (``ots_oacp.c``), which carries the read, write, create and similar object operations;
- the **Object List Control Point** (``ots_olcp.c``), which navigates the list of objects;
- an **object manager** (``ots_obj_manager.c``) and a **directory listing** object (``ots_dir_list.c``);
- the **L2CAP transport** (``ots_l2cap.c``) that carries object data over the credit-based channel.

.. _arch-init-flow:

Initialization Flow
-------------------

An ESP-IDF Bluetooth LE Audio application brings the stack up with two calls, mirroring the transport: ``esp_ble_audio_common_init`` followed by ``esp_ble_audio_common_start``.

``esp_ble_audio_common_init`` runs ``bt_le_audio_init`` in ``init.c``, which first establishes the boundary with the prebuilt library -- it checks that the shared-structure ABI matches and pushes the active Kconfig values into the library -- and then calls the host-specific ``bt_le_{host}_audio_init``. The host initializer brings up the standard GAP and GATT services and then registers each enabled Bluetooth LE Audio service through its adapter, in a fixed order: PACS, ASCS, BASS, TMAS, GTBS, HAS, CSIS and CAS, then the media, volume and microphone services. Each service is compiled in only when its Kconfig role option is set, so a build contains exactly the services its roles need. That fixed order is the order the services register in; *when* most of them actually enter the GATT table is a separate question -- by default the add is deferred to the application's per-role ``esp_ble_audio_<profile>_register`` call rather than happening here at init (see :ref:`deferred registration <arch-audio-deferred-add>`).

``esp_ble_audio_common_start`` runs ``bt_le_audio_start`` and dispatches to the host-specific start, which first registers the coordinated-set services (CSIS and CAS) from ``start_info`` and then commits the GATT server. On Bluedroid the commit is the second phase that actually starts the BTA services registered during init -- the ``*_init`` and ``*_start`` pairing from :ref:`The Service Adapter Pattern <arch-audio-adapter>`; on NimBLE it calls ``ble_gatts_start`` and then reconciles the assigned attribute handles back into the library.

.. mermaid::

   %%{init: {'sequence': {'noteAlign': 'left'}}}%%
   sequenceDiagram
       participant APP as Application
       participant CMN as ESP-BLE-AUDIO (init.c)
       participant LIB as Prebuilt library
       participant AD as Host adapter
       APP->>CMN: esp_ble_audio_common_init (gap_cb, gatt_cb)
       CMN->>LIB: check ABI, push configuration
       CMN->>AD: bt_le_{host}_audio_init
       Note over AD: bring up GAP/GATT, then register<br/>enabled services in fixed order
       APP->>CMN: esp_ble_audio_common_start
       CMN->>AD: bt_le_{host}_audio_start
       Note over AD: register CSIS/CAS from start_info, then commit the GATT server<br/>Bluedroid: phase-2 start of the registered BTA services<br/>NimBLE: ble_gatts_start, then reconcile attr handles

.. _arch-audio-mtu:

ATT MTU
~~~~~~~

Bluetooth LE Audio depends on an ATT MTU large enough to carry its control PDUs.

**Spec floor.** The Basic Audio Profile (BAP) requires a minimum ATT MTU of only 64 octets (the public ``ESP_BLE_AUDIO_ATT_MTU_MIN``).

**Why 128.** 64 octets is not enough to operate four ASEs at once, so ESP-IDF raises the default to 128 octets (``BLE_AUDIO_ATT_MTU_MIN``) to leave headroom for the ASCS and PACS control PDUs that Bluetooth LE Audio relies on. Both hosts set the device's preferred ATT MTU to this value during initialization (``bt_le_{host}_audio_init``), through ``BTA_GATT_SetLocalMTU`` on Bluedroid and ``ble_att_set_preferred_mtu`` on NimBLE.

**Negotiation rule.** This is the value the device offers as a client and accepts as a server, so the MTU that ends up in force is the smaller of the two peers' preferred values.

The exchange itself is the GATT procedure described in :ref:`MTU Exchange <arch-iso-mtu>`: the client sends the request, the server responds, and one negotiation governs the whole connection. In the common Bluetooth LE Audio topology -- a phone as central and an ESP device as peripheral -- each device runs **both** a GATT client and a GATT server, since the GATT roles are independent of the GAP role. Yet a single MTU exchange covers the whole connection: the phone's client initiates it, the ESP's server responds, and the one negotiated value then governs all ATT traffic between them -- no matter which side is acting as client or server at a given moment:

.. mermaid::

   sequenceDiagram
       box transparent Phone (GAP central)
       participant PS as GATT server
       participant PC as GATT client
       end
       box transparent ESP (GAP peripheral)
       participant ES as GATT server
       participant EC as GATT client
       end
       PC->>ES: ATT Exchange MTU Request
       ES-->>PC: ATT Exchange MTU Response
       Note over ES: server responds, never initiates
       Note over PS,EC: the one negotiated MTU (min of both sides)<br/>governs all ATT traffic, both directions
       PC->>ES: service discovery
       PC->>ES: subscribe (CCCD), then PACS / ASCS reads, writes
       EC->>PS: service discovery (peripheral as client, reuses the same MTU)

The other two roles -- the phone's GATT server and the ESP's GATT client -- are present on the same connection: the ESP's client can run its own service discovery against the phone's server, but neither role runs a second MTU exchange; both reuse the value the phone's client already negotiated. Because the ESP peripheral only responds and never initiates the MTU exchange, that one negotiation is paced by the central, not by the peripheral's host stack.

End-to-End Flows
----------------

The four flows below tie the layers together for the two unicast roles and the two broadcast roles. They are deliberately high-level -- each arrow may stand for several exchanges -- and show which component owns each step.

Unicast Initiator
~~~~~~~~~~~~~~~~~

A unicast initiator connects to an acceptor, configures its audio stream endpoints, establishes a Connected Isochronous Stream and starts sending audio:

.. mermaid::

   sequenceDiagram
       participant APP as Application (initiator)
       participant AUD as ESP-BLE-AUDIO
       participant ISO as ESP-BLE-ISO
       participant CTRL as Controller
       APP->>ISO: scan and connect (GAP)
       APP->>ISO: start GATT discovery
       AUD->>ISO: discover PACS and ASCS, subscribe to ASEs
       AUD->>ISO: configure ASEs (write the ASE control point)
       AUD->>ISO: create CIG and connect CIS
       ISO->>CTRL: LE Set CIG Parameters, Create CIS
       CTRL->>ISO: CIS Established event
       AUD->>ISO: set up the ISO data path (input)
       APP->>CTRL: send ISO SDUs
       CTRL->>ISO: ISO transmit complete
       Note over APP,CTRL: subsequent SDUs repeat the send / transmit-complete cycle

The stream setup -- discovering PACS and ASCS, configuring the ASEs, creating the CIG, connecting the CIS and setting up the data path -- is driven by the ESP-BLE-AUDIO profile library on top of ESP-BLE-ISO's GATT and ISO layers, not by the application directly. The application drives only the high-level flow through the ``esp_ble_audio_*`` API -- connect, start discovery, start the stream and send audio -- and ESP-BLE-AUDIO carries out the underlying ESP-BLE-ISO operations on its behalf.

Unicast Acceptor
~~~~~~~~~~~~~~~~

A unicast acceptor advertises, accepts a connection from an initiator and lets the initiator configure its audio stream endpoints before receiving audio:

.. mermaid::

   sequenceDiagram
       participant APP as Application (acceptor)
       participant AUD as ESP-BLE-AUDIO
       participant ISO as ESP-BLE-ISO
       participant CTRL as Controller
       APP->>ISO: advertise (GAP, audio announcement)
       CTRL->>ISO: ACL connection established (peripheral)
       CTRL->>ISO: remote initiator writes the ASE control point
       ISO->>AUD: ASCS server applies config / QoS / enable
       AUD->>APP: config / QoS / enable callbacks
       APP->>AUD: response (accept / reject + QoS preferences)
       AUD->>ISO: notify ASE state and control-point result
       ISO->>CTRL: send to the remote initiator
       CTRL->>ISO: CIS Established event
       AUD->>ISO: set up the ISO data path (output)
       CTRL->>ISO: ISO SDUs
       ISO->>APP: recv callback on the ISO task
       Note over APP,CTRL: subsequent ISO SDUs repeat the receive / recv-callback cycle

Unlike the initiator, the acceptor is passive. The remote initiator drives the ASCS state machine by writing the ASE control point, and the acceptor's ESP-BLE-AUDIO ASCS server surfaces each step -- config, QoS, enable -- to the application as callbacks. Each callback returns the application's response -- accept or reject, plus QoS preferences at codec configuration -- which the ASCS server notifies back to the initiator as the updated ASE state and control-point result. The Connected Isochronous Stream is established by the initiator (the acceptor is the peripheral); ESP-BLE-AUDIO sets up the ISO data path and delivers the received SDUs to the application through the recv callback on the ISO task. As on the initiator, the application never calls the ESP-BLE-ISO ISO API directly.

Broadcast Source
~~~~~~~~~~~~~~~~

A broadcast source configures its audio, starts a periodic advertising train carrying the BASE and transmits audio into a Broadcast Isochronous Group without any connection:

.. mermaid::

   sequenceDiagram
       participant APP as Application (broadcast source)
       participant AUD as ESP-BLE-AUDIO
       participant ISO as ESP-BLE-ISO
       participant CTRL as Controller
       APP->>AUD: create broadcast source (codec, QoS, BASE)
       APP->>ISO: start extended and periodic advertising (GAP, carries the BASE)
       APP->>AUD: start the broadcast source
       AUD->>ISO: create BIG
       ISO->>CTRL: LE Create BIG
       CTRL->>ISO: BIG Complete event
       AUD->>ISO: set up the ISO data path (input)
       AUD->>APP: stream started callback
       APP->>CTRL: send ISO SDUs
       CTRL->>ISO: ISO transmit complete
       Note over APP,CTRL: subsequent SDUs repeat the send / transmit-complete cycle

The application drives the high-level flow through the ``esp_ble_audio_*`` API -- create the broadcast source, start advertising, start the source and send audio -- while ESP-BLE-AUDIO builds the BASE, creates the BIG on top of ESP-BLE-ISO's ISO subsystem and sets up the data path. As with the unicast initiator, the underlying ESP-BLE-ISO operations are carried out by ESP-BLE-AUDIO, not the application. A broadcast source has no connection and no GATT: it only advertises and transmits.

Broadcast Sink
~~~~~~~~~~~~~~

A broadcast sink synchronizes to a broadcaster's periodic train, joins its Broadcast Isochronous Group and receives audio:

.. mermaid::

   sequenceDiagram
       participant APP as Application (broadcast sink)
       participant AUD as ESP-BLE-AUDIO
       participant ISO as ESP-BLE-ISO
       participant CTRL as Controller
       APP->>ISO: scan and synchronize to periodic advertising
       CTRL->>ISO: BIGInfo report
       AUD->>ISO: BIG sync (with broadcast code if encrypted)
       ISO->>CTRL: LE BIG Create Sync
       CTRL->>ISO: BIG Sync Established event
       AUD->>ISO: set up the ISO data path (output)
       CTRL->>ISO: ISO SDUs
       ISO->>APP: recv callback on the ISO task
       Note over APP,CTRL: subsequent ISO SDUs repeat the receive / recv-callback cycle

The application drives the high level -- periodic-advertising synchronization (through GAP) and the broadcast-sink start -- while the ESP-BLE-AUDIO broadcast-sink profile performs the BIG sync and the data path. Both rest on ESP-BLE-ISO: its scanning layer for PA sync and the BIGInfo report, its ISO subsystem for the BIG sync and data path. If the group is encrypted and the broadcast code is wrong, the controller still synchronizes but the stream fails its message-integrity check shortly afterward, so an application should treat an early post-sync failure as a likely bad code.

.. _arch-audio-api:

Public API
----------

The application drives the ESP-IDF Bluetooth LE Audio implementation through the ``esp_ble_audio_*`` API. It has two surfaces: one common header, ``api/include/esp_ble_audio_common_api.h``, for bring-up and the cross-cutting entry points, and one ``<profile>_api.h`` header per profile and service for the role-specific operations -- the bulk of the API, cataloged in :ref:`Clients and Controllers <arch-audio-profiles>`.

Shape and Conventions
~~~~~~~~~~~~~~~~~~~~~

The common API sits on the ESP-BLE-ISO :ref:`transport <arch-iso-transport>` and the prebuilt profile library, and follows the same conventions as the transport's public API:

- **Two-call bring-up.** ``esp_ble_audio_common_init`` takes an ``esp_ble_audio_init_info_t`` -- a GAP callback and a GATT callback, the single application sinks of :ref:`Application Event Interface <arch-app-event>`. ``esp_ble_audio_common_start`` takes an ``esp_ble_audio_start_info_t`` carrying the coordinated-set (CSIS) service instances to start. What the two calls do internally is the subject of :ref:`Initialization Flow <arch-init-flow>`: a handshake with the prebuilt library -- the shared-structure ABI (Application Binary Interface) check and the Kconfig configuration push -- followed by the per-service registration.
- **Opaque, transport-aliased types.** The event types an application handles -- ``esp_ble_audio_gap_app_event_t`` and ``esp_ble_audio_gatt_app_event_t`` -- are typedefs of the transport's ``bt_le_gap_app_event`` / ``bt_le_gatt_app_event``, and the ``ESP_BLE_AUDIO_GAP_EVENT_*`` / ``ESP_BLE_AUDIO_GATT_EVENT_*`` codes alias the transport's, so the audio and ISO layers present one event model.
- **Error codes.** Every function returns ``esp_err_t``.
- **Per-role registration.** A profile role follows the uniform shape from :ref:`Clients and Controllers <arch-audio-profiles>`: enable the role's Kconfig option, register its callback structure through the role's ``<profile>_api.h``, and drive it through that header's functions. Registering a role is also what adds its GATT service to the table (see :ref:`deferred registration <arch-audio-deferred-add>`).

Functional Groups
~~~~~~~~~~~~~~~~~

.. list-table:: ESP-BLE-AUDIO public API by purpose
   :header-rows: 1
   :widths: 24 42 34

   * - Group
     - Representative functions
     - Purpose
   * - Initialization
     - ``esp_ble_audio_common_init``
     - Register the GAP and GATT callbacks and bring up the prebuilt library, GAP and GATT.
   * - Start
     - ``esp_ble_audio_common_start``
     - Register the coordinated-set services (CSIS, CAS) from ``start_info`` and start the GATT server (the second BTA phase on Bluedroid).
   * - GATT discovery
     - ``esp_ble_audio_gattc_disc_start``
     - Start GATT discovery of a peer's services on a connection.
   * - Event forwarding
     - ``esp_ble_audio_gap_app_post_event``,\ |br|\ ``esp_ble_audio_gatt_app_post_event``
     - Forward host GAP and GATT events into the engine (NimBLE only -- see below).
   * - LTV helpers
     - ``esp_ble_audio_data_parse``,\ |br|\ ``esp_ble_audio_data_get_val``
     - Parse the length-type-value metadata Bluetooth LE Audio uses throughout.
   * - Per-profile roles
     - the ``<profile>_api.h`` register and operation functions
     - Take and drive a profile role -- the bulk of the API (see :ref:`Clients and Controllers <arch-audio-profiles>`).

Per-Host Integration Differences
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As with ESP-BLE-ISO, the only host-divergent entry points are the event-forwarding functions, and they exist for the same reason -- letting the engine see the GAP and GATT events for a connection the application drives:

- ``esp_ble_audio_gap_app_post_event`` and ``esp_ble_audio_gatt_app_post_event`` are **needed only on NimBLE**: the host delivers GAP and GATT events to the callbacks the application registered, and the application forwards them inward with these. On Bluedroid the adapter installs its own BTA/BTM callbacks and captures the events directly, so no forwarding is required -- ``esp_ble_audio_gatt_app_post_event`` is even hidden from Bluedroid builds, so calling it there is a compile-time error.
