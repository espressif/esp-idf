# Generate toolchain download links from toolchain info makefile
from __future__ import print_function
import os.path
from collections import namedtuple
from .util import copy_if_modified

BASE_URL = 'https://dl.espressif.com/dl/'

PlatformInfo = namedtuple("PlatformInfo", [
    "platform_name",
    "platform_archive_suffix",
    "extension",
    "unpack_cmd",
    "unpack_code"
])


def setup(app):
    # we don't actually need idf-info, just a convenient event to trigger this on
    app.connect('idf-info', generate_toolchain_download_links)

    return {'parallel_read_safe': True, 'parallel_write_safe': True, 'version': '0.1'}


def generate_toolchain_download_links(app, project_description):
    print("Generating toolchain download links")
    toolchain_tmpdir = '{}/toolchain_inc'.format(app.config.build_dir)
    toolchain_versions = os.path.join(app.config.idf_path, "tools/toolchain_versions.mk")
    gen_toolchain_links(toolchain_versions, toolchain_tmpdir)
    copy_if_modified(toolchain_tmpdir, '{}/inc'.format(app.config.build_dir))


def gen_toolchain_links(versions_file, out_dir):
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

    try:
        os.mkdir(out_dir)
    except OSError:
        pass

    with open(os.path.join(out_dir, 'download-links.inc'), "w") as links_file:
        for p in platform_info:
            archive_name = 'xtensa-esp32-elf-gcc{}-{}-{}.{}'.format(
                gcc_version.replace('.', '_'), toolchain_desc, p.platform_archive_suffix, p.extension)

            print('.. |download_link_{}| replace:: {}{}'.format(
                p.platform_name, BASE_URL, archive_name), file=links_file)

            if p.unpack_code is not None:
                with open(os.path.join(out_dir, 'unpack-code-%s.inc' % p.platform_name), "w") as f:
                    print(p.unpack_code.format(p.unpack_cmd, archive_name), file=f)

    with open(os.path.join(out_dir, 'scratch-build-code.inc'), "w") as code_file:
        print(scratch_build_code_linux_macos.format(toolchain_desc), file=code_file)
