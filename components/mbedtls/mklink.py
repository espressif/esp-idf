#!/usr/bin/env python
#
# Wrapper for symbolic link creation on Windows that works around issues
# with native path conversion. See the component CMakeLists.txt for more details.
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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
import os
import argparse
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("type")
parser.add_argument("link")
parser.add_argument("target")

args = parser.parse_args()

link = os.path.abspath(args.link)
target = os.path.abspath(args.target)

try:
    os.makedirs(os.path.dirname(link))
except WindowsError:
    pass

mklink_cmd = ["mklink", args.type, link, target]

subprocess.call(mklink_cmd, shell=True)
