
# !/usr/bin/env python
#
# Copyright 2019 Espressif Systems (Shanghai) PTE LTD
#
# This script is used by the linker script generation in order to list a template's
# INCLUDE'd linker scripts recursively. This is so that updates to these INCLUDE'd
# scripts also trigger a relink of the app.
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
import os
import re
import argparse


INCLUDE_PATTERN = re.compile(r"INCLUDE[\t ]+([^\n]+)")


def find_includes(file_path, includes_list):
    # Find include files recursively
    file_dir = os.path.dirname(file_path)

    with open(file_path, "r") as f:
        includes_list.append(file_path)
        includes = re.findall(INCLUDE_PATTERN, f.read())

    for include in includes:
        include_file_path = os.path.abspath(os.path.join(file_dir, include))
        find_includes(include_file_path, includes_list)


def main():
    argparser = argparse.ArgumentParser(description="List INCLUDE'd linker scripts recursively")
    argparser.add_argument("input", help="input linker script")
    args = argparser.parse_args()

    includes_list = list()
    find_includes(args.input, includes_list)
    includes_list.remove(args.input)

    print(" ".join(includes_list))


if __name__ == "__main__":
    main()
