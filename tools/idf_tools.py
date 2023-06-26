#!/usr/bin/env python
# coding=utf-8
#
# SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#
# This script helps installing tools required to use the ESP-IDF, and updating PATH
# to use the installed tools. It can also create a Python virtual environment,
# and install Python requirements into it.
#  It does not install OS dependencies. It does install tools such as the Xtensa
#  GCC toolchain and ESP32 ULP coprocessor toolchain.
#
# By default, downloaded tools will be installed under $HOME/.espressif directory
# (%USERPROFILE%/.espressif on Windows). This path can be modified by setting
# IDF_TOOLS_PATH variable prior to running this tool.
#
# Users do not need to interact with this script directly. In IDF root directory,
# install.sh (.bat) and export.sh (.bat) scripts are provided to invoke this script.
#
# Usage:
#
# * To install the tools, run `idf_tools.py install`.
#
# * To install the Python environment, run `idf_tools.py install-python-env`.
#
# * To start using the tools, run `eval "$(idf_tools.py export)"` — this will update
#   the PATH to point to the installed tools and set up other environment variables
#   needed by the tools.

import argparse
import contextlib
import copy
import datetime
import errno
import functools
import hashlib
import json
import os
import platform
import re
import shutil
import ssl
import subprocess
import sys
import tarfile
import tempfile
import time
from collections import OrderedDict, namedtuple
from json import JSONEncoder
from ssl import SSLContext  # noqa: F401
from tarfile import TarFile  # noqa: F401
from zipfile import ZipFile

# Important notice: Please keep the lines above compatible with old Pythons so it won't fail with ImportError but with
# a nice message printed by python_version_checker.check()
try:
    import python_version_checker

    # check the Python version before it will fail with an exception on syntax or package incompatibility.
    python_version_checker.check()
except RuntimeError as e:
    print(e)
    raise SystemExit(1)

from typing import IO, Any, Callable, Dict, Iterator, List, Optional, Set, Tuple, Union  # noqa: F401
from urllib.error import ContentTooShortError
from urllib.parse import urljoin, urlparse
from urllib.request import urlopen
# the following is only for typing annotation
from urllib.response import addinfourl  # noqa: F401

try:
    from exceptions import WindowsError
except ImportError:
    # Unix
    class WindowsError(OSError):  # type: ignore
        pass


TOOLS_FILE = 'tools/tools.json'
TOOLS_SCHEMA_FILE = 'tools/tools_schema.json'
TOOLS_FILE_NEW = 'tools/tools.new.json'
IDF_ENV_FILE = 'idf-env.json'
TOOLS_FILE_VERSION = 1
IDF_TOOLS_PATH_DEFAULT = os.path.join('~', '.espressif')
UNKNOWN_VERSION = 'unknown'
SUBST_TOOL_PATH_REGEX = re.compile(r'\${TOOL_PATH}')
VERSION_REGEX_REPLACE_DEFAULT = r'\1'
IDF_MAINTAINER = os.environ.get('IDF_MAINTAINER') or False
TODO_MESSAGE = 'TODO'
DOWNLOAD_RETRY_COUNT = 3
URL_PREFIX_MAP_SEPARATOR = ','
IDF_TOOLS_INSTALL_CMD = os.environ.get('IDF_TOOLS_INSTALL_CMD')
IDF_TOOLS_EXPORT_CMD = os.environ.get('IDF_TOOLS_INSTALL_CMD')
IDF_DL_URL = 'https://dl.espressif.com/dl/esp-idf'
IDF_PIP_WHEELS_URL = os.environ.get('IDF_PIP_WHEELS_URL', 'https://dl.espressif.com/pypi')

PYTHON_PLATFORM = platform.system() + '-' + platform.machine()

# Identifiers used in tools.json for different platforms.
PLATFORM_WIN32 = 'win32'
PLATFORM_WIN64 = 'win64'
PLATFORM_MACOS = 'macos'
PLATFORM_MACOS_ARM64 = 'macos-arm64'
PLATFORM_LINUX32 = 'linux-i686'
PLATFORM_LINUX64 = 'linux-amd64'
PLATFORM_LINUX_ARM32 = 'linux-armel'
PLATFORM_LINUX_ARMHF = 'linux-armhf'
PLATFORM_LINUX_ARM64 = 'linux-arm64'


class Platforms:
    # Mappings from various other names these platforms are known as, to the identifiers above.
    # This includes strings produced from "platform.system() + '-' + platform.machine()", see PYTHON_PLATFORM
    # definition above.
    # This list also includes various strings used in release archives of xtensa-esp32-elf-gcc, OpenOCD, etc.
    PLATFORM_FROM_NAME = {
        # Windows
        PLATFORM_WIN32: PLATFORM_WIN32,
        'Windows-i686': PLATFORM_WIN32,
        'Windows-x86': PLATFORM_WIN32,
        'i686-w64-mingw32': PLATFORM_WIN32,
        PLATFORM_WIN64: PLATFORM_WIN64,
        'Windows-x86_64': PLATFORM_WIN64,
        'Windows-AMD64': PLATFORM_WIN64,
        'x86_64-w64-mingw32': PLATFORM_WIN64,
        # macOS
        PLATFORM_MACOS: PLATFORM_MACOS,
        'osx': PLATFORM_MACOS,
        'darwin': PLATFORM_MACOS,
        'Darwin-x86_64': PLATFORM_MACOS,
        'x86_64-apple-darwin': PLATFORM_MACOS,
        PLATFORM_MACOS_ARM64: PLATFORM_MACOS_ARM64,
        'Darwin-arm64': PLATFORM_MACOS_ARM64,
        'aarch64-apple-darwin': PLATFORM_MACOS_ARM64,
        'arm64-apple-darwin': PLATFORM_MACOS_ARM64,
        # Linux
        PLATFORM_LINUX64: PLATFORM_LINUX64,
        'linux64': PLATFORM_LINUX64,
        'Linux-x86_64': PLATFORM_LINUX64,
        'FreeBSD-amd64': PLATFORM_LINUX64,
        'x86_64-linux-gnu': PLATFORM_LINUX64,
        PLATFORM_LINUX32: PLATFORM_LINUX32,
        'linux32': PLATFORM_LINUX32,
        'Linux-i686': PLATFORM_LINUX32,
        'FreeBSD-i386': PLATFORM_LINUX32,
        'i586-linux-gnu': PLATFORM_LINUX32,
        'i686-linux-gnu': PLATFORM_LINUX32,
        PLATFORM_LINUX_ARM64: PLATFORM_LINUX_ARM64,
        'Linux-arm64': PLATFORM_LINUX_ARM64,
        'Linux-aarch64': PLATFORM_LINUX_ARM64,
        'Linux-armv8l': PLATFORM_LINUX_ARM64,
        'aarch64': PLATFORM_LINUX_ARM64,
        PLATFORM_LINUX_ARMHF: PLATFORM_LINUX_ARMHF,
        'arm-linux-gnueabihf': PLATFORM_LINUX_ARMHF,
        PLATFORM_LINUX_ARM32: PLATFORM_LINUX_ARM32,
        'arm-linux-gnueabi': PLATFORM_LINUX_ARM32,
        'Linux-armv7l': PLATFORM_LINUX_ARM32,
        'Linux-arm': PLATFORM_LINUX_ARM32,
    }

    @staticmethod
    def get(platform_alias):  # type: (Optional[str]) -> Optional[str]
        if platform_alias is None:
            return None

        if platform_alias == 'any' and CURRENT_PLATFORM:
            platform_alias = CURRENT_PLATFORM

        platform_name = Platforms.PLATFORM_FROM_NAME.get(platform_alias, None)

        # ARM platform may run on armhf hardware but having armel installed packages.
        # To avoid possible armel/armhf libraries mixing need to define user's
        # packages architecture to use the same
        # See note section in https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html#index-mfloat-abi
        if platform_name in (PLATFORM_LINUX_ARM32, PLATFORM_LINUX_ARMHF) and 'arm' in platform.machine():
            # suppose that installed python was built with a right ABI
            with open(sys.executable, 'rb') as f:
                if int.from_bytes(f.read(4), sys.byteorder) != int.from_bytes(b'\x7fELF', sys.byteorder):
                    return platform_name  # ELF magic not found. Use default platform name from PLATFORM_FROM_NAME
                f.seek(36)  # seek to e_flags (https://man7.org/linux/man-pages/man5/elf.5.html)
                e_flags = int.from_bytes(f.read(4), sys.byteorder)
                platform_name = PLATFORM_LINUX_ARMHF if e_flags & 0x400 else PLATFORM_LINUX_ARM32
        return platform_name

    @staticmethod
    def get_by_filename(file_name):  # type: (str) -> Optional[str]
        found_alias = ''
        for platform_alias in Platforms.PLATFORM_FROM_NAME:
            # Find the longest alias which matches with file name to avoid mismatching
            if platform_alias in file_name and len(found_alias) < len(platform_alias):
                found_alias = platform_alias
        return Platforms.get(found_alias)


CURRENT_PLATFORM = Platforms.get(PYTHON_PLATFORM)

EXPORT_SHELL = 'shell'
EXPORT_KEY_VALUE = 'key-value'

# the older "DigiCert Global Root CA" certificate used with github.com
DIGICERT_ROOT_CA_CERT = """
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
"""

# the newer "DigiCert Global Root G2" certificate used with dl.espressif.com
DIGICERT_ROOT_G2_CERT = """
-----BEGIN CERTIFICATE-----
MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH
MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI
2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx
1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ
q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz
tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ
vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP
BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV
5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY
1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4
NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG
Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91
8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe
pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl
MrY=
-----END CERTIFICATE-----
"""

DL_CERT_DICT = {'dl.espressif.com': DIGICERT_ROOT_G2_CERT,
                'github.com': DIGICERT_ROOT_CA_CERT}


global_quiet = False
global_non_interactive = False
global_idf_path = None  # type: Optional[str]
global_idf_tools_path = None  # type: Optional[str]
global_tools_json = None  # type: Optional[str]


def fatal(text, *args):  # type: (str, str) -> None
    if not global_quiet:
        sys.stderr.write('ERROR: ' + text + '\n', *args)


def warn(text, *args):  # type: (str, str) -> None
    if not global_quiet:
        sys.stderr.write('WARNING: ' + text + '\n', *args)


def info(text, f=None, *args):  # type: (str, Optional[IO[str]], str) -> None
    if not global_quiet:
        if f is None:
            f = sys.stdout
        f.write(text + '\n', *args)


def print_hints_on_download_error(err):  # type: (str) -> None
    info('Please make sure you have a working Internet connection.')

    if 'CERTIFICATE' in err:
        info('Certificate issues are usually caused by an outdated certificate database on your computer.')
        info('Please check the documentation of your operating system for how to upgrade it.')

        if sys.platform == 'darwin':
            info('Running "./Install\\ Certificates.command" might be able to fix this issue.')

        info('Running "{} -m pip install --upgrade certifi" can also resolve this issue in some cases.'.format(sys.executable))

    # Certificate issue on Windows can be hidden under different errors which might be even translated,
    # e.g. "[WinError -2146881269] ASN1 valor de tag inválido encontrado"
    if sys.platform == 'win32':
        info('By downloading and using the offline installer from https://dl.espressif.com/dl/esp-idf '
             'you might be able to work around this issue.')


def run_cmd_check_output(cmd, input_text=None, extra_paths=None):
    # type: (List[str], Optional[str], Optional[List[str]]) -> bytes
    # If extra_paths is given, locate the executable in one of these directories.
    # Note: it would seem logical to add extra_paths to env[PATH], instead, and let OS do the job of finding the
    # executable for us. However this does not work on Windows: https://bugs.python.org/issue8557.
    if extra_paths:
        found = False
        extensions = ['']
        if sys.platform == 'win32':
            extensions.append('.exe')
        for path in extra_paths:
            for ext in extensions:
                fullpath = os.path.join(path, cmd[0] + ext)
                if os.path.exists(fullpath):
                    cmd[0] = fullpath
                    found = True
                    break
            if found:
                break

    try:
        input_bytes = None
        if input_text:
            input_bytes = input_text.encode()
        result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True, input=input_bytes)
        return result.stdout + result.stderr
    except (AttributeError, TypeError):
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = p.communicate(input_bytes)
        if p.returncode != 0:
            try:
                raise subprocess.CalledProcessError(p.returncode, cmd, stdout, stderr)
            except TypeError:
                raise subprocess.CalledProcessError(p.returncode, cmd, stdout)
        return stdout + stderr


def to_shell_specific_paths(paths_list):  # type: (List[str]) -> List[str]
    if sys.platform == 'win32':
        paths_list = [p.replace('/', os.path.sep) if os.path.sep in p else p for p in paths_list]

    return paths_list


def get_env_for_extra_paths(extra_paths):  # type: (List[str]) -> Dict[str, str]
    """
    Return a copy of environment variables dict, prepending paths listed in extra_paths
    to the PATH environment variable.
    """
    env_arg = os.environ.copy()
    new_path = os.pathsep.join(extra_paths) + os.pathsep + env_arg['PATH']
    if sys.version_info.major == 2:
        env_arg['PATH'] = new_path.encode('utf8')  # type: ignore
    else:
        env_arg['PATH'] = new_path
    return env_arg


def get_file_size_sha256(filename, block_size=65536):  # type: (str, int) -> Tuple[int, str]
    sha256 = hashlib.sha256()
    size = 0
    with open(filename, 'rb') as f:
        for block in iter(lambda: f.read(block_size), b''):
            sha256.update(block)
            size += len(block)
    return size, sha256.hexdigest()


