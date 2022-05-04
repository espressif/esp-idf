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
import time
from collections import OrderedDict, namedtuple
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

from typing import IO, Any, Callable, Optional, Tuple, Union  # noqa: F401
from urllib.error import ContentTooShortError
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


# Mappings from various other names these platforms are known as, to the identifiers above.
# This includes strings produced from "platform.system() + '-' + platform.machine()", see PYTHON_PLATFORM
# definition above.
# This list also includes various strings used in release archives of xtensa-esp32-elf-gcc, OpenOCD, etc.
PLATFORM_FROM_NAME = {
    # Windows
    PLATFORM_WIN32: PLATFORM_WIN32,
    'Windows-i686': PLATFORM_WIN32,
    'Windows-x86': PLATFORM_WIN32,
    PLATFORM_WIN64: PLATFORM_WIN64,
    'Windows-x86_64': PLATFORM_WIN64,
    'Windows-AMD64': PLATFORM_WIN64,
    # macOS
    PLATFORM_MACOS: PLATFORM_MACOS,
    'osx': PLATFORM_MACOS,
    'darwin': PLATFORM_MACOS,
    'Darwin-x86_64': PLATFORM_MACOS,
    PLATFORM_MACOS_ARM64: PLATFORM_MACOS_ARM64,
    'Darwin-arm64': PLATFORM_MACOS_ARM64,
    # Linux
    PLATFORM_LINUX64: PLATFORM_LINUX64,
    'linux64': PLATFORM_LINUX64,
    'Linux-x86_64': PLATFORM_LINUX64,
    'FreeBSD-amd64': PLATFORM_LINUX64,
    PLATFORM_LINUX32: PLATFORM_LINUX32,
    'linux32': PLATFORM_LINUX32,
    'Linux-i686': PLATFORM_LINUX32,
    'FreeBSD-i386': PLATFORM_LINUX32,
    PLATFORM_LINUX_ARM32: PLATFORM_LINUX_ARM32,
    'Linux-arm': PLATFORM_LINUX_ARM32,
    'Linux-armv7l': PLATFORM_LINUX_ARM32,
    PLATFORM_LINUX_ARMHF: PLATFORM_LINUX_ARMHF,
    PLATFORM_LINUX_ARM64: PLATFORM_LINUX_ARM64,
    'Linux-arm64': PLATFORM_LINUX_ARM64,
    'Linux-aarch64': PLATFORM_LINUX_ARM64,
    'Linux-armv8l': PLATFORM_LINUX_ARM64,
}

UNKNOWN_PLATFORM = 'unknown'
CURRENT_PLATFORM = PLATFORM_FROM_NAME.get(PYTHON_PLATFORM, UNKNOWN_PLATFORM)

EXPORT_SHELL = 'shell'
EXPORT_KEY_VALUE = 'key-value'

ISRG_X1_ROOT_CERT = u"""
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
"""


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


def run_cmd_check_output(cmd, input_text=None, extra_paths=None):
    # type: (list[str], Optional[str], Optional[list[str]]) -> bytes
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


def to_shell_specific_paths(paths_list):  # type: (list[str]) -> list[str]
    if sys.platform == 'win32':
        paths_list = [p.replace('/', os.path.sep) if os.path.sep in p else p for p in paths_list]

        if 'MSYSTEM' in os.environ:
            paths_msys = run_cmd_check_output(['cygpath', '-u', '-f', '-'],
                                              input_text='\n'.join(paths_list))
            paths_list = paths_msys.decode().strip().split('\n')

    return paths_list


def get_env_for_extra_paths(extra_paths):  # type: (list[str]) -> dict[str, str]
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
            msg = 'Rename {} to {} failed'.format(path_from, path_to)
            if retry == retry_count - 1:
                fatal(msg + '. Antivirus software might be causing this. Disabling it temporarily could solve the issue.')
                raise
            warn(msg + ', retrying...')
            # Sleep before the next try in order to pass the antivirus check on Windows
            time.sleep(0.5)


def strip_container_dirs(path, levels):  # type: (str, int) -> None
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


