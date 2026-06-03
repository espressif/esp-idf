.. _arch-iso-transport:

ESP-BLE-ISO
===========

:link_to_translation:`zh_CN:[中文]`

.. |br| raw:: html

   <br/>

.. contents:: Table of Contents
   :local:
   :depth: 2

ESP-BLE-ISO provides every BLE transport primitive an upper-layer profile needs: ACL connection state, advertising and scanning, periodic-advertising sync, the HCI command path, ISO (CIS and BIS), GATT, GAP and L2CAP, and the ISO task event loop and global lock described in :ref:`Concurrency and Thread Safety <arch-concurrency>`. None of it is audio-specific; ESP-IDF Bluetooth LE Audio is simply the profile layer that currently sits on top. This chapter covers the whole component in turn: connection management, advertising and scanning, the HCI command path, the ISO subsystem, GAP, GATT and L2CAP, and the public API.

Two conventions recur throughout the component and are worth stating once:

- **The** ``_safe`` **suffix.** Most internal operations exist as a pair: a core function that assumes the global ISO lock is already held, and a wrapper of the same name with a ``_safe`` suffix that acquires the lock, calls the core function, and releases it. Public entry points and callers from outside the ISO task use the ``_safe`` variant; code already running under the lock (an ISO task handler, or another core function) calls the bare variant directly. This is how the single global lock from :ref:`Protecting Shared State <arch-host-lock>` is applied uniformly without re-entering it accidentally -- although the mutex is recursive, the split keeps the lock boundary explicit at every call site.
- **The** ``bt_le_*`` / ``bt_*`` **naming.** Symbols beginning with ``bt_`` are the host-agnostic interface defined in ``host/common``; symbols beginning with ``bt_le_bluedroid_`` or ``bt_le_nimble_`` are the adapter implementations that exactly one build compiles.

  - One family is exempt: the ``hci_le_*`` ISO event handlers in ``host/common`` (for example ``hci_le_biginfo_adv_report``) are named after the HCI event they decode rather than the ``bt_`` interface, and are internal handlers rather than part of the public surface.

.. _arch-concurrency:

Concurrency and Thread Safety
-----------------------------

All ESP-IDF Bluetooth LE Audio processing above the host stack runs on a single task -- the **ISO task** (``iso_task``) -- and all access to shared state is serialized by a single global recursive mutex, the **ISO lock** (``bt_le_host_lock``). These two mechanisms together are what make the stack thread-safe; understanding them is the key to reasoning about ordering and races.

ISO Task Event Loop
~~~~~~~~~~~~~~~~~~~

The ISO task (in ``host/common/task.c``) is a single FreeRTOS task that loops forever, draining work the rest of the stack posts to it with ``bt_le_iso_task_post()``. Each item carries an event-type tag, and the task dispatches it to the handler for that category -- timer, GAP, GATT, ISO HCI, ISO transmit-complete, or ISO receive-data.

Posted work is not one queue but **three priority tiers**, each a separate FreeRTOS queue, joined into one queue set the task blocks on. On each wakeup the task services the tiers **strictly by priority** -- critical first, then normal, then floodable -- processing one item per loop and re-checking the critical tier first on the next:

.. list-table:: ISO task priority tiers
   :header-rows: 1
   :widths: 16 12 22 50

   * - Tier
     - Depth
     - On overflow
     - Events
   * - **Critical**
     - 32
     - drop newest
     - ISO receive-data and transmit-complete -- the latency-critical data path, posted non-blocking from the controller task.
   * - **Normal**
     - 64
     - blocks (never dropped)
     - Timer, GAP lifecycle, GATT and ISO HCI events -- reliable; the producer blocks until space frees.
   * - **Floodable**
     - 32
     - drop newest
     - High-volume best-effort GAP reports -- extended-advertising, periodic-advertising and BIGInfo reports -- posted non-blocking.

The split exists so a burst of GAP reports cannot delay ISO data: the critical tier is always drained first, and the two non-blocking tiers drop their newest item rather than stall their producer when full. Which tier an event lands in is independent of the handler that runs it -- the advertising and periodic-advertising reports on the floodable tier are dispatched by the same ``bt_le_gap_handle_event`` as the normal-tier GAP events, and a floodable BIGInfo report by the same ``bt_le_iso_handle_hci_event`` as a normal ISO HCI event.

.. note::

   A single task drains all three tiers, so events are still processed **one at a time and never overlap**, whatever task produced them -- the serialization guarantee the rest of the stack relies on is unchanged. What the tiers change is **ordering**: events are dispatched in priority order, not in the order they were posted, so a higher-priority ISO data event can be serviced ahead of an earlier-posted GAP report. Code that relies on the serialization guarantee stays correct; code that assumed strict first-in, first-out ordering across event categories does not.

A few properties of the task are worth keeping in mind:

- It runs on a **4 KB stack**. Every upper layer, including application and profile callbacks, executes on this stack, so deep call chains and large stack-allocated buffers (for example arrays sized from configuration values) must be avoided.
- Each queued item carries an event-type tag and a heap-allocated payload; the producer allocates the payload and the handler frees it after processing -- except an item dropped on a full non-blocking tier, which the producer frees instead.
- Its CPU core and priority are matched to the active host stack, as described in :ref:`Dual-Host Design <arch-dual-host>`.

An optional **dispatch monitor** (``CONFIG_BT_ISO_DISPATCH_MONITOR``, off by default) times every callback the task dispatches and keeps per-event-type statistics -- a count, the maximum duration, and a slow-count for callbacks exceeding ``CONFIG_BT_ISO_DISPATCH_THRESHOLD_US`` (default 2000 us, about a quarter of a 7.5--10 ms SDU interval). The table is logged periodically (``CONFIG_BT_ISO_DISPATCH_DUMP_PERIOD_S``, default 10 s) and at deinit, to surface a callback that runs long enough to delay the ISO data path. It adds per-dispatch timing overhead and measures wall-clock time, so it is intended for profiling only.