def report_progress(count, block_size, total_size):  # type: (int, int, int) -> None
    percent = int(count * block_size * 100 / total_size)
    percent = min(100, percent)
    sys.stdout.write('\r%d%%' % percent)
    sys.stdout.flush()


def mkdir_p(path):  # type: (str) -> None
    try:
        os.makedirs(path)
    except OSError as exc:
        if exc.errno != errno.EEXIST or not os.path.isdir(path):
            raise


def unpack(filename, destination):  # type: (str, str) -> None
    info('Extracting {0} to {1}'.format(filename, destination))
    if filename.endswith(('.tar.gz', '.tgz')):
        archive_obj = tarfile.open(filename, 'r:gz')  # type: Union[TarFile, ZipFile]
    elif filename.endswith(('.tar.xz')):
        archive_obj = tarfile.open(filename, 'r:xz')
    elif filename.endswith('zip'):
        archive_obj = ZipFile(filename)
    else:
        raise NotImplementedError('Unsupported archive type')
    if sys.version_info.major == 2:
        # This is a workaround for the issue that unicode destination is not handled:
        # https://bugs.python.org/issue17153
        destination = str(destination)
    archive_obj.extractall(destination)


def splittype(url):  # type: (str) -> Tuple[Optional[str], str]
    match = re.match('([^/:]+):(.*)', url, re.DOTALL)
    if match:
        scheme, data = match.groups()
        return scheme.lower(), data
    return None, url


# An alternative version of urlretrieve which takes SSL context as an argument
def urlretrieve_ctx(url, filename, reporthook=None, data=None, context=None):
    # type: (str, str, Optional[Callable[[int, int, int], None]], Optional[bytes], Optional[SSLContext]) -> Tuple[str, addinfourl]
    url_type, path = splittype(url)

    # urlopen doesn't have context argument in Python <=2.7.9
    extra_urlopen_args = {}
    if context:
        extra_urlopen_args['context'] = context
    with contextlib.closing(urlopen(url, data, **extra_urlopen_args)) as fp:  # type: ignore
        headers = fp.info()

        # Just return the local path and the "headers" for file://
        # URLs. No sense in performing a copy unless requested.
        if url_type == 'file' and not filename:
            return os.path.normpath(path), headers

        # Handle temporary file setup.
        tfp = open(filename, 'wb')

        with tfp:
            result = filename, headers
            bs = 1024 * 8
            size = int(headers.get('content-length', -1))
            read = 0
            blocknum = 0

            if reporthook:
                reporthook(blocknum, bs, size)

            while True:
                block = fp.read(bs)
                if not block:
                    break
                read += len(block)
                tfp.write(block)
                blocknum += 1
                if reporthook:
                    reporthook(blocknum, bs, size)

    if size >= 0 and read < size:
        raise ContentTooShortError(
            'retrieval incomplete: got only %i out of %i bytes'
            % (read, size), result)

    return result


def download(url, destination):  # type: (str, str) -> Optional[Exception]
    info(f'Downloading {url}')
    info(f'Destination: {destination}')
    try:
        for site, cert in DL_CERT_DICT.items():
            # For dl.espressif.com and github.com, add the DigiCert root certificate.
            # This works around the issue with outdated certificate stores in some installations.
            if site in url:
                ctx = ssl.create_default_context()
                ctx.load_verify_locations(cadata=cert)
                break
        else:
            ctx = None

        urlretrieve_ctx(url, destination, report_progress if not global_non_interactive else None, context=ctx)
        sys.stdout.write('\rDone\n')
        return None
    except Exception as e:
        # urlretrieve could throw different exceptions, e.g. IOError when the server is down
        return e
    finally:
        sys.stdout.flush()


# Sometimes renaming a directory on Windows (randomly?) causes a PermissionError.
# This is confirmed to be a workaround:
# https://github.com/espressif/esp-idf/issues/3819#issuecomment-515167118
# https://github.com/espressif/esp-idf/issues/4063#issuecomment-531490140
# https://stackoverflow.com/a/43046729
def rename_with_retry(path_from, path_to):  # type: (str, str) -> None
    retry_count = 20 if sys.platform.startswith('win') else 1
    for retry in range(retry_count):
        try:
            os.rename(path_from, path_to)
            return
        except OSError:
            msg = f'Rename {path_from} to {path_to} failed'
            if retry == retry_count - 1:
                fatal(msg + '. Antivirus software might be causing this. Disabling it temporarily could solve the issue.')
                raise
            warn(msg + ', retrying...')
            # Sleep before the next try in order to pass the antivirus check on Windows
            time.sleep(0.5)


def do_strip_container_dirs(path, levels):  # type: (str, int) -> None
    assert levels > 0
    # move the original directory out of the way (add a .tmp suffix)
    tmp_path = path + '.tmp'
    if os.path.exists(tmp_path):
        shutil.rmtree(tmp_path)
    rename_with_retry(path, tmp_path)
    os.mkdir(path)
    base_path = tmp_path
    # walk given number of levels down
    for level in range(levels):
        contents = os.listdir(base_path)
        if len(contents) > 1:
            raise RuntimeError('at level {}, expected 1 entry, got {}'.format(level, contents))
        base_path = os.path.join(base_path, contents[0])
        if not os.path.isdir(base_path):
            raise RuntimeError('at level {}, {} is not a directory'.format(level, contents[0]))
    # get the list of directories/files to move
    contents = os.listdir(base_path)
    for name in contents:
        move_from = os.path.join(base_path, name)
        move_to = os.path.join(path, name)
        rename_with_retry(move_from, move_to)
    shutil.rmtree(tmp_path)


class ToolNotFound(RuntimeError):
    pass


class ToolExecError(RuntimeError):
    pass


class IDFToolDownload(object):
    def __init__(self, platform_name, url, size, sha256):  # type: (str, str, int, str) -> None
        self.platform_name = platform_name
        self.url = url
        self.size = size
        self.sha256 = sha256
        self.platform_name = platform_name


@functools.total_ordering
class IDFToolVersion(object):
    STATUS_RECOMMENDED = 'recommended'
    STATUS_SUPPORTED = 'supported'
    STATUS_DEPRECATED = 'deprecated'

    STATUS_VALUES = [STATUS_RECOMMENDED, STATUS_SUPPORTED, STATUS_DEPRECATED]

    def __init__(self, version, status):  # type: (str, str) -> None
        self.version = version
        self.status = status
        self.downloads = OrderedDict()  # type: OrderedDict[str, IDFToolDownload]
        self.latest = False

    def __lt__(self, other):  # type: (IDFToolVersion) -> bool
        if self.status != other.status:
            return self.status > other.status
        else:
            assert not (self.status == IDFToolVersion.STATUS_RECOMMENDED
                        and other.status == IDFToolVersion.STATUS_RECOMMENDED)
            return self.version < other.version

    def __eq__(self, other):  # type: (object) -> bool
        if not isinstance(other, IDFToolVersion):
            return NotImplemented
        return self.status == other.status and self.version == other.version

    def add_download(self, platform_name, url, size, sha256):  # type: (str, str, int, str) -> None
        self.downloads[platform_name] = IDFToolDownload(platform_name, url, size, sha256)

    def get_download_for_platform(self, platform_name):  # type: (Optional[str]) -> Optional[IDFToolDownload]
        platform_name = Platforms.get(platform_name)
        if platform_name and platform_name in self.downloads.keys():
            return self.downloads[platform_name]
        if 'any' in self.downloads.keys():
            return self.downloads['any']
        return None

    def compatible_with_platform(self, platform_name=PYTHON_PLATFORM):
        # type: (Optional[str]) -> bool
        return self.get_download_for_platform(platform_name) is not None

    def get_supported_platforms(self):  # type: () -> set[str]
        return set(self.downloads.keys())


IDFToolOptions = namedtuple('IDFToolOptions', [
    'version_cmd',
    'version_regex',
    'version_regex_replace',
    'is_executable',
    'export_paths',
    'export_vars',
    'install',
    'info_url',
    'license',
    'strip_container_dirs',
    'supported_targets'])


