# SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
# internal use only for CI
# some CI related util functions
import logging
import os
import subprocess
import sys
import typing as t
from functools import cached_property

IDF_PATH: str = os.path.abspath(os.getenv('IDF_PATH', os.path.join(os.path.dirname(__file__), '..', '..')))

# S3 artifact type for app-dir files outside build_*
APP_EXTRA_S3_ARTIFACT_TYPE = 'app_extra'


def get_submodule_dirs(full_path: bool = False) -> list[str]:
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


def get_git_files(path: str = IDF_PATH, full_path: bool = False) -> list[str]:
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


def to_list(s: t.Any) -> list[t.Any]:
    if not s:
        return []

    if isinstance(s, set | tuple):
        return list(s)

    if isinstance(s, list):
        return s

    return [s]


class GitlabYmlConfig:
    def __init__(self, root_yml_filepath: str = os.path.join(IDF_PATH, '.gitlab-ci.yml')) -> None:
        self._config: dict[str, t.Any] = {}
        self._defaults: dict[str, t.Any] = {}

        self._load(root_yml_filepath)

    def _load(self, root_yml_filepath: str) -> None:
        # avoid unused import in other pre-commit hooks
        import yaml

        merged_yaml = self._compile_via_gitlab_api(root_yml_filepath)
        all_config = yaml.load(merged_yaml, Loader=yaml.FullLoader) or {}

        if 'default' in all_config:
            self._defaults = all_config.pop('default')

        self._config = all_config

    def _inline_local_includes(self, root_yml_filepath: str) -> str:
        """
        Recursively resolve `include: local` entries straight from disk (so uncommitted local
        changes are always picked up -- CI runners also work off a disk checkout, so there's no
        need to fetch a ref remotely via `ci_lint`'s `content_ref`/`ref`/`dry_run_ref` GET
        params). `include: project` entries (including ones nested inside local files, e.g.
        `.gitlab/ci/common.yml` including `templates/idf/common-scripts.yml`) are collected and
        left in the final `include:` list, since those files live in another GitLab project and
        can only be resolved remotely.

        A dedicated Loader/Dumper pair round-trips `!reference` tags as a marker list subclass,
        since GitLab CI uses `!reference` which plain YAML doesn't know, and we need to parse
        (to merge dicts, not just string-concat) then re-dump losslessly.

        :param root_yml_filepath: path to the local root yml file to compile
        :return: yml content with local includes resolved and merged
        """
        import yaml

        class _Loader(yaml.FullLoader):
            pass

        class _Dumper(yaml.Dumper):
            pass

        class _Reference(list):
            pass

        _Loader.add_constructor(
            '!reference', lambda loader, node: _Reference(loader.construct_sequence(t.cast(yaml.SequenceNode, node)))
        )
        _Dumper.add_representer(_Reference, lambda dumper, data: dumper.represent_sequence('!reference', list(data)))

        def resolve(yml_filepath: str) -> tuple[dict, list]:
            with open(yml_filepath) as fr:
                data = yaml.load(fr, Loader=_Loader) or {}

            includes = to_list(data.pop('include', None))

            merged: dict = {}
            remaining_project_includes: list = []
            for item in includes:
                if isinstance(item, dict):
                    if 'project' in item:
                        remaining_project_includes.append(item)
                        continue
                    elif 'local' in item:
                        local_path = item['local']
                    else:
                        continue
                elif isinstance(item, str):
                    local_path = item
                else:
                    continue

                sub_merged, sub_remaining = resolve(os.path.join(IDF_PATH, local_path.lstrip('/')))
                merged.update(sub_merged)
                remaining_project_includes.extend(sub_remaining)

            # this file's own top-level keys override whatever its includes defined
            merged.update(data)
            return merged, remaining_project_includes

        merged_config, project_includes = resolve(root_yml_filepath)
        if project_includes:
            merged_config['include'] = project_includes

        return yaml.dump(merged_config, Dumper=_Dumper, sort_keys=False)  # type: ignore

    def _compile_via_gitlab_api(self, root_yml_filepath: str) -> str:
        """
        Call the GitLab CI Lint API to get the fully compiled (all `include`s resolved) yml,
        same as what `glab ci config compile` does. This replaces the old recursive local-only
        parsing, since the project now includes configs from other projects as well.

        :param root_yml_filepath: path to the local root yml file to compile
        :return: merged (fully resolved) yml content as a string
        """
        sys.path.insert(0, os.path.join(IDF_PATH, 'tools', 'ci', 'python_packages'))
        import gitlab_api

        content = self._inline_local_includes(root_yml_filepath)

        gitlab_inst = gitlab_api.Gitlab()
        project_id = os.getenv('CI_PROJECT_ID') or gitlab_inst.get_project_id('esp-idf', namespace='espressif')
        project = gitlab_inst.gitlab_inst.projects.get(project_id, lazy=True)

        lint_result = project.ci_lint.create({'content': content})
        if not lint_result.valid:
            raise RuntimeError(f'Failed to compile {root_yml_filepath} via GitLab CI Lint API: {lint_result.errors}')

        return lint_result.merged_yaml  # type: ignore

    @property
    def default(self) -> dict[str, t.Any]:
        return self._defaults

    @property
    def config(self) -> dict[str, t.Any]:
        return self._config

    @cached_property
    def global_keys(self) -> list[str]:
        return ['default', 'include', 'workflow', 'variables', 'stages']

    @cached_property
    def jobs(self) -> dict[str, t.Any]:
        return {k: v for k, v in self.config.items() if not k.startswith('.') and k not in self.global_keys}


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
