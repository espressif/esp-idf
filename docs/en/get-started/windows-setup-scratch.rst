************************************
Setup Windows Toolchain from Scratch
************************************

Setting up the environment gives you some more control over the process, and also provides the information for advanced users to customize the install. The :doc:`pre-built environment <windows-setup>`, addressed to less experienced users, has been prepared by following these steps. 

To quickly setup the toolchain in standard way, using a prebuilt environment, proceed to section :doc:`windows-setup`.


.. _configure-windows-toolchain-from-scratch:

Configure Toolchain & Environment from Scratch
==============================================

This process involves installing MSYS2_, then installing the MSYS2_ and Python packages which ESP-IDF uses, and finally downloading and installing the Xtensa toolchain.

* Navigate to the MSYS2_ installer page and download the ``msys2-i686-xxxxxxx.exe`` installer executable (we only support a 32-bit MSYS environment, it works on both 32-bit and 64-bit Windows.) At time of writing, the latest installer is ``msys2-i686-20161025.exe``.

* Run through the installer steps. **Uncheck the "Run MSYS2 32-bit now" checkbox at the end.**

* Once the installer exits, open Start Menu and find "MSYS2 MinGW 32-bit" to run the terminal.

  *(Why launch this different terminal? MSYS2 has the concept of different kinds of environments. The default "MSYS" environment is Cygwin-like and uses a translation layer for all Windows API calls. We need the "MinGW" environment in order to have a native Python which supports COM ports.)*

* The ESP-IDF repository on github contains a script in the tools directory titled ``windows_install_prerequisites.sh``. If you haven't got a local copy of the ESP-IDF yet, that's OK - you can just download that one file in Raw format from here: :idf_raw:`tools/windows/windows_install_prerequisites.sh`. Save it somewhere on your computer.

* Type the path to the shell script into the MSYS2 terminal window. You can type it as a normal Windows path, but use forward-slashes instead of back-slashes. ie: ``C:/Users/myuser/Downloads/windows_install_prerequisites.sh``. You can read the script beforehand to check what it does.

* The ``windows_install_prerequisites.sh`` script will download and install packages for ESP-IDF support, and the ESP32 toolchain.


Troubleshooting
~~~~~~~~~~~~~~~

* While the install script runs, MSYS may update itself into a state where it can no longer operate. You may see errors like the following::

     *** fatal error - cygheap base mismatch detected - 0x612E5408/0x612E4408. This problem is probably due to using incompatible versions of the cygwin DLL.

  If you see errors like this, close the terminal window entirely (terminating the processes running there) and then re-open a new terminal. Re-run ``windows_install_prerequisites.sh`` (tip: use the up arrow key to see the last run command). The update process will resume after this step.

* MSYS2 is a "rolling" distribution so running the installer script may install newer packages than what is used in the prebuilt environments. If you see any errors that appear to be related to installing MSYS2 packages, please check the `MSYS2-packages issues list`_ for known issues. If you don't see any relevant issues, please `raise an IDF issue`_.


MSYS2 Mirrors in China
~~~~~~~~~~~~~~~~~~~~~~

There are some (unofficial) MSYS2 mirrors inside China, which substantially improves download speeds inside China.

To add these mirrors, edit the following two MSYS2 mirrorlist files before running the setup script. The mirrorlist files can be found in the ``/etc/pacman.d`` directory (i.e. ``c:\msys2\etc\pacman.d``).

Add these lines at the top of ``mirrorlist.mingw32``::

  Server = https://mirrors.ustc.edu.cn/msys2/mingw/i686/
  Server = http://mirror.bit.edu.cn/msys2/REPOS/MINGW/i686

Add these lines at the top of ``mirrorlist.msys``::

  Server = http://mirrors.ustc.edu.cn/msys2/msys/$arch
  Server = http://mirror.bit.edu.cn/msys2/REPOS/MSYS2/$arch


HTTP Proxy
~~~~~~~~~~

You can enable an HTTP proxy for MSYS and PIP downloads by setting the ``http_proxy`` variable in the terminal before running the setup script::

    export http_proxy='http://http.proxy.server:PORT'

Or with credentials::

    export http_proxy='http://user:password@http.proxy.server:PORT'

Add this line to ``/etc/profile`` in the MSYS directory in order to permanently enable the proxy when using MSYS.


Alternative Setup: Just download a toolchain
============================================

If you already have an MSYS2 install or want to do things differently, you can download just the toolchain here:

https://dl.espressif.com/dl/xtensa-esp32-elf-win32-1.22.0-80-g6c4433a-5.2.0.zip

.. note::

	If you followed instructions :ref:`configure-windows-toolchain-from-scratch`, you already have the toolchain and you won't need this download.

.. important::

	Just having this toolchain is *not enough* to use ESP-IDF on Windows. You will need GNU make, bash, and sed at minimum. The above environments provide all this, plus a host compiler (required for menuconfig support).


Next Steps
==========

To carry on with development environment setup, proceed to section :ref:`get-started-get-esp-idf`.

.. _updating-existing-windows-environment:

Updating The Environment
========================

When IDF is updated, sometimes new toolchains are required or new system requirements are added to the Windows MSYS2 environment.

Rather than setting up a new environment, you can update an existing Windows environment & toolchain:

- Update IDF to the new version you want to use.
- Run the ``tools/windows/windows_install_prerequisites.sh`` script inside IDF. This will install any new software packages that weren't previously installed, and download and replace the toolchain with the latest version.

The script to update MSYS2 may also fail with the same errors mentioned under Troubleshooting_.

If you need to support multiple IDF versions concurrently, you can have different independent MSYS2 environments in different directories. Alternatively you can download multiple toolchains and unzip these to different directories, then use the PATH environment variable to set which one is the default.

.. _MSYS2: https://msys2.github.io/
.. _MSYS2-packages issues list: https://github.com/Alexpux/MSYS2-packages/issues/
.. _raise an IDF issue: https://github.com/espressif/esp-idf/issues/new
