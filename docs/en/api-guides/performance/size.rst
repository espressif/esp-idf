Minimizing Binary Size
======================

{IDF_TARGET_REDUCED_BY_IRAM: default="DRAM", esp32="IRAM and/or DRAM (depending on sizes)"}

The ESP-IDF build system compiles all source files in the project and ESP-IDF, but only functions and variables that are actually referenced by the program are linked into the final binary. In some cases, it is necessary to reduce the total size of the firmware binary (for example, in order to fit it into the available flash partition size).

The first step to reducing the total firmware binary size is measuring what is causing the size to increase.

.. _idf.py-size:

Measuring Static Sizes
----------------------

To optimize both firmware binary size and memory usage it's necessary to measure statically allocated RAM ("data", "bss"), code ("text") and read-only data ("rodata") in your project.

Using the :ref:`idf.py` sub-commands ``size``, ``size-components`` and ``size-files`` provides a summary of memory used by the project:

Size Summary (idf.py size)
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32

    .. code-block:: bash

        $ idf.py size
        [...]
            Total sizes:
         DRAM .data size:   14956 bytes
         DRAM .bss  size:   15808 bytes
        Used static DRAM:   30764 bytes ( 149972 available, 17.0% used)
        Used static IRAM:   83918 bytes (  47154 available, 64.0% used)
              Flash code:  559943 bytes
            Flash rodata:  176736 bytes
        Total image size:~ 835553 bytes (.bin may be padded larger)


.. only:: not esp32

    .. code-block:: bash

        $ idf.py size
        [...]
        Total sizes:
        DRAM .data size:   11584 bytes
        DRAM .bss  size:   19624 bytes
       Used static DRAM:       0 bytes (      0 available, nan% used)
       Used static IRAM:       0 bytes (      0 available, nan% used)
       Used stat D/IRAM:  136276 bytes ( 519084 available, 20.8% used)
             Flash code:  630508 bytes
           Flash rodata:  177048 bytes
       Total image size:~ 924208 bytes (.bin may be padded larger)

This output breaks down the size of all static memory regions in the firmware binary:

.. list::

    - ``DRAM .data size`` is statically allocated RAM that is assigned to non-zero values at startup. This uses RAM (DRAM) at runtime and also uses space in the binary file.
    - ``DRAM .bss size`` is statically allocated RAM that is assigned zero at startup. This uses RAM (DRAM) at runtime but doesn't use any space in the binary file.
    :esp32: - ``Used static DRAM`` is the total DRAM used by .data + .bss. The ``available`` size is the estimated amount of DRAM which will be available as heap memory at runtime (due to metadata overhead and implementation constraints, and heap allocations done by ESP-IDF during startup, the actual free heap at startup will be lower than this).
    :esp32: - ``Used static IRAM`` is the total size of executable code :ref:`executed from IRAM <iram>`. This uses space in the binary file and also reduces {IDF_TARGET_REDUCED_BY_IRAM} available as heap memory at runtime. See :ref:`optimize-iram-usage`.
    :not esp32: - ``Used static DRAM``, ``Used static IRAM`` - these options are kept for compatibility with ESP32 target, and currently read 0.
    :not esp32: - ``Used stat D/IRAM`` - This is total internal RAM usage, the sum of static DRAM .data + .bss, and also static :ref:`iram` used by the application for executable code. The ``available`` size is the estimated amount of DRAM which will be available as heap memory at runtime (due to metadata overhead and implementation constraints, and heap allocations done by ESP-IDF during startup, the actual free heap at startup will be lower than this).
    - ``Flash code`` is the total size of executable code executed from flash cache (:ref:`IROM <irom>`). This uses space in the binary file.
    - ``Flash rodata`` is the total size of read-only data loaded from flash cache (:ref:`DROM <drom>`). This uses space in the binary file.
    - ``Total image size`` is the estimated total binary file size, which is the total of all the used memory types except for .bss.

Component Usage Summary (idf.py size-components)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The summary output provided by ``idf.py size`` does not give enough detail to find the main contributor to excessive binary size. To analyze in more detail, use ``idf.py size-components``

