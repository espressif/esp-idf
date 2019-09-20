虚拟文件系统组件
============================

:link_to_translation:`en:[English]`

概述
--------

虚拟文件系统 (VFS) 组件可为一些驱动提供一个统一接口。有了该接口，用户可像操作普通文件一样操作虚拟文件。这类驱动程序可以是 FAT、SPIFFS 等真实文件系统，也可以是有文件类接口的设备驱动程序。

VFS 组件支持 C 库函数（如 fopen 和 fprintf 等）与文件系统 (FS) 驱动程序协同工作。在高层级，每个 FS 驱动程序均与某些路径前缀相关联。当一个 C 库函数需要打开文件时，VFS 组件将搜索与该文件所在文件路径相关联的 FS 驱动程序，并将调用传递给该驱动程序。针对该文件的读取、写入等其他操作的调用也将传递给这个驱动程序。

例如，您可以使用 ``/fat`` 前缀注册 FAT 文件系统驱动，之后即可调用 ``fopen("/fat/file.txt", "w")``。之后，VFS 将调用 FAT 驱动的 ``open`` 函数，并将参数 ``/file.txt`` 和合适的打开模式传递给 ``open`` 函数；后续对返回的 ``FILE*`` 数据流调用 C 库函数也同样会传递给 FAT 驱动。

注册 FS 驱动程序
---------------------

如需注册 FS 驱动程序，首先要定义一个 :cpp:type:`esp_vfs_t` 结构体实例，并用指向 FS API 的函数指针填充它。

.. highlight:: c

::

    esp_vfs_t myfs = {
        .flags = ESP_VFS_FLAG_DEFAULT,
        .write = &myfs_write,
        .open = &myfs_open,
        .fstat = &myfs_fstat,
        .close = &myfs_close,
        .read = &myfs_read,
    };

    ESP_ERROR_CHECK(esp_vfs_register("/data", &myfs, NULL));

在上述代码中需要用到 ``read``、 ``write`` 或 ``read_p``、 ``write_p``，具体使用哪组函数由 FS 驱动程序 API 的声明方式决定。

示例 1：声明 API 函数时不带额外的上下文指针参数，即 FS 驱动程序为单例模式，此时使用 ``write`` ::

    ssize_t myfs_write(int fd, const void * data, size_t size);

    // In definition of esp_vfs_t:
        .flags = ESP_VFS_FLAG_DEFAULT,
        .write = &myfs_write,
    // ... other members initialized

    // When registering FS, context pointer (third argument) is NULL:
    ESP_ERROR_CHECK(esp_vfs_register("/data", &myfs, NULL));

示例 2：声明 API 函数时需要一个额外的上下文指针作为参数，即可支持多个 FS 驱动程序实例，此时使用 ``write_p`` ::

    ssize_t myfs_write(myfs_t* fs, int fd, const void * data, size_t size);

    // In definition of esp_vfs_t:
        .flags = ESP_VFS_FLAG_CONTEXT_PTR,
        .write_p = &myfs_write,
    // ... other members initialized

    // When registering FS, pass the FS context pointer into the third argument
    // (hypothetical myfs_mount function is used for illustrative purposes)
    myfs_t* myfs_inst1 = myfs_mount(partition1->offset, partition1->size);
    ESP_ERROR_CHECK(esp_vfs_register("/data1", &myfs, myfs_inst1));

    // Can register another instance:
    myfs_t* myfs_inst2 = myfs_mount(partition2->offset, partition2->size);
    ESP_ERROR_CHECK(esp_vfs_register("/data2", &myfs, myfs_inst2));

同步输入/输出多路复用
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

如需通过 :cpp:func:`select` 使用同步输入/输出多路复用，首先需要把 :cpp:func:`start_select` 和 :cpp:func:`end_select` 注册到 VFS，如下所示：

.. highlight:: c

::

    // In definition of esp_vfs_t:
        .start_select = &uart_start_select,
        .end_select = &uart_end_select,
    // ... other members initialized

调用 :cpp:func:`start_select` 设置环境，用以检测某一 VFS 文件描述符的读取/写入/错误条件。调用 :cpp:func:`end_select` 终止、析构或释放 :cpp:func:`start_select` 设置的资源。请在 :component_file:`vfs/vfs_uart.c` 中查看 UART 外设参考实现、:cpp:func:`esp_vfs_dev_uart_register`、:cpp:func:`uart_start_select` 和 :cpp:func:`uart_end_select` 函数。

请参考以下示例，查看如何使用 VFS 文件描述符调用 :cpp:func:`select`：

- :example:`peripherals/uart_select`
- :example:`system/select`

如果 :cpp:func:`select` 用于套接字文件描述符，您可以启用 :envvar:`CONFIG_LWIP_USE_ONLY_LWIP_SELECT` 选项来减少代码量，提高性能。

路径
-----

已注册的 FS 驱动程序均有一个路径前缀与之关联，此路径前缀即为分区的挂载点。

如果挂载点中嵌套了其他挂载点，则在打开文件时使用具有最长匹配路径前缀的挂载点。例如，假设以下文件系统已在 VFS 中注册：

- 在 /data 下注册 FS 驱动程序 1
- 在 /data/static 下注册 FS 驱动程序 2

那么：

- 打开 ``/data/log.txt`` 会调用驱动程序 FS 1；
- 打开 ``/data/static/index.html`` 需调用 FS 驱动程序 2；
- 即便 FS 驱动程序 2 中没有 ``/index.html``，也不会在 FS 驱动程序 1 中查找 ``/static/index.html``。

