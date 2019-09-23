# Utility functions used in conf.py
#
# Copyright 2017 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import unicode_literals
from io import open
import os
import shutil

try:
    import urllib.request
    _urlretrieve = urllib.request.urlretrieve
except ImportError:
    # Python 2 fallback
    import urllib
    _urlretrieve = urllib.urlretrieve


def run_cmd_get_output(cmd):
    return os.popen(cmd).read().strip()


def files_equal(path_1, path_2):
    if not os.path.exists(path_1) or not os.path.exists(path_2):
        return False
    file_1_contents = ''
    with open(path_1, "r", encoding='utf-8') as f_1:
        file_1_contents = f_1.read()
    file_2_contents = ''
    with open(path_2, "r", encoding='utf-8') as f_2:
        file_2_contents = f_2.read()
    return file_1_contents == file_2_contents


def copy_file_if_modified(src_file_path, dst_file_path):
    if not files_equal(src_file_path, dst_file_path):
        dst_dir_name = os.path.dirname(dst_file_path)
        if not os.path.isdir(dst_dir_name):
            os.makedirs(dst_dir_name)
        shutil.copy(src_file_path, dst_file_path)


def copy_if_modified(src_path, dst_path):
    if os.path.isfile(src_path):
        copy_file_if_modified(src_path, dst_path)
        return

    src_path_len = len(src_path)
    for root, dirs, files in os.walk(src_path):
        for src_file_name in files:
            src_file_path = os.path.join(root, src_file_name)
            dst_file_path = os.path.join(dst_path + root[src_path_len:], src_file_name)
            copy_file_if_modified(src_file_path, dst_file_path)


def download_file_if_missing(from_url, to_path):
    filename_with_path = to_path + "/" + os.path.basename(from_url)
    exists = os.path.isfile(filename_with_path)
    if exists:
        print("The file '%s' already exists" % (filename_with_path))
    else:
        tmp_file, header = _urlretrieve(from_url)
        with open(filename_with_path, 'wb') as fobj:
            with open(tmp_file, 'rb') as tmp:
                fobj.write(tmp.read())
