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
