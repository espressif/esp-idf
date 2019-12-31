FreeRTOS Additions
==================

Overview
--------

ESP-IDF FreeRTOS is based on the Xtensa port of FreeRTOS v8.2.0 with significant modifications
for SMP compatibility (see :doc:`ESP-IDF FreeRTOS SMP Changes<../../api-guides/freertos-smp>`).
However various features specific to ESP-IDF FreeRTOS have been added. The features are as follows:

:ref:`ring-buffers`: Ring buffers were added to provide a form of buffer that could accept
entries of arbitrary lengths.

:ref:`hooks`: ESP-IDF FreeRTOS hooks provides support for registering extra Idle and
Tick hooks at run time. Moreover, the hooks can be asymmetric amongst both CPUs.


.. _ring-buffers:

Ring Buffers
------------

The ESP-IDF FreeRTOS ring buffer is a strictly FIFO buffer that supports arbitrarily sized items.
Ring buffers are a more memory efficient alternative to FreeRTOS queues in situations where the
size of items is variable. The capacity of a ring buffer is not measured by the number of items
it can store, but rather by the amount of memory used for storing items. You may apply for a
piece of memory on the ring buffer to send an item, or just use the API to copy your data and send
(according to the send API you call). For efficiency reasons,
**items are always retrieved from the ring buffer by reference**. As a result, all retrieved
items *must also be returned* in order for them to be removed from the ring buffer completely.
The ring buffers are split into the three following types:

**No-Split** buffers will guarantee that an item is stored in contiguous memory and will not
attempt to split an item under any circumstances. Use no-split buffers when items must occupy
contiguous memory. *Only this buffer type allows you getting the data item address and writting
to the item by yourself.*

**Allow-Split** buffers will allow an item to be split when wrapping around if doing so will allow
the item to be stored. Allow-split buffers are more memory efficient than no-split buffers but
can return an item in two parts when retrieving.

**Byte buffers** do not store data as separate items. All data is stored as a sequence of bytes,
and any number of bytes and be sent or retrieved each time. Use byte buffers when separate items
do not need to be maintained (e.g. a byte stream).

.. note::
    No-split/allow-split buffers will always store items at 32-bit aligned addresses. Therefore when
    retrieving an item, the item pointer is guaranteed to be 32-bit aligned. This is useful
    especially when you need to send some data to the DMA.

.. note::
    Each item stored in no-split/allow-split buffers will **require an additional 8 bytes for a header**.
    Item sizes will also be rounded up to a 32-bit aligned size (multiple of 4 bytes), however the true
    item size is recorded within the header. The sizes of no-split/allow-split buffers will also
    be rounded up when created.

Usage
^^^^^

The following example demonstrates the usage of :cpp:func:`xRingbufferCreate`
and :cpp:func:`xRingbufferSend` to create a ring buffer then send an item to it.

.. code-block:: c

    #include "freertos/ringbuf.h"
    static char tx_item[] = "test_item";

    ...

        //Create ring buffer
        RingbufHandle_t buf_handle;
        buf_handle = xRingbufferCreate(1028, RINGBUF_TYPE_NOSPLIT);
        if (buf_handle == NULL) {
            printf("Failed to create ring buffer\n");
        }

        //Send an item
        UBaseType_t res =  xRingbufferSend(buf_handle, tx_item, sizeof(tx_item), pdMS_TO_TICKS(1000));
        if (res != pdTRUE) {
            printf("Failed to send item\n");
        }

The following example demonstrates the usage of :cpp:func:`xRingbufferSendAcquire` and
:cpp:func:`xRingbufferSendComplete` instead of :cpp:func:`xRingbufferSend` to apply for the
memory on the ring buffer (of type `RINGBUF_TYPE_NOSPLIT`) and then send an item to it. This way
adds one more step, but allows getting the address of the memory to write to, and writing to the
memory yourself.

.. code-block:: c

    #include "freertos/ringbuf.h"
    #include "soc/lldesc.h"

    typedef struct {
        lldesc_t dma_desc;
        uint8_t buf[1];
    } dma_item_t;

    #define DMA_ITEM_SIZE(N) (sizeof(lldesc_t)+(((N)+3)&(~3)))

    ...

        //Retrieve space for DMA descriptor and corresponding data buffer
        //This has to be done with SendAcquire, or the address may be different when copy
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
        //Actually send to the ring buffer for consumer to use
        res = xRingbufferSendComplete(buf_handle, &item);
        if (res != pdTRUE) {
            printf("Failed to send item\n");
        }