class DownloadError(RuntimeError):
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

    def get_download_for_platform(self, platform_name):  # type: (str) -> Optional[IDFToolDownload]
        if platform_name in PLATFORM_FROM_NAME.keys():
            platform_name = PLATFORM_FROM_NAME[platform_name]
        if platform_name in self.downloads.keys():
            return self.downloads[platform_name]
        if 'any' in self.downloads.keys():
            return self.downloads['any']
        return None

    def compatible_with_platform(self, platform_name=PYTHON_PLATFORM):
        # type: (str) -> bool
        return self.get_download_for_platform(platform_name) is not None

    def get_supported_platforms(self):  # type: () -> set[str]
        return set(self.downloads.keys())


OPTIONS_LIST = ['version_cmd',
                'version_regex',
                'version_regex_replace',
                'export_paths',
                'export_vars',
                'install',
                'info_url',
                'license',
                'strip_container_dirs',
                'supported_targets']

IDFToolOptions = namedtuple('IDFToolOptions', OPTIONS_LIST)  # type: ignore


class IDFTool(object):
    # possible values of 'install' field
    INSTALL_ALWAYS = 'always'
    INSTALL_ON_REQUEST = 'on_request'
    INSTALL_NEVER = 'never'

    def __init__(self, name, description, install, info_url, license, version_cmd, version_regex, supported_targets, version_regex_replace=None,
                 strip_container_dirs=0):
        # type: (str, str, str, str, str, list[str], str, list[str], Optional[str], int) -> None
        self.name = name
        self.description = description
        self.versions = OrderedDict()  # type: dict[str, IDFToolVersion]
        self.version_in_path = None  # type: Optional[str]
        self.versions_installed = []  # type: list[str]
        if version_regex_replace is None:
            version_regex_replace = VERSION_REGEX_REPLACE_DEFAULT
        self.options = IDFToolOptions(version_cmd, version_regex, version_regex_replace,
                                      [], OrderedDict(), install, info_url, license, strip_container_dirs, supported_targets)  # type: ignore
        self.platform_overrides = []  # type: list[dict[str, str]]
        self._platform = CURRENT_PLATFORM
        self._update_current_options()

    def copy_for_platform(self, platform):  # type: (str) -> IDFTool
        result = copy.deepcopy(self)
        result._platform = platform
        result._update_current_options()
        return result

    def _update_current_options(self):  # type: () -> None
        self._current_options = IDFToolOptions(*self.options)
        for override in self.platform_overrides:
            if self._platform not in override['platforms']:
                continue
            override_dict = override.copy()
            del override_dict['platforms']
            self._current_options = self._current_options._replace(**override_dict)  # type: ignore

    def add_version(self, version):  # type: (IDFToolVersion) -> None
        assert(type(version) is IDFToolVersion)
        self.versions[version.version] = version

    def get_path(self):  # type: () -> str
        return os.path.join(global_idf_tools_path, 'tools', self.name)  # type: ignore

    def get_path_for_version(self, version):  # type: (str) -> str
        assert(version in self.versions)
        return os.path.join(self.get_path(), version)

    def get_export_paths(self, version):  # type: (str) -> list[str]
        tool_path = self.get_path_for_version(version)
        return [os.path.join(tool_path, *p) for p in self._current_options.export_paths]  # type: ignore

    def get_export_vars(self, version):  # type: (str) -> dict[str, str]
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
            result[k] = v_repl
        return result

    def check_version(self, extra_paths=None):  # type: (Optional[list[str]]) -> str
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

    def get_install_type(self):  # type: () -> Callable[[str], None]
        return self._current_options.install  # type: ignore

    def get_supported_targets(self):  # type: ()  -> list[str]
        return self._current_options.supported_targets  # type: ignore

    def compatible_with_platform(self):  # type: () -> bool
        return any([v.compatible_with_platform() for v in self.versions.values()])

    def get_supported_platforms(self):  # type: () -> set[str]
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
            ver_str = self.check_version()
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
            try:
                ver_str = self.check_version(self.get_export_paths(version))
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

    def download(self, version):  # type: (str) -> None
        assert(version in self.versions)
        download_obj = self.versions[version].get_download_for_platform(self._platform)
        if not download_obj:
            fatal('No packages for tool {} platform {}!'.format(self.name, self._platform))
            raise DownloadError()

        url = download_obj.url
        archive_name = os.path.basename(url)
        local_path = os.path.join(global_idf_tools_path, 'dist', archive_name)  # type: ignore
        mkdir_p(os.path.dirname(local_path))

        if os.path.isfile(local_path):
            if not self.check_download_file(download_obj, local_path):
                warn('removing downloaded file {0} and downloading again'.format(archive_name))
                os.unlink(local_path)
            else:
                info('file {0} is already downloaded'.format(archive_name))
                return

        downloaded = False
        for retry in range(DOWNLOAD_RETRY_COUNT):
            local_temp_path = local_path + '.tmp'
            info('Downloading {} to {}'.format(archive_name, local_temp_path))
            try:
                ctx = None
                # For dl.espressif.com, add the ISRG x1 root certificate.
                # This works around the issue with outdated certificate stores in some installations.
                if 'dl.espressif.com' in url:
                    try:
                        ctx = ssl.create_default_context()
                        ctx.load_verify_locations(cadata=ISRG_X1_ROOT_CERT)
                    except AttributeError:
                        # no ssl.create_default_context or load_verify_locations cadata argument
                        # in Python <=2.7.8
                        pass

                urlretrieve_ctx(url, local_temp_path, report_progress if not global_non_interactive else None, context=ctx)
                sys.stdout.write('\rDone\n')
            except Exception as e:
                # urlretrieve could throw different exceptions, e.g. IOError when the server is down
                # Errors are ignored because the downloaded file is checked a couple of lines later.
                warn('Download failure {}'.format(e))
            sys.stdout.flush()
            if not os.path.isfile(local_temp_path) or not self.check_download_file(download_obj, local_temp_path):
                warn('Failed to download {} to {}'.format(url, local_temp_path))
                continue
            rename_with_retry(local_temp_path, local_path)
            downloaded = True
            break
        if not downloaded:
            fatal('Failed to download, and retry count has expired')
            raise DownloadError()

    def install(self, version):  # type: (str) -> None
        # Currently this is called after calling 'download' method, so here are a few asserts
        # for the conditions which should be true once that method is done.
        assert (version in self.versions)
        download_obj = self.versions[version].get_download_for_platform(self._platform)
        assert (download_obj is not None)
        archive_name = os.path.basename(download_obj.url)
        archive_path = os.path.join(global_idf_tools_path, 'dist', archive_name)  # type: ignore
        assert (os.path.isfile(archive_path))
        dest_dir = self.get_path_for_version(version)
        if os.path.exists(dest_dir):
            warn('destination path already exists, removing')
            shutil.rmtree(dest_dir)
        mkdir_p(dest_dir)
        unpack(archive_path, dest_dir)
        if self._current_options.strip_container_dirs:  # type: ignore
            strip_container_dirs(dest_dir, self._current_options.strip_container_dirs)  # type: ignore

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
    def from_json(cls, tool_dict):  # type: (dict[str, Union[str, list[str], dict[str, str]]]) -> IDFTool
        # json.load will return 'str' types in Python 3 and 'unicode' in Python 2
        expected_str_type = type(u'')

        # Validate json fields
        tool_name = tool_dict.get('name')  # type: ignore
        if type(tool_name) is not expected_str_type:
            raise RuntimeError('tool_name is not a string')

        description = tool_dict.get('description')  # type: ignore
        if type(description) is not expected_str_type:
            raise RuntimeError('description is not a string')

        version_cmd = tool_dict.get('version_cmd')
        if type(version_cmd) is not list:
            raise RuntimeError('version_cmd for tool %s is not a list of strings' % tool_name)

        version_regex = tool_dict.get('version_regex')
        if type(version_regex) is not expected_str_type or not version_regex:
            raise RuntimeError('version_regex for tool %s is not a non-empty string' % tool_name)

        version_regex_replace = tool_dict.get('version_regex_replace')
        if version_regex_replace and type(version_regex_replace) is not expected_str_type:
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
        if type(install) is not expected_str_type:
            raise RuntimeError('install for tool %s is not a string' % tool_name)

        info_url = tool_dict.get('info_url', False)  # type: ignore
        if type(info_url) is not expected_str_type:
            raise RuntimeError('info_url for tool %s is not a string' % tool_name)

        license = tool_dict.get('license', False)  # type: ignore
        if type(license) is not expected_str_type:
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
                       strip_container_dirs)  # type: ignore

        for path in export_paths:  # type: ignore
            tool_obj.options.export_paths.append(path)  # type: ignore

        for name, value in export_vars.items():  # type: ignore
            tool_obj.options.export_vars[name] = value  # type: ignore

        for index, override in enumerate(overrides_list):
            platforms_list = override.get('platforms')  # type: ignore
            if type(platforms_list) is not list:
                raise RuntimeError('platforms for override %d of tool %s is not a list' % (index, tool_name))

            install = override.get('install')  # type: ignore
            if install is not None and type(install) is not expected_str_type:
                raise RuntimeError('install for override %d of tool %s is not a string' % (index, tool_name))

            version_cmd = override.get('version_cmd')  # type: ignore
            if version_cmd is not None and type(version_cmd) is not list:
                raise RuntimeError('version_cmd for override %d of tool %s is not a list of strings' %
                                   (index, tool_name))

            version_regex = override.get('version_regex')  # type: ignore
            if version_regex is not None and (type(version_regex) is not expected_str_type or not version_regex):
                raise RuntimeError('version_regex for override %d of tool %s is not a non-empty string' %
                                   (index, tool_name))

            version_regex_replace = override.get('version_regex_replace')  # type: ignore
            if version_regex_replace is not None and type(version_regex_replace) is not expected_str_type:
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
            if type(version) is not expected_str_type:
                raise RuntimeError('version name for tool {} is not a string'.format(tool_name))

            version_status = version_dict.get('status')  # type: ignore
            if type(version_status) is not expected_str_type and version_status not in IDFToolVersion.STATUS_VALUES:
                raise RuntimeError('tool {} version {} status is not one of {}', tool_name, version,
                                   IDFToolVersion.STATUS_VALUES)

            version_obj = IDFToolVersion(version, version_status)
            for platform_id, platform_dict in version_dict.items():  # type: ignore
                if platform_id in ['name', 'status']:
                    continue
                if platform_id not in PLATFORM_FROM_NAME.keys():
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
        return tool_json


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


