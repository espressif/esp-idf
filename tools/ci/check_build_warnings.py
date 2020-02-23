#!/usr/bin/env python
# coding=utf-8
#
# CI script to check build logs for warnings.
# Reads the list of builds, in the format produced by find_apps.py or build_apps.py, and finds warnings in the
# log files for every build.
# Exits with a non-zero exit code if any warning is found.

import os
import sys
import argparse
import logging
import re

try:
    from find_build_apps import BuildItem, setup_logging
except ImportError:
    sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
    from find_build_apps import BuildItem, setup_logging

WARNING_REGEX = r"error|warning"

IGNORE_WARNS = [
    re.compile(r_str) for r_str in [
        r"library/error\.o",
        r".*error.*\.c\.obj",
        r"-Werror",
        r"error\.d",
        r"reassigning to symbol",
        r"changes choice state",
        r"crosstool_version_check\.cmake",
    ]
]


def line_has_warnings(line):  # type: (str) -> bool
    if not re.search(WARNING_REGEX, line):
        return False

    has_warnings = True
    for ignored in IGNORE_WARNS:
        if re.search(ignored, line):
            has_warnings = False
            break

    return has_warnings


def main():
    parser = argparse.ArgumentParser(description="ESP-IDF app builder")
    parser.add_argument(
        "-v",
        "--verbose",
        action="count",
        help="Increase the logging level of the script. Can be specified multiple times.",
    )
    parser.add_argument(
        "--log-file",
        type=argparse.FileType("w"),
        help="Write the script log to the specified file, instead of stderr",
    )
    parser.add_argument(
        "build_list",
        type=argparse.FileType("r"),
        nargs="?",
        default=sys.stdin,
        help="Name of the file to read the list of builds from. If not specified, read from stdin.",
    )
    args = parser.parse_args()
    setup_logging(args)

    build_items = [BuildItem.from_json(line) for line in args.build_list]

    if not build_items:
        logging.error("Empty build list!")
        raise SystemExit(1)

    found_warnings = 0
    for build_item in build_items:
        if not build_item.build_log_path:
            logging.debug("No log file for {}".format(build_item.work_dir))
            continue
        with open(build_item.build_log_path, "r") as log_file:
            for line_no, line in enumerate(log_file):
                if line_has_warnings(line):
                    logging.error("Issue in app {}, config {}:".format(build_item.app_dir, build_item.config_name))
                    logging.error(line.rstrip("\n"))
                    logging.error("See {}:{} for details".format(os.path.basename(build_item.build_log_path),
                                                                 line_no + 1))
                    found_warnings += 1
                    break

    if found_warnings:
        logging.error("Checked {} builds, found {} warnings".format(len(build_items), found_warnings))
        raise SystemExit(1)

    logging.info("No warnings found")


if __name__ == "__main__":
    main()