The following example demonstrates retrieving and returning an item from a **no-split ring buffer**
using :cpp:func:`xRingbufferReceive` and :cpp:func:`vRingbufferReturnItem`

.. code-block:: c

    ...

        //Receive an item from no-split ring buffer
        size_t item_size;
        char *item = (char *)xRingbufferReceive(buf_handle, &item_size, pdMS_TO_TICKS(1000));

        //Check received item
        if (item != NULL) {
            //Print item
            for (int i = 0; i < item_size; i++) {
                printf("%c", item[i]);
            }
            printf("\n");
            //Return Item
            vRingbufferReturnItem(buf_handle, (void *)item);
        } else {
            //Failed to receive item
            printf("Failed to receive item\n");
        }


The following example demonstrates retrieving and returning an item from an **allow-split ring buffer**
using :cpp:func:`xRingbufferReceiveSplit` and :cpp:func:`vRingbufferReturnItem`

.. code-block:: c

    ...

        //Receive an item from allow-split ring buffer
        size_t item_size1, item_size2;
        char *item1, *item2;
        BaseType_t ret = xRingbufferReceiveSplit(buf_handle, (void **)&item1, (void **)&item2, &item_size1, &item_size2, pdMS_TO_TICKS(1000));

        //Check received item
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
            //Failed to receive item
            printf("Failed to receive item\n");
        }


The following example demonstrates retrieving and returning an item from a **byte buffer**
using :cpp:func:`xRingbufferReceiveUpTo` and :cpp:func:`vRingbufferReturnItem`

.. code-block:: c

    ...

        //Receive data from byte buffer
        size_t item_size;
        char *item = (char *)xRingbufferReceiveUpTo(buf_handle, &item_size, pdMS_TO_TICKS(1000), sizeof(tx_item));

        //Check received data
        if (item != NULL) {
            //Print item
            for (int i = 0; i < item_size; i++) {
                printf("%c", item[i]);
            }
            printf("\n");
            //Return Item
            vRingbufferReturnItem(buf_handle, (void *)item);
        } else {
            //Failed to receive item
            printf("Failed to receive item\n");
        }


For ISR safe versions of the functions used above, call :cpp:func:`xRingbufferSendFromISR`, :cpp:func:`xRingbufferReceiveFromISR`,
:cpp:func:`xRingbufferReceiveSplitFromISR`, :cpp:func:`xRingbufferReceiveUpToFromISR`, and :cpp:func:`vRingbufferReturnItemFromISR`


Sending to Ring Buffer
^^^^^^^^^^^^^^^^^^^^^^

The following diagrams illustrate the differences between no-split/allow-split buffers
and byte buffers with regards to sending items/data. The diagrams assume that three
items of sizes **18, 3, and 27 bytes** are sent respectively to a **buffer of 128 bytes**.

.. packetdiag:: ../../../_static/diagrams/ring-buffer/ring_buffer_send_non_byte_buf.diag
    :caption: Sending items to no-split/allow-split ring buffers
    :align: center

For no-split/allow-split buffers, a header of 8 bytes precedes every data item. Furthermore, the space
occupied by each item is **rounded up to the nearest 32-bit aligned size** in order to maintain overall
32-bit alignment. However the true size of the item is recorded inside the header which will be
returned when the item is retrieved.

Referring to the diagram above, the 18, 3, and 27 byte items are **rounded up to 20, 4, and 28 bytes**
respectively. An 8 byte header is then added in front of each item.

.. packetdiag:: ../../../_static/diagrams/ring-buffer/ring_buffer_send_byte_buf.diag
    :caption: Sending items to byte buffers
    :align: center

Byte buffers treat data as a sequence of bytes and does not incur any overhead
(no headers). As a result, all data sent to a byte buffer is merged into a single item.

Referring to the diagram above, the 18, 3, and 27 byte items are sequentially written to the
byte buffer and **merged into a single item of 48 bytes**.

Using SendAcquire and SendComplete
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Items in no-split buffers are acquired (by SendAcquire) in strict FIFO order and must be sent to
the buffer by SendComplete for the data to be accessible by the consumer. Multiple items can be
sent or acquired without calling SendComplete, and the items do not necessarily need to be
completed in the order they were acquired. However the receiving of data items must occur in FIFO
order, therefore not calling SendComplete the earliest acquired item will prevent the subsequent
items from being received.

