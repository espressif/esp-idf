最小化二进制文件大小
===============================

:link_to_translation:`en:[English]`

{IDF_TARGET_REDUCED_BY_IRAM: default="DRAM", esp32="IRAM 和/或 DRAM（取决于大小）"}

ESP-IDF 构建系统会编译项目和 ESP-IDF 中所有源文件，但只有程序实际引用的函数和变量才会链接到最终的二进制文件中。在某些情况下，需要减小固件二进制文件的总大小，例如，为使固件适配 flash 分区大小。

要减小固件二进制文件总大小，首先要找到导致其大小增加的原因。

.. _idf.py-size:

测量静态数据大小
---------------------------

为了优化固件二进制文件大小和内存使用，需要测量项目中静态分配的 RAM (``data``, ``bss``)，代码 (``text``) 和只读数据 (``rodata``)。

使用 :ref:`idf.py` 的子命令 ``size``， ``size-components`` 和 ``size-files`` 可以输出项目使用内存概况：

.. note::

    添加 ``-DOUTPUT_FORMAT=csv`` 或 ``-DOUTPUT_FORMAT=json``，即可用 CSV 或 JSON 格式输出文件。

数据大小概况 ``idf.py size``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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

该输出结果细分了固件二进制文件中所有静态内存区域的大小：

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

    - ``Used static DRAM``：编译时分配的 DRAM 大小。 ``remain`` 表示在运行时可用作堆内存的 DRAM 余量。请注意，由于元数据开销、实现限制和启动时的堆分配，实际的 DRAM 堆会更小。

        - ``.data size``：编译时为 ``.data`` （即所有初始化值为非零值的静态变量）分配的 DRAM 大小。 ``.data`` 还在二进制映像中占用空间来存储非零初始化值。
        - ``.bss size``：编译时为 ``.bss`` （即所有初始化值为零的静态变量）分配的 DRAM 大小。``.bss`` 不会在 flash 中占用额外空间。

    - ``Used static IRAM``：编译时分配的 IRAM 大小。 ``remain`` 表示在运行时可用作堆内存的 IRAM 余量。请注意，由于元数据开销、实现限制和启动时的堆分配，实际的 IRAM 堆会更小。

        - ``.text size``：用于 ``.text`` 的 IRAM 大小（即所有从 :ref:`IRAM <iram>` 执行的代码）。由于代码最初存储在 ``.text`` 中，在启动时才会复制到 IRAM，因此 ``.text`` 在二进制映像中也会占用空间。

    - ``Used Flash size``：使用的 flash 大小（不包括 DRAM 和 IRAM 的使用量）。

        - ``.text``：用于 ``.text`` （即通过 flash 缓存执行的所有代码，请参阅 :ref:`IROM <irom>`）的 flash 大小。
        - ``.rodata``：用于 ``.rodata`` （即通过 flash 缓存加载的只读数据，参阅 :ref:`DROM <drom>`）的 flash 大小。

    - ``Total image size`` 是二进制文件的预估总大小。

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

    - ``Used stat D/IRAM``：编译时使用的 D/IRAM 大小。``remain`` 表示在运行时可用作堆内存的 D/IRAM 余量。请注意，由于元数据开销、实现限制和启动时的堆分配，实际的 DRAM 堆会更小。

        - ``.data size``：编译时为 ``.data`` （即所有初始化值为非零值的静态变量）分配的 D/IRAM 大小。 ``.data`` 还在二进制映像中占用空间来存储非零初始化值。
        - ``.bss size``：编译时为 ``.bss`` （即所有初始化值为零的静态变量）分配的 D/IRAM 大小。``.bss`` 不会在 flash 中占用额外空间。
        - ``.text size``：用于 ``.text`` 的 D/IRAM 大小（即所有从内部 RAM 执行的代码）。由于代码最初存储在 ``.text`` 中，在启动时才会复制到 D/IRAM，因此 ``.text`` 在二进制映像中也会占用空间。

    - ``Used Flash size``：使用的 flash 总大小（不包括 D/IRAM 的使用量）。

        - ``.text``：用于 ``.text`` （即通过 flash 缓存执行的所有代码，请参阅 :ref:`IROM <irom>`）的 flash 大小。
        - ``.rodata``：用于 ``.rodata`` （即通过 flash 缓存加载的只读数据，参阅 :ref:`DROM <drom>`）的 flash 大小。

    - ``Total image size`` is the estimated total size of the binary file.


