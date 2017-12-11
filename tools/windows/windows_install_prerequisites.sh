#!/bin/bash
#
# Setup script to configure an MSYS2 environment for ESP-IDF.
#
# Use of this script is optional, there is also a prebuilt MSYS2 environment available
# which can be downloaded and used as-is.
#
# See http://esp-idf.readthedocs.io/en/latest/windows-setup.html for full details.

if [ "$OSTYPE" != "msys" ]; then
  echo "This setup script expects to be run from an MSYS2 environment on Windows."
  exit 1
fi
if ! [ -x /bin/pacman ]; then
  echo "This setup script expects to use the pacman package manager from MSYS2."
  exit 1
fi
if [ "$MSYSTEM" != "MINGW32" ]; then
  echo "This setup script must be started from the 'MSYS2 MinGW 32-bit' start menu shortcut"
  echo "OR by running `cygpath -w /mingw32.exe`"
  echo "(The current MSYSTEM mode is $MSYSTEM but it expects it to be MINGW32)"
  exit 1
fi

# if update-core still exists, run it to get the latest core MSYS2 system
# (which no longer needs or includes update-core!)
#
# If this step runs, it will require a full restart of MSYS2 before it
# can continue.
[ -x /usr/bin/update-core ] && /usr/bin/update-core

set -e

pacman --noconfirm -Syu # This step may require the terminal to be closed and restarted

pacman --noconfirm -S --needed gettext-devel gcc git make ncurses-devel flex bison gperf vim mingw-w64-i686-python2-pip unzip winpty

# Workaround for errors when running "git submodule" commands
# See https://github.com/Alexpux/MSYS2-packages/issues/735
rm -f /mingw32/bin/envsubst.exe

python -m pip install --upgrade pip

pip install pyserial

# Automatically download precompiled toolchain, unpack at /opt/xtensa-esp32-elf/
TOOLCHAIN_ZIP=xtensa-esp32-elf-win32-1.22.0-75-gbaf03c2-5.2.0.zip
echo "Downloading precompiled toolchain ${TOOLCHAIN_ZIP}..."
cd ~
curl -LO --retry 10 http://dl.espressif.com/dl/${TOOLCHAIN_ZIP}
cd /opt
rm -rf /opt/xtensa-esp32-elf  # for upgrades
unzip ~/${TOOLCHAIN_ZIP}
rm ~/${TOOLCHAIN_ZIP}

cat > /etc/profile.d/esp32_toolchain.sh << EOF
# This file was created by ESP-IDF windows_install_prerequisites.sh
# and will be overwritten if that script is run again.
export PATH="\$PATH:/opt/xtensa-esp32-elf/bin"
EOF

# clean up pacman package cache to save some disk space
pacman --noconfirm -Scc

cat << EOF
************************************************
MSYS2 environment is now ready to use ESP-IDF.

1) Run 'source /etc/profile' to add the toolchain to
your path in this terminal. This command produces no output.
You only need to do this once, future terminals do this
automatically when opened.

2) After ESP-IDF is set up (see setup guide), edit the file
`cygpath -w /etc/profile`
and add a line to set the variable IDF_PATH so it points to the
IDF directory, ie:

export IDF_PATH=/c/path/to/esp-idf/directory

************************************************
EOF
