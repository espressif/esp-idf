最小化二进制文件大小
===============================

:link_to_translation:`en:[English]`

ESP-IDF 构建系统会编译项目和 ESP-IDF 中所有源文件，但只有程序实际引用的函数和变量才会链接到最终的二进制文件中。在某些情况下，需要减小固件二进制文件的总大小，例如，为使固件适配 flash 分区大小。

要减小固件二进制文件总大小，首先要找到导致其大小增加的原因。

.. _idf.py-size:

测量静态数据大小
---------------------------

为了优化固件二进制文件大小和内存使用，需要测量项目中静态分配的 RAM (``data``, ``bss``)，代码 (``text``) 和只读数据 (``rodata``)。:ref:`idf.py` 工具的子命令 ``size``、``size-components`` 和 ``size-files`` 可分别用于检查不同级别静态分配的 RAM 使用情况。详情请参阅 :doc:`/api-guides/tools/idf-size` 工具。

.. _linker-map-file:

链接器映射文件
---------------------------

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
    - 如果应用程序不需要动态更改日志级别，并且不需要使用标签来控制每个模块的日志，建议禁用 :ref:`CONFIG_LOG_DYNAMIC_LEVEL_CONTROL` 并更改 :ref:`CONFIG_LOG_TAG_LEVEL_IMPL`。与默认选项相比，这可以节约大概 260 字节的 IRAM、264 字节的 DRAM、以及 1 KB 的 flash，同时还可以加快日志记录的速度。
    - 将 :ref:`CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL` 设置为 ``Silent``，可以避免为所有可能失败的断言编译专门的断言字符串和源文件名。尽管如此，仍可以通过查看断言失败时的内存地址以在代码中找到失败断言。
    - 除 :ref:`CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL` 外，还可以通过设置 :ref:`CONFIG_HAL_DEFAULT_ASSERTION_LEVEL` 单独禁用或静默 HAL 组件的断言。即使将 :ref:`CONFIG_HAL_DEFAULT_ASSERTION_LEVEL` 设置为 full-assertion 级别，ESP-IDF 在引导加载程序中也会把 HAL 断言级别降为 silent，以减小引导加载程序的大小。
    - 设置 :ref:`CONFIG_COMPILER_OPTIMIZATION_CHECKS_SILENT` 会移除针对 ESP-IDF 内部错误检查宏的特定错误消息。错误消息移除后，通过阅读日志输出来调试某些错误条件可能变得更加困难。
    :esp32: - 如果二进制文件只需要在某些特定的 ESP32 版本上运行，将 :ref:`CONFIG_ESP32_REV_MIN` 增加到相应版本的匹配值可以减小二进制文件的大小。如果设置 ESP32 最低版本为 3，并且启用 PSRAM，将大幅减小二进制文件的大小。
    :esp32c3: - 如果二进制文件只需要在某些特定的 ESP32-C3 版本上运行，将 :ref:`CONFIG_ESP32C3_REV_MIN` 增加到相应版本的匹配值可以减小二进制文件的大小。由于某些功能已经移至 ROM 代码中，如果设置 ESP32-C3 最低版本为 3 并且使用 Wi-Fi 功能，将明显减小二进制文件的大小。
    - 不要启用 :ref:`CONFIG_COMPILER_CXX_EXCEPTIONS` 或 :ref:`CONFIG_COMPILER_CXX_RTTI`，也不要将 :ref:`CONFIG_COMPILER_STACK_CHECK_MODE` 设置为 Overall。这些选项已默认禁用，启用这些选项会大幅增加二进制文件的大小。
    - 禁用 :ref:`CONFIG_ESP_ERR_TO_NAME_LOOKUP` 将会移除查找表，该表用于将错误日志中的错误值转换成用户友好名称（参阅 :doc:`/api-guides/error-handling`）。这样做可以减小二进制文件的大小，但错误值将只以整数形式输出。
    - 将 :ref:`CONFIG_ESP_SYSTEM_PANIC` 设置为 ``Silent reboot`` 可以减小一小部分二进制文件的大小，但此操作 **仅** 建议在没有任何开发者使用 UART 输出来调试设备时进行。
    :CONFIG_IDF_TARGET_ARCH_RISCV: - 设置 :ref:`CONFIG_COMPILER_SAVE_RESTORE_LIBCALLS` 以库调用替代内联的入口/出口代码，可以减小二进制文件的大小。
    - 如果应用程序的二进制文件只使用 protocomm 组件的某个安全版本，取消对其他版本的支持可以减小部分代码大小。请通过 :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0`、:ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1` 或者 :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_2` 方式，取消对应版本的支持。
    :CONFIG_SOC_CPU_ZCMP_WORKAROUND: - 启用 :ref:`CONFIG_COMPILER_ENABLE_RISCV_ZCMP`，通过使用压缩的函数序言/尾声来减少二进制文件大小。在启用此选项前，请务必仔细阅读 :ref:`CONFIG_COMPILER_ENABLE_RISCV_ZCMP` 的说明。

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
    - 如果不需要启用 WPA3-SAE 的哈希到元素（hash-to-element）认证方法，禁用 :ref:`CONFIG_ESP_WIFI_ENABLE_SAE_H2E` 可以减少 Wi-Fi 二进制文件的大小。请注意，与默认的 “hunting-and-pecking” 认证方法相比，哈希到元素方法更快、更安全，并且对侧信道攻击更具有免疫能力。

.. only:: esp32

    ADC
    @@@

    - 如果使用 ADC 驱动程序，禁用 :ref:`CONFIG_ADC_CALI_EFUSE_TP_ENABLE`、:ref:`CONFIG_ADC_CALI_EFUSE_VREF_ENABLE` 和 :ref:`CONFIG_ADC_CALI_LUT_ENABLE` 可以减小一小部分二进制文件的大小，但准确性会降低。

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

.. _picolibc-instead-of-newlib:

使用 Picolibc 替代 Newlib
@@@@@@@@@@@@@@@@@@@@@@@@@

默认情况下，ESP-IDF 使用 Newlib C 库，同时也对 Picolibc C 库提供实验性支持。

Picolibc C 库提供了更精简的 ``printf`` 系列函数，并且根据应用程序，可以将二进制文件大小减少最多 30 KB。

如需切换链接到 Picolibc C 库，请启用配置选项 :ref:`CONFIG_IDF_EXPERIMENTAL_FEATURES` 和 :ref:`CONFIG_LIBC_PICOLIBC<CONFIG_LIBC_PICOLIBC>`。

.. _newlib-nano-formatting:

Newlib Nano 格式化
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ESP-IDF 的 I/O 函数（ ``printf()`` 和 ``scanf()`` 等）默认使用 Newlib 的 “完整” 格式化功能。

.. only:: CONFIG_ESP_ROM_HAS_NEWLIB_NANO_FORMAT

    启用配置选项 :ref:`CONFIG_LIBC_NEWLIB_NANO_FORMAT` 将 Newlib 切换到 Nano 格式化模式。从而减小了代码体积，同时大部分内容被编译到 {IDF_TARGET_NAME} 的 ROM 中，因此不需要将其添加至二进制文件中。

    具体的二进制文件大小差异取决于固件使用的功能，但通常为 25 KB 到 50 KB。

.. only:: CONFIG_ESP_ROM_HAS_NEWLIB_NORMAL_FORMAT

    禁用配置选项 :ref:`CONFIG_LIBC_NEWLIB_NANO_FORMAT` 将 Newlib 切换到完整格式化模式。从而减小二进制文件的大小，因为 {IDF_TARGET_NAME} 的 ROM 中已存有完整格式化版本的函数，因此无需将其添加至二进制文件中。

启用 Nano 格式化会减少调用 ``printf()`` 或其他字符串格式化函数的堆栈使用量，参阅 :ref:`optimize-stack-sizes`。

“Nano”  格式化不支持 64 位整数或 C99 格式化功能。请在 `Newlib README 文件`_ 中搜索 ``--enable-newlib-nano-formatted-io`` 来获取完整的限制列表。


.. only:: esp32c2

    .. note::

        {IDF_TARGET_NAME} 会默认启用 :ref:`CONFIG_LIBC_NEWLIB_NANO_FORMAT`。


.. _Newlib README 文件: https://sourceware.org/newlib/README

libstdc++
@@@@@@@@@

- 启用 :ref:`CONFIG_COMPILER_CXX_GLIBCXX_CONSTEXPR_COLD_CONSTEXPR<CONFIG_COMPILER_CXX_GLIBCXX_CONSTEXPR_COLD_CONSTEXPR>` 或 :ref:`CONFIG_COMPILER_CXX_GLIBCXX_CONSTEXPR_COLD<CONFIG_COMPILER_CXX_GLIBCXX_CONSTEXPR_COLD>` 观察对应用程序二进制大小的影响。

.. _minimizing_binary_mbedtls:

MbedTLS 功能
@@@@@@@@@@@@@@@@@@@@@

在 **Component Config** > **mbedTLS** 下有多个默认启用的 mbedTLS 功能，如果不需要，可以禁用相应功能以减小代码大小。

这些功能包括：

.. list::

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
    - 如果不需要 mbedTLS 的服务器和客户端功能，可以修改 :ref:`CONFIG_MBEDTLS_TLS_MODE`。
    - 可以考虑禁用在 ``TLS Key Exchange Methods`` 子菜单中列出的一些密码套件（例如 :ref:`CONFIG_MBEDTLS_KEY_EXCHANGE_RSA`），以减小代码大小。
    - 如果应用程序已经通过使用 :cpp:func:`mbedtls_strerror` 拉取 mbedTLS 错误字符串，则可以考虑禁用 :ref:`CONFIG_MBEDTLS_ERROR_STRINGS`。
    :esp32h2: - 对于 {IDF_TARGET_NAME} v1.2 及以上版本，可以考虑禁用 :ref:`CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_MASKING_CM` 和 :ref:`CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM`，因为无需再使用 ECDSA 签名的软件防护措施。
    :SOC_AES_SUPPORT_DMA: - 如果应用程序不涉及或不需要针对小数据长度操作进行性能优化，例如在处理小数据段时进行的 NVS 加密/解密操作、TLS 通信等，可以考虑禁用 :ref:`CONFIG_MBEDTLS_AES_HW_SMALL_DATA_LEN_OPTIM`。

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


.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

    控制台
    @@@@@@

    对于支持 USB-Serial-JTAG 的目标芯片，默认情况下会同时启用 USB-Serial-JTAG 和 UART 控制台输出。如果只需要使用单个控制台，可以通过以下操作减少二进制文件大小和 RAM 使用量：

    1. 禁用次要控制台：将 :ref:`CONFIG_ESP_CONSOLE_SECONDARY` 设置为 ``CONFIG_ESP_CONSOLE_SECONDARY_NONE``。
    2. 将 :ref:`CONFIG_ESP_CONSOLE_UART` 设置为以下选项之一：

        * ``UART``：可减少约 2.5 KB 的二进制文件大小。
        * ``USB-Serial-JTAG``：可减少约 10 KB 的二进制文件大小和约 1.5 KB 的 DRAM 使用量。

    请注意，以上空间节省的前提条件是 UART/USB-Serial-JTAG 驱动代码未被应用程序引用。如果因为其他用途使用了这些驱动程序，则节省效果会有所降低。


引导加载程序大小
------------------------------

本文档仅涉及 ESP-IDF 应用程序的二进制文件大小，而不涉及 ESP-IDF :ref:`second-stage-bootloader`。

关于 ESP-IDF 引导加载程序二进制文件大小的讨论，请参阅 :ref:`bootloader-size`。

IRAM 二进制文件大小
------------------------------------

如果二进制文件的 IRAM 部分过大，可以通过减少 IRAM 使用来解决这个问题，参阅 :ref:`optimize-iram-usage`。
