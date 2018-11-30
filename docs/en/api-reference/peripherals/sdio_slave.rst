SDIO Card Slave Driver
======================

Overview
--------

The ESP32 SDIO Card peripherals (Host, Slave) shares two sets of pins as below table.
The first set is usually occupied by SPI0 bus which is responsible for the SPI flash holding the code to run.
This means SDIO slave driver can only runs on the second set of pins while SDIO host is not using it.

+----------+-------+-------+
| Pin Name | Slot1 | Slot2 |
+          +-------+-------+
|          | GPIO Number   |
+==========+=======+=======+
| CLK      | 6     | 14    |
+----------+-------+-------+
| CMD      | 11    | 15    |
+----------+-------+-------+
| DAT0     | 7     | 2     |
+----------+-------+-------+
| DAT1     | 8     | 4     |
+----------+-------+-------+
| DAT2     | 9     | 12    |
+----------+-------+-------+
| DAT3     | 10    | 13    |
+----------+-------+-------+

The SDIO slave can run under 3 modes: SPI, 1-bit SD and 4-bit SD modes, which
is detected automatically by the hardware. According to the SDIO
specification, CMD and DAT0-3 lines should be pulled up no matter in 1-bit,
4-bit or SPI mode. Then the host initialize the slave into SD mode by first
sending CMD0 with DAT3 pin high, while initialize the slave into SPI mode by
sending CMD0 with CS pin (the same pin as DAT3) low.

.. note:: CMD and DATA lines D0-D3 of the card should be pulled up by 50KOhm resistor
  even in 1-bit mode or SPI mode. Most official devkits don't meet the pullup
  requirements by default, and there are conflicts on strapping pins as well.
  Please refer to :doc:`sd_pullup_requirements` to see how to setup your
  system correctly.

.. toctree::
    :hidden:

    sd_pullup_requirements

After the initialization, the host can enable the 4-bit SD mode by writing
CCCR register 0x07 by CMD52. All the bus detection process are handled by the
slave peripheral.

The host has to communicate with the slave by an ESP-slave-specific protocol.
The slave driver offers 3 services over Function 1 access by CMD52 and CMD53:
(1) a sending FIFO and a receiving FIFO, (2) 52 8-bit R/W registers shared by
host and slave, (3) 16 interrupt sources (8 from host to slave, and 8 from
slave to host).

Terminology
^^^^^^^^^^^

The SDIO slave driver uses the following terms:

- Transfer: a transfer is always started by a command token from the host, and may contain a reply and several data
  blocks. ESP32 slave software is based on transfers.
- Sending: slave to host transfers.
- Receiving: host to slave transfers.

.. note:: Register names in ESP Rechnical Reference Manual are oriented from the point of view of the host, i.e. 'rx'
  registers refer to sending, while 'tx' registers refer to receiving. We're not using `tx` or `rx` in the driver to
  avoid ambiguities.

- FIFO: specific address in Function 1 that can be access by CMD53 to read/write large amount of data. The address is
  related to the length requested to read from/write to the slave in a single transfer:
  *requested length* = 0x1F800-address.
- Ownership: When the driver takes ownership of a buffer, it means the driver can randomly read/write the buffer
  (usually via DMA). The application should not read/write the buffer until the ownership is returned to the
  application. If the application reads from a buffer owned by a receiving driver, the data read can be random; if
  the application writes to a buffer owned by a sending driver, the data sent may be corrupted.
- Requested length: The length requested in one transfer determined by the FIFO address.
- Transfer length: The length requested in one transfer determined by the CMD53 byte/block count field.

.. note:: Requested length is different from the transfer length. ESP32 slave DMA base on the *requested length* rather
    than the *transfer length*. The *transfer length* should be no shorter than the *requested length*, and the rest
    part will be filled with 0 (sending) or discard (receiving).

- Receiving buffer size: The buffer size is pre-defined between the host and the slave before communication starts.
  Slave application has to set the buffer size during initialization by the ``recv_buffer_size`` member of
  ``sdio_slave_config_t``.
- Interrupts: the esp32 slave support interrupts in two directions: from host to slave (called slave interrupts below)
  and from slave to host (called host interrupts below). See more in :ref:`interrupts`.
- Registers: specific address in Function 1 access by CMD52 or CMD53.

Communication with ESP SDIO Slave
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The host should initialize the ESP32 SDIO slave according to the standard
SDIO initialization process (Sector 3.1.2 of `SDIO Simplified
Specification <https://www.sdcard.org/downloads/pls/>`_), which is described
briefly in :ref:`esp_slave_init`.

However, there's an ESP32-specific upper-level communication protocol upon
the CMD52/CMD53 to Func 1. Please refer to :ref:`esp_slave_protocol_layer`,
or example :example:`peripherals/sdio` when programming your host.

.. toctree::
    :hidden:

    esp_slave_protocol

.. _interrupts:

Interrupts
^^^^^^^^^^

There are interrupts from host to slave, and from slave to host to help communicating conveniently.

Slave Interrupts
""""""""""""""""

The host can interrupt the slave by writing any one bit in the register 0x08D. Once any bit of the register is
set, an interrupt is raised and the SDIO slave driver calls the callback function defined in the ``slave_intr_cb`` member
in the ``sdio_slave_config_t`` structure.

.. note:: The callback function is called in the ISR, do not use any delay, loop or spinlock in the callback.

There's another set of functions can be used. You can call ``sdio_slave_wait_int`` to wait for an interrupt within a
certain time, or call ``sdio_slave_clear_int`` to clear interrupts from host. The callback function can work with the
wait functions perfectly.

Host Interrupts
"""""""""""""""

