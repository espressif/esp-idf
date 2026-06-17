#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
Pre-commit check: ensure every header defining ESP_ERR_* codes is registered
via idf_define_esp_err_codes() in its component's CMakeLists.txt.

Compares the set of headers found by scanning all components against those
explicitly listed in idf_define_esp_err_codes(HEADERS ...) calls.
"""

import os
import re
import sys

# Add tools/ to path so we can import err_codes_extract
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))
from err_codes_extract import EXCLUDE_SEARCH_DIRS
from err_codes_extract import extract_from_file

# Additional directories to exclude beyond those in err_codes_extract
EXTRA_EXCLUDE_DIRS = {'remote'}

# Pattern matching idf_define_esp_err_codes(HEADERS ...) in CMakeLists.txt
ERR_CODES_CALL = re.compile(r'idf_define_esp_err_codes\s*\(')


def find_all_err_headers(components_dir: str) -> dict[str, set[str]]:
    """
    Scan all component headers for ESP_ERR_* definitions using
    err_codes_extract.extract_from_file().

    Returns:
        dict mapping component name -> set of header paths (relative to component dir)
    """
    exclude = EXCLUDE_SEARCH_DIRS | EXTRA_EXCLUDE_DIRS
    result: dict[str, set[str]] = {}

    for component_name in sorted(os.listdir(components_dir)):
        component_dir = os.path.join(components_dir, component_name)
        if not os.path.isdir(component_dir):
            continue

        for root, dirnames, filenames in os.walk(component_dir, topdown=True):
            dirnames[:] = [d for d in dirnames if d not in exclude]
            for filename in filenames:
                if not filename.endswith('.h'):
                    continue
                filepath = os.path.join(root, filename)
                rel_path = os.path.relpath(filepath, component_dir)
                if extract_from_file(filepath, rel_path):
                    result.setdefault(component_name, set()).add(rel_path)

    return result


def parse_registered_headers(components_dir: str) -> dict[str, set[str]]:
    """
    Parse all CMakeLists.txt files for idf_define_esp_err_codes(HEADERS ...) calls.

    Returns:
        dict mapping component name -> set of header paths (relative to component dir)
    """
    result: dict[str, set[str]] = {}

    for component_name in sorted(os.listdir(components_dir)):
        component_dir = os.path.join(components_dir, component_name)
        cmake_file = os.path.join(component_dir, 'CMakeLists.txt')
        if not os.path.isfile(cmake_file):
            continue

        try:
            with open(cmake_file, encoding='utf-8') as f:
                content = f.read()
        except (UnicodeDecodeError, OSError):
            continue

        # Find all idf_define_esp_err_codes() calls and extract HEADERS args
        for match in ERR_CODES_CALL.finditer(content):
            start = match.end()
            # Find the matching closing paren, handling nesting
            depth = 1
            pos = start
            while pos < len(content) and depth > 0:
                if content[pos] == '(':
                    depth += 1
                elif content[pos] == ')':
                    depth -= 1
                pos += 1

            args_str = content[start : pos - 1]
            # Remove comments
            args_str = re.sub(r'#.*$', '', args_str, flags=re.MULTILINE)
            # Extract tokens after HEADERS keyword
            headers_match = re.search(r'\bHEADERS\b\s+(.*)', args_str, re.DOTALL)
            if headers_match:
                tokens = headers_match.group(1).split()
                # Stop at next keyword (uppercase word) or end
                for token in tokens:
                    if re.match(r'^[A-Z_]+$', token):
                        break
                    # Normalize path separators
                    header_path = token.replace('\\', '/')
                    result.setdefault(component_name, set()).add(header_path)

    return result


def main() -> int:
    idf_path = os.environ.get('IDF_PATH', os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
    components_dir = os.path.join(idf_path, 'components')

    if not os.path.isdir(components_dir):
        print(f'Error: components directory not found: {components_dir}', file=sys.stderr)
        return 1

    found_headers = find_all_err_headers(components_dir)
    registered_headers = parse_registered_headers(components_dir)

    errors: list[str] = []

    for component, headers in sorted(found_headers.items()):
        registered = registered_headers.get(component, set())
        missing = headers - registered
        if missing:
            for header in sorted(missing):
                errors.append(
                    f'{component}: {header} defines ESP_ERR_* codes but is not registered in idf_define_esp_err_codes()'
                )

    if errors:
        print('Error code registration check failed:', file=sys.stderr)
        for error in errors:
            print(f'  {error}', file=sys.stderr)
        print(
            f'\n{len(errors)} header(s) define ESP_ERR_* codes but are not registered.\n'
            f"Add idf_define_esp_err_codes(HEADERS ...) to the component's CMakeLists.txt.",
            file=sys.stderr,
        )
        return 1

    return 0


if __name__ == '__main__':
    sys.exit(main())
