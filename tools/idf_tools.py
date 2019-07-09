#!/usr/bin/env python
# coding=utf-8
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
#
###
#
# Copyright 2019 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import json
import os
import subprocess
import sys
import argparse
import re
import platform
import hashlib
import tarfile
import zipfile
import errno
import shutil
import functools
from collections import OrderedDict, namedtuple

try:
    from urllib.request import urlretrieve
except ImportError:
    from urllib import urlretrieve


TOOLS_FILE = 'tools/tools.json'
TOOLS_SCHEMA_FILE = 'tools/tools_schema.json'
TOOLS_FILE_NEW = 'tools/tools.new.json'
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
PLATFORM_LINUX32 = 'linux-i686'
PLATFORM_LINUX64 = 'linux-amd64'
PLATFORM_LINUX_ARM32 = 'linux-armel'
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
    # Linux
    PLATFORM_LINUX64: PLATFORM_LINUX64,
    'linux64': PLATFORM_LINUX64,
    'Linux-x86_64': PLATFORM_LINUX64,
    PLATFORM_LINUX32: PLATFORM_LINUX32,
    'linux32': PLATFORM_LINUX32,
    'Linux-i686': PLATFORM_LINUX32,
    PLATFORM_LINUX_ARM32: PLATFORM_LINUX_ARM32,
    'Linux-arm': PLATFORM_LINUX_ARM32,
    'Linux-armv7l': PLATFORM_LINUX_ARM32,
    PLATFORM_LINUX_ARM64: PLATFORM_LINUX_ARM64,
    'Linux-arm64': PLATFORM_LINUX_ARM64,
    'Linux-aarch64': PLATFORM_LINUX_ARM64,
    'Linux-armv8l': PLATFORM_LINUX_ARM64,
}

UNKNOWN_PLATFORM = 'unknown'
CURRENT_PLATFORM = PLATFORM_FROM_NAME.get(PYTHON_PLATFORM, UNKNOWN_PLATFORM)

EXPORT_SHELL = 'shell'
EXPORT_KEY_VALUE = 'key-value'


global_quiet = False
global_non_interactive = False
global_idf_path = None
global_idf_tools_path = None
global_tools_json = None


def fatal(text, *args):
    if not global_quiet:
        sys.stderr.write('ERROR: ' + text + '\n', *args)


def warn(text, *args):
    if not global_quiet:
        sys.stderr.write('WARNING: ' + text + '\n', *args)


def info(text, f=None, *args):
    if not global_quiet:
        if f is None:
            f = sys.stdout
        f.write(text + '\n', *args)


def run_cmd_check_output(cmd, input_text=None, extra_paths=None):
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
        if input_text:
            input_text = input_text.encode()
        result = subprocess.run(cmd, capture_output=True, check=True, input=input_text)
        return result.stdout + result.stderr
    except (AttributeError, TypeError):
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = p.communicate(input_text)
        if p.returncode != 0:
            try:
                raise subprocess.CalledProcessError(p.returncode, cmd, stdout, stderr)
            except TypeError:
                raise subprocess.CalledProcessError(p.returncode, cmd, stdout)
        return stdout + stderr


def to_shell_specific_paths(paths_list):
    if sys.platform == 'win32':
        paths_list = [p.replace('/', os.path.sep) if os.path.sep in p else p for p in paths_list]

        if 'MSYSTEM' in os.environ:
            paths_msys = run_cmd_check_output(['cygpath', '-u', '-f', '-'],
                                              input_text='\n'.join(paths_list))
            paths_list = paths_msys.decode().strip().split('\n')

    return paths_list


def get_env_for_extra_paths(extra_paths):
    """
    Return a copy of environment variables dict, prepending paths listed in extra_paths
    to the PATH environment variable.
    """
    env_arg = os.environ.copy()
    new_path = os.pathsep.join(extra_paths) + os.pathsep + env_arg['PATH']
    if sys.version_info.major == 2:
        env_arg['PATH'] = new_path.encode('utf8')
    else:
        env_arg['PATH'] = new_path
    return env_arg


