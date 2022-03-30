SPI Features
============

.. _spi_master_features:

SPI Master
----------

.. _spi_bus_lock:

SPI Bus Lock
^^^^^^^^^^^^

To realize the multiplexing of different devices from different drivers (SPI Master, SPI Flash,
etc.), an SPI bus lock is applied on each SPI bus. Drivers can attach their devices onto the bus
with the arbitration of the lock.

Each bus lock are initialized with a BG (background) service registered, all devices request to
do transactions on the bus should wait until the BG to be successfully disabled.

- For SPI1 bus, the BG is the cache, the bus lock will help to disable the cache before device
  operations starts, and enable it again after device releasing the lock. No devices on SPI1 is
  allowed using ISR (it's meaningless for the task to yield to other tasks when the cache is
  disabled).

  .. only:: esp32

      There are quite a few limitations when using SPI Master driver on the SPI1 bus, see
      :ref:`spi_master_on_spi1_bus`.

  .. only:: not esp32

      The SPI Master driver hasn't supported SPI1 bus. Only SPI Flash driver can attach to the bus.

- For other buses, the driver may register its ISR as the BG. The bus lock will block a device
  task when it requests for exclusive use of the bus, try to disable the ISR, and unblock the
  device task allowed to exclusively use the bus when the ISR is successfully disabled. When the
  task releases the lock, the lock will also try to resume the ISR if there are pending
  transactions to be done in the ISR.
