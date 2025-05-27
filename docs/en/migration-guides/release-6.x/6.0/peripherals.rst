Peripherals
===========

:link_to_translation:`zh_CN:[中文]`

Common Changes
--------------

All drivers' ``io_loop_back`` configuration have been removed
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Different driver objects can share the same GPIO number, enabling more complex functionalities. For example, you can bind the TX and RX channels of the RMT peripheral to the same GPIO to simulate 1-Wire bus read and write timing. In previous versions, you needed to configure the ``io_loop_back`` setting in the driver to achieve this "loopback" functionality. Now, this configuration has been removed. Simply configuring the same GPIO number in different drivers will achieve the same functionality.

RMT
---

The ``io_od_mode`` member in the :cpp:type:`rmt_tx_channel_config_t` configuration structure has been removed. If you want to use open-drain mode, you need to manually call the :func:`gpio_od_enable` function.

MCPWM
-----

The ``io_od_mode`` member in the :cpp:type:`mcpwm_generator_config_t` configuration structure has been removed. If you want to use open-drain mode, you need to manually call the :func:`gpio_od_enable` function.

The ``pull_up`` and ``pull_down`` members have been removed from the following configuration structures. You need to manually call the :func:`gpio_set_pull_mode` function to configure the pull-up and pull-down resistors for the IO:

.. list::

    - :cpp:type:`mcpwm_generator_config_t`
    - :cpp:type:`mcpwm_gpio_fault_config_t`
    - :cpp:type:`mcpwm_gpio_sync_src_config_t`
    - :cpp:type:`mcpwm_capture_channel_config_t`

I2C
---

I2C slave has been redesigned in v5.4. In the current version, the old I2C slave driver has been removed. For details, please refer to the I2C slave section in the programming guide.

The major breaking changes in concept and usage are listed as follows:

Major Changes in Concepts
~~~~~~~~~~~~~~~~~~~~~~~~~

- Previously, the I2C slave driver performed active read and write operations. In the new version, these operations are handled passively via callbacks triggered by master events, aligning with standard I2C slave behavior.

Major Changes in Usage
~~~~~~~~~~~~~~~~~~~~~~

- ``i2c_slave_receive`` has been removed. In the new driver, data reception is handled via callbacks.
- ``i2c_slave_transmit`` has been replaced by ``i2c_slave_write``.
- ``i2c_slave_write_ram`` has been removed。
- ``i2c_slave_read_ram`` has been removed。
