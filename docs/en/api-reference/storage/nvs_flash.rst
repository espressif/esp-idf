Non-Volatile Storage Library
============================

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

Non-volatile storage (NVS) library is designed to store key-value pairs in flash. This section introduces some concepts used by NVS.

Initialization
^^^^^^^^^^^^^^

NVS uses partitions from the partition table with ``data`` type and ``nvs`` subtype. The library can be initialized using:

- :cpp:func:`nvs_flash_init` initializes the default NVS partition with label ``nvs``.
- :cpp:func:`nvs_flash_init_partition` initializes a specific NVS partition by its label.
- :cpp:func:`nvs_flash_init_partition_ptr` initializes NVS from an ``esp_partition_t`` pointer.

Once initialized, applications access NVS namespaces using :cpp:func:`nvs_open` (for the default partition) or :cpp:func:`nvs_open_from_partition` (for a specific partition by label).

.. note::

    NVS can also operate through the Block Device Layer (BDL) when :ref:`CONFIG_NVS_BDL_STACK` is enabled, allowing use of alternative storage backends beyond standard flash partitions. In BDL mode, :cpp:func:`nvs_flash_init_partition_ptr` is not available, but :cpp:func:`nvs_flash_init_partition_bdl` becomes available for custom block device initialization. See :ref:`nvs_internals` > :ref:`nvs_underlying_storage` for details.

.. note::

    If an NVS partition is truncated (for example, when the partition table layout is changed), its contents should be erased. ESP-IDF build system provides a ``idf.py erase-flash`` target to erase all contents of the flash chip.

Keys and Values
^^^^^^^^^^^^^^^

NVS operates on key-value pairs. Keys are ASCII strings; the maximum key length is currently 15 characters. Values can have one of the following types:

-  integer types: ``uint8_t``, ``int8_t``, ``uint16_t``, ``int16_t``, ``uint32_t``, ``int32_t``, ``uint64_t``, ``int64_t``
-  zero-terminated string
-  variable length binary data (blob)

.. note::

    NVS works best for storing many small values, rather than a few large values of the type ``string`` and ``blob``. If you need to store large blobs or strings, consider using the facilities provided by the FAT filesystem on top of the wear levelling library.

.. note::

    String values are currently limited to 4000 bytes. This includes the null terminator. Blob values are limited to 508,000 bytes or 97.6% of the partition size - 4000 bytes, whichever is lower.

.. note::

    Before setting new or updating existing key-value pair, free entries in nvs pages have to be available. For integer types, at least one free entry has to be available. For the string value, at least one page capable of keeping the whole string in a contiguous row of free entries has to be available. For the blob value, the size of new data has to be available in free entries.

Additional data types, such as ``float`` and ``double`` might be added later.

Keys are required to be unique. Assigning a new value to an existing key replaces the old value and data type with the value and data type specified by a write operation.

A data type check is performed when reading a value. An error is returned if the data type expected by read operation does not match the data type of entry found for the key provided.


Namespaces
^^^^^^^^^^

To mitigate potential conflicts in key names between different components, NVS assigns each key-value pair to one of the namespaces. Namespace names follow the same rules as key names, i.e., the maximum length is 15 characters. Furthermore, there can be no more than 254 different namespaces in one NVS partition. Namespace name is specified in the :cpp:func:`nvs_open` or :cpp:type:`nvs_open_from_partition` call. This call returns an opaque handle, which is used in subsequent calls to the ``nvs_get_*``, ``nvs_set_*``, and :cpp:func:`nvs_commit` functions. This way, a handle is associated with a namespace and partition, and key names will not collide with same names in other namespaces.

The open mode parameter controls the access level and security behavior:

- ``NVS_READONLY``: Read-only access. All write operations will be rejected.
- ``NVS_READWRITE``: Standard read-write access. Erased data is marked as deleted but remains in flash.
- ``NVS_READWRITE_PURGE``: Secure read-write access. Erased data is physically purged from flash memory.

.. note::

    Namespaces with the same name in different NVS partitions are considered as separate namespaces.