def get_python_env_path():  # type: () -> Tuple[str, str, str]
    python_ver_major_minor = '{}.{}'.format(sys.version_info.major, sys.version_info.minor)

    version_file_path = os.path.join(global_idf_path, 'version.txt')  # type: ignore
    if os.path.exists(version_file_path):
        with open(version_file_path, 'r') as version_file:
            idf_version_str = version_file.read()
    else:
        idf_version_str = ''
        try:
            idf_version_str = subprocess.check_output(['git', 'describe'],
                                                      cwd=global_idf_path, env=os.environ).decode()
        except OSError:
            # OSError should cover FileNotFoundError and WindowsError
            warn('Git was not found')
        except subprocess.CalledProcessError as e:
            warn('Git describe was unsuccessful: {}'.format(e.output))
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

    idf_python_env_path = os.path.join(global_idf_tools_path, 'python_env',  # type: ignore
                                       'idf{}_py{}_env'.format(idf_version, python_ver_major_minor))

    if sys.platform == 'win32':
        subdir = 'Scripts'
        python_exe = 'python.exe'
    else:
        subdir = 'bin'
        python_exe = 'python'

    idf_python_export_path = os.path.join(idf_python_env_path, subdir)
    virtualenv_python = os.path.join(idf_python_export_path, python_exe)

    return idf_python_env_path, idf_python_export_path, virtualenv_python


