**********************
Eclipse IDE on Windows
**********************

Configuring Eclipse on Windows requires some different steps. The full configuration steps for Windows are shown below.

(For OS X and Linux instructions, see the :doc:`Eclipse IDE page <eclipse-setup>`.)

Installing Eclipse IDE
======================

Follow the steps under :ref:`Installing Eclipse IDE <eclipse-install-steps>` for all platforms.

.. _eclipse-windows-setup:

Setting up Eclipse on Windows
=============================

Once your new Eclipse installation launches, follow these steps:

Import New Project
------------------

* Eclipse makes use of the Makefile support in ESP-IDF. This means you need to start by creating an ESP-IDF project. You can use the idf-template project from github, or open one of the examples in the esp-idf examples subdirectory.

* Once Eclipse is running, choose File -> Import...

* In the dialog that pops up, choose "C/C++" -> "Existing Code as Makefile Project" and click Next.

* On the next page, enter "Existing Code Location" to be the directory of your IDF project. Don't specify the path to the ESP-IDF directory itself (that comes later). The directory you specify should contain a file named "Makefile" (the project Makefile).

* On the same page, under "Toolchain for Indexer Settings" uncheck "Show only available toolchains that support this platform".

* On the extended list that appears, choose "Cygwin GCC". Then click Finish.

*Note: you may see warnings in the UI that Cygwin GCC Toolchain could not be found. This is OK, we're going to reconfigure Eclipse to find our toolchain.*

Project Properties
------------------

* The new project will appear under Project Explorer. Right-click the project and choose Properties from the context menu.

* Click on the "C/C++ Build" properties page (top-level):

  * Uncheck "Use default build command" and enter this for the custom build command: ``python ${IDF_PATH}/tools/windows/eclipse_make.py``.

* Click on the "Environment" properties page under "C/C++ Build":

  * Click "Add..." and enter name ``BATCH_BUILD`` and value ``1``.

  * Click "Add..." again, and enter name ``IDF_PATH``. The value should be the full path where ESP-IDF is installed. The IDF_PATH directory should be specified using forwards slashes not backslashes, ie *C:/Users/MyUser/Development/esp-idf*.

  * Edit the PATH environment variable. Delete the existing value and replace it with ``C:\msys32\usr\bin;C:\msys32\mingw32\bin;C:\msys32\opt\xtensa-esp32-elf\bin`` (If you installed msys32 to a different directory then you'll need to change these paths to match).

* Click on "C/C++ General" -> "Preprocessor Include Paths, Macros, etc." property page:

  * Click the "Providers" tab

     * In the list of providers, click "CDT GCC Built-in Compiler Settings Cygwin". Under "Command to get compiler specs", replace the text ``${COMMAND}`` at the beginning of the line with ``xtensa-esp32-elf-gcc``. This means the full "Command to get compiler specs" should be ``xtensa-esp32-elf-gcc ${FLAGS} -E -P -v -dD "${INPUTS}"``.

     * In the list of providers, click "CDT GCC Build Output Parser" and type ``xtensa-esp32-elf-`` at the beginning of the Compiler command pattern, and wrap remaining part with brackets. This means the full Compiler command pattern should be ``xtensa-esp32-elf-((g?cc)|([gc]\+\+)|(clang))``


Building in Eclipse
-------------------

Continue from :ref:`Building in Eclipse <eclipse-build-project>` for all platforms.

Technical Details
=================

**Of interest to Windows gurus or very curious parties, only.**

Explanations of the technical reasons for some of these steps. You don't need to know this to use esp-idf with Eclipse on Windows, but it may be helpful background knowledge if you plan to do dig into the Eclipse support:

* The xtensa-esp32-elf-gcc cross-compiler is *not* a Cygwin toolchain, even though we tell Eclipse that it is one. This is because msys2 uses Cygwin and supports Unix-style paths (of the type ``/c/blah`` instead of ``c:/blah`` or ``c:\\blah``). In particular, xtensa-esp32-elf-gcc reports to the Eclipse "built-in compiler settings" function that its built-in include directories are all under ``/usr/``, which is a Unix/Cygwin-style path that Eclipse otherwise can't resolve. By telling Eclipse the compiler is Cygwin, it resolves these paths internally using the ``cygpath`` utility.

* The same problem occurs when parsing make output from esp-idf. Eclipse parses this output to find header directories, but it can't resolve include directories of the form ``/c/blah`` without using ``cygpath``. There is a heuristic that Eclipse Build Output Parser uses to determine whether it should call ``cygpath``, but for currently unknown reasons the esp-idf configuration doesn't trigger it. For this reason, the ``eclipse_make.py`` wrapper script is used to call ``make`` and then use ``cygpath`` to process the output for Eclipse.