.. _arch-host-lock:

Protecting Shared State
~~~~~~~~~~~~~~~~~~~~~~~

Shared state -- the connection table, the periodic-advertising sync table, the GATT subscription lists, the GATT-server configuration, the ISO bookkeeping -- is reachable from more than one task. The stack does **not** use fine-grained, per-structure locks. Instead, thread safety relies on one coarse-grained **global recursive mutex** -- the **ISO lock**, ``bt_le_host_lock`` / ``bt_le_host_unlock`` (implemented in ``host/common/host.c`` over a Zephyr ``k_mutex``).

The rule is simple and applies everywhere: **any code path that touches shared state acquires this one mutex first.** In practice that means:

- **Public API functions**, called from the application task, take the lock on entry and release it before returning.
- **ISO task event handlers** take the lock around the part of each handler that reads or mutates shared state.
- **NimBLE synchronous callbacks** (subscription changes, received notifications, the GATT-server attribute access path) take the lock before touching shared state.
- **Timer callbacks** take the lock as well.

Because every reader and writer of a given variable holds the same lock, no two tasks can ever touch it concurrently. The mutex is **recursive**, so a handler that already holds the lock can call helper functions that re-acquire it without deadlocking.

If the lock cannot be acquired within a short bounded time, ``bt_le_host_lock`` calls ``abort()`` rather than continuing. A lock that is held that long means the stack is wedged (a deadlock or a stuck callback), which is a programming error rather than a runtime condition to recover from. ``abort()`` is used deliberately instead of ``assert()``, because ``assert()`` becomes a no-op in ``NDEBUG`` builds and would let the caller enter the critical section without the lock held, reintroducing the race. The unlock path additionally verifies that the calling task is the current holder, catching unbalanced unlocks.

.. important::

   Two distinct serialization mechanisms are in play, and they cover different paths:

   #. The **single consumer** behind the three priority queues serializes everything *posted* to the ISO task (the GAP, GATT, ISO and timer events that originate from the host stack). Posted events are processed one at a time and never overlap, though across tiers they are dispatched in priority order rather than post order.
   #. The **global ISO lock** serializes the *other* contexts -- direct application API calls and the NimBLE synchronous callbacks -- against the ISO task handlers.

   A bug report that assumes two posted events run concurrently is almost always a false positive: the single consumer rules that out.

.. _arch-callback-context:

Callback Execution Context
~~~~~~~~~~~~~~~~~~~~~~~~~~

A callback that the application registers (a GAP event handler, a GATT attribute handler, a profile event callback) does not always run on the same task, and the task **differs between the two hosts** for one important case. Knowing the context matters: a callback that runs on the ISO task shares its 4 KB stack and must not block it, while a callback that runs on the host task executes synchronously inside the host's attribute-access path.

.. list-table:: Which task a registered callback runs on
   :header-rows: 1
   :widths: 44 28 28

   * - Callback category
     - Bluedroid
     - NimBLE
   * - GAP events (connect, disconnect, security change, PA sync)
     - ISO task
     - ISO task
   * - GATT client notifications and discovery results
     - ISO task
     - ISO task
   * - GATT client read and write completions
     - ISO task
     - **NimBLE host task**
   * - ISO events (connect, data, transmit-complete)
     - ISO task
     - ISO task
   * - Timer callbacks
     - ISO task
     - ISO task
   * - GATT-server attribute read/write and CCC subscription, and any profile callback driven synchronously by an inbound write
     - ISO task
     - **NimBLE host task**

Two rows place a callback on the NimBLE host task rather than the ISO task. The GATT-client read and write completions land there because NimBLE reports a procedure result through a completion callback on its host task, and the adapter invokes the application callback inline under the ISO lock instead of re-posting it -- whereas Bluedroid posts every BTA GATT event to the ISO task. The GATT-server attribute-access row has a sharper cause: the two hosts complete the access in opposite ways. On Bluedroid the request is posted to the ISO task, the registered callback runs there, and the response is sent **asynchronously** afterwards, as illustrated below:

.. mermaid::

   sequenceDiagram
       participant P as Peer
       participant BTU as BTU task
       participant T as ISO task
       participant CB as attr read/write callback
       P->>BTU: ATT read / write request
       BTU->>T: post GATTS event
       T->>CB: invoke callback (under ISO lock)
       CB-->>T: value / status
       T->>BTU: BTA_GATTS_SendRsp (async)
       BTU-->>P: ATT response

On NimBLE, by contrast, the host requires the value to be returned **synchronously**, so the callback runs inline on the host task and hands the value straight back:

.. mermaid::

   sequenceDiagram
       participant P as Peer
       participant N as NimBLE host task
       participant CB as attr read/write callback
       P->>N: ATT read / write request
       N->>CB: invoke callback inline (under ISO lock)
       CB-->>N: value / status
       N-->>P: send response synchronously

.. _arch-event-flow:

Cross-Layer Event Flow
~~~~~~~~~~~~~~~~~~~~~~

The diagram below traces a typical inbound event -- a GAP, GATT-client or ISO event raised by the controller -- from the controller up to the application callback, and shows where the two hosts differ.

.. mermaid::

   %%{init: {'sequence': {'noteAlign': 'left'}}}%%
   sequenceDiagram
       participant C as BLE controller
       participant H as Host stack task
       participant A as ESP-BLE-ISO adapter
       participant T as ISO task
       participant CB as App/Profile callbacks
       C->>H: HCI event
       Note over H: Bluedroid: received on the HCI host task, callback runs on BTU<br/>NimBLE: one host task does both
       H->>A: host callback
       Note over A: Bluedroid: BTA callback<br/>NimBLE: *_cb_safe (takes ISO lock)
       A->>A: build event payload on the heap
       A->>T: bt_le_iso_task_post() — enqueue
       T->>T: dequeue, take ISO lock
       T->>CB: invoke callback in ISO task context
       Note over C,T: ISO data / tx-complete is the exception — the controller calls a registered callback directly, bypassing the Host stack
       C-->>T: bt_le_iso_task_post() — enqueue (direct callback, on the controller task)
       T->>T: dequeue, take ISO lock
       T->>CB: channel callback in ISO task context