def get_idf_env():  # type: () -> Any
    try:
        idf_env_file_path = os.path.join(global_idf_tools_path, IDF_ENV_FILE)  # type: ignore
        with open(idf_env_file_path, 'r') as idf_env_file:
            return json.load(idf_env_file)
    except (IOError, OSError):
        if not os.path.exists(idf_env_file_path):
            warn('File {} was not found. '.format(idf_env_file_path))
        else:
            filename, ending = os.path.splitext(os.path.basename(idf_env_file_path))
            warn('File {} can not be opened, renaming to {}'.format(idf_env_file_path,filename + '_failed' + ending))
            os.rename(idf_env_file_path, os.path.join(os.path.dirname(idf_env_file_path), (filename + '_failed' + ending)))

        info('Creating {}' .format(idf_env_file_path))
        return {'idfSelectedId': 'sha', 'idfInstalled': {'sha': {'targets': {}}}}


def export_targets_to_idf_env_json(targets):  # type: (list[str]) -> None
    idf_env_json = get_idf_env()
    targets = list(set(targets + get_user_defined_targets()))

    for env in idf_env_json['idfInstalled']:
        if env == idf_env_json['idfSelectedId']:
            idf_env_json['idfInstalled'][env]['targets'] = targets
            break

    try:
        if global_idf_tools_path:  # mypy fix for Optional[str] in the next call
            # the directory doesn't exist if this is run on a clean system the first time
            mkdir_p(global_idf_tools_path)
        with open(os.path.join(global_idf_tools_path, IDF_ENV_FILE), 'w') as w:  # type: ignore
            json.dump(idf_env_json, w, indent=4)
    except (IOError, OSError):
        warn('File {} can not be created. '.format(os.path.join(global_idf_tools_path, IDF_ENV_FILE)))  # type: ignore


