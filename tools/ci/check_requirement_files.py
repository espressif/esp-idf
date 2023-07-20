#!/usr/bin/env python
# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import json
import os
import re
from typing import Any

import jsonschema
from idf_ci_utils import IDF_PATH

JSON_PATH = os.path.join(IDF_PATH, 'tools', 'requirements.json')
SCHEMA_PATH = os.path.join(IDF_PATH, 'tools', 'requirements_schema.json')
REQ_DIR = os.path.join(IDF_PATH, 'tools', 'requirements')
RE_FEATURE = re.compile(r'requirements\.(\w+)\.txt')


def action_validate(req_obj: Any) -> None:  # "Any" because we are checking this in this script
    '''
    Check that the parsed JSON object is valid according to the JSON schema provided
    '''
    with open(SCHEMA_PATH, 'r') as schema_file:
        schema_json = json.load(schema_file)
        jsonschema.validate(req_obj, schema_json)


def action_check_directory(req_obj: Any) -> None:  # "Any" because we are checking this in this script
    '''
    Check that all directory items are listed in the JSON file
    '''
    features = set(d['name'] for d in req_obj['features'])
    features_found = set()

    for file_name in os.listdir(REQ_DIR):
        m = re.match(RE_FEATURE, file_name)
        if m:
            if m.group(1) not in features:
                raise RuntimeError(f'Cannot find a feature for {file_name} in {JSON_PATH}')
            features_found.add(m.group(1))
        else:
            raise RuntimeError(f'{file_name} in {REQ_DIR} doesn\'t match the expected name')

    features_not_found = features - features_found

    if len(features_not_found) > 0:
        raise RuntimeError(f'There are no requirements file in {REQ_DIR} for {features_not_found}')


def main() -> None:
    with open(JSON_PATH, 'r') as f:
        req_obj = json.load(f)
    action_validate(req_obj)
    action_check_directory(req_obj)


if __name__ == '__main__':
    main()