组件使用概况 ``idf.py size-components``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``idf.py size`` 的输出结果不够详细，无法找出导致二进制文件过大的主要原因。要进行更详细的分析，请使用 ``idf.py size-components`` 。

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

``idf.py size-components``  输出的前几行与 ``idf.py size`` 相同，此外还会输出 ``Per-archive contributions to ELF file`` 表格，显示每个静态库对最终二进制文件大小的贡献程度。

通常，每个组件都会构建一个静态库归档文件，尽管部分是由特定组件包含的二进制库，例如， ``esp_wifi`` 组件包含了 ``libnet80211.a``。此外，这里还列出了一些工具链库，例如 ``libc.a`` 和 ``libgcc.a``，用于提供 C/C++ 标准库和工具链内置功能。

对于只有一个 ``main`` 组件的简单项目，可在 ``libmain.a`` 目录下找到所有项目代码。若项目包含其特有组件（参阅 :doc:`/api-guides/build-system`），则每个组件将单独在一行中显示。

该表格按静态库归档文件对最终二进制文件大小的贡献程度降序排序。

各列含义如下：

.. list::

    - ``DRAM .data & .bss & other`` - ``.data`` 和 ``.bss`` 分别与上方显示的总数相同。两者都是静态变量，且都会减少运行时的可用 RAM，但 ``.bss`` 不会增加二进制文件大小。 ``other`` 列指任何会增加 RAM 大小的自定义数据段，该值通常为 0。
    :esp32: - ``IRAM`` - 该列与上方显示的总数相同，表示链接到从 IRAM 执行的代码，这些代码占用二进制文件空间，并且会减少执行 ``HEAP_CAP_32BIT`` 时可动态分配的 IRAM。
    :esp32: - ``D/IRAM`` - 显示了 IRAM 占用的空间。D/IRAM 占用的空间会减少运行时可用作堆内存的 DRAM 空间。
    :not esp32: - ``IRAM`` - 与上方显示的总数相同，表示链接到从 IRAM 执行的代码，这些代码占用二进制文件空间，并且会减少运行时可用作堆内存的 DRAM 空间。
    - ``Flash code & rodata`` - 这些值与上方显示总数相同，指通过 flash 缓存访问的 IROM 和 DROM 空间，对二进制文件大小的贡献。

源文件使用概况 ``idf.py size-files``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

要了解更多详情，请运行 ``idf.py size-files``，获取每个目标文件对最终二进制文件大小的贡献概况。每个目标文件对应一个单独的源文件。

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

文件总大小概况下方会显示 ``Per-file contributions to ELF file`` 表格。

该表格的列与上文运行 ``idy.py size-components`` 显示的列相同，但该表格的粒度更细，展示了每个目标文件对二进制文件大小的贡献。

例如，文件 ``x509_crt_bundle.S.o`` 对总固件大小贡献了 64,212 字节，全都存储在 flash 中的 ``.rodata`` 区域。由此可以推知，该应用程序正在使用 :doc:`/api-reference/protocols/esp_crt_bundle` 功能。如果不使用该功能，固件大小至少可以减少 64,212 字节。

某些目标文件从二进制库中链接至此，因此无法找到对应源文件。要确定一个源文件属于哪个组件，通常可以在 ESP-IDF 源代码树中搜索，或者在 :ref:`linker-map-file` 中查找完整路径。

比较两个二进制文件
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

如果某些改动影响了二进制文件大小，可以使用 ESP-IDF 工具来详细分析文件大小的确切差异。

该操作不是通过运行 ``idf.py`` 进行的，而是需要直接运行 Python 工具 `esp_idf_size <https://github.com/espressif/esp-idf-size>`_ 。

执行该操作，首先需要在构建目录中找到链接器映射文件 ``PROJECTNAME.map``。 ``esp_idf_size`` 工具会基于链接器映射文件的输出结果分析文件大小差异。

要与另一个二进制文件进行比较，还需要保存该文件对应的 ``.map``  文件，该文件位于构建目录中。

例如，要比较两个构建文件，其中一个使用默认的 :ref:`CONFIG_COMPILER_OPTIMIZATION` ``Debug (-Og)`` 配置，而另一个使用 ``Optimize for size (-Os)`` 配置：

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

