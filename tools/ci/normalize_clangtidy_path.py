#!/usr/bin/env python
import argparse
import re
from os.path import dirname, join, normpath, relpath

CLANG_TIDY_REGEX = re.compile(r'(.+|[a-zA-Z]:\\\\.+):([0-9]+):([0-9]+): ([^:]+): (.+)')


def normalize_clang_tidy_path(file_path, output_path, base_dir):
    with open(output_path, 'w') as fw:
        for line in open(file_path):
            result = CLANG_TIDY_REGEX.match(line)
            if result:
                path = result.group(1)
                abs_path = normpath(join(dirname(file_path), path))
                rel_path = relpath(abs_path, base_dir)
                line = line.replace(path, rel_path)
            fw.write(line)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('file', help='clang tidy report path')
    parser.add_argument('output_file', help='normalized clang tidy report path')
    parser.add_argument('base_dir', help='relative path base dir')
    args = parser.parse_args()

    normalize_clang_tidy_path(args.file, args.output_file, args.base_dir)
