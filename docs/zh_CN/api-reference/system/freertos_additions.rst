FreeRTOS（附加功能）
================================

:link_to_translation:`en:[English]`

ESP-IDF 为 FreeRTOS 提供了多种附加功能。这些附加功能适用于 ESP-IDF 支持的所有 FreeRTOS 实现，即 ESP-IDF FreeRTOS 和 Amazon SMP FreeRTOS。本文档介绍了这些附加功能，内容包括以下几个部分：

.. contents:: 目录
    :depth: 2

.. ---------------------------------------------------- Overview -------------------------------------------------------

概述
--------

ESP-IDF 为 FreeRTOS 提供了以下附加功能：

- **环形 buffer**：FIFO 缓冲区，支持任意长度的数据项。
- **ESP-IDF tick 钩子和 idle 钩子**：ESP-IDF 提供了多个自定义的 tick 钩子和 idle 钩子，相较于 FreeRTOS，支持的钩子数量更多且更灵活。
- **线程本地存储指针 (TLSP) 删除回调**：当一个任务被删除时，TLSP 删除回调会自动运行，从而自动清理 TLSP。
- **IDF 附加 API**：专用于 ESP-IDF 的附加函数，用于增强 FreeRTOS 的功能。
- **组件专用功能**：目前只添加了一个专用于组件的功能，即 ``ORIG_INCLUDE_PATH``。

.. -------------------------------------------------- Ring buffers -----------------------------------------------------

环形 buffer
------------

FreeRTOS 提供了流 buffer 和消息 buffer，作为在任务和 ISR 之间发送任意大小数据的主要机制。然而，FreeRTOS 流 buffer 和消息 buffer 具有以下限制：

- 仅支持单一的发送者和单一的接收者
- 数据通过复制的方式进行传递
- 无法为延迟发送（即发送获取）预留 buffer 空间

为此，ESP-IDF 提供了一个单独的环形 buffer 来解决上述问题。

ESP-IDF 环形 buffer 是一个典型的 FIFO buffer，支持任意大小的数据项。在数据项大小可变的情况下，环形 buffer 比 FreeRTOS 队列更节约内存，可以替代 FreeRTOS 队列使用。环形 buffer 的容量不是由可以存储的数据项数量衡量的，而是由用于存储数据项的内存量来衡量的。

环形 buffer 提供了 API 来发送数据项，或为环形 buffer 中的数据项分配空间，以便进行手动填充。为提高效率， **数据项都通过引用的方式从环形 buffer 中检索出来**。因此，所有检索出的数据项也 **必须** 通过 :cpp:func:`vRingbufferReturnItem` 或 :cpp:func:`vRingbufferReturnItemFromISR` 返回到环形 buffer，以便将其从环形 buffer 中完全移除。

环形 buffer 分为以下三种类型：

**不可分割 buffer**：确保将一个数据项存储在连续的内存中，并且在任何情况下都不会尝试分割数据项。当数据项必须占用连续的内存时，请使用不可分割 buffer。 **仅不可分割 buffer 允许为延迟发送保留缓冲空间。** 更多信息请参考函数 :cpp:func:`xRingbufferSendAcquire` 和 :cpp:func:`xRingbufferSendComplete` 的文档。

**可分割 buffer**：当数据项在 buffer 末尾绕回时，如果 buffer 头部和尾部的总空间足够，则支持将一个数据项分成两部分进行存储。可分割 buffer 比不可分割 buffer 更节省内存，但在检索时可能会返回数据项的两个部分。

**字节 buffer**：不将数据存储为单独的数据项。所有数据都存储为字节序列，每次可以发送或检索任意大小的字节。当不需要单独维护数据项时，推荐使用字节 buffer，例如字节流。

.. note::

    不可分割 buffer 和可分割 buffer 在 32 位对齐地址上存储数据项。因此，在检索一个数据项时，数据项指针一定也是 32 位对齐的。这在向 DMA 发送数据时非常有用。