NVS Iterators
^^^^^^^^^^^^^

Iterators allow to list key-value pairs stored in NVS, based on specified partition name, namespace, and data type.

There are the following functions available:

- :cpp:func:`nvs_entry_find` creates an opaque handle, which is used in subsequent calls to the :cpp:func:`nvs_entry_next` and :cpp:func:`nvs_entry_info` functions.
- :cpp:func:`nvs_entry_next` advances an iterator to the next key-value pair.
- :cpp:func:`nvs_entry_info` returns information about each key-value pair

In general, all iterators obtained via :cpp:func:`nvs_entry_find` have to be released using :cpp:func:`nvs_release_iterator`, which also tolerates ``NULL`` iterators.

:cpp:func:`nvs_entry_find` and :cpp:func:`nvs_entry_next` set the given iterator to ``NULL`` or a valid iterator in all cases except a parameter error occurred (i.e., return ``ESP_ERR_NVS_NOT_FOUND``). In case of a parameter error, the given iterator will not be modified. Hence, it is best practice to initialize the iterator to ``NULL`` before calling :cpp:func:`nvs_entry_find` to avoid complicated error checking before releasing the iterator.


Security, Tampering, and Robustness
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: not SOC_HMAC_SUPPORTED

    NVS is not directly compatible with the {IDF_TARGET_NAME} flash encryption system. However, data can still be stored in encrypted form if NVS encryption is used together with {IDF_TARGET_NAME} flash encryption. Please refer to :doc:`nvs_encryption` for more details.

.. only:: SOC_HMAC_SUPPORTED

    NVS is not directly compatible with the {IDF_TARGET_NAME} flash encryption system. However, data can still be stored in encrypted form if NVS encryption is used together with {IDF_TARGET_NAME} flash encryption or with the help of the HMAC peripheral. Please refer to :doc:`nvs_encryption` for more details.

If NVS encryption is not used, it is possible for anyone with physical access to the flash chip to read, alter, erase, or add key-value pairs. With NVS encryption enabled, it is not possible to read, alter or add a key-value pair and get recognized as a valid pair without knowing corresponding NVS encryption keys. However, there is no tamper-resistance against the erase operation.

The library does try to recover from conditions when flash memory is in an inconsistent state. In particular, one should be able to power off the device at any point and time and then power it back on. This should not result in loss of data, except for the new key-value pair if it was being written at the moment of powering off. The library should also be able to initialize properly with any random data present in flash memory.

.. _data_purging_security:

Data Purging and Security
^^^^^^^^^^^^^^^^^^^^^^^^^

By default, when NVS updates or erases key-value pairs, the data in flash is only marked as erased in metadata section. The values still remain in flash memory. This approach optimizes write performance.

For applications requiring enhanced security where sensitive data must be physically removed (purged in terms of setting all bits to zeroes) from flash memory after calling erase or update functions, NVS provides two mechanisms:

**One Time Purge**

    The :cpp:func:`nvs_purge_all` function purges all existing items marked as erased within a namespace. It is useful in scenarios where the continuous purging mode was not used yet and the application needs to purge existing erased flash content. This function may be used with handles opened either using ``NVS_READWRITE`` or ``NVS_READWRITE_PURGE`` mode.

**Continuous Purging Mode**

    Namespace handle opened with ``NVS_READWRITE_PURGE`` mode automatically purges the flash space occupied by the erased or overwritten values on top of just marking them as erased.

.. note::

    Opening NVS namespace in ``NVS_READWRITE_PURGE`` mode does not purge data marked as erased already present in the flash. Perform One Time Purge before using Continuous Purging Mode if the namespace already contains some updated or erased data.

.. note::

    Purging operations represent additional flash write cycles compared to standard marking as erased mode. Applications should balance security requirements with flash write performance when deciding whether to use data purging features.


Special Use Cases
-----------------

Large Amount of Data in NVS
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Although not recommended, NVS can store tens of thousands of keys and NVS partition can reach up to megabytes in size.

