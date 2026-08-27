# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Unit tests for patch_buildv2_child_pipeline.patch()."""

import importlib.util
from pathlib import Path
from typing import Any

import yaml

_SCRIPT = Path(__file__).parent.parent / 'scripts' / 'patch_buildv2_child_pipeline.py'
_spec = importlib.util.spec_from_file_location('patch_buildv2_child_pipeline', _SCRIPT)
assert _spec is not None and _spec.loader is not None
patch_mod: Any = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(patch_mod)


def _write_and_patch(tmp_path: Path, content: dict[str, Any]) -> dict[str, Any]:
    p = tmp_path / 'buildv2_child_pipeline.yml'
    p.write_text(yaml.safe_dump(content, sort_keys=False))
    patch_mod.patch(str(p))
    result: dict[str, Any] = yaml.safe_load(p.read_text())
    return result


def test_fake_pass_skip_pipeline_is_left_untouched(tmp_path: Path) -> None:
    # idf-ci emits a skip pipeline with only a `fake_pass` job when no apps match
    # the modified files. There is no included generate_pytest_child_pipeline to
    # redefine, so the patch must leave the pipeline untouched and let it skip.
    # Mirrors idf_ci.idf_gitlab.pipeline._get_fake_pass_job(): the sole job is
    # `fake_pass` and no test_child_pipeline.yml is included.
    skip_pipeline = {
        'fake_pass': {
            'stage': 'build',
            'tags': ['build', 'shiny'],
            'before_script': [],
            'after_script': [],
            'cache': [],
            'needs': [],
            'script': ['echo "skip the entire child pipeline"'],
        },
        'workflow': {'name': 'build_child_pipeline', 'rules': [{'when': 'always'}]},
    }

    result = _write_and_patch(tmp_path, dict(skip_pipeline))

    assert 'generate_pytest_child_pipeline' not in result
    assert result == skip_pipeline


def test_build_jobs_get_buildv2_injection(tmp_path: Path) -> None:
    # A real pipeline (apps matched) carries build jobs that `extends` the dynamic
    # template. Those must get IDF_BUILD_V2 / PIPELINE_COMMIT_SHA injected and
    # generate_pytest_child_pipeline redefined with the post-processing step.
    pipeline = {
        'build_test_related_apps': {
            'extends': '.dynamic_build_template',
            'needs': [{'pipeline': '$PARENT_PIPELINE_ID', 'job': 'generate_build_child_pipeline'}],
        },
    }

    result = _write_and_patch(tmp_path, pipeline)

    job = result['build_test_related_apps']
    assert job['variables']['IDF_BUILD_V2'] == '1'
    assert job['variables']['PIPELINE_COMMIT_SHA'] == patch_mod.PIPELINE_COMMIT_SHA_V2
    assert 'generate_pytest_child_pipeline' in result
    assert result['generate_pytest_child_pipeline']['variables']['IDF_BUILD_V2'] == '1'