从 ``Difference`` 列可以看出，改变该设置导致整个二进制文件减小了 60 KB 以上，并且可用的 RAM 增加了 5 KB 以上。

还可以使用 ``diff`` 模式来输出表格，显示组件级（静态库）的差异：

.. note::

    运行 ``esp_idf_size`` 时可以使用 ``--format`` 选项输出 JSON 或 CSV 格式的结果。

.. code-block:: bash

    python -m esp_idf_size --archives --diff build_Og/https_request.map build_Oshttps_request.map

同样适用于比较单个源文件级的差异：

.. code-block:: bash

    python -m esp_idf_size --files --diff build_Og/https_request.map build_Oshttps_request.map

了解将输出写入文件等其他选项，可以输入 ``--help`` 查看完整列表。

.. _idf-size-linker-failed:

链接器失败时显示文件大小
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

如果被分配的静态内存大小超越上限，链接器会失败并显示错误信息，例如 ``DRAM segment data does not fit`` 和 ``region `iram0_0_seg' overflowed by 44 bytes`` 等。

在这些情况下， ``idf.py size`` 也无法成功执行。然而，通过手动运行 ``esp_idf_size``，可以查看 **部分静态内存使用情况** 。内存使用情况将不包含无法链接的变量，因此仍然会显示有部分可用空间。

映射文件参数为构建目录下的 ``<projectname>.map`` 文件。

.. code-block:: bash

    python -m esp_idf_size build/project_name.map

还可以查看类似于 ``size-components`` 或 ``size-files`` 输出的内容：

.. code-block:: bash

    python -m esp_idf_size --archives build/project_name.map
    python -m esp_idf_size --files build/project_name.map

.. _linker-map-file:

链接器映射文件
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. note::

    这是一种非常有用的进阶分析方法。可以先跳转到 :ref:`reducing-overall-size`，以后再详阅这一部分。

分析工具 ``idf.py size`` 通过解析 GNU binutils 的“链接器映射文件”来输出结果，该文件囊括了链接器在创建（即链接到）最终固件二进制文件时的所有操作。

链接器映射文件本身是纯文本文件，因此可以进行读取并准确了解链接器的操作，但这些文件非常复杂冗长，通常有 100,000 行甚至更多。

映射文件分为多个部分，每个部分各有标题，包括：

- ``Archive member included to satisfy reference by file (symbol)``

    - 该列表显示了链接器链接各个目标文件时所搜寻的特定符号（函数或变量）。
    - 要了解二进制文件包含特定目标文件的原因，可以查看该列表以及文件末尾的 ``Cross Reference Table``。

    .. note::

        请注意，并非每个显示在该列表中的目标文件最后都会出现在最终二进制文件中，有些目标文件可能会列在 ``Discarded input sections`` 中。

- ``Allocating common symbols``

    - 该列表显示了部分全局变量及其大小。常见符号在 ELF 二进制文件中具有特定含义，但 ESP-IDF 并未广泛使用这些符号。

- ``Discarded input sections``

    - 在链接器读取目标文件时，会将一些输入段作为文件的一部分读取并准备链接到最终的二进制文件中，但由于没有其他部分引用这些输入段，这些段最终会被丢弃。
    - 对于 ESP-IDF 项目来说，这个列表可能会非常长，因为我们将每个函数和静态变量都编译到一个独立的段中，以最小化最终二进制文件的大小。具体而言，ESP-IDF 将使用编译器选项 ``-ffunction-sections -fdata-sections`` 和链接器选项 ``--gc-sections``。
    - 在这个列表中出现的条目 **不会** 对最终的二进制文件大小产生影响。

- ``Memory Configuration`` 和 ``Linker script and memory map``

    - 这两部分相互关联。输出结果的一部分来自由 :doc:`/api-guides/build-system` 提供的链接器命令行和链接脚本，部分链接脚本由 ESP-IDF 项目通过 :doc:`/api-guides/linker-script-generation` 功能生成。

    - 在 map 文件的 ``Linker script and memory map`` 输出中，会显示链接到最终二进制文件中的每个符号（函数或静态变量）及其地址（以 16 位十六进制数字表示）和长度（也以十六进制表示），还有链接的库和目标文件（可以用于确定组件和源文件）。

    - 在所有占用最终 ``.bin`` 文件的输出段之后， ``memory map`` 还会显示一些 ELF 文件中用于调试的段，如 ``.debug_*`` 等。这些段不会对最终的二进制文件大小产生影响，且这些符号的地址数值很小，从 ``0x0000000000000000`` 开始递增。

- ``Cross Reference Table``

    - 该表格显示了引用了各个符号（函数或静态变量）的目标文件。了解二进制文件包含某个特定符号的原因，可参考该表格以确定引用特定符号的目标文件。

    .. note::

        ``Cross Reference Table`` 不仅包含最终二进制文件中的符号，还包含已丢弃的段内符号。因此，某个符号出现在该表中并不意味着最终二进制文件包含这一符号，需要单独检查。


.. note::

   链接器映射文件由 GNU binutils 的链接器 ``ld`` 而非由 ESP-IDF 生成。本快速概览专从 ESP-IDF 构建系统的角度编写而成，建议自行搜索更多关于链接器映射文件格式的信息。

.. _reducing-overall-size:

减小总体文件大小
-------------------------------

可以通过以下配置选项减小几乎所有 ESP-IDF 项目最终二进制文件的大小：

.. list::

    - 将 :ref:`CONFIG_COMPILER_OPTIMIZATION` 设置为 ``Optimize for size (-Os)``。在某些情况下，相较于默认设置， ``Optimize for size (-Os)`` 也可以减小二进制文件的大小。请注意，若代码包含 C 或 C++ 的未定义行为，提高编译器优化级别可能会暴露出原本不存在的错误。
    - 通过降低应用程序的 :ref:`CONFIG_LOG_DEFAULT_LEVEL` ，可以减少编译时的日志输出。如果改变 :ref:`CONFIG_LOG_MAXIMUM_LEVEL` 的默认选项，则可以控制二进制文件的大小。减少编译时的日志输出可以减少二进制文件中的字符串数量，并减小调用日志函数的代码大小。
    - 将 :ref:`CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL` 设置为 ``Silent``，可以避免为所有可能失败的断言编译专门的断言字符串和源文件名。尽管如此，仍可以通过查看断言失败时的内存地址以在代码中找到失败断言。
    - 除 :ref:`CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL` 外，还可以通过设置 :ref:`CONFIG_HAL_DEFAULT_ASSERTION_LEVEL` 单独禁用或静默 HAL 组件的断言。请注意，即使将 :ref:`CONFIG_HAL_DEFAULT_ASSERTION_LEVEL` 设置为 full-assertion 级别，ESP-IDF 在引导加载程序中也会把 HAL 断言级别降为 silent，以减小引导加载程序的大小。
    - 设置 :ref:`CONFIG_COMPILER_OPTIMIZATION_CHECKS_SILENT` 会移除针对 ESP-IDF 内部错误检查宏的特定错误消息。错误消息移除后，通过阅读日志输出来调试某些错误条件可能变得更加困难。
    :esp32: - 如果二进制文件只需要在某些特定的 ESP32 版本上运行，将 :ref:`CONFIG_ESP32_REV_MIN` 增加到相应版本的匹配值可以减小二进制文件的大小。如果设置 ESP32 最低版本为 3，并且启用 PSRAM，将大幅减小二进制文件的大小。
    :esp32c3: - 如果二进制文件只需要在某些特定的 ESP32-C3 版本上运行，将 :ref:`CONFIG_ESP32C3_REV_MIN` 增加到相应版本的匹配值可以减小二进制文件的大小。由于某些功能已经移至 ROM 代码中，如果设置 ESP32-C3 最低版本为 3 并且使用 Wi-Fi 功能，将明显减小二进制文件的大小。
    - 不要启用 :ref:`CONFIG_COMPILER_CXX_EXCEPTIONS` 或 :ref:`CONFIG_COMPILER_CXX_RTTI`，也不要将 :ref:`CONFIG_COMPILER_STACK_CHECK_MODE` 设置为 Overall。这些选项已默认禁用，启用这些选项会大幅增加二进制文件的大小。
    - 禁用 :ref:`CONFIG_ESP_ERR_TO_NAME_LOOKUP` 将会移除查找表，该表用于将错误日志中的错误值转换成用户友好名称（参阅 :doc:`/api-guides/error-handling`）。这样做可以减小二进制文件的大小，但错误值将只以整数形式输出。
    - 将 :ref:`CONFIG_ESP_SYSTEM_PANIC` 设置为 ``Silent reboot`` 可以减小一小部分二进制文件的大小，但此操作 **仅** 建议在没有任何开发者使用 UART 输出来调试设备时进行。
    :CONFIG_IDF_TARGET_ARCH_RISCV: - 设置 :ref:`CONFIG_COMPILER_SAVE_RESTORE_LIBCALLS` 以库调用替代内联的入口/出口代码，可以减小二进制文件的大小。
    - 如果应用程序的二进制文件只使用 protocomm 组件的某个安全版本，取消对其他版本的支持可以减小部分代码大小。请通过 :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0`、:ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1` 或者 :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_2` 方式，取消对应版本的支持。

