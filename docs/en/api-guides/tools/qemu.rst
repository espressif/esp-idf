QEMU Emulator
=============

:link_to_translation:`zh_CN:[中文]`

Espressif maintains a `fork <https://github.com/espressif/qemu>`_ of the QEMU emulator with support for {IDF_TARGET_NAME}. This fork implements emulation of the CPU, memory, and several peripherals of {IDF_TARGET_NAME}. For more information about QEMU for {IDF_TARGET_NAME}, see the `QEMU README documentation <https://github.com/espressif/esp-toolchain-docs/blob/main/qemu/README.md>`_.

:doc:`idf.py <idf-py>` allows for running and debugging applications in QEMU. This is a convenient way to test applications without having to flash them to real hardware.

Prerequisites
-------------

To use QEMU with ``idf.py``, you first need to install the above-mentioned fork of QEMU. ESP-IDF provides pre-built binaries for x86_64 and arm64 Linux and macOS, as well as x86_64 Windows. Before you use the pre-built binaries on Linux and macOS platroms please install system dependencies:

- Ubuntu and Debian:

.. code-block:: console

    sudo apt-get install -y libgcrypt20 libglib2.0-0 libpixman-1-0 libsdl2-2.0-0 libslirp0

- CentOS:

.. code-block:: console

    sudo yum install -y --enablerepo=powertools libgcrypt glib2 pixman SDL2 libslirp

- Arch:

.. code-block:: console

    sudo pacman -S --needed libgcrypt glib2 pixman sdl2 libslirp

- macOS:

.. code-block:: console

    brew install libgcrypt glib pixman sdl2 libslirp

Then install the pre-built binaries with the following command:

.. code-block:: console

    python $IDF_PATH/tools/idf_tools.py install qemu-xtensa qemu-riscv32

After installing QEMU, make sure it is added to PATH by running ``. ./export.sh`` in the IDF directory.

If you are using a different platform, you need to build QEMU from source. Refer to official QEMU documentation for instructions.

Usage
-----

Running an Application
~~~~~~~~~~~~~~~~~~~~~~

To run an IDF application in QEMU, use the following command:

.. code-block:: console

    idf.py qemu monitor

This command builds the application, starts QEMU and opens :doc:`IDF monitor <idf-monitor>`, and connects IDF Monitor to the emulated UART port. You can see the console output of the application and interact with it. IDF Monitor also provides automatic decoding of panic backtraces and UART core dumps.

Debugging
~~~~~~~~~

To debug an application in QEMU, use the following command:

.. code-block:: console

    idf.py qemu gdb

This command builds the application, starts QEMU with the GDB server enabled, and opens an interactive GDB session. You can use GDB to debug the application as if it was running on real hardware.

To see console output while debugging in QEMU, use two terminals.

* In the first terminal, run:

  .. code-block:: console

    idf.py qemu --gdb monitor

  This command starts QEMU and IDF Monitor, and tells QEMU to wait for a GDB connection.

* In the second terminal, run:

  .. code-block:: console

    idf.py gdb

  This command starts an interactive GDB sessions and connects it to QEMU. You can now debug the application, and the console output will be visible in the first terminal.

It is also possible to run QEMU without the IDF Monitor:

.. code-block:: console

    idf.py qemu

In this case, the IDF Monitor is not used, and you can interact with QEMU process directly. To switch between the emulated UART console and QEMU console ("QEMU monitor"), use Ctrl-A shortcut. For example, to exit QEMU, press Ctrl-A, then type ``q`` and press Enter. You can use the QEMU console to enter commands, such as for inspecting registers and memory.

Graphics Support
~~~~~~~~~~~~~~~~

QEMU supports a virtual framebuffer device. This device doesn't exist in the real {IDF_TARGET_NAME} hardware, but it can be used to test graphics applications in QEMU.

To launch QEMU with a virtual framebuffer device enabled, use the following command:

.. code-block:: console

    idf.py qemu --graphics monitor

When the ``--graphics`` option is used, QEMU opens an additional window where the framebuffer contents are displayed.

To use the virtual framebuffer device in your application, you can add the `espressif/esp_lcd_qemu_rgb <https://components.espressif.com/components/espressif/esp_lcd_qemu_rgb>`_ component to your project. This component provides an esp_lcd compatible driver for the virtual framebuffer device.