.. note::

    NVS component leaves RAM footprint on the heap. The footprint depends on the size of the NVS partition on flash and the number of keys in use. For RAM usage estimation, please use the following approximate figures: each 1 MB of NVS flash partition consumes 22 KB of RAM and each 1000 keys consumes 5.5 KB of RAM.

.. note::

    Duration of NVS initialization using :cpp:func:`nvs_flash_init` is proportional to the number of existing keys. Initialization of NVS requires typically 0.5 seconds per 1000 keys.

.. note::

    The duration of initialization gradually grows with the population of NVS partition and number of value updates. To avoid unexpected watchdog timeout during initialization when your application is already in customer use, test your NVS initialization in advance on a NVS partition containing all keys including expected history of updates.

.. only:: SOC_SPIRAM_SUPPORTED

    By default, internal NVS allocates a heap in internal RAM. With a large NVS partition or big number of keys, the application can exhaust the internal RAM heap just on NVS overhead.

    Applications using modules with SPI-connected PSRAM can overcome this limitation by enabling the Kconfig option :ref:`CONFIG_NVS_ALLOCATE_CACHE_IN_SPIRAM` which redirects RAM allocation to the SPI-connected PSRAM.

    This option is available in the nvs_flash component of the menuconfig menu when SPIRAM is enabled and :ref:`CONFIG_SPIRAM_USE` is set to ``CONFIG_SPIRAM_USE_CAPS_ALLOC``.

    .. note::

        Using SPI-connected PSRAM slows down NVS API for integer operations by an approximate factor of 2.5.

Unstable Power Conditions
^^^^^^^^^^^^^^^^^^^^^^^^^

When NVS is used in systems powered by weak or unstable energy sources (such as solar or battery), flash erase operations may occasionally fail to complete without being detected by the application. This can create a mismatch between the actual flash contents and the expected layout of reserved pages. In rare cases, especially during unexpected power loss, this may exhaust the available NVS pages and cause partition initialization to fail with the error ``ESP_ERR_NVS_NO_FREE_PAGES``.

To address this issue, the Kconfig option :ref:`CONFIG_NVS_FLASH_VERIFY_ERASE` enables verification of flash erase operations by reading back the affected page. If the page is not fully erased to ``0xFF`` after a ``flash_erase`` operation, the erase is retried until the page is correctly cleared. The total number of erase attempts, including the initial attempt, is controlled by the Kconfig option :ref:`CONFIG_NVS_FLASH_ERASE_ATTEMPTS`.

.. note::

    When NVS is initialized on the writeable partition, the library will attempt to perform recovery operations if the partition is found to be in an inconsistent state. This may involve erasing and rewriting some pages and in continuously unstable power environment subsequently lead to the unintended loss of factory default data. For this reason, it is recommended to keep vital factory default data in separate, read-only partition where the recovery is not performed.


.. _nvs_bootloader:

Use of NVS in Bootloader Code
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The standard NVS API described in this guide is available to the running application. It is also possible to read data from NVS in the custom bootloader code. More information can be found in the :doc:`nvs_bootloader` guide.

.. _nvs_encryption:

NVS Encryption
--------------

Please refer to the :doc:`nvs_encryption` guide for more details.

NVS Partition Generator Utility
-------------------------------

This utility helps generate NVS partition binary files which can be flashed separately on a dedicated partition via a flashing utility. Key-value pairs to be flashed onto the partition can be provided via a CSV file. For more details, please refer to :doc:`nvs_partition_gen`.

Instead of calling the ``nvs_partition_gen.py`` tool manually, the creation of the partition binary files can also be done directly from CMake using the function ``nvs_create_partition_image``::

    nvs_create_partition_image(<partition> <csv> [FLASH_IN_PROJECT] [DEPENDS  dep dep dep ...])

**Positional Arguments**:

.. list-table::
    :header-rows: 1

    * - Parameter
      - Description
    * - ``partition``
      - Name of the NVS partition
    * - ``csv``
      - Path to CSV file to parse


**Optional Arguments**:

.. list-table::
   :header-rows: 1

   * - Parameter
     - Description
   * - ``FLASH_IN_PROJECT``
     - Name of the NVS partition
   * - ``DEPENDS``
     - Specify files on which the command depends


