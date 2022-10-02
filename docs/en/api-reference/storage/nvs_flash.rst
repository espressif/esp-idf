Non-volatile Storage Library
============================

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

Non-volatile storage (NVS) library is designed to store key-value pairs in flash. This section introduces some concepts used by NVS.

Underlying Storage
^^^^^^^^^^^^^^^^^^

Currently, NVS uses a portion of main flash memory through the :ref:`esp_partition <flash-partition-apis>` API. The library uses all the partitions with ``data`` type and ``nvs`` subtype.  The application can choose to use the partition with the label ``nvs`` through the :cpp:func:`nvs_open` API function or any other partition by specifying its name using the :cpp:func:`nvs_open_from_partition` API function.

Future versions of this library may have other storage backends to keep data in another flash chip (SPI or I2C), RTC, FRAM, etc.

.. note:: if an NVS partition is truncated (for example, when the partition table layout is changed), its contents should be erased. ESP-IDF build system provides a ``idf.py erase-flash`` target to erase all contents of the flash chip.

.. note:: NVS works best for storing many small values, rather than a few large values of the type 'string' and 'blob'. If you need to store large blobs or strings, consider using the facilities provided by the FAT filesystem on top of the wear levelling library.


Keys and Values
^^^^^^^^^^^^^^^

NVS operates on key-value pairs. Keys are ASCII strings; the maximum key length is currently 15 characters. Values can have one of the following types:

-  integer types: ``uint8_t``, ``int8_t``, ``uint16_t``, ``int16_t``, ``uint32_t``, ``int32_t``, ``uint64_t``, ``int64_t``
-  zero-terminated string
-  variable length binary data (blob)

.. note::

    String values are currently limited to 4000 bytes. This includes the null terminator. Blob values are limited to 508,000 bytes or 97.6% of the partition size - 4000 bytes, whichever is lower.

Additional types, such as ``float`` and ``double`` might be added later.

Keys are required to be unique. Assigning a new value to an existing key works as follows:

-  If the new value is of the same type as the old one, value is updated.
-  If the new value has a different data type, an error is returned.

Data type check is also performed when reading a value. An error is returned if the data type of the read operation does not match the data type of the value.


Namespaces
^^^^^^^^^^

To mitigate potential conflicts in key names between different components, NVS assigns each key-value pair to one of namespaces. Namespace names follow the same rules as key names, i.e., the maximum length is 15 characters. Furthermore, there can be no more than 254 different namespaces in one NVS partition. Namespace name is specified in the :cpp:func:`nvs_open` or :cpp:type:`nvs_open_from_partition` call. This call returns an opaque handle, which is used in subsequent calls to the ``nvs_get_*``, ``nvs_set_*``, and :cpp:func:`nvs_commit` functions. This way, a handle is associated with a namespace, and key names will not collide with same names in other namespaces. Please note that the namespaces with the same name in different NVS partitions are considered as separate namespaces.

NVS Iterators
^^^^^^^^^^^^^

Iterators allow to list key-value pairs stored in NVS, based on specified partition name, namespace, and data type.

There are the following functions available:

- :cpp:func:`nvs_entry_find` creates an opaque handle, which is used in subsequent calls to the :cpp:func:`nvs_entry_next` and :cpp:func:`nvs_entry_info` functions.
- :cpp:func:`nvs_entry_next` advances an iterator to the next key-value pair.
- :cpp:func:`nvs_entry_info` returns information about each key-value pair

In general, all iterators obtained via :cpp:func:`nvs_entry_find` have to be released using :cpp:func:`nvs_release_iterator`, which also tolerates ``NULL`` iterators.
:cpp:func:`nvs_entry_find` and :cpp:func:`nvs_entry_next` will set the given iterator to ``NULL`` or a valid iterator in all cases except a parameter error occured (i.e., return ``ESP_ERR_NVS_NOT_FOUND``). In case of a parameter error, the given iterator will not be modified. Hence, it is best practice to initialize the iterator to ``NULL`` before calling :cpp:func:`nvs_entry_find` to avoid complicated error checking before releasing the iterator.


