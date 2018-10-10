Non-volatile storage library
============================

Introduction
------------

Non-volatile storage (NVS) library is designed to store key-value pairs in flash. This sections introduces some concepts used by NVS.

Underlying storage
^^^^^^^^^^^^^^^^^^

Currently NVS uses a portion of main flash memory through ``spi_flash_{read|write|erase}`` APIs. The library uses the all the partitions with ``data`` type and ``nvs`` subtype. The application can choose to use the partition with label ``nvs`` through ``nvs_open`` API or any of the other partition by specifying its name through ``nvs_open_from_part`` API.

Future versions of this library may add other storage backends to keep data in another flash chip (SPI or I2C), RTC, FRAM, etc.

.. note:: if an NVS partition is truncated (for example, when the partition table layout is changed), its contents should be erased. ESP-IDF build system provides a ``make erase_flash`` target to erase all contents of the flash chip.

.. note:: NVS works best for storing many small values, rather than a few large values of type 'string' and 'blob'. If storing large blobs or strings is required, consider using the facilities provided by the FAT filesystem on top of the wear levelling library.

Keys and values
^^^^^^^^^^^^^^^

NVS operates on key-value pairs. Keys are ASCII strings, maximum key length is currently 15 characters. Values can have one of the following types:

-  integer types: ``uint8_t``, ``int8_t``, ``uint16_t``, ``int16_t``, ``uint32_t``, ``int32_t``, ``uint64_t``, ``int64_t``
-  zero-terminated string
-  variable length binary data (blob)

.. note::
   String values are currently limited to 4000 bytes. This includes the null terminator. Blob values are limited to 508000 bytes or (97.6% of the partition size - 4000) bytes whichever is lower.

Additional types, such as ``float`` and ``double`` may be added later.

Keys are required to be unique. Writing a value for a key which already exists behaves as follows:

-  if the new value is of the same type as old one, value is updated
-  if the new value has different data type, an error is returned

Data type check is also performed when reading a value. An error is returned if data type of read operation doesn’t match the data type of the value.

Namespaces
^^^^^^^^^^

To mitigate potential conflicts in key names between different components, NVS assigns each key-value pair to one of namespaces. Namespace names follow the same rules as key names, i.e. 15 character maximum length. Namespace name is specified in the ``nvs_open`` or ``nvs_open_from_part`` call. This call returns an opaque handle, which is used in subsequent calls to ``nvs_read_*``, ``nvs_write_*``, and ``nvs_commit`` functions. This way, handle is associated with a namespace, and key names will not collide with same names in other namespaces.
Please note that the namespaces with same name in different NVS partitions are considered as separate namespaces.

Security, tampering, and robustness
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

NVS is not directly compatible with the ESP32 flash encryption system. However, data can still be stored in encrypted form if NVS encryption is used together with ESP32 flash encryption. Please refer to :ref:`nvs_encryption` for more details.

If NVS encryption is not used, it is possible for anyone with physical access to the flash chip to alter, erase, or add key-value pairs. With NVS encryption enabled, it is not possible to alter or add a key-value pair and get recognized as a valid pair without knowing corresponding NVS encryption keys. However, there is no tamper-resistance against erase operation.

The library does try to recover from conditions when flash memory is in an inconsistent state. In particular, one should be able to power off the device at any point and time and then power it back on. This should not result in loss of data, expect for the new key-value pair if it was being written at the moment of power off. The library should also be able to initialize properly with any random data present in flash memory.

Internals
---------

Log of key-value pairs
^^^^^^^^^^^^^^^^^^^^^^

NVS stores key-value pairs sequentially, with new key-value pairs being added at the end. When a value of any given key has to be updated, new key-value pair is added at the end of the log and old key-value pair is marked as erased.

Pages and entries
^^^^^^^^^^^^^^^^^

NVS library uses two main entities in its operation: pages and entries. Page is a logical structure which stores a portion of the overall log. Logical page corresponds to one physical sector of flash memory. Pages which are in use have a *sequence number* associated with them. Sequence numbers impose an ordering on pages. Higher sequence numbers correspond to pages which were created later. Each page can be in one of the following states:

Empty/uninitialized
    Flash storage for the page is empty (all bytes are ``0xff``). Page isn't used to store any data at this point and doesn’t have a sequence number.

Active
    Flash storage is initialized, page header has been written to flash, page has a valid sequence number. Page has some empty entries and data can be written there. At most one page can be in this state at any given moment.

Full
    Flash storage is in a consistent state and is filled with key-value pairs.
    Writing new key-value pairs into this page is not possible. It is still possible to mark some key-value pairs as erased.