.. note::

   除了上述众多配置项之外，还有一些配置选项在更改为非默认设置时会增加二进制文件的大小，这些选项未在此列出。配置项的帮助文本中通常会阐明显著增加二进制文件大小的设置。

.. _size-targeted-optimizations:

针对性优化
^^^^^^^^^^^^^^^^^^^^^^^^^^^

以下二进制文件大小优化适用于特定的组件或函数：

.. only:: SOC_WIFI_SUPPORTED

    Wi-Fi
    @@@@@

    - 如果不需要启用 WPA3 支持，禁用 :ref:`CONFIG_ESP_WIFI_ENABLE_WPA3_SAE` 可以减小 Wi-Fi 二进制文件的大小。请注意，WPA3 支持是目前认证新 Wi-Fi 设备的必要标准。
    - 如果不需要启用 soft-AP 支持，禁用 :ref:`CONFIG_ESP_WIFI_SOFTAP_SUPPORT` 可以减小 Wi-Fi 二进制文件的大小。
    - 如不需要启用企业支持，禁用 :ref:`CONFIG_ESP_WIFI_ENTERPRISE_SUPPORT` 可以减小 Wi-Fi 二进制文件的大小。

.. only:: esp32

    ADC
    @@@

    - 如果使用 ADC 驱动程序，禁用 :ref:`CONFIG_ADC_CAL_EFUSE_TP_ENABLE`、:ref:`CONFIG_ADC_CAL_EFUSE_VREF_ENABLE` 和 :ref:`CONFIG_ADC_CAL_LUT_ENABLE` 可以减小一小部分二进制文件的大小，但准确性会降低。

