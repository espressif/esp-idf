Miscellaneous System APIs
=========================

{IDF_TARGET_BASE_MAC_BLOCK: default="BLK1", esp32="BLK0"}

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

.. _MAC-Address-Allocation:

MAC Address
-----------

These APIs allow querying and customizing MAC addresses for different network interfaces that supported (e.g. Wi-Fi, Bluetooth, Ethernet).

.. only:: SOC_BT_SUPPORTED

    To fetch MAC address for a specific interface (e.g. Wi-Fi, Bluetooth, Ethernet), call the function :cpp:func:`esp_read_mac` function.

.. only:: not SOC_BT_SUPPORTED

    To fetch MAC address for a specific interface (e.g. Wi-Fi Station, Wi-Fi SoftAP), call the function :cpp:func:`esp_read_mac` function.

In ESP-IDF these addresses are calculated from a single *Base MAC address*. By default, the Espressif base MAC address is used. This MAC is pre-programmed into {IDF_TARGET_NAME} eFuse from the factory.

.. only:: not esp32s2

    +---------------+---------------------------------------+-----------------------------------------------+
    | Interface     | MAC address                           | MAC address                                   |
    |               | (4 universally administered, default) | (2 universally administered)                  |
    +===============+=======================================+===============================================+
    | Wi-Fi Station | base_mac                              | base_mac                                      |
    +---------------+---------------------------------------+-----------------------------------------------+
    | Wi-Fi SoftAP  | base_mac, +1 to the last octet        | :ref:`Local MAC <local-mac-addresses>`        |
    |               |                                       | derived from Wi-Fi Station MAC)               |
    +---------------+---------------------------------------+-----------------------------------------------+
    | Bluetooth     | base_mac, +2 to the last octet        | base_mac, +1 to the last octet                |
    +---------------+---------------------------------------+-----------------------------------------------+
    | Ethernet      | base_mac, +3 to the last octet        | :ref:`Local MAC <local-mac-addresses>`        |
    |               |                                       | (derived from Bluetooth MAC)                  |
    +---------------+---------------------------------------+-----------------------------------------------+

    .. note::

       The default :ref:`configuration <CONFIG_{IDF_TARGET_CFG_PREFIX}_UNIVERSAL_MAC_ADDRESSES>`
       is 4 universally administered MAC addresses, and this is recommended when using
       Espressif-provided MAC addresses.

.. only:: esp32s2

    +---------------+---------------------------------------+-----------------------------------------------+
    | Interface     | MAC address                           | MAC address                                   |
    |               | (2 universally administered, default) | (1 universally administered)                  |
    +===============+=======================================+===============================================+
    | Wi-Fi Station | base_mac                              | base_mac                                      |
    +---------------+---------------------------------------+-----------------------------------------------+
    | Wi-Fi SoftAP  | base_mac, +1 to the last octet        | :ref:`Local MAC <local-mac-addresses>`        |
    |               |                                       | (derived from Wi-Fi Station MAC)              |
    +---------------+---------------------------------------+-----------------------------------------------+
    | Ethernet      | :ref:`Local MAC <local-mac-addresses>`| :ref:`Local MAC <local-mac-addresses>`        |
    | (see note)    | (derived from Wi-Fi SoftAP MAC        | (derived from base_mac with +1 to last octet. |
    |               |                                       | Not recommended.)                             |
    +---------------+---------------------------------------+-----------------------------------------------+

    .. note::

       The default :ref:`configuration <CONFIG_{IDF_TARGET_CFG_PREFIX}_UNIVERSAL_MAC_ADDRESSES>`
       is 2 universally administered MAC addresses, and this is recommended when using
       Espressif-provided MAC addresses.

.. only:: not SOC_EMAC_SUPPORTED

   .. note:: {IDF_TARGET_NAME} has no integrated Ethernet MAC, but it's still possible to calculate an Ethernet MAC address. This MAC address can only be used with an external interface such as a SPI-Ethernet device, see :doc:`/api-reference/network/esp_eth`.

Custom Base MAC
^^^^^^^^^^^^^^^

The default Base MAC is pre-programmed by Espressif in eFuse {IDF_TARGET_BASE_MAC_BLOCK}. To set a custom Base MAC instead, call the function :cpp:func:`esp_base_mac_addr_set` before initializing any network interfaces or calling the :cpp:func:`esp_read_mac` function. The customized MAC address can be stored in any supported storage device (e.g. Flash, NVS, etc).

The custom base MAC addresses should be allocated such that derived MAC addresses will not overlap. Configure the option :ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_UNIVERSAL_MAC_ADDRESSES` to set the number of valid universal MAC addresses that can be derived from the custom base MAC, according to the table above.

.. note::

   It is also possible to call the function :cpp:func:`esp_netif_set_mac` to set the specific MAC used by a network interface, after network initialization. It's recommended to use the Base MAC approach documented here instead, to avoid the possibility of the original MAC address briefly appearing on the network before it is changed.


Custom MAC address in eFuse
@@@@@@@@@@@@@@@@@@@@@@@@@@@

When reading custom MAC addresses from eFuse, ESP-IDF provides a helper function :cpp:func:`esp_efuse_mac_get_custom`. This loads the MAC address from eFuse BLK3. This function assumes that the custom base MAC address is stored in the following format:

.. only:: esp32

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

    .. note::

        If the 3/4 coding scheme is enabled, all eFuse fields in this block must be burnt at the same time.

.. only:: not esp32

    +-----------------+-----------+---------------+
    | Field           | # of bits | Range of bits |
    +=================+===========+===============+
    | MAC address     | 48        | 200:248       |
    +-----------------+-----------+---------------+

    .. note::

        The eFuse BLK3 uses RS-coding during a burn operation it means that all eFuse fields in this block must be burnt at the same time.

Once MAC address has been obtained using :cpp:func:`esp_efuse_mac_get_custom`, call :cpp:func:`esp_base_mac_addr_set` to set this MAC address as base MAC address.



.. _local-mac-addresses:

Local vs Universal MAC addresses
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

{IDF_TARGET_NAME} comes pre-programmed with enough valid Espressif universally administered MAC addresses for all internal interfaces. The specific calculations to derive an interface's MAC address from the base MAC address is shown in the table above..

When using a custom MAC address scheme, it's possible that not all interfaces can be assigned a universally administered MAC address. In these cases, a locally administered MAC address is assigned. Note that these addresses are intended for use on a single local network, only.

See `this article <https://en.wikipedia.org/wiki/MAC_address#Universal_vs._local_(U/L_bit)>`_ for the definition of local and universally administered MAC addresses.

Function :cpp:func:`esp_derive_local_mac` is called internally to derive a local MAC address from a universal MAC address. The process is as follows:

1. The U/L bit (bit value 0x2) is set in the first octet of the universal MAC address, creating a local MAC address.
2. If this bit is already set in the supplied universal MAC address (meaning: the supplied "universal" MAC address was in fact already a local MAC address), then the first octet of the local MAC address is XORed with 0x4.

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


.. _app-version:

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

