外设
====

:link_to_translation:`en:[English]`

驱动程序
---------------

为了细粒度地控制其他组件对外设驱动的依赖，原先位于 ``driver`` 组件下的驱动程序被拆分到了各自独立的组件中。这些组件包括：

- `esp_driver_gptimer` - 通用定时器驱动
- `esp_driver_pcnt` - 脉冲计数器驱动
- `esp_driver_gpio` - GPIO 驱动
- `esp_driver_spi` - 通用 SPI 驱动
- `esp_driver_mcpwm` - 电机控制 PWM 驱动
- `esp_driver_sdmmc` - SDMMC 驱动
- `esp_driver_sdspi` - SDSPI 驱动
- `esp_driver_sdio` - SDIO 驱动
- `esp_driver_ana_cmpr` - 模拟比较器驱动
- `esp_driver_i2s` - I2S 驱动
- `esp_driver_dac` - DAC 驱动
- `esp_driver_rmt` - RMT 驱动
- `esp_driver_tsens` - 温度传感器驱动
- `esp_driver_sdm` - Sigma-Delta 调制器驱动
- `esp_driver_i2c` - I2C 驱动
- `esp_driver_uart` - UART 驱动
- `esp_driver_ledc` - LEDC 驱动
- `esp_driver_parlio` - 并行 IO 驱动
- `esp_driver_usb_serial_jtag` - USB_SERIAL_JTAG 驱动

为了兼容性，原来的 ``driver`` 组件仍然存在，并作为一个 “all-in-one" 的组件，将以上这些 `esp_driver_xyz` 组件注册成自己的公共依赖。换句话说，你无需修改既有项目的 CMake 文件，但是你现在多了一个途径去指定你项目依赖的具体的外设驱动。

原来你可能使用 **linker.lf** 指定了一些驱动函数在内存空间的链接位置，但是现在，因为驱动文件的位置发生了挪动，就需要你对 **linker.lf** 文件做出相应的改动的。假如，你的 linker.lf 文件里面有以下的条目：

.. code-block:: none

    [mapping:my_mapping_scheme]
    archive: libdriver.a
    entries:
        gpio (noflash)

现在需要修改成：

.. code-block:: none

    [mapping:my_mapping_scheme]
    archive: libesp_driver_gpio.a
    entries:
        gpio (noflash)


安全元素
--------------

ATECC608A 安全元素接口示例现已移至 GitHub 上的 `esp-cryptoauthlib 仓库 <https://github.com/espressif/esp-cryptoauthlib/tree/master/examples/atecc608_ecdsa>`_ 中。

该示例也是组件管理器注册表中 `esp-cryptoauthlib <https://components.espressif.com/component/espressif/esp-cryptoauthlib>`_ 的一部分。

I2S
-------

回调事件 :cpp:type:`i2s_event_data_t` 中指向 DMA 数组的二级指针 ``data`` 因使用过于繁琐已被弃用，请使用新增的指向 DMA 数组的一级指针 ``dma_buf`` 字段代替。
