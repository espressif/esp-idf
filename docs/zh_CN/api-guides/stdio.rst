标准 I/O 与控制台输出
========================

:link_to_translation:`en:[English]`

ESP-IDF 提供了 C 标准输入输出功能，如 ``stdin``、``stdout`` 和 ``stderr`` 流，以及操作这些流的 C 标准库函数，如 ``printf()``。

类似 POSIX 系统，这些流是文件描述符的缓冲封装：

- ``stdin`` 用于读取用户输入的缓冲流，封装了文件描述符 ``STDIN_FILENO`` (0)。
- ``stdout`` 用于向用户写入输出的缓冲流，封装了文件描述符 ``STDOUT_FILENO`` (1)。
- ``stderr`` 用于向用户写入错误信息的缓冲流，封装了文件描述符 ``STDERR_FILENO`` (2)。

在 ESP-IDF 中， ``stdout`` 与 ``stderr`` 没有实际区别，因为两者都发送到相同的物理接口。大多数应用程序通常只使用 ``stdout``。例如，对于 ESP-IDF 的日志函数，无论日志等级如何，始终写入 ``stdout``。

底层的 `stdin`、`stdout` 和 `stderr` 文件描述符是基于 :doc:`VFS 驱动 <../api-reference/storage/vfs>` 实现的。

在 {IDF_TARGET_NAME} 上，ESP-IDF 提供了用于以下 I/O 接口的 VFS 驱动实现：

.. list::

    - UART
    :SOC_USB_SERIAL_JTAG_SUPPORTED: - USB Serial/JTAG
    :SOC_USB_OTG_CONSOLE_SUPPORTED: - USB CDC（使用 USB_OTG 外设）
    - "Null"（无输出）

不过，标准 I/O 并不限于这些选项。请参见下文，了解如何为标准 I/O 启用自定义目标。

.. note::

    控制台通路与通过 :doc:`UART 驱动 <../api-reference/peripherals/uart>` 在其他管脚上打开的应用 UART 彼此独立。若控制台输出由 UART 外设提供，则需要 USB 至 UART 桥芯片，许多开发板上已板载该桥芯片，用于 PC 与默认 UART0 之间的控制台通信；若控制台输出由 USB 或 USB Serial/JTAG 外设提供，则无需转接芯片，开发板上的 USB 接口直连芯片的 USB D+/D- 管脚。除非将某端口配置为控制台，否则其他 UART 端口上的数据不会出现在 ``idf.py monitor`` 中。有关详细信息，请参阅 :doc:`与 {IDF_TARGET_NAME} 创建串口连接 <../get-started/establish-serial-connection>`。

配置
-----

通过若干 Kconfig 选项，可以选择内置的标准 I/O 实现：

.. list::

    - :menuitem:`CONFIG_ESP_CONSOLE_UART_DEFAULT` — 启用 UART 用于标准 I/O，保持默认选项项（管脚号、波特率）。
    - :menuitem:`CONFIG_ESP_CONSOLE_UART_CUSTOM` — 启用 UART 用于标准 I/O，通过 Kconfig 配置 TX/RX 管脚号和波特率。
    :SOC_USB_OTG_CONSOLE_SUPPORTED: - :menuitem:`CONFIG_ESP_CONSOLE_USB_CDC` — 启用 USB CDC（使用 USB_OTG 外设）用于标准 I/O。硬件连接要求请参见 :doc:`usb-otg-console`。
    :SOC_USB_SERIAL_JTAG_SUPPORTED: - :menuitem:`CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG` — 启用 USB Serial/JTAG 用于标准 I/O。硬件连接要求请参见 :doc:`usb-serial-jtag-console`。
    - :menuitem:`CONFIG_ESP_CONSOLE_NONE` — 禁用标准 I/O。选择此选项时， ``stdin``、 ``stdout`` 和 ``stderr`` 将映射到 ``/dev/null``，不会产生输出或输入。