Where the application and profile callbacks run follows from this flow, and it differs by host:

- On **Bluedroid**, **every upper-layer callback runs in the ISO task context**, whatever triggered it -- GAP, GATT client (notification, discovery, read or write completion), GATT server, ISO or timer. The adapter posts every BTA event to the queue, so there is no exception. These callbacks run on the task's 4 KB stack while the ISO lock is held, and calling a blocking transport API from one stalls the entire event loop.
- On **NimBLE**, the same is true *except* for two categories, which run **inline on the NimBLE host task** rather than the ISO task: **GATT-server attribute access** (the value must be returned synchronously) and **GATT-client read and write completions** (NimBLE reports the result through a host-task completion callback that the adapter invokes directly). A profile callback driven by either -- for example a server-side control-point write handler, or a client read or write completion -- therefore runs on the host task, not the ISO task, and does not pass through the queue. These are the two rows flagged in :ref:`Callback Execution Context <arch-callback-context>`.

Layer and File Map
------------------

The component is organized into a host-agnostic ``host/common`` layer, the two ``host/adapter`` layers, a standalone ISO engine under ``host/iso``, and shared helpers under ``host/utils``. The host-agnostic files are:

.. list-table:: ``host/common`` -- host-agnostic transport
   :header-rows: 1
   :widths: 24 76

   * - File
     - Role
   * - ``host.c``
     - Init and deinit orchestration; the global recursive lock (``bt_le_host_lock`` / ``bt_le_host_unlock``).
   * - ``task.c``
     - The ISO task event loop, its three priority queues and queue set, and ``bt_le_iso_task_post()``.
   * - ``conn.c``
     - ACL connection table and the connection-event listener fan-out.
   * - ``adv.c``
     - Extended-advertising set bookkeeping.
   * - ``scan.c``
     - Scanning, periodic-advertising sync, and BIGInfo reports.
   * - ``hci.c``
     - Builds HCI command buffers and dispatches them to the active adapter.
   * - ``iso.c``
     - Decodes ISO HCI meta-events and bridges the ISO data path to the engine.
   * - ``gatt.c``
     - Host-agnostic GATT state (subscriptions, attribute database cache).
   * - ``l2cap.c``
     - Generic L2CAP channel and server dispatch.
   * - ``app/gap.c``
     - The application-facing GAP entry points.
   * - ``app/gatt.c``
     - The application-facing GATT entry points.

The ISO state machine lives apart from the rest of the common layer, in ``host/iso/iso.c``: it owns the CIG, BIG and ISO-channel logic and the static, configuration-sized pools that back them. It is the single file under ``host/iso`` and is detailed in :ref:`ISO Subsystem <arch-iso>` below.

Each adapter directory mirrors the common interface for its host: ``host/adapter/bluedroid`` and ``host/adapter/nimble`` each provide ``gap.c``, ``gatt``, ``iso.c`` and the host-specific headers, and ``host/utils`` holds address, UUID, CRC, crypto, timer and buffer helpers that neither layer needs to reimplement.

.. note::

   Two files named ``iso.c`` exist and play different roles. ``host/iso/iso.c`` is the engine -- the CIS and BIG state machines and the public ISO channel API. ``host/common/iso.c`` is the glue -- it decodes raw HCI ISO meta-events into the engine's handlers and carries the ISO data tx/rx path. The adapters' ``iso.c`` files translate engine commands into host-native calls.

Connection Management
---------------------

``conn.c`` owns the ACL connection table -- an array of ``bt_conn`` objects keyed by connection handle -- and the registration list for connection-event listeners. The connection lifecycle is driven entirely from the host adapter: when the controller reports a connection event, the adapter calls one of the listener entry points, which update the table and fan the event out to every ``bt_conn_cb`` the upper layers registered with ``bt_conn_cb_register``.

The listener entry points form the host-agnostic seam:

- ``bt_le_acl_conn_connected_listener`` and ``bt_le_acl_conn_disconnected_listener`` add and remove table entries.
- ``bt_le_acl_conn_security_changed_listener``, ``..._identity_resolved_listener``, ``..._pairing_completed_listener`` and ``..._bond_deleted_listener`` carry the security and bonding events.

Because the adapter posts these through the ISO task (see :ref:`Dual-Host Design <arch-dual-host>`), the registered ``bt_conn_cb`` callbacks run in the ISO task context on both hosts -- the GAP-events row of :ref:`Callback Execution Context <arch-callback-context>`. Operations that act on a live connection (``bt_conn_set_security``, ``bt_conn_disconnect``, ``bt_conn_get_info``) and the lookup helpers (``bt_conn_lookup_handle``, ``bt_le_acl_conn_find``, ``bt_conn_foreach``) read from the same table under the ISO lock. The host-specific *sourcing* of these events -- which host task the adapter is on when it calls a listener, and how it maps native event structures -- is covered under :ref:`Application Event Interface <arch-app-event>` below.

Advertising and Scanning
------------------------

``adv.c`` is deliberately small: it tracks extended-advertising *sets* by handle (``bt_le_ext_adv_find`` / ``bt_le_ext_adv_new_safe`` / ``bt_le_ext_adv_delete_safe``). An advertising set is the anchor a broadcast ISO group attaches to, so this table is a prerequisite for the BIG broadcaster flow in the ISO subsystem.

