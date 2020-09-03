#!/usr/bin/env python
#
# Utility script for ESP-IDF developers to work with the CODEOWNERS file.
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

import argparse
from collections import defaultdict
import os
import subprocess
import sys

CODEOWNERS_PATH = os.path.join(os.path.dirname(__file__), "..", ".gitlab", "CODEOWNERS")
CODEOWNER_GROUP_PREFIX = "@esp-idf-codeowners/"


def action_identify(args):
    best_match = []
    with open(CODEOWNERS_PATH) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            tokens = line.split()
            path_pattern = tokens[0]
            owners = tokens[1:]
            files = files_by_pattern(path_pattern)
            if args.path in files:
                best_match = owners
    for owner in best_match:
        print(owner)


def files_by_pattern(pattern=None):
    args = ["git", "ls-files"]
    if pattern:
        args.append(pattern)
    idf_root = os.path.join(os.path.dirname(__file__), "..")
    return subprocess.check_output(args, cwd=idf_root).decode("utf-8").split()


def action_ci_check(args):
    errors = []

    def add_error(msg):
        errors.append("Error at CODEOWNERS:{}: {}".format(line_no, msg))

    files_by_owner = defaultdict(int)
    prev_path_pattern = ""
    with open(CODEOWNERS_PATH) as f:
        for line_no, line in enumerate(f, start=1):
            # Skip empty lines and comments
            line = line.strip()
            if not line or line.startswith("#"):
                continue

            # Each line has a form of "<path> <owners>+"
            tokens = line.split()
            path_pattern = tokens[0]
            owners = tokens[1:]
            if not owners:
                add_error("no owners specified for {}".format(path_pattern))

            # Check that the file is sorted by path patterns
            path_pattern_for_cmp = path_pattern.replace("-", "_")  # ignore difference between _ and - for ordering
            if path_pattern_for_cmp < prev_path_pattern:
                add_error("file is not sorted: {} < {}".format(path_pattern_for_cmp, prev_path_pattern))
            prev_path_pattern = path_pattern_for_cmp

            # Check that the pattern matches at least one file
            files = files_by_pattern(path_pattern)
            if not files:
                add_error("no files matched by pattern {}".format(path_pattern))

            # Count the number of files per owner
            for o in owners:
                # Sanity-check the owner group name
                if not o.startswith(CODEOWNER_GROUP_PREFIX):
                    add_error("owner {} doesn't start with {}".format(o, CODEOWNER_GROUP_PREFIX))
                files_by_owner[o] += len(files)

    owners_sorted = sorted([(owner, cnt) for owner, cnt in files_by_owner.items()], key=lambda p: p[0])
    print("File count per owner (not including submodules):")
    for owner, cnt in owners_sorted:
        print("{}: {} files".format(owner, cnt))

    if not errors:
        print("No errors found.")
    else:
        print("Errors found!")
        for e in errors:
            print(e)
        raise SystemExit(1)


def main():
    parser = argparse.ArgumentParser(
        sys.argv[0], description="Internal helper script for working with the CODEOWNERS file."
    )
    subparsers = parser.add_subparsers(dest="action")
    identify = subparsers.add_parser(
        "identify",
        help="Lists the owners of the specified path within IDF."
        "This command doesn't support files inside submodules, or files not added to git repository.",
    )
    identify.add_argument("path", help="Path of the file relative to the root of the repository")
    subparsers.add_parser(
        "ci-check",
        help="Check CODEOWNERS file: every line should match at least one file, sanity-check group names, "
        "check that the file is sorted by paths",
    )
    args = parser.parse_args()

    if args.action is None:
        parser.print_help()
        parser.exit(1)

    action_func_name = "action_" + args.action.replace("-", "_")
    action_func = globals()[action_func_name]
    action_func(args)


if __name__ == "__main__":
    main()
