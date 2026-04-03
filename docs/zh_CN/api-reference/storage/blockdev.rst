块设备层
========

:link_to_translation:`en:[English]`

概述
----

块设备层 (BDL) 定义了一套 C 语言接口，使面向存储的组件可在无需专用适配器的情况下完成数据交换。每个块设备以 :cpp:type:`esp_blockdev_handle_t` 句柄的形式对外提供该接口，通过该句柄可访问设备标志、几何信息以及 ``components/esp_blockdev/include/esp_blockdev.h`` 中定义的一组受支持操作。上层代码可读取这些元数据，并调用可用的回调执行 I/O 操作。

统一的接口支持串联多个通用组件，灵活组合成可适配各类存储场景的 BDL 协议栈。驱动程序提供用于访问物理设备的句柄；中间件组件使用该句柄并扩展其能力（如划分设备空间、添加磨损均衡功能），同时向下一层暴露新的句柄。协议栈最顶层的组件（如文件系统）是纯设备使用者。只要每一层均遵循下文约定的接口规范，该模型即可实现文件系统、中间件与物理驱动的任意组合使用。

.. blockdiag::
    :caption: 块设备层堆栈示例
    :align: center

    blockdiag blockdev-stack {
        default_fontsize = 14;
        node_height = 60;
        orientation = portrait;
        default_group_color = none;

        nvs [label = "NVS\n(nvs_flash)"];
        fatfs [label = "FATFS\n(fatfs)"];
        littlefs [label = "LittleFS\n(esp_littlefs)"];
        consumer [label = "Block device\nconsumers", shape = ellipse];

        wl [label = "Wear Levelling\n(wear_levelling)"];
        middleware_1 [label = "Block device\nmiddleware", shape = ellipse];

        nvs_part [label = "NVS Partition\n(esp_partition)"];
        littlefs_part [label = "LittleFS Partition\n(esp_partition)"];
        fat_part [label = "FAT Partition\n(esp_partition)"];
        middleware_2 [label = "Block device\nmiddleware", shape = ellipse];

        spi [label = "Flash\n(spi_flash)"];
        provider [label = "Block device\nprovider", shape = ellipse];

        d1 [shape = none, width = 1, height = 1];
        d2 [shape = none, width = 1, height = 1];
        d3 [shape = none, width = 1, height = 1];

        nvs -> nvs_part -> spi;
        fatfs -> wl -> fat_part -> spi;
        littlefs -> littlefs_part -> spi;

        consumer -> middleware_1 -> middleware_2 -> provider;

        group { orientation = landscape; fatfs; littlefs; nvs; }
        group { orientation = landscape; wl; }
        group { orientation = landscape; fat_part; littlefs_part; nvs_part; }
        group { orientation = landscape; d3; spi; }
    }


使用块设备
----------

句柄
^^^^

块设备通过 :cpp:type:`esp_blockdev_handle_t` 进行访问。可以从对应的组件获取该句柄，遵循 ``<component>_get_blockdev()`` 的命名约定；当设备不再使用时，必须调用对应的 ``<component>_release_blockdev()`` 辅助函数来释放该句柄。注意，应该将句柄视为黑盒对象：只能通过 ``components/esp_blockdev/include/esp_blockdev.h`` 中提供的公开 API 来使用，不要移动或修改其所引用的内存。

几何与标志
^^^^^^^^^^

每个设备会发布一个 :cpp:type:`esp_blockdev_geometry_t` 结构体，用于报告容量以及最小读、写、擦除粒度。可选的推荐大小可作为性能提示，但不能替代针对强制取值的对齐检查。配套的 :cpp:type:`esp_blockdev_flags_t` 结构体声明只读介质、加密或先擦后写等属性。中间件可以改变表观几何大小，但在创建时必须验证底层是否满足其要求，并确保后续对底层设备的访问始终符合其约束。

操作
^^^^