.. note::

    存储在不可分割或可分割 buffer 中的每个数据项 **需要额外的 8 字节用于标头**。数据项大小会向上取整为 32 位对齐大小，即 4 字节的倍数，实际的数据项大小则记录在标头中。不可分割和可分割 buffer 的大小在创建时也会向上取整。

使用方法
^^^^^^^^^^

以下示例演示了如何使用 :cpp:func:`xRingbufferCreate` 和 :cpp:func:`xRingbufferSend` 来创建环形 buffer，并向其发送数据项：

.. code-block:: c

    #include "freertos/ringbuf.h"
    static char tx_item[] = "test_item";

    ...

        //创建环形 buffer
        RingbufHandle_t buf_handle;
        buf_handle = xRingbufferCreate(1028, RINGBUF_TYPE_NOSPLIT);
        if (buf_handle == NULL) {
            printf("Failed to create ring buffer\n");
        }

        //发送一个数据项
        UBaseType_t res =  xRingbufferSend(buf_handle, tx_item, sizeof(tx_item), pdMS_TO_TICKS(1000));
        if (res != pdTRUE) {
            printf("Failed to send item\n");
        }

以下示例演示了如何使用 :cpp:func:`xRingbufferSendAcquire` 和 :cpp:func:`xRingbufferSendComplete` 代替 :cpp:func:`xRingbufferSend` 来获取环形 buffer（:cpp:enumerator:`RINGBUF_TYPE_NOSPLIT` 类型）上的内存，然后向其发送一个数据项。虽然增加了一个步骤，但可以实现获取要写入内存的地址，并自行写入内存。

.. code-block:: c

    #include "freertos/ringbuf.h"
    #include "soc/lldesc.h"

    typedef struct {
        lldesc_t dma_desc;
        uint8_t buf[1];
    } dma_item_t;

    #define DMA_ITEM_SIZE(N) (sizeof(lldesc_t)+(((N)+3)&(~3)))

    ...

        //为 DMA 描述符和相应的数据 buffer 检索空间
        //此步骤必须通过 SendAcquire 完成，否则，复制时地址可能会不同
        dma_item_t item;
        UBaseType_t res =  xRingbufferSendAcquire(buf_handle,
                            &item, DMA_ITEM_SIZE(buffer_size), pdMS_TO_TICKS(1000));
        if (res != pdTRUE) {
            printf("Failed to acquire memory for item\n");
        }
        item->dma_desc = (lldesc_t) {
            .size = buffer_size,
            .length = buffer_size,
            .eof = 0,
            .owner = 1,
            .buf = &item->buf,
        };
        //实际发送到环形 buffer 以供使用
        res = xRingbufferSendComplete(buf_handle, &item);
        if (res != pdTRUE) {
            printf("Failed to send item\n");
        }

以下示例演示了使用 :cpp:func:`xRingbufferReceive` 和 :cpp:func:`vRingbufferReturnItem` 从 **不可分割环形 buffer** 中检索和返回数据项：

.. code-block:: c

    ...

        //从不可分割环形 buffer 中接收一个数据项
        size_t item_size;
        char *item = (char *)xRingbufferReceive(buf_handle, &item_size, pdMS_TO_TICKS(1000));

        //Check received item
        if (item != NULL) {
            //Print item
            for (int i = 0; i < item_size; i++) {
                printf("%c", item[i]);
            }
            printf("\n");
            //返回数据项
            vRingbufferReturnItem(buf_handle, (void *)item);
        } else {
            //数据项检索失败
            printf("Failed to receive item\n");
        }


以下示例演示了使用 :cpp:func:`xRingbufferReceiveSplit` 和 :cpp:func:`vRingbufferReturnItem` 从 **可分割环形 buffer** 中检索和返回数据项：

