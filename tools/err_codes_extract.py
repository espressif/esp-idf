# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# Extract ESP_ERR_* error code definitions from C header files and output them as CSV.
# This script is used by the composable error code registration system.
#
# Usage:
#   python err_codes_extract.py --headers file1.h file2.h --output errors.csv
#   python err_codes_extract.py --search-dirs components/ --output errors.csv
#

import argparse
import csv
import fnmatch
import os
import re
import sys

# Directories to exclude from recursive search
EXCLUDE_SEARCH_DIRS = {'test_apps', 'unit-test-app', 'CMock', 'test'}

# Files to exclude
EXCLUDE_FILES: list[str] = []


class ErrCode:
    """Represents a single extracted error code definition."""

    def __init__(
        self,
        name: str,
        file: str,
        value: int | None = None,
        base_name: str = '',
        base_offset: int = 0,
        comment: str = '',
    ) -> None:
        self.name = name
        self.file = file
        self.value = value  # Resolved integer value, or None if unresolved
        self.base_name = base_name  # Name of base error if this is BASE + offset
        self.base_offset = base_offset
        self.comment = comment

    def __repr__(self) -> str:
        if self.value is not None:
            return f'{self.name} = {self.value} (0x{self.value & 0xFFFFFFFF:x}) from {self.file}'
        return f'{self.name} = {self.base_name}+{self.base_offset} from {self.file}'


def _parse_define_value(value_str: str) -> tuple[int | None, str, int]:
    """
    Parse the value part of a #define.

    Returns:
        (resolved_value, base_name, base_offset)
        If the value is a plain number, resolved_value is set and base_name is empty.
        If the value references a base, resolved_value is None and base_name/base_offset are set.
    """
    # Strip surrounding parentheses
    value_str = value_str.strip()
    m = re.match(r'^\((.+)\)$', value_str)
    if m:
        value_str = m.group(1).strip()

    # Check for BASE + offset pattern
    m = re.match(r'(\w+)\s*\+\s*(.+)', value_str)
    if m:
        base_name = m.group(1)
        offset_str = m.group(2).strip()
        # Try to parse offset as hex or decimal
        offset = _parse_number(offset_str)
        if offset is not None:
            return (None, base_name, offset)
        # offset might be another symbol — treat as unresolvable for now
        return (None, base_name, 0)

    # Try to parse as a plain number
    num = _parse_number(value_str)
    if num is not None:
        return (num, '', 0)

    # It might be a reference to another symbol (no offset)
    m = re.match(r'^(\w+)$', value_str)
    if m:
        return (None, m.group(1), 0)

    return (None, '', 0)


def _parse_number(s: str) -> int | None:
    """Parse a hexadecimal or decimal integer from a string."""
    s = s.strip()
    m = re.match(r'^0x([0-9A-Fa-f]+)$', s)
    if m:
        return int(m.group(1), 16)
    m = re.match(r'^(-?[0-9]+)$', s)
    if m:
        return int(m.group(1), 10)
    return None


def extract_from_file(filepath: str, rel_path: str) -> list[ErrCode]:
    """
    Extract ESP_ERR_* defines from a single header file.

    Args:
        filepath: Absolute path to the file.
        rel_path: Relative path for display/CSV output.

    Returns:
        List of ErrCode objects.
    """
    results: list[ErrCode] = []
    define_pattern = re.compile(
        r'^\s*#\s*define\s+(ESP_ERR_\w+|ESP_OK\w*|ESP_FAIL)\s+(.*?)(?:\s*/\*[*!]?<?\s*(.*?)\s*\*/)?(?:\s*/{2,3}\s*!?<?\s*(.*))?$'
    )

    try:
        with open(filepath, encoding='utf-8') as f:
            for line in f:
                line = line.rstrip('\n')
                m = define_pattern.match(line)
                if not m:
                    continue

                name = m.group(1)
                value_str = m.group(2).strip()
                comment = (m.group(3) or m.group(4) or '').strip()

                # Remove trailing inline comments from value_str
                # e.g. "0x101   /*!< Out of memory */" -> "0x101"
                comment_match = re.search(r'\s*/\*[*!]?<?\s*(.*?)\s*\*/', value_str)
                if comment_match:
                    if not comment:
                        comment = comment_match.group(1).strip()
                    value_str = value_str[: comment_match.start()].strip()

                # Handle multi-line comments that start with /*!< but don't close on same line
                comment_match_open = re.search(r'\s*/\*[*!]?<?\s*(.*)', value_str)
                if comment_match_open:
                    if not comment:
                        comment = comment_match_open.group(1).strip().rstrip(',')
                    value_str = value_str[: comment_match_open.start()].strip()

                # Also handle // and /// comments at end
                comment_match2 = re.search(r'\s*/{2,3}\s*!?<?\s*(.*)', value_str)
                if comment_match2:
                    if not comment:
                        comment = comment_match2.group(1).strip()
                    value_str = value_str[: comment_match2.start()].strip()

                resolved, base_name, base_offset = _parse_define_value(value_str)
                results.append(
                    ErrCode(
                        name=name,
                        file=rel_path,
                        value=resolved,
                        base_name=base_name,
                        base_offset=base_offset,
                        comment=comment,
                    )
                )
    except UnicodeDecodeError:
        print(f'Warning: Cannot decode {filepath}, skipping', file=sys.stderr)

    return results