The following diagrams illustrate what will happen when SendAcquire/SendComplete don't happen in
the same order. At the beginning, there is already an data item of 16 bytes sent to the ring
buffer. Then SendAcquire is called to acquire space of 20, 8, 24 bytes on the ring buffer.

.. packetdiag:: ../../../_static/diagrams/ring-buffer/ring_buffer_send_acquire_complete.diag
    :caption: SendAcquire/SendComplete items in no-split ring buffers
    :align: center

After that, we fill (use) the buffers, and send them to the ring buffer by SendComplete in the
order of 8, 24, 20. When 8 bytes and 24 bytes data are sent, the consumer still can only get the
16 bytes data item. Due to the usage if 20 bytes item is not complete, it's not available, nor
the following data items.

When the 20 bytes item is finally completed, all the 3 data items can be received now, in the
order of 20, 8, 24 bytes, right after the 16 bytes item existing in the buffer at the beginning.

Allow-split/byte buffers do not allow using SendAcquire/SendComplete since acquired buffers are
required to be complete (not wrapped).


Wrap around
^^^^^^^^^^^

The following diagrams illustrate the differences between no-split, allow-split, and byte
buffers when a sent item requires a wrap around. The diagrams assumes a buffer of **128 bytes**
with **56 bytes of free space that wraps around** and a sent item of **28 bytes**.

.. packetdiag:: ../../../_static/diagrams/ring-buffer/ring_buffer_wrap_no_split.diag
    :caption: Wrap around in no-split buffers
    :align: center

No-split buffers will **only store an item in continuous free space and will not split
an item under any circumstances**. When the free space at the tail of the buffer is insufficient
to completely store the item and its header, the free space at the tail will be **marked as dummy data**.
The buffer will then wrap around and store the item in the free space at the head of the buffer.

Referring to the diagram above, the 16 bytes of free space at the tail of the buffer is
insufficient to store the 28 byte item. Therefore the 16 bytes is marked as dummy data and
the item is written to the free space at the head of the buffer instead.

.. packetdiag:: ../../../_static/diagrams/ring-buffer/ring_buffer_wrap_allow_split.diag
    :caption: Wrap around in allow-split buffers
    :align: center

Allow-split buffers will attempt to **split the item into two parts** when the free space at the tail
of the buffer is insufficient to store the item data and its header. Both parts of the
split item will have their own headers (therefore incurring an extra 8 bytes of overhead).

Referring to the diagram above, the 16 bytes of free space at the tail of the buffer is insufficient
to store the 28 byte item. Therefore the item is split into two parts (8 and 20 bytes) and written
as two parts to the buffer.

.. note::
    Allow-split buffers treats the both parts of the split item as two separate items, therefore call
    :cpp:func:`xRingbufferReceiveSplit` instead of :cpp:func:`xRingbufferReceive` to receive both
    parts of a split item in a thread safe manner.

.. packetdiag:: ../../../_static/diagrams/ring-buffer/ring_buffer_wrap_byte_buf.diag
    :caption: Wrap around in byte buffers
    :align: center

Byte buffers will **store as much data as possible into the free space at the tail of buffer**. The remaining
data will then be stored in the free space at the head of the buffer. No overhead is incurred when wrapping
around in byte buffers.

Referring to the diagram above, the 16 bytes of free space at the tail of the buffer is insufficient to
completely store the 28 bytes of data. Therefore the 16 bytes of free space is filled with data, and the
remaining 12 bytes are written to the free space at the head of the buffer. The buffer now contains
data in two separate continuous parts, and each part continuous will be treated as a separate item by the
byte buffer.

Retrieving/Returning
^^^^^^^^^^^^^^^^^^^^

The following diagrams illustrates the differences between no-split/allow-split and
byte buffers in retrieving and returning data.

.. packetdiag:: ../../../_static/diagrams/ring-buffer/ring_buffer_read_ret_non_byte_buf.diag
    :caption: Retrieving/Returning items in no-split/allow-split ring buffers
    :align: center

Items in no-split/allow-split buffers are **retrieved in strict FIFO order** and **must be returned**
for the occupied space to be freed. Multiple items can be retrieved before returning, and the items
do not necessarily need to be returned in the order they were retrieved. However the freeing of space
must occur in FIFO order, therefore not returning the earliest retrieved item will prevent the space
of subsequent items from being freed.