.. code-block:: c

    ...

        //从可分割环形 buffer 中接收一个数据项
        size_t item_size1, item_size2;
        char *item1, *item2;
        BaseType_t ret = xRingbufferReceiveSplit(buf_handle, (void **)&item1, (void **)&item2, &item_size1, &item_size2, pdMS_TO_TICKS(1000));

        //检查收到的数据项
        if (ret == pdTRUE && item1 != NULL) {
            for (int i = 0; i < item_size1; i++) {
                printf("%c", item1[i]);
            }
            vRingbufferReturnItem(buf_handle, (void *)item1);
            //Check if item was split
            if (item2 != NULL) {
                for (int i = 0; i < item_size2; i++) {
                    printf("%c", item2[i]);
                }
                vRingbufferReturnItem(buf_handle, (void *)item2);
            }
            printf("\n");
        } else {
            //接收数据项失败
            printf("Failed to receive item\n");
        }


以下示例演示了使用 :cpp:func:`xRingbufferReceiveUpTo` 和 :cpp:func:`vRingbufferReturnItem` 从 **字节 buffer** 中检索和返回数据项：

.. code-block:: c

    ...

        //从字节 buffer 中接收数据
        size_t item_size;
        char *item = (char *)xRingbufferReceiveUpTo(buf_handle, &item_size, pdMS_TO_TICKS(1000), sizeof(tx_item));

        //检查接收到的数据
        if (item != NULL) {
            //Print item
            for (int i = 0; i < item_size; i++) {
                printf("%c", item[i]);
            }
            printf("\n");
            //返回数据项
            vRingbufferReturnItem(buf_handle, (void *)item);
        } else {
            //接收数据项失败
            printf("Failed to receive item\n");
        }


对于以上函数的 ISR 安全版本，请调用 :cpp:func:`xRingbufferSendFromISR`、 :cpp:func:`xRingbufferReceiveFromISR`、 :cpp:func:`xRingbufferReceiveSplitFromISR`、 :cpp:func:`xRingbufferReceiveUpToFromISR` 和 :cpp:func:`vRingbufferReturnItemFromISR`。

.. note::

    当字节在环形 buffer 的末端绕回时，需调用 ``RingbufferReceive[UpTo][FromISR]()`` 两次。

发送到环形 buffer
^^^^^^^^^^^^^^^^^^^^^^

以下图表将不可分割和可分割 buffer 与字节 buffer 进行对比，说明了三者在发送数据或数据项方面的差异。图表中，假设分别向 **128 字节的 buffer** 发送大小为 **18、3 和 27 字节** 的三个数据项：

.. packetdiag:: ../../../_static/diagrams/ring-buffer/ring_buffer_send_non_byte_buf.diag
    :caption: 向不可分割或可分割的环形 buffer 发送数据项
    :align: center

对于不可分割和可分割 buffer，每个数据项前都有 8 字节标头信息。此外，为了保持整体的 32 位对齐，每个数据项占用的空间都会 **向上取整到最接近的 32 位对齐大小**。数据项的实际大小会记录在标头中，并在检索数据项时返回。

参考上图，18、3 和 27 字节的数据项分别 **向上取整为 20、4 和 28 字节**，然后在每个数据项前面添加一个 8 字节的标头。

.. packetdiag:: ../../../_static/diagrams/ring-buffer/ring_buffer_send_byte_buf.diag
    :caption: 向字节 buffer 发送数据项
    :align: center

字节 buffer 将数据视为一个字节序列，不会产引入任何额外开销，不添加标头信息。因此，发送到字节 buffer 的所有数据都会合并成一个数据项。

参考上图，18、3 和 27 字节的数据项被顺序写入字节 buffer，并 **合并成一个 48 字节的数据项**。

使用 SendAcquire 和 SendComplete
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

不可分割 buffer 中的数据项严格按照 FIFO 顺序通过 ``SendAcquire`` 获取，并且必须通过 ``SendComplete`` 发送到 buffer 以便访问。也可以发送或获取多个数据项，且无需严格遵照获取顺序，但接收数据项却必须遵循 FIFO。所以，如果不为最早获取的数据项调用 ``SendComplete``，就无法接收后续数据项。

