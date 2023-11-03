外设
====

:link_to_translation:`en:[English]`

为了细粒度地控制其他组件对外设驱动的依赖，原先位于 `driver` 组件下的驱动程序被拆分到了各自独立的组件中。这些组件包括：

- `esp_driver_gptimer` - 通用定时器驱动
- `esp_driver_pcnt` - 脉冲计数器驱动
- `esp_driver_gpio` - GPIO 驱动

为了兼容性，原来的 `driver` 组件仍然存在，并作为一个 “all-in-one" 的组件，将以上这些 `esp_driver_xyz` 组件注册成自己的公共依赖。换句话说，你无需修改既有项目的 CMake 文件，但是你现在多了一个途径去指定你项目依赖的具体的外设驱动。

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