Erasing
    Non-erased key-value pairs are being moved into another page so that the current page can be erased. This is a transient state, i.e. page should never stay in this state when any API call returns. In case of a sudden power off, move-and-erase process will be completed upon next power on.

Corrupted
    Page header contains invalid data, and further parsing of page data was canceled. Any items previously written into this page will not be accessible. Corresponding flash sector will not be erased immediately, and will be kept along with sectors in *uninitialized* state for later use. This may be useful for debugging.

Mapping from flash sectors to logical pages doesn't have any particular order. Library will inspect sequence numbers of pages found in each flash sector and organize pages in a list based on these numbers.

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

Structure of a page
^^^^^^^^^^^^^^^^^^^

For now we assume that flash sector size is 4096 bytes and that ESP32 flash encryption hardware operates on 32-byte blocks. It is possible to introduce some settings configurable at compile-time (e.g. via menuconfig) to accommodate flash chips with different sector sizes (although it is not clear if other components in the system, e.g. SPI flash driver and SPI flash cache can support these other sizes).

Page consists of three parts: header, entry state bitmap, and entries themselves. To be compatible with ESP32 flash encryption, entry size is 32 bytes. For integer types, entry holds one key-value pair. For strings and blobs, an entry holds part of key-value pair (more on that in the entry structure description).

The following diagram illustrates page structure. Numbers in parentheses indicate size of each part in bytes. ::

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

Page header and entry state bitmap are always written to flash unencrypted. Entries are encrypted if flash encryption feature of the ESP32 is used.

Page state values are defined in such a way that changing state is possible by writing 0 into some of the bits. Therefore it not necessary to erase the page to change page state, unless that is a change to *erased* state.

The version field in the header reflects NVS format version used. For backward compatibility reasons, it is decremented for every version upgrade starting at 0xff (i.e. 0xff for version-1, 0xfe for version-2 and so on).

CRC32 value in header is calculated over the part which doesn't include state value (bytes 4 to 28). Unused part is currently filled with ``0xff`` bytes.

The following sections describe structure of entry state bitmap and entry itself.

Entry and entry state bitmap
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Each entry can be in one of the following three states. Each state is represented with two bits in the entry state bitmap. Final four bits in the bitmap (256 - 2 * 126) are unused.

