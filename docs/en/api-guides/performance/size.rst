Minimizing Binary Size
======================

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_REDUCED_BY_IRAM: default="DRAM", esp32="IRAM and/or DRAM (depending on sizes)"}

The ESP-IDF build system compiles all source files in the project and ESP-IDF, but only functions and variables that are actually referenced by the program are linked into the final binary. In some cases, it is necessary to reduce the total size of the firmware binary, e.g., in order to fit it into the available flash partition size.

The first step to reducing the total firmware binary size is measuring what is causing the size to increase.

.. _idf.py-size:

Measuring Static Sizes
----------------------

To optimize both the firmware binary size and the memory usage, it is necessary to measure statically-allocated RAM (``data``, ``bss``), code (``text``), and read-only data (``rodata``) in your project.

Using the :ref:`idf.py` sub-commands ``size``, ``size-components``, and ``size-files`` provides a summary of memory used by the project:

.. note::

    It is possible to add ``-DOUTPUT_FORMAT=csv`` or ``-DOUTPUT_FORMAT=json`` to get the output in CSV or JSON format.

Size Summary ``idf.py size``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32

    .. code-block:: bash

        $ idf.py size
        [...]
        Total sizes:
        Used static DRAM:   10608 bytes ( 170128 remain, 5.9% used)
              .data size:    8464 bytes
              .bss  size:    2144 bytes
        Used static IRAM:   48834 bytes (  82238 remain, 37.3% used)
              .text size:   47807 bytes
           .vectors size:    1027 bytes
        Used Flash size :  117391 bytes
                   .text:   80103 bytes
                 .rodata:   37032 bytes
        Total image size:  174689 bytes (.bin may be padded larger)


.. only:: not esp32

    .. code-block:: bash

        $ idf.py size
        [...]
        Total sizes:
        Used stat D/IRAM:   53743 bytes ( 122385 remain, 30.5% used)
              .data size:    6504 bytes
              .bss  size:    1984 bytes
              .text size:   44228 bytes
           .vectors size:    1027 bytes
        Used Flash size :  118879 bytes
                   .text:   83467 bytes
                 .rodata:   35156 bytes
        Total image size:  170638 bytes (.bin may be padded larger)

This output breaks down the size of all static memory regions in the firmware binary:

.. only:: esp32

    .. code-block:: bash

        $ idf.py size
        [...]
        Total sizes:
        Used static DRAM:   10608 bytes ( 170128 remain, 5.9% used)
              .data size:    8464 bytes
              .bss  size:    2144 bytes
        Used static IRAM:   48834 bytes (  82238 remain, 37.3% used)
              .text size:   47807 bytes
           .vectors size:    1027 bytes
        Used Flash size :  117391 bytes
                   .text:   80103 bytes
                 .rodata:   37032 bytes
        Total image size:  174689 bytes (.bin may be padded larger)

    - ``Used static DRAM``: Total amount of DRAM allocated at compile time. ``remain`` indicates the amount of DRAM left to be used as heap memory at runtime. Note that due to meta data overhead, implementation constraints, and startup heap allocations, the actual size of the DRAM heap is smaller.

        - ``.data size``: Amount of DRAM allocated at compile time for the ``.data`` (i.e., all statically allocated variables that are initialized to non-zero values). ``.data`` also consumes space in the binary image to store the non-zero initialization values.
        - ``.bss  size``: Amount of DRAM allocated at compile time for ``.bss``  (i.e., all statically allocated variables that are initialized to zero). ``.bss`` does not consume extra space in flash.

    - ``Used static IRAM``: Total amount of IRAM allocated at compile time. ``remain`` indicates the amount of IRAM left to be used as heap memory at runtime. Note that due to meta data overhead, implementation constraints, and startup heap allocations, the actual size of the IRAM heap is smaller.

        - ``.text size``: Amount of IRAM used for ``.text`` (i.e., all code that is executed from :ref:`IRAM <iram>`). ``.text`` also consumes space in the binary image as the code is initially stored there and is then copied over to IRAM on startup.

    - ``Used Flash size``: Total amount of flash used (excluding usage by DRAM and IRAM)

        - ``.text``: Amount of flash used for ``.text`` (i.e., all code that is executed via the flash cache, see :ref:`IROM <irom>`).
        - ``.rodata``: Amount of flash used for ``.rodata`` (i.e., read-only data that is loaded via the flash cache, see :ref:`DROM <drom>`).

    - ``Total image size`` is the estimated total size of the binary file.