以下图表说明了当 ``SendAcquire`` 和 ``SendComplete`` 顺序不同时的情形。一开始，已经有一个 16 字节的数据项发送到环形 buffer。然后调用 ``SendAcquire`` 在环形 buffer 上获取 20、8、24 字节的空间。

.. packetdiag:: ../../../_static/diagrams/ring-buffer/ring_buffer_send_acquire_complete.diag
    :caption: 在不可分割环形 buffer 中 SendAcquire/SendComplete 数据项
    :align: center

然后填充 buffer，按照 8、24、20 字节的顺序通过 ``SendComplete`` 将数据项发送到环形 buffer。当 8 字节和 24 字节的数据发送后，仍只能获取 16 字节的数据项。因此，如果不为 20 字节数据项调用 ``SendComplete``，就无法获取该数据项，也无法获取 20 字节后的数据项。

当 20 字节数据项最终发送完成后，就可以在 buffer 中最初的 16 字节数据项之后，按照 20、8、24 字节的顺序接收所有的三个数据项。

由于 ``SendAcquire`` 及 ``SendComplete`` 要求所获取的 buffer 必须是完整的（未包装的），故可分割 buffer 和字节 buffer 不支持上述调用操作。


绕回
^^^^^^^^^^^

以下图表说明了发送数据项需要绕回时，不可分割、可分割和字节 buffer 之间的差异。图表假设有一个 **128 字节的 buffer**，其中有 **56 字节的空闲空间可以绕回使用**，并发送了一个 **28 字节** 的数据项。

.. packetdiag:: ../../../_static/diagrams/ring-buffer/ring_buffer_wrap_no_split.diag
    :caption: 在不可分割 buffer 中绕回
    :align: center

不可分割 buffer **只在连续的空闲空间中存储数据项，在任何情况下都不分割数据项**。当 buffer 尾部的空闲空间不足以完全存储数据项及其标头时，尾部的空闲空间将被 **标记为虚拟数据**。然后，数据项将绕回并存储在 buffer 头部的空闲空间中。

参考上图， buffer 尾部的 16 字节空闲空间不足以存储 28 字节的数据项，因此，这 16 字节被标记为虚拟数据，然后将数据项写入了 buffer 头部的空闲空间中。

.. packetdiag:: ../../../_static/diagrams/ring-buffer/ring_buffer_wrap_allow_split.diag
    :caption: 在可分割 buffer 中绕回
    :align: center

当 buffer 尾部的空闲空间不足以存储数据项及其标头时，可分割 buffer 会尝试 **将数据项分割成两部分**。分割的的两部分数据项都将有自己的标头，因此会产生额外的 8 字节开销。

参考上图， buffer 尾部的 16 字节空闲空间不足以存储 28 字节的数据项。因此将数据项分割成两部分（8 字节和 20 字节），并将两部分写入 buffer。

.. note::

    可分割 buffer 将其分割好的两部分数据视为两个独立的数据项，因此不应调用 :cpp:func:`xRingbufferReceive`。需调用 :cpp:func:`xRingbufferReceiveSplit` 以线程安全的方式接收分割的两部分数据项。

.. packetdiag:: ../../../_static/diagrams/ring-buffer/ring_buffer_wrap_byte_buf.diag
    :caption: 在字节 buffer 中绕回
    :align: center

字节 buffer **将尽可能多的数据存储到 buffer 尾部的空闲空间** 中。剩余的数据会存储在 buffer 头部的空闲空间。在字节 buffer 中绕回不会产生任何额外开销。

参考上图，buffer 尾部的 16 字节空闲空间不足以完全存储 28 字节的数据，因此，将数据填入这 16 字节空闲空间后，剩余的 12 字节会被写入 buffer 头部的空闲空间。此时，buffer 包含两个独立的连续数据，并且每个连续数据都被字节 buffer 视为一个独立数据项。