启用上述任一选项，会将相应的 VFS 驱动编译进应用程序，用于打开 ``stdin``、 ``stdout`` 和 ``stderr`` 流。写入 ``stdout`` 和 ``stderr`` 的数据将通过选定接口发送，而来自选定接口的输入将可通过 ``stdin`` 读取。

.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

    次级输出
    ^^^^^^^^

    ESP-IDF 内置支持，能将标准输出发送到次级目标。此选项可使应用输出同时显示在两个接口上，例如同时显示在 UART 和 USB Serial/JTAG 上。

    注意，次级控制台仅支持输出：

        - 应用程序写入 ``stdout`` 和 ``stderr`` 的数据将发送到主、次控制台
        - ``stdin`` 仅包含主机发送到主控制台的数据

    可用的次级控制台选项包括：

        - :menuitem:`CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG`

    次级控制台相当于下文介绍的运行时辅助输出端，在编译时即已固定，并且仅用于输出。

标准流与 FreeRTOS 任务
-----------------------

ESP-IDF 根据 :menuitem:`CONFIG_LIBC` 中所选择的 LibC 实现，提供了两种不同的标准 I/O 流实现方式。 ``stdin``、 ``stdout`` 和 ``stderr`` 流在这两种实现下的行为存在差异，尤其体现在 FreeRTOS 任务之间的共享方式上。

两种实现的共同点是，每个流 (``stdin``、 ``stdout``、 ``stderr``) 都具有一个互斥锁 (mutex) 用于保护，防止多个任务并发访问同一个流。例如，当两个任务同时向 ``stdout`` 写数据时，该互斥锁可以确保各个任务的输出不会相互混杂。

Newlib
^^^^^^

在 ESP-IDF 中，为节省 RAM， ``stdin`` 、 ``stdout`` 和 ``stderr`` 的 ``FILE`` 对象在所有 FreeRTOS 任务间共享，但每个任务具有唯一的指针。这就说明：

- 可以为某个任务单独更改 ``stdin``、 ``stdout`` 和 ``stderr`` 而不影响其他任务。例如 ``stdin = fopen("/dev/uart/1", "r")``。
- 若要更改新任务的默认 ``stdin``, ``stdout``, ``stderr`` 流，请在创建任务前修改 ``_GLOBAL_REENT->_stdin`` (``_stdout``, ``_stderr``)。
- 使用 ``fclose`` 关闭默认 ``stdin``、 ``stdout`` 或 ``stderr`` 时也会关闭 ``FILE`` 流对象，并影响所有其他任务。

Picolibc
^^^^^^^^

根据 POSIX 标准，默认的 ``stdin``、 ``stdout`` 和 ``stderr`` 流是全局的，在所有 FreeRTOS 任务之间共享。这意味着：

- 修改 ``stdin``、 ``stdout`` 或 ``stderr`` 会影响所有其他任务，无法为特定任务单独更改标准 I/O 流。
- 对于需要线程本地 (thread-local) 的流，应在应用程序代码中通过打开文件流并在任务中使用它来实现，例如 ``fscanf()``、 ``fprintf()`` 等。

阻塞与非阻塞 I/O
-----------------

UART
^^^^

默认情况下，UART VFS 使用简化函数进行读写。所有数据进入 UART FIFO 之前，写操作都会忙等待；读操作为非阻塞，只返回 FIFO 中的现有数据。由于这种非阻塞读取操作，高级 C 库调用时（如 ``fscanf("%d\n", &var);``）可能无法得到预期结果。

使用 UART 驱动的应用程序，可通过调用 :cpp:func:`uart_vfs_dev_use_driver` 来使用该驱动提供的中断驱动型阻塞读写函数。也可通过 :cpp:func:`uart_vfs_dev_use_nonblocking` 回退到基本的非阻塞函数。

安装中断驱动后，也可使用带有 ``O_NONBLOCK`` 标志的 ``fcntl`` 函数来启用/禁用非阻塞行为。