.. only:: not esp32

    .. code-block:: bash

        $ idf.py size
        [...]
        Total sizes:
        Used stat D/IRAM:   53743 bytes ( 122385 remain, 30.5% used)
              .data size:    6504 bytes
              .bss  size:    1984 bytes
              .text size:   44228 bytes
           .vectors size:    1027 bytes
        Used Flash size :  118879 bytes
                   .text:   83467 bytes
                 .rodata:   35156 bytes
        Total image size:  170638 bytes (.bin may be padded larger)

    - ``Used stat D/IRAM``: Total amount of D/IRAM used at compile time. ``remain`` indicates the amount of D/IRAM left to be used as heap memory at runtime. Note that due to meta data overhead, implementation constraints, and startup heap allocations, the actual size of the DRAM heap is smaller.

        - ``.data size``: Amount of D/IRAM allocated at compile time for the ``.data`` (i.e., all statically allocated variables that are initialized to non-zero values). ``.data`` also consumes space in the binary image to store the non-zero initialization values.
        - ``.bss  size``: Amount of D/IRAM allocated at compile time for ``.bss``  (i.e., all statically allocated variables that are initialized to zero). ``.bss`` does not consume extra space in flash.
        - ``.text size``: Amount of D/IRAM used for ``.text`` (i.e., all code that is executed from internal RAM). ``.text`` also consumes space in the binary image as the code is initially stored there and is then copied over to D/IRAM on startup.

    - ``Used Flash size``: Total amount of flash used (excluding usage by D/IRAM)

        - ``.text``: Amount of flash used for ``.text`` (i.e., all code that is executed via the flash cache, see :ref:`IROM <irom>`).
        - ``.rodata``: Amount of flash used for ``.rodata`` (i.e., read-only data that is loaded via the flash cache, see :ref:`DROM <drom>`).

    - ``Total image size`` is the estimated total size of the binary file.


Component Usage Summary ``idf.py size-components``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The summary output provided by ``idf.py size`` does not give enough details to find the main contributor to excessive binary size. To analyze in detail, use ``idf.py size-components``.

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
                libesp_rom.a          0      0       0    112        0          0        0     112
                    libcxx.a          0      0       0      0        0         47        0      47
                       (exe)          0      0       0      3        0          3       12      18
                 libesp_pm.a          0      0       0      0        0          8        0       8
                libesp_eth.a          0      0       0      0        0          0        0       0
                   libmesh.a          0      0       0      0        0          0        0       0

The first lines of the output from ``idf.py size-components`` are the same as that from ``idf.py size``. After this, a table is printed as ``Per-archive contributions to ELF file``. This means how much each static library archive has contributed to the final binary size.

Generally, one static library archive is built per component, although some are binary libraries included by a particular component, for example, ``libnet80211.a`` is included by ``esp_wifi`` component. There are also toolchain libraries such as ``libc.a`` and ``libgcc.a`` listed here, these provide Standard C/C++ Library and toolchain built-in functionality.

If your project is simple and only has a ``main`` component, then all of the project's code will be shown under ``libmain.a``. If your project includes its own components (see :doc:`/api-guides/build-system`), then they will each be shown on a separate line.

The table is sorted in descending order of the total contribution of the static archive to the binary size.

The columns are as follows:

.. list::

    - ``DRAM .data & .bss & other`` - ``.data`` and ``.bss`` are the same as for the totals shown above. Both are static variables and reduce the total available RAM at runtime, but ``.bss`` does not contribute to the binary file size. ``other`` is a column for any custom section types that also contribute to RAM size. Usually, the value is 0.
    :esp32: - ``IRAM`` - is the same as for the totals shown above. It refers to code linked to execute from IRAM, which uses space in the binary file and also reduces IRAM that can be dynamically allocated at runtime using ``HEAP_CAP_32BIT``.
    :esp32: - ``D/IRAM`` - shows IRAM space which, due to occupying D/IRAM space, is also reducing available DRAM available as heap at runtime.
    :not esp32: - ``IRAM`` - is the same as for the totals shown above. It refers to code linked to execute from IRAM, which uses space in the binary file and also reduces DRAM available as heap at runtime.
    - ``Flash code & rodata`` - these are the same as the totals above, IROM and DROM space accessed from the flash cache that contribute to the binary size.

