# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import re
import shutil
import subprocess
import typing
from pathlib import Path

from test_build_system_helpers import EnvDict, run_idf_py


def run_git_cmd(*args: str,
                workdir: Path,
                env: typing.Optional[EnvDict] = None) -> subprocess.CompletedProcess:

    cmd = ['git'] + list(args)
    env_dict = dict(**os.environ)
    if env:
        env_dict.update(env)
    logging.debug('running {} in {}'.format(' '.join(cmd), workdir))

    return subprocess.run(cmd, cwd=workdir, env=env_dict,
                          stdout=subprocess.PIPE, stderr=subprocess.PIPE)


# In this test, the action needs to be performed in ESP-IDF that is valid git directory
# Copying ESP-IDF is not possible
def test_git_custom_tag() -> None:
    try:
        env_dict = dict(**os.environ)
        idf_build_test_app_path = Path(env_dict['IDF_PATH'], 'tools', 'test_build_system', 'build_test_app')

        logging.info('Project is in ESP-IDF which has a custom tag')
        env_dict['GIT_COMMITTER_NAME'] = 'No One'
        env_dict['GIT_COMMITTER_EMAIL'] = 'noone@espressif.com'
        run_git_cmd('tag', 'mytag', '-a', '-m', 'mytag', workdir=idf_build_test_app_path, env=env_dict)
        idf_ret = run_idf_py('reconfigure', workdir=idf_build_test_app_path)
        assert 'App "build_test_app" version: mytag' in idf_ret.stdout, 'Project version should be the custom tag'
        version = run_idf_py('--version', workdir=idf_build_test_app_path)
        assert 'mytag' not in version.stdout, 'IDF version should not contain mytag'

    finally:
        run_git_cmd('tag', '-d', 'mytag', workdir=idf_build_test_app_path)
        shutil.rmtree(idf_build_test_app_path / 'build')
        os.unlink(idf_build_test_app_path / 'sdkconfig')


def test_support_git_worktree(test_git_template_app: Path) -> None:
    logging.info('Supports git worktree')
    run_git_cmd('branch', 'test_build_system', workdir=test_git_template_app)
    run_git_cmd('worktree', 'add', '../esp-idf-worktree-app', 'test_build_system', workdir=test_git_template_app)
    try:
        idf_ret_template_app = run_idf_py('reconfigure', workdir=test_git_template_app)
        idf_ret_worktree_app = run_idf_py('reconfigure', workdir=os.path.join(os.path.dirname(test_git_template_app), 'esp-idf-worktree-app'))
        assert (re.search(r'-- App \"app-template\".*', idf_ret_template_app.stdout).group() ==  # type: ignore
                re.search(r'-- App \"app-template\".*', idf_ret_worktree_app.stdout).group())  # type: ignore
    finally:
        run_git_cmd('worktree', 'remove', '../esp-idf-worktree-app', workdir=test_git_template_app)
        run_git_cmd('branch', '-d', 'test_build_system', workdir=test_git_template_app)