.. code-block:: bash

    $ idf.py size-components
    [...]
        Total sizes:
     DRAM .data size:   14956 bytes
     DRAM .bss  size:   15808 bytes
    Used static DRAM:   30764 bytes ( 149972 available, 17.0% used)
    Used static IRAM:   83918 bytes (  47154 available, 64.0% used)
          Flash code:  559943 bytes
        Flash rodata:  176736 bytes
    Total image size:~ 835553 bytes (.bin may be padded larger)
    Per-archive contributions to ELF file:
                Archive File DRAM .data & .bss & other   IRAM   D/IRAM Flash code & rodata   Total
               libnet80211.a       1267   6044       0   5490        0     107445    18484  138730
                   liblwip.a         21   3838       0      0        0      97465    16116  117440
                libmbedtls.a         60    524       0      0        0      27655    69907   98146
             libmbedcrypto.a         64     81       0     30        0      76645    11661   88481
                     libpp.a       2427   1292       0  20851        0      37208     4708   66486
                      libc.a          4      0       0      0        0      57056     6455   63515
                    libphy.a       1439    715       0   7798        0      33074        0   43026
         libwpa_supplicant.a         12    848       0      0        0      35505     1446   37811
               libfreertos.a       3104    740       0  15711        0        367     4228   24150
              libnvs_flash.a          0     24       0      0        0      14347     2924   17295
              libspi_flash.a       1562    294       0   8851        0       1840     1913   14460
             libesp_system.a        245    206       0   3078        0       5990     3817   13336
                libesp-tls.a          0      4       0      0        0       5637     3524    9165
    [... removed some lines here ...]
          libtcpip_adapter.a          0     17       0      0        0        216        0     233
                libesp_rom.a          0      0       0    112        0          0        0     112
                    libcxx.a          0      0       0      0        0         47        0      47
                       (exe)          0      0       0      3        0          3       12      18
                 libesp_pm.a          0      0       0      0        0          8        0       8
                libesp_eth.a          0      0       0      0        0          0        0       0
                   libmesh.a          0      0       0      0        0          0        0       0

The first lines of output from ``idf.py size-components`` are the same as ``idf.py size``. After this a table is printed of "per-archive contributions to ELF file". This means how much each static library archive has contributed to the final binary size.

Generally, one static library archive is built per component, although some are binary libraries included by a particular component (for example, ``libnet80211.a`` is included by ``esp_wifi`` component). There are also toolchain libraries such as ``libc.a`` and ``libgcc.a`` listed here, these provide Standard C/C++ Library and toolchain built-in functionality.

If your project is simple and only has a "main" component, then all of the project's code will be shown under ``libmain.a``. If your project includes its own components (see :doc:`/api-guides/build-system`), then they will each be shown on a separate line.

The table is sorted in descending order of the total contribution to the binary size.

The columns are as follows:

.. list::

    - ``DRAM .data & .bss & other`` - .data and .bss are the same as for the totals shown above (static variables, these both reduce total available RAM at runtime but .bss doesn't contribute to the binary file size). "other" is a column for any custom section types that also contribute to RAM size (usually this value is 0).
    :esp32: - ``IRAM`` - is the same as for the totals shown above (code linked to execute from IRAM, uses space in the binary file and also reduces IRAM that can be dynamically allocated at runtime using ``HEAP_CAP_32BIT``.
    :esp32: - ``D/IRAM`` - Shows IRAM space which, due to occupying D/IRAM space, is also reducing available DRAM available as heap at runtime.
    :not esp32: - ``IRAM`` - is the same as for the totals shown above (code linked to execute from IRAM, uses space in the binary file and also reduces DRAM available as heap at runtime.
    - ``Flash code & rodata`` - these are the same as the totals above, IROM and DROM space accessed from flash cache that contribute to the binary size.

Source File Usage Summary (idf.py size-files)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For even more detail, run ``idf.py size-files`` to get a summary of the contribution each object file has made to the final binary size. Each object file corresponds to a single source file.

.. code-block:: bash

    $ idf.py size-files
    [...]
    Total sizes:
     DRAM .data size:   14956 bytes
     DRAM .bss  size:   15808 bytes
    Used static DRAM:   30764 bytes ( 149972 available, 17.0% used)
    Used static IRAM:   83918 bytes (  47154 available, 64.0% used)
          Flash code:  559943 bytes
        Flash rodata:  176736 bytes
    Total image size:~ 835553 bytes (.bin may be padded larger)
    Per-file contributions to ELF file:
                 Object File DRAM .data & .bss & other   IRAM   D/IRAM Flash code & rodata   Total
         x509_crt_bundle.S.o          0      0       0      0        0          0    64212   64212
                    wl_cnx.o          2   3183       0    221        0      13119     3286   19811
               phy_chip_v7.o        721    614       0   1642        0      16820        0   19797
           ieee80211_ioctl.o        740     96       0    437        0      15325     2627   19225
                        pp.o       1142     45       0   8871        0       5030      537   15625
          ieee80211_output.o          2     20       0   2118        0      11617      914   14671
             ieee80211_sta.o          1     41       0   1498        0      10858     2218   14616
            lib_a-vfprintf.o          0      0       0      0        0      13829      752   14581
           lib_a-svfprintf.o          0      0       0      0        0      13251      752   14003
                 ssl_tls.c.o         60      0       0      0        0      12769      463   13292
                 sockets.c.o          0    648       0      0        0      11096     1030   12774
                     nd6.c.o          8    932       0      0        0      11515      314   12769
           phy_chip_v7_cal.o        477     53       0   3499        0       8561        0   12590
                        pm.o         32    364       0   2673        0       7788      782   11639
            ieee80211_scan.o         18    288       0      0        0       8889     1921   11116
          lib_a-svfiprintf.o          0      0       0      0        0       9654     1206   10860
           lib_a-vfiprintf.o          0      0       0      0        0      10069      734   10803
              ieee80211_ht.o          0      4       0   1186        0       8628      898   10716
           phy_chip_v7_ana.o        241     48       0   2657        0       7677        0   10623
                  bignum.c.o          0      4       0      0        0       9652      752   10408
                  tcp_in.c.o          0     52       0      0        0       8750     1282   10084
                       trc.o        664     88       0   1726        0       6245     1108    9831
                   tasks.c.o          8    704       0   7594        0          0     1475    9781
              ecp_curves.c.o         28      0       0      0        0       7384     2325    9737
                     ecp.c.o          0     64       0      0        0       8864      286    9214
          ieee80211_hostap.o          1     41       0      0        0       8578      585    9205
                      wdev.o        121    125       0   4499        0       3684      580    9009
                 tcp_out.c.o          0      0       0      0        0       5686     2161    7847
                     tcp.c.o          2     26       0      0        0       6161     1617    7806
           ieee80211_input.o          0      0       0      0        0       6797      973    7770
                     wpa.c.o          0    656       0      0        0       6828       55    7539
    [... additional lines removed ...]

After the summary of total sizes, a table of "Per-file contributions to ELF file" is printed.

The columns are the same as shown above for ``idy.py size-components``, but this time the granularity is the contribution of each individual object file to the binary size.

For example, we can see that the file ``x509_crt_bundle.S.o`` contributed 64212 bytes to the total firmware size, all as ``.rodata`` in flash. Therefore we can guess that this application is using the :doc:`/api-reference/protocols/esp_crt_bundle` feature and not using this feature would save at last this many bytes from the firmware size.

Some of the object files are linked from binary libraries and therefore you won't find a corresponding source file. To locate which component a source file belongs to, it's generally possible to search in the ESP-IDF source tree or look in the :ref:`linker-map-file`  for the full path.

Comparing Two Binaries
^^^^^^^^^^^^^^^^^^^^^^

If making some changes that affect binary size, it's possible to use an ESP-IDF tool to break down the exact differences in size.

This operation isn't part of ``idf.py``, it's necessary to run the ``idf-size.py`` Python tool directly.

To do so, first locate the linker map file in the build directory. It will have the name ``PROJECTNAME.map``. The ``idf-size.py`` tool performs its analysis based on the output of the linker map file.

To compare with another binary, you will also need its corresponding ``.map`` file saved from the build directory.

For example, to compare two builds: one with the default :ref:`CONFIG_COMPILER_OPTIMIZATION` setting "Debug (-Og)" configuration and one with "Optimize for size (-Os)":

.. code-block:: bash

    $ $IDF_PATH/tools/idf_size.py --diff build_Og/https_request.map build_Os/https_request.map
    <CURRENT> MAP file: build_Os/https_request.map
    <REFERENCE> MAP file: build_Og/https_request.map
    Difference is counted as <CURRENT> - <REFERENCE>, i.e. a positive number means that <CURRENT> is larger.
    Total sizes of <CURRENT>:                                                 <REFERENCE>     Difference
     DRAM .data size:   14516 bytes                                                 14956           -440
     DRAM .bss  size:   15792 bytes                                                 15808            -16
    Used static DRAM:   30308 bytes ( 150428 available, 16.8% used)                 30764           -456 (   +456 available,      +0 total)
    Used static IRAM:   78498 bytes (  52574 available, 59.9% used)                 83918          -5420 (  +5420 available,      +0 total)
          Flash code:  509183 bytes                                                559943         -50760
        Flash rodata:  170592 bytes                                                176736          -6144
    Total image size:~ 772789 bytes (.bin may be padded larger)                    835553         -62764

We can see from the "Difference" column that changing this one setting caused the whole binary to be over 60 KB smaller and over 5 KB more RAM is available.

It's also possible to use the "diff" mode to output a table of component-level (static library archive) differences:

.. code-block:: bash

    $IDF_PATH/tools/idf_size.py --archives --diff build_Og/https_request.map build_Oshttps_request.map

Also at the individual source file level:

.. code-block:: bash

    $IDF_PATH/tools/idf_size.py --files --diff build_Og/https_request.map build_Oshttps_request.map

Other options (like writing the output to a file) are available, pass ``--help`` to see the full list.

.. _idf-size-linker-failed:

Showing Size When Linker Fails
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If too much static memory is used, then the linker will fail with an error such as ``DRAM segment data does not fit``, ``region `iram0_0_seg' overflowed by 44 bytes``, or similar.

In these cases, ``idf.py size`` will not succeed either. However it is possible to run ``idf_size.py`` manually in order to view the *partial static memory usage* (the memory usage will miss the variables which could not be linked, so there still appears to be some free space.)

The map file argument is ``<projectname>.map`` in the build directory

.. code-block:: bash

    $IDF_PATH/tools/idf_size.py build/project_name.map

It is also possible to view the equivalent of ``size-components`` or ``size-files`` output:

.. code-block:: bash

    $IDF_PATH/tools/idf_size.py --archives build/project_name.map
    $IDF_PATH/tools/idf_size.py --files build/project_name.map

.. _linker-map-file:

Linker Map File
^^^^^^^^^^^^^^^

*This is an advanced analysis method, but it can be very useful. Feel free to skip ahead to :ref:`reducing-overall-size` and possibly come back to this later.*

The ``idf.py size`` analysis tools all work by parsing the GNU binutils "linker map file", which is a summary of everything the linker did when it created ("linked") the final firmware binary file

Linker map files themselves are plain text files, so it's possible to read them and find out exactly what the linker did. However, they are also very complex and long - often 100,000 or more lines!

The map file itself is broken into parts and each part has a heading. The parts are:

- ``Archive member included to satisfy reference by file (symbol)``. This shows you: for each object file included in the link, what symbol (function or variable) was the linker searching for when it included that object file. If you're wondering why some object file in particular was included in the binary, this part may give a clue. This part can be used in conjunction with the ``Cross Reference Table`` at the end of the file. Note that not every object file shown in this list ends up included in the final binary, some end up in the ``Discarded input sections`` list instead.
- ``Allocating common symbols`` - This is a list of (some) global variables along with their sizes. Common symbols have a particular meaning in ELF binary files, but ESP-IDF doesn't make much use of them.
- ``Discarded input sections`` - These sections were read by the linker as part of an object file to be linked into the final binary, but then nothing else referred to them so they were discarded from the final binary. For ESP-IDF this list can be very long, as we compile each function and static variable to a unique section in order to minimize the final binary size (specifically ESP-IDF uses compiler options ``-ffunction-sections -fdata-sections`` and linker option ``--gc-sections``). Items mentioned in this list *do not* contribute to the final binary.
- ``Memory Configuration``, ``Linker script and memory map`` These two parts go together. Some of the output comes directly from the linker command line and the Linker Script, both provided by the :doc:`/api-guides/build-system`. The  linker script is partially generated from the ESP-IDF project using the :doc:`/api-guides/linker-script-generation` feature.

  As the output of the ``Linker script and memory map`` part of the map unfolds, you can see each symbol (function or static variable) linked into the final binary along with its address (as a 16 digit hex number), its length (also in hex), and the library and object file it was linked from (which can be used to determine the component and the source file).

  Following all of the output sections that take up space in the final ``.bin`` file, the ``memory map`` also includes some sections in the ELF file that are only used for debugging (ELF sections ``.debug_*``, etc.). These don't contribute to the final binary size. You'll notice the address of these symbols is a very low number (starting from 0x0000000000000000 and counting up).
- ``Cross Reference Table``. This table shows for each symbol (function or static variable), the list of object file(s) that referred to it. If you're wondering why a particular thing is included in the binary, this will help determine what included it.

  .. note:: Unfortunately, the ``Cross Reference Table`` doesn't only include symbols that made it into the final binary. It also includes symbols in discarded sections. Therefore, just because something is shown here doesn't mean that it was included in the final binary - this needs to be checked separately.

.. note::

   Linker map files are generated by the GNU binutils linker "ld", not ESP-IDF. You can find additional information online about the linker map file format. This quick summary is written from the perspective of ESP-IDF build system in particular.

.. _reducing-overall-size:

Reducing Overall Size
---------------------

The following configuration options will reduce the final binary size of almost any ESP-IDF project:

.. list::

    - Set :ref:`CONFIG_COMPILER_OPTIMIZATION` to "Optimize for size (-Os)". In some cases, "Optimize for performance (-O2)" will also reduce the binary size compared to the default. Note that if your code contains C or C++ Undefined Behaviour then increasing the compiler optimization level may expose bugs that otherwise don't happen.
    - Reduce the compiled-in log output by lowering the app :ref:`CONFIG_LOG_DEFAULT_LEVEL`. If the :ref:`CONFIG_LOG_MAXIMUM_LEVEL` is changed from the default then this setting controls the binary size instead. Reducing compiled-in logging reduces the number of strings in the binary, and also the code size of the calls to logging functions.
    - Set the :ref:`CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL` to "Silent". This avoids compiling in a dedicated assertion string and source file name for each assert that may fail. It's still possible to find the failed assert in the code by looking at the memory address where the assertion failed.
    - Besides the :ref:`CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL`, you can disable or silent the assertion for HAL component separately by setting :ref:`CONFIG_HAL_DEFAULT_ASSERTION_LEVEL`.
    - Set :ref:`CONFIG_COMPILER_OPTIMIZATION_CHECKS_SILENT`. This removes specific error messages for particular internal ESP-IDF error check macros. This may make it harder to debug some error conditions by reading the log output.
    :esp32: - If the binary needs to run on only certain revision(s) of ESP32, increasing :ref:`CONFIG_ESP32_REV_MIN` to match can result in a reduced binary size. This will make a large difference if setting ESP32 minimum revision 3, and PSRAM is enabled.
    :esp32c3: - If the binary needs to run on only certain revision(s) of ESP32-C3, increasing :ref:`CONFIG_ESP32C3_REV_MIN` to match can result in a reduced binary size. This is particularly true if setting ESP32-C3 minimum revision 3 and using Wi-Fi, as some functionality was moved to ROM code.
    - Don't enable :ref:`CONFIG_COMPILER_CXX_EXCEPTIONS`, :ref:`CONFIG_COMPILER_CXX_RTTI`, or set the :ref:`CONFIG_COMPILER_STACK_CHECK_MODE` to Overall. All of these options are already disabled by default, but they have a large impact on binary size.
    - Disabling :ref:`CONFIG_ESP_ERR_TO_NAME_LOOKUP` will remove the lookup table to translate user-friendly names for error values (see :doc:`/api-guides/error-handling`) in error logs, etc. This saves some binary size, but error values will be printed as integers only.
    - Setting :ref:`CONFIG_ESP_SYSTEM_PANIC` to "Silent reboot" will save a small amount of binary size, however this is *only* recommended if no one will use UART output to debug the device.

.. note::

   In addition to the many configuration items shown here, there are a number of configuration options where changing the option from the default will increase binary size. These are not noted here. Where the increase is significant, this is usually noted in the configuration item help text.

.. _size-targeted-optimizations:

Targeted Optimizations
^^^^^^^^^^^^^^^^^^^^^^

The following binary size optimizations apply to a particular component or a function:

Wi-Fi
@@@@@

- Disabling :ref:`CONFIG_ESP32_WIFI_ENABLE_WPA3_SAE` will save some Wi-Fi binary size if WPA3 support is not needed. (Note that WPA3 is mandatory for new Wi-Fi device certifications.)

.. only:: esp32

    ADC
    @@@

    - Disabling ADC calibration features :ref:`CONFIG_ADC_CAL_EFUSE_TP_ENABLE`, :ref:`CONFIG_ADC_CAL_EFUSE_VREF_ENABLE`, :ref:`CONFIG_ADC_CAL_LUT_ENABLE` will save a small amount of binary size if ADC driver is used, at expense of accuracy.

.. only:: SOC_BT_SUPPORTED

    Bluetooth NimBLE
    @@@@@@@@@@@@@@@@

    If using :doc:`NimBLE Bluetooth Host </api-reference/bluetooth/nimble/index>` then the following modifications can reduce binary size:

    .. list::

        :esp32: - Set :ref:`CONFIG_BTDM_CTRL_BLE_MAX_CONN` to 1 if only one BLE connection is needed.
        - :ref:`CONFIG_BT_NIMBLE_MAX_CONNECTIONS` to 1 if only one BLE connection is needed.
        - Disable either :ref:`CONFIG_BT_NIMBLE_ROLE_CENTRAL` or :ref:`CONFIG_BT_NIMBLE_ROLE_OBSERVER` if these roles are not needed.
        - Reducing :ref:`CONFIG_BT_NIMBLE_LOG_LEVEL` can reduce binary size. Note that if the overall log level has been reduced as described above in :ref:`reducing-overall-size` then this also reduces the NimBLE log level.

lwIP IPv6
@@@@@@@@@

- Setting :ref:`CONFIG_LWIP_IPV6` to false will reduce the size of the lwIP TCP/IP stack, at the cost of only supporting IPv4.

  .. note::

      IPv6 is required by some components such as ``coap`` and :doc:`/api-reference/protocols/asio`, These components will not be available if IPV6 is disabled.

.. _newlib-nano-formatting:

Newlib nano formatting
@@@@@@@@@@@@@@@@@@@@@@

By default, ESP-IDF uses newlib "full" formating for I/O (printf, scanf, etc.)

Enabling the config option :ref:`CONFIG_NEWLIB_NANO_FORMAT` will switch newlib to the "nano" formatting mode. This both smaller in code size and a large part of the implementation is compiled into the {IDF_TARGET_NAME} ROM, so it doesn't need to be included in the binary at all.

The exact difference in binary size depends on which features the firmware uses, but 25 KB ~ 50 KB is typical.

Enabling Nano formatting also reduces the stack usage of each function that calls printf() or another string formatting function, see :ref:`optimize-stack-sizes`.

"Nano" formatting doesn't support 64-bit integers, or C99 formatting features. For a full list of restrictions, search for ``--enable-newlib-nano-formatted-io`` in the `Newlib README file`_.

.. _Newlib README file: https://sourceware.org/newlib/README

mbedTLS features
@@@@@@@@@@@@@@@@

Under *Component Config* -> *mbedTLS* there are multiple mbedTLS features which are enabled by default but can be disabled if not needed to save code size.

These include:

- :ref:`CONFIG_MBEDTLS_HAVE_TIME`
- :ref:`CONFIG_MBEDTLS_ECDSA_DETERMINISTIC`
- :ref:`CONFIG_MBEDTLS_SHA512_C`
- :ref:`CONFIG_MBEDTLS_SSL_PROTO_TLS1`
- :ref:`CONFIG_MBEDTLS_SSL_PROTO_TLS1_1`
- :ref:`CONFIG_MBEDTLS_CLIENT_SSL_SESSION_TICKETS`
- :ref:`CONFIG_MBEDTLS_SERVER_SSL_SESSION_TICKETS`
- :ref:`CONFIG_MBEDTLS_SSL_ALPN`
- :ref:`CONFIG_MBEDTLS_CCM_C`
- :ref:`CONFIG_MBEDTLS_GCM_C`
- :ref:`CONFIG_MBEDTLS_ECP_C` (Alternatively: Leave this option enabled but disable some of the elliptic curves listed in the sub-menu.)
- :ref:`CONFIG_MBEDTLS_SSL_RENEGOTIATION`
- Change :ref:`CONFIG_MBEDTLS_TLS_MODE` if both Server & Client are not needed
- Consider disabling some ciphersuites listed in the "TLS Key Exchange Methods" sub-menu (i.e. :ref:`CONFIG_MBEDTLS_KEY_EXCHANGE_RSA`)

The help text for each option has some more information.

.. important::

   It is **strongly not recommended to disable all these mbedTLS options**. Only disable options where you understand the functionality and are certain that it is not needed in the application. In particular:

   - Ensure that any TLS server(s) the device connects to can still be used. If the server is controlled by a third party or a cloud service, recommend ensuring that the firmware supports at least two of the supported cipher suites in case one is disabled in a future update.
   - Ensure that any TLS client(s) that connect to the device can still connect with supported/recommended cipher suites. Note that future versions of client operating systems may remove support for some features, so it is recommended to enable multiple supported cipher suites or algorithms for redundancy.

   If depending on third party clients or servers, always pay attention to announcements about future changes to supported TLS features. If not, the {IDF_TARGET_NAME} device may become inaccessible if support changes.

.. note::

   Not every combination of mbedTLS compile-time config is tested in ESP-IDF. If you find a combination that fails to compile or function as expected, please report the details on GitHub.

FreeModBus
@@@@@@@@@@

If using Modbus, enable or disable :ref:`CONFIG_FMB_COMM_MODE_TCP_EN`, :ref:`CONFIG_FMB_COMM_MODE_RTU_EN`, :ref:`CONFIG_FMB_COMM_MODE_ASCII_EN` as applicable for the necessary functionality.

VFS
@@@

:doc:`Virtual filesystem </api-reference/storage/vfs>` feature in ESP-IDF allows multiple filesystem drivers and file-like peripheral drivers to be accessed using standard I/O functions (``open``, ``read``, ``write``, etc.) and C library functions (``fopen``, ``fread``, ``fwrite``, etc.). When filesystem or file-like peripheral driver functionality is not used in the application this feature can be fully or partially disabled. VFS component provides the following configuration options:

* :ref:`CONFIG_VFS_SUPPORT_TERMIOS` — can be disabled if the application doesn't use ``termios`` family of functions. Currently, these functions are implemented only for UART VFS driver. Most applications can disable this option. Disabling this option reduces the code size by about 1.8 kB.
* :ref:`CONFIG_VFS_SUPPORT_SELECT` — can be disabled if the application doesn't use ``select`` function with file descriptors. Currently, only the UART and eventfd VFS drivers implement ``select`` support. Note that when this option is disabled, ``select`` can still be used for socket file descriptors. Disabling this option reduces the code size by about 2.7 kB.
* :ref:`CONFIG_VFS_SUPPORT_DIR` — can be disabled if the application doesn't use directory related functions, such as ``readdir`` (see the description of this option for the complete list). Applications which only open, read and write specific files and don't need to enumerate or create directories can disable this option, reducing the code size by 0.5 kB or more, depending on the filesystem drivers in use.
* :ref:`CONFIG_VFS_SUPPORT_IO` — can be disabled if the application doesn't use filesystems or file-like peripheral drivers. This disables all VFS functionality, including the three options mentioned above. When this option is disabled, :doc:`console </api-reference/system/console>` can't be used. Note that the application can still use standard I/O functions with socket file descriptors when this option is disabled. Compared to the default configuration, disabling this option reduces code size by about 9.4 kB. 

Bootloader Size
---------------

This document deals with the size of an ESP-IDF app binary only, and not the ESP-IDF :ref:`second-stage-bootloader`.

For a discussion of ESP-IDF bootloader binary size, see :ref:`bootloader-size`.

IRAM Binary Size
----------------

If the IRAM section of a binary is too large, this issue can be resolved by reducing IRAM memory usage. See :ref:`optimize-iram-usage`.