If ``FLASH_IN_PROJECT`` is not specified, the image will still be generated, but you will have to flash it manually using ``idf.py <partition>-flash`` (e.g., if your partition name is ``nvs``, then use ``idf.py nvs-flash``).

``nvs_create_partition_image`` must be called from one of the component ``CMakeLists.txt`` files. Currently, only non-encrypted partitions are supported.

Application Example
-------------------

You can find code examples in the :example:`storage/nvs` directory of ESP-IDF examples:

:example:`storage/nvs/nvs_rw_value`

  Demonstrates how to read a single integer value from, and write it to NVS.

  The value checked in this example holds the number of the {IDF_TARGET_NAME} module restarts. The value's function as a counter is only possible due to its storing in NVS.

  The example also shows how to check if a read/write operation was successful, or if a certain value has not been initialized in NVS. The diagnostic procedure is provided in plain text to help you track the program flow and capture any issues on the way.

:example:`storage/nvs/nvs_rw_blob`

  Demonstrates how to read a single integer value and a blob (binary large object), and write them to NVS to preserve this value between {IDF_TARGET_NAME} module restarts.

    * value - tracks the number of the {IDF_TARGET_NAME} module soft and hard restarts.
    * blob - contains a table with module run times. The table is read from NVS to dynamically allocated RAM. A new run time is added to the table on each manually triggered soft restart, and then the added run time is written to NVS. Triggering is done by pulling down GPIO0.

  The example also shows how to implement the diagnostic procedure to check if the read/write operation was successful.

:example:`storage/nvs/nvs_rw_value_cxx`

  This example does exactly the same as :example:`storage/nvs/nvs_rw_value`, except that it uses the C++ NVS handle class.

:example:`storage/nvs/nvs_statistics`

  This example demonstrates how to obtain and interpret NVS usage statistics: free/used/available/total number of entries and number of namespaces in given NVS partition.

  Default NVS partition is erased for a clean run of this example. Then mock data string values are written.

  Usage statistics are obtained prior to and post writing, with the differences being compared to expected values of newly used entries.

:example:`storage/nvs/nvs_iteration`

  This example demonstrates how to iterate over entries of specific (or any) NVS data type and how to obtain info about such entries.

  Default NVS partition is erased for a clean run of this example. Then mock data consisting of different NVS integer data types are written.

  After that, the example iterates over each individual data type as well as the generic ``NVS_TYPE_ANY`` type, and logs the information obtained from each iteration.

.. _nvs_internals:

Internals
---------

Log of Key-Value Pairs
^^^^^^^^^^^^^^^^^^^^^^

NVS stores key-value pairs sequentially, with new key-value pairs being added at the end. When a value of any given key has to be updated, a new key-value pair is added at the end of the log and the old key-value pair is marked as erased.

.. note::

    NVS component includes flash wear levelling by design. Set operations are appending new data to the free space after existing entries. Invalidation of old values doesn't require immediate flash erase operations. The organization of NVS space to pages and entries effectively reduces the frequency of flash erase to flash write operations for data types fitting one entry by a factor of 126.

Pages and Entries
^^^^^^^^^^^^^^^^^

NVS library uses two main entities in its operation: pages and entries. Page is a logical structure which stores a portion of the overall log. Logical page corresponds to one physical sector of flash memory. Pages which are in use have a *sequence number* associated with them. Sequence numbers impose an ordering on pages. Higher sequence numbers correspond to pages which were created later. Each page can be in one of the following states:

Empty/uninitialized
    Flash storage for the page is empty (all bytes are ``0xff``). Page is not used to store any data at this point and does not have a sequence number.

Active
    Flash storage is initialized, page header has been written to flash, page has a valid sequence number. Page has some empty entries and data can be written there. No more than one page can be in this state at any given moment.

Full
    Flash storage is in a consistent state and is filled with key-value pairs.
    Writing new key-value pairs into this page is not possible. It is still possible to mark some key-value pairs as erased.