.. only:: SOC_BT_SUPPORTED

    Bluetooth NimBLE
    @@@@@@@@@@@@@@@@

    如果使用 :doc:`/api-reference/bluetooth/nimble/index`，要减小二进制文件的大小，可以执行以下操作：

    .. list::

        :esp32: - 如果只需要连接一个 Bluetooth LE，则将 :ref:`CONFIG_BTDM_CTRL_BLE_MAX_CONN` 设置为 1。
        - 如果只需要连接一个 Bluetooth LE，则将 :ref:`CONFIG_BT_NIMBLE_MAX_CONNECTIONS` 设置为 1。
        - 如果不需要 :ref:`CONFIG_BT_NIMBLE_ROLE_CENTRAL` 和 :ref:`CONFIG_BT_NIMBLE_ROLE_OBSERVER`，可以选择禁用其一。
        - 降低 :ref:`CONFIG_BT_NIMBLE_LOG_LEVEL` 可以减小二进制文件的大小。请注意，如果在上述 :ref:`reducing-overall-size` 中已经降低了整体日志级别，那么也会降低 NimBLE 的日志级别。

lwIP IPv6
@@@@@@@@@

- 将 :ref:`CONFIG_LWIP_IPV6` 设置为 ``false`` 可以减小 lwIP TCP/IP 堆栈的大小，但将仅支持 IPv4。

  .. note::

      如果禁用 IPv6，:doc:`/api-reference/protocols/asio` 等组件将无法使用。

lwIP IPv4
@@@@@@@@@

- 如果不需要 IPv4 连接功能，将 :ref:`CONFIG_LWIP_IPV4` 设置为 ``false`` 可以减小 lwIP 的大小，使其成为仅支持 IPv6 的 TCP/IP 堆栈。

  .. note::

      在禁用 IPv4 支持之前，请注意，仅支持 IPv6 的网络环境尚未普及，必须在本地网络中提供支持，例如，由互联网服务供应商提供支持，或使用受限制的本地网络设置。

