#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# This script generates toolchain download links and toolchain unpacking
# code snippets based on information found in $IDF_PATH/tools/toolchain_versions.mk
#

from __future__ import print_function

import sys
import os

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

    cd ~/esp
    git clone -b xtensa-1.22.x https://github.com/espressif/crosstool-NG.git
    cd crosstool-NG
    ./bootstrap && ./configure --enable-local && make install
"""

    platform_info = [ ["linux64", "tar.gz", "z", unpack_code_linux_macos],
                      ["linux32", "tar.gz", "z", unpack_code_linux_macos],
                      ["osx", "tar.gz", "z", unpack_code_linux_macos],
                      ["win32", "zip", None, None]]

    with open(os.path.join(out_dir, 'download-links.inc'), "w") as links_file:
        for p in platform_info:
            platform_name = p[0]
            extension = p[1]
            unpack_cmd = p[2]
            unpack_code = p[3]

            archive_name = 'xtensa-esp32-elf-{}-{}-{}.{}'.format(
                    platform_name, toolchain_desc, gcc_version, extension)
            
            print('.. |download_link_{}| replace:: {}{}'.format(
                platform_name, base_url, archive_name), file=links_file)

            if unpack_code is not None:
                with open(os.path.join(out_dir, 'unpack-code-%s.inc' % platform_name), "w") as f:
                    print(unpack_code.format(unpack_cmd, archive_name), file=f)

    with open(os.path.join(out_dir, 'scratch-build-code.inc'), "w") as code_file:
        print(scratch_build_code_linux_macos, file=code_file)

if __name__ == "__main__":
    main()
