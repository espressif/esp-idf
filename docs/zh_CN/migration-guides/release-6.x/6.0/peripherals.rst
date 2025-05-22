外设驱动
========

:link_to_translation:`en:[English]`

公共变化
--------

所有驱动的 ``io_loop_back`` 配置已被移除
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

不同的驱动对象可以共享同一个 GPIO 编号，联合起来可以实现更加复杂的功能。比如将 RMT 外设的 TX 通道和 RX 通道绑定在同一个 GPIO 上，进而模拟单总线的读写时序。在以前的版本中，你需要在驱动的配置中额外设置 ``io_loop_back`` 来实现这种“回环”功能，现在，这个配置已经被移除。不同的驱动只需要在配置中设置相同的 GPIO 编号就能实现这个功能。

RMT
---

:cpp:type:`rmt_tx_channel_config_t` 配置结构体中的 ``io_od_mode`` 已经被移除。如果想要使用开漏模式，你需要手动调用 GPIO 驱动中的 :func:`gpio_od_enable` 函数。

MCPWM
-----

:cpp:type:`mcpwm_generator_config_t` 配置结构体中的 ``io_od_mode`` 已经被移除。如果想要使用开漏模式，你需要手动调用 GPIO 驱动中的 :func:`gpio_od_enable` 函数。

以下配置结构体中的 ``pull_up`` 和 ``pull_down`` 成员已经被移除，你需要手动调用 GPIO 驱动中的 :func:`gpio_set_pull_mode` 函数来配置 IO 上拉和下拉电阻：

.. list::

    - :cpp:type:`mcpwm_generator_config_t`
    - :cpp:type:`mcpwm_gpio_fault_config_t`
    - :cpp:type:`mcpwm_gpio_sync_src_config_t`
    - :cpp:type:`mcpwm_capture_channel_config_t`

I2C
---

I2C 从机在 v5.4 上已经被重新设计。在当前版本上，老的 I2C 从机驱动已经被移除，详细内容请参考编程指南中关于 I2C 从机的部分。

主要的概念上和用法上的改变如下所示:

主要概念更新
~~~~~~~~~~~~~~~~~~

- 老版本的 I2C 从机驱动是主动读写，这不符合 I2C 从机的一般用法。在新版的 I2C 从机中，I2C 的读写通过主机驱动产生的事件以触发回调被动完成。

主要用法更新
~~~~~~~~~~~~~~~~~~

- ``i2c_slave_receive`` 被移除， 在新驱动中使用回调接收数据。
- ``i2c_slave_transmit`` 已被 ``i2c_slave_write`` 取代.
- ``i2c_slave_write_ram`` 被移除。
- ``i2c_slave_read_ram`` 被移除。