Source File Usage Summary ``idf.py size-files``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For even more details, run ``idf.py size-files`` to get a summary of the contribution each object file has made to the final binary size. Each object file corresponds to a single source file.

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

After the summary of total sizes, a table of ``Per-file contributions to ELF file`` is printed.

The columns are the same as shown above for ``idy.py size-components``, but this time the granularity is the contribution of each individual object file to the binary size.

For example, we can see that the file ``x509_crt_bundle.S.o`` contributed 64,212 bytes to the total firmware size, all as ``.rodata`` in flash. Therefore we can guess that this application is using the :doc:`/api-reference/protocols/esp_crt_bundle` feature and not using this feature would save at last this many bytes from the firmware size.

Some of the object files are linked from binary libraries and therefore you will not find a corresponding source file. To locate which component a source file belongs to, it is generally possible to search in the ESP-IDF source tree or look in the :ref:`linker-map-file`  for the full path.

Comparing Two Binaries
^^^^^^^^^^^^^^^^^^^^^^

If making some changes that affect binary size, it is possible to use an ESP-IDF tool to break down the exact differences in size.

This operation is not part of ``idf.py``, it is necessary to run the `esp_idf_size <https://github.com/espressif/esp-idf-size>`_ Python tool directly.

To do so, first, locate the linker map file with the name ``PROJECTNAME.map`` in the build directory. The ``esp_idf_size`` tool performs its analysis based on the output of the linker map file.

To compare with another binary, you also need its corresponding ``.map`` file saved from the build directory.

For example, to compare two builds, one of which with the default :ref:`CONFIG_COMPILER_OPTIMIZATION` setting ``Debug (-Og)`` configuration while another with ``Optimize for size (-Os)``:

.. code-block:: bash

    $ python -m esp_idf_size --diff build_Og/https_request.map build_Os/https_request.map
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

We can see from the ``Difference`` column that changing this one setting caused the whole binary to be over 60 KB smaller and over 5 KB more RAM is available.

It is also possible to use the ``diff`` mode to output a table of component-level (static library archive) differences:

.. note::

    To get the output in JSON or CSV format using ``esp_idf_size``, it is possible to use the ``--format`` option.

.. code-block:: bash

    python -m esp_idf_size --archives --diff build_Og/https_request.map build_Oshttps_request.map

Also at the individual source file level:

.. code-block:: bash

    python -m esp_idf_size --files --diff build_Og/https_request.map build_Oshttps_request.map

Other options, like writing the output to a file, are available, pass ``--help`` to see the full list.

.. _idf-size-linker-failed:

Showing Size When Linker Fails
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If too much static memory is allocated, the linker will fail with an error such as ``DRAM segment data does not fit``, ``region `iram0_0_seg' overflowed by 44 bytes``, or similar.

In these cases, ``idf.py size`` will not succeed either. However, it is possible to run ``esp_idf_size`` manually to view the **partial static memory usage**. The memory usage will miss the variables that could not be linked, so there still appears to be some free space.

The map file argument is ``<projectname>.map`` in the build directory.

.. code-block:: bash

    python -m esp_idf_size build/project_name.map

It is also possible to view the equivalent of ``size-components`` or ``size-files`` output:

.. code-block:: bash

    python -m esp_idf_size --archives build/project_name.map
    python -m esp_idf_size --files build/project_name.map

.. _linker-map-file:

Linker Map File
^^^^^^^^^^^^^^^

.. note::

    This is an advanced analysis method, but it can be very useful. Feel free to skip ahead to :ref:`reducing-overall-size` and possibly come back to this later.

The ``idf.py size`` analysis tools all work by parsing the GNU binutils ``linker map file``, which is a summary of everything the linker did when it created (i.e., linked) the final firmware binary file.

Linker map files themselves are plain text files, so it is possible to read them and find out exactly what the linker did. However, they are also very complex and long, often exceeding 100,000 lines.

The map file itself is broken into parts and each part has a heading. The parts are:

- ``Archive member included to satisfy reference by file (symbol)``

    - This shows you: for each object file included in the link, what symbol (function or variable) was the linker searching for when it included that object file.
    - If you are wondering why some object file in particular was included in the binary, this part may give a clue. This part can be used in conjunction with the ``Cross Reference Table`` at the end of the file.

    .. note::

        Not every object file shown in this list ends up included in the final binary, some end up in the ``Discarded input sections`` list instead.

- ``Allocating common symbols``

    - This is a list of some global variables along with their sizes. Common symbols have a particular meaning in ELF binary files, but ESP-IDF does not make much use of them.

- ``Discarded input sections``

    - These sections were read by the linker as part of an object file to be linked into the final binary, but then nothing else referred to them, so they were discarded from the final binary.
    - For ESP-IDF, this list can be very long, as we compile each function and static variable to a unique section in order to minimize the final binary size. Specifically, ESP-IDF uses compiler options ``-ffunction-sections -fdata-sections`` and linker option ``--gc-sections``.
    - Items mentioned in this list **do not** contribute to the final binary.

- ``Memory Configuration``, ``Linker script and memory map``

    - These two parts go together. Some of the output comes directly from the linker command line and the Linker Script, both provided by :doc:`/api-guides/build-system`. The linker script is partially generated from the ESP-IDF project using the :doc:`/api-guides/linker-script-generation` feature.

    - As the output of the ``Linker script and memory map`` part of the map unfolds, you can see each symbol (function or static variable) linked into the final binary along with its address (as a 16 digit hex number), its length (also in hex), and the library and object file it was linked from (which can be used to determine the component and the source file).

    - Following all of the output sections that take up space in the final ``.bin`` file, the ``memory map`` also includes some sections in the ELF file that are only used for debugging, e.g., ELF sections ``.debug_*``, etc. These do not contribute to the final binary size. You can notice the address of these symbols is a very small number, starting from ``0x0000000000000000`` and counting up.

- ``Cross Reference Table``

    - This table shows the symbol (function or static variable) that the list of object file(s) refers to. If you are wondering why a particular thing is included in the binary, this will help determine what included it.

    .. note::

        Unfortunately, the ``Cross Reference Table`` does not only include symbols that made it into the final binary. It also includes symbols in discarded sections. Therefore, just because something is shown here does not mean that it was included in the final binary - this needs to be checked separately.


.. note::

   Linker map files are generated by the GNU binutils linker ``ld``, not ESP-IDF. You can find additional information online about the linker map file format. This quick summary is written from the perspective of ESP-IDF build system in particular.

.. _reducing-overall-size:

Reducing Overall Size
---------------------

The following configuration options reduces the final binary size of almost any ESP-IDF project:

.. list::

    - Set :ref:`CONFIG_COMPILER_OPTIMIZATION` to ``Optimize for size (-Os)``. In some cases, ``Optimize for performance (-O2)`` will also reduce the binary size compared to the default. Note that if your code contains C or C++ Undefined Behavior then increasing the compiler optimization level may expose bugs that otherwise do not happen.
    - Reduce the compiled-in log output by lowering the app :ref:`CONFIG_LOG_DEFAULT_LEVEL`. If the :ref:`CONFIG_LOG_MAXIMUM_LEVEL` is changed from the default then this setting controls the binary size instead. Reducing compiled-in logging reduces the number of strings in the binary, and also the code size of the calls to logging functions.
    - Set the :ref:`CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL` to ``Silent``. This avoids compiling in a dedicated assertion string and source file name for each assert that may fail. It is still possible to find the failed assert in the code by looking at the memory address where the assertion failed.
    - Besides the :ref:`CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL`, you can disable or silent the assertion for the HAL component separately by setting :ref:`CONFIG_HAL_DEFAULT_ASSERTION_LEVEL`. It is to notice that ESP-IDF lowers the HAL assertion level in bootloader to be silent even if :ref:`CONFIG_HAL_DEFAULT_ASSERTION_LEVEL` is set to full-assertion level. This is to reduce the bootloader size.
    - Setting :ref:`CONFIG_COMPILER_OPTIMIZATION_CHECKS_SILENT` removes specific error messages for particular internal ESP-IDF error check macros. This may make it harder to debug some error conditions by reading the log output.
    :esp32: - If the binary needs to run on only certain revision(s) of ESP32, increasing :ref:`CONFIG_ESP32_REV_MIN` to match can result in a reduced binary size. This will make a large difference if setting ESP32 minimum revision 3, and PSRAM is enabled.
    :esp32c3: - If the binary needs to run on only certain revision(s) of ESP32-C3, increasing :ref:`CONFIG_ESP32C3_REV_MIN` to match can result in a reduced binary size. This is particularly true if setting ESP32-C3 minimum revision 3 and using Wi-Fi, as some functionality was moved to ROM code.
    - Do not enable :ref:`CONFIG_COMPILER_CXX_EXCEPTIONS`, :ref:`CONFIG_COMPILER_CXX_RTTI`, or set the :ref:`CONFIG_COMPILER_STACK_CHECK_MODE` to Overall. All of these options are already disabled by default, but they have a large impact on binary size.
    - Disabling :ref:`CONFIG_ESP_ERR_TO_NAME_LOOKUP` removes the lookup table to translate user-friendly names for error values (see :doc:`/api-guides/error-handling`) in error logs, etc. This saves some binary size, but error values will be printed as integers only.
    - Setting :ref:`CONFIG_ESP_SYSTEM_PANIC` to ``Silent reboot`` saves a small amount of binary size, however this is **only** recommended if no one will use UART output to debug the device.
    :CONFIG_IDF_TARGET_ARCH_RISCV: - Setting :ref:`CONFIG_COMPILER_SAVE_RESTORE_LIBCALLS` reduces binary size by replacing inlined prologues/epilogues with library calls.
    - If the application binary uses only one of the security versions of the protocomm component, then the support for others can be disabled to save some code size. The support can be disabled through :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0`, :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1` or :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_2` respectively.