挂载点名称必须以路径分隔符 (``/``) 开头，且分隔符后至少包含一个字符。但在以下情况中，VFS 同样支持空的挂载点名称：1. 应用程序需要提供一个”最后方案“下使用的文件系统；2. 应用程序需要同时覆盖 VFS 功能。如果没有与路径匹配的前缀，就会使用到这种文件系统。

VFS 不会对路径中的点 (``.``) 进行特殊处理，也不会将 ``..`` 视为对父目录的引用。在上述示例中，使用 ``/data/static/../log.txt`` 路径不会调用 FS 驱动程序 1 打开 ``/log.txt``。特定的 FS 驱动程序（如 FATFS）可能以不同的方式处理文件名中的点。

执行打开文件操作时，FS 驱动程序仅得到文件的相对路径（挂载点前缀已经被去除）：

1. 以 ``/data`` 为路径前缀注册 ``myfs`` 驱动；
2. 应用程序调用 ``fopen("/data/config.json", ...)``；
3. VFS 调用 ``myfs_open("/config.json", ...)``；
4. ``myfs`` 驱动打开 ``/config.json`` 文件。

VFS 对文件路径长度没有限制，但文件系统路径前缀受 ``ESP_VFS_PATH_MAX`` 限制，即路径前缀上限为 ``ESP_VFS_PATH_MAX``。各个文件系统驱动则可能会对自己的文件名长度设置一些限制。


文件描述符
----------------

文件描述符是一组很小的正整数，从 ``0`` 到 ``FD_SETSIZE - 1``，``FD_SETSIZE`` 在 newlib ``sys/types.h`` 中定义。最大文件描述符由 ``CONFIG_LWIP_MAX_SOCKETS`` 定义，且为套接字保留。VFS 中包含一个名为 ``s_fd_table`` 的查找表，用于将全局文件描述符映射至 ``s_vfs`` 数组中注册的 VFS 驱动索引。


标准 IO 流 (stdin, stdout, stderr)
-------------------------------------------

如果 menuconfig 中 ``UART for console output`` 选项没有设置为 ``None``，则 ``stdin``、 ``stdout`` 和 ``stderr`` 将默认从 UART 读取或写入。UART0 或 UART1 可用作标准 IO。默认情况下，UART0 使用 115200 波特率，TX 管脚为 GPIO1，RX 管脚为 GPIO3。您可以在 menuconfig 中更改上述参数。

对 ``stdout`` 或 ``stderr`` 执行写入操作将会向 UART 发送 FIFO 发送字符，对 ``stdin`` 执行读取操作则会从 UART 接收 FIFO 中取出字符。

默认情况下，VFS 使用简单的函数对 UART 进行读写操作。在所有数据放进 UART FIFO 之前，写操作将处于 busy-wait 状态，读操处于非阻塞状态，仅返回 FIFO 中已有数据。由于读操作为非阻塞，高层级 C 库函数调用（如 ``fscanf("%d\n", &var);``）可能获取不到所需结果。

如果应用程序使用 UART 驱动，则可以调用 ``esp_vfs_dev_uart_use_driver`` 函数来指导 VFS 使用驱动中断、读写阻塞功能等。您也可以调用 ``esp_vfs_dev_uart_use_nonblocking`` 来恢复非阻塞函数。

VFS 还为输入和输出提供换行符转换功能（可选）。多数应用程序在程序内部发送或接收以 LF (''\n'') 结尾的行，但不同的终端程序可能需要不同的换行符，比如 CR 或 CRLF。应用程序可以通过 menuconfig 或者调用 ``esp_vfs_dev_uart_set_rx_line_endings`` 和 ``esp_vfs_dev_uart_set_tx_line_endings`` 为输入输出配置换行符。


标准流和 FreeRTOS 任务
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``stdin``、``stdout`` 和 ``stderr`` 的 ``FILE`` 对象在所有 FreeRTOS 任务之间共享，指向这些对象的指针分别存储在每个任务的 ``struct _reent`` 中。

预处理器把如下代码：

.. highlight:: c

::

    fprintf(stderr, "42\n");

解释为：

.. highlight:: c

::

    fprintf(__getreent()->_stderr, "42\n");

其中 ``__getreent()`` 函数将为每个任务返回一个指向 ``struct _reent`` 的指针 (:component_file:`newlib/include/sys/reent.h#L370-L417`)。每个任务的 TCB 均拥有一个 ``struct _reent`` 结构体，任务初始化后，``struct _reent`` 结构体中的 ``_stdin``、``_stdout`` 和 ``_stderr`` 将会被赋予 ``_GLOBAL_REENT`` 中 ``_stdin``、 ``_stdout`` 和 ``_stderr`` 的值，``_GLOBAL_REENT`` 即为 FreeRTOS 启动之前所用结构体。 

这样设计带来的结果是：

- 允许重定向给定任务的 ``stdin``、 ``stdout`` 和 ``stderr``，而不影响其他任务，例如通过 ``stdin = fopen("/dev/uart/1", "r")``；
- 但使用 ``fclose`` 关闭默认 ``stdin``、 ``stdout`` 或 ``stderr`` 将同时关闭相应的 ``FILE`` 流对象，因此会影响其他任务；
- 如需更改新任务的默认 ``stdin``、 ``stdout`` 和 ``stderr`` 流，请在创建新任务之前修改 ``_GLOBAL_REENT->_stdin`` (``_stdout``、``_stderr``)。
