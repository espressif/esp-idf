#!/usr/bin/env python
#
# Wrapper to run make and preprocess any paths in the output from MSYS Unix-style paths
# to Windows paths, for Eclipse
from __future__ import print_function, division
import sys
import subprocess
import os.path
import re

UNIX_PATH_RE = re.compile(r'(/[^ \'"]+)+')

paths = {}


def check_path(path):
    try:
        return paths[path]
    except KeyError:
        pass
    paths[path] = path  # cache as failed, replace with success if it works
    try:
        winpath = subprocess.check_output(['cygpath', '-w', path]).decode('utf-8').strip()
    except subprocess.CalledProcessError:
        return path  # something went wrong running cygpath, assume this is not a path!
    if not os.path.exists(winpath):
        return path  # not actually a valid path
    winpath = winpath.replace("\\", "/")  # make consistent with forward-slashes used elsewhere
    paths[path] = winpath
    return winpath


def main():
    print("Running make in '%s'" % check_path(os.getcwd()))
    make = subprocess.Popen(["make"] + sys.argv[1:] + ["BATCH_BUILD=1"], stdout=subprocess.PIPE)
    for line in iter(make.stdout.readline, ''):
        line = re.sub(UNIX_PATH_RE, lambda m: check_path(m.group(0)), line)
        print(line.rstrip())
    sys.exit(make.wait())


if __name__ == "__main__":
    main()
