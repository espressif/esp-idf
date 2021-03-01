Override Default Chip Drivers
=============================

The flash driver has a chip detection step, during which the driver go through the default chip driver list and see which driver can properly fit the current flash chip. The default chip driver is provided by the IDF, and will update together with IDF version. However IDF also allows user customizing their own chip drivers.

.. note::
   Customize the flash driver is an expert feature, please use it on your own risk:

   1. You may have to rely on some non-public IDF functions, which have slightly possibility to change among versions. On one hand, these changes may be useful bug fixes for your driver, on the other hand, they may also break your code.
   2. Some IDF bug fixes to other chip drivers will not be automatically applied to your own custom chip drivers.
   3. If the protection of flash is not handled properly, there may be some random reliability issue.

Steps of Making Custom Chip Driver and Override the IDF Default Driver
----------------------------------------------------------------------

1. Enables the SPI_FLASH_OVERRIDE_CHIP_DRIVER_LIST config option. The default chip driver list (`default_registered_chips`) provided by IDF will no longer be compiled or linked. Instead, the linker will search for the structure, which is supposed to be provided by the user.
2. Add a new component in your project, e.g. 'custom_chip_driver'.
3. Add dependency from `spi_flash` component to the new `custom_chip_driver` component, by a CMake file `project_include.cmake` under the component folder, with the following code:
   ```
   idf_build_set_property(___COMPONENT_REQUIRES_COMMON idf::custom_chip_driver APPEND)
   ```
4. Copy the necessary chip driver files from IDF spi_flash component. It may includes:

    - spi_flash_chip_drivers.c (to provide the `default_registered_chips` structure)
    - any of the `spi_flash_chip_*.c` files that matches your own flash model best
    - CMakeLists.txt and linker.lf file

   Modify the files above properly.

5. The linker.lf is used to put every you are going to use, when the cache is disabled, into internal RAM. Make sure this file covers all the source files you add.

6. Build your project, and you will see the new flash driver is used.

Example
-------

See also :example:`storage/custom_flash_driver`.