Erasing
    Non-erased key-value pairs are being moved into another page so that the current page can be erased. This is a transient state, i.e., page should never stay in this state at the time when any API call returns. In case of a sudden power off, the move-and-erase process will be completed upon the next power-on.

Corrupted
    Page header contains invalid data, and further parsing of page data was cancelled. Any items previously written into this page will not be accessible. The corresponding flash sector will not be erased immediately and will be kept along with sectors in **uninitialized** state for later use. This may be useful for debugging.

Mapping from flash sectors to logical pages does not have any particular order. The library will inspect sequence numbers of pages found in each flash sector and organize pages in a list based on these numbers.

::

    +--------+     +--------+     +--------+     +--------+
    | Page 1 |     | Page 2 |     | Page 3 |     | Page 4 |
    | Full   +---> | Full   +---> | Active |     | Empty  |   <- states
    | #11    |     | #12    |     | #14    |     |        |   <- sequence numbers
    +---+----+     +----+---+     +----+---+     +---+----+
        |               |              |             |
        |               |              |             |
        |               |              |             |
    +---v------+  +-----v----+  +------v---+  +------v---+
    | Sector 3 |  | Sector 0 |  | Sector 2 |  | Sector 1 |    <- physical sectors
    +----------+  +----------+  +----------+  +----------+

Structure of a Page
^^^^^^^^^^^^^^^^^^^

For now, we assume that flash sector size is 4096 bytes and that {IDF_TARGET_NAME} flash encryption hardware operates on 32-byte blocks. It is possible to introduce some settings configurable at compile-time (e.g., via menuconfig) to accommodate flash chips with different sector sizes (although it is not clear if other components in the system, e.g., SPI flash driver and SPI flash cache can support these other sizes).

Page consists of three parts: header, entry state bitmap, and entries themselves. To be compatible with {IDF_TARGET_NAME} flash encryption, the entry size is 32 bytes. For integer types, an entry holds one key-value pair. For strings and blobs, an entry holds part of key-value pair (more on that in the entry structure description).

The following diagram illustrates the page structure. Numbers in parentheses indicate the size of each part in bytes.

::

    +-----------+--------------+-------------+-------------------------+
    | State (4) | Seq. no. (4) | version (1) | Unused (19) | CRC32 (4) |   Header (32)
    +-----------+--------------+-------------+-------------------------+
    |                Entry state bitmap (32)                           |
    +------------------------------------------------------------------+
    |                       Entry 0 (32)                               |
    +------------------------------------------------------------------+
    |                       Entry 1 (32)                               |
    +------------------------------------------------------------------+
    /                                                                  /
    /                                                                  /
    +------------------------------------------------------------------+
    |                       Entry 125 (32)                             |
    +------------------------------------------------------------------+

Page header and entry state bitmap are always written to flash unencrypted. Entries are encrypted if flash encryption feature of {IDF_TARGET_NAME} is used.

Page state values are defined in such a way that changing state is possible by writing 0 into some of the bits. Therefore it is not necessary to erase the page to change its state unless that is a change to the *erased* state.

The version field in the header reflects the NVS format version used. For backward compatibility reasons, it is decremented for every version upgrade starting at 0xff (i.e., 0xff for version-1, 0xfe for version-2 and so on).

CRC32 value in the header is calculated over the part which does not include a state value (bytes 4 to 28). The unused part is currently filled with ``0xff`` bytes.

The following sections describe the structure of entry state bitmap and entry itself.

Entry and Entry State Bitmap
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Each entry can be in one of the following three states represented with two bits in the entry state bitmap. The final four bits in the bitmap (256 - 2 * 126) are not used.

