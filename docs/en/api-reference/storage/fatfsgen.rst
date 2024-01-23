:orphan:

.. linked from fatfs.rst

Generating and Parsing FATFS on Host
====================================

:link_to_translation:`zh_CN:[中文]`

This document is intended mainly for developers of Python tools :component_file:`fatfsgen.py <fatfs/fatfsgen.py>` and :component_file:`fatfsparse.py <fatfs/fatfsparse.py>`, and people with special interest in these tools and implementation of the FAT file system in ESP-IDF. If you are interested in using these tools, please refer to the user guide at :ref:`fatfs-partition-generator`.

The FAT file system is composed of various logical units. The units are used to store general information about the file system, allocations, content of files and directories, and file's metadata. The tools ``fatfsgen.py`` and ``fatfsparse.py`` are used to implement the FAT file system while considering all these logical units, and they also provide support for wear levelling.


FAT File System Generator and Parser Design
-------------------------------------------

This section describes particular units of the FAT file system generator and parser design. The implementation aims to create a valid model of the FAT structure with a focus on macro operations, generating and parsing the whole partition without modifying it in the run (mounting).

.. figure:: ../../../_static/classes_fatfsgen.svg
    :align: center
    :alt: Class diagram

    FAT File System Generator and Parser Design Architecture


Class FATFS
^^^^^^^^^^^

This is the most general entity responsible for modeling the FAT file system. It is composed of **FATFSState** (holding metadata and boot sector), **FAT** (holding file allocation table), and **Directory** (representing the root directory required by FAT12 and FAT16). The class processes all the requirements for the partition, analyses the local folder dedicated to transforming it into a binary image, and generates an internal representation of the local folder. Then, the class can generate a binary image from the internal FAT file system model.

Class WLFATFS
^^^^^^^^^^^^^

The class extends the functionality of the class **FATFS**. It implements an encapsulation of the file system into the wear levelling, by adding the "dummy" sector for balancing the load (a redundant sector, see the section :ref:`fafsgen-wear-levelling`), configuration sector and state sector. This class generates a binary FATFS partition with initialized wear levelling layer. For further analysis, it also provides an option to remove the wear levelling completely. The class is instantiated and invoked by the ``wl_fatfsgen.py`` script.

Class BootSectorState
^^^^^^^^^^^^^^^^^^^^^

The instance of this class contains the metadata required for building a boot sector and BPB (BIOS Parameter Block). Boot sector is basically implemented for the cross-platform compatibility, i.e., when ESP chipsets are connected with other platforms, it will always follow all the FAT file system standards. However, during partition generation, chip does not consume the data in this boot sector and all the other data needed, as the data is constant. In other words, changing the fields with the prefix "BS" is usually unnecessary and often does not work. If you want to add new features, please focus on fields with the prefix "BPB". Another critical role of this class is to share access to the metadata and binary image over the whole class system. Because of this, every class in the system can access this singleton.

Class FATFSState
^^^^^^^^^^^^^^^^

The class **FATFSState** might be obsolete in the future, so developers could transfer its functionality into the **BootSectorState**. The class contains a reference to the **BootSectorState** and extends the data with some unknown information when creating a boot sector or unnecessary for the boot sector, such as the information generated when the file system supports long file names.

Class FAT
^^^^^^^^^

FAT represents the File Allocation Table. FAT is a sequence of bytes spread over one or more sectors. The number of sectors is determined by the number of clusters and is calculated by the function ``get_fat_sectors_count`` in ``utils.py``. The aim is to have as few sectors for one FAT as possible when you refer to every physical cluster in the file system. The FAT works as follows: For every physical cluster at ``i * some_constant`` address, FAT contains an entry at the ``i``-th location which represents next address of the clusters in the file chain. Each version of the FAT file system uses a different size for FAT entries. FAT12 uses 12 bits per entry, thus 2 entries span 3 bytes. FAT16 uses 16 bits per entry, thus 1 entry spans 2 bytes. FAT32 uses 32 bits per FAT entry, thus one entry spans 4 bytes. All entries are in little-endian byte order.