Referring to the diagram above, the **16, 20, and 8 byte items are retrieved in FIFO order**. However the items
are not returned in they were retrieved (20, 8, 16). As such, the space is not freed until the first item
(16 byte) is returned.

.. packetdiag:: ../../../_static/diagrams/ring-buffer/ring_buffer_read_ret_byte_buf.diag
    :caption: Retrieving/Returning data in byte buffers
    :align: center

Byte buffers **do not allow multiple retrievals before returning** (every retrieval must be followed by a return
before another retrieval is permitted). When using :cpp:func:`xRingbufferReceive` or
:cpp:func:`xRingbufferReceiveFromISR`, all continuous stored data will be retrieved. :cpp:func:`xRingbufferReceiveUpTo`
or :cpp:func:`xRingbufferReceiveUpToFromISR` can be used to restrict the maximum number of bytes retrieved. Since
every retrieval must be followed by a return, the space will be freed as soon as the data is returned.

Referring to the diagram above, the 38 bytes of continuous stored data at the tail of the buffer is retrieved,
returned, and freed. The next call to :cpp:func:`xRingbufferReceive` or :cpp:func:`xRingbufferReceiveFromISR`
then wraps around and does the same to the 30 bytes of continuous stored data at the head of the buffer.

Ring Buffers with Queue Sets
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Ring buffers can be added to FreeRTOS queue sets using :cpp:func:`xRingbufferAddToQueueSetRead` such that every
time a ring buffer receives an item or data, the queue set is notified. Once added to a queue set, every
attempt to retrieve an item from a ring buffer should be preceded by a call to :cpp:func:`xQueueSelectFromSet`.
To check whether the selected queue set member is the ring buffer, call :cpp:func:`xRingbufferCanRead`.

The following example demonstrates queue set usage with ring buffers.

.. code-block:: c

    #include "freertos/queue.h"
    #include "freertos/ringbuf.h"

    ...

        //Create ring buffer and queue set
        RingbufHandle_t buf_handle = xRingbufferCreate(1028, RINGBUF_TYPE_NOSPLIT);
        QueueSetHandle_t queue_set = xQueueCreateSet(3);

        //Add ring buffer to queue set
        if (xRingbufferAddToQueueSetRead(buf_handle, queue_set) != pdTRUE) {
            printf("Failed to add to queue set\n");
        }

    ...

        //Block on queue set
        xQueueSetMemberHandle member = xQueueSelectFromSet(queue_set, pdMS_TO_TICKS(1000));

        //Check if member is ring buffer
        if (member != NULL && xRingbufferCanRead(buf_handle, member) == pdTRUE) {
            //Member is ring buffer, receive item from ring buffer
            size_t item_size;
            char *item = (char *)xRingbufferReceive(buf_handle, &item_size, 0);

            //Handle item
            ...

        } else {
            ...
        }

Ring Buffers with Static Allocation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :cpp:func:`xRingbufferCreateStatic` can be used to create ring buffers with specific memory requirements (such as a ring buffer being allocated in external RAM). All blocks of memory used by a ring buffer must be manually allocated beforehand then passed to the :cpp:func:`xRingbufferCreateStatic` to be initialized as a ring buffer. These blocks include the following:

- The ring buffer's data structure of type :cpp:type:`StaticRingbuffer_t`
- The ring buffer's storage area of size ``xBufferSize``. Note that ``xBufferSize`` must be 32-bit aligned for no-split/allow-split buffers.

The manner in which these blocks are allocated will depend on the users requirements (e.g. all blocks being statically declared, or dynamically allocated with specific capabilities such as external RAM).

.. note::
    The :ref:`CONFIG_FREERTOS_SUPPORT_STATIC_ALLOCATION` option must be enabled in `menuconfig` for statically allocated ring buffers to be available.

.. note::
    When deleting a ring buffer created via :cpp:func:`xRingbufferCreateStatic`,
    the function :cpp:func:`vRingbufferDelete` will not free any of the memory blocks. This must be done manually by the user after :cpp:func:`vRingbufferDelete` is called.

The code snippet below demonstrates a ring buffer being allocated entirely in external RAM.

