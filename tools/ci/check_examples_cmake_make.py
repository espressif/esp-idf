#!/usr/bin/env python

import json
import os
import pprint
import subprocess
import sys

# =============================================================================
# Service funcs
# =============================================================================


def _build_path(path, *paths):
    return str(os.path.normpath(os.path.join(path, *paths)).replace('\\', '/'))


def _unify_paths(path_list):
    return [_build_path(p) for p in path_list]


def _exclude_by_pat_list(path_list, ignore_list):
    print('- Applying ignore list')
    path_list_res = list(path_list)
    for ign in ignore_list:
        if len(ign.strip()):
            for p in path_list:
                if p.find(ign) != -1:
                    try:
                        path_list_res.remove(p)
                    except ValueError:
                        pass
    return path_list_res


def _file2linelist(path):
    with open(path) as f:
        lines = [line.rstrip() for line in f]
    return [str(line) for line in lines]


# =============================================================================
# Test funcs
# =============================================================================


def get_idf_path(path, *paths):
    IDF_PATH = os.getenv('IDF_PATH')
    return _build_path(IDF_PATH, path, *paths)


def _get_apps(target, build_system):
    print('- Getting paths of apps')
    args = [sys.executable,
            get_idf_path('tools/find_apps.py'),
            '-p',
            get_idf_path('examples'),
            '--recursive',
            '--target', target,
            '--build-system', build_system]
    output = subprocess.check_output(args).decode('utf-8')
    o_list = output.split('\n')
    json_list = []
    for j in o_list:
        if j:
            json_list.append(json.loads(j))
    app_paths = []
    for j in json_list:
        app_paths.append(j['app_dir'])
    return _unify_paths(app_paths)


def get_apps(target, build_system, ignorelist):
    apps = _get_apps(target, build_system)
    if len(ignorelist):
        return _exclude_by_pat_list(apps, ignorelist)
    else:
        return apps


def get_cmake_ignore_list():
    print('- Getting CMake ignore list')
    return _file2linelist(
        get_idf_path('tools', 'ci',
                     'check_examples_cmake_make-cmake_ignore.txt'))


def get_make_ignore_list():
    print('- Getting Make ignore list')
    return _file2linelist(
        get_idf_path('tools', 'ci',
                     'check_examples_cmake_make-make_ignore.txt'))


def diff(first, second):
    print('- Comparing...')
    first = set(first)
    second = set(second)
    res = list(first - second) + list(second - first)
    return res


def main():
    cmake_ignore = get_cmake_ignore_list()
    make_ignore = get_make_ignore_list()
    cmakes = get_apps('esp32', 'cmake', cmake_ignore)
    makes = get_apps('esp32', 'make', make_ignore)

    res = diff(cmakes, makes)

    if len(res):
        pp = pprint.PrettyPrinter(indent=4)
        print(
            '[ ERROR ] Some projects are not containing Make and Cmake project files:'
        )
        pp.pprint(res)
        raise ValueError('Test is not passed')
    else:
        print('[ DONE ]')


if __name__ == '__main__':
    main()