class IDFTool(object):
    # possible values of 'install' field
    INSTALL_ALWAYS = 'always'
    INSTALL_ON_REQUEST = 'on_request'
    INSTALL_NEVER = 'never'

    def __init__(self, name, description, install, info_url, license, version_cmd, version_regex, supported_targets, version_regex_replace=None,
                 strip_container_dirs=0, is_executable=True):
        # type: (str, str, str, str, str, List[str], str, List[str], Optional[str], int, bool) -> None
        self.name = name
        self.description = description
        self.drop_versions()
        self.version_in_path = None  # type: Optional[str]
        self.versions_installed = []  # type: List[str]
        if version_regex_replace is None:
            version_regex_replace = VERSION_REGEX_REPLACE_DEFAULT
        self.options = IDFToolOptions(version_cmd, version_regex, version_regex_replace, is_executable,
                                      [], OrderedDict(), install, info_url, license, strip_container_dirs, supported_targets)  # type: ignore
        self.platform_overrides = []  # type: List[Dict[str, str]]
        self._platform = CURRENT_PLATFORM
        self._update_current_options()
        self.is_executable = is_executable

    def copy_for_platform(self, platform):  # type: (str) -> IDFTool
        result = copy.deepcopy(self)
        result._platform = platform
        result._update_current_options()
        return result

    def _update_current_options(self):  # type: () -> None
        self._current_options = IDFToolOptions(*self.options)
        for override in self.platform_overrides:
            if self._platform and self._platform not in override['platforms']:
                continue
            override_dict = override.copy()
            del override_dict['platforms']
            self._current_options = self._current_options._replace(**override_dict)  # type: ignore

    def drop_versions(self):  # type: () -> None
        self.versions = OrderedDict()  # type: Dict[str, IDFToolVersion]

    def add_version(self, version):  # type: (IDFToolVersion) -> None
        assert type(version) is IDFToolVersion
        self.versions[version.version] = version

    def get_path(self):  # type: () -> str
        return os.path.join(global_idf_tools_path or '', 'tools', self.name)

    def get_path_for_version(self, version):  # type: (str) -> str
        assert version in self.versions
        return os.path.join(self.get_path(), version)

    def get_export_paths(self, version):  # type: (str) -> List[str]
        tool_path = self.get_path_for_version(version)
        return [os.path.join(tool_path, *p) for p in self._current_options.export_paths]  # type: ignore

    def get_export_vars(self, version):  # type: (str) -> Dict[str, str]
        """
        Get the dictionary of environment variables to be exported, for the given version.
        Expands:
        - ${TOOL_PATH} => the actual path where the version is installed
        """
        result = {}
        for k, v in self._current_options.export_vars.items():  # type: ignore
            replace_path = self.get_path_for_version(version).replace('\\', '\\\\')
            v_repl = re.sub(SUBST_TOOL_PATH_REGEX, replace_path, v)
            if v_repl != v:
                v_repl = to_shell_specific_paths([v_repl])[0]
            old_v = os.environ.get(k)
            if old_v is None or old_v != v_repl:
                result[k] = v_repl
        return result

    def get_version(self, extra_paths=None, executable_path=None):  # type: (Optional[List[str]], Optional[str]) -> str
        """
        Execute the tool, optionally prepending extra_paths to PATH,
        extract the version string and return it as a result.
        Raises ToolNotFound if the tool is not found (not present in the paths).
        Raises ToolExecError if the tool returns with a non-zero exit code.
        Returns 'unknown' if tool returns something from which version string
        can not be extracted.
        """
        # this function can not be called for a different platform
        assert self._platform == CURRENT_PLATFORM
        cmd = self._current_options.version_cmd  # type: ignore
        if executable_path:
            cmd[0] = executable_path
        try:
            version_cmd_result = run_cmd_check_output(cmd, None, extra_paths)
        except OSError:
            # tool is not on the path
            raise ToolNotFound('Tool {} not found'.format(self.name))
        except subprocess.CalledProcessError as e:
            raise ToolExecError('returned non-zero exit code ({}) with error message:\n{}'.format(
                e.returncode, e.stderr.decode('utf-8',errors='ignore')))  # type: ignore

        in_str = version_cmd_result.decode('utf-8')
        match = re.search(self._current_options.version_regex, in_str)  # type: ignore
        if not match:
            return UNKNOWN_VERSION
        return re.sub(self._current_options.version_regex, self._current_options.version_regex_replace, match.group(0))  # type: ignore

    def check_version(self, executable_path):  # type: (Optional[str]) -> bool
        version = self.get_version(executable_path=executable_path)
        return version in self.versions

    def get_install_type(self):  # type: () -> Callable[[str], None]
        return self._current_options.install  # type: ignore

    def get_supported_targets(self):  # type: ()  -> list[str]
        return self._current_options.supported_targets  # type: ignore

    def compatible_with_platform(self):  # type: () -> bool
        return any([v.compatible_with_platform() for v in self.versions.values()])

    def get_supported_platforms(self):  # type: () -> Set[str]
        result = set()
        for v in self.versions.values():
            result.update(v.get_supported_platforms())
        return result

    def get_recommended_version(self):  # type: () -> Optional[str]
        recommended_versions = [k for k, v in self.versions.items()
                                if v.status == IDFToolVersion.STATUS_RECOMMENDED
                                and v.compatible_with_platform(self._platform)]
        assert len(recommended_versions) <= 1
        if recommended_versions:
            return recommended_versions[0]
        return None

    def get_preferred_installed_version(self):  # type: () -> Optional[str]
        recommended_versions = [k for k in self.versions_installed
                                if self.versions[k].status == IDFToolVersion.STATUS_RECOMMENDED
                                and self.versions[k].compatible_with_platform(self._platform)]
        assert len(recommended_versions) <= 1
        if recommended_versions:
            return recommended_versions[0]
        return None

    def find_installed_versions(self):  # type: () -> None
        """
        Checks whether the tool can be found in PATH and in global_idf_tools_path.
        Writes results to self.version_in_path and self.versions_installed.
        """
        # this function can not be called for a different platform
        assert self._platform == CURRENT_PLATFORM
        # First check if the tool is in system PATH
        try:
            ver_str = self.get_version()
        except ToolNotFound:
            # not in PATH
            pass
        except ToolExecError as e:
            warn('tool {} found in path, but {}'.format(
                self.name, e))
        else:
            self.version_in_path = ver_str

        # Now check all the versions installed in global_idf_tools_path
        self.versions_installed = []
        for version, version_obj in self.versions.items():
            if not version_obj.compatible_with_platform():
                continue
            tool_path = self.get_path_for_version(version)
            if not os.path.exists(tool_path):
                # version not installed
                continue
            if not self.is_executable:
                self.versions_installed.append(version)
                continue
            try:
                ver_str = self.get_version(self.get_export_paths(version))
            except ToolNotFound:
                warn('directory for tool {} version {} is present, but tool was not found'.format(
                    self.name, version))
            except ToolExecError as e:
                warn('tool {} version {} is installed, but {}'.format(
                    self.name, version, e))
            else:
                if ver_str != version:
                    warn('tool {} version {} is installed, but has reported version {}'.format(
                        self.name, version, ver_str))
                else:
                    self.versions_installed.append(version)

    def latest_installed_version(self):  # type: () -> Optional[str]
        """
        Get the latest installed tool version by directly checking the
        tool's version directories.
        """
        tool_path = self.get_path()
        if not os.path.exists(tool_path):
            return None
        dentries = os.listdir(tool_path)
        dirs = [d for d in dentries if os.path.isdir(os.path.join(tool_path, d))]
        for version in sorted(dirs, reverse=True):
            # get_path_for_version() has assert to check if version is in versions
            # dict, so get_export_paths() cannot be used. Let's just create the
            # export paths list directly here.
            paths = [os.path.join(tool_path, version, *p) for p in self._current_options.export_paths]
            try:
                ver_str = self.get_version(paths)
            except (ToolNotFound, ToolExecError):
                continue
            if ver_str != version:
                continue
            return version

        return None

    def download(self, version):  # type: (str) -> None
        assert version in self.versions
        download_obj = self.versions[version].get_download_for_platform(self._platform)
        if not download_obj:
            fatal('No packages for tool {} platform {}!'.format(self.name, self._platform))
            raise SystemExit(1)

        url = download_obj.url
        archive_name = os.path.basename(url)
        local_path = os.path.join(global_idf_tools_path or '', 'dist', archive_name)
        mkdir_p(os.path.dirname(local_path))

        if os.path.isfile(local_path):
            if not self.check_download_file(download_obj, local_path):
                warn('removing downloaded file {0} and downloading again'.format(archive_name))
                os.unlink(local_path)
            else:
                info('file {0} is already downloaded'.format(archive_name))
                return

        downloaded = False
        local_temp_path = local_path + '.tmp'
        for retry in range(DOWNLOAD_RETRY_COUNT):
            err = download(url, local_temp_path)
            if not os.path.isfile(local_temp_path) or not self.check_download_file(download_obj, local_temp_path):
                warn('Download failure: {}'.format(err))
                warn('Failed to download {} to {}'.format(url, local_temp_path))
                continue
            rename_with_retry(local_temp_path, local_path)
            downloaded = True
            break
        if not downloaded:
            fatal('Failed to download, and retry count has expired')
            print_hints_on_download_error(str(err))
            raise SystemExit(1)

    def install(self, version):  # type: (str) -> None
        # Currently this is called after calling 'download' method, so here are a few asserts
        # for the conditions which should be true once that method is done.
        assert version in self.versions
        download_obj = self.versions[version].get_download_for_platform(self._platform)
        assert download_obj is not None
        archive_name = os.path.basename(download_obj.url)
        archive_path = os.path.join(global_idf_tools_path or '', 'dist', archive_name)
        assert os.path.isfile(archive_path)
        dest_dir = self.get_path_for_version(version)
        if os.path.exists(dest_dir):
            warn('destination path already exists, removing')
            shutil.rmtree(dest_dir)
        mkdir_p(dest_dir)
        unpack(archive_path, dest_dir)
        if self._current_options.strip_container_dirs:  # type: ignore
            do_strip_container_dirs(dest_dir, self._current_options.strip_container_dirs)  # type: ignore

    @staticmethod
    def check_download_file(download_obj, local_path):  # type: (IDFToolDownload, str) -> bool
        expected_sha256 = download_obj.sha256
        expected_size = download_obj.size
        file_size, file_sha256 = get_file_size_sha256(local_path)
        if file_size != expected_size:
            warn('file size mismatch for {}, expected {}, got {}'.format(local_path, expected_size, file_size))
            return False
        if file_sha256 != expected_sha256:
            warn('hash mismatch for {}, expected {}, got {}'.format(local_path, expected_sha256, file_sha256))
            return False
        return True

    @classmethod
    def from_json(cls, tool_dict):  # type: (Dict[str, Union[str, List[str], Dict[str, str]]]) -> IDFTool
        # Validate json fields
        tool_name = tool_dict.get('name')  # type: ignore
        if not isinstance(tool_name, str):
            raise RuntimeError('tool_name is not a string')

        description = tool_dict.get('description')  # type: ignore
        if not isinstance(description, str):
            raise RuntimeError('description is not a string')

        is_executable = tool_dict.get('is_executable', True)  # type: ignore
        if not isinstance(is_executable, bool):
            raise RuntimeError('is_executable for tool %s is not a bool' % tool_name)

        version_cmd = tool_dict.get('version_cmd')
        if type(version_cmd) is not list:
            raise RuntimeError('version_cmd for tool %s is not a list of strings' % tool_name)

        version_regex = tool_dict.get('version_regex')
        if not isinstance(version_regex, str) or (not version_regex and is_executable):
            raise RuntimeError('version_regex for tool %s is not a non-empty string' % tool_name)

        version_regex_replace = tool_dict.get('version_regex_replace')
        if version_regex_replace and not isinstance(version_regex_replace, str):
            raise RuntimeError('version_regex_replace for tool %s is not a string' % tool_name)

        export_paths = tool_dict.get('export_paths')
        if type(export_paths) is not list:
            raise RuntimeError('export_paths for tool %s is not a list' % tool_name)

        export_vars = tool_dict.get('export_vars', {})  # type: ignore
        if type(export_vars) is not dict:
            raise RuntimeError('export_vars for tool %s is not a mapping' % tool_name)

        versions = tool_dict.get('versions')
        if type(versions) is not list:
            raise RuntimeError('versions for tool %s is not an array' % tool_name)

        install = tool_dict.get('install', False)  # type: ignore
        if not isinstance(install, str):
            raise RuntimeError('install for tool %s is not a string' % tool_name)

        info_url = tool_dict.get('info_url', False)  # type: ignore
        if not isinstance(info_url, str):
            raise RuntimeError('info_url for tool %s is not a string' % tool_name)

        license = tool_dict.get('license', False)  # type: ignore
        if not isinstance(license, str):
            raise RuntimeError('license for tool %s is not a string' % tool_name)

        strip_container_dirs = tool_dict.get('strip_container_dirs', 0)
        if strip_container_dirs and type(strip_container_dirs) is not int:
            raise RuntimeError('strip_container_dirs for tool %s is not an int' % tool_name)

        overrides_list = tool_dict.get('platform_overrides', [])  # type: ignore
        if type(overrides_list) is not list:
            raise RuntimeError('platform_overrides for tool %s is not a list' % tool_name)

        supported_targets = tool_dict.get('supported_targets')
        if not isinstance(supported_targets, list):
            raise RuntimeError('supported_targets for tool %s is not a list of strings' % tool_name)

        # Create the object
        tool_obj = cls(tool_name, description, install, info_url, license,  # type: ignore
                       version_cmd, version_regex, supported_targets, version_regex_replace,  # type: ignore
                       strip_container_dirs, is_executable)  # type: ignore

        for path in export_paths:  # type: ignore
            tool_obj.options.export_paths.append(path)  # type: ignore

        for name, value in export_vars.items():  # type: ignore
            tool_obj.options.export_vars[name] = value  # type: ignore

        for index, override in enumerate(overrides_list):
            platforms_list = override.get('platforms')  # type: ignore
            if type(platforms_list) is not list:
                raise RuntimeError('platforms for override %d of tool %s is not a list' % (index, tool_name))

            install = override.get('install')  # type: ignore
            if install is not None and not isinstance(install, str):
                raise RuntimeError('install for override %d of tool %s is not a string' % (index, tool_name))

            version_cmd = override.get('version_cmd')  # type: ignore
            if version_cmd is not None and type(version_cmd) is not list:
                raise RuntimeError('version_cmd for override %d of tool %s is not a list of strings' %
                                   (index, tool_name))

            version_regex = override.get('version_regex')  # type: ignore
            if version_regex is not None and (not isinstance(version_regex, str) or not version_regex):
                raise RuntimeError('version_regex for override %d of tool %s is not a non-empty string' %
                                   (index, tool_name))

            version_regex_replace = override.get('version_regex_replace')  # type: ignore
            if version_regex_replace is not None and not isinstance(version_regex_replace, str):
                raise RuntimeError('version_regex_replace for override %d of tool %s is not a string' %
                                   (index, tool_name))

            export_paths = override.get('export_paths')  # type: ignore
            if export_paths is not None and type(export_paths) is not list:
                raise RuntimeError('export_paths for override %d of tool %s is not a list' % (index, tool_name))

            export_vars = override.get('export_vars')  # type: ignore
            if export_vars is not None and type(export_vars) is not dict:
                raise RuntimeError('export_vars for override %d of tool %s is not a mapping' % (index, tool_name))
            tool_obj.platform_overrides.append(override)  # type: ignore

        recommended_versions = {}  # type: dict[str, list[str]]
        for version_dict in versions:  # type: ignore
            version = version_dict.get('name')  # type: ignore
            if not isinstance(version, str):
                raise RuntimeError('version name for tool {} is not a string'.format(tool_name))

            version_status = version_dict.get('status')  # type: ignore
            if not isinstance(version_status, str) and version_status not in IDFToolVersion.STATUS_VALUES:
                raise RuntimeError('tool {} version {} status is not one of {}', tool_name, version,
                                   IDFToolVersion.STATUS_VALUES)

            version_obj = IDFToolVersion(version, version_status)
            for platform_id, platform_dict in version_dict.items():  # type: ignore
                if platform_id in ['name', 'status']:
                    continue
                if Platforms.get(platform_id) is None:
                    raise RuntimeError('invalid platform %s for tool %s version %s' %
                                       (platform_id, tool_name, version))

                version_obj.add_download(platform_id,
                                         platform_dict['url'], platform_dict['size'], platform_dict['sha256'])

                if version_status == IDFToolVersion.STATUS_RECOMMENDED:
                    if platform_id not in recommended_versions:
                        recommended_versions[platform_id] = []
                    recommended_versions[platform_id].append(version)

            tool_obj.add_version(version_obj)
        for platform_id, version_list in recommended_versions.items():
            if len(version_list) > 1:
                raise RuntimeError('tool {} for platform {} has {} recommended versions'.format(
                    tool_name, platform_id, len(recommended_versions)))
            if install != IDFTool.INSTALL_NEVER and len(recommended_versions) == 0:
                raise RuntimeError('required/optional tool {} for platform {} has no recommended versions'.format(
                    tool_name, platform_id))

        tool_obj._update_current_options()
        return tool_obj

    def to_json(self):  # type: ignore
        versions_array = []
        for version, version_obj in self.versions.items():
            version_json = {
                'name': version,
                'status': version_obj.status
            }
            for platform_id, download in version_obj.downloads.items():
                version_json[platform_id] = {
                    'url': download.url,
                    'size': download.size,
                    'sha256': download.sha256
                }
            versions_array.append(version_json)
        overrides_array = self.platform_overrides

        tool_json = {
            'name': self.name,
            'description': self.description,
            'export_paths': self.options.export_paths,
            'export_vars': self.options.export_vars,
            'install': self.options.install,
            'info_url': self.options.info_url,
            'license': self.options.license,
            'version_cmd': self.options.version_cmd,
            'version_regex': self.options.version_regex,
            'supported_targets': self.options.supported_targets,
            'versions': versions_array,
        }
        if self.options.version_regex_replace != VERSION_REGEX_REPLACE_DEFAULT:
            tool_json['version_regex_replace'] = self.options.version_regex_replace
        if overrides_array:
            tool_json['platform_overrides'] = overrides_array
        if self.options.strip_container_dirs:
            tool_json['strip_container_dirs'] = self.options.strip_container_dirs
        if self.options.is_executable is False:
            tool_json['is_executable'] = self.options.is_executable
        return tool_json


class IDFEnvEncoder(JSONEncoder):
    """
    IDFEnvEncoder is used for encoding IDFEnv, IDFRecord, SelectedIDFRecord classes to JSON in readable format. Not as (__main__.IDFRecord object at '0x7fcxx')
    Additionally remove first underscore with private properties when processing
    """
    def default(self, obj):  # type: ignore
        return {k.lstrip('_'): v for k, v in vars(obj).items()}


