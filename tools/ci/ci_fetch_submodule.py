#!/usr/bin/env python
# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# internal use only for CI
# download archive of one commit instead of cloning entire submodule repo

import argparse
import os
import re
import shutil
import subprocess
import time
from typing import Any, List

import gitlab_api

SUBMODULE_PATTERN = re.compile(r"\[submodule \"([^\"]+)\"]")
PATH_PATTERN = re.compile(r'path\s+=\s+(\S+)')
URL_PATTERN = re.compile(r'url\s+=\s+(\S+)')

SUBMODULE_ARCHIVE_TEMP_FOLDER = 'submodule_archive'
# need to match the one defined in CI yaml files for caching purpose
SUBMODULE_ARCHIVE_CACHE_DIR = '.cache/submodule_archives'


class SubModule(object):
    # We don't need to support recursive submodule clone now

    GIT_LS_TREE_OUTPUT_PATTERN = re.compile(r'\d+\s+commit\s+([0-9a-f]+)\s+')

    def __init__(self, gitlab_inst: gitlab_api.Gitlab, path: str, url: str) -> None:
        self.path = path
        self.url = url
        self.gitlab_inst = gitlab_inst
        self.project_id = self._get_project_id(url)
        self.commit_id = self._get_commit_id(path)

    def _get_commit_id(self, path: str) -> str:
        output = subprocess.check_output(['git', 'ls-tree', 'HEAD', path]).decode()
        # example output: 160000 commit d88a262fbdf35e5abb372280eb08008749c3faa0	components/esp_wifi/lib
        match = self.GIT_LS_TREE_OUTPUT_PATTERN.search(output)
        return match.group(1) if match is not None else ''

    def _get_project_id(self, url: str) -> Any:
        base_name = os.path.basename(url)
        project_id = self.gitlab_inst.get_project_id(os.path.splitext(base_name)[0],  # remove .git
                                                     namespace='espressif')
        return project_id

    def download_archive(self) -> None:
        print('Update submodule: {}: {}'.format(self.path, self.commit_id))
        path_name = self.gitlab_inst.download_archive(self.commit_id, SUBMODULE_ARCHIVE_TEMP_FOLDER,
                                                      self.project_id, SUBMODULE_ARCHIVE_CACHE_DIR)
        renamed_path = os.path.join(os.path.dirname(path_name), os.path.basename(self.path))
        os.rename(path_name, renamed_path)
        shutil.rmtree(self.path, ignore_errors=True)
        shutil.move(renamed_path, os.path.dirname(self.path))


def update_submodule(git_module_file: str, submodules_to_update: List) -> None:
    gitlab_inst = gitlab_api.Gitlab()
    submodules = []
    with open(git_module_file, 'r') as f:
        data = f.read()
    match = SUBMODULE_PATTERN.search(data)
    if match is not None:
        while True:
            next_match = SUBMODULE_PATTERN.search(data, pos=match.end())
            if next_match:
                end_pos = next_match.start()
            else:
                end_pos = len(data)
            path_match = PATH_PATTERN.search(data, pos=match.end(), endpos=end_pos)
            url_match = URL_PATTERN.search(data, pos=match.end(), endpos=end_pos)
            path = path_match.group(1) if path_match is not None else ''
            url = url_match.group(1) if url_match is not None else ''

            filter_result = True
            if submodules_to_update:
                if path not in submodules_to_update:
                    filter_result = False
            if filter_result:
                submodules.append(SubModule(gitlab_inst, path, url))

            match = next_match
            if not match:
                break

    shutil.rmtree(SUBMODULE_ARCHIVE_TEMP_FOLDER, ignore_errors=True)

    for submodule in submodules:
        submodule.download_archive()


if __name__ == '__main__':
    start_time = time.time()
    parser = argparse.ArgumentParser()
    parser.add_argument('--repo_path', '-p', default='.', help='repo path')
    parser.add_argument('--submodule', '-s', default='all',
                        help='Submodules to update. By default update all submodules. '
                             'For multiple submodules, separate them with `;`. '
                             '`all` and `none` are special values that indicates we fetch all / none submodules')
    args = parser.parse_args()
    if args.submodule == 'none':
        print("don't need to update submodules")
        exit(0)
    if args.submodule == 'all':
        _submodules = []
    else:
        _submodules = args.submodule.split(';')
    update_submodule(os.path.join(args.repo_path, '.gitmodules'), _submodules)
    print('total time spent on update submodule: {:.02f}s'.format(time.time() - start_time))