def get_file_size_sha256(filename, block_size=65536):
    sha256 = hashlib.sha256()
    size = 0
    with open(filename, 'rb') as f:
        for block in iter(lambda: f.read(block_size), b''):
            sha256.update(block)
            size += len(block)
    return size, sha256.hexdigest()


def report_progress(count, block_size, total_size):
    percent = int(count * block_size * 100 / total_size)
    percent = min(100, percent)
    sys.stdout.write("\r%d%%" % percent)
    sys.stdout.flush()


def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc:
        if exc.errno != errno.EEXIST or not os.path.isdir(path):
            raise


def unpack(filename, destination):
    info('Extracting {0} to {1}'.format(filename, destination))
    if filename.endswith('tar.gz'):
        archive_obj = tarfile.open(filename, 'r:gz')
    elif filename.endswith('zip'):
        archive_obj = zipfile.ZipFile(filename)
    else:
        raise NotImplementedError('Unsupported archive type')
    if sys.version_info.major == 2:
        # This is a workaround for the issue that unicode destination is not handled:
        # https://bugs.python.org/issue17153
        destination = str(destination)
    archive_obj.extractall(destination)


def strip_container_dirs(path, levels):
    assert levels > 0
    # move the original directory out of the way (add a .tmp suffix)
    tmp_path = path + '.tmp'
    if os.path.exists(tmp_path):
        shutil.rmtree(tmp_path)
    os.rename(path, tmp_path)
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
        os.rename(move_from, move_to)
    shutil.rmtree(tmp_path)


class ToolNotFound(RuntimeError):
    pass


class ToolExecError(RuntimeError):
    pass


class DownloadError(RuntimeError):
    pass


class IDFToolDownload(object):
    def __init__(self, platform_name, url, size, sha256):
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

    def __init__(self, version, status):
        self.version = version
        self.status = status
        self.downloads = OrderedDict()
        self.latest = False

    def __lt__(self, other):
        if self.status != other.status:
            return self.status > other.status
        else:
            assert not (self.status == IDFToolVersion.STATUS_RECOMMENDED
                        and other.status == IDFToolVersion.STATUS_RECOMMENDED)
            return self.version < other.version

    def __eq__(self, other):
        return self.status == other.status and self.version == other.version

    def add_download(self, platform_name, url, size, sha256):
        self.downloads[platform_name] = IDFToolDownload(platform_name, url, size, sha256)

    def get_download_for_platform(self, platform_name):
        if platform_name in PLATFORM_FROM_NAME.keys():
            platform_name = PLATFORM_FROM_NAME[platform_name]
        if platform_name in self.downloads.keys():
            return self.downloads[platform_name]
        if 'any' in self.downloads.keys():
            return self.downloads['any']
        return None

    def compatible_with_platform(self, platform_name=PYTHON_PLATFORM):
        return self.get_download_for_platform(platform_name) is not None


OPTIONS_LIST = ['version_cmd',
                'version_regex',
                'version_regex_replace',
                'export_paths',
                'export_vars',
                'install',
                'info_url',
                'license',
                'strip_container_dirs']

IDFToolOptions = namedtuple('IDFToolOptions', OPTIONS_LIST)