:cpp:type:`esp_blockdev_ops_t` 结构体定义读、写、擦除、同步、ioctl 与释放等回调。在调用回调之前，调用方必须确保对应函数指针非 ``NULL``； ``NULL`` 表示不支持该操作。调用方有责任根据几何数据校验对齐与边界，并遵守由标识位指定的约束要求（例如在类 NAND 介质上写之前先执行擦除）。

典型流程
^^^^^^^^

1. 从驱动程序或中间件提供商处获取一个句柄。
2. 检查几何结构和标志，以确定所需的对齐方式、可用容量和特殊处理方式。
3. 通过提供商公开的操作表发出读取、写入、擦除和同步请求。
4. 将句柄转发给上层的组件，或者在所有操作完成后释放该句柄。

示例
^^^^

.. code-block:: c

   esp_blockdev_handle_t dev = my_component_get_blockdev();
   const esp_blockdev_geometry_t *geometry = dev->geometry;
   if (dev->ops->read && (sizeof(buffer) % geometry->read_size) == 0) {
       ESP_ERROR_CHECK(dev->ops->read(dev, buffer, sizeof(buffer), 0, sizeof(buffer)));
   }
   if (dev->ops->release) {
       ESP_ERROR_CHECK(dev->ops->release(dev));
   }

约定
----

标志（:cpp:type:`esp_blockdev_flags_t`）
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* 标志在设备创建时初始化一次，且在句柄整个生命周期内必须保持不变。
* ``read_only`` 要求写、擦除及会改变状态的 ioctl 命令失败，并返回诸如 :c:macro:`ESP_ERR_INVALID_STATE` 的错误。
* ``encrypted`` 表示介质上的数据已加密；上层不得假定可见明文或透明映射。
* ``erase_before_write`` 告知调用方：成功写入前必须先擦除目标范围。若在未插入擦除操作的情况下对同一范围多次写入，行为未定义，但很可能导致数据损坏。
* ``and_type_write`` 表示 NAND/NOR 风格行为：编程仅将位清为 0（1→0），实际存储 ``existing_bits & new_bits``。已为 0 的位在写请求写入 1 时仍保持为 0；只有先擦除才能恢复为 1。
* ``default_val_after_erase`` 标识擦除后区域读回为 ``0x00`` 还是 ``0xFF``，以便中间件保持哨兵值一致。

几何参数 (:cpp:type:`esp_blockdev_geometry_t`)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* ``disk_size`` 为可访问的总容量（字节）；任何结束偏移量超出该值的请求都必须被拒绝。
* ``read_size``、 ``write_size`` 与 ``erase_size`` 为强制对齐单位（字节）；在执行操作前，偏移与长度都必须和相应的大小对齐。
* 调用方遵循推荐大小时可提高吞吐量，但不能替代最低对齐检查；实现方必须接受所有符合强制粒度要求的请求。
* 当用户看到同一底层设备的可读写与只读两种变体时，除 ``read_only`` 标志外，两者的几何参数必须相同。特别是 ``read_size``、 ``write_size`` 与 ``erase_size`` 应保持一致； ``recommended_*`` 也应该保持一致，除非有特别的需要（此类情况应在文档中说明）。

操作 (:cpp:type:`esp_blockdev_ops_t`)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``read(dev_handle, dst_buf, dst_buf_size, src_addr, data_read_len)``
  * 行为：成功时，将恰好 ``data_read_len`` 字节数据复制到 ``dst_buf``。
  * 前置条件： ``dst_buf`` 有效， ``src_addr`` 与 ``data_read_len`` 按 ``read_size`` 对齐， ``src_addr + data_read_len <= disk_size``；且 ``data_read_len <= dst_buf_size``。
  * 后置条件：复制成功则返回 :c:macro:`ESP_OK`，失败则透传相应的 ``ESP_ERR_*`` 错误码。

