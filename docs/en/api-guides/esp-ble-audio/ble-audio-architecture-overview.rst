.. _ble-audio-architecture:

ESP-IDF Bluetooth LE Audio Architecture
=======================================

:link_to_translation:`zh_CN:[中文]`

This document describes the internal architecture of the ESP-IDF Bluetooth LE Audio stack together with the general-purpose BLE transport it is built on, across two Bluetooth components:

- **ESP-BLE-ISO** -- a general-purpose BLE *transport* layer. It provides the GATT, GAP, ISO, L2CAP and HCI primitives, together with the dedicated processing task and locking model that serialize all host events. It is **not specific to Bluetooth LE Audio** -- it is a self-contained transport that any upper-layer profile can build on, and that can also **run on its own**: it has its own public API (``esp_ble_iso_*``) and its own initialization, so an application can use it directly without ESP-BLE-AUDIO or any profile layer above it.
- **ESP-BLE-AUDIO** -- the upper *profile* layer. It implements the Bluetooth LE Audio profiles and services (PACS, ASCS, BASS, BAP, CAP, VCP, MICP, CSIP, MCP, CCP, TMAP, and so on) and exposes them through the public ``esp_ble_audio_*`` API.

.. note::

   ESP-BLE-AUDIO is currently the only consumer of ESP-BLE-ISO, but it is not the only intended one, and it is not required at all.

   ESP-BLE-ISO is an independent component: it can be built and initialized on its own and driven through its public ``esp_ble_iso_*`` API for ISO-only use (CIS or BIS without any profile layer) -- see :ref:`ESP-BLE-ISO Public API <arch-iso-api>`.

   The transport is also profile-agnostic: its GATT, GAP, ISO, L2CAP and HCI primitives are general BLE building blocks, and other profiles -- for example a future HID-over-ISO -- are expected to build on the same component. Wherever this document describes ESP-BLE-ISO, the behavior applies to any consumer, not only to Bluetooth LE Audio.

Both components run on either of two BLE host stacks -- **Bluedroid** or **NimBLE** -- behind a common interface. Making the *behavioral differences* between the two hosts explicit is a primary goal of this document, because they determine the task context in which callbacks run, the order of operations during connection setup, and how events are dispatched.

.. note::

   This guide serves two audiences. The *Overview* sections give a layer-level mental model usable by anyone integrating the public API of either component (:ref:`ESP-BLE-ISO <arch-iso-api>`, :ref:`ESP-BLE-AUDIO <arch-audio-api>`). The later sections descend to the implementation level (tasks, queues, locks, per-host dispatch paths) for maintainers working on the components themselves. Source is referenced by file path and symbol name rather than line number, so the references stay valid as the code evolves.

.. |br| raw:: html

   <br/>

.. contents:: Table of Contents
   :local:
   :depth: 1

Architecture Overview
---------------------

Layered Stack
~~~~~~~~~~~~~

ESP-IDF Bluetooth LE Audio is organized as a stack of cooperating layers. Each layer talks only to the layer directly below it, and all host-specific knowledge is confined to the *adapter* sublayer inside each component.

.. mermaid::

   flowchart TB
       APP["Application / Examples (app task)"]
       AUDIO["ESP-BLE-AUDIO —<br/>Bluetooth LE Audio profiles and services<br/>(esp_ble_audio_* API)"]
       ISO["ESP-BLE-ISO — transport primitives:<br/>GATT, GAP, ISO, L2CAP, HCI<br/>all serialized by the global ISO lock —<br/>most events run on the ISO task event loop"]
       HOST["BLE Host Stack<br/>(Bluedroid or NimBLE)"]
       CTRL["BLE controller"]
       APP <--> AUDIO
       AUDIO <--> ISO
       ISO <--> HOST
       HOST <--> CTRL
       ISO <-. ISO Data Path .-> CTRL

.. note::

   The solid links are the control path, which follows the layering in both directions -- commands flow down, events flow up. The **ISO data path** is the exception (the dashed link): it bypasses the host stack in both of the Core Specification's data-path directions. On the **Input** data path (Host to Controller), ESP-BLE-ISO writes SDUs straight to the controller transport, and the controller reports transmit-complete back through a callback into the ISO task; on the **Output** data path (Controller to Host), the controller delivers received ISO data through the same kind of callback. Neither path goes through the host stack's HCI event dispatch, which keeps the high-rate audio data path short.

The application calls only the public ``esp_ble_audio_*`` API (or, for ISO-only use cases, ``esp_ble_iso_*``). Everything below the public API is internal and may change between releases.