class IDFTool(object):
    # possible values of 'install' field
    INSTALL_ALWAYS = 'always'
    INSTALL_ON_REQUEST = 'on_request'
    INSTALL_NEVER = 'never'

    def __init__(self, name, description, install, info_url, license, version_cmd, version_regex, version_regex_replace=None,
                 strip_container_dirs=0):
        self.name = name
        self.description = description
        self.versions = OrderedDict()
        self.version_in_path = None
        self.versions_installed = []
        if version_regex_replace is None:
            version_regex_replace = VERSION_REGEX_REPLACE_DEFAULT
        self.options = IDFToolOptions(version_cmd, version_regex, version_regex_replace,
                                      [], OrderedDict(), install, info_url, license, strip_container_dirs)
        self.platform_overrides = []
        self._update_current_options()

    def _update_current_options(self):
        self._current_options = IDFToolOptions(*self.options)
        for override in self.platform_overrides:
            if CURRENT_PLATFORM not in override['platforms']:
                continue
            override_dict = override.copy()
            del override_dict['platforms']
            self._current_options = self._current_options._replace(**override_dict)

    def add_version(self, version):
        assert(type(version) is IDFToolVersion)
        self.versions[version.version] = version

    def get_path(self):
        return os.path.join(global_idf_tools_path, 'tools', self.name)

    def get_path_for_version(self, version):
        assert(version in self.versions)
        return os.path.join(self.get_path(), version)

    def get_export_paths(self, version):
        tool_path = self.get_path_for_version(version)
        return [os.path.join(tool_path, *p) for p in self._current_options.export_paths]

    def get_export_vars(self, version):
        """
        Get the dictionary of environment variables to be exported, for the given version.
        Expands:
        - ${TOOL_PATH} => the actual path where the version is installed
        """
        result = {}
        for k, v in self._current_options.export_vars.items():
            replace_path = self.get_path_for_version(version).replace('\\', '\\\\')
            v_repl = re.sub(SUBST_TOOL_PATH_REGEX, replace_path, v)
            if v_repl != v:
                v_repl = to_shell_specific_paths([v_repl])[0]
            result[k] = v_repl
        return result

    def check_version(self, extra_paths=None):
        """
        Execute the tool, optionally prepending extra_paths to PATH,
        extract the version string and return it as a result.
        Raises ToolNotFound if the tool is not found (not present in the paths).
        Raises ToolExecError if the tool returns with a non-zero exit code.
        Returns 'unknown' if tool returns something from which version string
        can not be extracted.
        """
        cmd = self._current_options.version_cmd
        try:
            version_cmd_result = run_cmd_check_output(cmd, None, extra_paths)
        except OSError:
            # tool is not on the path
            raise ToolNotFound('Tool {} not found'.format(self.name))
        except subprocess.CalledProcessError as e:
            raise ToolExecError('Command {} has returned non-zero exit code ({})\n'.format(
                ' '.join(self._current_options.version_cmd), e.returncode))

        in_str = version_cmd_result.decode()
        match = re.search(self._current_options.version_regex, in_str)
        if not match:
            return UNKNOWN_VERSION
        return re.sub(self._current_options.version_regex, self._current_options.version_regex_replace, match.group(0))

    def get_install_type(self):
        return self._current_options.install

    def get_recommended_version(self):
        recommended_versions = [k for k, v in self.versions.items()
                                if v.status == IDFToolVersion.STATUS_RECOMMENDED
                                and v.compatible_with_platform()]
        assert len(recommended_versions) <= 1
        if recommended_versions:
            return recommended_versions[0]
        return None

    def get_preferred_installed_version(self):
        recommended_versions = [k for k in self.versions_installed
                                if self.versions[k].status == IDFToolVersion.STATUS_RECOMMENDED
                                and self.versions[k].compatible_with_platform()]
        assert len(recommended_versions) <= 1
        if recommended_versions:
            return recommended_versions[0]
        return None

    def find_installed_versions(self):
        """
        Checks whether the tool can be found in PATH and in global_idf_tools_path.
        Writes results to self.version_in_path and self.versions_installed.
        """
        # First check if the tool is in system PATH
        try:
            ver_str = self.check_version()
        except ToolNotFound:
            # not in PATH
            pass
        except ToolExecError:
            warn('tool {} found in path, but failed to run'.format(self.name))
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
            except ToolExecError:
                warn('tool {} version {} is installed, but the tool failed to run'.format(
                    self.name, version))
            else:
                if ver_str != version:
                    warn('tool {} version {} is installed, but has reported version {}'.format(
                        self.name, version, ver_str))
                else:
                    self.versions_installed.append(version)

    def download(self, version):
        assert(version in self.versions)
        download_obj = self.versions[version].get_download_for_platform(PYTHON_PLATFORM)
        if not download_obj:
            fatal('No packages for tool {} platform {}!'.format(self.name, PYTHON_PLATFORM))
            raise DownloadError()

        url = download_obj.url
        archive_name = os.path.basename(url)
        local_path = os.path.join(global_idf_tools_path, 'dist', archive_name)
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
            urlretrieve(url, local_temp_path, report_progress if not global_non_interactive else None)
            sys.stdout.write("\rDone\n")
            sys.stdout.flush()
            if not self.check_download_file(download_obj, local_temp_path):
                warn('Failed to download file {}'.format(local_temp_path))
                continue
            os.rename(local_temp_path, local_path)
            downloaded = True
            break
        if not downloaded:
            fatal('Failed to download, and retry count has expired')
            raise DownloadError()

    def install(self, version):
        # Currently this is called after calling 'download' method, so here are a few asserts
        # for the conditions which should be true once that method is done.
        assert (version in self.versions)
        download_obj = self.versions[version].get_download_for_platform(PYTHON_PLATFORM)
        assert (download_obj is not None)
        archive_name = os.path.basename(download_obj.url)
        archive_path = os.path.join(global_idf_tools_path, 'dist', archive_name)
        assert (os.path.isfile(archive_path))
        dest_dir = self.get_path_for_version(version)
        if os.path.exists(dest_dir):
            warn('destination path already exists, removing')
            shutil.rmtree(dest_dir)
        mkdir_p(dest_dir)
        unpack(archive_path, dest_dir)
        if self._current_options.strip_container_dirs:
            strip_container_dirs(dest_dir, self._current_options.strip_container_dirs)

    @staticmethod
    def check_download_file(download_obj, local_path):
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
    def from_json(cls, tool_dict):
        # json.load will return 'str' types in Python 3 and 'unicode' in Python 2
        expected_str_type = type(u'')

        # Validate json fields
        tool_name = tool_dict.get('name')
        if type(tool_name) is not expected_str_type:
            raise RuntimeError('tool_name is not a string')

        description = tool_dict.get('description')
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

        export_vars = tool_dict.get('export_vars', {})
        if type(export_vars) is not dict:
            raise RuntimeError('export_vars for tool %s is not a mapping' % tool_name)

        versions = tool_dict.get('versions')
        if type(versions) is not list:
            raise RuntimeError('versions for tool %s is not an array' % tool_name)

        install = tool_dict.get('install', False)
        if type(install) is not expected_str_type:
            raise RuntimeError('install for tool %s is not a string' % tool_name)

        info_url = tool_dict.get('info_url', False)
        if type(info_url) is not expected_str_type:
            raise RuntimeError('info_url for tool %s is not a string' % tool_name)

        license = tool_dict.get('license', False)
        if type(license) is not expected_str_type:
            raise RuntimeError('license for tool %s is not a string' % tool_name)

        strip_container_dirs = tool_dict.get('strip_container_dirs', 0)
        if strip_container_dirs and type(strip_container_dirs) is not int:
            raise RuntimeError('strip_container_dirs for tool %s is not an int' % tool_name)

        overrides_list = tool_dict.get('platform_overrides', [])
        if type(overrides_list) is not list:
            raise RuntimeError('platform_overrides for tool %s is not a list' % tool_name)

        # Create the object
        tool_obj = cls(tool_name, description, install, info_url, license,
                       version_cmd, version_regex, version_regex_replace,
                       strip_container_dirs)

        for path in export_paths:
            tool_obj.options.export_paths.append(path)

        for name, value in export_vars.items():
            tool_obj.options.export_vars[name] = value

        for index, override in enumerate(overrides_list):
            platforms_list = override.get('platforms')
            if type(platforms_list) is not list:
                raise RuntimeError('platforms for override %d of tool %s is not a list' % (index, tool_name))

            install = override.get('install')
            if install is not None and type(install) is not expected_str_type:
                raise RuntimeError('install for override %d of tool %s is not a string' % (index, tool_name))

            version_cmd = override.get('version_cmd')
            if version_cmd is not None and type(version_cmd) is not list:
                raise RuntimeError('version_cmd for override %d of tool %s is not a list of strings' %
                                   (index, tool_name))

            version_regex = override.get('version_regex')
            if version_regex is not None and (type(version_regex) is not expected_str_type or not version_regex):
                raise RuntimeError('version_regex for override %d of tool %s is not a non-empty string' %
                                   (index, tool_name))

            version_regex_replace = override.get('version_regex_replace')
            if version_regex_replace is not None and type(version_regex_replace) is not expected_str_type:
                raise RuntimeError('version_regex_replace for override %d of tool %s is not a string' %
                                   (index, tool_name))

            export_paths = override.get('export_paths')
            if export_paths is not None and type(export_paths) is not list:
                raise RuntimeError('export_paths for override %d of tool %s is not a list' % (index, tool_name))

            export_vars = override.get('export_vars')
            if export_vars is not None and type(export_vars) is not dict:
                raise RuntimeError('export_vars for override %d of tool %s is not a mapping' % (index, tool_name))
            tool_obj.platform_overrides.append(override)

        recommended_versions = {}
        for version_dict in versions:
            version = version_dict.get('name')
            if type(version) is not expected_str_type:
                raise RuntimeError('version name for tool {} is not a string'.format(tool_name))

            version_status = version_dict.get('status')
            if type(version_status) is not expected_str_type and version_status not in IDFToolVersion.STATUS_VALUES:
                raise RuntimeError('tool {} version {} status is not one of {}', tool_name, version,
                                   IDFToolVersion.STATUS_VALUES)

            version_obj = IDFToolVersion(version, version_status)
            for platform_id, platform_dict in version_dict.items():
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

    def to_json(self):
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
            'versions': versions_array,
        }
        if self.options.version_regex_replace != VERSION_REGEX_REPLACE_DEFAULT:
            tool_json['version_regex_replace'] = self.options.version_regex_replace
        if overrides_array:
            tool_json['platform_overrides'] = overrides_array
        if self.options.strip_container_dirs:
            tool_json['strip_container_dirs'] = self.options.strip_container_dirs
        return tool_json