.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

    USB Serial/JTAG
    ^^^^^^^^^^^^^^^

    与 UART 类似，USB Serial/JTAG VFS 驱动默认使用简化实现：写操作阻塞（忙等待直到所有数据发送完成），读操作非阻塞，只返回 FIFO 中的数据。可通过调用 :cpp:func:`usb_serial_jtag_vfs_use_nonblocking` 使用中断驱动阻塞读写函数，将此行为更改为使用 USB Serial/JTAG 驱动程序的中断驱动和阻塞式读写函数。注意，USB Serial/JTAG 驱动需先使用 :cpp:func:`usb_serial_jtag_driver_install` 初始化。也可通过相应函数回退到基本非阻塞函数。

    安装中断驱动后，也可使用带有 ``O_NONBLOCK`` 标志的 ``fcntl`` 函数启用/禁用非阻塞行为。

.. only:: SOC_USB_OTG_CONSOLE_SUPPORTED

    USB CDC（使用 USB_OTG 外设）
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    USB CDC VFS 驱动默认提供阻塞 I/O。带有 ``O_NONBLOCK`` 标志的 ``fcntl`` 函数启用非阻塞行为。

换行符转换
------------

虚拟文件系统 (VFS) 驱动程序为输入和输出提供了可选的换行转换功能。在内部，大多数应用程序发送和接收以 LF (``\n``) 字符结尾的行。不同的终端程序可能需要不同的行终止符，例如 CR 或 CRLF。

应用程序可以使用以下 Kconfig 选项全局配置此行为：

    - 配置输出
        - :menuitem:`CONFIG_LIBC_STDOUT_LINE_ENDING_CRLF`
        - :menuitem:`CONFIG_LIBC_STDOUT_LINE_ENDING_CR`
        - :menuitem:`CONFIG_LIBC_STDOUT_LINE_ENDING_LF`
    - 配置输入
        - :menuitem:`CONFIG_LIBC_STDIN_LINE_ENDING_CRLF`
        - :menuitem:`CONFIG_LIBC_STDIN_LINE_ENDING_CR`
        - :menuitem:`CONFIG_LIBC_STDIN_LINE_ENDING_LF`


也可以为特定 VFS 驱动配置行结束符转换：

.. list::

    - 对于 UART：:cpp:func:`uart_vfs_dev_port_set_rx_line_endings` 和 :cpp:func:`uart_vfs_dev_port_set_tx_line_endings`
    :SOC_USB_SERIAL_JTAG_SUPPORTED: - 对于 USB Serial/JTAG：:cpp:func:`usb_serial_jtag_vfs_set_rx_line_endings` 和 :cpp:func:`usb_serial_jtag_vfs_set_tx_line_endings`
    :SOC_USB_OTG_CONSOLE_SUPPORTED: - 对于 USB CDC（使用 USB_OTG 外设）：:cpp:func:`esp_vfs_dev_cdcacm_set_rx_line_endings` 和 :cpp:func:`esp_vfs_dev_cdcacm_set_tx_line_endings`

缓冲
-----

默认情况下，标准 I/O 流是按行缓冲的。这意味着写入流的数据在写入换行符或缓冲区满之前不会发送到底层设备。例如，调用 ``printf("Hello")`` 时，文本不会立即发送到 UART；只有当你调用 ``printf("\n")`` 或者由于其他打印操作导致 stream 缓冲被填满时，文本才会真正发送。

可使用 ``setvbuf()`` 函数更改此行为。例如，禁用 ``stdout`` 缓冲：

.. code-block:: c

    setvbuf(stdout, NULL, _IONBF, 0);

也可以使用 ``setvbuf()`` 增大缓冲区大小或切换到完全缓冲模式。

标准 I/O 的自定义通道
---------------------