``scan.c`` covers three related responsibilities:

- **Scanning.** The ``bt_le_scan_cb`` registry; advertising reports arrive at ``bt_le_scan_recv_listener`` and are delivered to the application.
- **Periodic-advertising sync.** The sync table (``bt_le_per_adv_sync_new`` / ``..._delete`` / ``..._lookup_addr``) and its listeners -- ``..._establish_listener``, ``..._lost_listener`` and ``..._report_recv_listener`` -- track synchronization to a broadcaster's periodic train.
- **BIGInfo reports.** ``hci_le_biginfo_adv_report`` surfaces the BIGInfo that rides a periodic-advertising train. BIGInfo carries the parameters a receiver needs to synchronize to a Broadcast Isochronous Group, so this handler is the bridge from scanning into the BIG-receiver flow.

Periodic-advertising sync is therefore the entry point for *receiving* broadcast audio: scan, synchronize to the periodic train, read the BIGInfo, and then ask the ISO subsystem to synchronize to the BIG.

HCI Command Path
----------------

The ISO engine never talks to a host stack directly. It builds a standard HCI command buffer with ``bt_hci_cmd_create(opcode, len)`` and submits it with ``bt_hci_cmd_send_sync(opcode, buf, rsp)`` (both in ``host/common/hci.c``). ``bt_hci_cmd_send_sync`` is a one-line dispatcher to the active adapter's ISO command entry point, and this is where the two hosts diverge sharply:

.. list-table:: HCI command translation per host
   :header-rows: 1
   :widths: 20 40 40

   * - Aspect
     - Bluedroid
     - NimBLE
   * - Entry point
     - ``bt_le_bluedroid_iso_cmd_send_sync``
     - ``bt_le_nimble_iso_cmd_send_sync``
   * - Translation
     - An opcode switch; each command is forwarded as a raw HCI command over a private *direct-HCI* path.
     - An opcode switch; each command is unpacked into NimBLE's typed ``ble_hs_hci_*`` ISO helpers.
   * - Synchronization
     - A private completion semaphore in ``adapter/bluedroid/hci.c``; the completion callback runs on the HCI-layer task.
     - Handled inside the ``ble_hs_hci_*`` call.

The reason Bluedroid carries its own command path is concurrency. Bluedroid's BTU task uses a single global slot to match a synchronous command to its completion; issuing ISO commands from the ISO task through that same slot would race the BTU task. The direct-HCI path in ``adapter/bluedroid/hci.c`` sidesteps this entirely: each command carries its own completion callback and the caller waits on a dedicated semaphore, so the shared BTU slot is never touched. The callback runs on the HCI-layer task and is kept minimal -- it copies the response into a static sink and signals the semaphore, taking no lock. NimBLE needs none of this, because its ``ble_hs_hci_*`` helpers already provide a self-contained synchronous command interface.

.. mermaid::

   sequenceDiagram
       participant CORE as ISO engine (host/iso/iso.c)
       participant HCI as bt_hci_cmd_send_sync (common/hci.c)
       participant AD as Active adapter
       participant CTRL as Controller
       CORE->>HCI: build command buffer (opcode + params)
       HCI->>AD: *_iso_cmd_send_sync(opcode)
       Note over AD: Bluedroid: direct-HCI (private sem)<br/>NimBLE: ble_hs_hci_* typed helper
       AD->>CTRL: HCI command
       alt Command Complete (e.g. Set CIG Parameters)
           CTRL-->>AD: Command Complete
           AD-->>CORE: status + return parameters
       else Command Status (e.g. Create CIS)
           CTRL-->>AD: Command Status
           AD-->>CORE: status (no return parameters)
           Note over CORE,CTRL: real outcome arrives later as an LE meta-event<br/>(e.g. CIS Established),<br/>handled in the ISO Subsystem below
       end

.. _arch-iso:

ISO Subsystem
-------------

