#!/usr/bin/env python
#
# internal use only for CI
# get latest MR information by source branch
#
# SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import argparse
import os
import subprocess
import typing as t
from pathlib import Path

from gitlab_api import Gitlab

if t.TYPE_CHECKING:
    from gitlab.v4.objects import ProjectCommit, ProjectMergeRequest


def _get_mr_obj(source_branch: str) -> t.Optional['ProjectMergeRequest']:
    gl = Gitlab(os.getenv('CI_PROJECT_ID', 'espressif/esp-idf'))
    if not gl.project:
        return None

    mrs = gl.project.mergerequests.list(state='opened', source_branch=source_branch)
    if mrs:
        return mrs[0]  # one source branch can only have one opened MR at one moment
    else:
        return None


def get_mr_iid(source_branch: str) -> str:
    mr = _get_mr_obj(source_branch)
    if not mr:
        return ''
    else:
        return str(mr.iid)


def get_mr_changed_files(source_branch: str) -> t.List[str]:
    mr = _get_mr_obj(source_branch)
    if not mr:
        return []

    git_output = subprocess.check_output(
        ['git', 'diff', '--name-only', '--diff-filter=d', f'origin/{mr.target_branch}...origin/{source_branch}']
    ).decode('utf8')

    return [line.strip() for line in git_output.splitlines() if line.strip()]


def get_mr_commits(source_branch: str) -> t.List['ProjectCommit']:
    mr = _get_mr_obj(source_branch)
    if not mr:
        return []

    return list(mr.commits())


def get_mr_components(source_branch: str) -> t.List[str]:
    components: t.Set[str] = set()
    for f in get_mr_changed_files(source_branch):
        file = Path(f)
        if (
            file.parts[0] == 'components'
            and 'test_apps' not in file.parts
            and file.parts[-1] != '.build-test-rules.yml'
        ):
            components.add(file.parts[1])

    return list(components)


def _print_list(_list: t.List[str], separator: str = '\n') -> None:
    print(separator.join(_list))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Get the latest merge request info by pipeline')
    actions = parser.add_subparsers(dest='action', help='info type', required=True)

    common_args = argparse.ArgumentParser(add_help=False)
    common_args.add_argument('src_branch', help='source branch')

    actions.add_parser('id', parents=[common_args])
    actions.add_parser('files', parents=[common_args])
    actions.add_parser('commits', parents=[common_args])
    actions.add_parser('components', parents=[common_args])

    args = parser.parse_args()

    if args.action == 'id':
        print(get_mr_iid(args.src_branch))
    elif args.action == 'files':
        _print_list(get_mr_changed_files(args.src_branch))
    elif args.action == 'commits':
        _print_list([commit.id for commit in get_mr_commits(args.src_branch)])
    elif args.action == 'components':
        _print_list(get_mr_components(args.src_branch))
    else:
        raise NotImplementedError('not possible to get here')
