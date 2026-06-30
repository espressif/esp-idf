Block Device Layer
==================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The Block Device Layer (BDL) defines a C interface that lets storage-oriented components exchange data without custom-made adapters. Each block device exposes this interface in the form of an :cpp:type:`esp_blockdev_handle_t` handle giving access to device flags, geometry information, and a set of supported operations as defined in ``components/esp_blockdev/include/esp_blockdev.h``. Higher-level code inspects that metadata and may invoke the available callbacks to perform I/O operations.

The unified interface makes it possible to compose BDL stacks supporting storage use-cases by chaining multiple universal components. A driver provides a handle representing access to the physical device; a middleware component consumes this handle, augments behaviour (for example splits the space on the device or adds wear levelling capability), and exposes a new handle to the next layer. The topmost components of the chain like file-systems are pure device consumers. This model allows filesystems, middleware, and physical drivers to be mixed and matched as long as every layer honours the interface contracts described below.

.. blockdiag::
    :caption: Example Block Device Layer Stack
    :align: center

    blockdiag blockdev-stack {
        default_fontsize = 14;
        node_height = 60;
        orientation = portrait;
        default_group_color = none;

        nvs [label = "NVS\n(nvs_flash)"];
        fatfs [label = "FATFS\n(fatfs)"];
        littlefs [label = "LittleFS\n(esp_littlefs)"];
        consumer [label = "Block device\nconsumers", shape = ellipse];

        wl [label = "Wear Levelling\n(wear_levelling)"];
        middleware_1 [label = "Block device\nmiddleware", shape = ellipse];

        nvs_part [label = "NVS Partition\n(esp_partition)"];
        littlefs_part [label = "LittleFS Partition\n(esp_partition)"];
        fat_part [label = "FAT Partition\n(esp_partition)"];
        middleware_2 [label = "Block device\nmiddleware", shape = ellipse];

        spi [label = "Flash\n(spi_flash)"];
        provider [label = "Block device\nprovider", shape = ellipse];

        d1 [shape = none, width = 1, height = 1];
        d2 [shape = none, width = 1, height = 1];
        d3 [shape = none, width = 1, height = 1];

        nvs -> nvs_part -> spi;
        fatfs -> wl -> fat_part -> spi;
        littlefs -> littlefs_part -> spi;

        consumer -> middleware_1 -> middleware_2 -> provider;

        group { orientation = landscape; fatfs; littlefs; nvs; }
        group { orientation = landscape; wl; }
        group { orientation = landscape; fat_part; littlefs_part; nvs_part; }
        group { orientation = landscape; d3; spi; }
    }


Using Block Devices
-------------------

Handles
^^^^^^^

Block devices are accessed through :cpp:type:`esp_blockdev_handle_t`. Handles are obtained from the owning component via the ``<component>_get_blockdev()`` convention and must be released with the matching ``<component>_release_blockdev()`` helper once the device is no longer needed. Treat handles as opaque: only use the public API in ``components/esp_blockdev/include/esp_blockdev.h``, and do not move or modify the memory they reference.

Geometry and Flags
^^^^^^^^^^^^^^^^^^

Each device publishes an :cpp:type:`esp_blockdev_geometry_t` structure that reports capacity together with minimum read, write, and erase granularities. Optional recommended sizes act as performance hints but must not replace alignment checks against the mandatory values. The accompanying :cpp:type:`esp_blockdev_flags_t` structure advertises properties such as read-only media, encryption, or erase-before-write requirements. Middleware can change apparent geometry size, but must verify upon creation that the underlying layer fits its requirements, and ensure that the underlying device will only be accessed correctly.

Operations
^^^^^^^^^^

The :cpp:type:`esp_blockdev_ops_t` structure defines callbacks for read, write, erase, sync, ioctl, and release. Before invoking a callback, callers must ensure that the given pointer is not ``NULL``; a ``NULL`` pointer indicates that the operation is not supported. Callers are responsible for validating alignment and bounds using the geometry data and for respecting flag-driven requirements such as issuing an erase before writing to NAND-like media.

Typical Flow
^^^^^^^^^^^^

1. Acquire a handle from a driver or middleware provider.
2. Inspect geometry and flags to determine required alignment, available capacity, and special handling.
3. Issue read, write, erase, and sync requests through the operation table that the provider exposes.
4. Forward the handle to higher components or release it once all operations complete.

Example
^^^^^^^

.. code-block:: c

   esp_blockdev_handle_t dev = my_component_get_blockdev();
   const esp_blockdev_geometry_t *geometry = dev->geometry;
   if (dev->ops->read && (sizeof(buffer) % geometry->read_size) == 0) {
       ESP_ERROR_CHECK(dev->ops->read(dev, buffer, sizeof(buffer), 0, sizeof(buffer)));
   }
   if (dev->ops->release) {
       ESP_ERROR_CHECK(dev->ops->release(dev));
   }

Contracts
---------

Flags (:cpp:type:`esp_blockdev_flags_t`)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* Flags are initialised once during device creation and must remain immutable for the lifetime of the handle.
* ``read_only`` requires write, erase, and mutating ioctl commands to fail with an error such as :c:macro:`ESP_ERR_INVALID_STATE`.
* ``encrypted`` signals that on-media data is encrypted; higher layers must not assume plaintext visibility or transparent mapping.
* ``erase_before_write`` tells callers that a successful write requires an erase of the target range beforehand. If multiple write operations are issued to the same range without an erase operation in-between, the behavior is undefined, but will likely result in data corruption.
* ``and_type_write`` signals NAND/NOR-style behavior: programming only clears bits (1→0) and effectively stores ``existing_bits & new_bits``. Bits that are already zero remain zero even if the write request supplies ones; erasing first is the only way to restore them.
* ``default_val_after_erase`` identifies whether erased regions read as ``0x00`` or ``0xFF`` so middleware can keep sentinel values consistent.

