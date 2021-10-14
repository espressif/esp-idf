#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import json
import os
from sys import stdin

try:
    import jsonschema
except ImportError:
    raise RuntimeError('You need to install jsonschema package to use validate command')

input_json = ''
for line in stdin:
    input_json += line
size_json = json.loads(input_json)
with open(os.path.join(os.path.dirname(__file__), 'size_schema.json'), 'r') as schema_file:
    schema_json = json.load(schema_file)
jsonschema.validate(size_json, schema_json)
print(input_json.strip('\n'))