Security, Tampering, and Robustness
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

NVS is not directly compatible with the {IDF_TARGET_NAME} flash encryption system. However, data can still be stored in encrypted form if NVS encryption is used together with {IDF_TARGET_NAME} flash encryption. Please refer to :ref:`nvs_encryption` for more details.

If NVS encryption is not used, it is possible for anyone with physical access to the flash chip to alter, erase, or add key-value pairs. With NVS encryption enabled, it is not possible to alter or add a key-value pair and get recognized as a valid pair without knowing corresponding NVS encryption keys. However, there is no tamper-resistance against the erase operation.

The library does try to recover from conditions when flash memory is in an inconsistent state. In particular, one should be able to power off the device at any point and time and then power it back on. This should not result in loss of data, except for the new key-value pair if it was being written at the moment of powering off. The library should also be able to initialize properly with any random data present in flash memory.


.. _nvs_encryption:

NVS Encryption
--------------

Data stored in NVS partitions can be encrypted using AES-XTS in the manner similar to the one mentioned in disk encryption standard IEEE P1619. For the purpose of encryption, each entry is treated as one `sector` and relative address of the entry (w.r.t. partition-start) is fed to the encryption algorithm as `sector-number`. The NVS Encryption can be enabled by enabling :ref:`CONFIG_NVS_ENCRYPTION`. The keys required for NVS encryption are stored in yet another partition, which is protected using :doc:`Flash Encryption <../../security/flash-encryption>`. Therefore, enabling :doc:`Flash Encryption <../../security/flash-encryption>` is a prerequisite for NVS encryption.

The NVS Encryption is enabled by default when :doc:`Flash Encryption <../../security/flash-encryption>` is enabled. This is done because Wi-Fi driver stores credentials (like SSID and passphrase) in the default NVS partition. It is important to encrypt them as default choice if platform level encryption is already enabled.

For using NVS encryption, the partition table must contain the :ref:`nvs_key_partition`. Two partition tables containing the :ref:`nvs_key_partition` are provided for NVS encryption under the partition table option (``menuconfig`` > ``Partition Table``). They can be selected with the project configuration menu (``idf.py menuconfig``). Please refer to the example :example:`security/flash_encryption` for how to configure and use NVS encryption feature.

.. _nvs_key_partition:

NVS Key Partition
^^^^^^^^^^^^^^^^^

An application requiring NVS encryption support needs to be compiled with a key-partition of the type `data` and subtype `key`. This partition should be marked as `encrypted` and its size should be the minimum partition size (4KB). Refer to :doc:`Partition Tables <../../api-guides/partition-tables>` for more details. Two additional partition tables which contain the :ref:`nvs_key_partition` are provided under the partition table option (``menuconfig`` > ``Partition Table``). They can be directly used for :ref:`nvs_encryption`. The structure of these partitions is depicted below.

.. highlight:: none

::

    +-----------+--------------+-------------+----+
    |              XTS encryption key (32)        |
    +---------------------------------------------+
    |              XTS tweak key (32)             |
    +---------------------------------------------+
    |                  CRC32 (4)                  |
    +---------------------------------------------+

The XTS encryption keys in the :ref:`nvs_key_partition` can be generated in one of the following two ways.

