#!/usr/bin/env python
# SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
Check gitlab ci yaml files
"""

import argparse
import os
from functools import cached_property

from idf_ci_utils import IDF_PATH
from idf_ci_utils import GitlabYmlConfig
from idf_ci_utils import get_submodule_dirs


class YmlLinter:
    def __init__(self, yml_config: GitlabYmlConfig) -> None:
        self.yml_config = yml_config

        self._errors: list[str] = []

    @cached_property
    def lint_functions(self) -> list[str]:
        funcs = []
        for func in dir(self):
            if func.startswith('_lint_'):
                funcs.append(func)

        return funcs

    def lint(self) -> None:
        exit_code = 0

        for func in self.lint_functions:
            getattr(self, func)()

            if self._errors:
                print(f'Errors found while running {func}:')
                exit_code = 1
                print('\t- ' + '\n\t- '.join(self._errors))
                self._errors = []  # reset

        exit(exit_code)

    def _lint_default_values_artifacts(self) -> None:
        defaults_artifacts = self.yml_config.default.get('artifacts', {})

        for job_name, d in self.yml_config.jobs.items():
            for k, v in d.get('artifacts', {}).items():
                if k not in defaults_artifacts:
                    continue

                if v == defaults_artifacts[k]:
                    self._errors.append(f'job {job_name} key {k} has same value as default value {v}')

    def _lint_submodule_patterns(self) -> None:
        submodule_paths = sorted(['.gitmodules'] + get_submodule_dirs())
        submodule_paths_in_patterns = sorted(self.yml_config.config.get('.patterns-submodule', []))

        if submodule_paths != submodule_paths_in_patterns:
            unused_patterns = set(submodule_paths_in_patterns) - set(submodule_paths)
            if unused_patterns:
                for item in unused_patterns:
                    self._errors.append(f'non-exist pattern {item}. Please remove {item} from .patterns-submodule')
            undefined_patterns = set(submodule_paths) - set(submodule_paths_in_patterns)
            if undefined_patterns:
                for item in undefined_patterns:
                    self._errors.append(f'undefined pattern {item}. Please add {item} to .patterns-submodule')

    def _lint_dependencies_and_needs(self) -> None:
        """
        Use `dependencies: []` together with `needs: []` could cause missing artifacts issue.
        """
        for job_name, d in self.yml_config.jobs.items():
            if 'dependencies' in d and 'needs' in d:
                if d['dependencies'] is not None and d['needs']:
                    self._errors.append(
                        f'job {job_name} has both `dependencies` and `needs` defined. '
                        f'Please set `dependencies:` (to null) explicitly to avoid missing artifacts issue'
                    )

    def _lint_artifacts_expire_in_and_when(self) -> None:
        """
        Set `artifacts: expire_in` and `artifacts: when` together since gitlab has bugs:

        - https://gitlab.com/gitlab-org/gitlab/-/issues/404563 (expire_in)
        - https://gitlab.com/gitlab-org/gitlab/-/issues/440672 (when)
        """
        for job_name, d in self.yml_config.jobs.items():
            if 'artifacts' in d:
                if 'expire_in' not in d['artifacts']:
                    self._errors.append(f'job {job_name} missing `artifacts: expire_in`. (suggest to set to `1 week`)')

                if 'when' not in d['artifacts']:
                    self._errors.append(f'job {job_name} missing `artifacts: when`. (suggest to set to `always`)')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--root-yml-filepath', help='root yml file path', default=os.path.join(IDF_PATH, '.gitlab-ci.yml')
    )
    args = parser.parse_args()

    config = GitlabYmlConfig(args.root_yml_filepath)
    linter = YmlLinter(config)
    linter.lint()
