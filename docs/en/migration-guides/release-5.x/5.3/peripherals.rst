Peripherals
===========

:link_to_translation:`zh_CN:[中文]`

In order to control the dependence of other components on drivers at a smaller granularity, the original peripheral drivers under the `driver`` component were split into separate components:

- `esp_driver_gptimer` - Driver for general purpose timers
- `esp_driver_pcnt` - Driver for pulse counter
- `esp_driver_gpio` - Driver for GPIO
- `esp_driver_spi` - Driver for GPSPI
- `esp_driver_mcpwm` - Driver for Motor Control PWM
- `esp_driver_sdmmc` - Driver for SDMMC
- `esp_driver_sdspi` - Driver for SDSPI
- `esp_driver_sdio` - Driver for SDIO
- `esp_driver_ana_cmpr` - Driver for Analog Comparator
- `esp_driver_i2s` - Driver for I2S
- `esp_driver_dac` - Driver for DAC
- `esp_driver_rmt` - Driver for RMT

For compatibility, the original `driver`` component is still treated as an all-in-one component by registering these `esp_driver_xyz`` components as its public dependencies. In other words, you do not need to modify the CMake file of an existing project, but you now have a way to specify the specific peripheral driver that your project depends on.

Originally, you may have used **linker.lf** to specify the link location of some driver functions in memory space, but now, because the location of the driver files have been moved, you need to make changes your **linker.lf** file accordingly. For example, a linker.lf file with the following entries:

.. code-block:: none

    [mapping:my_mapping_scheme]
    archive: libdriver.a
    entries:
        gpio (noflash)

Should be changed to:

.. code-block:: none

    [mapping:my_mapping_scheme]
    archive: libesp_driver_gpio.a
    entries:
        gpio (noflash)