1. Generate the keys on the ESP chip:

    When NVS encryption is enabled the :cpp:func:`nvs_flash_init` API function can be used to initialize the encrypted default NVS partition. The API function internally generates the XTS encryption keys on the ESP chip. The API function finds the first :ref:`nvs_key_partition`. Then the API function automatically generates and stores the NVS keys in that partition by making use of the :cpp:func:`nvs_flash_generate_keys` API function provided by :component_file:`nvs_flash/include/nvs_flash.h`. New keys are generated and stored only when the respective key partition is empty. The same key partition can then be used to read the security configurations for initializing a custom encrypted NVS partition with help of :cpp:func:`nvs_flash_secure_init_partition`.

    The API functions :cpp:func:`nvs_flash_secure_init` and :cpp:func:`nvs_flash_secure_init_partition` do not generate the keys internally. When these API functions are used for initializing encrypted NVS partitions, the keys can be generated after startup using the :cpp:func:`nvs_flash_generate_keys` API function provided by ``nvs_flash.h``. The API function will then write those keys onto the key-partition in encrypted form.

    .. note:: Please note that `nvs_keys` partition must be completely erased before starting the application in this approach. Otherwise the application may generate :c:macro:`ESP_ERR_NVS_CORRUPT_KEY_PART` error code assuming that `nvs_keys` partition was not empty and contains malformatted data. You can use the following command for this:
        ::

            parttool.py --port PORT --partition-table-file=PARTITION_TABLE_FILE --partition-table-offset PARTITION_TABLE_OFFSET erase_partition --partition-type=data --partition-subtype=nvs_keys

2. Use pre-generated key partition:

    This option will be required by the user when keys in the :ref:`nvs_key_partition` are not generated by the application. The :ref:`nvs_key_partition` containing the XTS encryption keys can be generated with the help of :doc:`NVS Partition Generator Utility</api-reference/storage/nvs_partition_gen>`. Then the user can store the pre generated key partition on the flash with help of the following two commands:

    i) Build and flash the partition table
    ::

        idf.py partition-table partition-table-flash

    ii) Store the keys in the :ref:`nvs_key_partition` (on the flash) with the help of :component_file:`parttool.py<partition_table/parttool.py>` (see Partition Tool section in :doc:`partition-tables </api-guides/partition-tables>` for more details)
    ::

        parttool.py --port PORT --partition-table-offset PARTITION_TABLE_OFFSET write_partition --partition-name="name of nvs_key partition" --input NVS_KEY_PARTITION_FILE

    .. note:: If the device is encrypted in flash encryption development mode and you want to renew the NVS key partition, you need to tell :component_file:`parttool.py <partition_table/parttool.py>` to encrypt the NVS key partition and you also need to give it a pointer to the unencrypted partition table in your build directory (build/partition_table) since the partition table on the device is encrypted, too. You can use the following command:
        ::

            parttool.py --esptool-write-args encrypt --port PORT --partition-table-file=PARTITION_TABLE_FILE --partition-table-offset PARTITION_TABLE_OFFSET write_partition --partition-name="name of nvs_key partition" --input NVS_KEY_PARTITION_FILE

Since the key partition is marked as `encrypted` and :doc:`Flash Encryption <../../security/flash-encryption>` is enabled, the bootloader will encrypt this partition using flash encryption key on the first boot.

It is possible for an application to use different keys for different NVS partitions and thereby have multiple key-partitions. However, it is a responsibility of the application to provide correct key-partition/keys for the purpose of encryption/decryption.

Encrypted Read/Write
^^^^^^^^^^^^^^^^^^^^

The same NVS API functions ``nvs_get_*`` or ``nvs_set_*`` can be used for reading of, and writing to an encrypted nvs partition as well.

**Encrypt the default NVS partition:**
To enable encryption for the default NVS partition no additional steps are necessary. When :ref:`CONFIG_NVS_ENCRYPTION` is enabled, the :cpp:func:`nvs_flash_init` API function internally performs some additional steps using the first :ref:`nvs_key_partition` found to enable encryption for the default NVS partition (refer to the API documentation for more details). Alternatively, :cpp:func:`nvs_flash_secure_init` API function can also be used to enable encryption for the default NVS partition.

**Encrypt a custom NVS partition:**
To enable encryption for a custom NVS partition, :cpp:func:`nvs_flash_secure_init_partition` API function is used instead of :cpp:func:`nvs_flash_init_partition`.

