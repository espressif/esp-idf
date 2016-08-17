#!/bin/bash
#
# Setup script to configure an MSYS2 environment for ESP32 SDK.
# See docs/windows-setup.rst for details.

if [ "$OSTYPE" != "msys" ]; then
  echo "This setup script expects to be run from an MSYS2 environment on Windows."
  exit 1
fi
if ! [ -x /bin/pacman ]; then
  echo "This setup script expects to use the pacman package manager from MSYS2."
  exit 1
fi

# if update-core still exists, run it to get the latest core MSYS2 system
# (which no longer needs or includes update-core!)
#
# If this step runs, it will require a full restart of MSYS2 before it
# can continue.
[ -x /usr/bin/update-core ] && /usr/bin/update-core

set -e

pacman --noconfirm -Syu

pacman --noconfirm -S gettext-devel gcc git make ncurses-devel flex bison gperf vim mingw-w64-i686-python2-pip unzip

python -m pip install --upgrade pip

pip install pyserial

# TODO: automatically download precompiled toolchain, unpack at /opt/xtensa-esp32-elf/
TOOLCHAIN_ZIP=xtensa-esp32-elf-win32-1.22.0-59.zip
echo "Downloading precompiled toolchain ${TOOLCHAIN_ZIP}..."
cd ~
curl -LO --retry 10 http://dl.espressif.com/dl/${TOOLCHAIN_ZIP}
cd /opt
unzip ~/${TOOLCHAIN_ZIP}
rm ~/${TOOLCHAIN_ZIP}

cat > /etc/profile.d/esp32_toolchain.sh << EOF
export PATH="$PATH:/opt/xtensa-esp32-elf/bin"
EOF

# clean up pacman packages to save some disk space
pacman --noconfirm -R unzip
pacman --noconfirm -Scc

echo "************************************************"
echo "MSYS2 environment is now ready to use ESP32 SDK."
echo "Run 'source /etc/profile' to add the toolchain to"
echo "your path. Execute 'msys_shell.cmd' to launch an"
echo "MSYS terminal."
echo
echo "Once ESP32 SDK is downloaded/checked out, set the"
echo "environment variable SDK_PATH in /etc/profile to"
echo "point to the directory."
echo "************************************************"
echo

