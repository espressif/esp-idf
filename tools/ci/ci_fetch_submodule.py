#!/usr/bin/env python
# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# internal use only for CI
# download archive of one commit instead of cloning entire submodule repo

import argparse
import subprocess
import sys
import time

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--repo_path', '-p', default='.', help='repo path')
    parser.add_argument(
        '--submodule',
        '-s',
        default='all',
        help='Submodules to update. By default update all submodules. '
        'For multiple submodules, separate them with `;`. '
        '`all` and `none` are special values that indicates we fetch all / none submodules',
    )
    args = parser.parse_args()

    print('This script is deprecated, please use the following git command with gitlab cache `.git/modules` instead.')
    print('Calling `git submodule update --init --depth=1` ...')

    start_time = time.time()
    subprocess.check_call(
        ['git', 'submodule', 'update', '--init', '--depth=1'], stdout=sys.stdout, stderr=sys.stderr, cwd=args.repo_path
    )
    print('total time spent on update submodule: {:.02f}s'.format(time.time() - start_time))