class IDFRecord:
    """
    IDFRecord represents one record of installed ESP-IDF on system.
    Contains:
        * version - actual version of ESP-IDF (example '5.0')
        * path - absolute path to the ESP-IDF
        * features - features using ESP-IDF
        * targets - ESP chips for which are installed needed toolchains (example ['esp32' , 'esp32s2'])
                  - Default value is [], since user didn't define any targets yet
    """
    def __init__(self) -> None:
        self.version = ''  # type: str
        self.path = ''  # type: str
        self._features = ['core']  # type: list[str]
        self._targets = []  # type: list[str]

    def __iter__(self):  # type: ignore
        yield from {
            'version': self.version,
            'path': self.path,
            'features': self._features,
            'targets': self._targets
        }.items()

    def __str__(self) -> str:
        return json.dumps(dict(self), ensure_ascii=False, indent=4)  # type: ignore

    def __repr__(self) -> str:
        return self.__str__()

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, IDFRecord):
            return False
        return all(getattr(self, x) == getattr(other, x) for x in ('version', 'path', 'features', 'targets'))

    def __ne__(self, other: object) -> bool:
        if not isinstance(other, IDFRecord):
            return False
        return not self.__eq__(other)

    @property
    def features(self) -> List[str]:
        return self._features

    def update_features(self, add: Tuple[str, ...] = (), remove: Tuple[str, ...] = ()) -> None:
        # Update features, but maintain required feature 'core'
        # If the same feature is present in both argument's tuples, do not update this feature
        add_set = set(add)
        remove_set = set(remove)
        # Remove duplicates
        features_to_add = add_set.difference(remove_set)
        features_to_remove = remove_set.difference(add_set)

        features = set(self._features)
        features.update(features_to_add)
        features.difference_update(features_to_remove)
        features.add('core')
        self._features = list(features)

    @property
    def targets(self) -> List[str]:
        return self._targets

    def extend_targets(self, targets: List[str]) -> None:
        # Targets can be only updated, but always maintain existing targets.
        self._targets = list(set(targets + self._targets))

    @classmethod
    def get_active_idf_record(cls):  # type: () -> IDFRecord
        idf_record_obj = cls()
        idf_record_obj.version = get_idf_version()
        idf_record_obj.path = global_idf_path or ''
        return idf_record_obj

    @classmethod
    def get_idf_record_from_dict(cls, record_dict):  # type: (Dict[str, Any]) -> IDFRecord
        idf_record_obj = cls()
        try:
            idf_record_obj.version = record_dict['version']
            idf_record_obj.path = record_dict['path']
        except KeyError:
            # When some of these key attributes, which are irreplaceable with default values, are not found, raise VallueError
            raise ValueError('Inconsistent record')

        idf_record_obj.update_features(record_dict.get('features', []))
        idf_record_obj.extend_targets(record_dict.get('targets', []))

        return idf_record_obj


class IDFEnv:
    """
    IDFEnv represents ESP-IDF Environments installed on system and is responsible for loading and saving structured data
    All information is saved and loaded from IDF_ENV_FILE
    Contains:
        * idf_installed - all installed environments of ESP-IDF on system
    """

    def __init__(self) -> None:
        active_idf_id = active_repo_id()
        self.idf_installed = {active_idf_id: IDFRecord.get_active_idf_record()}  # type: Dict[str, IDFRecord]

    def __iter__(self):  # type: ignore
        yield from {
            'idfInstalled': self.idf_installed,
        }.items()

    def __str__(self) -> str:
        return json.dumps(dict(self), cls=IDFEnvEncoder, ensure_ascii=False, indent=4)  # type: ignore

    def __repr__(self) -> str:
        return self.__str__()

    def save(self) -> None:
        """
        Diff current class instance with instance loaded from IDF_ENV_FILE and save only if are different
        """
        # It is enough to compare just active records because others can't be touched by the running script
        if self.get_active_idf_record() != self.get_idf_env().get_active_idf_record():
            idf_env_file_path = os.path.join(global_idf_tools_path or '', IDF_ENV_FILE)
            try:
                if global_idf_tools_path:  # mypy fix for Optional[str] in the next call
                    # the directory doesn't exist if this is run on a clean system the first time
                    mkdir_p(global_idf_tools_path)
                with open(idf_env_file_path, 'w', encoding='utf-8') as w:
                    info('Updating {}'.format(idf_env_file_path))
                    json.dump(dict(self), w, cls=IDFEnvEncoder, ensure_ascii=False, indent=4)  # type: ignore
            except (IOError, OSError):
                if not os.access(global_idf_tools_path or '', os.W_OK):
                    raise OSError('IDF_TOOLS_PATH {} is not accessible to write. Required changes have not been saved'.format(global_idf_tools_path or ''))
                raise OSError('File {} is not accessible to write or corrupted. Required changes have not been saved'.format(idf_env_file_path))

    def get_active_idf_record(self) -> IDFRecord:
        return self.idf_installed[active_repo_id()]

    @classmethod
    def get_idf_env(cls):  # type: () -> IDFEnv
        # IDFEnv class is used to process IDF_ENV_FILE file. The constructor is therefore called only in this method that loads the file and checks its contents
        idf_env_obj = cls()
        try:
            idf_env_file_path = os.path.join(global_idf_tools_path or '', IDF_ENV_FILE)
            with open(idf_env_file_path, 'r', encoding='utf-8') as idf_env_file:
                idf_env_json = json.load(idf_env_file)

                try:
                    idf_installed = idf_env_json['idfInstalled']
                except KeyError:
                    # If no ESP-IDF record is found in loaded file, do not update and keep default value from constructor
                    pass
                else:
                    # Load and verify ESP-IDF records found in IDF_ENV_FILE
                    idf_installed.pop('sha', None)
                    idf_installed_verified = {}  # type: dict[str, IDFRecord]
                    for idf in idf_installed:
                        try:
                            idf_installed_verified[idf] = IDFRecord.get_idf_record_from_dict(idf_installed[idf])
                        except ValueError as err:
                            warn('{} "{}" found in {}, removing this record.' .format(err, idf, idf_env_file_path))
                    # Combine ESP-IDF loaded records with the one in constructor, to be sure that there is an active ESP-IDF record in the idf_installed
                    # If the active record is already in idf_installed, it is not overwritten
                    idf_env_obj.idf_installed = dict(idf_env_obj.idf_installed, **idf_installed_verified)

        except (IOError, OSError, ValueError):
            # If no, empty or not-accessible to read IDF_ENV_FILE found, use default values from constructor
            pass

        return idf_env_obj


class ENVState:
    """
    ENVState is used to handle IDF global variables that are set in environment and need to be removed when switching between ESP-IDF versions in opened shell
    Every opened shell/terminal has it's own temporary file to store these variables
    The temporary file's name is generated automatically with suffix 'idf_ + opened shell ID'. Path to this tmp file is stored as env global variable (env_key)
    The shell ID is crucial, since in one terminal can be opened more shells
    * env_key - global variable name/key
    * deactivate_file_path - global variable value (generated tmp file name)
    * idf_variables - loaded IDF variables from file
    """
    env_key = 'IDF_DEACTIVATE_FILE_PATH'
    deactivate_file_path = os.environ.get(env_key, '')

    def __init__(self) -> None:
        self.idf_variables = {}  # type: Dict[str, Any]

    @classmethod
    def get_env_state(cls):  # type: () -> ENVState
        env_state_obj = cls()

        if cls.deactivate_file_path:
            try:
                with open(cls.deactivate_file_path, 'r') as fp:
                    env_state_obj.idf_variables = json.load(fp)
            except (IOError, OSError, ValueError):
                pass
        return env_state_obj

    def save(self) -> str:
        try:
            if self.deactivate_file_path and os.path.basename(self.deactivate_file_path).endswith('idf_' + str(os.getppid())):
                # If exported file path/name exists and belongs to actual opened shell
                with open(self.deactivate_file_path, 'w') as w:
                    json.dump(self.idf_variables, w, ensure_ascii=False, indent=4)  # type: ignore
            else:
                with tempfile.NamedTemporaryFile(delete=False, suffix='idf_' + str(os.getppid())) as fp:
                    self.deactivate_file_path = fp.name
                    fp.write(json.dumps(self.idf_variables, ensure_ascii=False, indent=4).encode('utf-8'))
        except (IOError, OSError):
            warn('File storing IDF env variables {} is not accessible to write. '
                 'Potentional switching ESP-IDF versions may cause problems'.format(self.deactivate_file_path))
        return self.deactivate_file_path


def load_tools_info():  # type: () -> dict[str, IDFTool]
    """
    Load tools metadata from tools.json, return a dictionary: tool name - tool info
    """
    tool_versions_file_name = global_tools_json

    with open(tool_versions_file_name, 'r') as f:  # type: ignore
        tools_info = json.load(f)

    return parse_tools_info_json(tools_info)  # type: ignore


def parse_tools_info_json(tools_info):  # type: ignore
    """
    Parse and validate the dictionary obtained by loading the tools.json file.
    Returns a dictionary of tools (key: tool name, value: IDFTool object).
    """
    if tools_info['version'] != TOOLS_FILE_VERSION:
        raise RuntimeError('Invalid version')

    tools_dict = OrderedDict()

    tools_array = tools_info.get('tools')
    if type(tools_array) is not list:
        raise RuntimeError('tools property is missing or not an array')

    for tool_dict in tools_array:
        tool = IDFTool.from_json(tool_dict)
        tools_dict[tool.name] = tool

    return tools_dict


def dump_tools_json(tools_info):  # type: ignore
    tools_array = []
    for tool_name, tool_obj in tools_info.items():
        tool_json = tool_obj.to_json()
        tools_array.append(tool_json)
    file_json = {'version': TOOLS_FILE_VERSION, 'tools': tools_array}
    return json.dumps(file_json, indent=2, separators=(',', ': '), sort_keys=True)


def get_python_exe_and_subdir() -> Tuple[str, str]:
    if sys.platform == 'win32':
        subdir = 'Scripts'
        python_exe = 'python.exe'
    else:
        subdir = 'bin'
        python_exe = 'python'
    return python_exe, subdir


def get_idf_version() -> str:
    version_file_path = os.path.join(global_idf_path, 'version.txt')  # type: ignore
    if os.path.exists(version_file_path):
        with open(version_file_path, 'r') as version_file:
            idf_version_str = version_file.read()
    else:
        idf_version_str = ''
        try:
            idf_version_str = subprocess.check_output(['git', 'describe'],
                                                      cwd=global_idf_path, env=os.environ,
                                                      stderr=subprocess.DEVNULL).decode()
        except OSError:
            # OSError should cover FileNotFoundError and WindowsError
            warn('Git was not found')
        except subprocess.CalledProcessError:
            # This happens quite often when the repo is shallow. Don't print a warning because there are other
            # possibilities for version detection.
            pass
    match = re.match(r'^v([0-9]+\.[0-9]+).*', idf_version_str)
    if match:
        idf_version = match.group(1)  # type: Optional[str]
    else:
        idf_version = None
        # fallback when IDF is a shallow clone
        try:
            with open(os.path.join(global_idf_path, 'components', 'esp_common', 'include', 'esp_idf_version.h')) as f:  # type: ignore
                m = re.search(r'^#define\s+ESP_IDF_VERSION_MAJOR\s+(\d+).+?^#define\s+ESP_IDF_VERSION_MINOR\s+(\d+)',
                              f.read(), re.DOTALL | re.MULTILINE)
                if m:
                    idf_version = '.'.join((m.group(1), m.group(2)))
                else:
                    warn('Reading IDF version from C header file failed!')
        except Exception as e:
            warn('Is it not possible to determine the IDF version: {}'.format(e))

    if idf_version is None:
        fatal('IDF version cannot be determined')
        raise SystemExit(1)

    return idf_version


def get_python_env_path() -> Tuple[str, str, str, str]:
    python_ver_major_minor = '{}.{}'.format(sys.version_info.major, sys.version_info.minor)

    idf_version = get_idf_version()
    idf_python_env_path = os.getenv('IDF_PYTHON_ENV_PATH') or os.path.join(global_idf_tools_path or '', 'python_env',
                                                                           'idf{}_py{}_env'.format(idf_version, python_ver_major_minor))

    python_exe, subdir = get_python_exe_and_subdir()
    idf_python_export_path = os.path.join(idf_python_env_path, subdir)
    virtualenv_python = os.path.join(idf_python_export_path, python_exe)

    return idf_python_env_path, idf_python_export_path, virtualenv_python, idf_version


def add_and_check_targets(idf_env_obj, targets_str):  # type: (IDFEnv, str) -> list[str]
    """
    Define targets from targets_str, check that the target names are valid and add them to idf_env_obj
    """
    targets_from_tools_json = get_all_targets_from_tools_json()
    invalid_targets = []

    targets_str = targets_str.lower()
    targets = targets_str.replace('-', '').split(',')
    if targets != ['all']:
        invalid_targets = [t for t in targets if t not in targets_from_tools_json]
        if invalid_targets:
            warn('Targets: "{}" are not supported. Only allowed options are: {}.'.format(', '.join(invalid_targets), ', '.join(targets_from_tools_json)))
            raise SystemExit(1)
        idf_env_obj.get_active_idf_record().extend_targets(targets)
    else:
        idf_env_obj.get_active_idf_record().extend_targets(targets_from_tools_json)

    return idf_env_obj.get_active_idf_record().targets


def feature_to_requirements_path(feature):  # type: (str) -> str
    return os.path.join(global_idf_path or '', 'tools', 'requirements', 'requirements.{}.txt'.format(feature))