def load_tools_info():
    """
    Load tools metadata from tools.json, return a dictionary: tool name - tool info
    """
    tool_versions_file_name = global_tools_json

    with open(tool_versions_file_name, 'r') as f:
        tools_info = json.load(f)

    return parse_tools_info_json(tools_info)


def parse_tools_info_json(tools_info):
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


def dump_tools_json(tools_info):
    tools_array = []
    for tool_name, tool_obj in tools_info.items():
        tool_json = tool_obj.to_json()
        tools_array.append(tool_json)
    file_json = {'version': TOOLS_FILE_VERSION, 'tools': tools_array}
    return json.dumps(file_json, indent=2, separators=(',', ': '), sort_keys=True)


def get_python_env_path():
    python_ver_major_minor = '{}.{}'.format(sys.version_info.major, sys.version_info.minor)

    version_file_path = os.path.join(global_idf_path, 'version.txt')
    if os.path.exists(version_file_path):
        with open(version_file_path, "r") as version_file:
            idf_version_str = version_file.read()
    else:
        idf_version_str = subprocess.check_output(['git', '-C', global_idf_path, 'describe', '--tags'], cwd=global_idf_path, env=os.environ).decode()
    match = re.match(r'^v([0-9]+\.[0-9]+).*', idf_version_str)
    idf_version = match.group(1)

    idf_python_env_path = os.path.join(global_idf_tools_path, 'python_env',
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


def action_list(args):
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
        versions_sorted = sorted(versions_for_platform.keys(), key=tool.versions.get, reverse=True)
        for version in versions_sorted:
            version_obj = tool.versions[version]
            info('  - {} ({}{})'.format(version, version_obj.status,
                                        ', installed' if version in tool.versions_installed else ''))


def action_check(args):
    tools_info = load_tools_info()
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


def action_export(args):
    tools_info = load_tools_info()
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
                if args.prefer_system:
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


def apply_mirror_prefix_map(args, tool_obj, tool_version):
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
    download_obj = tool_obj.versions[tool_version].get_download_for_platform(PYTHON_PLATFORM)
    if mirror_prefix_map and download_obj:
        for item in mirror_prefix_map:
            if URL_PREFIX_MAP_SEPARATOR not in item:
                warn('invalid mirror-prefix-map item (missing \'{}\') {}'.format(URL_PREFIX_MAP_SEPARATOR, item))
                continue
            search, replace = item.split(URL_PREFIX_MAP_SEPARATOR, 1)
            old_url = download_obj.url
            new_url = re.sub(search, replace, old_url)
            if new_url != old_url:
                info('Changed download URL: {} => {}'.format(old_url, new_url))
                download_obj.url = new_url
                break


def action_install(args):
    tools_info = load_tools_info()
    tools_spec = args.tools
    if not tools_spec:
        tools_spec = [k for k, v in tools_info.items() if v.get_install_type() == IDFTool.INSTALL_ALWAYS]
        info('Installing tools: {}'.format(', '.join(tools_spec)))
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
        apply_mirror_prefix_map(args, tool_obj, tool_version)

        tool_obj.download(tool_version)
        tool_obj.install(tool_version)


def action_install_python_env(args):
    idf_python_env_path, _, virtualenv_python = get_python_env_path()

    if args.reinstall and os.path.exists(idf_python_env_path):
        warn('Removing the existing Python environment in {}'.format(idf_python_env_path))
        shutil.rmtree(idf_python_env_path)

    if not os.path.exists(virtualenv_python):
        info('Creating a new Python environment in {}'.format(idf_python_env_path))

        try:
            import virtualenv   # noqa: F401
        except ImportError:
            info('Installing virtualenv')
            subprocess.check_call([sys.executable, '-m', 'pip', 'install', '--user', 'virtualenv'],
                                  stdout=sys.stdout, stderr=sys.stderr)

        subprocess.check_call([sys.executable, '-m', 'virtualenv', '--no-site-packages', idf_python_env_path],
                              stdout=sys.stdout, stderr=sys.stderr)
    run_args = [virtualenv_python, '-m', 'pip', 'install', '--no-warn-script-location']
    requirements_txt = os.path.join(global_idf_path, 'requirements.txt')
    run_args += ['-r', requirements_txt]
    if args.extra_wheels_dir:
        run_args += ['--find-links', args.extra_wheels_dir]
    info('Installing Python packages from {}'.format(requirements_txt))
    subprocess.check_call(run_args, stdout=sys.stdout, stderr=sys.stderr)


def action_add_version(args):
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


def action_rewrite(args):
    tools_info = load_tools_info()
    json_str = dump_tools_json(tools_info)
    if not args.output:
        args.output = os.path.join(global_idf_path, TOOLS_FILE_NEW)
    with open(args.output, 'w') as f:
        f.write(json_str)
        f.write('\n')
    info('Wrote output to {}'.format(args.output))


def action_validate(args):
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


def main(argv):
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
    if IDF_MAINTAINER:
        install.add_argument('--mirror-prefix-map', nargs='*',
                             help='Pattern to rewrite download URLs, with source and replacement separated by comma.' +
                                  ' E.g. http://foo.com,http://test.foo.com')
    install.add_argument('tools', nargs='*', help='Tools to install. ' +
                                                  'To install a specific version use tool_name@version syntax.' +
                                                  'Use \'all\' to install all tools, including the optional ones.')

    install_python_env = subparsers.add_parser('install-python-env',
                                               help='Create Python virtual environment and install the ' +
                                                    'required Python packages')
    install_python_env.add_argument('--reinstall', help='Discard the previously installed environment',
                                    action='store_true')
    install_python_env.add_argument('--extra-wheels-dir', help='Additional directories with wheels ' +
                                    'to use during installation')

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
        global_idf_path = os.path.realpath(os.path.join(os.path.dirname(__file__), ".."))

    global global_idf_tools_path
    global_idf_tools_path = os.environ.get('IDF_TOOLS_PATH') or os.path.expanduser(IDF_TOOLS_PATH_DEFAULT)

    if sys.version_info.major == 2:
        try:
            global_idf_tools_path.decode('ascii')
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