def clean_targets(targets_str):  # type: (str) -> list[str]
    targets_from_tools_json = get_all_targets_from_tools_json()
    invalid_targets = []

    targets_str = targets_str.lower()
    targets = targets_str.replace('-', '').split(',')
    if targets != ['all']:
        invalid_targets = [t for t in targets if t not in targets_from_tools_json]
        if invalid_targets:
            warn('Targets: "{}" are not supported. Only allowed options are: {}.'.format(', '.join(invalid_targets), ', '.join(targets_from_tools_json)))
            raise SystemExit(1)
        # removing duplicates
        targets = list(set(targets))
        export_targets_to_idf_env_json(targets)
    else:
        export_targets_to_idf_env_json(targets_from_tools_json)
    return targets


def get_user_defined_targets():  # type: () -> list[str]
    try:
        with open(os.path.join(global_idf_tools_path, IDF_ENV_FILE), 'r') as idf_env_file:  # type: ignore
            idf_env_json = json.load(idf_env_file)
    except OSError:
        # warn('File {} was not found. Installing tools for all esp targets.'.format(os.path.join(global_idf_tools_path, IDF_ENV_FILE)))  # type: ignore
        return []

    targets = []
    for env in idf_env_json['idfInstalled']:
        if env == idf_env_json['idfSelectedId']:
            targets = idf_env_json['idfInstalled'][env]['targets']
            break
    return targets


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


def filter_tools_info(tools_info):  # type: (OrderedDict[str, IDFTool]) -> OrderedDict[str,IDFTool]
    targets = get_user_defined_targets()
    if not targets:
        return tools_info
    else:
        filtered_tools_spec = {k:v for k, v in tools_info.items() if
                               (v.get_install_type() == IDFTool.INSTALL_ALWAYS or v.get_install_type() == IDFTool.INSTALL_ON_REQUEST) and
                               (any(item in targets for item in v.get_supported_targets()) or v.get_supported_targets() == ['all'])}
        return OrderedDict(filtered_tools_spec)


def action_list(args):  # type: ignore
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


def action_check(args):  # type: ignore
    tools_info = load_tools_info()
    tools_info = filter_tools_info(tools_info)
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
    tools_info = load_tools_info()
    tools_info = filter_tools_info(tools_info)
    all_tools_found = True
    export_vars = {}
    paths_to_export = []
    for name, tool in tools_info.items():
        if tool.get_install_type() == IDFTool.INSTALL_NEVER:
            continue
        tool.find_installed_versions()

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

        self_restart_cmd = '{} {}{}'.format(sys.executable, __file__,
                                            (' --tools-json ' + args.tools_json) if args.tools_json else '')
        self_restart_cmd = to_shell_specific_paths([self_restart_cmd])[0]

        if IDF_TOOLS_EXPORT_CMD:
            prefer_system_hint = ''
        else:
            prefer_system_hint = ' To use it, run \'{} export --prefer-system\''.format(self_restart_cmd)

        if IDF_TOOLS_INSTALL_CMD:
            install_cmd = to_shell_specific_paths([IDF_TOOLS_INSTALL_CMD])[0]
        else:
            install_cmd = self_restart_cmd + ' install'

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

        version_to_use = tool.get_preferred_installed_version()
        export_paths = tool.get_export_paths(version_to_use)
        if export_paths:
            paths_to_export += export_paths
        tool_export_vars = tool.get_export_vars(version_to_use)
        for k, v in tool_export_vars.items():
            old_v = os.environ.get(k)
            if old_v is None or old_v != v:
                export_vars[k] = v

    current_path = os.getenv('PATH')
    idf_python_env_path, idf_python_export_path, virtualenv_python = get_python_env_path()
    if os.path.exists(virtualenv_python):
        idf_python_env_path = to_shell_specific_paths([idf_python_env_path])[0]
        if os.getenv('IDF_PYTHON_ENV_PATH') != idf_python_env_path:
            export_vars['IDF_PYTHON_ENV_PATH'] = to_shell_specific_paths([idf_python_env_path])[0]
        if idf_python_export_path not in current_path:
            paths_to_export.append(idf_python_export_path)

    idf_tools_dir = os.path.join(global_idf_path, 'tools')
    idf_tools_dir = to_shell_specific_paths([idf_tools_dir])[0]
    if idf_tools_dir not in current_path:
        paths_to_export.append(idf_tools_dir)

    if sys.platform == 'win32' and 'MSYSTEM' not in os.environ:
        old_path = '%PATH%'
        path_sep = ';'
    else:
        old_path = '$PATH'
        # can't trust os.pathsep here, since for Windows Python started from MSYS shell,
        # os.pathsep will be ';'
        path_sep = ':'

    if args.format == EXPORT_SHELL:
        if sys.platform == 'win32' and 'MSYSTEM' not in os.environ:
            export_format = 'SET "{}={}"'
            export_sep = '\n'
        else:
            export_format = 'export {}="{}"'
            export_sep = ';'
    elif args.format == EXPORT_KEY_VALUE:
        export_format = '{}={}'
        export_sep = '\n'
    else:
        raise NotImplementedError('unsupported export format {}'.format(args.format))

    if paths_to_export:
        export_vars['PATH'] = path_sep.join(to_shell_specific_paths(paths_to_export) + [old_path])

    export_statements = export_sep.join([export_format.format(k, v) for k, v in export_vars.items()])

    if export_statements:
        print(export_statements)

    if not all_tools_found:
        raise SystemExit(1)