检索/返回
^^^^^^^^^^^^^^^^^^^^

以下图表说明了在检索和返回数据时，不可分割、可分割 buffer 和字节 buffer 之间的差异：

.. packetdiag:: ../../../_static/diagrams/ring-buffer/ring_buffer_read_ret_non_byte_buf.diag
    :caption: 在不可分割和可分割环形 buffer 中检索/返回数据项
    :align: center

不可分割 buffer 和可分割 buffer 中的数据项 **按严格的 FIFO 顺序检索** 并 **必须返回**，以释放占用的空间。在返回之前可以检索多个数据项，且不必按照检索的顺序返回数据项。但是，释放空间必须按 FIFO 顺序进行，因此如果不返回最早检索的数据项，就无法释放后续数据项占用的空间。

参考上图， **16、20 和 8 字节的数据项按 FIFO 顺序被检索出来**。但是，这些数据项并不是按照被检索的顺序返回的。最先返回的是 20 字节的数据项，然后分别返回 8 字节和 16 字节的数据项。直到第一个数据项（即 16 字节的数据项）返回后，空间才会被释放。

.. packetdiag:: ../../../_static/diagrams/ring-buffer/ring_buffer_read_ret_byte_buf.diag
    :caption: 在字节 buffer 中检索/返回数据
    :align: center

字节 buffer **不允许在返回之前进行多次检索** （每次检索必须在下一次检索之前返回结果）。使用 :cpp:func:`xRingbufferReceive` 或 :cpp:func:`xRingbufferReceiveFromISR` 时，会检索所有连续存储的数据。使用 :cpp:func:`xRingbufferReceiveUpTo` 或 :cpp:func:`xRingbufferReceiveUpToFromISR` 可限制检索的最大字节数。由于每次检索后都必须返回，因此数据一返回就会释放空间。

参考上图， buffer 尾部 38 字节连续存储的数据被检索、返回和释放。然后，下一次调用 :cpp:func:`xRingbufferReceive` 或 :cpp:func:`xRingbufferReceiveFromISR` 时，buffer 将绕回并对头部的 30 字节连续存储数据进行同样的处理。

使用队列集的环形 buffer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

使用 :cpp:func:`xRingbufferAddToQueueSetRead` 可以将环形 buffer 添加到 FreeRTOS 队列集中，这样每次环形 buffer 接收一个数据项或数据时，队列集都会收到通知。添加到队列集后，每次从环形 buffer 检索数据项时都应该先调用 :cpp:func:`xQueueSelectFromSet`。要检查选定的队列集成员是否为环形 buffer，调用 :cpp:func:`xRingbufferCanRead`。

以下示例演示了如何使用包含环形 buffer 的队列集：

.. code-block:: c

    #include "freertos/queue.h"
    #include "freertos/ringbuf.h"

    ...

        //创建环形 buffer 和队列集
        RingbufHandle_t buf_handle = xRingbufferCreate(1028, RINGBUF_TYPE_NOSPLIT);
        QueueSetHandle_t queue_set = xQueueCreateSet(3);

        //向队列集中添加环形 buffer
        if (xRingbufferAddToQueueSetRead(buf_handle, queue_set) != pdTRUE) {
            printf("Failed to add to queue set\n");
        }

    ...

        //阻塞队列集
        QueueSetMemberHandle_t member = xQueueSelectFromSet(queue_set, pdMS_TO_TICKS(1000));

        //检查成员是否为环形 buffer
        if (member != NULL && xRingbufferCanRead(buf_handle, member) == pdTRUE) {
            //Member is ring buffer, receive item from ring buffer
            size_t item_size;
            char *item = (char *)xRingbufferReceive(buf_handle, &item_size, 0);

            //处理数据项
            ...

        } else {
            ...
        }