def resolve_error_codes(err_codes: list[ErrCode]) -> list[ErrCode]:
    """
    Resolve error codes that depend on base codes.
    Performs multiple passes to handle transitive dependencies.

    Returns list of resolved ErrCode objects (with .value set).
    Unresolvable codes are printed as warnings and excluded.
    """
    # Build a lookup from name to resolved value
    resolved: dict[str, int] = {}
    unresolved: list[ErrCode] = []

    for ec in err_codes:
        if ec.value is not None:
            resolved[ec.name] = ec.value
        else:
            unresolved.append(ec)

    # Iteratively resolve
    max_iterations = 10
    for _ in range(max_iterations):
        still_unresolved = []
        for ec in unresolved:
            if ec.base_name in resolved:
                ec.value = resolved[ec.base_name] + ec.base_offset
                resolved[ec.name] = ec.value
            else:
                still_unresolved.append(ec)
        if len(still_unresolved) == len(unresolved):
            break  # No progress
        unresolved = still_unresolved

    for ec in unresolved:
        print(f'Warning: Cannot resolve {ec.name} (depends on {ec.base_name}) in {ec.file}', file=sys.stderr)

    return [ec for ec in err_codes if ec.value is not None]


def search_headers(search_dirs: list[str], base_path: str) -> list[str]:
    """
    Recursively find all .h files in the given directories,
    excluding test directories.
    """
    headers: list[str] = []
    for search_dir in search_dirs:
        full_dir = os.path.join(base_path, search_dir) if not os.path.isabs(search_dir) else search_dir
        for root, dirnames, filenames in os.walk(full_dir, topdown=True):
            # Filter out excluded directories
            dirnames[:] = [d for d in dirnames if d not in EXCLUDE_SEARCH_DIRS]
            for filename in fnmatch.filter(filenames, '*.h'):
                headers.append(os.path.join(root, filename))
    return headers


def extract_all(headers: list[str], base_path: str, context_headers: list[str] | None = None) -> list[ErrCode]:
    """
    Extract and resolve error codes from a list of header files.

    Args:
        headers: Headers whose error codes will be emitted.
        base_path: Base path for relative paths.
        context_headers: Additional headers scanned only for base definitions
                         (e.g. ESP_ERR_*_BASE). Their codes are used for
                         resolution but are excluded from the final output.
    """
    context_names: set = set()
    all_codes: list[ErrCode] = []

    # Scan context headers first — their defines help resolve bases
    if context_headers:
        for header in context_headers:
            rel = os.path.relpath(header, base_path) if base_path else header
            ctx_codes = extract_from_file(header, rel)
            for ec in ctx_codes:
                context_names.add(ec.name)
            all_codes.extend(ctx_codes)

    # Scan primary headers
    for header in headers:
        rel = os.path.relpath(header, base_path) if base_path else header
        all_codes.extend(extract_from_file(header, rel))

    resolved = resolve_error_codes(all_codes)

    # Remove context-only codes from the output
    if context_names:
        resolved = [ec for ec in resolved if ec.name not in context_names]

    return resolved