.. note::

   In addition to the many configuration items shown here, there are a number of configuration options where changing the option from the default increases binary size. These are not noted here. Where the increase is significant is usually noted in the configuration item help text.

.. _size-targeted-optimizations:

Targeted Optimizations
^^^^^^^^^^^^^^^^^^^^^^

The following binary size optimizations apply to a particular component or a function:

.. only:: SOC_WIFI_SUPPORTED

    Wi-Fi
    @@@@@

    - Disabling :ref:`CONFIG_ESP_WIFI_ENABLE_WPA3_SAE` will save some Wi-Fi binary size if WPA3 support is not needed. Note that WPA3 is mandatory for new Wi-Fi device certifications.
    - Disabling :ref:`CONFIG_ESP_WIFI_SOFTAP_SUPPORT` will save some Wi-Fi binary size if soft-AP support is not needed.
    - Disabling :ref:`CONFIG_ESP_WIFI_ENTERPRISE_SUPPORT` will save some Wi-Fi binary size if enterprise support is not needed.

.. only:: esp32

    ADC
    @@@

    - Disabling ADC calibration features :ref:`CONFIG_ADC_CAL_EFUSE_TP_ENABLE`, :ref:`CONFIG_ADC_CAL_EFUSE_VREF_ENABLE`, :ref:`CONFIG_ADC_CAL_LUT_ENABLE` will save a small amount of binary size if ADC driver is used, at expense of accuracy.

.. only:: SOC_BT_SUPPORTED

    Bluetooth NimBLE
    @@@@@@@@@@@@@@@@

    If using :doc:`/api-reference/bluetooth/nimble/index` then the following modifications can reduce binary size:

    .. list::

        :esp32: - Set :ref:`CONFIG_BTDM_CTRL_BLE_MAX_CONN` to 1 if only one Bluetooth LE connection is needed.
        - Set :ref:`CONFIG_BT_NIMBLE_MAX_CONNECTIONS` to 1 if only one Bluetooth LE connection is needed.
        - Disable either :ref:`CONFIG_BT_NIMBLE_ROLE_CENTRAL` or :ref:`CONFIG_BT_NIMBLE_ROLE_OBSERVER` if these roles are not needed.
        - Reducing :ref:`CONFIG_BT_NIMBLE_LOG_LEVEL` can reduce binary size. Note that if the overall log level has been reduced as described above in :ref:`reducing-overall-size` then this also reduces the NimBLE log level.

lwIP IPv6
@@@@@@@@@

- Setting :ref:`CONFIG_LWIP_IPV6` to ``false`` will reduce the size of the lwIP TCP/IP stack, at the cost of only supporting IPv4.

  .. note::

      IPv6 is required by some components such as :doc:`/api-reference/protocols/asio`. These components will not be available if IPV6 is disabled.

lwIP IPv4
@@@@@@@@@