def process_and_check_features(idf_env_obj, features_str):  # type: (IDFEnv, str) -> list[str]
    new_features = []
    remove_features = []
    for new_feature_candidate in features_str.split(','):
        if new_feature_candidate.startswith('-'):
            remove_features += [new_feature_candidate.lstrip('-')]
        else:
            new_feature_candidate = new_feature_candidate.lstrip('+')
            # Feature to be added needs to be checked if is valid
            if os.path.isfile(feature_to_requirements_path(new_feature_candidate)):
                new_features += [new_feature_candidate]
    idf_env_obj.get_active_idf_record().update_features(tuple(new_features), tuple(remove_features))
    return idf_env_obj.get_active_idf_record().features


def get_all_targets_from_tools_json():  # type: () -> list[str]
    tools_info = load_tools_info()
    targets_from_tools_json = []  # type: list[str]

    for _, v in tools_info.items():
        targets_from_tools_json.extend(v.get_supported_targets())
    # remove duplicates
    targets_from_tools_json = list(set(targets_from_tools_json))
    if 'all' in targets_from_tools_json:
        targets_from_tools_json.remove('all')
    return sorted(targets_from_tools_json)


def filter_tools_info(idf_env_obj, tools_info):  # type: (IDFEnv, OrderedDict[str, IDFTool]) -> OrderedDict[str,IDFTool]
    targets = idf_env_obj.get_active_idf_record().targets
    if not targets:
        return tools_info
    else:
        filtered_tools_spec = {k:v for k, v in tools_info.items() if
                               (v.get_install_type() == IDFTool.INSTALL_ALWAYS or v.get_install_type() == IDFTool.INSTALL_ON_REQUEST) and
                               (any(item in targets for item in v.get_supported_targets()) or v.get_supported_targets() == ['all'])}
        return OrderedDict(filtered_tools_spec)


def add_variables_to_deactivate_file(args, new_idf_vars):  # type: (list[str], dict[str, Any]) -> str
    """
    Add IDF global variables that need to be removed when the active esp-idf environment is deactivated.
    """
    if 'PATH' in new_idf_vars:
        new_idf_vars['PATH'] = new_idf_vars['PATH'].split(':')[:-1]  # PATH is stored as list of sub-paths without '$PATH'

    new_idf_vars['PATH'] = new_idf_vars.get('PATH', [])
    args_add_paths_extras = vars(args).get('add_paths_extras')  # remove mypy error with args
    new_idf_vars['PATH'] = new_idf_vars['PATH'] + args_add_paths_extras.split(':') if args_add_paths_extras else new_idf_vars['PATH']

    env_state_obj = ENVState.get_env_state()

    if env_state_obj.idf_variables:
        exported_idf_vars = env_state_obj.idf_variables
        new_idf_vars['PATH'] = list(set(new_idf_vars['PATH'] + exported_idf_vars.get('PATH', [])))  # remove duplicates
        env_state_obj.idf_variables = dict(exported_idf_vars, **new_idf_vars)  # merge two dicts
    else:
        env_state_obj.idf_variables = new_idf_vars
    deactivate_file_path = env_state_obj.save()

    return deactivate_file_path


def deactivate_statement(args):  # type: (list[str]) -> None
    """
    Deactivate statement is sequence of commands, that remove IDF global variables from enviroment,
        so the environment gets to the state it was before calling export.{sh/fish} script.
    """
    env_state_obj = ENVState.get_env_state()
    if not env_state_obj.idf_variables:
        warn('No IDF variables to remove from environment found. Deactivation of previous esp-idf version was not successful.')
        return
    unset_vars = env_state_obj.idf_variables
    env_path = os.getenv('PATH')  # type: Optional[str]
    if env_path:
        cleared_env_path = ':'.join([k for k in env_path.split(':') if k not in unset_vars['PATH']])

    unset_list = [k for k in unset_vars.keys() if k != 'PATH']
    unset_format, sep = get_unset_format_and_separator(args)
    unset_statement = sep.join([unset_format.format(k) for k in unset_list])

    export_format, sep = get_export_format_and_separator(args)
    export_statement = export_format.format('PATH', cleared_env_path)

    deactivate_statement_str = sep.join([unset_statement, export_statement])

    print(deactivate_statement_str)
    # After deactivation clear old variables
    env_state_obj.idf_variables.clear()
    env_state_obj.save()
    return


def get_export_format_and_separator(args):  # type: (list[str]) -> Tuple[str, str]
    return {EXPORT_SHELL: ('export {}="{}"', ';'), EXPORT_KEY_VALUE: ('{}={}', '\n')}[args.format]  # type: ignore


def get_unset_format_and_separator(args):  # type: (list[str]) -> Tuple[str, str]
    return {EXPORT_SHELL: ('unset {}', ';'), EXPORT_KEY_VALUE: ('{}', '\n')}[args.format]  # type: ignore


def different_idf_detected() -> bool:

    # If IDF global variable found, test if belong to different ESP-IDF version
    if 'IDF_TOOLS_EXPORT_CMD' in os.environ:
        if global_idf_path != os.path.dirname(os.environ['IDF_TOOLS_EXPORT_CMD']):
            return True

    # No previous ESP-IDF export detected, nothing to be unset
    if all(s not in os.environ for s in ['IDF_PYTHON_ENV_PATH', 'OPENOCD_SCRIPTS', 'ESP_IDF_VERSION']):
        return False

    # User is exporting the same version as is in env
    if os.getenv('ESP_IDF_VERSION') == get_idf_version():
        return False

    # Different version detected
    return True


# Function returns unique id of running ESP-IDF combining current idfpath with version.
# The id is unique with same version & different path or same path & different version.
def active_repo_id() -> str:
    if global_idf_path is None:
        return 'UNKNOWN_PATH' + '-v' + get_idf_version()
    return global_idf_path + '-v' + get_idf_version()


def list_default(args):  # type: ignore
    tools_info = load_tools_info()
    for name, tool in tools_info.items():
        if tool.get_install_type() == IDFTool.INSTALL_NEVER:
            continue
        optional_str = ' (optional)' if tool.get_install_type() == IDFTool.INSTALL_ON_REQUEST else ''
        info('* {}: {}{}'.format(name, tool.description, optional_str))
        tool.find_installed_versions()
        versions_for_platform = {k: v for k, v in tool.versions.items() if v.compatible_with_platform()}
        if not versions_for_platform:
            info('  (no versions compatible with platform {})'.format(PYTHON_PLATFORM))
            continue
        versions_sorted = sorted(versions_for_platform.keys(), key=tool.versions.get, reverse=True)  # type: ignore
        for version in versions_sorted:
            version_obj = tool.versions[version]
            info('  - {} ({}{})'.format(version, version_obj.status,
                                        ', installed' if version in tool.versions_installed else ''))


def list_outdated(args):  # type: ignore
    tools_info = load_tools_info()
    for name, tool in tools_info.items():
        if tool.get_install_type() == IDFTool.INSTALL_NEVER:
            continue
        versions_for_platform = {k: v for k, v in tool.versions.items() if v.compatible_with_platform()}
        if not versions_for_platform:
            continue
        version_installed = tool.latest_installed_version()
        if not version_installed:
            continue
        version_available = sorted(versions_for_platform.keys(), key=tool.versions.get, reverse=True)[0]
        if version_installed < version_available:
            info(f'{name}: version {version_installed} is outdated by {version_available}')


def action_list(args):  # type: ignore
    if args.outdated:
        list_outdated(args)
    else:
        list_default(args)


def action_check(args):  # type: ignore
    tools_info = load_tools_info()
    tools_info = filter_tools_info(IDFEnv.get_idf_env(), tools_info)
    not_found_list = []
    info('Checking for installed tools...')
    for name, tool in tools_info.items():
        if tool.get_install_type() == IDFTool.INSTALL_NEVER:
            continue
        tool_found_somewhere = False
        info('Checking tool %s' % name)
        tool.find_installed_versions()
        if tool.version_in_path:
            info('    version found in PATH: %s' % tool.version_in_path)
            tool_found_somewhere = True
        else:
            info('    no version found in PATH')

        for version in tool.versions_installed:
            info('    version installed in tools directory: %s' % version)
            tool_found_somewhere = True
        if not tool_found_somewhere and tool.get_install_type() == IDFTool.INSTALL_ALWAYS:
            not_found_list.append(name)
    if not_found_list:
        fatal('The following required tools were not found: ' + ' '.join(not_found_list))
        raise SystemExit(1)


def action_export(args):  # type: ignore
    if args.deactivate and different_idf_detected():
        deactivate_statement(args)
        return

    tools_info = load_tools_info()
    tools_info = filter_tools_info(IDFEnv.get_idf_env(), tools_info)
    all_tools_found = True
    export_vars = {}
    paths_to_export = []

    self_restart_cmd = f'{sys.executable} {__file__}{(" --tools-json " + args.tools_json) if args.tools_json else ""}'
    self_restart_cmd = to_shell_specific_paths([self_restart_cmd])[0]
    prefer_system_hint = '' if IDF_TOOLS_EXPORT_CMD else f' To use it, run \'{self_restart_cmd} export --prefer-system\''
    install_cmd = to_shell_specific_paths([IDF_TOOLS_INSTALL_CMD])[0] if IDF_TOOLS_INSTALL_CMD else self_restart_cmd + ' install'

    for name, tool in tools_info.items():
        if tool.get_install_type() == IDFTool.INSTALL_NEVER:
            continue
        tool.find_installed_versions()
        version_to_use = tool.get_preferred_installed_version()

        if not tool.is_executable and version_to_use:
            tool_export_vars = tool.get_export_vars(version_to_use)
            export_vars = {**export_vars, **tool_export_vars}
            continue

        if tool.version_in_path:
            if tool.version_in_path not in tool.versions:
                # unsupported version
                if args.prefer_system:  # type: ignore
                    warn('using an unsupported version of tool {} found in PATH: {}'.format(
                        tool.name, tool.version_in_path))
                    continue
                else:
                    # unsupported version in path
                    pass
            else:
                # supported/deprecated version in PATH, use it
                version_obj = tool.versions[tool.version_in_path]
                if version_obj.status == IDFToolVersion.STATUS_SUPPORTED:
                    info('Using a supported version of tool {} found in PATH: {}.'.format(name, tool.version_in_path),
                         f=sys.stderr)
                    info('However the recommended version is {}.'.format(tool.get_recommended_version()),
                         f=sys.stderr)
                elif version_obj.status == IDFToolVersion.STATUS_DEPRECATED:
                    warn('using a deprecated version of tool {} found in PATH: {}'.format(name, tool.version_in_path))
                continue

        if not tool.versions_installed:
            if tool.get_install_type() == IDFTool.INSTALL_ALWAYS:
                all_tools_found = False
                fatal('tool {} has no installed versions. Please run \'{}\' to install it.'.format(
                    tool.name, install_cmd))
                if tool.version_in_path and tool.version_in_path not in tool.versions:
                    info('An unsupported version of tool {} was found in PATH: {}. '.format(name, tool.version_in_path) +
                         prefer_system_hint, f=sys.stderr)
                continue
            else:
                # tool is optional, and does not have versions installed
                # use whatever is available in PATH
                continue

        if tool.version_in_path and tool.version_in_path not in tool.versions:
            info('Not using an unsupported version of tool {} found in PATH: {}.'.format(
                 tool.name, tool.version_in_path) + prefer_system_hint, f=sys.stderr)

        export_paths = tool.get_export_paths(version_to_use)
        if export_paths:
            paths_to_export += export_paths
        tool_export_vars = tool.get_export_vars(version_to_use)
        export_vars = {**export_vars, **tool_export_vars}

    current_path = os.getenv('PATH')
    idf_python_env_path, idf_python_export_path, virtualenv_python, _ = get_python_env_path()
    if os.path.exists(virtualenv_python):
        idf_python_env_path = to_shell_specific_paths([idf_python_env_path])[0]
        if os.getenv('IDF_PYTHON_ENV_PATH') != idf_python_env_path:
            export_vars['IDF_PYTHON_ENV_PATH'] = to_shell_specific_paths([idf_python_env_path])[0]
        if idf_python_export_path not in current_path:
            paths_to_export.append(idf_python_export_path)

    idf_version = get_idf_version()
    if os.getenv('ESP_IDF_VERSION') != idf_version:
        export_vars['ESP_IDF_VERSION'] = idf_version

    idf_tools_dir = os.path.join(global_idf_path, 'tools')
    idf_tools_dir = to_shell_specific_paths([idf_tools_dir])[0]
    if idf_tools_dir not in current_path:
        paths_to_export.append(idf_tools_dir)

    if sys.platform == 'win32':
        old_path = '%PATH%'
        path_sep = ';'
    else:
        old_path = '$PATH'
        path_sep = ':'

    export_format, export_sep = get_export_format_and_separator(args)

    if paths_to_export:
        export_vars['PATH'] = path_sep.join(to_shell_specific_paths(paths_to_export) + [old_path])

    if export_vars:
        # if not copy of export_vars is given to function, it brekas the formatting string for 'export_statements'
        deactivate_file_path = add_variables_to_deactivate_file(args, export_vars.copy())
        export_vars[ENVState.env_key] = deactivate_file_path
        export_statements = export_sep.join([export_format.format(k, v) for k, v in export_vars.items()])
        print(export_statements)

    if not all_tools_found:
        raise SystemExit(1)


def get_idf_download_url_apply_mirrors(args=None, download_url=IDF_DL_URL):  # type: (Any, str) -> str
    url = apply_mirror_prefix_map(args, download_url)
    url = apply_github_assets_option(url)
    return url