def apply_url_mirrors(args, tool_download_obj):  # type: ignore
    apply_mirror_prefix_map(args, tool_download_obj)
    apply_github_assets_option(tool_download_obj)


def apply_mirror_prefix_map(args, tool_download_obj):  # type: ignore
    """Rewrite URL for given tool_obj, given tool_version, and current platform,
       if --mirror-prefix-map flag or IDF_MIRROR_PREFIX_MAP environment variable is given.
    """
    mirror_prefix_map = None
    mirror_prefix_map_env = os.getenv('IDF_MIRROR_PREFIX_MAP')
    if mirror_prefix_map_env:
        mirror_prefix_map = mirror_prefix_map_env.split(';')
    if IDF_MAINTAINER and args.mirror_prefix_map:
        if mirror_prefix_map:
            warn('Both IDF_MIRROR_PREFIX_MAP environment variable and --mirror-prefix-map flag are specified, ' +
                 'will use the value from the command line.')
        mirror_prefix_map = args.mirror_prefix_map
    if mirror_prefix_map and tool_download_obj:
        for item in mirror_prefix_map:
            if URL_PREFIX_MAP_SEPARATOR not in item:
                warn('invalid mirror-prefix-map item (missing \'{}\') {}'.format(URL_PREFIX_MAP_SEPARATOR, item))
                continue
            search, replace = item.split(URL_PREFIX_MAP_SEPARATOR, 1)
            old_url = tool_download_obj.url
            new_url = re.sub(search, replace, old_url)
            if new_url != old_url:
                info('Changed download URL: {} => {}'.format(old_url, new_url))
                tool_download_obj.url = new_url
                break


def apply_github_assets_option(tool_download_obj):  # type: ignore
    """ Rewrite URL for given tool_obj if the download URL is an https://github.com/ URL and the variable
    IDF_GITHUB_ASSETS is set. The github.com part of the URL will be replaced.
    """
    try:
        github_assets = os.environ['IDF_GITHUB_ASSETS'].strip()
    except KeyError:
        return  # no IDF_GITHUB_ASSETS
    if not github_assets:  # variable exists but is empty
        return

    # check no URL qualifier in the mirror URL
    if '://' in github_assets:
        fatal("IDF_GITHUB_ASSETS shouldn't include any URL qualifier, https:// is assumed")
        raise SystemExit(1)

    # Strip any trailing / from the mirror URL
    github_assets = github_assets.rstrip('/')

    old_url = tool_download_obj.url
    new_url = re.sub(r'^https://github.com/', 'https://{}/'.format(github_assets), old_url)
    if new_url != old_url:
        info('Using GitHub assets mirror for URL: {} => {}'.format(old_url, new_url))
        tool_download_obj.url = new_url


