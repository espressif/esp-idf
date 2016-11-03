Build and Flash with Eclipse IDE
********************************

Installing Eclipse IDE
======================

The Eclipse IDE gives you a graphical integrated development environment for writing, compiling and debugging ESP-IDF projects.

* Start by installing the esp-idf for your platform (see files in this directory with steps for Windows, OS X, Linux).

* Download the Eclipse Installer for your platform from eclipse.org_.

* When running the Eclipse Installer, choose "Eclipse for C/C++ Development" (in other places you'll see this referred to as CDT.)

Setting up Eclipse
==================

Once your new Eclipse installation launches, follow these steps:

Import New Project
------------------

* Eclipse makes use of the Makefile support in ESP-IDF. This means you need to start by creating an ESP-IDF project. You can use the skeleton project from github.

* Once Eclipse is running, choose File -> Import...

* In the dialog that pops up, choose "C/C++" -> "Existing Code as Makefile Project" and click Next.

* On the next page, enter "Existing Code Location" to be the directory of your IDF project. Don't specify the path to the ESP-IDF directory itself.

* On the same page, under "Toolchain for Indexer Settings" choose "Cross GCC". Then click Finish.


Project Properties
------------------

* The new project will appear under Project Explorer. Right-click the project and choose Properties from the context menu.

* Click on the "Environment" properties page under "C/C++ Build". Click "Add..." and enter name ``V`` and value ``1``.

* Click "Add..." again, and enter name ``IDF_PATH``. The value should be the full path where ESP-IDF is installed. *Windows users: Use forward-slashes not backslashes for this path, ie C:/Users/MyUser/Development/esp-idf*.

*Windows users only, follow these two additional steps:*

* On the same Environment property page, edit the PATH environment variable. Delete the existing value and replace it with ``C:\msys32\usr\bin;C:\msys32\mingw32\bin;C:\msys32\opt\xtensa-esp32-elf\bin`` (If you installed msys32 to a different directory then you'll need to change these paths to match).

* Click on the "C/C++ Build" top-level properties page then uncheck "Use default build command" and enter this for the custom build command: ``bash ${IDF_PATH}/tools/windows/eclipse_make.sh``.

*All users, continue with these steps:*

Navigate to "C/C++ General" -> "Preprocessor Include Paths" property page:

* Click the "Providers" tab

* In the list of providers, click "CDT Cross GCC Built-in Compiler Settings". Under "Command to get compiler specs", replace the text ``${COMMAND}`` at the beginning of the line with ``xtensa-esp32-elf-gcc``. This means the full "Command to get compiler specs" should be ``xtensa-esp32-elf-gcc ${FLAGS} -E -P -v -dD "${INPUTS}"``.

* In the list of providers, click "CDT GCC Build Output Parser" and type ``xtensa-esp32-elf-`` at the beginning of the Compiler command pattern. This means the full Compiler command pattern should be ``xtensa-esp32-elf-(g?cc)|([gc]\+\+)|(clang)``

* Click OK to close the Properties dialog, and choose Project -> Build to build your project.

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

Eclipse Troubleshooting
-----------------------

* ``*** Make was invoked from ... However please do not run make from the sdk or a component directory; ...`` - Eclipse will detect any directory with a Makefile in it as being a possible directory to run "make" in. All component directories also contain a Makefile (the wrong one), so it is important when using Project -> Make Target to always select the top-level project directory in Project Explorer.