Two Components
~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 16 40 44

   * - Component
     - Responsibilities
     - Key directories
   * - ESP-BLE-ISO
     - - ACL connection management
       - Advertising and scanning
       - GATT client and server
       - ISO (CIS and BIS)
       - HCI and L2CAP
       - ISO task event loop
       - ``esp_ble_iso_*`` API
     - - ``host/common``
       - ``host/adapter/bluedroid``
       - ``host/adapter/nimble``
       - ``api/include``
   * - ESP-BLE-AUDIO
     - - Bluetooth LE Audio GATT services
       - Profile clients and controllers
       - Object Transfer Service (OTS)
       - ``esp_ble_audio_*`` API
     - - ``host/common``
       - ``host/adapter/bluedroid/profiles``
       - ``host/adapter/nimble/profiles``
       - ``host/services/ots``
       - ``api/include``

.. _arch-dual-host:

Dual-Host Design
----------------

Each component is split into a **host-agnostic common** part and a **host-specific adapter** part. The directory under ``host/common`` holds one implementation that does not depend on the host stack, while ``host/adapter/bluedroid`` and ``host/adapter/nimble`` hold the code that talks to each host directly. Exactly one adapter is compiled, selected by the active host (``CONFIG_BT_BLUEDROID_ENABLED`` or ``CONFIG_BT_NIMBLE_ENABLED``). The common layer defines the internal interface that both adapters implement, so the upper layers never branch on the host stack.

The two hosts are *not* behaviorally identical. They differ in the task their callbacks run on, in how an inbound event reaches the ISO task, and in a number of procedure-level details (MTU exchange, service discovery, ISO setup) that are covered in the per-layer sections. The structural differences are summarized below; the most important one for application developers -- the task a callback runs in -- is detailed in :ref:`Callback Execution Context <arch-callback-context>`.

.. list-table:: Host-level differences
   :header-rows: 1
   :widths: 26 37 37

   * - Aspect
     - Bluedroid
     - NimBLE
   * - Task that raises inbound events
     - The BTU task (BTA GATT and GAP callbacks run here).
     - The NimBLE host task.
   * - How the adapter forwards an event
     - A BTA callback builds an event and posts it to the ISO task queue.
     - A ``*_cb_safe`` wrapper takes the ISO lock first; most events are then posted to the ISO task queue.
   * - Synchronous exception
     - GATT-server attribute access is completed asynchronously: the request is posted to the ISO task, and the response is sent later.
     - GATT-server attribute access is completed inline on the host task, because NimBLE requires the value to be returned synchronously.
   * - ISO task CPU core
     - Pinned to the configured Bluedroid core.
     - Pinned to the configured NimBLE core.
   * - ISO task priority
     - Matched to the BTU task priority.
     - Matched to the NimBLE host task priority.

.. note::

   The ISO task is deliberately given the same CPU core and the same priority as the host stack's main task (see ``host/common/include/common/task.h``). Running the transport task at the host's priority keeps the two cooperatively scheduled instead of preempting each other.

Component Internals
-------------------

The implementation details of each component are listed in the table below, covering tasks, queues, locks, and the per-host dispatch paths, for maintainers working inside the component:

.. list-table::
   :header-rows: 1
   :widths: 26 74

   * - Page
     - What it covers
   * - :ref:`ESP-BLE-ISO <arch-iso-transport>`
     - - concurrency model -- the single ISO task and global lock, and the task each callback runs on
       - connection management
       - advertising, scanning and periodic-advertising sync
       - HCI command path
       - ISO subsystem -- CIS and BIG state machines, and the SDU data path
       - GATT client and server, and the ATT MTU exchange
       - L2CAP (draft) and the application event interface
       - ``esp_ble_iso_*`` public API
   * - :ref:`ESP-BLE-AUDIO <arch-audio>`
     - - open profile adapters and the prebuilt library
       - GATT services and the service-adapter pattern
       - profile clients and controllers
       - object transfer (draft)
       - initialization sequence and ATT-MTU sizing
       - end-to-end unicast and broadcast flows
       - ``esp_ble_audio_*`` public API

.. toctree::
   :hidden:
   :maxdepth: 1

   ble-audio-architecture-iso
   ble-audio-architecture-lea

Cross-Cutting Concerns
----------------------

This section collects the invariants and practical concerns that span both components and recur throughout the implementation.

Host Parity
~~~~~~~~~~~

The most important maintenance invariant is that the two host adapters stay behaviorally equivalent. The common layers above them assume identical behavior, so a change to one adapter's connection, cleanup, discovery or event path must be mirrored in the other -- otherwise the upper layers behave differently depending on the host. Where the two genuinely cannot match, the asymmetry is deliberate, and the complete set is small:

