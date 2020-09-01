#!/usr/bin/env python
#
# internal use only for CI
# get latest MR IID by source branch

import argparse
import os

from gitlab_api import Gitlab


def get_MR_IID_by_source_branch(source_branch):
    if not source_branch:
        return ''
    gl = Gitlab(os.getenv('CI_PROJECT_ID'))
    if not gl.project:
        return ''
    mrs = gl.project.mergerequests.list(state='opened', source_branch=source_branch)
    if mrs:
        mr = mrs[0]  # one source branch can only have one opened MR at one moment
        return mr.iid
    return ''


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Get the latest MR IID by source branch, if not found, return empty string')
    parser.add_argument('source_branch', nargs='?', help='source_branch')  # won't fail if it's empty

    args = parser.parse_args()

    print(get_MR_IID_by_source_branch(args.source_branch))