Empty (2'b11)
    Nothing is written into the specific entry yet. It is in an uninitialized state (all bytes ``0xff``). 

Written (2'b10)
    A key-value pair (or part of key-value pair which spans multiple entries) has been written into the entry.

Erased (2'b00)
    A key-value pair in this entry has been discarded. Contents of this entry will not be parsed anymore.


.. _structure_of_entry:

Structure of entry
^^^^^^^^^^^^^^^^^^

For values of primitive types (currently integers from 1 to 8 bytes long), entry holds one key-value pair. For string and blob types, entry holds part of the whole key-value pair. For strings, in case when a key-value pair spans multiple entries, all entries are stored in the same page. Blobs are allowed to span over multiple pages by dividing them into smaller chunks. For the purpose tracking these chunks, an additional fixed length metadata entry is stored called "blob index" entry. Earlier format of blobs are still supported (can be read and modified). However, once the blobs are modified, they are stored using the new format.

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
    Namespace index for this entry. See section on namespaces implementation for explanation of this value.

Type
    One byte indicating data type of value. See ``ItemType`` enumeration in ``nvs_types.h`` for possible values.

Span
    Number of entries used by this key-value pair. For integer types, this is equal to 1. For strings and blobs this depends on value length.

ChunkIndex
    Used to store index of the blob-data chunk for blob types. For other types, this should be ``0xff``.

CRC32
    Checksum calculated over all the bytes in this entry, except for the CRC32 field itself.

Key
    Zero-terminated ASCII string containing key name. Maximum string length is 15 bytes, excluding zero terminator.

Data
    For integer types, this field contains the value itself. If the value itself is shorter than 8 bytes it is padded to the right, with unused bytes filled with ``0xff``. 

    For "blob index" entry, these 8 bytes hold the following information about data-chunks:

    - Size
        (Only for blob index.) Size, in bytes, of complete blob data.

    - ChunkCount 
        (Only for blob index.) Total number of blob-data chunks into which the blob was divided during storage. 
     
    - ChunkStart 
        (Only for blob index.) ChunkIndex of the first blob-data chunk of this blob. Subsequent chunks have chunkIndex incrementely allocated (step of 1). 

    For string and blob data chunks, these 8 bytes hold additional data about the value, described next:
  
    - Size
        (Only for strings and blobs.) Size, in bytes, of actual data. For strings, this includes zero terminator.

    - CRC32
        (Only for strings and blobs.) Checksum calculated over all bytes of data.

Variable length values (strings and blobs) are written into subsequent entries, 32 bytes per entry. `Span` field of the first entry indicates how many entries are used.


Namespaces
^^^^^^^^^^

As mentioned above, each key-value pair belongs to one of the namespaces. Namespaces identifiers (strings) are stored as keys of key-value pairs in namespace with index 0. Values corresponding to these keys are indexes of these namespaces. 

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


Item hash list
^^^^^^^^^^^^^^

To reduce the number of reads performed from flash memory, each member of Page class maintains a list of pairs: (item index; item hash). This list makes searches much quicker. Instead of iterating over all entries, reading them from flash one at a time, ``Page::findItem`` first performs search for item hash in the hash list. This gives the item index within the page, if such an item exists. Due to a hash collision it is possible that a different item will be found. This is handled by falling back to iteration over items in flash.

Each node in hash list contains a 24-bit hash and 8-bit item index. Hash is calculated based on item namespace, key name and ChunkIndex. CRC32 is used for calculation, result is truncated to 24 bits. To reduce overhead of storing 32-bit entries in a linked list, list is implemented as a doubly-linked list of arrays. Each array holds 29 entries, for the total size of 128 bytes, together with linked list pointers and 32-bit count field. Minimal amount of extra RAM useage per page is therefore 128 bytes, maximum is 640 bytes.

.. _nvs_encryption:

NVS Encryption
--------------

Data stored in NVS partitions can be encrypted using AES-XTS in the manner similar to one mentioned in disc encryption standard IEEE P1619. For the purpose of encryption, each entry is considered as one `sector` and relative address of the entry (w.r.t. partition-start) is fed to the encryption algorithm as `sector-number`. The keys required for nvs encryption are stored in yet another partition, which is protected using :doc:`Flash Encryption <../../security/flash-encryption>`. Therefore, enabling :doc:`Flash Encryption <../../security/flash-encryption>` is a prerequisite for NVS encryption.

.. _nvs_key_partition:

NVS key partition
^^^^^^^^^^^^^^^^^

An application requiring NVS encryption support needs to be compiled with a key-partition of type `data` and subtype `key`. This partition should be marked as `encrypted`. Refer to :doc:`Partition Tables <../../api-guides/partition-tables>` for more details. The size of the partition should be 4096 bytes (minimum partition size). The structure of this partition is depicted below. 

::

    +-----------+--------------+-------------+----+
    |              XTS encryption key(32)         |
    +---------------------------------------------+
    |              XTS tweak key (32)             |
    +---------------------------------------------+
    |                  CRC32(4)                   |
    +---------------------------------------------+

This partition can be generated using `nvs partition generator` utility and flashed onto the device. Since the partition is marked `encrypted` and :doc:`Flash Encryption <../../security/flash-encryption>` is enabled, bootloader will encrypt this partition using flash encryption key on first boot. Alternatively, the keys can be generated after startup using ``nvs_flash_generate_keys`` API provided by ``nvs_flash.h``, which will then write those keys onto the key-partition in encrypted form.

It is possible for an application to use different keys for different NVS partitions and thereby have multiple key-partitions. However, it is a responsibilty of the application to provide correct key-partition/keys for the purpose of encryption/decryption.

Encrypted Read/Write
^^^^^^^^^^^^^^^^^^^^

The same NVS APIs ``nvs_read_*`` or ``nvs_write_*`` can be used for reading and writing of encrypted nvs partition as well. However, the APIs for initialising NVS partitions are different. ``nvs_flash_secure_init`` and ``nvs_flash_secure_init_partition`` are used for initialising instead of ``nvs_flash_init`` and ``nvs_flash_init_partition`` respectively. ``nvs_sec_cfg_t`` structure required for these APIs can be populated using ``nvs_flash_generate_keys`` or ``nvs_flash_read_security_cfg``.

Applications are expected to follow the following steps in order to perform NVS read/write operations with encryption enabled.

    1. Find key partition and NVS data partition using ``esp_partition_find*`` APIs.
    2. Populate ``nvs_sec_cfg_t`` struct using ``nvs_flash_read_security_cfg`` or ``nvs_flash_generate_keys`` APIs.
    3. Initialise NVS flash partition using ``nvs_flash_secure_init`` or ``nvs_flash_secure_init_partition`` APIs.
    4. Open a namespace using ``nvs_open`` or ``nvs_open_from_part`` APIs
    5. Perform NVS read/write operations using ``nvs_read_*`` or ``nvs_write_*``
    6. Deinitialise NVS partition using ``nvs_flash_deinit``.