.. list-table:: Intentional per-host differences
   :header-rows: 1
   :widths: 30 35 35

   * - Area
     - Bluedroid
     - NimBLE
   * - Task that raises inbound events
     - BTU task
     - NimBLE host task
   * - GATT-server attribute access
     - Posted to the ISO task; response sent asynchronously through BTA
     - Inline on the host task; value returned synchronously
   * - GATT-client read/write completion
     - Posted to the ISO task
     - Inline on the host task
   * - HCI ISO commands
     - Private direct-HCI path with its own completion semaphore
     - Typed ``ble_hs_hci_*`` helpers
   * - GATT service registration
     - Two-phase and asynchronous (``*_init`` then ``*_start``)
     - Single-phase ``ble_gatts_add_svcs``
   * - L2CAP connection-oriented channels
     - Not implemented (TODO)
     - Implemented (Draft)
   * - Object Transfer Service (OTS)
     - Not implemented (builds on L2CAP)
     - Implemented (Draft)
   * - Connection-initiation event routing
     - Share the engine's BTA GATTC interface (``esp_ble_iso_bluedroid_get_gattc_if``)
     - Forward GAP events into the engine (``*_gap_app_post_event``)

Any divergence beyond these needs the same kind of justification, recorded at the point where it is introduced.

Layer Boundaries
~~~~~~~~~~~~~~~~

The architecture depends on a few boundaries that callers and maintainers should respect:

- **The public API is the only stable surface.** Everything below the ``esp_ble_iso_*`` and ``esp_ble_audio_*`` API is internal and may change between releases.
- **The two BLE host stacks are adapted, not modified.** The adapter layers exist precisely so the components never change Bluedroid or NimBLE themselves; host behavior is taken as given.
- **The profile and controller logic is a prebuilt library with a fixed ABI.** The open code interacts with it only through the shared function and configuration tables that are validated at initialization; its internals are not part of this contract.

Common Pitfalls
~~~~~~~~~~~~~~~

The concurrency model in :ref:`Concurrency and Thread Safety <arch-concurrency>` has practical consequences that are easy to get wrong:

- **Do not block in a callback.** Most callbacks run on the ISO task; blocking there stalls every other event in the stack. Its stack is only 4 KB, so deep call chains and large stack-allocated buffers -- including arrays sized from configuration values -- must be avoided.
- **Posted events do not race each other.** Because a single task drains the ISO task queues, two events that were both posted cannot run concurrently; reasoning that assumes they can is almost always wrong.
- **Mind the callback's task on NimBLE.** A GATT read or write completion, or an inbound write to a local service, runs on the NimBLE host task rather than the ISO task; code shared between paths must be correct on both.
- **The ISO lock orders access, not completion.** Holding it guarantees mutual exclusion, not that an operation you started has finished -- many transport operations complete on a later event.

Memory Footprint
~~~~~~~~~~~~~~~~

The open ESP-BLE-ISO and ESP-BLE-AUDIO code has small, deterministic fixed costs:

- The ISO task uses a 4 KB stack and three priority event queues (32 + 64 + 32 entries) drained by one consumer.
- The ISO engine's channel, group and broadcast pools are sized by ``CONFIG_BT_ISO_MAX_CHAN``, ``CONFIG_BT_ISO_MAX_CIG`` and ``CONFIG_BT_ISO_MAX_BIG``; the HCI command pool holds a single buffer.
- Per connection, NimBLE caches the discovered attribute database and holds deep-copied parameters for GATT procedures in flight; subscriptions are tracked in per-connection lists on both hosts.
- GATT service attribute tables are allocated at initialization, sized by the services that are enabled.

Because these pools and tables are sized from Kconfig, the open code's footprint is fixed at build time, with no per-stream dynamic growth beyond the SDU buffers in flight. The prebuilt profile-and-controller library is the exception: it allocates its own profile and controller state dynamically -- at initialization and as connections and streams are set up -- so that part of the footprint is not fixed at build time.

Appendix -- File and Directory Map
----------------------------------

ESP-BLE-ISO
~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 34 66

   * - Directory
     - Contents
   * - ``api/include``
     - The public ``esp_ble_iso_*`` header.
   * - ``host/common``
     - Host-agnostic transport: connection, advertising, scanning, GATT, HCI, ISO glue, L2CAP, the ISO task and the ISO lock; ``app/`` holds the application GAP and GATT event surface.
   * - ``host/iso``
     - The ISO engine (CIS and BIG state machines and channel API).
   * - ``host/adapter/bluedroid``,\ |br|\ ``host/adapter/nimble``
     - Per-host adapters for GAP, GATT, ISO, HCI and (NimBLE) L2CAP.
   * - ``host/utils``
     - Address, UUID, CRC, crypto, timer and buffer helpers.

ESP-BLE-AUDIO
~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 34 66

   * - Directory
     - Contents
   * - ``api/include``
     - Public ``esp_ble_audio_*_api.h`` headers, one per profile and service.
   * - ``host/common``
     - Initialization orchestration (``init.c``).
   * - ``host/adapter/bluedroid/profiles``,\ |br|\ ``host/adapter/nimble/profiles``
     - The GATT service adapters (PACS, ASCS, BASS, CAS, CSIS, HAS, MCS, MICS, TBS, TMAS, VCS).
   * - ``host/services/ots``
     - The Object Transfer Service.
   * - ``lib``
     - The prebuilt per-target profile and controller library.
