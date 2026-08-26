SPI Features
============

.. _spi_master_features:

SPI Master
----------

.. _spi_bus_lock:

SPI Bus Lock
^^^^^^^^^^^^

To realize the multiplexing of different devices from different drivers, including SPI Master, SPI Flash, etc., an SPI bus lock is applied on each SPI bus. Drivers can attach their devices to the bus with the arbitration of the lock.

Each bus lock is initialized with a BG (background) service registered. All devices that request transactions on the bus should wait until the BG is successfully disabled.

- For the SPI1 bus, the BG is the cache. The bus lock disables the cache before device operations start, and enables it again after the device releases the lock. No devices on SPI1 are allowed to use ISR, since it is meaningless for the task to yield to other tasks when the cache is disabled.

  .. only:: esp32

      There are quite a few limitations when using the SPI Master driver on the SPI1 bus. See :ref:`spi_master_on_spi1_bus`.

  .. only:: not esp32

      The SPI Master driver has not supported SPI1 bus. Only the SPI Flash driver can attach to the bus.

- For other buses, the driver can register the ISR as a BG. If a device task requests exclusive bus access, the bus lock will block the task, disable the ISR, and then unblock the task. After the task releases the lock, the lock will try to re-enable the ISR if there are still pending transactions in the ISR.
