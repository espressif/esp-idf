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

from gitlab_api import Gitlab

try:
    from typing import Any, Union
except ImportError:
    # Only used for type annotations
    pass


def _get_mr_obj(source_branch):  # type: (str) -> Union[Gitlab, None]
    if not source_branch:
        return None
    gl = Gitlab(os.getenv('CI_PROJECT_ID', 'espressif/esp-idf'))
    if not gl.project:
        return None
    mrs = gl.project.mergerequests.list(state='opened', source_branch=source_branch)
    if mrs:
        return mrs[0]  # one source branch can only have one opened MR at one moment
    else:
        return None


def get_mr_iid(source_branch):  # type: (str) -> str
    mr = _get_mr_obj(source_branch)
    if not mr:
        return ''
    else:
        return str(mr.iid)


def get_mr_changed_files(source_branch):  # type: (str) -> Any
    mr = _get_mr_obj(source_branch)
    if not mr:
        return ''

    return subprocess.check_output(['git', 'diff', '--name-only',
                                    'origin/{}...origin/{}'.format(mr.target_branch, source_branch)]).decode('utf8')


def get_mr_commits(source_branch):  # type: (str) -> str
    mr = _get_mr_obj(source_branch)
    if not mr:
        return ''
    return '\n'.join([commit.id for commit in mr.commits()])


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Get the latest merge request info by pipeline')
    actions = parser.add_subparsers(dest='action', help='info type')

    common_args = argparse.ArgumentParser(add_help=False)
    common_args.add_argument('src_branch', nargs='?', help='source branch')

    actions.add_parser('id', parents=[common_args])
    actions.add_parser('files', parents=[common_args])
    actions.add_parser('commits', parents=[common_args])

    args = parser.parse_args()

    if args.action == 'id':
        print(get_mr_iid(args.src_branch))
    elif args.action == 'files':
        print(get_mr_changed_files(args.src_branch))
    elif args.action == 'commits':
        print(get_mr_commits(args.src_branch))
    else:
        raise NotImplementedError('not possible to get here')