- If IPv4 connectivity is not required, setting :ref:`CONFIG_LWIP_IPV4` to ``false`` will reduce the size of the lwIP, supporting IPv6-only TCP/IP stack.

  .. note::

      Before disabling IPv4 support, please note that IPv6 only network environments are not ubiquitous and must be supported in the local network, e.g., by your internet service provider or using constrained local network settings.

.. _newlib-nano-formatting:

Newlib Nano Formatting
@@@@@@@@@@@@@@@@@@@@@@

By default, ESP-IDF uses Newlib "full" formatting for I/O functions (``printf()``, ``scanf()``, etc.)

.. only:: CONFIG_ESP_ROM_HAS_NEWLIB_NANO_FORMAT

    Enabling the config option :ref:`CONFIG_NEWLIB_NANO_FORMAT` will switch Newlib to the "Nano" formatting mode. This is smaller in code size, and a large part of the implementation is compiled into the {IDF_TARGET_NAME} ROM, so it does not need to be included in the binary at all.

    The exact difference in binary size depends on which features the firmware uses, but 25 KB ~ 50 KB is typical.

.. only:: CONFIG_ESP_ROM_HAS_NEWLIB_NORMAL_FORMAT

    Disabling the config option :ref:`CONFIG_NEWLIB_NANO_FORMAT` will switch Newlib to the "full" formatting mode. This will reduce the binary size, as {IDF_TARGET_NAME} has the full formatting version of the functions in ROM, so it does not need to be included in the binary at all.

Enabling "Nano" formatting reduces the stack usage of each function that calls ``printf()`` or another string formatting function, see :ref:`optimize-stack-sizes`.

"Nano" formatting does not support 64-bit integers, or C99 formatting features. For a full list of restrictions, search for ``--enable-newlib-nano-formatted-io`` in the `Newlib README file`_.


.. only:: esp32c2

    .. note::

        :ref:`CONFIG_NEWLIB_NANO_FORMAT` is enabled by default on {IDF_TARGET_NAME}.


.. _Newlib README file: https://sourceware.org/newlib/README

.. _minimizing_binary_mbedtls:

MbedTLS Features
@@@@@@@@@@@@@@@@

Under **Component Config** > **mbedTLS**, there are multiple mbedTLS features enabled default, some of which can be disabled if not needed to save code size.

These include:

- :ref:`CONFIG_MBEDTLS_HAVE_TIME`
- :ref:`CONFIG_MBEDTLS_ECDSA_DETERMINISTIC`
- :ref:`CONFIG_MBEDTLS_SHA512_C`
- :ref:`CONFIG_MBEDTLS_CLIENT_SSL_SESSION_TICKETS`
- :ref:`CONFIG_MBEDTLS_SERVER_SSL_SESSION_TICKETS`
- :ref:`CONFIG_MBEDTLS_SSL_CONTEXT_SERIALIZATION`
- :ref:`CONFIG_MBEDTLS_SSL_ALPN`
- :ref:`CONFIG_MBEDTLS_SSL_RENEGOTIATION`
- :ref:`CONFIG_MBEDTLS_CCM_C`
- :ref:`CONFIG_MBEDTLS_GCM_C`
- :ref:`CONFIG_MBEDTLS_ECP_C` (Alternatively: Leave this option enabled but disable some of the elliptic curves listed in the sub-menu.)
- :ref:`CONFIG_MBEDTLS_ECP_NIST_OPTIM`
- :ref:`CONFIG_MBEDTLS_ECP_FIXED_POINT_OPTIM`
- Change :ref:`CONFIG_MBEDTLS_TLS_MODE` if both server & client functionalities are not needed
- Consider disabling some cipher suites listed in the ``TLS Key Exchange Methods`` sub-menu (i.e., :ref:`CONFIG_MBEDTLS_KEY_EXCHANGE_RSA`)
- Consider disabling :ref:`CONFIG_MBEDTLS_ERROR_STRINGS` if the application is pulling in mbedTLS error strings because of :cpp:func:`mbedtls_strerror` usage

The help text for each option has some more information for reference.

.. important::

   It is **strongly not recommended to disable all these mbedTLS options**. Only disable options of which you understand the functionality and are certain that it is not needed in the application. In particular:

   - Ensure that any TLS server(s) the device connects to can still be used. If the server is controlled by a third party or a cloud service, it is recommended to ensure that the firmware supports at least two of the supported cipher suites in case one is disabled in a future update.
   - Ensure that any TLS client(s) that connect to the device can still connect with supported/recommended cipher suites. Note that future versions of client operating systems may remove support for some features, so it is recommended to enable multiple supported cipher suites, or algorithms for redundancy.

   If depending on third party clients or servers, always pay attention to announcements about future changes to supported TLS features. If not, the {IDF_TARGET_NAME} device may become inaccessible if support changes.

