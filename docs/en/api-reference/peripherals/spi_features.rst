SPI Features
============

.. _spi_master_features:

SPI Master
----------

.. _spi_bus_lock:

SPI Bus Lock
^^^^^^^^^^^^

To realize the multiplexing of different devices from different drivers, including SPI Master, SPI Flash, etc., an SPI bus lock is applied on each SPI bus. Drivers can attach their devices onto the bus with the arbitration of the lock.

Each bus lock is initialized with a BG (background) service registered. All devices that request transactions on the bus should wait until the BG is successfully disabled.

- For SPI1 bus, the BG is the cache. The bus lock will disable the cache before device operations start, and enable it again after device releases the lock. No devices on SPI1 is allowed to use ISR, since it is meaningless for the task to yield to other tasks when the cache is disabled.

  .. only:: esp32

      There are quite a few limitations when using SPI Master driver on the SPI1 bus. See :ref:`spi_master_on_spi1_bus`.

  .. only:: not esp32

      The SPI Master driver hasn't supported SPI1 bus. Only SPI Flash driver can attach to the bus.

- For other buses, the driver may register its ISR as the BG. When a device task requests for exclusive use of the bus, the bus lock will block the task and try to disable ISR. After ISR is successfully disabled, the bus lock will then unblock the device task and allow it to exclusively use the bus. When the task releases the lock, the lock will also try to resume ISR if there are pending transactions in ISR.
