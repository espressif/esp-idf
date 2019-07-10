**************************************************
Build and Flash with Eclipse IDE (Legacy GNU Make)
**************************************************
:link_to_translation:`zh_CN:[中文]`

.. include:: ../gnu-make-legacy.rst
                     
.. _eclipse-install-steps-legacy:

Installing Eclipse IDE
======================

The Eclipse IDE gives you a graphical integrated development environment for writing, compiling and debugging ESP-IDF projects.

* Start by installing the esp-idf for your platform (see files in this directory with steps for Windows, OS X, Linux).

* We suggest building a project from the command line first, to get a feel for how that process works. You also need to use the command line to configure your esp-idf project (via ``make menuconfig``), this is not currently supported inside Eclipse.

* Download the Eclipse Installer for your platform from eclipse.org_.

* When running the Eclipse Installer, choose "Eclipse for C/C++ Development" (in other places you'll see this referred to as CDT.)

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

* Click "Add..." again, and enter name ``IDF_PATH``. The value should be the full path where ESP-IDF is installed. Windows users can copy the ``IDF_PATH`` from windows explorer.

* Edit the ``PATH`` environment variable. Keep the current value, and append the path to the Xtensa toolchain installed as part of IDF setup, if this is not already listed on the PATH. A typical path to the toolchain looks like ``/home/user-name/esp/xtensa-esp32-elf/bin``. Note that you need to add a colon ``:`` before the appended path. Windows users will need to prepend ``C:\msys32\mingw32\bin;C:\msys32\opt\xtensa-esp32-elf\bin;C:\msys32\usr\bin`` to ``PATH`` environment variable (If you installed msys32 to a different directory then you’ll need to change these paths to match).

* On macOS, add a ``PYTHONPATH`` environment variable and set it to ``/Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages``. This is so that the system Python, which has pyserial installed as part of the setup steps, overrides any built-in Eclipse Python.

**ADDITIONAL NOTE**: If either the IDF_PATH directory or the project directory is located outside ``C:\msys32\home`` directory, you will have to give custom build command in C/C++ Build properties as: ``python ${IDF_PATH}/tools/windows/eclipse_make.py`` (Please note that the build time may get significantly increased by this method.)

Navigate to "C/C++ General" -> "Preprocessor Include Paths" property page:

* Click the "Providers" tab

* In the list of providers, click "CDT Cross GCC Built-in Compiler Settings". Change "Command to get compiler specs" to ``xtensa-esp32-elf-gcc ${FLAGS} -std=c++11 -E -P -v -dD "${INPUTS}"``.

* In the list of providers, click "CDT GCC Build Output Parser" and change the "Compiler command pattern" to ``xtensa-esp32-elf-(gcc|g\+\+|c\+\+|cc|cpp|clang)``

Navigate to "C/C++ General" -> "Indexer" property page:

* Check "Enable project specific settings" to enable the rest of the settings on this page.

* Uncheck "Allow heuristic resolution of includes". When this option is enabled Eclipse sometimes fails to find correct header directories.

Navigate to "C/C++ Build" -> "Behavior" property page:

* Check "Enable parallel build" to enable multiple build jobs in parallel.

.. _eclipse-build-project-legacy:

Building in Eclipse
-------------------

Before your project is first built, Eclipse may show a lot of errors and warnings about undefined values. This is because some source files are automatically generated as part of the esp-idf build process. These errors and warnings will go away after you build the project.

* Click OK to close the Properties dialog in Eclipse.

* Outside Eclipse, open a command line prompt. Navigate to your project directory, and run ``make menuconfig`` to configure your project's esp-idf settings. This step currently has to be run outside Eclipse.

*If you try to build without running a configuration step first, esp-idf will prompt for configuration on the command line - but Eclipse is not able to deal with this, so the build will hang or fail.*

* Back in Eclipse, choose Project -> Build to build your project.

**TIP**: If your project had already been built outside Eclipse, you may need to do a Project -> Clean before choosing Project -> Build. This is so Eclipse can see the compiler arguments for all source files. It uses these to determine the header include paths.

Flash from Eclipse
------------------

You can integrate the "make flash" target into your Eclipse project to flash using esptool.py from the Eclipse UI:

* Right-click your project in Project Explorer (important to make sure you select the project, not a directory in the project, or Eclipse may find the wrong Makefile.)

* Select Build Targets -> Create... from the context menu.

* Type "flash" as the target name. Leave the other options as their defaults.

* Now you can use Project -> Build Target -> Build (Shift+F9) to build the custom flash target, which will compile and flash the project.

Note that you will need to use "make menuconfig" to set the serial port and other config options for flashing. "make menuconfig" still requires a command line terminal (see the instructions for your platform.)

Follow the same steps to add ``bootloader`` and ``partition_table`` targets, if necessary.


.. _eclipse.org: https://www.eclipse.org/