The ISO subsystem is split across three places: the engine (``host/iso/iso.c``), the meta-event and data glue (``host/common/iso.c``), and the adapters (``host/adapter/*/iso.c``). The engine owns three static, configuration-sized pools -- one for ISO channels (``CONFIG_BT_ISO_MAX_CHAN``), one for Connected Isochronous Groups (``CONFIG_BT_ISO_MAX_CIG``), and one for Broadcast Isochronous Groups (``CONFIG_BT_ISO_MAX_BIG``). Every public engine operation follows the ``_safe`` convention from the start of this section.

Inbound ISO meta-events follow the same shape on both hosts: the adapter registers a host-native ISO event callback, packages the event, and posts an ``ISO_HCI_EVENT`` item to the ISO task; ``host/common/iso.c`` then decodes the LE subevent and dispatches it to the engine. One wire-format difference is absorbed here: NimBLE's meta-event structures already include the subevent code, while the Bluedroid adapter prepends it, so the decoder in ``host/common/iso.c`` sees a uniform layout regardless of host.

.. mermaid::

   %%{init: {'sequence': {'noteAlign': 'left'}}}%%
   sequenceDiagram
       participant CTRL as Controller
       participant AD as Active adapter
       participant T as ISO task
       participant CORE as ISO engine
       participant APP as App callback
       CTRL->>AD: ISO meta-event (HCI)
       Note over AD: Bluedroid: BTM ISO callback (BTU task)<br/>NimBLE: ISO callback (host task)
       AD->>T: post ISO_HCI_EVENT
       CTRL-->>T: ISO data, post ISO_RX_DATA
       T->>CORE: handle in common/iso.c, then engine handler
       CORE->>APP: channel callback (under ISO lock)

Connected ISO (CIS)
~~~~~~~~~~~~~~~~~~~

A Connected Isochronous Stream is point-to-point and rides an ACL connection. The two roles drive the engine through different entry points:

.. list-table:: CIS roles
   :header-rows: 1
   :widths: 16 44 40

   * - Role
     - Flow
     - Key symbols
   * - Central
     - Configure a group, then establish one or more streams to the peers; the controller confirms each stream as it comes up.
     - ``bt_iso_cig_create``,\ |br|\ ``bt_iso_cig_reconfigure``,\ |br|\ ``bt_iso_cig_terminate``,\ |br|\ ``bt_iso_chan_connect`` / ``hci_le_cis_established``
   * - Peripheral
     - Register a server that decides whether to accept incoming streams; each peer request is then accepted or rejected and, if accepted, confirmed.
     - ``bt_iso_server_register`` /\ |br|\ ``hci_le_cis_req`` / ``hci_le_cis_established``

Broadcast ISO (BIG)
~~~~~~~~~~~~~~~~~~~

A Broadcast Isochronous Group is connectionless and rides a periodic-advertising train rather than an ACL connection:

.. list-table:: BIG roles
   :header-rows: 1
   :widths: 16 44 40

   * - Role
     - Flow
     - Key symbols
   * - Broadcaster
     - Attach a BIG to an extended-advertising set that carries a periodic train; the controller confirms the group, after which the streams can be fed.
     - ``bt_iso_big_create`` / ``hci_le_big_complete``,\ |br|\ ``bt_iso_big_terminate`` / ``hci_le_big_terminate``
   * - Receiver
     - Synchronize to the periodic train, read its BIGInfo, then synchronize to the group; loss of sync is reported back.
     - ``bt_iso_big_sync`` / ``hci_le_big_sync_established``,\ |br|\ ``hci_le_big_sync_lost``

The receiver flow begins in ``scan.c``: the BIGInfo delivered by ``hci_le_biginfo_adv_report`` on an established periodic-advertising sync is what ``bt_iso_big_sync`` consumes to join the group.

Data Path and Data Flow
~~~~~~~~~~~~~~~~~~~~~~~

Establishing a CIS or a BIS is not enough to move audio; each stream must be bound to the controller's ISO data path. ``bt_iso_setup_data_path`` binds a direction (input for transmit, output for receive) and ``bt_iso_remove_data_path`` releases it. A stream that is connected but has no data path set up carries no SDUs: establishing the stream and binding its data path are separate steps, so audio does not flow until an upper layer explicitly sets up the path.

Once a data path is set up, SDUs flow through ``host/common/iso.c``:

- **Transmit.** The application submits an SDU with ``bt_iso_chan_send`` (or the timestamped ``bt_iso_chan_send_ts``); the engine hands it to ``bt_le_iso_tx``. If the controller has a free buffer and nothing is queued ahead, the SDU is sent straight to the controller; otherwise it is held in a host-side TX queue. The controller's transmit-complete signal returns as an ``ISO_TX_COMP`` event posted to the ISO task, where ``bt_le_iso_handle_tx_comp`` sends as many queued SDUs as the controller now has free buffers for, then invokes the application's transmit-complete callback.
- **Receive.** An incoming SDU is delivered by the adapter, packaged, and posted as an ``ISO_RX_DATA`` event; ``bt_le_iso_handle_rx_data`` passes it to ``bt_iso_recv`` in the engine, which invokes the channel's receive callback.

Both directions converge on the ISO task: transmit completion and receive delivery are ordinary queued events, so the application's ISO callbacks run in the ISO task context on its 4 KB stack, under the ISO lock, exactly like every other queued event in :ref:`Cross-Layer Event Flow <arch-event-flow>`.

The remaining ESP-BLE-ISO transport primitives are GATT, GAP and L2CAP. As with the rest of the component, the host-agnostic API (``bt_gatt_*``, ``bt_l2cap_*`` and the GAP helpers) lives in ``host/common`` and dispatches to whichever adapter is compiled.

GATT
----

GATT spans a **client** side (this device discovering, reading, writing and subscribing on a peer), a **server** side (this device exposing attributes a peer accesses), and the ATT **MTU exchange** that bounds both. All three are implemented in ``host/common/gatt.c`` over the active adapter.

Client
~~~~~~

The host-agnostic client API in ``host/common/gatt.c`` -- ``bt_gatt_discover``, ``bt_gatt_read``, ``bt_gatt_write``, ``bt_gatt_write_without_response_cb``, ``bt_gatt_subscribe`` and ``bt_gatt_unsubscribe`` -- dispatches to the active adapter's ``bt_le_*_gattc_*`` implementation. The two adapters back the same API with very different machinery:

.. list-table:: GATT client backing per host
   :header-rows: 1
   :widths: 22 39 39

   * - Aspect
     - Bluedroid
     - NimBLE
   * - Discovery
     - BTA GATTC procedures; BTA owns the discovered attribute cache.
     - A one-time full attribute-table walk cached in ``gatt.db.c``.
   * - Procedure serialization
     - BTA GATTC's own per-connection queue.
     - The NRP queue in ``gatt.nrp.c``; params and data are deep-copied on insert.
   * - Result delivery
     - Every BTA GATT event is posted to the ISO task.
     - Notifications and ``bt_gatt_discover`` results are posted to the ISO task; the peer attribute-table walk and the read and write completions run inline on the host task.

Discovery
^^^^^^^^^

Client discovery has two distinct flows that run on different tasks. The split exists because the hosts differ in whether they keep an attribute cache: Bluedroid's BTA GATTC discovers a peer's attribute table and caches it automatically, whereas NimBLE does not -- so the port adds that cache in ``gatt.db.c``.

**Walking the peer's attribute table (building the cache).** When the upper layer calls ``bt_gattc_disc_start``, the NimBLE adapter (``bt_le_nimble_gattc_db_auto_disc``) runs real ATT discovery against the peer with NimBLE's ``ble_gattc_disc_*`` procedures, walking the whole table once -- every primary service, included service, characteristic and descriptor, including each CCCD. These procedure callbacks run on the **NimBLE host task** and populate the per-connection cached database; the upper layer is notified on the ISO task only once the walk completes. On Bluedroid this step is implicit: ``bt_le_bluedroid_gattc_disc_start`` drives BTA GATTC, which performs the ATT procedures and maintains its own cache.

**Serving a discovery request.** When the upper layer -- for example the audio profiles -- calls ``bt_gatt_discover`` for specific attributes, the NimBLE adapter posts a discovery event to the **ISO task**, where the cached hierarchy answers the request locally with no further ATT traffic and the caller's callback is invoked. On Bluedroid the equivalent results surface from BTA's cache as discovery events on the ISO task.

Subscription Lifecycle
^^^^^^^^^^^^^^^^^^^^^^

Subscriptions are tracked host-agnostically. ``bt_gatt_subscribe`` records each ``bt_gatt_subscribe_params`` on a per-connection list (``gattc_sub``) and, unless an equivalent subscription already exists, writes the CCC through the adapter. Two behaviors are worth noting:

- The subscription is appended to the list *before* the CCC write completes, because some peers send the first notification before replying to the CCC write.
- The ``subscribe`` callback is invoked **synchronously**, completing the procedure in the caller's context rather than after the CCC-write response. On NimBLE this pairs with the cached database: the CCCD handle is already known, so no discovery round-trip is needed and the subscribe path does not block.

On disconnect, ``bt_le_acl_conn_disconnected_gatt_listener`` cleans up in a fixed order: the upper-layer disconnect callbacks run first, then ``gattc_sub_clear`` walks the subscription list, clears each entry's handles and value, and re-initializes the list. Upper layers must therefore **not** zero their own subscribe params inside a disconnect callback -- the params are still linked on the list at that point, and the common layer clears them immediately afterwards.

Procedure Serialization
^^^^^^^^^^^^^^^^^^^^^^^

**Background.** ``gatt.nrp.c`` exists for the same reason as ``gatt.db.c``: it supplies a service BTA GATTC provides natively but NimBLE does not. ATT permits only one outstanding request per connection at a time; BTA GATTC hides that behind its own per-connection procedure queue, whereas NimBLE leaves the caller to honor it.

**What NRP does.** The NRP (need-response PDU) queue in ``gatt.nrp.c`` is the NimBLE-side equivalent: a single per-connection queue spanning every response-bearing PDU -- the client's reads, writes and subscriptions, plus the server's indications (described in the server section below) -- so only one is in flight at a time and the rest wait their turn.

**Enqueue rules.** For the client procedures it bridges to the params-with-callback model the upper layers expect: ``bt_le_nimble_gatt_nrp_insert`` enqueues a read, write or subscribe -- dispatching it at once when the queue is idle, otherwise holding it until the in-flight procedure completes. ``bt_le_nimble_gatt_nrp_remove`` completes the head entry and starts the next; ``bt_le_nimble_gatt_nrp_clear`` discards pending entries on disconnect. Reads come in three forms -- by-UUID, long (fragmented) and single -- each delivering one event per result with explicit end-of-procedure handling.

**Deep copy.** ``bt_le_nimble_gatt_nrp_insert`` deep-copies the params and any data so the caller may pass a stack buffer and free it on return.

**EATT note.** The one-in-flight rule follows ATT's model of a single bearer with one outstanding request per connection. EATT introduces multiple concurrent bearers per connection, so supporting it would mean revisiting the NRP queue to allow per-bearer concurrency rather than a single procedure in flight.

The callback context follows from this. NimBLE delivers a read or write completion on its host task, and the NRP handler invokes the application callback there, under the ISO lock -- the same host-task context as the GATT-server attribute access. Bluedroid instead posts every BTA GATT completion to the ISO task, so the corresponding callbacks run there. This is the read-and-write-completion row of :ref:`Callback Execution Context <arch-callback-context>`.

Server
~~~~~~

The server side of ``host/common/gatt.c`` registers services (``bt_gatt_service_register``), provides the standard attribute read helpers (``bt_gatt_attr_read`` and the service, included-service, characteristic and CCC variants), the CCC write path (``bt_gatt_attr_write_ccc``, ``bt_gatts_sub_changed``) and the outbound notify and indicate API (``bt_gatt_notify_cb``, ``bt_gatt_indicate``). The behavior that differs most between hosts is how an inbound attribute access is completed, the exception first described in :ref:`Callback Execution Context <arch-callback-context>`:

- Bluedroid posts the read or write to the ISO task; the registered attribute callback runs there, and the response is returned to the peer through BTA asynchronously, coordinated with a dedicated server semaphore.
- NimBLE runs the attribute callback inline on the host task, because its access callback must return the value synchronously.

Outbound transfers follow the host's model as well. On NimBLE a notification is a synchronous buffer copy and dispatch, while an indication -- which must await the peer's confirmation -- is queued through NRP and completed when the confirmation arrives. On Bluedroid both go through BTA. In every case the payload handed to notify or indicate is copied before the call returns, so callers may reuse their buffers immediately.

.. _arch-iso-mtu:

MTU Exchange
~~~~~~~~~~~~

The ATT MTU exchange lets the two peers raise the ATT MTU above its 23-octet default so a single PDU can carry a larger attribute value. It is a GATT procedure: the **client** sends the ATT Exchange MTU Request and the **server** responds, and the smaller of the two peers' preferred values becomes the MTU in force. These GATT roles are independent of the GAP central/peripheral role -- a device usually runs both a client and a server regardless of which side opened the connection -- so the behavior is best read per GATT role.

**As a GATT server.** Both hosts always respond to the peer's ATT Exchange MTU Request and accept the negotiated value. This needs nothing from the application and is independent of the GAP role.

**As a GATT client.** Whether the device raises the MTU itself depends on the host stack and, on Bluedroid, on the GAP role:

- On Bluedroid it is automatic: the GATTC adapter issues the exchange when a GATT client connection opens (``handle_gattc_open_event`` calls ``BTA_GATTC_ConfigureMTU``, inside the ``BTA_GATTC_Enh_Open`` path). In practice this fires for a **central**; for a **peripheral** the GATT-client open happens inside the discovery-start path, which is itself gated behind the MTU-updated event -- a circular dependency that keeps it from initiating.
- On NimBLE the component initiates nothing, so the application triggers it with ``ble_gattc_exchange_mtu()`` -- typically right after the security-change event. On Bluedroid that same call is a no-op for the MTU, since the adapter has already exchanged it.

Because a GATT server only ever responds, a single exchange -- initiated by whichever side acts as client first -- settles the MTU for the whole connection; a peer that is also a client on that connection reuses the negotiated value rather than running a second exchange. The preferred value a device offers, and the reason a profile may raise it above the ATT default, are set by the layer above the transport: for Bluetooth LE Audio see :ref:`ATT MTU <arch-audio-mtu>`.

.. _arch-l2cap:

L2CAP (Draft)
-------------

.. warning::

   L2CAP connection-oriented channels are an **early draft and are not yet officially supported**. The implementation is incomplete -- it exists only on NimBLE (``host/adapter/nimble/l2cap.c``) and there is **no Bluedroid support** -- and its API and behavior are provisional and may change. Do not rely on this layer in production.

``host/common/l2cap.c`` provides credit-based L2CAP connection-oriented channels: a server registry (``bt_l2cap_server_register``), the outbound channel operations (``bt_l2cap_chan_connect``, ``bt_l2cap_chan_disconnect``, ``bt_l2cap_chan_send``) and the inbound event handlers the host calls when a channel is accepted, connected, disconnected or receives data (``bt_le_l2cap_accept``, ``bt_le_l2cap_connected``, ``bt_le_l2cap_disconnected``, ``bt_le_l2cap_received``). The only consumer in the stack is the Object Transfer Service, which moves bulk objects over a credit-based channel; profiles that do not use object transfer never open one.

.. _arch-app-event:

Application Event Interface
---------------------------

GAP and GATT events do not reach the application straight from the adapter. They pass through a thin application-event interface in ``host/common/app/gap.c`` and ``host/common/app/gatt.c`` -- the single point at which GAP and GATT events leave and enter the application. The paragraphs below follow an inbound event from the adapter up to the application callback, then the reverse injection path.

**Registration.** The application registers exactly one callback per category, through ``bt_le_gap_app_cb_register`` and ``bt_le_gatt_app_cb_register``. Each category keeps a single callback pointer, so there is one application sink for all GAP events and one for all GATT events.

**Sourcing -- adapter to ISO task.** Before the interface can deliver an event, the adapter sources it from the host and posts it to the ISO task queue. For GAP, the Bluedroid adapter registers a BTA/BTM GAP callback in ``bt_le_bluedroid_gap_init``; the callback runs on the BTU task, builds an event, and enqueues it with ``bt_le_bluedroid_gap_post_event``. The NimBLE adapter supplies a ``ble_gap_event`` callback whose ``*_cb_safe`` wrapper takes the ISO lock and posts the event. GATT events are sourced the same way by the GATT adapters, detailed under GATT above. GAP has no engine of its own beyond this -- it is only an *event surface* over state that other modules own: the ACL connection table in ``conn.c`` (ACL connect and disconnect, security and identity changes, bond deletion) and the scanning and periodic-advertising-sync state in ``scan.c`` (extended-scan reports, PA-sync establish, lost and report, BIGInfo).

**Delivery -- ISO task to application.** When the ISO task dequeues a GAP or GATT event it calls ``bt_le_gap_handle_event`` or ``bt_le_gatt_handle_event`` (from ``task.c``). These dispatch on the event type, build a typed ``bt_le_gap_app_event`` or ``bt_le_gatt_app_event``, invoke the registered callback, and free the queued payload. Because this runs inside the ISO task, the application callback executes in the ISO task context under the ISO lock -- the context described in :ref:`Callback Execution Context <arch-callback-context>`. Delivered GAP events include ACL connect and disconnect, security and identity changes, bond deletion, extended-scan reports, periodic-advertising-sync state and BIGInfo; delivered GATT events include the ATT MTU change, GATT-client discovery completion and GATT-server subscription changes.

**Injection -- application to ISO task.** The reverse direction lets the application post an event into the ISO task, through ``bt_le_gap_app_post_event`` (exposed publicly as ``esp_ble_iso_gap_app_post_event``). It is host-divergent: on Bluedroid it forwards to ``bt_le_bluedroid_gap_post_event``, on NimBLE to ``bt_le_nimble_gap_post_event``. In the configuration these components ship, the injection path is used only on NimBLE.

.. mermaid::

   flowchart TB
       ADP["Adapter<br/>(host events)"]
       T["ISO task queue"]
       AIF["app/ interface<br/>(gap.c, gatt.c)"]
       APP["Application<br/>(one GAP and<br/>one GATT callback)"]
       ADP -->|post raw event| T
       T -->|dequeue and dispatch| AIF
       AIF -->|typed app event| APP
       APP -->|app_post_event| AIF
       AIF -->|inject into queue| T

**Why the interface is shaped this way.** Three goals drive this design:

- **One event model regardless of host.** The adapter normalizes every native event -- a BTA/BTM callback on Bluedroid, a ``ble_gap_event`` on NimBLE -- into the same typed ``bt_le_gap_app_event`` / ``bt_le_gatt_app_event`` before it reaches the application. The upper layers -- the examples and the audio profiles -- therefore handle an identical set of events and never branch on the host stack. This is the most important property of the interface: moving an application between the two hosts needs no change to its event handling.
- **Internal consumers get the event too.** Several events are not only for the application: the ISO engine in ``host/iso/iso.c`` and the prebuilt audio library consume them as well -- a periodic-advertising sync and its BIGInfo drive a BIG sync, and connect, disconnect and security changes drive the profiles. The interface therefore guarantees a copy reaches the ISO task, where that internal work runs under the ISO lock. On Bluedroid the BTU-task callback posts it directly; on NimBLE the application forwards the event it received into the ISO task with ``esp_ble_iso_gap_app_post_event`` (see :ref:`Per-Host Integration Differences <arch-iso-host-diff>`).
- **Coexistence with an ordinary BLE application.** Posting a copy to the ISO task does not consume the event. On Bluedroid the same callback still forwards the native event to the Bluedroid application-callback layer (BTC), so an application that also registered through ``esp_ble_gap_register_callback`` keeps receiving it for its own, non-audio BLE work; on NimBLE the application is already inside its ``ble_gap_event`` callback and can go on to handle whatever else it needs there. An ESP-IDF Bluetooth LE Audio application and a conventional BLE application can run side by side on one device.

.. important::

   These application and profile callbacks run **on the ISO task** (see :ref:`Callback Execution Context <arch-callback-context>`), so a callback must return quickly. Blocking, sleeping, waiting on I/O, or running a long computation in one stalls the single event loop that every other GAP, GATT and -- most critically -- ISO data event depends on, which surfaces directly as late or dropped audio. Hand any lengthy work off to another task.

.. _arch-iso-api:

Public API
----------

Everything described so far is internal. An application sees only one public header, ``api/include/esp_ble_iso_common_api.h``, which exposes the transport as a small ``esp_ble_iso_*`` API for ISO-only use cases -- CIS or BIS without the Bluetooth LE Audio profiles.

Shape and Conventions
~~~~~~~~~~~~~~~~~~~~~

The public API is a thin facade over the ISO engine of the :ref:`transport layer <arch-iso-transport>`:

- **Opaque types.** Public types such as ``esp_ble_iso_chan_t``, ``esp_ble_iso_cig_t`` and ``esp_ble_iso_big_t`` are typedefs of the internal ``bt_iso_*`` structures, and ``esp_ble_conn_t`` is a typedef of ``bt_conn``. The application holds them as opaque handles.
- **Error codes.** Every function returns ``esp_err_t`` rather than the negative ``errno`` values the engine uses internally.
- **Lock on entry.** Each call enters the engine through its ``_safe`` wrapper, so the public API is the outermost point at which the global ISO lock is taken; the application never manages the lock itself.
- **One init.** ``esp_ble_iso_common_init`` takes an ``esp_ble_iso_init_info_t`` whose only field is the application's GAP callback; ISO events are then delivered through that callback or through the per-channel operations.

Functional Groups
~~~~~~~~~~~~~~~~~

.. list-table:: ESP-BLE-ISO public API by purpose
   :header-rows: 1
   :widths: 24 42 34

   * - Group
     - Representative functions
     - Purpose
   * - Initialization
     - ``esp_ble_iso_common_init``
     - Register the GAP callback and bring up the transport.
   * - CIS -- central
     - ``esp_ble_iso_cig_create``,\ |br|\ ``esp_ble_iso_cig_reconfigure``,\ |br|\ ``esp_ble_iso_cig_terminate``,\ |br|\ ``esp_ble_iso_chan_connect``
     - Configure a Connected Isochronous Group and establish its streams.
   * - CIS -- peripheral
     - ``esp_ble_iso_server_register``,\ |br|\ ``esp_ble_iso_server_unregister``
     - Accept incoming Connected Isochronous Streams.
   * - BIG -- broadcaster
     - ``esp_ble_iso_big_ext_adv_add``,\ |br|\ ``esp_ble_iso_big_create``,\ |br|\ ``esp_ble_iso_big_terminate``,\ |br|\ ``esp_ble_iso_big_register_cb``
     - Broadcast an Isochronous Group over an advertising set.
   * - BIG -- receiver
     - ``esp_ble_iso_big_sync``
     - Synchronize to a broadcast Isochronous Group.
   * - Data path
     - ``esp_ble_iso_setup_data_path``,\ |br|\ ``esp_ble_iso_remove_data_path``,\ |br|\ ``esp_ble_iso_chan_send``,\ |br|\ ``esp_ble_iso_chan_send_ts``
     - Bind a stream to the controller's ISO data path and move SDUs.
   * - Information
     - ``esp_ble_iso_chan_get_info``,\ |br|\ ``esp_ble_iso_chan_get_tx_sync``
     - Query channel and transmit-timing information.
   * - Helpers
     - ``esp_ble_iso_data_parse``
     - Parse length-type-value (LTV) encoded data.

These map one-to-one onto the engine operations in the :ref:`ISO subsystem <arch-iso>`; the public layer adds only the lock, the error translation and the opaque typedefs.

.. _arch-iso-host-diff:

Per-Host Integration Differences
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Two parts of the public API exist specifically because the hosts route connection and GAP events differently. Both solve the same problem -- letting the engine see the events for a connection the application initiated -- in the way each host requires:

- ``esp_ble_iso_gap_app_post_event`` is **needed only on NimBLE**. When the application initiates a connection or scan on NimBLE, the host delivers GAP events to the callback the application registered; the application forwards them into the engine with this function. On Bluedroid the engine installs its own BTM GAP callback and captures the events directly, so no forwarding is required.
- ``esp_ble_iso_bluedroid_get_gattc_if`` is **Bluedroid only**. It returns the engine's internal BTA GATTC interface, which the application passes to ``esp_ble_gattc_open`` when initiating a connection so the resulting ACL events route back to the engine, avoiding a second BTA GATTC registration. The NimBLE counterpart is the event forwarding above.