def apply_mirror_prefix_map(args, idf_download_url):  # type: (Any, str) -> str
    """Rewrite URL for given idf_download_url.
       if --mirror-prefix-map flag or IDF_MIRROR_PREFIX_MAP environment variable is given.
    """
    new_url = idf_download_url
    mirror_prefix_map = None
    mirror_prefix_map_env = os.getenv('IDF_MIRROR_PREFIX_MAP')
    if mirror_prefix_map_env:
        mirror_prefix_map = mirror_prefix_map_env.split(';')
    if IDF_MAINTAINER and args and args.mirror_prefix_map:
        if mirror_prefix_map:
            warn('Both IDF_MIRROR_PREFIX_MAP environment variable and --mirror-prefix-map flag are specified, ' +
                 'will use the value from the command line.')
        mirror_prefix_map = args.mirror_prefix_map
    if mirror_prefix_map:
        for item in mirror_prefix_map:
            if URL_PREFIX_MAP_SEPARATOR not in item:
                warn('invalid mirror-prefix-map item (missing \'{}\') {}'.format(URL_PREFIX_MAP_SEPARATOR, item))
                continue
            search, replace = item.split(URL_PREFIX_MAP_SEPARATOR, 1)
            new_url = re.sub(search, replace, idf_download_url)
            if new_url != idf_download_url:
                info('Changed download URL: {} => {}'.format(idf_download_url, new_url))
                break
    return new_url


def apply_github_assets_option(idf_download_url):  # type: (str) -> str
    """ Rewrite URL for given idf_download_url if the download URL is an https://github.com/ URL and the variable
    IDF_GITHUB_ASSETS is set. The github.com part of the URL will be replaced.
    """
    new_url = idf_download_url
    github_assets = os.environ.get('IDF_GITHUB_ASSETS', '').strip()
    if not github_assets:
        # no IDF_GITHUB_ASSETS or variable exists but is empty
        return new_url

    # check no URL qualifier in the mirror URL
    if '://' in github_assets:
        fatal("IDF_GITHUB_ASSETS shouldn't include any URL qualifier, https:// is assumed")
        raise SystemExit(1)

    # Strip any trailing / from the mirror URL
    github_assets = github_assets.rstrip('/')

    new_url = re.sub(r'^https://github.com/', 'https://{}/'.format(github_assets), idf_download_url)
    if new_url != idf_download_url:
        info('Using GitHub assets mirror for URL: {} => {}'.format(idf_download_url, new_url))
    return new_url


def get_tools_spec_and_platform_info(selected_platform, targets, tools_spec,
                                     quiet=False):  # type: (Optional[str], list[str], list[str], bool) -> Tuple[list[str], Dict[str, IDFTool]]
    selected_platform = Platforms.get(selected_platform)
    if selected_platform is None:
        fatal(f'unknown platform: {selected_platform}')
        raise SystemExit(1)

    # If this function is not called from action_download, but is used just for detecting active tools, info about downloading is unwanted.
    global global_quiet
    try:
        old_global_quiet = global_quiet
        global_quiet = quiet
        tools_info = load_tools_info()
        tools_info_for_platform = OrderedDict()
        for name, tool_obj in tools_info.items():
            tool_for_platform = tool_obj.copy_for_platform(selected_platform)
            tools_info_for_platform[name] = tool_for_platform

        if not tools_spec or 'required' in tools_spec:
            # Downloading tools for all ESP_targets required by the operating system.
            tools_spec = [k for k, v in tools_info_for_platform.items() if v.get_install_type() == IDFTool.INSTALL_ALWAYS]
            # Filtering tools user defined list of ESP_targets
            if 'all' not in targets:
                def is_tool_selected(tool):  # type: (IDFTool) -> bool
                    supported_targets = tool.get_supported_targets()
                    return (any(item in targets for item in supported_targets) or supported_targets == ['all'])
                tools_spec = [k for k in tools_spec if is_tool_selected(tools_info[k])]
            info('Downloading tools for {}: {}'.format(selected_platform, ', '.join(tools_spec)))

        # Downloading tools for all ESP_targets (MacOS, Windows, Linux)
        elif 'all' in tools_spec:
            tools_spec = [k for k, v in tools_info_for_platform.items() if v.get_install_type() != IDFTool.INSTALL_NEVER]
            info('Downloading tools for {}: {}'.format(selected_platform, ', '.join(tools_spec)))
    finally:
        global_quiet = old_global_quiet

    return tools_spec, tools_info_for_platform


def action_download(args):  # type: ignore
    tools_spec = args.tools
    targets = []  # type: list[str]
    # Downloading tools required for defined ESP_targets
    if 'required' in tools_spec:
        idf_env_obj = IDFEnv.get_idf_env()
        targets = add_and_check_targets(idf_env_obj, args.targets)
        try:
            idf_env_obj.save()
        except OSError as err:
            if args.targets in targets:
                targets.remove(args.targets)
            warn('Downloading tools for targets was not successful with error: {}'.format(err))

    tools_spec, tools_info_for_platform = get_tools_spec_and_platform_info(args.platform, targets, args.tools)

    for tool_spec in tools_spec:
        if '@' not in tool_spec:
            tool_name = tool_spec
            tool_version = None
        else:
            tool_name, tool_version = tool_spec.split('@', 1)
        if tool_name not in tools_info_for_platform:
            fatal('unknown tool name: {}'.format(tool_name))
            raise SystemExit(1)
        tool_obj = tools_info_for_platform[tool_name]
        if tool_version is not None and tool_version not in tool_obj.versions:
            fatal('unknown version for tool {}: {}'.format(tool_name, tool_version))
            raise SystemExit(1)
        if tool_version is None:
            tool_version = tool_obj.get_recommended_version()
        if tool_version is None:
            fatal('tool {} not found for {} platform'.format(tool_name, platform))
            raise SystemExit(1)
        tool_spec = '{}@{}'.format(tool_name, tool_version)

        info('Downloading {}'.format(tool_spec))
        _idf_tool_obj = tool_obj.versions[tool_version].get_download_for_platform(args.platform)
        _idf_tool_obj.url = get_idf_download_url_apply_mirrors(args, _idf_tool_obj.url)

        tool_obj.download(tool_version)


def action_install(args):  # type: ignore
    tools_info = load_tools_info()
    tools_spec = args.tools  # type: ignore
    targets = []  # type: list[str]
    info('Current system platform: {}'.format(CURRENT_PLATFORM))
    # No single tool '<tool_name>@<version>' was defined, install whole toolchains
    if 'required' in tools_spec or 'all' in tools_spec:
        idf_env_obj = IDFEnv.get_idf_env()
        targets = add_and_check_targets(idf_env_obj, args.targets)
        try:
            idf_env_obj.save()
        except OSError as err:
            if args.targets in targets:
                targets.remove(args.targets)
            warn('Installing targets was not successful with error: {}'.format(err))
        info('Selected targets are: {}'.format(', '.join(targets)))

        # Installing tools for defined ESP_targets
        if 'required' in tools_spec:
            tools_spec = [k for k, v in tools_info.items() if v.get_install_type() == IDFTool.INSTALL_ALWAYS]
            # If only some ESP_targets are defined, filter tools for those
            if len(get_all_targets_from_tools_json()) != len(targets):
                def is_tool_selected(tool):  # type: (IDFTool) -> bool
                    supported_targets = tool.get_supported_targets()
                    return (any(item in targets for item in supported_targets) or supported_targets == ['all'])
                tools_spec = [k for k in tools_spec if is_tool_selected(tools_info[k])]
            info('Installing tools: {}'.format(', '.join(tools_spec)))

        # Installing all available tools for all operating systems (MacOS, Windows, Linux)
        else:
            tools_spec = [k for k, v in tools_info.items() if v.get_install_type() != IDFTool.INSTALL_NEVER]
            info('Installing tools: {}'.format(', '.join(tools_spec)))

    for tool_spec in tools_spec:
        if '@' not in tool_spec:
            tool_name = tool_spec
            tool_version = None
        else:
            tool_name, tool_version = tool_spec.split('@', 1)
        if tool_name not in tools_info:
            fatal('unknown tool name: {}'.format(tool_name))
            raise SystemExit(1)
        tool_obj = tools_info[tool_name]
        if not tool_obj.compatible_with_platform():
            fatal('tool {} does not have versions compatible with platform {}'.format(tool_name, CURRENT_PLATFORM))
            raise SystemExit(1)
        if tool_version is not None and tool_version not in tool_obj.versions:
            fatal('unknown version for tool {}: {}'.format(tool_name, tool_version))
            raise SystemExit(1)
        if tool_version is None:
            tool_version = tool_obj.get_recommended_version()
        assert tool_version is not None
        tool_obj.find_installed_versions()
        tool_spec = '{}@{}'.format(tool_name, tool_version)
        if tool_version in tool_obj.versions_installed:
            info('Skipping {} (already installed)'.format(tool_spec))
            continue

        info('Installing {}'.format(tool_spec))
        _idf_tool_obj = tool_obj.versions[tool_version].get_download_for_platform(PYTHON_PLATFORM)
        _idf_tool_obj.url = get_idf_download_url_apply_mirrors(args, _idf_tool_obj.url)

        tool_obj.download(tool_version)
        tool_obj.install(tool_version)


def get_wheels_dir():  # type: () -> Optional[str]
    tools_info = load_tools_info()
    wheels_package_name = 'idf-python-wheels'
    if wheels_package_name not in tools_info:
        return None
    wheels_package = tools_info[wheels_package_name]
    recommended_version = wheels_package.get_recommended_version()
    if recommended_version is None:
        return None
    wheels_dir = wheels_package.get_path_for_version(recommended_version)
    if not os.path.exists(wheels_dir):
        return None
    return wheels_dir


def get_requirements(new_features):  # type: (str) -> list[str]
    idf_env_obj = IDFEnv.get_idf_env()
    features = process_and_check_features(idf_env_obj, new_features)
    try:
        idf_env_obj.save()
    except OSError as err:
        if new_features in features:
            features.remove(new_features)
        warn('Updating features was not successful with error: {}'.format(err))
    return [feature_to_requirements_path(feature) for feature in features]


def get_constraints(idf_version, online=True):  # type: (str, bool) -> str
    idf_download_url = get_idf_download_url_apply_mirrors()
    constraint_file = 'espidf.constraints.v{}.txt'.format(idf_version)
    constraint_path = os.path.join(global_idf_tools_path or '', constraint_file)
    constraint_url = '/'.join([idf_download_url, constraint_file])
    temp_path = constraint_path + '.tmp'

    if not online:
        if os.path.isfile(constraint_path):
            return constraint_path
        else:
            fatal(f'{constraint_path} doesn\'t exist. Perhaps you\'ve forgotten to run the install scripts. '
                  f'Please check the installation guide for more information.')
            raise SystemExit(1)

    mkdir_p(os.path.dirname(temp_path))

    try:
        age = datetime.date.today() - datetime.date.fromtimestamp(os.path.getmtime(constraint_path))
        if age < datetime.timedelta(days=1):
            info(f'Skipping the download of {constraint_path} because it was downloaded recently.')
            return constraint_path
    except OSError:
        # doesn't exist or inaccessible
        pass

    for _ in range(DOWNLOAD_RETRY_COUNT):
        err = download(constraint_url, temp_path)
        if not os.path.isfile(temp_path):
            warn('Download failure: {}'.format(err))
            warn('Failed to download {} to {}'.format(constraint_url, temp_path))
            continue
        if os.path.isfile(constraint_path):
            # Windows cannot rename to existing file. It needs to be deleted.
            os.remove(constraint_path)
        rename_with_retry(temp_path, constraint_path)
        return constraint_path

    if os.path.isfile(constraint_path):
        warn('Failed to download, retry count has expired, using a previously downloaded version')
        return constraint_path
    else:
        fatal('Failed to download, and retry count has expired')
        print_hints_on_download_error(str(err))
        info('See the help on how to disable constraints in order to work around this issue.')
        raise SystemExit(1)


def install_legacy_python_virtualenv(path):  # type: (str) -> None
    # Before creating the virtual environment, check if pip is installed.
    try:
        subprocess.check_call([sys.executable, '-m', 'pip', '--version'])
    except subprocess.CalledProcessError:
        fatal('Python interpreter at {} doesn\'t have pip installed. '
              'Please check the Getting Started Guides for the steps to install prerequisites for your OS.'.format(sys.executable))
        raise SystemExit(1)

    virtualenv_installed_via_pip = False
    try:
        import virtualenv  # noqa: F401
    except ImportError:
        info('Installing virtualenv')
        subprocess.check_call([sys.executable, '-m', 'pip', 'install', '--user', 'virtualenv'],
                              stdout=sys.stdout, stderr=sys.stderr)
        virtualenv_installed_via_pip = True
        # since we just installed virtualenv via pip, we know that version is recent enough
        # so the version check below is not necessary.

    with_seeder_option = True
    if not virtualenv_installed_via_pip:
        # virtualenv is already present in the system and may have been installed via OS package manager
        # check the version to determine if we should add --seeder option
        try:
            major_ver = int(virtualenv.__version__.split('.')[0])
            if major_ver < 20:
                warn('Virtualenv version {} is old, please consider upgrading it'.format(virtualenv.__version__))
                with_seeder_option = False
        except (ValueError, NameError, AttributeError, IndexError):
            pass

    info(f'Creating a new Python environment using virtualenv in {path}')
    virtualenv_options = ['--python', sys.executable]
    if with_seeder_option:
        virtualenv_options += ['--seeder', 'pip']

    subprocess.check_call([sys.executable, '-m', 'virtualenv',
                           *virtualenv_options,
                           path],
                          stdout=sys.stdout, stderr=sys.stderr)