def action_download(args):  # type: ignore
    tools_info = load_tools_info()
    tools_spec = args.tools
    targets = []  # type: list[str]
    # Installing only single tools, no targets are specified.
    if 'required' in tools_spec:
        targets = clean_targets(args.targets)

    if args.platform not in PLATFORM_FROM_NAME:
        fatal('unknown platform: {}' % args.platform)
        raise SystemExit(1)
    platform = PLATFORM_FROM_NAME[args.platform]

    tools_info_for_platform = OrderedDict()
    for name, tool_obj in tools_info.items():
        tool_for_platform = tool_obj.copy_for_platform(platform)
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
        info('Downloading tools for {}: {}'.format(platform, ', '.join(tools_spec)))

    # Downloading tools for all ESP_targets (MacOS, Windows, Linux)
    elif 'all' in tools_spec:
        tools_spec = [k for k, v in tools_info_for_platform.items() if v.get_install_type() != IDFTool.INSTALL_NEVER]
        info('Downloading tools for {}: {}'.format(platform, ', '.join(tools_spec)))

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
        apply_url_mirrors(args, tool_obj.versions[tool_version].get_download_for_platform(platform))

        tool_obj.download(tool_version)


def action_install(args):  # type: ignore
    tools_info = load_tools_info()
    tools_spec = args.tools  # type: ignore
    targets = []  # type: list[str]
    info('Current system platform: {}'.format(CURRENT_PLATFORM))
    # Installing only single tools, no targets are specified.
    if 'required' in tools_spec:
        targets = clean_targets(args.targets)
        info('Selected targets are: {}'.format(', '.join(get_user_defined_targets())))

    if not tools_spec or 'required' in tools_spec:
        # Installing tools for all ESP_targets required by the operating system.
        tools_spec = [k for k, v in tools_info.items() if v.get_install_type() == IDFTool.INSTALL_ALWAYS]
        # Filtering tools user defined list of ESP_targets
        if 'all' not in targets:
            def is_tool_selected(tool):  # type: (IDFTool) -> bool
                supported_targets = tool.get_supported_targets()
                return (any(item in targets for item in supported_targets) or supported_targets == ['all'])
            tools_spec = [k for k in tools_spec if is_tool_selected(tools_info[k])]
        info('Installing tools: {}'.format(', '.join(tools_spec)))

    # Installing tools for all ESP_targets (MacOS, Windows, Linux)
    elif 'all' in tools_spec:
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
        apply_url_mirrors(args, tool_obj.versions[tool_version].get_download_for_platform(PYTHON_PLATFORM))

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


def action_install_python_env(args):  # type: ignore
    reinstall = args.reinstall
    idf_python_env_path, _, virtualenv_python = get_python_env_path()

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

    if not os.path.exists(virtualenv_python):
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

        info('Creating a new Python environment in {}'.format(idf_python_env_path))
        virtualenv_options = ['--python', sys.executable]
        if with_seeder_option:
            virtualenv_options += ['--seeder', 'pip']

        subprocess.check_call([sys.executable, '-m', 'virtualenv',
                               *virtualenv_options,
                               idf_python_env_path],
                              stdout=sys.stdout, stderr=sys.stderr)
    env_copy = os.environ.copy()
    if env_copy.get('PIP_USER')  == 'yes':
        warn('Found PIP_USER="yes" in the environment. Disabling PIP_USER in this shell to install packages into a virtual environment.')
        env_copy['PIP_USER'] = 'no'
    run_args = [virtualenv_python, '-m', 'pip', 'install', '--no-warn-script-location']
    requirements_txt = os.path.join(global_idf_path, 'requirements.txt')
    run_args += ['-r', requirements_txt]
    if args.extra_wheels_dir:
        run_args += ['--find-links', args.extra_wheels_dir]
    if args.no_index:
        run_args += ['--no-index']
    if args.extra_wheels_url:
        run_args += ['--extra-index-url', args.extra_wheels_url]

    wheels_dir = get_wheels_dir()
    if wheels_dir is not None:
        run_args += ['--find-links', wheels_dir]

    info('Installing Python packages from {}'.format(requirements_txt))
    subprocess.check_call(run_args, stdout=sys.stdout, stderr=sys.stderr, env=env_copy)


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
    version_obj = tool_obj.versions.get(version)
    if version not in tool_obj.versions:
        info('Creating new version {}'.format(version))
        version_obj = IDFToolVersion(version, IDFToolVersion.STATUS_SUPPORTED)
        tool_obj.versions[version] = version_obj
    url_prefix = args.url_prefix or 'https://%s/' % TODO_MESSAGE
    for file_path in args.files:
        file_name = os.path.basename(file_path)
        # Guess which platform this file is for
        found_platform = None
        for platform_alias, platform_id in PLATFORM_FROM_NAME.items():
            if platform_alias in file_name:
                found_platform = platform_id
                break
        if found_platform is None:
            info('Could not guess platform for file {}'.format(file_name))
            found_platform = TODO_MESSAGE
        # Get file size and calculate the SHA256
        file_size, file_sha256 = get_file_size_sha256(file_path)
        url = url_prefix + file_name
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