使用静态分配的环形 buffer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:cpp:func:`xRingbufferCreateStatic` 可用于创建具有特定内存需求的环形 buffer（如在外部 RAM 中分配的环形 buffer）。环形 buffer 使用的所有内存块都必须在创建之前手动分配，然后传递给 :cpp:func:`xRingbufferCreateStatic` 以初始化为环形 buffer。这些内存块中包括：

- 环形 buffer 的数据结构类型 :cpp:type:`StaticRingbuffer_t`。
- 环形 buffer 的存储区域，大小为 ``xBufferSize``。注意，对于不可分割和可分割 buffer，``xBufferSize`` 必须为 32 位对齐大小。

这些块的分配方式取决于具体的需求。例如，静态声明所有块，或动态分配为具有特定功能的块，如外部 RAM。

.. note::

    当删除通过 :cpp:func:`xRingbufferCreateStatic` 创建的环形 buffer 时，:cpp:func:`vRingbufferDelete` 函数不会释放任何内存块。释放内存必须在调用 :cpp:func:`vRingbufferDelete` 后手动完成。

下面的代码片段演示了一个完全在外部 RAM 中分配的环形 buffer：

.. code-block:: c

    #include "freertos/ringbuf.h"
    #include "freertos/semphr.h"
    #include "esp_heap_caps.h"

    #define BUFFER_SIZE     400      //32 位对齐大小
    #define BUFFER_TYPE     RINGBUF_TYPE_NOSPLIT
    ...

    //将 环形 buffer 数据结构体和存储区分配到外部 RAM 中
    StaticRingbuffer_t *buffer_struct = (StaticRingbuffer_t *)heap_caps_malloc(sizeof(StaticRingbuffer_t), MALLOC_CAP_SPIRAM);
    uint8_t *buffer_storage = (uint8_t *)heap_caps_malloc(sizeof(uint8_t)*BUFFER_SIZE, MALLOC_CAP_SPIRAM);

    //使用手动分配的内存创建环形 buffer
    RingbufHandle_t handle = xRingbufferCreateStatic(BUFFER_SIZE, BUFFER_TYPE, buffer_storage, buffer_struct);

    ...

    //使用后删除环形 buffer
    vRingbufferDelete(handle);

    //手动释放所有内存块
    free(buffer_struct);
    free(buffer_storage);


.. ------------------------------------------- ESP-IDF Tick and Idle Hooks ---------------------------------------------

ESP-IDF tick 钩子 和 idle 钩子
----------------------------------

FreeRTOS 允许应用程序在编译时提供一个 tick 钩子和一个 idle 钩子：

- FreeRTOS tick 钩子可以通过 :ref:`CONFIG_FREERTOS_USE_TICK_HOOK` 选项启用。应用程序必须提供 ``void vApplicationTickHook( void )`` 回调。
- FreeRTOS idle 钩子可以通过 :ref:`CONFIG_FREERTOS_USE_IDLE_HOOK` 选项启用。应用程序必须提供 ``void vApplicationIdleHook( void )`` 回调。

然而，FreeRTOS tick 钩子和 idle 钩子有以下不足：

- FreeRTOS 钩子是在编译时注册的
- 每种钩子只能注册一个
- 在多核目标芯片上，FreeRTOS 钩子是对称的，即每个内核的 tick 中断和 idle 任务最终都会调用同一个钩子

因此，ESP-IDF 提供了 tick 钩子和 idle 钩子来补充 FreeRTOS tick 和 idle 钩子的功能。ESP-IDF 钩子具有以下功能：

- 钩子可以在运行时注册和注销
- 可以注册多个钩子。每个内核中，同一类型的钩子最多可以注册 8 个
- 在多核目标芯片上，钩子可以是不对称的，即可以为每个内核注册不同的钩子

使用以下 API 注册和注销 ESP-IDF 钩子：

- 对于 tick 钩子：

    - 用 :cpp:func:`esp_register_freertos_tick_hook` 或 :cpp:func:`esp_register_freertos_tick_hook_for_cpu` 注册
    - 用 :cpp:func:`esp_deregister_freertos_tick_hook` 或 :cpp:func:`esp_deregister_freertos_tick_hook_for_cpu` 注销