def validate_error_codes(err_codes: list[ErrCode]) -> list[str]:
    """
    Validate extracted error codes and return a list of error messages.
    Returns an empty list if all checks pass.

    Checks performed:
      - No two non-BASE error codes share the same numeric value.
      - No duplicate names with conflicting values.
    """
    errors: list[str] = []

    # --- Check 1: duplicate values (skip _BASE codes, they are offset anchors) ---
    value_to_codes: dict[int, list[ErrCode]] = {}
    for ec in err_codes:
        if ec.value is None or ec.name.endswith('_BASE'):
            continue
        value_to_codes.setdefault(ec.value, []).append(ec)

    for value, codes in sorted(value_to_codes.items()):
        # Deduplicate by name — same name from different files is OK
        unique_names = {ec.name for ec in codes}
        if len(unique_names) > 1:
            names_str = ', '.join(f'{ec.name} ({ec.file})' for ec in codes)
            errors.append(f'Duplicate value 0x{value & 0xFFFFFFFF:x} ({value}): {names_str}')

    # --- Check 2: same name defined with different values ---
    name_to_values: dict[str, list[ErrCode]] = {}
    for ec in err_codes:
        if ec.value is None:
            continue
        name_to_values.setdefault(ec.name, []).append(ec)

    for name, codes in sorted(name_to_values.items()):
        unique_values = {ec.value for ec in codes}
        if len(unique_values) > 1:
            defs_str = ', '.join(f'{ec.value} in {ec.file}' for ec in codes)
            errors.append(f'Conflicting definitions for {name}: {defs_str}')

    return errors


def write_csv(err_codes: list[ErrCode], output_path: str) -> None:
    """Write resolved error codes to a CSV file."""
    # Deduplicate: keep the first occurrence of each name
    seen = set()
    unique_codes: list[ErrCode] = []
    for ec in err_codes:
        if ec.name not in seen:
            seen.add(ec.name)
            unique_codes.append(ec)

    # Sort by value then name
    unique_codes.sort(key=lambda e: (e.value if e.value is not None else 0, e.name))

    with open(output_path, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        writer.writerow(['name', 'value', 'file', 'comment'])
        for ec in unique_codes:
            writer.writerow([ec.name, ec.value, ec.file, ec.comment])


def main() -> None:
    parser = argparse.ArgumentParser(description='Extract ESP error codes from header files and output as CSV')
    parser.add_argument('--headers', nargs='*', default=[], help='Specific header files to process')
    parser.add_argument(
        '--context-headers',
        nargs='*',
        default=[],
        help='Header files scanned only for base definitions (e.g. ESP_ERR_*_BASE). '
        'Codes found here are used for resolution but not emitted.',
    )
    parser.add_argument(
        '--search-dirs', nargs='*', default=[], help='Directories to search recursively for header files'
    )
    parser.add_argument(
        '--base-path', default=None, help='Base path for computing relative paths (default: IDF_PATH or cwd)'
    )
    parser.add_argument('--output', required=True, help='Output CSV file path')
    parser.add_argument(
        '--validate',
        action='store_true',
        help='Validate error codes (fail on duplicate values or conflicting definitions)',
    )

    args = parser.parse_args()

    base_path = args.base_path
    if base_path is None:
        base_path = os.environ.get('IDF_PATH', os.getcwd())

    headers = list(args.headers)

    if args.search_dirs:
        headers.extend(search_headers(args.search_dirs, base_path))

    if not headers:
        print('Error: No header files specified. Use --headers or --search-dirs.', file=sys.stderr)
        sys.exit(1)

    context_headers = list(args.context_headers) if args.context_headers else None

    err_codes = extract_all(headers, base_path, context_headers=context_headers)
    write_csv(err_codes, args.output)

    if args.validate:
        validation_errors = validate_error_codes(err_codes)
        if validation_errors:
            print(f'\nValidation FAILED with {len(validation_errors)} error(s):', file=sys.stderr)
            for err in validation_errors:
                print(f'  ERROR: {err}', file=sys.stderr)
            sys.exit(1)
        else:
            print('Validation passed: no duplicate values or conflicting definitions.')


if __name__ == '__main__':
    main()
