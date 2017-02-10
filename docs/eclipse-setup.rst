Build and Flash with Eclipse IDE
********************************

.. _eclipse-install-steps:

Installing Eclipse IDE
======================

The Eclipse IDE gives you a graphical integrated development environment for writing, compiling and debugging ESP-IDF projects.

* Start by installing the esp-idf for your platform (see files in this directory with steps for Windows, OS X, Linux).

* We suggest building a project from the command line first, to get a feel for how that process works. You also need to use the command line to configure your esp-idf project (via ``make menuconfig``), this is not currently supported inside Eclipse.

* Download the Eclipse Installer for your platform from eclipse.org_.

* When running the Eclipse Installer, choose "Eclipse for C/C++ Development" (in other places you'll see this referred to as CDT.)

Windows Users
=============

Using ESP-IDF with Eclipse on Windows requires different configuration steps. :ref:`See the Eclipse IDE on Windows guide <eclipse-windows-setup>`.

Setting up Eclipse
==================

Once your new Eclipse installation launches, follow these steps:

Import New Project
------------------

* Eclipse makes use of the Makefile support in ESP-IDF. This means you need to start by creating an ESP-IDF project. You can use the idf-template project from github, or open one of the examples in the esp-idf examples subdirectory.

* Once Eclipse is running, choose File -> Import...

* In the dialog that pops up, choose "C/C++" -> "Existing Code as Makefile Project" and click Next.

* On the next page, enter "Existing Code Location" to be the directory of your IDF project. Don't specify the path to the ESP-IDF directory itself (that comes later). The directory you specify should contain a file named "Makefile" (the project Makefile).

* On the same page, under "Toolchain for Indexer Settings" choose "Cross GCC". Then click Finish.


Project Properties
------------------

* The new project will appear under Project Explorer. Right-click the project and choose Properties from the context menu.

* Click on the "Environment" properties page under "C/C++ Build". Click "Add..." and enter name ``BATCH_BUILD`` and value ``1``.

* Click "Add..." again, and enter name ``IDF_PATH``. The value should be the full path where ESP-IDF is installed.

*All users, continue with these steps:*

Navigate to "C/C++ General" -> "Preprocessor Include Paths" property page:

* Click the "Providers" tab

* In the list of providers, click "CDT Cross GCC Built-in Compiler Settings". Under "Command to get compiler specs", replace the text ``${COMMAND}`` at the beginning of the line with ``xtensa-esp32-elf-gcc``. This means the full "Command to get compiler specs" should be ``xtensa-esp32-elf-gcc ${FLAGS} -E -P -v -dD "${INPUTS}"``.

* In the list of providers, click "CDT GCC Build Output Parser" and type ``xtensa-esp32-elf-`` at the beginning of the Compiler command pattern. This means the full Compiler command pattern should be ``xtensa-esp32-elf-(g?cc)|([gc]\+\+)|(clang)``

.. _eclipse-build-project:

Building in Eclipse
-------------------

Before your project is first built, Eclipse may show a lot of errors and warnings about undefined values. This is because some source files are automatically generated as part of the esp-idf build process. These errors and warnings will go away after you build the project.

* Click OK to close the Properties dialog in Eclipse.

* Outside Eclipse, open a command line prompt. Navigate to your project directory, and run ``make menuconfig`` to configure your project's esp-idf settings. This step currently has to be run outside Eclipse.

*If you try to build without running a configuration step first, esp-idf will prompt for configuration on the command line - but Eclipse is not able to deal with this, so the build will hang or fail.*

* Back in Eclipse, choose Project -> Build to build your project.

**TIP**: If your project had already been built outside Eclipse, you may need to do a Project -> Clean before chosing Project -> Build. This is so Eclipse can see the compiler arguments for all source files. It uses these to determine the header include paths.

Flash from Eclipse
------------------

You can integrate the "make flash" target into your Eclipse project to flash using esptool.py from the Eclipse UI:

* Right-click your project in Project Explorer (important to make sure you select the project, not a directory in the project, or Eclipse may find the wrong Makefile.)

* Select Make Targets -> Create from the context menu.

* Type "flash" as the target name. Leave the other options as their defaults.

* Now you can use Project -> Make Target -> Build (Shift+F9) to build the custom flash target, which will compile and flash the project.

Note that you will need to use "make menuconfig" to set the serial port and other config options for flashing. "make menuconfig" still requires a command line terminal (see the instructions for your platform.)

Follow the same steps to add ``bootloader`` and ``partition_table`` targets, if necessary.

.. _eclipse.org: http://www.eclipse.org/