All zeros at the ``i``-th entry indicates that corresponding cluster is free, while all ones at the ``i``-th entry indicates that corresponding cluster is occupied and is the last cluster in the file chain. The other number at ``ith``-th entry determines the next cluster's address in the file chain. These clusters are not necessarily stored adjacent to one another in the memory but instead are often fragmented throughout the data region.

For partition generation, the file is divided into several parts to fit the cluster. Notice that the structure allocation of the files is a linked list. Every cluster in the file allocation chain has entry in the FAT which refers to the next cluster or the information about the last cluster in the file chain. As mentioned, FAT12 uses 12 bits per FAT entry, thus it can sets a maximum number of 4096 clusters, as with 12 bits (one and a half bytes), it can enumerate 4096 clusters at most. However, because of other overhead, FAT12 can have 4085 clusters at most. Similarly, FAT16 can have 65525 clusters at most and for FAT32 can have 268435445 clusters at most (as practically only 28 bits out of 32 bits are used to denote each FAT entry). The current implementation doesn't allow forcibly redefining the FAT file system with less than 4085 clusters to FAT16, even though the documentation claims it is possible. Notice that it would be meaningless to define it vice versa, i.e., to FAT12 with more than 4085 clusters (which implies clusters on higher addresses being inaccessible).

Class Cluster
^^^^^^^^^^^^^

The **Cluster** class is responsible for accessing the FAT entries and the corresponding physical cluster. The class **FAT** is an aggregation of a particular number of **Cluster** instances. Every cluster is uniquely defined by the ID, which also determines its position in FAT and corresponding sectors in the data region. When the cluster is allocated, it includes a reference to a file or directory. It happens only if the cluster is the first in the allocation chain. The cluster contains information about whether it is empty and the last in the file allocation linked list. If not, it includes a reference to the next cluster in the linked list. In practical usage, the cluster doesn't need to access its file, but the other way around. The **File** or **Directory** accesses its cluster, to retrieve the entire content that might be chained.

.. figure:: ../../../_static/fat_table.svg
   :align: center
   :alt: Table diagram


Class Directory
^^^^^^^^^^^^^^^

This class represents the file system directory. An instance of **Directory** contains the reference to the corresponding instance of **Cluster**, which has the first cluster in the allocation chain for the directory given. The root directory is a special case with a different count of sectors and a slightly different instantiation process. However, the root directory is still an instance of this class and is the only **Directory** instance associated with the class **FATFS** and **WLFATFS** respectively. The class **Directory** (except for the root directory) has one-to-one association with the class **Entry** that defines its entry in the parent directory. It also has an aggregation associated with the class **Entry**, because every directory contains multiple entries that consist of the actual directory's content (for example, aliases, files, and directories).

Class File
^^^^^^^^^^

Similar to the class **Directory**, **File** represents single file in the file system. This class has one-to-one association with its first cluster in the allocation chain. Through this cluster, the **File** class may access the corresponding physical address and thus modifying its content. Every file also has one-to-one association with **Entry** instance belonging to its parent directory.

Class Entry
^^^^^^^^^^^

**Entry** encapsulates information about the file/directory name in the data region of corresponding parent directory. Every file system entity (File/Directory) has an entry. In case of the symlink, the entity can have multiple entries. The directory uses entries to access its descendant files and sub-directories, and enables traversing the tree structure. Except for that, **Entry** holds the name, extension, size, and information regarding the used file name size (long file names or file names 8.3), etc.

.. figure:: ../../../_static/tree_fatfs.svg
   :align: center
   :alt: Tree diagram


``fatfsgen.py``
---------------

:component_file:`fatfsgen.py <fatfs/fatfsgen.py>` generates FAT file systems on the host.

