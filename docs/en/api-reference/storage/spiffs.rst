SPIFFS Filesystem
=================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

SPIFFS is a file system intended for SPI NOR flash devices on embedded targets. It supports wear levelling, file system consistency checks, and more.

Notes
-----

 - Currently, SPIFFS does not support directories, it produces a flat structure. If SPIFFS is mounted under ``/spiffs``, then creating a file with the path ``/spiffs/tmp/myfile.txt`` will create a file called ``/tmp/myfile.txt`` in SPIFFS, instead of ``myfile.txt`` in the directory ``/spiffs/tmp``.
 - It is not a real-time stack. One write operation might take much longer than another.
 - For now, it does not detect or handle bad blocks.
 - SPIFFS is able to reliably utilize only around 75% of assigned partition space.
 - When the filesystem is running out of space, the garbage collector is trying to find free space by scanning the filesystem multiple times, which can take up to several seconds per write function call, depending on required space. This is caused by the SPIFFS design and the issue has been reported multiple times (e.g., `here <https://github.com/espressif/esp-idf/issues/1737>`_) and in the official `SPIFFS github repository <https://github.com/pellepl/spiffs/issues/>`_. The issue can be partially mitigated by the `SPIFFS configuration <https://github.com/pellepl/spiffs/wiki/Configure-spiffs>`_.
 - When the garbage collector attempts to reclaim space by scanning the entire filesystem multiple times (usually 10 times by default), during each scan, the garbage collector frees up one block if available. Therefore, if the maximum number of runs set for the garbage collector is 'n' (configured by the SPIFFS_GC_MAX_RUNS option located in `SPIFFS configuration <https://github.com/pellepl/spiffs/wiki/Configure-spiffs>`_), then n times the block size will become available for data writing. If you attempt to write data exceeding n times the block size, the write operation may fail and return an error.
 - When the chip experiences a power loss during a file system operation it could result in SPIFFS corruption. However the file system still might be recovered via ``esp_spiffs_check`` function. More details in the official SPIFFS `FAQ <https://github.com/pellepl/spiffs/wiki/FAQ>`_.

Tools
-----

``spiffsgen.py``
^^^^^^^^^^^^^^^^

:component_file:`spiffsgen.py<spiffs/spiffsgen.py>` is a write-only Python SPIFFS implementation used to create filesystem images from the contents of a host folder. To use ``spiffsgen.py``, open Terminal and run::

    python spiffsgen.py <image_size> <base_dir> <output_file>

The required arguments are as follows:

- **image_size**: size of the partition onto which the created SPIFFS image will be flashed.
- **base_dir**: directory for which the SPIFFS image needs to be created.
- **output_file**: SPIFFS image output file.

There are also other arguments that control image generation. Documentation on these arguments can be found in the tool's help::

    python spiffsgen.py --help

These optional arguments correspond to a possible SPIFFS build configuration. To generate the right image, please make sure that you use the same arguments/configuration as were used to build SPIFFS. As a guide, the help output indicates the SPIFFS build configuration to which the argument corresponds. In cases when these arguments are not specified, the default values shown in the help output will be used.

When the image is created, it can be flashed using ``esptool.py`` or ``parttool.py``.

Aside from invoking the ``spiffsgen.py`` standalone by manually running it from the command line or a script, it is also possible to invoke ``spiffsgen.py`` directly from the build system by calling ``spiffs_create_partition_image``::

    spiffs_create_partition_image(<partition> <base_dir> [FLASH_IN_PROJECT] [DEPENDS dep dep dep...])

This is more convenient as the build configuration is automatically passed to the tool, ensuring that the generated image is valid for that build. An example of this is while the **image_size** is required for the standalone invocation, only the **partition** name is required when using ``spiffs_create_partition_image`` -- the image size is automatically obtained from the project's partition table.

``spiffs_create_partition_image`` must be called from one of the component ``CMakeLists.txt`` files.

Optionally, users can opt to have the image automatically flashed together with the app binaries, partition tables, etc. on ``idf.py flash`` by specifying ``FLASH_IN_PROJECT``.  For example::

    spiffs_create_partition_image(my_spiffs_partition my_folder FLASH_IN_PROJECT)

If FLASH_IN_PROJECT/SPIFFS_IMAGE_FLASH_IN_PROJECT is not specified, the image will still be generated, but you will have to flash it manually using ``esptool.py``, ``parttool.py``, or a custom build system target.

There are cases where the contents of the base directory itself is generated at build time. Users can use DEPENDS/SPIFFS_IMAGE_DEPENDS to specify targets that should be executed before generating the image::

    add_custom_target(dep COMMAND ...)

    spiffs_create_partition_image(my_spiffs_partition my_folder DEPENDS dep)

For an example, see :example:`storage/spiffsgen`.

``mkspiffs``
^^^^^^^^^^^^

Another tool for creating SPIFFS partition images is `mkspiffs <https://github.com/igrr/mkspiffs>`_. Similar to ``spiffsgen.py``, it can be used to create an image from a given folder and then flash that image using ``esptool.py``

For that, you need to obtain the following parameters:

- **Block Size**: 4096 (standard for SPI Flash)
- **Page Size**: 256 (standard for SPI Flash)
- **Image Size**: Size of the partition in bytes (can be obtained from a partition table)
- **Partition Offset**: Starting address of the partition (can be obtained from a partition table)

To pack a folder into a 1-Megabyte image, run::

    mkspiffs -c [src_folder] -b 4096 -p 256 -s 0x100000 spiffs.bin

To flash the image onto {IDF_TARGET_NAME} at offset 0x110000, run::

    python esptool.py --chip {IDF_TARGET_PATH_NAME} --port [port] --baud [baud] write_flash -z 0x110000 spiffs.bin

.. note::

    You can configure the ``write_flash`` command of ``esptool.py`` to `write the spiffs data to an external SPI flash chip <https://docs.espressif.com/projects/esptool/en/latest/esptool/advanced-options.html#custom-spi-pin-configuration>`_ using the ``--spi-connection <CLK>,<Q>,<D>,<HD>,<CS>`` option. Just specify the GPIO pins assigned to the external flash, e.g. ``python esptool.py write_flash --spi-connection 6,7,8,9,11 -z 0x110000 spiffs.bin``.

Notes on Which SPIFFS Tool to Use
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The two tools presented above offer very similar functionality. However, there are reasons to prefer one over the other, depending on the use case.

Use ``spiffsgen.py`` in the following cases:

1. If you want to simply generate a SPIFFS image during the build. ``spiffsgen.py`` makes it very convenient by providing functions/commands from the build system itself.
2. If the host has no C/C++ compiler available, because ``spiffsgen.py`` does not require compilation.

Use ``mkspiffs`` in the following cases:

1. If you need to unpack SPIFFS images in addition to image generation. For now, it is not possible with ``spiffsgen.py``.
2. If you have an environment where a Python interpreter is not available, but a host compiler is available. Otherwise, a pre-compiled ``mkspiffs`` binary can do the job. However, there is no build system integration for ``mkspiffs`` and the user has to do the corresponding work: compiling ``mkspiffs`` during build (if a pre-compiled binary is not used), creating build rules/targets for the output files, passing proper parameters to the tool, etc.

See Also
--------

- :doc:`Partition Table documentation <../../api-guides/partition-tables>`


Application Example
-------------------

An example of using SPIFFS is provided in the :example:`storage/spiffs` directory. This example initializes and mounts a SPIFFS partition, then writes and reads data from it using POSIX and C library APIs. See the README.md file in the example directory for more information.

High-level API Reference
------------------------

.. include-build-file:: inc/esp_spiffs.inc