.. _newlib-nano-formatting:

Newlib Nano 格式化
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ESP-IDF 的 I/O 函数（ ``printf()`` 和 ``scanf()`` 等）默认使用 Newlib 的 “完整” 格式化功能。

.. only:: CONFIG_ESP_ROM_HAS_NEWLIB_NANO_FORMAT

    启用配置选项 :ref:`CONFIG_NEWLIB_NANO_FORMAT` 将使 Newlib 切换到 Nano 格式化模式。这种模式的代码量更小，并且大部分内容被编译到了 {IDF_TARGET_NAME} 的 ROM 中，因此不需要将其添加至二进制文件中。

    具体的二进制文件大小差异取决于固件使用的功能，但通常为 25 KB 到 50 KB。

.. only:: CONFIG_ESP_ROM_HAS_NEWLIB_NORMAL_FORMAT

    禁用配置选项 :ref:`CONFIG_NEWLIB_NANO_FORMAT` 将切换 Newlib 到“完整”格式化模式。这将减小二进制文件的大小，因为 {IDF_TARGET_NAME} 的 ROM 中已存有完整格式化版本的函数，因此不需要将其添加至二进制文件中。

启用 Nano 格式化会减少调用 ``printf()`` 或其他字符串格式化函数的堆栈使用量，参阅 :ref:`optimize-stack-sizes`。

“Nano”  格式化不支持 64 位整数或 C99 格式化功能。请在 `Newlib README 文件`_ 中搜索 ``--enable-newlib-nano-formatted-io`` 来获取完整的限制列表。


.. only:: esp32c2

    .. note::

        {IDF_TARGET_NAME} 会默认启用 :ref:`CONFIG_NEWLIB_NANO_FORMAT`。


.. _Newlib README 文件: https://sourceware.org/newlib/README

.. _minimizing_binary_mbedtls:

MbedTLS 功能
@@@@@@@@@@@@@@@@@@@@@

在 **Component Config** > **mbedTLS** 下有多个默认启用的 mbedTLS 功能，如果不需要，可以禁用相应功能以减小代码大小。

这些功能包括：

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
- :ref:`CONFIG_MBEDTLS_ECP_C` （或者：启用此选项，但在子菜单中禁用部分椭圆曲线）
- :ref:`CONFIG_MBEDTLS_ECP_NIST_OPTIM`
- :ref:`CONFIG_MBEDTLS_ECP_FIXED_POINT_OPTIM`
- 如果不需要 mbedTLS 的服务器和客户端功能，可以修改 :ref:`CONFIG_MBEDTLS_TLS_MODE`
- 可以考虑禁用在 ``TLS Key Exchange Methods`` 子菜单中列出的一些密码套件（例如 :ref:`CONFIG_MBEDTLS_KEY_EXCHANGE_RSA`），以减小代码大小。

每个选项的帮助文本中都有更多信息可供参考。

.. important::

   **强烈建议不要禁用所有 mbedTLS 选项。** 仅在理解功能用途，并确定在应用程序中不需要此功能时，方可禁用相应选项。请特别注意以下两点：

   - 确保设备连接的任何 TLS 服务器仍然可用。如果服务器由第三方或云服务控制，建议确保固件至少支持两种 TLS 密码套件，以防未来某次更新禁用了其中一种。
   - 确保连接设备的任何 TLS 客户端仍然可以使用支持/推荐的密码套件进行连接。请注意，未来版本的客户端操作系统可能会移除对某些功能的支持，因此建议启用多个支持的密码套件或算法以实现冗余。

   如果依赖于第三方客户端或服务器，请密切关注其有关支持的 TLS 功能的公告和变更。否则，当所支持功能变更时，{IDF_TARGET_NAME} 设备可能无法访问。

.. only:: CONFIG_ESP_ROM_HAS_MBEDTLS_CRYPTO_LIB

   启用配置选项 :ref:`CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL` 时 mbedtls 使用由 ROM 提供的加密算法。
   禁用配置选项 :ref:`CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL` 时mbedtls 完全使用由 ESP-IDF 中提供的加密算法。这会导致二进制文件大小增加。