When :cpp:func:`nvs_flash_secure_init` and :cpp:func:`nvs_flash_secure_init_partition` API functions are used, the applications are expected to follow the steps below in order to perform NVS read/write operations with encryption enabled.

    1. Find key partition and NVS data partition using ``esp_partition_find*`` API functions.
    2. Populate the :cpp:type:`nvs_sec_cfg_t` struct using the :cpp:func:`nvs_flash_read_security_cfg` or :cpp:func:`nvs_flash_generate_keys` API functions.
    3. Initialise NVS flash partition using the :cpp:func:`nvs_flash_secure_init` or :cpp:func:`nvs_flash_secure_init_partition` API functions.
    4. Open a namespace using the :cpp:func:`nvs_open` or :cpp:func:`nvs_open_from_partition` API functions.
    5. Perform NVS read/write operations using ``nvs_get_*`` or ``nvs_set_*``.
    6. Deinitialise an NVS partition using :cpp:func:`nvs_flash_deinit`.

NVS Partition Generator Utility
-------------------------------

This utility helps generate NVS partition binary files which can be flashed separately on a dedicated partition via a flashing utility. Key-value pairs to be flashed onto the partition can be provided via a CSV file. For more details, please refer to :doc:`NVS Partition Generator Utility <nvs_partition_gen>`.

Application Example
-------------------

You can find code examples in the :example:`storage` directory of ESP-IDF examples:

:example:`storage/nvs_rw_value`

  Demonstrates how to read a single integer value from, and write it to NVS.

  The value checked in this example holds the number of the {IDF_TARGET_NAME} module restarts. The value's function as a counter is only possible due to its storing in NVS.

  The example also shows how to check if a read / write operation was successful, or if a certain value has not been initialized in NVS. The diagnostic procedure is provided in plain text to help you track the program flow and capture any issues on the way.

:example:`storage/nvs_rw_blob`

  Demonstrates how to read a single integer value and a blob (binary large object), and write them to NVS to preserve this value between {IDF_TARGET_NAME} module restarts.

    * value - tracks the number of the {IDF_TARGET_NAME} module soft and hard restarts.
    * blob - contains a table with module run times. The table is read from NVS to dynamically allocated RAM. A new run time is added to the table on each manually triggered soft restart, and then the added run time is written to NVS. Triggering is done by pulling down GPIO0.

  The example also shows how to implement the diagnostic procedure to check if the read / write operation was successful.

:example:`storage/nvs_rw_value_cxx`

  This example does exactly the same as :example:`storage/nvs_rw_value`, except that it uses the C++ NVS handle class.

Internals
---------

Log of Key-Value Pairs
^^^^^^^^^^^^^^^^^^^^^^

NVS stores key-value pairs sequentially, with new key-value pairs being added at the end. When a value of any given key has to be updated, a new key-value pair is added at the end of the log and the old key-value pair is marked as erased.

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
    Page header contains invalid data, and further parsing of page data was canceled. Any items previously written into this page will not be accessible. The corresponding flash sector will not be erased immediately and will be kept along with sectors in *uninitialized* state for later use. This may be useful for debugging.

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

Variable length values (strings and blobs) are written into subsequent entries, 32 bytes per entry. The `Span` field of the first entry indicates how many entries are used.


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

To reduce the number of reads from flash memory, each member of the Page class maintains a list of pairs: item index; item hash. This list makes searches much quicker. Instead of iterating over all entries, reading them from flash one at a time, `Page::findItem` first performs a search for the item hash in the hash list. This gives the item index within the page if such an item exists. Due to a hash collision, it is possible that a different item will be found. This is handled by falling back to iteration over items in flash.

Each node in the hash list contains a 24-bit hash and 8-bit item index. Hash is calculated based on item namespace, key name, and ChunkIndex. CRC32 is used for calculation; the result is truncated to 24 bits. To reduce the overhead for storing 32-bit entries in a linked list, the list is implemented as a double-linked list of arrays. Each array holds 29 entries, for the total size of 128 bytes, together with linked list pointers and a 32-bit count field. The minimum amount of extra RAM usage per page is therefore 128 bytes; maximum is 640 bytes.

API Reference
-------------

.. include-build-file:: inc/nvs_flash.inc

.. include-build-file:: inc/nvs.inc