The slave can interrupt the host by an interrupt line (at certain time) which is level sensitive. When the host see the
interrupt line pulled down, it may read the slave interrupt status register, to see the interrupt source. Host can clear
interrupt bits, or choose to disable a interrupt source. The interrupt line will hold active until all the sources are
cleared or disabled.

There are several dedicated interrupt sources as well as general purpose sources. see ``sdio_slave_hostint_t`` for
more information.

Shared Registers
^^^^^^^^^^^^^^^^

There are 52 8-bit R/W shared registers to share information between host and slave. The slave can write or read the
registers at any time by ``sdio_slave_read_reg`` and ``sdio_slave_write_reg``. The host can access (R/W) the register by CMD52 or CMD53.

Receiving FIFO
^^^^^^^^^^^^^^

When the host is going to send the slave some packets, it has to check whether the slave is ready to receive by reading
the buffer number of slave.

To allow the host sending data to the slave, the application has to load buffers to the slave driver by the following steps:

1. Register the buffer by calling ``sdio_slave_recv_register_buf``, and get the handle of the registered buffer. The driver
   will allocate memory for the linked-list descriptor needed to link the buffer onto the hardware.
2. Load buffers onto the driver by passing the buffer handle to ``sdio_slave_recv_load_buf``.
3. Call ``sdio_slave_recv`` to get the received data. If non-blocking call is needed, set ``wait=0``.
4. Pass the handle of processed buffer back to the driver by ``sdio_recv_load_buf`` again.

.. note:: To avoid overhead from copying data, the driver itself doesn't have any buffer inside, the application is
    responsible to offer new buffers in time. The DMA will automatically store received data to the buffer.

Sending FIFO
^^^^^^^^^^^^

Each time the slave has data to send, it raises an interrupt and the host will request for the packet length. There are
two sending modes:

- Stream Mode: when a buffer is loaded to the driver, the buffer length will be counted into the packet length requested
  by host in the incoming communications. Regardless previous packets are sent or not. This means the host can get data
  of several buffers in one transfer.
- Packet Mode: the packet length is updated packet by packet, and only when previous packet is sent. This means that the
  host can only get data of one buffer in one transfer.

.. note:: To avoid overhead from copying data, the driver itself doesn't have any buffer inside. Namely, the DMA takes
    data directly from the buffer provided by the application. The application should not touch the buffer until the
    sending is finished.

The sending mode can be set in the ``sending_mode`` member of ``sdio_slave_config_t``, and the buffer numbers can be
set in the ``send_queue_size``. All the buffers are restricted to be no larger than 4092 bytes. Though in the stream
mode several buffers can be sent in one transfer, each buffer is still counted as one in the queue.

The application can call ``sdio_slave_transmit`` to send packets. In this case the function returns when the transfer
is sucessfully done, so the queue is not fully used. When higher effeciency is required, the application can use the
following functions instead:

1. Pass buffer information (address, length, as well as an ``arg`` indicating the buffer) to ``sdio_slave_send_queue``.
   If non-blocking call is needed, set ``wait=0``. If the ``wait`` is not ``portMAX_DELAY`` (wait until success),
   application has to check the result to know whether the data is put in to the queue or discard.

2. Call ``sdio_slave_send_get_finished`` to get and deal with a finished transfer. A buffer should be keep unmodified
   until returned from ``sdio_slave_send_get_finished``. This means the buffer is actually sent to the host, rather
   than just staying in the queue.

There are several ways to use the ``arg`` in the queue parameter:

    1. Directly point ``arg`` to a dynamic-allocated buffer, and use the ``arg`` to free it when transfer finished.
    2. Wrap transfer informations in a transfer structure, and point ``arg`` to the structure. You can use the
       structure to do more things like::

          typedef struct {
              uint8_t* buffer;
              size_t   size;
              int      id;
          }sdio_transfer_t;

          //and send as:
          sdio_transfer_t trans = {
              .buffer = ADDRESS_TO_SEND,
              .size = 8,
              .id = 3,  //the 3rd transfer so far
          };
          sdio_slave_send_queue(trans.buffer, trans.size, &trans, portMAX_DELAY);

          //... maybe more transfers are sent here

          //and deal with finished transfer as:
          sdio_transfer_t* arg = NULL;
          sdio_slave_send_get_finished((void**)&arg, portMAX_DELAY);
          ESP_LOGI("tag", "(%d) successfully send %d bytes of %p", arg->id, arg->size, arg->buffer);
          some_post_callback(arg); //do more things

    3. Working with the receiving part of this driver, point ``arg`` to the receive buffer handle of this buffer. So
       that we can directly use the buffer to receive data when it's sent::

           uint8_t buffer[256]={1,2,3,4,5,6,7,8};
           sdio_slave_buf_handle_t handle = sdio_slave_recv_register_buf(buffer);
           sdio_slave_send_queue(buffer, 8, handle, portMAX_DELAY);

           //... maybe more transfers are sent here

           //and load finished buffer to receive as
           sdio_slave_buf_handle_t handle = NULL;
           sdio_slave_send_get_finished((void**)&handle, portMAX_DELAY);
           sdio_slave_recv_load_buf(handle);

       More about this, see :example:`peripherals/sdio`.


Application Example
-------------------

Slave/master communication: :example:`peripherals/sdio`.

API Reference
-------------

.. include:: /_build/inc/sdio_slave.inc

