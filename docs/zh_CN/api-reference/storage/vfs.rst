虚拟文件系统组件
============================

:link_to_translation:`en:[English]`

概述
--------

虚拟文件系统 (VFS) 组件为驱动程序提供一个统一接口，可以操作类文件对象。这类驱动程序可以是 FAT、SPIFFS 等真实文件系统，也可以是提供文件类接口的设备驱动程序。

VFS 组件支持 C 库函数（如 fopen 和 fprintf 等）与文件系统 (FS) 驱动程序协同工作。在高层级，每个 FS 驱动程序均与某些路径前缀相关联。当一个 C 库函数需要打开文件时，VFS 组件将搜索与该文件所在文件路径相关联的 FS 驱动程序，并将调用传递给该驱动程序。针对该文件的读取、写入等其他操作的调用也将传递给这个驱动程序。

例如，使用 ``/fat`` 前缀注册 FAT 文件系统驱动，之后即可调用 ``fopen("/fat/file.txt", "w")``。之后，VFS 将调用 FAT 驱动的 ``open`` 函数，并将参数 ``/file.txt`` 和合适的打开模式传递给 ``open`` 函数；后续对返回的 ``FILE*`` 数据流调用 C 库函数也同样会传递给 FAT 驱动。


注册 FS 驱动程序
---------------------

如需注册 FS 驱动程序，应用程序首先要定义一个 :cpp:type:`esp_vfs_t` 结构体实例，并用指向 FS API 的函数指针填充它。

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

VFS 组件支持通过 :cpp:func:`select` 进行同步输入/输出多路复用，其实现方式如下：

1. 调用 :cpp:func:`select`，使用时提供的文件描述符可以属于不同的 VFS 驱动。

2. 文件描述符被分为几组，每组属于一个 VFS 驱动。

3. 非套接字 VFS 驱动的文件描述符由 :cpp:func:`start_select` 移交给指定的 VFS 驱动，后文会对此进行详述。该函数代表指定驱动 :cpp:func:`select` 的实现。这是一个非阻塞的调用，意味着在设置好检查与指定文件描述符相关事件的环境后，该函数应该立即返回。

4. 套接字 VFS 驱动的文件描述符由 :cpp:func:`socket_select` 移交给套接字 VFS 驱动，后文会对此进行详述。这是一个阻塞调用，意味着只有当有一个与套接字文件描述符相关的事件或非套接字驱动发出信号让 :cpp:func:`socket_select` 退出时，它才会返回。

5. 从各个 VFS 驱动程序收集结果，并通过对事件检查环境取消初始化来终止所有驱动程序。

6. :cpp:func:`select` 调用结束并返回适当的结果。

非套接字 VFS 驱动
""""""""""""""""""""""

如果要使用非套接字 VFS 驱动的文件描述符调用 :cpp:func:`select`，那么需要用函数 :cpp:func:`start_select` 和 :cpp:func:`end_select` 注册该驱动，具体如下：

.. highlight:: c

::

    // In definition of esp_vfs_t:
        .start_select = &uart_start_select,
        .end_select = &uart_end_select,
    // ... other members initialized

调用 :cpp:func:`start_select` 函数可以设置环境，检测指定 VFS 驱动的文件描述符读取/写入/错误条件。

调用 :cpp:func:`end_select` 函数可以终止/取消初始化/释放由 :cpp:func:`start_select` 设置的环境。

.. note::
    在少数情况下，在调用 :cpp:func:`end_select` 之前可能并没有调用过 :cpp:func:`start_select`。因此 :cpp:func:`end_select` 的实现必须在该情况下返回错误而不能崩溃。

如需获取更多信息，请参考 :component_file:`esp_driver_uart/src/uart_vfs.c` 中 UART 外设的 VFS 驱动，尤其是函数 :cpp:func:`uart_vfs_dev_register`、:cpp:func:`uart_start_select` 和 :cpp:func:`uart_end_select`。

请参考以下示例，查看如何使用 VFS 文件描述符调用 :cpp:func:`select`：

- :example:`peripherals/uart/uart_select`
- :example:`system/select`


套接字 VFS 驱动
""""""""""""""""""""""