def action_install_python_env(args):  # type: ignore
    use_constraints = not args.no_constraints
    reinstall = args.reinstall
    idf_python_env_path, _, virtualenv_python, idf_version = get_python_env_path()

    is_virtualenv = hasattr(sys, 'real_prefix') or (hasattr(sys, 'base_prefix') and sys.base_prefix != sys.prefix)
    if is_virtualenv and (not os.path.exists(idf_python_env_path) or reinstall):
        fatal('This script was called from a virtual environment, can not create a virtual environment again')
        raise SystemExit(1)

    if os.path.exists(virtualenv_python):
        try:
            subprocess.check_call([virtualenv_python, '--version'], stdout=sys.stdout, stderr=sys.stderr)
        except (OSError, subprocess.CalledProcessError):
            # At this point we can reinstall the virtual environment if it is non-functional. This can happen at least
            # when the Python interpreter was removed which was used to create the virtual environment.
            reinstall = True

        try:
            subprocess.check_call([virtualenv_python, '-m', 'pip', '--version'], stdout=sys.stdout, stderr=sys.stderr)
        except subprocess.CalledProcessError:
            warn('pip is not available in the existing virtual environment, new virtual environment will be created.')
            # Reinstallation of the virtual environment could help if pip was installed for the main Python
            reinstall = True

    if reinstall and os.path.exists(idf_python_env_path):
        warn('Removing the existing Python environment in {}'.format(idf_python_env_path))
        shutil.rmtree(idf_python_env_path)

    venv_can_upgrade = False

    if not os.path.exists(virtualenv_python):
        if subprocess.run([sys.executable, '-m', 'venv', '-h'], check=False, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode == 0:
            # venv available
            virtualenv_options = ['--clear']  # delete environment if already exists
            if sys.version_info[:2] >= (3, 9):
                # upgrade pip & setuptools
                virtualenv_options += ['--upgrade-deps']
                venv_can_upgrade = True

            info('Creating a new Python environment in {}'.format(idf_python_env_path))
            subprocess.check_call([sys.executable, '-m', 'venv',
                                  *virtualenv_options,
                                  idf_python_env_path],
                                  stdout=sys.stdout, stderr=sys.stderr)
        else:
            # The embeddable Python for Windows doesn't have the built-in venv module
            install_legacy_python_virtualenv(idf_python_env_path)

    env_copy = os.environ.copy()
    if env_copy.get('PIP_USER')  == 'yes':
        warn('Found PIP_USER="yes" in the environment. Disabling PIP_USER in this shell to install packages into a virtual environment.')
        env_copy['PIP_USER'] = 'no'

    if not venv_can_upgrade:
        info('Upgrading pip and setuptools...')
        subprocess.check_call([virtualenv_python, '-m', 'pip', 'install', '--upgrade', 'pip', 'setuptools'],
                              stdout=sys.stdout, stderr=sys.stderr, env=env_copy)

    run_args = [virtualenv_python, '-m', 'pip', 'install', '--no-warn-script-location']
    requirements_file_list = get_requirements(args.features)
    for requirement_file in requirements_file_list:
        run_args += ['-r', requirement_file]
    if use_constraints:
        constraint_file = get_constraints(idf_version)
        run_args += ['--upgrade', '--constraint', constraint_file]
    if args.extra_wheels_dir:
        run_args += ['--find-links', args.extra_wheels_dir]
    if args.no_index:
        run_args += ['--no-index']
    if args.extra_wheels_url:
        run_args += ['--extra-index-url', args.extra_wheels_url]

    wheels_dir = get_wheels_dir()
    if wheels_dir is not None:
        run_args += ['--find-links', wheels_dir]

    info('Installing Python packages')
    if use_constraints:
        info(' Constraint file: {}'.format(constraint_file))
    info(' Requirement files:')
    info(os.linesep.join('  - {}'.format(path) for path in requirements_file_list))
    subprocess.check_call(run_args, stdout=sys.stdout, stderr=sys.stderr, env=env_copy)


def action_check_python_dependencies(args):  # type: ignore
    use_constraints = not args.no_constraints
    req_paths = get_requirements('')  # no new features -> just detect the existing ones

    _, _, virtualenv_python, idf_version = get_python_env_path()

    if not os.path.isfile(virtualenv_python):
        fatal('{} doesn\'t exist! Please run the install script or "idf_tools.py install-python-env" in order to '
              'create it'.format(virtualenv_python))
        raise SystemExit(1)

    if use_constraints:
        constr_path = get_constraints(idf_version, online=False)  # keep offline for checking
        info('Constraint file: {}'.format(constr_path))

    info('Requirement files:')
    info(os.linesep.join(' - {}'.format(path) for path in req_paths))

    info('Python being checked: {}'.format(virtualenv_python))

    # The dependency checker will be invoked with virtualenv_python. idf_tools.py could have been invoked with a
    # different one, therefore, importing is not a suitable option.
    dep_check_cmd = [virtualenv_python,
                     os.path.join(global_idf_path,
                                  'tools',
                                  'check_python_dependencies.py')]

    if use_constraints:
        dep_check_cmd += ['-c', constr_path]

    for req_path in req_paths:
        dep_check_cmd += ['-r', req_path]

    try:
        ret = subprocess.run(dep_check_cmd)
        if ret and ret.returncode:
            # returncode is a negative number and system exit output is usually expected be positive.
            raise SystemExit(-ret.returncode)
    except FileNotFoundError:
        # Python environment not yet created
        fatal('Requirements are not satisfied!')
        raise SystemExit(1)


class ChecksumCalculator():
    """
    A class used to get size/checksum/basename of local artifact files.
    """
    def __init__(self, files):  # type: (list[str]) -> None
        self.files = files

    def __iter__(self):  # type: () -> Iterator[Tuple[int, str, str]]
        for f in self.files:
            yield (*get_file_size_sha256(f), os.path.basename(f))


class ChecksumParsingError(RuntimeError):
    pass


class ChecksumFileParser():
    """
    A class used to get size/sha256/filename of artifact using checksum-file with format:
        # <artifact-filename>: <size> bytes
        <sha256sum-string> *<artifact-filename>
        ... (2 lines for every artifact) ...
    """
    def __init__(self, tool_name, url):  # type: (str, str) -> None
        self.tool_name = tool_name

        sha256_file_tmp = os.path.join(global_idf_tools_path or '', 'tools', 'add-version.sha256.tmp')
        sha256_file = os.path.abspath(url)

        # download sha256 file if URL presented
        if urlparse(url).scheme:
            sha256_file = sha256_file_tmp
            download(url, sha256_file)

        with open(sha256_file, 'r') as f:
            self.checksum = f.read().splitlines()

        # remove temp file
        if os.path.isfile(sha256_file_tmp):
            os.remove(sha256_file_tmp)

    def parseLine(self, regex, line):  # type: (str, str) -> str
        match = re.search(regex, line)
        if not match:
            raise ChecksumParsingError(f'Can not parse line "{line}" with regex "{regex}"')
        return match.group(1)

    # parse checksum file with formatting used by crosstool-ng, gdb, ... releases
    # e.g. https://github.com/espressif/crosstool-NG/releases/download/esp-2021r2/crosstool-NG-esp-2021r2-checksum.sha256
    def __iter__(self):  # type: () -> Iterator[Tuple[int, str, str]]
        try:
            for bytes_str, hash_str in zip(self.checksum[0::2], self.checksum[1::2]):
                bytes_filename = self.parseLine(r'^# (\S*):', bytes_str)
                hash_filename = self.parseLine(r'^\S* \*(\S*)', hash_str)
                if hash_filename != bytes_filename:
                    fatal('filename in hash-line and in bytes-line are not the same')
                    raise SystemExit(1)
                # crosstool-ng checksum file contains info about few tools
                # e.g.: "xtensa-esp32-elf", "xtensa-esp32s2-elf"
                # filter records for file by tool_name to avoid mismatch
                if not hash_filename.startswith(self.tool_name):
                    continue
                size = self.parseLine(r'^# \S*: (\d*) bytes', bytes_str)
                sha256 = self.parseLine(r'^(\S*) ', hash_str)
                yield int(size), sha256, hash_filename
        except (TypeError, AttributeError) as err:
            fatal(f'Error while parsing, check checksum file ({err})')
            raise SystemExit(1)


def action_add_version(args):  # type: ignore
    tools_info = load_tools_info()
    tool_name = args.tool
    tool_obj = tools_info.get(tool_name)
    if not tool_obj:
        info('Creating new tool entry for {}'.format(tool_name))
        tool_obj = IDFTool(tool_name, TODO_MESSAGE, IDFTool.INSTALL_ALWAYS,
                           TODO_MESSAGE, TODO_MESSAGE, [TODO_MESSAGE], TODO_MESSAGE)
        tools_info[tool_name] = tool_obj
    version = args.version
    version_status = IDFToolVersion.STATUS_SUPPORTED
    if args.override and len(tool_obj.versions):
        tool_obj.drop_versions()
        version_status = IDFToolVersion.STATUS_RECOMMENDED
    version_obj = tool_obj.versions.get(version)
    if not version_obj:
        info('Creating new version {}'.format(version))
        version_obj = IDFToolVersion(version, version_status)
        tool_obj.versions[version] = version_obj
    url_prefix = args.url_prefix or 'https://%s/' % TODO_MESSAGE
    checksum_info = ChecksumFileParser(tool_name, args.checksum_file) if args.checksum_file else ChecksumCalculator(args.artifact_file)
    for file_size, file_sha256, file_name in checksum_info:
        # Guess which platform this file is for
        found_platform = Platforms.get_by_filename(file_name)
        if found_platform is None:
            info('Could not guess platform for file {}'.format(file_name))
            found_platform = TODO_MESSAGE
        url = urljoin(url_prefix, file_name)
        info('Adding download for platform {}'.format(found_platform))
        info('    size:   {}'.format(file_size))
        info('    SHA256: {}'.format(file_sha256))
        info('    URL:    {}'.format(url))
        version_obj.add_download(found_platform, url, file_size, file_sha256)
    json_str = dump_tools_json(tools_info)
    if not args.output:
        args.output = os.path.join(global_idf_path, TOOLS_FILE_NEW)
    with open(args.output, 'w') as f:
        f.write(json_str)
        f.write('\n')
    info('Wrote output to {}'.format(args.output))


def action_rewrite(args):  # type: ignore
    tools_info = load_tools_info()
    json_str = dump_tools_json(tools_info)
    if not args.output:
        args.output = os.path.join(global_idf_path, TOOLS_FILE_NEW)
    with open(args.output, 'w') as f:
        f.write(json_str)
        f.write('\n')
    info('Wrote output to {}'.format(args.output))


def action_uninstall(args):  # type: (Any) -> None
    """ Print or remove installed tools versions, that are not used by active ESP-IDF version anymore.
    Additionally remove all older versions of previously downloaded archives.
    """

    tools_info = load_tools_info()
    tools_path = os.path.join(global_idf_tools_path or '', 'tools')
    dist_path = os.path.join(global_idf_tools_path or '', 'dist')
    installed_tools = os.listdir(tools_path) if os.path.isdir(tools_path) else []

    unused_tools_versions = {}
    for tool in installed_tools:
        tool_versions = os.listdir(os.path.join(tools_path, tool)) if os.path.isdir(os.path.join(tools_path, tool)) else []
        try:
            unused_versions = ([x for x in tool_versions if x != tools_info[tool].get_recommended_version()])
        except KeyError:  # When tool that is not supported by tools_info (tools.json) anymore, remove the whole tool file
            unused_versions = ['']
        if unused_versions:
            unused_tools_versions[tool] = unused_versions

    # Keeping tools added by windows installer
    KEEP_WIN_TOOLS = ['idf-git', 'idf-python']
    for tool in KEEP_WIN_TOOLS:
        if tool in unused_tools_versions:
            unused_tools_versions.pop(tool)

    # Print unused tools.
    if args.dry_run:
        if unused_tools_versions:
            print('For removing old versions of {} use command \'{} {} {}\''.format(', '.join(unused_tools_versions), get_python_exe_and_subdir()[0],
                  os.path.join(global_idf_path or '', 'tools', 'idf_tools.py'), 'uninstall'))
        return

    # Remove installed tools that are not used by current ESP-IDF version.
    for tool in unused_tools_versions:
        for version in unused_tools_versions[tool]:
            try:
                if version:
                    path_to_remove = os.path.join(tools_path, tool, version)
                else:
                    path_to_remove = os.path.join(tools_path, tool)
                shutil.rmtree(path_to_remove)
                info(path_to_remove + ' was removed.')
            except OSError as error:
                warn(f'{error.filename} can not be removed because {error.strerror}.')

    # Remove old archives versions and archives that are not used by the current ESP-IDF version.
    if args.remove_archives:
        tools_spec, tools_info_for_platform = get_tools_spec_and_platform_info(CURRENT_PLATFORM, ['all'], ['all'], quiet=True)
        used_archives = []

        # Detect used active archives
        for tool_spec in tools_spec:
            if '@' not in tool_spec:
                tool_name = tool_spec
                tool_version = None
            else:
                tool_name, tool_version = tool_spec.split('@', 1)
            tool_obj = tools_info_for_platform[tool_name]
            if tool_version is None:
                tool_version = tool_obj.get_recommended_version()
            # mypy-checks
            if tool_version is not None:
                archive_version = tool_obj.versions[tool_version].get_download_for_platform(CURRENT_PLATFORM)
            if archive_version is not None:
                archive_version_url = archive_version.url

            archive = os.path.basename(archive_version_url)
            used_archives.append(archive)

        downloaded_archives = os.listdir(dist_path)
        for archive in downloaded_archives:
            if archive not in used_archives:
                os.remove(os.path.join(dist_path, archive))
                info(os.path.join(dist_path, archive) + ' was removed.')


def action_validate(args):  # type: ignore
    try:
        import jsonschema
    except ImportError:
        fatal('You need to install jsonschema package to use validate command')
        raise SystemExit(1)

    with open(os.path.join(global_idf_path, TOOLS_FILE), 'r') as tools_file:
        tools_json = json.load(tools_file)

    with open(os.path.join(global_idf_path, TOOLS_SCHEMA_FILE), 'r') as schema_file:
        schema_json = json.load(schema_file)
    jsonschema.validate(tools_json, schema_json)
    # on failure, this will raise an exception with a fairly verbose diagnostic message


def action_gen_doc(args):  # type: ignore
    f = args.output
    tools_info = load_tools_info()

    def print_out(text):  # type: (str) -> None
        f.write(text + '\n')

    print_out('.. |zwsp| unicode:: U+200B')
    print_out('   :trim:')
    print_out('')

    idf_gh_url = 'https://github.com/espressif/esp-idf'
    for tool_name, tool_obj in tools_info.items():
        info_url = tool_obj.options.info_url
        if idf_gh_url + '/tree' in info_url:
            info_url = re.sub(idf_gh_url + r'/tree/\w+/(.*)', r':idf:`\1`', info_url)

        license_url = 'https://spdx.org/licenses/' + tool_obj.options.license

        print_out("""
.. _tool-{name}:

{name}
{underline}

{description}

.. include:: idf-tools-notes.inc
   :start-after: tool-{name}-notes
   :end-before: ---

License: `{license} <{license_url}>`_

More info: {info_url}

.. list-table::
   :widths: 10 10 80
   :header-rows: 1

   * - Platform
     - Required
     - Download
""".rstrip().format(name=tool_name,
                    underline=args.heading_underline_char * len(tool_name),
                    description=tool_obj.description,
                    license=tool_obj.options.license,
                    license_url=license_url,
                    info_url=info_url))

        for platform_name in sorted(tool_obj.get_supported_platforms()):
            platform_tool = tool_obj.copy_for_platform(platform_name)
            install_type = platform_tool.get_install_type()
            if install_type == IDFTool.INSTALL_NEVER:
                continue
            elif install_type == IDFTool.INSTALL_ALWAYS:
                install_type_str = 'required'
            elif install_type == IDFTool.INSTALL_ON_REQUEST:
                install_type_str = 'optional'
            else:
                raise NotImplementedError()

            version = platform_tool.get_recommended_version()
            version_obj = platform_tool.versions[version]
            download_obj = version_obj.get_download_for_platform(platform_name)

            # Note: keep the list entries indented to the same number of columns
            # as the list header above.
            print_out("""
   * - {}
     - {}
     - {}

       .. rst-class:: tool-sha256

          SHA256: {}
""".strip('\n').format(platform_name, install_type_str, download_obj.url, download_obj.sha256))

        print_out('')
    print_out('')


def action_check_tool_supported(args):  # type: (Any) -> None
    """
    Print "True"/"False" to stdout as a result that tool is supported in IDF
    Print erorr message to stderr otherwise and set exit code to 1
    """
    try:
        tools_info = load_tools_info()
        for _, v in tools_info.items():
            if v.name == args.tool_name:
                print(v.check_version(args.exec_path))
                break
    except (RuntimeError, ToolNotFound, ToolExecError) as err:
        fatal(f'Failed to check tool support: (name: {args.tool_name}, exec: {args.exec_path})')
        fatal(f'{err}')
        raise SystemExit(1)


def action_get_tool_supported_versions(args):  # type: (Any) -> None
    """
    Print supported versions of a tool to stdout
    Print erorr message to stderr otherwise and set exit code to 1
    """
    try:
        tools_info = load_tools_info()
        for _, v in tools_info.items():
            if v.name == args.tool_name:
                print(list(v.versions.keys()))
                break
    except RuntimeError as err:
        fatal(f'Failed to get tool supported versions. (tool: {args.tool_name})')
        fatal(f'{err}')
        raise SystemExit(1)


def main(argv):  # type: (list[str]) -> None
    parser = argparse.ArgumentParser()

    parser.add_argument('--quiet', help='Don\'t output diagnostic messages to stdout/stderr', action='store_true')
    parser.add_argument('--non-interactive', help='Don\'t output interactive messages and questions', action='store_true')
    parser.add_argument('--tools-json', help='Path to the tools.json file to use')
    parser.add_argument('--idf-path', help='ESP-IDF path to use')

    subparsers = parser.add_subparsers(dest='action')
    list_parser = subparsers.add_parser('list', help='List tools and versions available')
    list_parser.add_argument('--outdated', help='Print only outdated installed tools', action='store_true')
    subparsers.add_parser('check', help='Print summary of tools installed or found in PATH')
    export = subparsers.add_parser('export', help='Output command for setting tool paths, suitable for shell')
    export.add_argument('--format', choices=[EXPORT_SHELL, EXPORT_KEY_VALUE], default=EXPORT_SHELL,
                        help='Format of the output: shell (suitable for printing into shell), ' +
                             'or key-value (suitable for parsing by other tools')
    export.add_argument('--prefer-system', help='Normally, if the tool is already present in PATH, ' +
                                                'but has an unsupported version, a version from the tools directory ' +
                                                'will be used instead. If this flag is given, the version in PATH ' +
                                                'will be used.', action='store_true')
    export.add_argument('--deactivate', help='Output command for deactivate different ESP-IDF version, previously set with export', action='store_true')
    export.add_argument('--unset', help=argparse.SUPPRESS, action='store_true')
    export.add_argument('--add_paths_extras', help='Add idf-related path extras for deactivate option')
    install = subparsers.add_parser('install', help='Download and install tools into the tools directory')
    install.add_argument('tools', metavar='TOOL', nargs='*', default=['required'],
                         help='Tools to install. ' +
                         'To install a specific version use <tool_name>@<version> syntax. ' +
                         'Use empty or \'required\' to install required tools, not optional ones. ' +
                         'Use \'all\' to install all tools, including the optional ones.')
    install.add_argument('--targets', default='all', help='A comma separated list of desired chip targets for installing.' +
                         ' It defaults to installing all supported targets.')

    download = subparsers.add_parser('download', help='Download the tools into the dist directory')
    download.add_argument('--platform', default=CURRENT_PLATFORM, help='Platform to download the tools for')
    download.add_argument('tools', metavar='TOOL', nargs='*', default=['required'],
                          help='Tools to download. ' +
                          'To download a specific version use <tool_name>@<version> syntax. ' +
                          'Use empty or \'required\' to download required tools, not optional ones. ' +
                          'Use \'all\' to download all tools, including the optional ones.')
    download.add_argument('--targets', default='all', help='A comma separated list of desired chip targets for installing.' +
                          ' It defaults to installing all supported targets.')

    uninstall = subparsers.add_parser('uninstall', help='Remove installed tools, that are not used by current version of ESP-IDF.')
    uninstall.add_argument('--dry-run', help='Print unused tools.', action='store_true')
    uninstall.add_argument('--remove-archives', help='Remove old archive versions and archives from unused tools.', action='store_true')

    no_constraints_default = os.environ.get('IDF_PYTHON_CHECK_CONSTRAINTS', '').lower() in ['0', 'n', 'no']

    if IDF_MAINTAINER:
        for subparser in [download, install]:
            subparser.add_argument('--mirror-prefix-map', nargs='*',
                                   help='Pattern to rewrite download URLs, with source and replacement separated by comma.' +
                                        ' E.g. http://foo.com,http://test.foo.com')

    install_python_env = subparsers.add_parser('install-python-env',
                                               help='Create Python virtual environment and install the ' +
                                                    'required Python packages')
    install_python_env.add_argument('--reinstall', help='Discard the previously installed environment',
                                    action='store_true')
    install_python_env.add_argument('--extra-wheels-dir', help='Additional directories with wheels ' +
                                    'to use during installation')
    install_python_env.add_argument('--extra-wheels-url', help='Additional URL with wheels', default=IDF_PIP_WHEELS_URL)
    install_python_env.add_argument('--no-index', help='Work offline without retrieving wheels index')
    install_python_env.add_argument('--features', default='core', help='A comma separated list of desired features for installing.'
                                                                       ' It defaults to installing just the core funtionality.')
    install_python_env.add_argument('--no-constraints', action='store_true', default=no_constraints_default,
                                    help='Disable constraint settings. Use with care and only when you want to manage '
                                         'package versions by yourself. It can be set with the IDF_PYTHON_CHECK_CONSTRAINTS '
                                         'environment variable.')

    if IDF_MAINTAINER:
        add_version = subparsers.add_parser('add-version', help='Add or update download info for a version')
        add_version.add_argument('--output', help='Save new tools.json into this file')
        add_version.add_argument('--tool', help='Tool name to set add a version for', required=True)
        add_version.add_argument('--version', help='Version identifier', required=True)
        add_version.add_argument('--url-prefix', help='String to prepend to file names to obtain download URLs')
        add_version.add_argument('--override', action='store_true', help='Override tool versions with new data')
        add_version_files_group = add_version.add_mutually_exclusive_group(required=True)
        add_version_files_group.add_argument('--checksum-file', help='URL or path to local file with checksum/size for artifacts')
        add_version_files_group.add_argument('--artifact-file', help='File names of the download artifacts', nargs='*')

        rewrite = subparsers.add_parser('rewrite', help='Load tools.json, validate, and save the result back into JSON')
        rewrite.add_argument('--output', help='Save new tools.json into this file')

        subparsers.add_parser('validate', help='Validate tools.json against schema file')

        gen_doc = subparsers.add_parser('gen-doc', help='Write the list of tools as a documentation page')
        gen_doc.add_argument('--output', type=argparse.FileType('w'), default=sys.stdout,
                             help='Output file name')
        gen_doc.add_argument('--heading-underline-char', help='Character to use when generating RST sections', default='~')

    check_python_dependencies = subparsers.add_parser('check-python-dependencies',
                                                      help='Check that all required Python packages are installed.')
    check_python_dependencies.add_argument('--no-constraints', action='store_true', default=no_constraints_default,
                                           help='Disable constraint settings. Use with care and only when you want '
                                                'to manage package versions by yourself. It can be set with the IDF_PYTHON_CHECK_CONSTRAINTS '
                                                'environment variable.')

    if os.environ.get('IDF_TOOLS_VERSION_HELPER'):
        check_tool_supported = subparsers.add_parser('check-tool-supported',
                                                     help='Check that selected tool is compatible with IDF. Writes "True"/"False" to stdout in success.')
        check_tool_supported.add_argument('--tool-name', required=True, help='Tool name (from tools.json)')
        check_tool_supported.add_argument('--exec-path', required=True, help='Full path to executable under the test')

        get_tool_supported_versions = subparsers.add_parser('get-tool-supported-versions', help='Prints a list of tool\'s supported versions')
        get_tool_supported_versions.add_argument('--tool-name', required=True,  help='Tool name (from tools.json)')

    args = parser.parse_args(argv)

    if args.action is None:
        parser.print_help()
        parser.exit(1)

    if args.quiet:
        global global_quiet
        global_quiet = True

    if args.non_interactive:
        global global_non_interactive
        global_non_interactive = True

    if 'unset' in args and args.unset:
        args.deactivate = True

    global global_idf_path
    global_idf_path = os.environ.get('IDF_PATH')
    if args.idf_path:
        global_idf_path = args.idf_path
    if not global_idf_path:
        global_idf_path = os.path.realpath(os.path.join(os.path.dirname(__file__), '..'))
    os.environ['IDF_PATH'] = global_idf_path

    global global_idf_tools_path
    global_idf_tools_path = os.environ.get('IDF_TOOLS_PATH') or os.path.expanduser(IDF_TOOLS_PATH_DEFAULT)

    # On macOS, unset __PYVENV_LAUNCHER__ variable if it is set.
    # Otherwise sys.executable keeps pointing to the system Python, even when a python binary from a virtualenv is invoked.
    # See https://bugs.python.org/issue22490#msg283859.
    os.environ.pop('__PYVENV_LAUNCHER__', None)

    if sys.version_info.major == 2:
        try:
            global_idf_tools_path.decode('ascii')  # type: ignore
        except UnicodeDecodeError:
            fatal('IDF_TOOLS_PATH contains non-ASCII characters: {}'.format(global_idf_tools_path) +
                  '\nThis is not supported yet with Python 2. ' +
                  'Please set IDF_TOOLS_PATH to a directory with an ASCII name, or switch to Python 3.')
            raise SystemExit(1)

    if CURRENT_PLATFORM is None:
        fatal('Platform {} appears to be unsupported'.format(PYTHON_PLATFORM))
        raise SystemExit(1)

    global global_tools_json
    if args.tools_json:
        global_tools_json = args.tools_json
    else:
        global_tools_json = os.path.join(global_idf_path, TOOLS_FILE)

    action_func_name = 'action_' + args.action.replace('-', '_')
    action_func = globals()[action_func_name]

    action_func(args)


if __name__ == '__main__':
    if 'MSYSTEM' in os.environ:
        fatal('MSys/Mingw is not supported. Please follow the getting started guide of the documentation to set up '
              'a supported environment')
        raise SystemExit(1)
    main(sys.argv[1:])