.. note::

   ESP-IDF 并未测试所有 mbedTLS 编译配置组合。如果发现某个组合无法编译或无法按预期执行，请在 `GitHub <https://github.com/espressif/esp-idf>`_ 上报告详细信息。

虚拟文件系统 (VFS)
@@@@@@@@@@@@@@@@@@@@@

在 ESP-IDF 中，:doc:`/api-reference/storage/vfs` 功能允许使用标准的 I/O 函数（如 ``open``、 ``read``、 ``write`` 等）和 C 库函数（如 ``fopen``、 ``fread``、 ``fwrite`` 等）来访问多个文件系统驱动程序和类似文件的外设驱动程序。当应用程序中不需要文件系统或类似文件的外设驱动功能时，可以部分或完全禁用该功能。VFS 组件提供以下配置选项：

* :ref:`CONFIG_VFS_SUPPORT_TERMIOS` — 如果应用程序不使用 ``termios`` 函数族，可以禁用此选项。目前，这些函数仅在 UART VFS 驱动程序中实现，大多数应用程序可以禁用此选项。禁用后可以减小约 1.8 KB 代码大小。
* :ref:`CONFIG_VFS_SUPPORT_SELECT` — 如果应用程序不使用 ``select`` 函数处理文件描述符，可以禁用此选项。目前，只有 UART 和 eventfd VFS 驱动程序支持 ``select`` 函数。请注意，当禁用该选项时，仍然可以使用 ``select`` 处理套接字文件描述符。禁用此选项将减小约 2.7 KB 代码大小。
* :ref:`CONFIG_VFS_SUPPORT_DIR` — 如果应用程序不使用与目录相关的函数，例如 ``readdir`` （参阅此选项的描述以获取完整列表），可以禁用此选项。如果应用程序只需打开、读取和写入特定文件，而不需要枚举或创建目录，可以禁用此选项，从而减少超过 0.5 KB 代码大小，具体减小多少取决于使用的文件系统驱动程序。
* :ref:`CONFIG_VFS_SUPPORT_IO` — 如果应用程序不使用文件系统或类似文件的外设驱动程序，可以禁用此选项，这将禁用所有 VFS 功能，包括上述三个选项。当禁用此选项时，无法使用 :doc:`/api-reference/system/console`。请注意，当禁用此选项时，应用程序仍然可以使用标准 I/O 函数处理套接字文件描述符。相较于默认配置，禁用此选项可以减小约 9.4 KB 代码大小。

.. only:: CONFIG_ESP_ROM_HAS_HAL_SYSTIMER or CONFIG_ESP_ROM_HAS_HAL_WDT

    HAL
    @@@

    .. list::

        :CONFIG_ESP_ROM_HAS_HAL_SYSTIMER: * 启用 :ref:`CONFIG_HAL_SYSTIMER_USE_ROM_IMPL` 可以通过链接 ROM 实现的 systimer HAL 驱动程序来减少 IRAM 使用和二进制文件大小。
        :CONFIG_ESP_ROM_HAS_HAL_WDT: * 启用 :ref:`CONFIG_HAL_WDT_USE_ROM_IMPL` 可以通过链接 ROM 实现的看门狗 HAL 驱动程序来减少 IRAM 使用和二进制文件大小。

    堆
    @@@@@@

    .. list::
        * 启用 :ref:`CONFIG_HEAP_TLSF_USE_ROM_IMPL` 可以将整个堆功能放置在 flash 中，从而减少 IRAM 使用和二进制文件大小。
        :CONFIG_ESP_ROM_HAS_HEAP_TLSF: * 启用 :ref:`CONFIG_HEAP_TLSF_USE_ROM_IMPL` 可以通过链接 ROM 实现的 TLSF 库来减少 IRAM 使用和二进制文件大小。

引导加载程序大小
------------------------------

本文档仅涉及 ESP-IDF 应用程序的二进制文件大小，而不涉及 ESP-IDF :ref:`second-stage-bootloader`。

关于 ESP-IDF 引导加载程序二进制文件大小的讨论，参阅 :ref:`bootloader-size`。

IRAM 二进制文件大小
------------------------------------

如果二进制文件的 IRAM 部分过大，可以通过减少 IRAM 使用来解决这个问题，参阅 :ref:`optimize-iram-usage`。
