# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import typing as t

import yaml

from .models import Job


def dump_jobs_to_yaml(
    jobs: t.List[Job], output_filepath: str, extra_include_yml: t.Optional[t.List[str]] = None
) -> None:
    yaml_dict = {}
    for job in jobs:
        yaml_dict.update(job.to_dict())

    # global stuffs
    yaml_dict.update(
        {
            'include': [
                'tools/ci/dynamic_pipelines/templates/.dynamic_jobs.yml',
                '.gitlab/ci/common.yml',
            ],
            # https://gitlab.com/gitlab-org/gitlab/-/issues/222370#note_662695503
            'workflow': {
                'rules': [
                    {'if': '$CI_MERGE_REQUEST_IID'},
                    {'if': '$CI_COMMIT_BRANCH'},
                ],
            },
        }
    )
    yaml_dict['include'].extend(extra_include_yml or [])

    with open(output_filepath, 'w') as fw:
        yaml.dump(yaml_dict, fw, indent=2)
