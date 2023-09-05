# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import io
import typing as t
from contextlib import redirect_stdout
from pathlib import Path

import pytest
from _pytest.config import ExitCode
from idf_py_actions.constants import PREVIEW_TARGETS as TOOLS_PREVIEW_TARGETS
from idf_py_actions.constants import SUPPORTED_TARGETS as TOOLS_SUPPORTED_TARGETS
from pytest_embedded.utils import to_list

from .constants import PytestCase
from .plugin import IdfPytestEmbedded


def get_pytest_files(paths: t.List[str]) -> t.List[str]:
    # this is a workaround to solve pytest collector super slow issue
    # benchmark with
    # - time pytest -m esp32 --collect-only
    #   user=15.57s system=1.35s cpu=95% total=17.741
    # - time { find -name 'pytest_*.py'; } | xargs pytest -m esp32 --collect-only
    #   user=0.11s system=0.63s cpu=36% total=2.044
    #   user=1.76s system=0.22s cpu=43% total=4.539
    # use glob.glob would also save a bunch of time
    pytest_scripts: t.Set[str] = set()
    for p in paths:
        path = Path(p)
        pytest_scripts.update(str(_p) for _p in path.glob('**/pytest_*.py') if 'managed_components' not in _p.parts)

    return list(pytest_scripts)


def get_pytest_cases(
    paths: t.Union[str, t.List[str]],
    target: str = 'all',
    marker_expr: t.Optional[str] = None,
    filter_expr: t.Optional[str] = None,
) -> t.List[PytestCase]:
    if target == 'all':
        targets = TOOLS_SUPPORTED_TARGETS + TOOLS_PREVIEW_TARGETS
    else:
        targets = [target]

    paths = to_list(paths)

    cases: t.List[PytestCase] = []
    pytest_scripts = get_pytest_files(paths)  # type: ignore
    if not pytest_scripts:
        print(f'WARNING: no pytest scripts found for target {target} under paths {", ".join(paths)}')
        return cases

    for target in targets:
        collector = IdfPytestEmbedded(target)

        with io.StringIO() as buf:
            with redirect_stdout(buf):
                cmd = ['--collect-only', *pytest_scripts, '--target', target, '-q']
                if marker_expr:
                    cmd.extend(['-m', marker_expr])
                if filter_expr:
                    cmd.extend(['-k', filter_expr])
                res = pytest.main(cmd, plugins=[collector])

            if res.value != ExitCode.OK:
                if res.value == ExitCode.NO_TESTS_COLLECTED:
                    print(f'WARNING: no pytest app found for target {target} under paths {", ".join(paths)}')
                else:
                    print(buf.getvalue())
                    raise RuntimeError(f'pytest collection failed at {", ".join(paths)} with command \"{" ".join(cmd)}\"')

        cases.extend(collector.cases)

    return cases
