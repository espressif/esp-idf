Overriding Default Chip Drivers
===============================

.. warning::
   Customizing SPI Flash Chip Drivers is considered an "expert" feature. Users should only do so at their own risk. (See the notes below)

During the SPI Flash driver's initialization (i.e., :cpp:func:`esp_flash_init`), there is a chip detection step during which the driver will iterate through a Default Chip Driver List and determine which chip driver can properly support the currently connected flash chip. The Default Chip Drivers are provided by the IDF, thus are updated in together with each IDF version. However IDF also allows users to customize their own chip drivers.

Users should note the following when customizing chip drivers:

   1. You may need to rely on some non-public IDF functions, which have slight possibility to change between IDF versions. On the one hand, these changes may be useful bug fixes for your driver, on the other hand, they may also be breaking changes (i.e., breaks your code).
   2. Some IDF bug fixes to other chip drivers will not be automatically applied to your own custom chip drivers.
   3. If the protection of flash is not handled properly, there may be some random reliability issues.
   4. If you update to a newer IDF version that has support for more chips, you will have to manually add those new chip drivers into your custom chip driver list. Otherwise the driver will only search for the drivers in custom list you provided.


Steps For Creating Custom Chip Drivers and Overriding the IDF Default Driver List
---------------------------------------------------------------------------------

1. Enable the :ref:`CONFIG_SPI_FLASH_OVERRIDE_CHIP_DRIVER_LIST` config option. This will prevent compilation and linking of the Default Chip Driver List (`default_registered_chips`) provided by IDF. Instead, the linker will search for the structure of the same name (`default_registered_chips`) that must be provided by the user.
2. Add a new component in your project, e.g. `custom_chip_driver`.
3. Copy the necessary chip driver files from the `spi_flash` component in IDF. This may include:

    - `spi_flash_chip_drivers.c` (to provide the `default_registered_chips` structure)
    - Any of the `spi_flash_chip_*.c` files that matches your own flash model best
    - `CMakeLists.txt` and `linker.lf` files

   Modify the files above properly.

4. Add linking dependency from `spi_flash` component to the new `custom_chip_driver` component, by adding the following lines after the `idf_component_register`, in the `CMakeLists.txt` file of the `custom_chip_driver` component:

      idf_component_get_property(spi_flash_lib spi_flash COMPONENT_LIB)
      set_property(TARGET ${spi_flash_lib} APPEND PROPERTY INTERFACE_LINK_LIBRARIES $<LINK_ONLY:${COMPONENT_LIB}>)

5. The `linker.lf` is used to put every chip driver that you are going to use whilst cache is disabled into internal RAM. See :doc:`/api-guides/linker-script-generation` for more details. Make sure this file covers all the source files that you add.

6. Build your project, and you will see the new flash driver is used.

Example
-------

See also :example:`storage/custom_flash_driver`.
