Miscellaneous System APIs
=========================

Software reset
--------------

To perform software reset of the chip, :cpp:func:`esp_restart` function is provided. When the function is called, execution of the program will stop, both CPUs will be reset, application will be loaded by the bootloader and started again.

Additionally, :cpp:func:`esp_register_shutdown_handler` function is provided to register a routine which needs to be called prior to restart (when done by :cpp:func:`esp_restart`). This is similar to the functionality of ``atexit`` POSIX function.

Reset reason
------------

ESP-IDF application can be started or restarted due to a variety of reasons. To get the last reset reason, call :cpp:func:`esp_reset_reason` function. See description of :cpp:type:`esp_reset_reason_t` for the list of possible reset reasons.

Heap memory
-----------

Two heap memory related functions are provided:

* :cpp:func:`esp_get_free_heap_size` returns the current size of free heap memory
* :cpp:func:`esp_get_minimum_free_heap_size` returns the minimum size of free heap memory that was available during program execution.

Note that ESP-IDF supports multiple heaps with different capabilities. Functions mentioned in this section return the size of heap memory which can be allocated using ``malloc`` family of functions. For further information about heap memory see :doc:`Heap Memory Allocation <mem_alloc>`.

Random number generation
------------------------

{IDF_TARGET_NAME} contains a hardware random number generator, values from it can be obtained using :cpp:func:`esp_random`.

When Wi-Fi or Bluetooth are enabled, numbers returned by hardware random number generator (RNG) can be considered true random numbers. Without Wi-Fi or Bluetooth enabled, hardware RNG is a pseudo-random number generator. At startup, ESP-IDF bootloader seeds the hardware RNG with entropy, but care must be taken when reading random values between the start of ``app_main`` and initialization of Wi-Fi or Bluetooth drivers.

.. _MAC-Address-Allocation:

MAC Address
-----------

These APIs allow querying and customizing MAC addresses for different network interfaces that supported (e.g. Wi-Fi, Bluetooth, Ethernet).

In ESP-IDF these addresses are calculated from *Base MAC address*. Base MAC address can be initialized with factory-programmed value from internal eFuse, or with a user-defined value. In addition to setting the base MAC address, applications can specify the way in which MAC addresses are allocated to devices. See `Number of universally administered MAC address`_ section for more details.

.. only:: esp32

    +---------------+--------------------------------+----------------------------------+
    | Interface     | MAC address                    | MAC address                      |
    |               | (4 universally administered)   | (2 universally administered)     |
    +===============+================================+==================================+
    | Wi-Fi Station | base_mac                       | base_mac                         |
    +---------------+--------------------------------+----------------------------------+
    | Wi-Fi SoftAP  | base_mac, +1 to the last octet | base_mac, first octet randomized |
    +---------------+--------------------------------+----------------------------------+
    | Bluetooth     | base_mac, +2 to the last octet | base_mac, +1 to the last octet   |
    +---------------+--------------------------------+----------------------------------+
    | Ethernet      | base_mac, +3 to the last octet | base_mac, +1 to the last octet,  |
    |               |                                | first octet randomized           |
    +---------------+--------------------------------+----------------------------------+

.. only:: esp32s2

    +---------------+--------------------------------+----------------------------------+
    | Interface     | MAC address                    | MAC address                      |
    |               | (2 universally administered)   | (1 universally administered)     |
    +===============+================================+==================================+
    | Wi-Fi Station | base_mac                       | base_mac                         |
    +---------------+--------------------------------+----------------------------------+
    | Wi-Fi SoftAP  | base_mac, +1 to the last octet | base_mac, first octet randomized |
    +---------------+--------------------------------+----------------------------------+

Base MAC address
^^^^^^^^^^^^^^^^

To fetch MAC address for a specific interface (e.g. Wi-Fi, Bluetooth, Ethernet), you can simply use :cpp:func:`esp_read_mac` function.

By default, this function takes the eFuse value burned at a pre-defined block (e.g. BLK0 for ESP32, BLK1 for ESP32-S2) as the base MAC address. Per-interface MAC addresses will be calculated according to the table above.

Applications who want to customize base MAC address (not the one provided by Espressif) should call :cpp:func:`esp_base_mac_addr_set` before :cpp:func:`esp_read_mac`. The customized MAC address can be stored in any supported storage device (e.g. Flash, NVS, etc).

Note that, calls to :cpp:func:`esp_base_mac_addr_set` should take place before the initialization of network stack, for example, early in ``app_main``.

Custom MAC address in eFuse
^^^^^^^^^^^^^^^^^^^^^^^^^^^

To facilitate the usage of custom MAC addresses, ESP-IDF provides :cpp:func:`esp_efuse_mac_get_custom` function, which loads MAC address from internal pre-defined eFuse block (e.g. BLK3 for ESP32). This function assumes that custom MAC address is stored in the following format:

+-----------------+-----------+---------------+------------------------------+
| Field           | # of bits | Range of bits | Notes                        |
+=================+===========+===============+==============================+
| Version         | 8         | 191:184       | 0: invalid, others — valid   |
+-----------------+-----------+---------------+------------------------------+
| Reserved        | 128       | 183:56        |                              |
+-----------------+-----------+---------------+------------------------------+
| MAC address     | 48        | 55:8          |                              |
+-----------------+-----------+---------------+------------------------------+
| MAC address CRC | 8         | 7:0           | CRC-8-CCITT, polynomial 0x07 |
+-----------------+-----------+---------------+------------------------------+

Once MAC address has been obtained using :cpp:func:`esp_efuse_mac_get_custom`, call :cpp:func:`esp_base_mac_addr_set` to set this MAC address as base MAC address.


Number of universally administered MAC address
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Several MAC addresses (universally administered by IEEE) are uniquely assigned to the networking interfaces (Wi-Fi/BT/Ethernet). The final octet of each universally administered MAC address increases by one. Only the first one of them (which is called base MAC address) is stored in eFuse or external storage, the others are generated from it. Here, 'generate' means adding 0, 1, 2 and 3 (respectively) to the final octet of the base MAC address.

If the universally administered MAC addresses are not enough for all of the networking interfaces, locally administered MAC addresses which are derived from universally administered MAC addresses are assigned to the rest of networking interfaces.

See `this article <https://en.wikipedia.org/wiki/MAC_address#Universal_vs._local>`_ for the definition of local and universally administered MAC addresses.

.. only:: esp32

    The number of universally administered MAC address can be configured using :ref:`CONFIG_ESP32_UNIVERSAL_MAC_ADDRESSES`.

    If the number of universal MAC addresses is two, only two interfaces (Wi-Fi Station and Bluetooth) receive a universally administered MAC address. These are generated sequentially by adding 0 and 1 (respectively) to the base MAC address. The remaining two interfaces (Wi-Fi SoftAP and Ethernet) receive local MAC addresses. These are derived from the universal Wi-Fi station and Bluetooth MAC addresses, respectively.

    If the number of universal MAC addresses is four, all four interfaces (Wi-Fi Station, Wi-Fi SoftAP, Bluetooth and Ethernet) receive a universally administered MAC address. These are generated sequentially by adding 0, 1, 2 and 3 (respectively) to the final octet of the base MAC address.

    When using the default (Espressif-assigned) base MAC address, either setting can be used. When using a custom universal MAC address range, the correct setting will depend on the allocation of MAC addresses in this range (either 2 or 4 per device.)

.. todo::

   Add illustration for ESP32-S2 when multi-target doc feature is ready.

Chip version
------------

:cpp:func:`esp_chip_info` function fills :cpp:class:`esp_chip_info_t` structure with information about the chip. This includes the chip revision, number of CPU cores, and a bit mask of features enabled in the chip.

.. _idf-version-h:

SDK version
-----------

:cpp:func:`esp_get_idf_version` returns a string describing the IDF version which was used to compile the application. This is the same value as the one available through ``IDF_VER`` variable of the build system. The version string generally has the format of ``git describe`` output.

To get the version at build time, additional version macros are provided. They can be used to enable or disable parts of the program depending on IDF version.

* :c:macro:`ESP_IDF_VERSION_MAJOR`, :c:macro:`ESP_IDF_VERSION_MINOR`, :c:macro:`ESP_IDF_VERSION_PATCH` are defined to integers representing major, minor, and patch version.

* :c:macro:`ESP_IDF_VERSION_VAL` and :c:macro:`ESP_IDF_VERSION` can be used when implementing version checks:

  .. code-block:: c

      #include "esp_idf_version.h"

      #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
          // enable functionality present in IDF v4.0
      #endif


App version
-----------
Application version is stored in :cpp:class:`esp_app_desc_t` structure. It is located in DROM sector and has a fixed offset from the beginning of the binary file.
The structure is located after :cpp:class:`esp_image_header_t` and :cpp:class:`esp_image_segment_header_t` structures. The field version has string type and max length 32 chars.

To set version in your project manually you need to set ``PROJECT_VER`` variable in your project CMakeLists.txt/Makefile:

* In application CMakeLists.txt put ``set(PROJECT_VER "0.1.0.1")`` before including ``project.cmake``.

(For legacy GNU Make build system: in application Makefile put ``PROJECT_VER = "0.1.0.1"`` before including ``project.mk``.)

If :ref:`CONFIG_APP_PROJECT_VER_FROM_CONFIG` option is set, the value of :ref:`CONFIG_APP_PROJECT_VER` will be used. Otherwise if ``PROJECT_VER`` variable is not set in the project then it will be retrieved from either ``$(PROJECT_PATH)/version.txt`` file (if present) else using git command ``git describe``. If neither is available then ``PROJECT_VER`` will be set to "1". Application can make use of this by calling :cpp:func:`esp_ota_get_app_description` or :cpp:func:`esp_ota_get_partition_description` functions.



API Reference
-------------

.. include-build-file:: inc/esp_system.inc
.. include-build-file:: inc/esp_idf_version.inc