若要将应用输出发送到自定义通道（如 WebSocket 连接），可以创建自定义 VFS 驱动。详见 :doc:`VFS 文档 <../api-reference/storage/vfs>`。自定义 VFS 驱动必须至少实现以下函数：

    - ``open()`` 和 ``close()``
    - ``write()``
    - ``read()`` — 仅当自定义通道也用于输入
    - ``fstat()`` — 推荐，实现正确的 I/O 流缓冲行为
    - ``fcntl()`` — 仅当必须支持非阻塞 I/O

创建自定义 VFS 驱动后，使用 :cpp:func:`esp_vfs_register_fs()` 注册，然后用 ``fopen()`` 将 ``stdout`` 和 ``stderr`` 重定向到自定义通道。例如：

.. code-block:: c

    FILE *f = fopen("/dev/mychannel", "w");
    if (f == NULL) {
        // 在此处处理错误
    }
    stdout = f;
    stderr = f;

注意，日志函数（如 ``ESP_LOGE()``）会将其输出写入 ``stdout``。在自定义 VFS（或其调用的组件）实现中使用日志时，请尤为注意。例如，如果自定义 VFS 驱动的 ``write()`` 操作失败，并使用 ``ESP_LOGE()`` 记录错误，会将输出写入 ``stdout``，而这又会调用自定义 VFS 驱动程序的 ``write()`` 操作，可能导致无限循环。建议在 VFS 驱动的 ``write()`` 实现中跟踪此类情况，如果发现写入仍在进行中，则立即返回。

控制台 I/O 多路复用器
---------------------

除了手动重定向 ``stdout`` 和 ``stderr`` 外（如上文所示），ESP-IDF 还提供控制台 I/O 多路复用器，使应用程序可以注册一个或多个自定义 VFS 后端，并在运行时在这些后端之间切换，而无需自行重新指定 ``stdin``、``stdout`` 或 ``stderr`` 流。

多路复用器以 ``/dev/console`` 的形式对外提供，并将后端分为两类管理：

- **主控 (Primary)**：当前活跃的读写后端。应用程序的输入 (``stdin``) 和输出 (``stdout``/``stderr``) 都通过该后端。任一时刻只有一个后端作为主控。使用 :cpp:func:`esp_stdio_set_primary_driver` 选择主控；该调用会返回此前活跃的主控，应用程序可在稍后将其恢复。此前的主控不会被移除，而是保持注册状态，并转为只写辅助后端。Kconfig 选择的默认控制台会在启动时自动注册，并作为初始主控。
- **辅助 (Auxiliary)**：只写输出端。写入 ``stdout`` 和 ``stderr`` 的每个字节除了发送到主控外，还会分发到所有已注册的辅助后端。例如，可以在物理接口上保留交互式控制台，同时将控制台输出镜像到日志文件或网络连接。

使用 :cpp:func:`esp_stdio_register_driver` 添加后端（最初作为辅助后端），使用 :cpp:func:`esp_stdio_unregister_driver` 移除后端。当前作为主控的后端无法注销，必须先使用 :cpp:func:`esp_stdio_set_primary_driver` 将另一个后端提升为主控。

允许同时注册的后端数量（主控和所有辅助后端，包括 Kconfig 默认项）由 :ref:`CONFIG_ESP_STDIO_MAX_VFS_ENTRIES` 配置。

多路复用器 API 在 ``esp_stdio.h`` 中声明，启用 ``CONFIG_VFS_SUPPORT_IO`` 后即可使用。有关各调用的完整行为，请参阅下文的 :ref:`API 参考 <esp-stdio-api-reference>`。

示例
^^^^

:example:`system/esp_stdio/runtime_registration` 示例展示了完整流程：注册自定义后端，将其设为当前控制台，随后恢复此前的控制台。

上文关于日志的重入说明，此处同样适用。后端的 ``write()`` 实现不应触发会递归写回控制台的日志。

.. _esp-stdio-api-reference:

API 参考
^^^^^^^^

.. include-build-file:: inc/esp_stdio.inc
