# internal use only for CI
# some CI related util functions
#
# Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#
import functools
import logging
import os
import re
import subprocess
import sys

IDF_PATH = os.path.abspath(os.getenv('IDF_PATH', os.path.join(os.path.dirname(__file__), '..', '..')))


def get_submodule_dirs(full_path=False):  # type: (bool) -> list
    """
    To avoid issue could be introduced by multi-os or additional dependency,
    we use python and git to get this output
    :return: List of submodule dirs
    """
    dirs = []
    try:
        lines = subprocess.check_output(
            ['git', 'config', '--file', os.path.realpath(os.path.join(IDF_PATH, '.gitmodules')),
             '--get-regexp', 'path']).decode('utf8').strip().split('\n')
        for line in lines:
            _, path = line.split(' ')
            if full_path:
                dirs.append(os.path.join(IDF_PATH, path))
            else:
                dirs.append(path)
    except Exception as e:  # pylint: disable=W0703
        logging.warning(str(e))

    return dirs


def _check_git_filemode(full_path):  # type: (str) -> bool
    try:
        stdout = subprocess.check_output(['git', 'ls-files', '--stage', full_path]).strip().decode('utf-8')
    except subprocess.CalledProcessError:
        return True

    mode = stdout.split(' ', 1)[0]  # e.g. 100644 for a rw-r--r--
    if any([int(i, 8) & 1 for i in mode[-3:]]):
        return True
    return False


def is_executable(full_path):  # type: (str) -> bool
    """
    os.X_OK will always return true on windows. Use git to check file mode.
    :param full_path: file full path
    :return: True is it's an executable file
    """
    if sys.platform == 'win32':
        return _check_git_filemode(full_path)
    return os.access(full_path, os.X_OK)


def get_git_files(path=IDF_PATH, full_path=False):  # type: (str, bool) -> list[str]
    """
    Get the result of git ls-files
    :param path: path to run git ls-files
    :param full_path: return full path if set to True
    :return: list of file paths
    """
    try:
        files = subprocess.check_output(['git', 'ls-files'], cwd=path).decode('utf8').strip().split('\n')
    except Exception as e:  # pylint: disable=W0703
        logging.warning(str(e))
        files = []
    return [os.path.join(path, f) for f in files] if full_path else files


# this function is a commit from
# https://github.com/python/cpython/pull/6299/commits/bfd63120c18bd055defb338c075550f975e3bec1
# In order to solve python https://bugs.python.org/issue9584
# glob pattern does not support brace expansion issue
def _translate(pat):  # type: (str) -> str
    """Translate a shell PATTERN to a regular expression.
    There is no way to quote meta-characters.
    """
    i, n = 0, len(pat)
    res = ''
    while i < n:
        c = pat[i]
        i = i + 1
        if c == '*':
            res = res + '.*'
        elif c == '?':
            res = res + '.'
        elif c == '[':
            j = i
            if j < n and pat[j] == '!':
                j = j + 1
            if j < n and pat[j] == ']':
                j = j + 1
            while j < n and pat[j] != ']':
                j = j + 1
            if j >= n:
                res = res + '\\['
            else:
                stuff = pat[i:j]
                if '--' not in stuff:
                    stuff = stuff.replace('\\', r'\\')
                else:
                    chunks = []
                    k = i + 2 if pat[i] == '!' else i + 1
                    while True:
                        k = pat.find('-', k, j)
                        if k < 0:
                            break
                        chunks.append(pat[i:k])
                        i = k + 1
                        k = k + 3
                    chunks.append(pat[i:j])
                    # Escape backslashes and hyphens for set difference (--).
                    # Hyphens that create ranges shouldn't be escaped.
                    stuff = '-'.join(s.replace('\\', r'\\').replace('-', r'\-')
                                     for s in chunks)
                # Escape set operations (&&, ~~ and ||).
                stuff = re.sub(r'([&~|])', r'\\\1', stuff)
                i = j + 1
                if stuff[0] == '!':
                    stuff = '^' + stuff[1:]
                elif stuff[0] in ('^', '['):
                    stuff = '\\' + stuff
                res = '%s[%s]' % (res, stuff)
        elif c == '{':
            # Handling of brace expression: '{PATTERN,PATTERN,...}'
            j = 1
            while j < n and pat[j] != '}':
                j = j + 1
            if j >= n:
                res = res + '\\{'
            else:
                stuff = pat[i:j]
                i = j + 1

                # Find indices of ',' in pattern excluding r'\,'.
                # E.g. for r'a\,a,b\b,c' it will be [4, 8]
                indices = [m.end() for m in re.finditer(r'[^\\],', stuff)]

                # Splitting pattern string based on ',' character.
                # Also '\,' is translated to ','. E.g. for r'a\,a,b\b,c':
                # * first_part = 'a,a'
                # * last_part = 'c'
                # * middle_part = ['b,b']
                first_part = stuff[:indices[0] - 1].replace(r'\,', ',')
                last_part = stuff[indices[-1]:].replace(r'\,', ',')
                middle_parts = [
                    stuff[st:en - 1].replace(r'\,', ',')
                    for st, en in zip(indices, indices[1:])
                ]

                # creating the regex from splitted pattern. Each part is
                # recursivelly evaluated.
                expanded = functools.reduce(
                    lambda a, b: '|'.join((a, b)),
                    (_translate(elem) for elem in [first_part] + middle_parts + [last_part])
                )
                res = '%s(%s)' % (res, expanded)
        else:
            res = res + re.escape(c)
    return res


def translate(pat):  # type: (str) -> str
    res = _translate(pat)
    return r'(?s:%s)\Z' % res


magic_check = re.compile('([*?[{])')
magic_check_bytes = re.compile(b'([*?[{])')
# cpython github PR 6299 ends here

# Here's the code block we're going to use to monkey patch ``glob`` module and ``fnmatch`` modules
# DO NOT monkey patch here, only patch where you really needs
#
# import glob
# import fnmatch
# from idf_ci_utils import magic_check, magic_check_bytes, translate
# glob.magic_check = magic_check
# glob.magic_check_bytes = magic_check_bytes
# fnmatch.translate = translate