.. code-block:: c

    #include "freertos/ringbuf.h"
    #include "freertos/semphr.h"
    #include "esp_heap_caps.h"

    #define BUFFER_SIZE     400      //32-bit aligned size
    #define BUFFER_TYPE     RINGBUF_TYPE_NOSPLIT
    ...

    //Allocate ring buffer data structure and storage area into external RAM
    StaticRingbuffer_t *buffer_struct = (StaticRingbuffer_t *)heap_caps_malloc(sizeof(StaticRingbuffer_t), MALLOC_CAP_SPIRAM);
    uint8_t *buffer_storage = (uint8_t *)heap_caps_malloc(sizeof(uint8_t)*BUFFER_SIZE, MALLOC_CAP_SPIRAM);

    //Create a ring buffer with manually allocated memory
    RingbufHandle_t handle = xRingbufferCreateStatic(BUFFER_SIZE, BUFFER_TYPE, buffer_storage, buffer_struct);

    ...

    //Delete the ring buffer after used
    vRingbufferDelete(handle);

    //Manually free all blocks of memory
    free(buffer_struct);
    free(buffer_storage);


Ring Buffer API Reference
-------------------------

.. note::
    Ideally, ring buffers can be used with multiple tasks in an SMP fashion where the **highest
    priority task will always be serviced first.** However due to the usage of binary semaphores
    in the ring buffer's underlying implementation, priority inversion may occur under very
    specific circumstances.

    The ring buffer governs sending by a binary semaphore which is given whenever space is
    freed on the ring buffer. The highest priority task waiting to send will repeatedly take
    the semaphore until sufficient free space becomes available or until it times out. Ideally
    this should prevent any lower priority tasks from being serviced as the semaphore should
    always be given to the highest priority task.

    However in between iterations of acquiring the semaphore, there is a **gap in the critical
    section** which may permit another task (on the other core or with an even higher priority) to
    free some space on the ring buffer and as a result give the semaphore. Therefore the semaphore
    will be given before the highest priority task can re-acquire the semaphore. This will result
    in the **semaphore being acquired by the second highest priority task** waiting to send, hence
    causing priority inversion.

    This side effect will not affect ring buffer performance drastically given if the number
    of tasks using the ring buffer simultaneously is low, and the ring buffer is not operating
    near maximum capacity.

.. include:: /_build/inc/ringbuf.inc


.. _hooks:

Hooks
-----

FreeRTOS consists of Idle Hooks and Tick Hooks which allow for application
specific functionality to be added to the Idle Task and Tick Interrupt.
ESP-IDF provides its own Idle and Tick Hook API in addition to the hooks
provided by Vanilla FreeRTOS. ESP-IDF hooks have the added benefit of
being run time configurable and asymmetrical.

Vanilla FreeRTOS Hooks
^^^^^^^^^^^^^^^^^^^^^^

Idle and Tick Hooks in vanilla FreeRTOS are implemented by the user
defining the functions ``vApplicationIdleHook()`` and  ``vApplicationTickHook()``
respectively somewhere in the application. Vanilla FreeRTOS will run the user
defined Idle Hook and Tick Hook on every iteration of the Idle Task and Tick
Interrupt respectively.

Vanilla FreeRTOS hooks are referred to as **Legacy Hooks** in ESP-IDF FreeRTOS.
To enable legacy hooks, :ref:`CONFIG_FREERTOS_LEGACY_HOOKS` should be enabled
in :doc:`project configuration menu </api-reference/kconfig>`.

Due to vanilla FreeRTOS being designed for single core, ``vApplicationIdleHook()``
and ``vApplicationTickHook()`` can only be defined once. However, the ESP32 is dual core
in nature, therefore same Idle Hook and Tick Hook are used for both cores (in other words,
the hooks are symmetrical for both cores).

ESP-IDF Idle and Tick Hooks
^^^^^^^^^^^^^^^^^^^^^^^^^^^
Due to the the dual core nature of the ESP32, it may be necessary for some
applications to have separate hooks for each core. Furthermore, it may
be necessary for the Idle Tasks or Tick Interrupts to execute multiple hooks
that are configurable at run time. Therefore the ESP-IDF provides it's own hooks
API in addition to the legacy hooks provided by Vanilla FreeRTOS.

The ESP-IDF tick/idle hooks are registered at run time, and each tick/idle hook
must be registered to a specific CPU. When the idle task runs/tick Interrupt
occurs on a particular CPU, the CPU will run each of its registered idle/tick hooks
in turn.


Hooks API Reference
-------------------

.. include:: /_build/inc/esp_freertos_hooks.inc