``write(dev_handle, src_buf, dst_addr, data_write_len)``
  * 前置条件：设备非 ``read_only``； ``src_buf`` 至少覆盖 ``data_write_len`` 字节；偏移与长度按 ``write_size`` 对齐且在 ``disk_size`` 范围内。
  * 行为：若设置了 ``erase_before_write``，调用方必须先执行 ``erase``。若设置了 ``and_type_write``，硬件会将新内容与现有内容进行按位与运算 (AND)，存储结果变为 ``old_value & new_value``；除非先擦除该范围，否则先前写入操作清除的位将保持清除状态。
  * 后置条件：设备接受请求的范围后，将返回 :c:macro:`ESP_OK` （在 ``sync`` 运行之前，数据可能仍驻留在中间缓冲区中）。对齐错误、超出范围或只读尝试应会显现 :c:macro:`ESP_ERR_INVALID_ARG` 或 :c:macro:`ESP_ERR_INVALID_STATE`，且实现方必须避免使该范围处于部分更新状态。
  * 说明：在具有 ``and_type_write`` 的设备上，写操作依赖于现有内容，因此在写之后立即读取相同（或重叠）范围时，可能需要先执行 ``sync``，以确保缓存数据反映完全合并后的值。

``erase(dev_handle, start_addr, erase_len)``
  * 前置条件：设备允许擦除； ``start_addr`` 与 ``erase_len`` 按 ``erase_size`` 对齐；范围在 ``disk_size`` 内。
  * 后置条件：成功时该范围读回为 ``default_val_after_erase``。未对齐或越界请求应返回 :c:macro:`ESP_ERR_INVALID_ARG`；硬件故障应通过驱动相关的 ``ESP_ERR_*`` 码向上传递。

``sync(dev_handle)``
  * 刷新挂起的写入操作。省略此回调的设备将采用直写语义运行。
  * 后置条件：在返回 :c:macro:`ESP_OK` 之前，所有先前已报告的写操作均到达稳定存储（包括所有底层设备）。超时或传输问题应表现为 :c:macro:`ESP_ERR_TIMEOUT` 或其他相关 ``ESP_ERR_*``。

``ioctl(dev_handle, cmd, args)``
  * 命令标识 ``0x00–0x7F`` 保留给 ESP-IDF 系统使用； `0x80–0xFF` 可供用户自定义扩展。
  * 每个命令定义各自的载荷布局；由于 ``args`` 为 ``void *`` 类型，封装层只能对其理解的命令校验或重新解释缓冲区，否则必须将载荷视为不透明数据。
  * 无法处理某命令的封装层应在合适时机将其原样转发给协议栈中的下一设备；仅最底层设备应对无法识别的命令返回 :c:macro:`ESP_ERR_NOT_SUPPORTED`。
  * 当协议栈存在非透明地址映射时，转发嵌入原始地址的命令本质上不安全：中间层无法翻译不透明载荷，因此行为未定义且通常会失败。此类命令应被显式拦截，或在堆叠配置中注明为不支持。

``release(dev_handle)``
  * 可选的析构函数，用于释放设备资源。该函数必须具有幂等性，即确保重复调用时要么成功，要么返回诸如 :c:macro:`ESP_ERR_INVALID_STATE` 等无害错误。

错误处理
^^^^^^^^

回调在成功时返回 :c:macro:`ESP_OK`，并应原样透传 ``ESP_ERR_*`` 错误码以帮助调用方诊断错误。中间件与应用程序须透传底层设备的错误，而不是在协议栈内掩盖这些错误。 ``NULL`` 函数指针视为“不支持该操作”。

验证
^^^^

实现方应包含覆盖对齐检查、由标志驱动的行为（只读、先擦后写、NAND 风格写）以及错误在堆叠设备间正确透传等测试。封装较低层句柄的中间件还必须验证句柄生命周期管理在整个协议栈中保持一致。

.. _blockdev-apis:

API 参考
--------

.. include-build-file:: inc/esp_blockdev.inc