Geometry (:cpp:type:`esp_blockdev_geometry_t`)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* ``disk_size`` is the total accessible capacity in bytes; operations must reject any request whose end offset exceeds this value.
* ``read_size``, ``write_size``, and ``erase_size`` are mandatory alignment units, in bytes; both offsets and lengths must align to the corresponding size before the operation runs.
* Recommended sizes improve throughput when callers honour them but cannot replace the minimum alignment checks; implementations must accept any request that respects the required granularity.
* When a user sees read-write and read-only variants of the same underlying device, the geometry must be identical aside from the ``read_only`` flag. In particular, ``read_size``, ``write_size``, and ``erase_size`` should match between the two variants; ``recommended_*`` values are expected to match and should differ only when there is a clear benefit to doing so (and should be documented in such cases).

Operations (:cpp:type:`esp_blockdev_ops_t`)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``read(dev_handle, dst_buf, dst_buf_size, src_addr, data_read_len)``
  * Behaviour: Upon success copies exactly ``data_read_len`` bytes into ``dst_buf``.
  * Preconditions: ``dst_buf`` is valid, ``src_addr`` and ``data_read_len`` are aligned to ``read_size``, ``src_addr + data_read_len <= disk_size``, and ``data_read_len <= dst_buf_size``.
  * Postcondition: Returns :c:macro:`ESP_OK` when the copy succeeds or propagates a relevant ``ESP_ERR_*`` code on failure.

``write(dev_handle, src_buf, dst_addr, data_write_len)``
  * Preconditions: Device is not ``read_only``; ``src_buf`` spans at least ``data_write_len`` bytes; offset and length align to ``write_size`` and stay within ``disk_size``.
  * Behaviour: When ``erase_before_write`` is set, callers must issue an ``erase`` beforehand. When ``and_type_write`` is set, the hardware applies a bitwise AND with the existing contents, so the stored result becomes ``old_value & new_value``; bits cleared by earlier writes stay cleared unless the range is erased first.
  * Postcondition: Returns :c:macro:`ESP_OK` once the requested range has been accepted by the device (data may still reside in intermediate buffers until ``sync`` runs). Misaligned, out-of-range, or read-only attempts should surface :c:macro:`ESP_ERR_INVALID_ARG` or :c:macro:`ESP_ERR_INVALID_STATE`, and implementations must avoid leaving the range partially updated.
  * Note: On devices with ``and_type_write`` writes depend on the existing contents, so reading the same (or overlapping) range immediately after a write may require a preceding ``sync`` to ensure that cached data reflects the fully merged value.

``erase(dev_handle, start_addr, erase_len)``
  * Preconditions: Device permits erases; ``start_addr`` and ``erase_len`` align to ``erase_size``; the range remains inside ``disk_size``.
  * Postcondition: The range reads back as ``default_val_after_erase`` on success. Misaligned or out-of-range requests should return :c:macro:`ESP_ERR_INVALID_ARG`; hardware failures are expected to bubble up using driver-specific ``ESP_ERR_*`` codes.

``sync(dev_handle)``
  * Flushes pending writes. Devices that omit this callback operate with write-through semantics.
  * Postcondition: All previously reported writes reach stable storage before :c:macro:`ESP_OK` is returned (this includes all underlying devices). Timeouts or transport issues should surface as :c:macro:`ESP_ERR_TIMEOUT` or another relevant ``ESP_ERR_*``.

``ioctl(dev_handle, cmd, args)``
  * Command identifiers `0x00–0x7F` are reserved for ESP-IDF system use; `0x80–0xFF` are available for user-defined extensions.
  * Each command defines its own payload layout; because ``args`` is a ``void *``, wrappers can only validate or reinterpret the buffer for commands they understand, and must otherwise treat the payload as opaque.
  * Wrappers that cannot service a command should forward it unchanged to the next device in the stack when available; only the bottom device is expected to return :c:macro:`ESP_ERR_NOT_SUPPORTED` for unrecognised commands.
  * When the stack contains non-transparent address mapping, forwarding commands that embed raw addresses is inherently unsafe: intermediate layers cannot translate opaque payloads, so behaviour is undefined and will typically fail. Such commands should either be blocked explicitly or documented as unsupported in stacked configurations.

``release(dev_handle)``
  * Optional destructor that frees device resources. The function must be idempotent so that repeated calls either succeed or return a benign error like :c:macro:`ESP_ERR_INVALID_STATE`.

Error Handling
^^^^^^^^^^^^^^

Callbacks return :c:macro:`ESP_OK` on success and should propagate ``ESP_ERR_*`` codes unchanged to help callers diagnose failures. Middleware and applications are expected to propagate errors from underlying devices rather than masking them inside the stack. NULL function pointers should be treated as "operation not supported".

Validation
^^^^^^^^^^

Implementations should include tests that cover alignment checks, flag-driven behaviour (read-only, erase-before-write, NAND-style writes), and correct propagation of errors through stacked devices. Middleware that wraps lower handles must also verify that handle lifetime management remains consistent across the stack.

.. _blockdev-apis:

API Reference
-------------

.. include-build-file:: inc/esp_blockdev.inc
