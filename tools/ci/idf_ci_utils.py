# SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
# internal use only for CI
# some CI related util functions
import logging
import os
import re
import subprocess
import sys
import typing as t
from functools import cached_property
from pathlib import Path

IDF_PATH = os.path.abspath(os.getenv('IDF_PATH', os.path.join(os.path.dirname(__file__), '..', '..')))


def get_submodule_dirs(full_path: bool = False) -> t.List[str]:
    """
    To avoid issue could be introduced by multi-os or additional dependency,
    we use python and git to get this output
    :return: List of submodule dirs
    """
    dirs = []
    try:
        lines = (
            subprocess.check_output(
                [
                    'git',
                    'config',
                    '--file',
                    os.path.realpath(os.path.join(IDF_PATH, '.gitmodules')),
                    '--get-regexp',
                    'path',
                ]
            )
            .decode('utf8')
            .strip()
            .split('\n')
        )
        for line in lines:
            _, path = line.split(' ')
            if full_path:
                dirs.append(os.path.join(IDF_PATH, path))
            else:
                dirs.append(path)
    except Exception as e:  # pylint: disable=W0703
        logging.warning(str(e))

    return dirs


def _check_git_filemode(full_path: str) -> bool:
    try:
        stdout = subprocess.check_output(['git', 'ls-files', '--stage', full_path]).strip().decode('utf-8')
    except subprocess.CalledProcessError:
        return True

    mode = stdout.split(' ', 1)[0]  # e.g. 100644 for a rw-r--r--
    if any([int(i, 8) & 1 for i in mode[-3:]]):
        return True
    return False


def is_executable(full_path: str) -> bool:
    """
    os.X_OK will always return true on windows. Use git to check file mode.
    :param full_path: file full path
    :return: True is it's an executable file
    """
    if sys.platform == 'win32':
        return _check_git_filemode(full_path)
    return os.access(full_path, os.X_OK)


def get_git_files(path: str = IDF_PATH, full_path: bool = False) -> t.List[str]:
    """
    Get the result of git ls-files
    :param path: path to run git ls-files
    :param full_path: return full path if set to True
    :return: list of file paths
    """
    try:
        # this is a workaround when using under worktree
        # if you're using worktree, when running git commit a new environment variable GIT_DIR would be declared,
        # the value should be <origin_repo_path>/.git/worktrees/<worktree name>
        # This would affect the return value of `git ls-files`, unset this would use the `cwd`value or its parent
        # folder if no `.git` folder found in `cwd`.
        workaround_env = os.environ.copy()
        workaround_env.pop('GIT_DIR', None)
        files = (
            subprocess.check_output(['git', 'ls-files'], cwd=path, env=workaround_env)
            .decode('utf8')
            .strip()
            .split('\n')
        )
    except Exception as e:  # pylint: disable=W0703
        logging.warning(str(e))
        files = []
    return [os.path.join(path, f) for f in files] if full_path else files


def to_list(s: t.Any) -> t.List[t.Any]:
    if not s:
        return []

    if isinstance(s, (set, tuple)):
        return list(s)

    if isinstance(s, list):
        return s

    return [s]