``fatfsgen.py`` recursively traverses the given folder's directory structure and adds files and/or directories inside the binary partition. Users can set if the script generates the partition with wear levelling support, long file names support, and support for preserving the modification date and time from the original folder on the host.

The ``./fatfsgen.py Espressif`` command generates a simple binary partition with the default settings. Here ``Espressif`` is the local folder (containing files and/or sub-directories) from which binary image is generated.

There exist two scripts for that purpose, :component_file:`fatfsgen.py <fatfs/fatfsgen.py>` and :component_file:`wl_fatfsgen.py <fatfs/wl_fatfsgen.py>`. The difference is that ``wl_fatfsgen.py`` firstly uses ``fatfsgen.py`` for generating the partition and then initializes wear leveling.

The script command line arguments are as follows::

    fatfsgen.py [-h] [--output_file OUTPUT_FILE] [--partition_size PARTITION_SIZE] [--sector_size {4096}] [--long_name_support] [--use_default_datetime] input_directory

    --output_file: path to the generated binary partition
    --partition_size: defines the size of the binary partition (decimal, hexa or binary number)
    --sector_size: the size of the sector
    --long_name_support: flag for supporting long file names
    --use_default_datetime: this flag forces using default dates and times (date == 0x2100, time == 0x0000), not using argument to preserve the original file system metadata
    input_directory: required argument, name of the directory being encoded to the binary fat-compatibile partition

``fatfsparse.py``
-----------------

:component_file:`fatfsparse.py <fatfs/fatfsparse.py>` translates the binary image into the internal representation and generates the folder with equivalent content on the host. If user requires a parsing partition with initialized wear levelling, the ``fatfsparse.py`` will remove the wear levelling sectors using the function ``remove_wl`` provided by ``wl_fatfsgen.py``. After the sectors are removed, parsing of the partition is the same as with no initial wear levelling.

``./fatfsparse.py fatfs_image.img`` command yields the directory with the equivalent content as the binary data image ``fatfs_image.img``.

The script command line arguments are as follows::

    fatfsparse.py [-h] [--wl-layer {detect,enabled,disabled}] input_image

    --wl-layer: indicates if wear leveling is enabled, disabled or should be detected (detection is ambiguous)
    input_image: path to binary image

The long file names can be detected automatically. However, the wear leveling cannot be 100\% detected, because one partition can be valid either with or without wear leveling, according to the user's context. When the script finds wear leveling sectors (cfg and state), it assumes wear leveling is enabled, however it might be a false positive.


Features
--------

FAT12/FAT16
^^^^^^^^^^^

