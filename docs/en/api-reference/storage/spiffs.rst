SPIFFS Filesystem
=================

Overview
--------

SPIFFS is a file system intended for SPI NOR flash devices on embedded targets.
It supports wear leveling, file system consistency checks and more.

Notes
-----

 - Currently, SPIFFS does not support directories. It produces a flat structure. If SPIFFS is mounted under ``/spiffs``, then creating a file with path ``/spiffs/tmp/myfile.txt`` will create a file called ``/tmp/myfile.txt`` in SPIFFS, instead of ``myfile.txt`` under directory ``/spiffs/tmp``.
 - It is not a realtime stack. One write operation might last much longer than another.
 - Currently, it does not detect or handle bad blocks.

Tools
-----

spiffsgen.py
^^^^^^^^^^^^

:component_file:`spiffsgen.py<spiffs/spiffsgen.py>` is a write-only Python SPIFFS implementation used to create filesystem
images from the contents of a host folder. To use ``spiffsgen.py``, simply invoke it from your favorite terminal::

    python spiffsgen.py <image_size> <base_dir> <output_file>

- image_size: size of the partition on which the created SPIFFS image will be flashed to
- base_dir: directory to create the SPIFFS image of
- output_file: SPIFFS image output file

Besides the three required arguments: *image_size*, *base_dir* and *output_file*, there are other arguments
that control image generation. Documentation on these arguments exist in the tool's help::

    python spiffsgen.py --help

These optional arguments correspond to possible SPIFFS build configuration. 
User should make sure that the image is generated with the same arguments/configuration as
SPIFFS was built with, else the user ends up with an invalid image. As a guide, the help output indicates the SPIFFS
build configuration the argument corresponds to. In cases when these arguments
are not specified, the default values shown in the help output are used.

Once the image has been created, it can be flashed using ``esptool.py`` or ``parttool.py``.

Aside from invoking ``spiffsgen.py`` standalone, it is also possible to use it directly from the build system by calling
``spiffs_create_partition_image``.

Make::

    $(eval $(call spiffs_create_partition_image,<partition>,<base_dir>,[FLASH_IN_PROJECT]))

CMake::

    spiffs_create_partition_image(<partition> <base_dir> [FLASH_IN_PROJECT])

This is more convenient as the build configuration is automatically passed to the tool, 
ensuring that the image generated is valid for that build. An example of this is while the *image_size* is required
for the standalone invocation, only the *partition* name is required when using ``spiffs_create_partition_image`` --
the image size is automatically obtained from the project's partition table. 
It is important to note that due to the differences in structure between the two build systems, 
when using Make, ``spiffs_create_partition_image`` must be called from the project Makefile;
for CMake, it should be called from one of the component CMakeLists.txt. For both build systems, the image will be created in the build directory
with filename *partition*.bin. 

Optionally, user can opt to have the image automatically flashed together with the app binaries, partition tables, etc. on
``idf.py flash`` or ``make flash`` by specifying ``FLASH_IN_PROJECT``.  For example::

    spiffs_create_partition_image(my_spiffs_partition my_folder FLASH_IN_PROJECT)

If FLASH_IN_PROJECT is not specified, the image is still generated,
but user has to flash it manually using ``esptool.py``, ``parttool.py`` or a custom build system target.

For an example, see :example:`examples/storage/spiffsgen>`.


mkspiffs
^^^^^^^^

Another tool for creating SPIFS partition images is `mkspiffs <https://github.com/igrr/mkspiffs>`_.
Like ``spiffsgen.py``, it can be used to create image from a given folder and then flash that image with ``esptool.py``

To do that you need to obtain some parameters:

- Block Size: 4096 (standard for SPI Flash)
- Page Size: 256 (standard for SPI Flash)
- Image Size: Size of the partition in bytes (can be obtained from partition table)
- Partition Offset: Starting address of the partition (can be obtained from partition table)

To pack a folder into 1 Megabyte image::

    mkspiffs -c [src_folder] -b 4096 -p 256 -s 0x100000 spiffs.bin

To flash the image to ESP32 at offset 0x110000::

    python esptool.py --chip esp32 --port [port] --baud [baud] write_flash -z 0x110000 spiffs.bin


Notes on which SPIFFS tool to use
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The two tools presented above offer very similar functionality. There are, however, reasons to prefer one 
over the other depending on the use case.

If the intent is to simply generate a SPIFFS image during build, ``spiffsgen.py`` makes it very convenient
by providing functions/commands from the build system itself. This makes it easy to generate SPIFFS images
that match the build configuration and can be flashed together with the application. 
Another case for choosing ``spiffsgen.py`` is when the host has no C/C++ compiler available, since ``mkspiffs`` 
requires compilation.

On the other hand, ``mkspiffs`` offers unpacking SPIFFS images in addition to image generation. This is not
possible with ``spiffsgen.py``, at least not yet. There might also be environments where a Python interpreter
is not available, but a host compiler is or a pre-compiled ``mkspiffs`` binary 
can do the job. However, there is no build system integration for ``mkspiffs`` and the user has to 
do the corresponding work: compiling ``mkspiffs`` during build (if a pre-compiled binary is not used), creating build rules/targets
for the output files, passing proper parameters to the tool, etc.


See also
--------

- :doc:`Partition Table documentation <../../api-guides/partition-tables>`

Application Example
-------------------

An example for using SPIFFS is provided in :example:`storage/spiffs` directory. This example initializes and mounts SPIFFS partition, and writes and reads data from it using POSIX and C library APIs. See README.md file in the example directory for more information.

High level API Reference
------------------------

* :component_file:`spiffs/include/esp_spiffs.h`

.. include:: /_build/inc/esp_spiffs.inc