class GitlabYmlConfig:
    def __init__(self, root_yml_filepath: str = os.path.join(IDF_PATH, '.gitlab-ci.yml')) -> None:
        self._config: t.Dict[str, t.Any] = {}
        self._defaults: t.Dict[str, t.Any] = {}

        self._load(root_yml_filepath)

    def _load(self, root_yml_filepath: str) -> None:
        # avoid unused import in other pre-commit hooks
        import yaml

        all_config = dict()
        root_yml = yaml.load(open(root_yml_filepath), Loader=yaml.FullLoader)

        # expanding "include"
        for item in root_yml.pop('include', []) or []:
            all_config.update(yaml.load(open(os.path.join(IDF_PATH, item)), Loader=yaml.FullLoader))

        if 'default' in all_config:
            self._defaults = all_config.pop('default')

        self._config = all_config

        # anchor is the string that will be reused in templates
        self._anchor_keys: t.Set[str] = set()
        # template is a dict that will be extended
        self._template_keys: t.Set[str] = set()
        self._used_template_keys: t.Set[str] = set()  # tracing the used templates
        # job is a dict that will be executed
        self._job_keys: t.Set[str] = set()

        self.expand_extends()

    @property
    def default(self) -> t.Dict[str, t.Any]:
        return self._defaults

    @property
    def config(self) -> t.Dict[str, t.Any]:
        return self._config

    @cached_property
    def global_keys(self) -> t.List[str]:
        return ['default', 'include', 'workflow', 'variables', 'stages']

    @cached_property
    def anchors(self) -> t.Dict[str, t.Any]:
        return {k: v for k, v in self.config.items() if k in self._anchor_keys}

    @cached_property
    def jobs(self) -> t.Dict[str, t.Any]:
        return {k: v for k, v in self.config.items() if k in self._job_keys}

    @cached_property
    def templates(self) -> t.Dict[str, t.Any]:
        return {k: v for k, v in self.config.items() if k in self._template_keys}

    @cached_property
    def used_templates(self) -> t.Set[str]:
        return self._used_template_keys

    def expand_extends(self) -> None:
        """
        expand the `extends` key in-place.
        """
        for k, v in self.config.items():
            if k in self.global_keys:
                continue

            if isinstance(v, (str, list)):
                self._anchor_keys.add(k)
            elif k.startswith('.if-'):
                self._anchor_keys.add(k)
            elif k.startswith('.'):
                self._template_keys.add(k)
            elif isinstance(v, dict):
                self._job_keys.add(k)
            else:
                raise ValueError(f'Unknown type for key {k} with value {v}')

        # no need to expand anchor

        # expand template first
        for k in self._template_keys:
            self._expand_extends(k)

        # expand job
        for k in self._job_keys:
            self._expand_extends(k)

    def _merge_dict(self, d1: t.Dict[str, t.Any], d2: t.Dict[str, t.Any]) -> t.Any:
        for k, v in d2.items():
            if k in d1:
                if isinstance(v, dict) and isinstance(d1[k], dict):
                    d1[k] = self._merge_dict(d1[k], v)
                else:
                    d1[k] = v
            else:
                d1[k] = v

        return d1

    def _expand_extends(self, name: str) -> t.Dict[str, t.Any]:
        extends = to_list(self.config[name].pop('extends', None))
        if not extends:
            return self.config[name]  # type: ignore

        original_d = self.config[name].copy()
        d = {}
        while extends:
            self._used_template_keys.update(extends)  # for tracking

            for i in extends:
                d.update(self._expand_extends(i))

            extends = to_list(self.config[name].pop('extends', None))

        self.config[name] = self._merge_dict(d, original_d)
        return self.config[name]  # type: ignore


def get_all_manifest_files() -> t.List[str]:
    paths: t.List[str] = []

    for p in Path(IDF_PATH).glob('**/.build-test-rules.yml'):
        if 'managed_components' in p.parts:
            continue

        paths.append(str(p))

    return paths


def sanitize_job_name(name: str) -> str:
    """
    Sanitize the job name from CI_JOB_NAME

    - for job with `parallel: int` set, the `CI_JOB_NAME` would be `job_name index/total`, like `foo 1/3`
    - for job with `parallel: matrix` set, the `CI_JOB_NAME` would be `job_name: [var1, var2]`, like `foo: [a, b]`

    We consider
    - the jobs generated by `parallel: int` as the same job, i.e., we remove the index/total part.
    - the jobs generated by `parallel: matrix` as different jobs, so we keep the matrix part.

    :param name: job name
    :return: sanitized job name
    """
    return re.sub(r' \d+/\d+', '', name)


def idf_relpath(p: str) -> str:
    """
    Turn all paths under IDF_PATH to relative paths
    :param p: path
    :return: relpath to IDF_PATH, or absolute path if not under IDF_PATH
    """
    abs_path = os.path.abspath(p)
    if abs_path.startswith(IDF_PATH):
        return os.path.relpath(abs_path, IDF_PATH)
    else:
        return abs_path
