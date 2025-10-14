# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import typing as t
from xml.etree import ElementTree as ET


def format_case_id(
    target: t.Optional[str], config: t.Optional[str], case: str, is_qemu: bool = False, params: t.Optional[dict] = None
) -> str:
    parts = []
    if target:
        parts.append((str(target) + '_qemu') if is_qemu else str(target))
    if config:
        parts.append(str(config))
    parts.append(case)

    if params:
        parts.append(str(params))

    return '.'.join(parts)


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


def normalize_testcase_file_path(file: str, app_path: t.Union[str, tuple]) -> str:
    """
    Normalize file paths to a consistent format, resolving relative paths based on the `app_path`.

    This function ensures that file paths are correctly resolved and normalized:
    - If `app_path` is a tuple, the function will try each app path in the tuple in order and join the results with ':'.
    - If `app_path` is a string, the file will be resolved using that base path.

    :param file: The original file path, which can be relative, absolute.
    :param app_path: The base app path used to resolve relative paths, which can be a string or tuple.
    :return: A normalized file path with the IDF_PATH prefix removed if applicable.
    """

    def normalize_path(file_path: str, app_path: str) -> str:
        """Helper function to normalize a single path."""
        if not os.path.isabs(file_path):
            resolved_path = os.path.normpath(os.path.join(app_path, file_path.removeprefix('./').removeprefix('../')))
        else:
            resolved_path = os.path.normpath(file_path)

        return resolved_path.replace(f'{os.environ.get("IDF_PATH", "")}', '').replace('/IDF/', '').lstrip('/')

    if isinstance(app_path, tuple):
        normalized_paths = [normalize_path(file, base_path) for base_path in app_path]
        return '|'.join(normalized_paths)

    return normalize_path(file, app_path)
