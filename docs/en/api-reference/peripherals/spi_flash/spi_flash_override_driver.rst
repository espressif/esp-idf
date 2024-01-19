Overriding Default Chip Drivers
===============================

.. warning::

   Customizing SPI Flash Chip Drivers is considered an "expert" feature. Users should only do so at their own risk. (See the notes below)

During the SPI Flash driver's initialization (i.e., :cpp:func:`esp_flash_init`), there is a chip detection step during which the driver iterates through a Default Chip Driver List and determine which chip driver can properly support the currently connected flash chip. The Default Chip Drivers are provided by the ESP-IDF, thus are updated in together with each ESP-IDF version. However ESP-IDF also allows users to customize their own chip drivers.

Users should note the following when customizing chip drivers:

   1. You may need to rely on some non-public ESP-IDF functions, which have slight possibility to change between ESP-IDF versions. On the one hand, these changes may be useful bug fixes for your driver, on the other hand, they may also be breaking changes (i.e., breaks your code).
   2. Some ESP-IDF bug fixes to other chip drivers are not automatically applied to your own custom chip drivers.
   3. If the protection of flash is not handled properly, there may be some random reliability issues.
   4. If you update to a newer ESP-IDF version that has support for more chips, you will have to manually add those new chip drivers into your custom chip driver list. Otherwise the driver will only search for the drivers in custom list you provided.


Steps For Creating Custom Chip Drivers and Overriding the ESP-IDF Default Driver List
-------------------------------------------------------------------------------------

.. highlight: cmake

1. Enable the :ref:`CONFIG_SPI_FLASH_OVERRIDE_CHIP_DRIVER_LIST` config option. This prevents compilation and linking of the Default Chip Driver List (``default_registered_chips``) provided by ESP-IDF. Instead, the linker searches for the structure of the same name (``default_registered_chips``) that must be provided by the user.
2. Add a new component in your project, e.g., ``custom_chip_driver``.
3. Copy the necessary chip driver files from the ``spi_flash`` component in ESP-IDF. This may include:

    - ``spi_flash_chip_drivers.c`` (to provide the ``default_registered_chips`` structure)
    - Any of the ``spi_flash_chip_*.c`` files that matches your own flash model best
    - ``CMakeLists.txt`` and ``linker.lf`` files

   Modify the files above properly. Including:

   - Change the ``default_registered_chips`` variable to non-static and remove the #ifdef logic around it.
   - Update ``linker.lf`` file to rename the fragment header and the library name to match the new component.
   - If reusing other drivers, some header names need prefixing with ``spi_flash/`` when included from outside spi_flash component.

.. note::

   - When writing your own flash chip driver, you can set your flash chip capabilities through ``spi_flash_chip_***(vendor)_get_caps`` and points the function pointer ``get_chip_caps`` for protection to the ``spi_flash_chip_***_get_caps`` function. The steps are as follows.

      1. Please check whether your flash chip have the capabilities listed in ``spi_flash_caps_t`` by checking the flash datasheet.
      2. Write a function named ``spi_flash_chip_***(vendor)_get_caps``. Take the example below as a reference. (if the flash support ``suspend`` and ``read unique id``).
      3. Points the pointer ``get_chip_caps`` (in ``spi_flash_chip_t``) to the function mentioned above.

      .. code-block:: c

         spi_flash_caps_t spi_flash_chip_***(vendor)_get_caps(esp_flash_t *chip)
         {
            spi_flash_caps_t caps_flags = 0;
            // 32-bit-address flash is not supported
            flash-suspend is supported
            caps_flags |= SPI_FLAHS_CHIP_CAP_SUSPEND;
            // flash read unique id.
            caps_flags |= SPI_FLASH_CHIP_CAP_UNIQUE_ID;
            return caps_flags;
         }

      .. code-block:: c

         const spi_flash_chip_t esp_flash_chip_eon = {
            // Other function pointers
            .get_chip_caps = spi_flash_chip_eon_get_caps,
         };

   - You also can see how to implement this in the example :example:`storage/custom_flash_driver`.

4. Write a new ``CMakeLists.txt`` file for the ``custom_chip_driver`` component, including an additional line to add a linker dependency from ``spi_flash`` to ``custom_chip_driver``::

        idf_component_register(SRCS "spi_flash_chip_drivers.c"
                               "spi_flash_chip_mychip.c"  # modify as needed
                               REQUIRES hal
                               PRIV_REQUIRES spi_flash
                               LDFRAGMENTS linker.lf)
        idf_component_add_link_dependency(FROM spi_flash)

   - An example of this component CMakeLists.txt can be found in :example_file:`storage/custom_flash_driver/components/custom_chip_driver/CMakeLists.txt`

5. The ``linker.lf`` is used to put every chip driver that you are going to use whilst cache is disabled into internal RAM. See :doc:`/api-guides/linker-script-generation` for more details. Make sure this file covers all the source files that you add.

6. Build your project, and you will see the new flash driver is used.

Example
-------

See also :example:`storage/custom_flash_driver`.
