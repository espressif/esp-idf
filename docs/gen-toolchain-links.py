#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# This script generates toolchain download links and toolchain unpacking
# code snippets based on information found in $IDF_PATH/tools/toolchain_versions.mk
#

from __future__ import print_function

import sys
import os
from collections import namedtuple

PlatformInfo = namedtuple("PlatformInfo", [
    "platform_name",
    "platform_archive_suffix",
    "extension",
    "unpack_cmd",
    "unpack_code"
])


def main():
    if len(sys.argv) != 4:
        print("Usage: gen-toolchain-links.py <versions file> <base download URL> <output directory>")
        sys.exit(1)

    out_dir = sys.argv[3]
    if not os.path.exists(out_dir):
        print("Creating directory %s" % out_dir)
        os.mkdir(out_dir)

    base_url = sys.argv[2]

    versions_file = sys.argv[1]
    version_vars = {}
    with open(versions_file) as f:
        for line in f:
            name, var = line.partition("=")[::2]
            version_vars[name.strip()] = var.strip()

    gcc_version = version_vars["CURRENT_TOOLCHAIN_GCC_VERSION"]
    toolchain_desc = version_vars["CURRENT_TOOLCHAIN_COMMIT_DESC_SHORT"]

    unpack_code_linux_macos = """
::

    mkdir -p ~/esp
    cd ~/esp
    tar -x{}f ~/Downloads/{}
"""

    scratch_build_code_linux_macos = """
::

    git clone https://github.com/espressif/crosstool-NG.git
    cd crosstool-NG
    git checkout {}
    git submodule update --init
    ./bootstrap && ./configure --enable-local && make
"""

    platform_info = [
        PlatformInfo("linux64", "linux-amd64", "tar.gz", "z", unpack_code_linux_macos),
        PlatformInfo("linux32", "linux-i686","tar.gz", "z", unpack_code_linux_macos),
        PlatformInfo("osx", "macos", "tar.gz", "z", unpack_code_linux_macos),
        PlatformInfo("win32", "win32", "zip", None, None)
    ]

    with open(os.path.join(out_dir, 'download-links.inc'), "w") as links_file:
        for p in platform_info:
            archive_name = 'xtensa-esp32-elf-gcc{}-{}-{}.{}'.format(
                gcc_version.replace('.', '_'), toolchain_desc, p.platform_archive_suffix, p.extension)

            print('.. |download_link_{}| replace:: {}{}'.format(
                p.platform_name, base_url, archive_name), file=links_file)

            if p.unpack_code is not None:
                with open(os.path.join(out_dir, 'unpack-code-%s.inc' % p.platform_name), "w") as f:
                    print(p.unpack_code.format(p.unpack_cmd, archive_name), file=f)

    with open(os.path.join(out_dir, 'scratch-build-code.inc'), "w") as code_file:
        print(scratch_build_code_linux_macos.format(toolchain_desc), file=code_file)


if __name__ == "__main__":
    main()