套接字 VFS 驱动会使用自实现的 :cpp:func:`socket_select` 函数，在读取/写入/错误条件时，非套接字 VFS 驱动会通知该函数。

可通过定义以下函数注册套接字 VFS 驱动：

.. highlight:: c

::

    // In definition of esp_vfs_t:
        .socket_select = &lwip_select,
        .get_socket_select_semaphore = &lwip_get_socket_select_semaphore,
        .stop_socket_select = &lwip_stop_socket_select,
        .stop_socket_select_isr = &lwip_stop_socket_select_isr,
    // ... other members initialized

函数 :cpp:func:`socket_select` 是套接字驱动对 :cpp:func:`select` 的内部实现。该函数只对套接字 VFS 驱动的文件描述符起作用。

:cpp:func:`get_socket_select_semaphore` 返回信号对象 (semaphore)，用于非套接字驱动程序中，以终止 :cpp:func:`socket_select` 的等待。

:cpp:func:`stop_socket_select` 通过传递 :cpp:func:`get_socket_select_semaphore` 函数返回的对象来终止 :cpp:func:`socket_select` 函数的等待。

:cpp:func:`stop_socket_select_isr` 与 :cpp:func:`stop_socket_select` 的作用相似，但是前者可在 ISR 中使用。

请参考 :component_file:`lwip/port/esp32xx/vfs_lwip.c` 以了解使用 LWIP 的套接字驱动参考实现。

.. note::

    如果 :cpp:func:`select` 用于套接字文件描述符，可以禁用 :ref:`CONFIG_VFS_SUPPORT_SELECT` 选项来减少代码量，提高性能。
    不要在 :cpp:func:`select` 调用过程中更改套接字驱动，否则会出现一些未定义行为。

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

文件描述符是一组很小的正整数，从 ``0`` 到 ``FD_SETSIZE - 1``，``FD_SETSIZE`` 定义在 ``sys/select.h``。最大文件描述符由 ``CONFIG_LWIP_MAX_SOCKETS`` 定义，且为套接字保留。VFS 中包含一个名为 ``s_fd_table`` 的查找表，用于将全局文件描述符映射至 ``s_vfs`` 数组中注册的 VFS 驱动索引。

标准 I/O 流（stdin、stdout、stderr）分别映射到文件描述符 0、1 和 2。有关标准 I/O 的更多信息，请参见 :doc:`../../api-guides/stdio`。

``eventfd()``
-------------

``eventfd()`` 是一个很强大的工具，可以循环通知基于 ``select()`` 的自定义事件。在 ESP-IDF 中， ``eventfd()`` 的实现大体上与 `man(2) eventfd <https://man7.org/linux/man-pages/man2/eventfd.2.html>`_ 中的描述相同，主要区别如下：

- 在调用 ``eventfd()`` 之前必须先调用 ``esp_vfs_eventfd_register()``；
- 标志中没有 ``EFD_CLOEXEC``、``EFD_NONBLOCK`` 和 ``EFD_SEMAPHORE`` 选项；
- ``EFD_SUPPORT_ISR`` 选项已经被添加到标志中。在中断处理程序中读取和写入 eventfd 需要这个标志。

注意，用 ``EFD_SUPPORT_ISR`` 创建 eventfd 将导致在读取、写入文件时，以及在设置这个文件的 ``select()`` 开始和结束时，暂时禁用中断。

应用示例
----------------

- :example:`system/eventfd` 使用了两个任务和一个定时器 ISR 回调函数演示了如何使用 ``eventfd()`` 在基于 ``select()`` 的主循环中收集任务和中断服务程序的事件。

- :example:`system/select` 演示了如何使用 ``select()`` 函数进行同步 I/O 多路复用，使用 UART 和套接字文件描述符，并将二者配置为回环模式，以接收来自其他任务发送的消息。

API 参考
-------------

.. include-build-file:: inc/esp_vfs.inc

.. include-build-file:: inc/esp_vfs_dev.inc

.. include-build-file:: inc/uart_vfs.inc

.. include-build-file:: inc/esp_vfs_eventfd.inc

.. include-build-file:: inc/esp_vfs_null.inc