.. only:: CONFIG_ESP_ROM_HAS_MBEDTLS_CRYPTO_LIB

   Enabling the config option :ref:`CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL` will use the crypto algorithms from mbedTLS library inside the chip ROM.
   Disabling the config option :ref:`CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL` will use the crypto algorithms from the ESP-IDF mbedtls component library. This will increase the binary size (flash footprint).

.. note::

   Not every combination of mbedTLS compile-time config is tested in ESP-IDF. If you find a combination that fails to compile or function as expected, please report the details on `GitHub <https://github.com/espressif/esp-idf>`_.

VFS
@@@

:doc:`/api-reference/storage/vfs` feature in ESP-IDF allows multiple filesystem drivers and file-like peripheral drivers to be accessed using standard I/O functions (``open``, ``read``, ``write``, etc.) and C library functions (``fopen``, ``fread``, ``fwrite``, etc.). When filesystem or file-like peripheral driver functionality is not used in the application, this feature can be fully or partially disabled. VFS component provides the following configuration options:

* :ref:`CONFIG_VFS_SUPPORT_TERMIOS` — can be disabled if the application does not use ``termios`` family of functions. Currently, these functions are implemented only for UART VFS driver. Most applications can disable this option. Disabling this option reduces the code size by about 1.8 KB.
* :ref:`CONFIG_VFS_SUPPORT_SELECT` — can be disabled if the application does not use the ``select`` function with file descriptors. Currently, only the UART and eventfd VFS drivers implement ``select`` support. Note that when this option is disabled, ``select`` can still be used for socket file descriptors. Disabling this option reduces the code size by about 2.7 KB.
* :ref:`CONFIG_VFS_SUPPORT_DIR` — can be disabled if the application does not use directory-related functions, such as ``readdir`` (see the description of this option for the complete list). Applications that only open, read and write specific files and do not need to enumerate or create directories can disable this option, reducing the code size by 0.5 KB or more, depending on the filesystem drivers in use.
* :ref:`CONFIG_VFS_SUPPORT_IO` — can be disabled if the application does not use filesystems or file-like peripheral drivers. This disables all VFS functionality, including the three options mentioned above. When this option is disabled, :doc:`/api-reference/system/console` can not be used. Note that the application can still use standard I/O functions with socket file descriptors when this option is disabled. Compared to the default configuration, disabling this option reduces code size by about 9.4 KB.

.. only:: CONFIG_ESP_ROM_HAS_HAL_SYSTIMER or CONFIG_ESP_ROM_HAS_HAL_WDT

    HAL
    @@@

    .. list::

        :CONFIG_ESP_ROM_HAS_HAL_SYSTIMER: * Enabling :ref:`CONFIG_HAL_SYSTIMER_USE_ROM_IMPL` can reduce the IRAM usage and binary size by linking in the systimer HAL driver of ROM implementation.
        :CONFIG_ESP_ROM_HAS_HAL_WDT: * Enabling :ref:`CONFIG_HAL_WDT_USE_ROM_IMPL` can reduce the IRAM usage and binary size by linking in the watchdog HAL driver of ROM implementation.

    Heap
    @@@@

    .. list::
        * Enabling :ref:`CONFIG_HEAP_PLACE_FUNCTION_INTO_FLASH` can reduce the IRAM usage and binary size by placing the entirety of the heap functionalities in flash memory.
        :CONFIG_ESP_ROM_HAS_HEAP_TLSF: * Enabling :ref:`CONFIG_HEAP_TLSF_USE_ROM_IMPL` can reduce the IRAM usage and binary size by linking in the TLSF library of ROM implementation.

Bootloader Size
---------------

This document deals with the size of an ESP-IDF app binary only, and not the ESP-IDF :ref:`second-stage-bootloader`.

For a discussion of ESP-IDF bootloader binary size, see :ref:`bootloader-size`.

IRAM Binary Size
----------------

If the IRAM section of a binary is too large, this issue can be resolved by reducing IRAM memory usage. See :ref:`optimize-iram-usage`.