Empty (2'b11)
    Nothing is written into the specific entry yet. It is in an uninitialized state (all bytes are ``0xff``).

Written (2'b10)
    A key-value pair (or part of key-value pair which spans multiple entries) has been written into the entry.

Erased (2'b00)
    A key-value pair in this entry has been discarded. Contents of this entry will not be parsed anymore.


.. _structure_of_entry:

Structure of Entry
^^^^^^^^^^^^^^^^^^

For values of primitive types (currently integers from 1 to 8 bytes long), entry holds one key-value pair. For string and blob types, entry holds part of the whole key-value pair. For strings, in case when a key-value pair spans multiple entries, all entries are stored in the same page. Blobs are allowed to span over multiple pages by dividing them into smaller chunks. For tracking these chunks, an additional fixed length metadata entry is stored called "blob index". Earlier formats of blobs are still supported (can be read and modified). However, once the blobs are modified, they are stored using the new format.

::

    +--------+----------+----------+----------------+-----------+---------------+----------+
    | NS (1) | Type (1) | Span (1) | ChunkIndex (1) | CRC32 (4) |    Key (16)   | Data (8) |
    +--------+----------+----------+----------------+-----------+---------------+----------+

                                             Primitive  +--------------------------------+
                                            +-------->  |     Data (8)                   |
                                            | Types     +--------------------------------+
                       +-> Fixed length --
                       |                    |           +---------+--------------+---------------+-------+
                       |                    +-------->  | Size(4) | ChunkCount(1)| ChunkStart(1) | Rsv(2)|
        Data format ---+                    Blob Index  +---------+--------------+---------------+-------+
                       |
                       |                             +----------+---------+-----------+
                       +->   Variable length   -->   | Size (2) | Rsv (2) | CRC32 (4) |
                            (Strings, Blob Data)     +----------+---------+-----------+


Individual fields in entry structure have the following meanings:

NS
    Namespace index for this entry. For more information on this value, see the section on namespaces implementation.

Type
    One byte indicating the value data type. See the :cpp:type:`ItemType` enumeration in :component_file:`nvs_flash/include/nvs_handle.hpp` for possible values.

Span
    Number of entries used by this key-value pair. For integer types, this is equal to 1. For strings and blobs, this depends on value length.

ChunkIndex
    Used to store the index of a blob-data chunk for blob types. For other types, this should be ``0xff``.

CRC32
    Checksum calculated over all the bytes in this entry, except for the CRC32 field itself.

Key
    Zero-terminated ASCII string containing a key name. Maximum string length is 15 bytes, excluding a zero terminator.

Data
    For integer types, this field contains the value itself. If the value itself is shorter than 8 bytes, it is padded to the right, with unused bytes filled with ``0xff``.

    For "blob index" entry, these 8 bytes hold the following information about data-chunks:

    - Size
        (Only for blob index.) Size, in bytes, of complete blob data.

    - ChunkCount
        (Only for blob index.) Total number of blob-data chunks into which the blob was divided during storage.

    - ChunkStart
        (Only for blob index.) ChunkIndex of the first blob-data chunk of this blob. Subsequent chunks have chunkIndex incrementally allocated (step of 1).

    For string and blob data chunks, these 8 bytes hold additional data about the value, which are described below:

    - Size
        (Only for strings and blobs.) Size, in bytes, of actual data. For strings, this includes zero terminators.

    - CRC32
        (Only for strings and blobs.) Checksum calculated over all bytes of data.

Variable length values (strings and blobs) are written into subsequent entries, 32 bytes per entry. The ``Span`` field of the first entry indicates how many entries are used.


Namespaces
^^^^^^^^^^

As mentioned above, each key-value pair belongs to one of the namespaces. Namespace identifiers (strings) are stored as keys of key-value pairs in namespace with index 0. Values corresponding to these keys are indexes of these namespaces.

::

    +-------------------------------------------+
    | NS=0 Type=uint8_t Key="wifi" Value=1      |   Entry describing namespace "wifi"
    +-------------------------------------------+
    | NS=1 Type=uint32_t Key="channel" Value=6  |   Key "channel" in namespace "wifi"
    +-------------------------------------------+
    | NS=0 Type=uint8_t Key="pwm" Value=2       |   Entry describing namespace "pwm"
    +-------------------------------------------+
    | NS=2 Type=uint16_t Key="channel" Value=20 |   Key "channel" in namespace "pwm"
    +-------------------------------------------+


Item Hash List
^^^^^^^^^^^^^^

To reduce the number of reads from flash memory, each member of the Page class maintains a list of pairs: item index; item hash. This list makes searches much quicker. Instead of iterating over all entries, reading them from flash one at a time, `Page::findItem` first performs a search for the item hash in the hash list. This gives the item index within the page if such an item exists. Due to a hash collision, it is possible that a different item is found. This is handled by falling back to iteration over items in flash.

Each node in the hash list contains a 24-bit hash and 8-bit item index. Hash is calculated based on item namespace, key name, and ChunkIndex. CRC32 is used for calculation; the result is truncated to 24 bits. To reduce the overhead for storing 32-bit entries in a linked list, the list is implemented as a double-linked list of arrays. Each array holds 29 entries, for the total size of 128 bytes, together with linked list pointers and a 32-bit count field. The minimum amount of extra RAM usage per page is therefore 128 bytes; maximum is 640 bytes.

.. _read-only-nvs:

Read-only NVS
^^^^^^^^^^^^^^

There are two levels of read-only NVS support:

- At the partition table level, a partition can be flagged as ``readonly`` in the partition table CSV file.
- At the application level, NVS partitions can be opened in read-only mode using the :cpp:func:`nvs_open_from_partition` function with the ``NVS_READONLY`` flag.

The default minimal size for NVS to function properly is 12 KiB (``0x3000``), meaning there have to be at least 3 pages with at least one of them being in Empty state. However if the NVS partition is flagged as ``readonly`` in the partition table CSV and is being opened in read-only mode, the partition can be as small as 4 KiB (``0x1000``).

.. note::

    At the moment read-only flag is not reflected if NVS is using Block Device Layer as a storage.

.. _nvs_underlying_storage:

Underlying Storage
^^^^^^^^^^^^^^^^^^

At build time the mode NVS will use for accessing its underlying storage can be configured. Two options are available in menuconfig option :ref:`CONFIG_NVS_BDL_STACK`.

**ESP Partition API (default)**: NVS accesses storage using the :ref:`esp_partition <flash-partition-apis>`. This is the default mode of operation, where NVS uses SPI flash partitions defined in the partition table. In this mode:

- Initialization functions (:cpp:func:`nvs_flash_init`, :cpp:func:`nvs_flash_init_partition`) look up the partition and use :ref:`esp_partition <flash-partition-apis>` APIs to access it.
- The application can provide custom ``esp_partition_t`` pointer and call :cpp:func:`nvs_flash_init_partition_ptr`. This enables applications to overcome limitations of partition table based partitions, such as using partitions not defined in the partition table.
- This mode provides the best performance, as there is no additional abstraction layer between NVS and the underlying storage.

**Block Device Layer (BDL)**: NVS accesses storage using esp_blockdev. This option enables NVS to operate on a block device that implements the esp_blockdev interface. In this mode:

- Initialization functions (:cpp:func:`nvs_flash_init`, :cpp:func:`nvs_flash_init_partition`) transparently create block devices using :ref:`esp_partition <flash-partition-apis>`, manage their lifecycles and internally use the esp_blockdev. For the application, this mode is similar to the default mode of operation.
- The application can provide custom block device handle and call :cpp:func:`nvs_flash_init_partition_bdl` to register it for NVS. The application is responsible for managing the block device handle lifecycle.
- As the BDL abstracts the underlying storage, there is an overhead compared to using :ref:`esp_partition <flash-partition-apis>` APIs directly. Choose this mode when esp_partition backed storage is not sufficient for the needs of the application.

.. note::

    If custom Block Device is used in NVS, it has to satisfy the following requirements:

    - ``read_size`` and ``write_size`` must be 1 byte (NVS requires byte-level granular access).
    - ``erase_size`` must be a divisor of 4096 (NVS page size is fixed at 4096 bytes).
    - ``disk_size`` must be a multiple of 4096 bytes.
    - ``default_val_after_erase`` flag must be set to 1 (erased memory reads as ``0xFF``).
    - Operations (``read``, ``write``, ``erase``) must be implemented.



API Reference
-------------

.. include-build-file:: inc/nvs_flash.inc

.. include-build-file:: inc/nvs.inc