The supported FAT types are FAT12 and FAT16. For smaller partitions, FAT12 is sufficient. The type is detected according to the count of clusters, and cannot be changed by the user. If there are less than 4085 clusters, the selected type is FAT12 (FAT's entries have 12 bits). For partitions with 4085 to 65526 clusters (with 4085 and 65526 excluded), the type is FAT16. Currently ``fatfsgen.py`` or ``fatfsparse.py`` cannot process file systems with more than 65525 clusters.

.. _fafsgen-wear-levelling:

Wear Levelling
^^^^^^^^^^^^^^
There are two types of operations related to the wear levelling layer, initializing wear leveling records and removing wear leveling records during generation and parsing of the FAT file system image.

1. Initializing Wear Levelling

When a new image with wear leveling support is generated, the script initializes few extra sectors necessary for the wear leveling function.

    - The dummy sector: This is an empty sector placed at the beginning of the partition and it will be ignored when file system is being mounted. The dummy sector copies the content of the next sector and then swaps its position with the next sector (or the first sector in case dummy sector was the last) after particular number of erase cycles. In this way, each FAT file system sector traverses across the whole range of flash partition, and thus the erase cycles corresponding to this sector gets distributed across the entire flash.

    - The state sector: State sector has 64 byte data stored.
        - pos: position of the dummy sector
        - max_pos: number of sectors in the partition (excluding config and state sectors)
        - move_count: indicates how many times dummy sector traversed through the entire flash
        - access_count: count of sector erase cycles after which dummy sector will swap its position
        - max_count: equal to wl_config_t::updaterate
        - block_size: equal to wl_config_t::page_size
        - version: equal to wl_config_t::version
        - device_id: generated randomly when the state is first initialized
        - reserved: 7 x 32-bit words, set to 0
        - crc32: crc32 of all the previous fields, including reserved

      Also, the state sector will be appended by 16-byte ``pos update record`` for every value of ``pos``. Thus, this record will help us to determine the position of the dummy sector.

      Since ``erase + write`` operation of the state sector is not atomic, we may lose the data if the power is cut off between "erase" and "write". However, two copies of the state are maintained to recover the state after the power outage. On each update, both copies are updated. Thus, after power outage, we can revert the original valid state.

    - The config sector: This sector contains the information about the partition used by the wear leveling layer.
        - start_addr: start address of partition (always 0)
        - full_mem_size: size of the partition, including data, dummy, state x 2, config sectors. Value is in bytes
        - page_size: equal to sector size (generally 4096)
        - sector_size: always 4096 for the types of NOR flash supported by ESP-IDF
        - updaterate: ESP-IDF always sets this to 16. Could be made a config option at some point
        - wr_size: always set to 16
        - version: current version is 2
        - temp_buff_size: always set to 32 (This shouldn't actually have been stored in flash)
        - crc: crc32 of all the previous values

2. Removing Wear Levelling
While removing wear leveling records, we have to find the position of the dummy sector, and the original and valid orders of the partition (because traversing the dummy sector shuffles the partition). The script can remove other wear leveling sectors from the partition. Steps to remove wear leveling records are given below:

    - Find the ``pos``, position of the dummy sector, which will be determined by the number of ``pos update records`` in the state sector.
    - Create the new image by removing dummy sector and merging remaining sectors before and after dummy sector.
    - Then remove the wear leveling state sectors and config sector which are placed at the end of the partition.
    - Reorder the new image to get its original order. ``move_count`` helps us to find the beginning of the partition. The partition will start at the position ``end_of_partition - move_count``. Thus the beginning of the partition after removing wear leveling sectors will be ``partition[end_of_partition - (move_count*page_size)]``.

File Names Encoding
^^^^^^^^^^^^^^^^^^^

The protocol FAT supports two types of file names.

Short File Names (SFN)
^^^^^^^^^^^^^^^^^^^^^^

The SFN is mandatory for the implementation of file names. SFN refer to the 8.3 file name convention, with 8 characters for the file name and 3 characters for the extension. This pattern is case-insensitive, however, all file names are changed to uppercase in the inner representation of the generator. The entry describing the short file names is 32 bytes long and its structure is as follows::

    Offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
    0x000000: 46 49 4C 45 4E 41 4D 45 45 58 54 20 18 00 00 00    FILENAMEEXT.....
    0x000010: 21 00 21 00 00 00 00 00 21 00 02 00 1E 00 00 00    !.!.....!.......

The entry denotes the file with 8.3 file name ("FILENAME.EXT") __(0x00/00-0A)__ of size 0x1E = 30 bytes __(0x10/0x0C)__, with default times of modification and creation (0x0021) __(0x10/00,02 and 08)__. The relevant cluster for the file is located at __0x02 (0x10/0A)__. Please notice that a character is encoded using one byte (e.g., __0x46 == 'F'__)

Long File Names (LFN)
^^^^^^^^^^^^^^^^^^^^^

The LFN supports 255 characters excluding the trailing ``NULL``. The LFN supports any character as short file names with an additional period ``.`` and the following special characters: ``+ , ; = [ ]``. LFN uses UNICODE, so the character is encoded using 2 bytes.

The structure of one name encoded using LFN is as follows::

    00003000: 42 65 00 2E 00 74 00 78 00 74 00 0F 00 43 FF FF    Be...t.x.t...C..
    00003010: FF FF FF FF FF FF FF FF FF FF 00 00 FF FF FF FF    ................
    00003020: 01 74 00 68 00 69 00 73 00 69 00 0F 00 43 73 00    .t.h.i.s.i...Cs.
    00003030: 6C 00 6F 00 6E 00 67 00 66 00 00 00 69 00 6C 00    l.o.n.g.f...i.l.
    00003040: 54 48 49 53 49 53 7E 31 54 58 54 20 00 00 D6 45    THISIS~1TXT...VE
    00003050: 26 55 26 55 00 00 D6 45 26 55 02 00 1C 00 00 00    &U&U..VE&U......

The above example encodes a file name ``thisislongfile.txt``. The record is composed of multiple entries. The first entry contains metadata and is equivalent to the SFN entry. This entry might be final if the file name conforms to the 8.3 file name convention. In such scenarios, the SFN pattern is used. Otherwise, the generator adds various entries with the LFN structure above the SFN entry. These entries hold information about the file name and its checksum for consistency. Every LFN record can hold 13 characters (26 bytes). The file name is firstly cut into some amount of 13-character substrings and these are added above the SFN entry.

We add LFN entries in reversed order, so the first entry in the directory is the last part of the file name and the last is SFN entry. In the above example, we can see that the first entry contains text ``e.txt``, while the others contain the beginning of the name ``thisislongfil``. The first byte in LFN entries denotes an order or the sequence number (numbered from 1). To determine the first entry of the LFN, the first byte is masked with 0x40 (``first_byte =| 0x40``). The specification says that the last entry value will be ORed with 0x40 and it is the mark for the last entry. For example, when the record is the second and also the last in the LFN entry, its first byte is ``0x42``.

The LFN entry is signed at field **DIR_Attr** with value ``ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID`` (see the file ``long_filename_utils.py``). The SFN entry (possibly also within LFN) contains either ``ATTR_DIRECTORY`` or ``ATTR_ARCHIVE`` in this field for directory or file respectively.

The LFN entry is tagged at the field **DIR_NTRes** with the value ``0x00``. This is a sign of the SFN entry in the LFN record, if the entry is a whole SFN record, the value is ``0x18``. As you can see in the first example, the value at this field is ``0x18``, because the name **"FILENAME.EXT"** fits the SFN. However, the recent example showing **"thisislongfile.txt"** has value ``0x00`` at field **DIR_NTRes** in the last entry, since it is a LFN. The SFN needs to be unique. For that purpose, the ``fatfsgen.py`` uses the first 6 characters from the file name, concatenating with ``~`` and with ID denoting the order of the name with the same prefix. The ID is between 0 to 127, which is the maximal amount of files with the same prefix.

Calculation of the checksum is described and implemented in the ``utils.py`` by function ``lfn_checksum``. The ``fatfsparse.py`` assumes that the LFN entries might not be right next to each other, but it assumes the relative order is preserved. The approach is first to find the SFN belonging to some LFN record (using **DIR_NTRes** field). From then, the script starts to search by moving upwards to the beginning of the respective sector, until it finds the last entry in the LFN record (the one with the first half byte equal to 4). The entries are distinguished by their checksums. When finished, the file name can be composed.

Date and Time in FAT File System
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The FAT file system protocol used by ESP-IDF does not preserve the date or time on the chips' media, so all the images extracted from the device have the same default timestamp for all the FAT-specified date-time fields (creation and the last modification timestamp as well as creation, last modification and last access dates).

There are a couple of fields in the SFN entry describing time, such as **DIR_CrtTime** and **DIR_WrtTime**. Some fields are ignored by the FAT implementation used by ESP-IDF (see the file ``entry.py``). However, changes in the fields **DIR_WrtTime** and **DIR_WrtDate** are preserved in the chip. Both time and data entry are 16-bit, where the granularity of the time is 2 seconds.