- 对于 idle 钩子：

    - 使用 :cpp:func:`esp_register_freertos_idle_hook` 或 :cpp:func:`esp_register_freertos_idle_hook_for_cpu` 注册
    - 使用 :cpp:func:`esp_deregister_freertos_idle_hook` 或 :cpp:func:`esp_deregister_freertos_idle_hook_for_cpu` 注销

.. note::

    在 cache 被禁用时，tick 中断仍保持活动，因此任何 tick 钩子（FreeRTOS 或 ESP-IDF）函数都必须放在内部 RAM 中。请参考 :ref:`SPI flash API documentation <iram-safe-interrupt-handlers>` 了解详情。

.. -------------------------------------------------- TLSP Callback ----------------------------------------------------

TLSP 删除回调
-----------------------

原生 FreeRTOS 提供了线程本地存储指针 (TLSP) 功能，这些指针直接存储在特定任务的任务控制块 (TCB) 中。TLSP 允许每个任务拥有自己的数据结构指针集合。在原生 FreeRTOS 中：

- 在任务创建后，需调用 :cpp:func:`vTaskSetThreadLocalStoragePointer` 设置任务的 TLSP。
- 在任务的生命周期中，需调用 :cpp:func:`pvTaskGetThreadLocalStoragePointer` 获取任务的 TLSP。
- 在删除任务前，需释放 TLSP 指向的内存。

然而，为了能够欧自动释放 TLSP 内存，ESP-IDF 额外提供了 TLSP 删除回调功能。当删除任务时，这些删除回调函数会被自动调用，从而清除 TLSP 内存，无需在每个任务的代码中显式添加内存清除逻辑。

设置 TLSP 删除回调的方式与设置 TLSP 类似。

- :cpp:func:`vTaskSetThreadLocalStoragePointerAndDelCallback` 设置了特定的 TLSP 及其关联的回调。
- 调用原生 FreeRTOS 函数 :cpp:func:`vTaskSetThreadLocalStoragePointer` 只会将 TLSP 的关联删除回调设置为 `NULL`，也就是说，在任务删除期间不会调用该 TLSP 的回调。

在实现 TLSP 回调时，应注意以下几点：

- 回调 **绝对不能尝试阻塞或让出**，并且应尽可能缩短临界区的时间。
- 回调是在删除任务的内存即将被释放前调用的。因此，回调可以通过 :cpp:func:`vTaskDelete` 本身调用，也可以从空闲任务中调用。

.. --------------------------------------------- ESP-IDF Additional API ------------------------------------------------

.. _freertos-idf-additional-api:

IDF 附加 API
------------------

:component_file:`freertos/esp_additions/include/freertos/idf_additions.h` 头文件包含了 ESP-IDF 添加的与 FreeRTOS 相关的辅助函数。通过 ``#include "freertos/idf_additions.h"`` 可添加此头文件。

.. ------------------------------------------ Component Specific Properties --------------------------------------------

组件专用功能
-----------------------------

除了基本 CMake 构建属性中提供的标准组件变量外，FreeRTOS 组件还提供了参数（目前只有一个参数）以简化与其他模块的集成：

- `ORIG_INCLUDE_PATH` - 包含指向 freeRTOS 根包含文件夹的绝对路径。因此可以直接用 `#include "FreeRTOS.h"` 引用头文件，而无需使用 `#include "freertos/FreeRTOS.h"`。


.. -------------------------------------------------- API Reference ----------------------------------------------------

API 参考
-------------

环形 buffer API
^^^^^^^^^^^^^^^

.. include-build-file:: inc/ringbuf.inc

钩子 API
^^^^^^^^^

.. include-build-file:: inc/esp_freertos_hooks.inc

附加 API
^^^^^^^^^^^^^^

.. include-build-file:: inc/idf_additions.inc
