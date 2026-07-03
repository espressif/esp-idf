# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Patch the generated Build system v2 target_test child pipeline in place.

Inject ``PIPELINE_COMMIT_SHA: ${PIPELINE_COMMIT_SHA}_buildv2`` into every
target_test job so idf-ci downloads the cmakev2 binaries from the distinct s3
key namespace produced by the v2 build jobs, instead of racing with the default
pipeline's binaries.

Run from generate_pytest_child_pipeline (see patch_buildv2_child_pipeline.py),
after generate_target_test_child_pipeline.py has emitted the YAML.
"""

import argparse

import yaml

PIPELINE_COMMIT_SHA_V2 = '${PIPELINE_COMMIT_SHA}_buildv2'

# Target test jobs use extends: .dynamic_target_test_template, so their script:
# comes from the template, not their own definition. These top-level keys are
# pipeline configuration rather than jobs and must be skipped.
SKIP_KEYS = {'include', 'workflow', 'default', 'variables', 'stages'}


def patch(path: str) -> None:
    with open(path) as f:
        d = yaml.safe_load(f)

    modified = 0
    for k, v in d.items():
        if k in SKIP_KEYS or not isinstance(v, dict):
            continue
        v.setdefault('variables', {})['PIPELINE_COMMIT_SHA'] = PIPELINE_COMMIT_SHA_V2
        modified += 1

    with open(path, 'w') as f:
        yaml.safe_dump(d, f, sort_keys=False)

    print(f'Injected PIPELINE_COMMIT_SHA override into {modified} target_test jobs')


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument(
        'pipeline_yaml',
        nargs='?',
        default='target_test_child_pipeline.yml',
        help='Path to the generated target_test child pipeline YAML to patch in place.',
    )
    args = parser.parse_args()
    patch(args.pipeline_yaml)


if __name__ == '__main__':
    main()