def main(argv):  # type: (list[str]) -> None
    parser = argparse.ArgumentParser()

    parser.add_argument('--quiet', help='Don\'t output diagnostic messages to stdout/stderr', action='store_true')
    parser.add_argument('--non-interactive', help='Don\'t output interactive messages and questions', action='store_true')
    parser.add_argument('--tools-json', help='Path to the tools.json file to use')
    parser.add_argument('--idf-path', help='ESP-IDF path to use')

    subparsers = parser.add_subparsers(dest='action')
    subparsers.add_parser('list', help='List tools and versions available')
    subparsers.add_parser('check', help='Print summary of tools installed or found in PATH')
    export = subparsers.add_parser('export', help='Output command for setting tool paths, suitable for shell')
    export.add_argument('--format', choices=[EXPORT_SHELL, EXPORT_KEY_VALUE], default=EXPORT_SHELL,
                        help='Format of the output: shell (suitable for printing into shell), ' +
                             'or key-value (suitable for parsing by other tools')
    export.add_argument('--prefer-system', help='Normally, if the tool is already present in PATH, ' +
                                                'but has an unsupported version, a version from the tools directory ' +
                                                'will be used instead. If this flag is given, the version in PATH ' +
                                                'will be used.', action='store_true')
    install = subparsers.add_parser('install', help='Download and install tools into the tools directory')
    install.add_argument('tools', metavar='TOOL', nargs='*', default=['required'],
                         help='Tools to install. ' +
                         'To install a specific version use <tool_name>@<version> syntax. ' +
                         'Use empty or \'required\' to install required tools, not optional ones. ' +
                         'Use \'all\' to install all tools, including the optional ones.')
    install.add_argument('--targets', default='all', help='A comma separated list of desired chip targets for installing.' +
                         ' It defaults to installing all supported targets.')

    download = subparsers.add_parser('download', help='Download the tools into the dist directory')
    download.add_argument('--platform', help='Platform to download the tools for')
    download.add_argument('tools', metavar='TOOL', nargs='*', default=['required'],
                          help='Tools to download. ' +
                          'To download a specific version use <tool_name>@<version> syntax. ' +
                          'Use empty or \'required\' to download required tools, not optional ones. ' +
                          'Use \'all\' to download all tools, including the optional ones.')
    download.add_argument('--targets', default='all', help='A comma separated list of desired chip targets for installing.' +
                          ' It defaults to installing all supported targets.')

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
    install_python_env.add_argument('--extra-wheels-url', help='Additional URL with wheels', default='https://dl.espressif.com/pypi')
    install_python_env.add_argument('--no-index', help='Work offline without retrieving wheels index')

    if IDF_MAINTAINER:
        add_version = subparsers.add_parser('add-version', help='Add or update download info for a version')
        add_version.add_argument('--output', help='Save new tools.json into this file')
        add_version.add_argument('--tool', help='Tool name to set add a version for', required=True)
        add_version.add_argument('--version', help='Version identifier', required=True)
        add_version.add_argument('--url-prefix', help='String to prepend to file names to obtain download URLs')
        add_version.add_argument('files', help='File names of the download artifacts', nargs='*')

        rewrite = subparsers.add_parser('rewrite', help='Load tools.json, validate, and save the result back into JSON')
        rewrite.add_argument('--output', help='Save new tools.json into this file')

        subparsers.add_parser('validate', help='Validate tools.json against schema file')

        gen_doc = subparsers.add_parser('gen-doc', help='Write the list of tools as a documentation page')
        gen_doc.add_argument('--output', type=argparse.FileType('w'), default=sys.stdout,
                             help='Output file name')
        gen_doc.add_argument('--heading-underline-char', help='Character to use when generating RST sections', default='~')

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

    if CURRENT_PLATFORM == UNKNOWN_PLATFORM:
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
    main(sys.argv[1:])
