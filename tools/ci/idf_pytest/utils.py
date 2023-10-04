# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import os
import typing as t
from xml.etree import ElementTree as ET

from .constants import TARGET_MARKERS


def format_case_id(target: t.Optional[str], config: t.Optional[str], case: str, is_qemu: bool = False, params: t.Optional[dict] = None) -> str:
    parts = []
    if target:
        parts.append((str(target) + '_qemu') if is_qemu else str(target))
    if config:
        parts.append(str(config))
    parts.append(case)

    if params:
        parts.append(str(params))

    return '.'.join(parts)


def get_target_marker_from_expr(markexpr: str) -> str:
    candidates = set()
    # we use `-m "esp32 and generic"` in our CI to filter the test cases
    # this doesn't cover all use cases, but fit what we do in CI.
    for marker in markexpr.split('and'):
        marker = marker.strip()
        if marker in TARGET_MARKERS:
            candidates.add(marker)

    if len(candidates) > 1:
        raise ValueError(f'Specified more than one target markers: {candidates}. Please specify no more than one.')
    elif len(candidates) == 1:
        return candidates.pop()
    else:
        raise ValueError('Please specify one target marker via "--target [TARGET]" or via "-m [TARGET]"')


def merge_junit_files(junit_files: t.List[str], target_path: str) -> None:
    if len(junit_files) <= 1:
        return

    merged_testsuite: ET.Element = ET.Element('testsuite')
    testcases: t.Dict[str, ET.Element] = {}
    for junit in junit_files:
        logging.info(f'Merging {junit} to {target_path}')
        tree: ET.ElementTree = ET.parse(junit)
        testsuite: ET.Element = tree.getroot()

        for testcase in testsuite.findall('testcase'):
            name: str = testcase.get('name') if testcase.get('name') else ''  # type: ignore

            if name not in testcases:
                testcases[name] = testcase
                merged_testsuite.append(testcase)
                continue

            existing_testcase = testcases[name]
            for element_name in ['failure', 'error']:
                for element in testcase.findall(element_name):
                    existing_element = existing_testcase.find(element_name)
                    if existing_element is None:
                        existing_testcase.append(element)
                    else:
                        existing_element.attrib.setdefault('message', '')  # type: ignore
                        existing_element.attrib['message'] += '. ' + element.get('message', '')  # type: ignore

        os.remove(junit)

    merged_testsuite.set('tests', str(len(merged_testsuite.findall('testcase'))))
    merged_testsuite.set('failures', str(len(merged_testsuite.findall('.//testcase/failure'))))
    merged_testsuite.set('errors', str(len(merged_testsuite.findall('.//testcase/error'))))
    merged_testsuite.set('skipped', str(len(merged_testsuite.findall('.//testcase/skipped'))))

    with open(target_path, 'wb') as fw:
        fw.write(ET.tostring(merged_testsuite))
