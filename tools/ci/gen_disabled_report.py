#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import argparse
import fnmatch
import json
import os
import subprocess
import sys
from collections import Counter
from datetime import datetime

from dominate import document
from dominate.tags import a
from dominate.tags import button
from dominate.tags import div
from dominate.tags import h1
from dominate.tags import h3
from dominate.tags import input_
from dominate.tags import label
from dominate.tags import li
from dominate.tags import script
from dominate.tags import span
from dominate.tags import strong
from dominate.tags import style
from dominate.tags import table
from dominate.tags import tbody
from dominate.tags import td
from dominate.tags import th
from dominate.tags import thead
from dominate.tags import tr
from dominate.tags import ul
from dominate.util import raw
from idf_build_apps import App
from idf_build_apps.app import AppDeserializer


def run_idf_build_apps_find(
    output_file: str = 'apps.json',
    verbose: bool = False,
    enable_preview_targets: bool = False,
) -> str:
    """
    Run idf-build-apps find command to get application list

    :param output_file: Output file path
    :param verbose: Whether to enable verbose output
    :param enable_preview_targets: Whether to enable preview targets
    :return: Output file path
    """
    cmd = [
        'idf-build-apps',
        'find',
        '--output-format',
        'json',
        '--include-all-apps',
        '--output',
        output_file,
    ]

    # Add verbose parameter
    if verbose:
        cmd.append('--verbose')

    # Add enable_preview_targets parameter
    if enable_preview_targets:
        cmd.append('--enable-preview-targets')

    print(f'Running command: {" ".join(cmd)}')

    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True)
        print('Command completed successfully')
        print(f'stdout: {result.stdout}')
        if result.stderr:
            print(f'stderr: {result.stderr}')
        return output_file
    except subprocess.CalledProcessError as e:
        print(f'Command failed with exit code {e.returncode}')
        print(f'stdout: {e.stdout}')
        print(f'stderr: {e.stderr}')
        raise


def parse_codeowners(codeowners_path: str) -> list[tuple[str, list[str]]]:
    """
    Parse CODEOWNERS file and return a list of (pattern, codeowners) tuples.
    The list preserves the order of the rules in the file.

    :param codeowners_path: Path to CODEOWNERS file
    :return: List of (pattern, codeowners) tuples
    """
    codeowners_mapping: list[tuple[str, list[str]]] = []

    if not os.path.exists(codeowners_path):
        print(f'Warning: CODEOWNERS file not found at {codeowners_path}')
        return codeowners_mapping

    try:
        with open(codeowners_path, encoding='utf-8') as f:
            for line_num, line in enumerate(f, 1):
                line = line.strip()

                # Skip empty lines and comments
                if not line or line.startswith('#'):
                    continue

                # Parse pattern and codeowners
                parts = line.split()
                if len(parts) < 2:
                    print(f'Warning: Invalid CODEOWNERS line {line_num}: {line}')
                    continue

                pattern = parts[0]
                codeowners = parts[1:]

                # Remove @ symbol and the long prefix from codeowners if present
                codeowners = [owner.lstrip('@').replace('esp-idf-codeowners/', '') for owner in codeowners]

                codeowners_mapping.append((pattern, codeowners))

    except Exception as e:
        print(f'Error reading CODEOWNERS file: {e}')

    return codeowners_mapping


def match_codeowners(app_dir: str, codeowners_mapping: list[tuple[str, list[str]]]) -> list[str]:
    """
    Match app directory against CODEOWNERS patterns, supporting wildcards ('*' and '**').
    The matching traverses up from the app's directory and respects the 'last match wins' rule.

    :param app_dir: Application directory path (e.g., './components/esp_driver_touch_sens/test_apps/touch_sens')
    :param codeowners_mapping: List of (pattern, codeowners) tuples, in file order
    :return: List of matching codeowners from the last matching rule
    """
    # 1. Normalize the app_dir path
    # Remove leading './' and ensure it starts with a single '/'
    if app_dir.startswith('./'):
        app_dir = app_dir[1:]
    if not app_dir.startswith('/'):
        app_dir = '/' + app_dir
    app_dir = os.path.normpath(app_dir)

    # 2. Generate all parent paths
    parent_paths = []
    current_path = app_dir
    while True:
        parent_paths.append(current_path)
        if current_path == '/':
            break
        parent, _ = os.path.split(current_path)
        if parent == current_path:  # Root reached
            break
        current_path = parent

    # 3. Iterate through rules in reverse to find the last match
    for pattern, owners in reversed(codeowners_mapping):
        # The pattern needs to be anchored to the root for matching
        if not pattern.startswith('/'):
            pattern = '/**/' + pattern  # Handles patterns like '*.py' or 'docs' anywhere

        for path in parent_paths:
            if fnmatch.fnmatch(path, pattern) or fnmatch.fnmatch(path + '/', pattern):
                return owners

    # Fallback to the root-level wildcard rule if no other match is found
    for pattern, owners in reversed(codeowners_mapping):
        if pattern == '*':
            return owners

    return []


def add_codeowners_to_apps(apps: list[App], codeowners_mapping: list[tuple[str, list[str]]]) -> dict[str, list[str]]:
    """
    Match apps to codeowners and return a dictionary mapping app paths to their owners.
    If an app does not match any rule, it is assigned to the 'others' group.

    :param apps: List of App objects
    :param codeowners_mapping: List of (pattern, codeowners) tuples, in file order
    :return: A dictionary mapping app.app_dir to a list of codeowners
    """
    app_to_owners_map = {}
    for app in apps:
        matched_owners = match_codeowners(app.app_dir, codeowners_mapping)
        if not matched_owners:
            app_to_owners_map[app.app_dir] = ['others']
        else:
            app_to_owners_map[app.app_dir] = matched_owners
    return app_to_owners_map


def load_apps_from_json(json_file: str) -> list[App]:
    """
    Load application list from JSON file (clean version without codeowners)
    """
    apps = []

    with open(json_file, encoding='utf-8') as f:
        if json_file.endswith('.json'):
            # If it's JSON format, try to parse as array
            try:
                data = json.load(f)
                if isinstance(data, list):
                    for app_data in data:
                        # Handle build_system compatibility: 'idf_cmake' -> 'cmake'
                        if app_data.get('build_system') == 'idf_cmake':
                            app_data['build_system'] = 'cmake'
                        app = AppDeserializer.from_json(json.dumps(app_data))
                        apps.append(app)
                else:
                    # If not an array, try line-by-line parsing
                    f.seek(0)
                    for line in f:
                        line = line.strip()
                        if line:
                            app = AppDeserializer.from_json(line)
                            apps.append(app)
            except json.JSONDecodeError:
                # If JSON parsing fails, try line-by-line parsing
                f.seek(0)
                for line in f:
                    line = line.strip()
                    if line:
                        try:
                            app = AppDeserializer.from_json(line)
                            apps.append(app)
                        except Exception as e:
                            print(f'Warning: Failed to parse line: {line[:100]}... Error: {e}')
        else:
            # If not JSON format, parse line by line
            for line in f:
                line = line.strip()
                if line:
                    try:
                        app = AppDeserializer.from_json(line)
                        apps.append(app)
                    except Exception as e:
                        print(f'Warning: Failed to parse line: {line[:100]}... Error: {e}')

    print(f'Loaded {len(apps)} apps from {json_file}')
    return apps


def generate_disabled_report(apps: list[App], app_to_owners_map: dict[str, list[str]], report_path: str) -> None:
    """Generate disabled report"""
    # Categorize applications
    cant_build_temp = []
    can_build_cant_test_temp = []
    cant_build_not_temp = []
    can_build_cant_test_not_temp = []
    can_test = []
    all_codeowners: set[str] = set()
    all_targets: set[str] = set()
    owner_app_counts: Counter[str] = Counter()
    target_app_counts: Counter[str] = Counter()

    for app in apps:
        # Get owners from the map
        app_owners = app_to_owners_map.get(app.app_dir, [])

        # Update owner app counts
        for owner in app_owners:
            owner_app_counts[owner] += 1

        # Update target app counts
        all_targets.add(app.target)
        target_app_counts[app.target] += 1

        # Check for unsupported build_status values and abort if found
        if app.build_status not in ['should be built', 'disabled']:
            print(
                f'ERROR: Found unsupported build_status "{app.build_status}" for app {app.app_dir}, target {app.target}'
            )
            print(
                'This task only supports "should be built" and "disabled" status. Please fix the build configuration.'
            )
            sys.exit(1)

        # Map build_status to the expected status values
        if app.build_status == 'should be built':
            if app.test_comment is None:
                status = 'can_build_and_test'
            else:
                status = 'can_build_no_test'
        else:  # if app.build_status == 'disabled':
            status = 'cannot_build'

        # Categorize apps based on status and temporary flags
        if status == 'can_build_and_test':
            can_test.append(app)
        elif status == 'cannot_build':
            # Handle cases where build_comment might be None for a disabled app
            build_comment = app.build_comment or 'Reason not specified'
            if 'temporary' in build_comment.lower():
                cant_build_temp.append(app)
            else:
                cant_build_not_temp.append(app)
        elif status == 'can_build_no_test':
            # Handle cases where test_comment might be None
            test_comment = app.test_comment or 'Reason not specified'
            if 'temporary' in test_comment.lower():
                can_build_cant_test_temp.append(app)
            else:
                can_build_cant_test_not_temp.append(app)

        all_codeowners.update(app_owners)

    # Create HTML document
    doc = document(title='Build and Test Status Report')

    def get_css_styles() -> str:
        """Return CSS styles"""
        return """
            body {
                font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
                line-height: 1.6;
                margin: 0;
                padding: 20px;
                background-color: #f5f5f5;
            }
            .container {
                max-width: 1200px;
                margin: 0 auto;
                background-color: white;
                padding: 30px;
                border-radius: 8px;
                box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            }
            h1 {
                color: #333;
                text-align: center;
                margin-bottom: 30px;
                border-bottom: 3px solid #007acc;
                padding-bottom: 10px;
            }
            h2 {
                color: #007acc;
                margin-top: 40px;
                margin-bottom: 20px;
                padding: 10px;
                background-color: #f8f9fa;
                border-left: 4px solid #007acc;
                border-radius: 4px;
            }
            .navigation {
                background-color: #f8f9fa;
                padding: 20px;
                border-radius: 8px;
                margin-bottom: 30px;
                border: 1px solid #dee2e6;
            }
            .navigation h3 {
                margin-top: 0;
                color: #495057;
                margin-bottom: 15px;
            }
            .nav-links {
                display: flex;
                flex-wrap: wrap;
                gap: 10px;
            }
            .nav-link {
                background-color: #007acc;
                color: white;
                padding: 8px 16px;
                border-radius: 20px;
                text-decoration: none;
                font-size: 0.9em;
                transition: background-color 0.3s;
            }
            .nav-link:hover {
                background-color: #005a9e;
            }
            .control-buttons {
                text-align: center;
                margin: 20px 0;
            }
            .control-btn {
                background-color: #28a745;
                color: white;
                border: none;
                padding: 10px 20px;
                border-radius: 5px;
                cursor: pointer;
                margin: 0 10px;
                font-size: 0.9em;
                transition: background-color 0.3s;
            }
            .control-btn:hover {
                background-color: #218838;
            }
            .control-btn.collapse-all {
                background-color: #dc3545;
            }
            .control-btn.collapse-all:hover {
                background-color: #c82333;
            }
            .filters-wrapper {
                display: flex;
                gap: 20px;
                margin-bottom: 20px;
                justify-content: center;
            }
            .filter-container {
                flex: 1;
                max-width: 500px;
                margin-bottom: 20px;
                text-align: center;
                padding: 15px;
                border: 1px solid #dee2e6;
                border-radius: 8px;
                background-color: #f8f9fa;
            }
            #codeownerFilter {
                padding: 10px;
                border-radius: 5px;
                border: 1px solid #ccc;
                font-size: 1em;
            }
            table {
                width: 100%;
                border-collapse: collapse;
                margin-bottom: 30px;
                box-shadow: 0 2px 8px rgba(0,0,0,0.1);
            }
            th, td {
                padding: 12px;
                text-align: left;
                border-bottom: 1px solid #ddd;
            }
            th {
                background-color: #007acc;
                color: white;
                font-weight: 600;
            }
            tr:nth-child(even) {
                background-color: #f8f9fa;
            }
            tr:hover {
                background-color: #e3f2fd;
            }
            .summary {
                background-color: #e8f5e8;
                padding: 20px;
                border-radius: 8px;
                margin-bottom: 30px;
                border-left: 4px solid #4caf50;
            }
            .summary h3 {
                margin-top: 0;
                color: #2e7d32;
            }
            .summary ul {
                margin: 10px 0;
                padding-left: 20px;
            }
            .summary li {
                margin: 5px 0;
            }
            .category-summary {
                background-color: #f8f9fa;
                padding: 15px;
                border-radius: 6px;
                margin-bottom: 20px;
                border-left: 4px solid #007acc;
            }
            .category-summary h3 {
                margin: 0;
                color: #007acc;
                display: flex;
                justify-content: space-between;
                align-items: center;
            }
            .category-summary .count {
                background-color: #007acc;
                color: white;
                padding: 4px 8px;
                border-radius: 12px;
                font-size: 0.9em;
                font-weight: bold;
            }
            .category-details {
                margin-left: 20px;
                margin-bottom: 30px;
            }
            .directory-info {
                background-color: #fff3cd;
                padding: 10px;
                border-radius: 4px;
                margin-bottom: 15px;
                border-left: 3px solid #ffc107;
                font-family: monospace;
                font-size: 0.9em;
                cursor: pointer;
                transition: background-color 0.3s;
            }
            .directory-info:hover {
                background-color: #ffeaa7;
            }
            .total-count {
                color: #666;
                font-size: 0.9em;
                margin-left: 10px;
            }
            .empty-section {
                text-align: center;
                color: #666;
                font-style: italic;
                padding: 20px;
                background-color: #f8f9fa;
                border-radius: 4px;
            }
            .timestamp {
                text-align: center;
                color: #666;
                font-size: 0.9em;
                margin-bottom: 30px;
            }
            .table-container {
                margin-bottom: 20px;
            }
            .table-header {
                background-color: #f8f9fa;
                padding: 10px 15px;
                border-radius: 4px;
                cursor: pointer;
                border: 1px solid #dee2e6;
                transition: background-color 0.2s;
            }
            .table-header:hover {
                background-color: #e9ecef;
            }
            .table-header h4 {
                margin: 0;
                display: flex;
                justify-content: space-between;
                align-items: center;
                color: #495057;
            }
            .toggle-icon {
                font-size: 0.8em;
                transition: transform 0.2s;
            }
            .table-content {
                margin-top: 10px;
            }
            .app-row {
                display: table-row;
            }
            .checkbox-group {
                display: flex;
                flex-wrap: wrap;
                gap: 15px;
                justify-content: center;
                margin-top: 10px;
            }
            .checkbox-label {
                display: flex;
                align-items: center;
                cursor: pointer;
                font-size: 0.9em;
            }
            .checkbox-label input[type="checkbox"] {
                margin-right: 5px;
            }
            .owner-label {
                margin-left: 10px;
                font-size: 0.9em;
                color: #555;
            }
        """

    def create_summary_section() -> None:
        """Create summary section"""
        with div(cls='summary'):
            h3('Summary')
            with ul():
                li(strong('Total apps analyzed: '), str(len(apps)))
                li(strong('Build temporarily disabled: '), str(len(cant_build_temp)))
                li(strong('Test temporarily disabled: '), str(len(can_build_cant_test_temp)))
                li(strong('Build disabled permanently: '), str(len(cant_build_not_temp)))
                li(strong('Test disabled permanently: '), str(len(can_build_cant_test_not_temp)))
                li(strong('Normal: '), str(len(can_test)))

    def create_navigation_section() -> None:
        """Create navigation section with links to each category"""
        with div(cls='navigation'):
            h3('Quick Navigation')
            with div(cls='nav-links'):
                for category_id, title, _, _ in categories:
                    a(title, href=f'#{category_id}', cls='nav-link')

    def create_filter_group(
        title: str,
        filter_type: str,
        items: list[str],
        counts: Counter,
        total_count: int,
    ) -> None:
        """Create a generic filter checkbox group with app counts"""
        with div(cls='filter-container'):
            strong(title, style='display: block; margin-bottom: 10px;')
            with div(id=f'{filter_type}Filter', cls='checkbox-group'):
                # 'All' checkbox
                with label(cls='checkbox-label'):
                    input_(
                        type='checkbox',
                        id=f'{filter_type}-all',
                        value='all',
                        onchange=f'toggleAll(this, "{filter_type}")',
                        checked=True,
                    )
                    span(
                        f'All ({total_count})',
                        style='margin-left: 5px; font-weight: bold;',
                    )

                # Individual item checkboxes
                for item in items:
                    count = counts.get(item, 0)
                    with label(cls='checkbox-label'):
                        input_(
                            type='checkbox',
                            cls=f'{filter_type}-checkbox',
                            value=item,
                            onchange='applyFilters()',
                            checked=True,
                        )
                        span(f'{item} ({count})', style='margin-left: 5px;')

    def create_control_buttons() -> None:
        """Create control buttons for expand/collapse all"""
        with div(cls='control-buttons'):
            button('Expand All', onclick='expandAll()', cls='control-btn')
            button('Collapse All', onclick='collapseAll()', cls='control-btn collapse-all')

    def create_category_section(
        category_id: str,
        title: str,
        apps: list[App],
        all_apps: list[App],
        show_status: bool = False,
    ) -> None:
        """Create category section"""
        with div(cls='category-summary'):
            with h3():
                span(title)
                span(str(len(apps)), cls='count')

        # Always show section content (no collapse)
        with div(cls='category-details', id=category_id, style='display: block;'):
            if apps:
                # Group by full directory path (preserve last level directory)
                dir_groups: dict[str, list[App]] = {}
                for app in apps:
                    # Use full path as grouping key
                    dir_name = app.app_dir
                    if dir_name not in dir_groups:
                        dir_groups[dir_name] = []
                    dir_groups[dir_name].append(app)

                # Calculate total apps for each directory across all categories
                total_apps_by_dir = {}
                for app in all_apps:
                    dir_name = app.app_dir
                    if dir_name not in total_apps_by_dir:
                        total_apps_by_dir[dir_name] = 0
                    total_apps_by_dir[dir_name] += 1

                for dir_name, apps_in_dir in dir_groups.items():
                    total_apps = total_apps_by_dir.get(dir_name, len(apps_in_dir))

                    # Determine the owners for this directory group
                    dir_owners = set()
                    for app in apps_in_dir:
                        dir_owners.update(app_to_owners_map.get(app.app_dir, []))
                    owners_str = ', '.join(sorted(list(dir_owners)))

                    with div(cls='directory-info', onclick=f"toggleTable('{category_id}-{hash(dir_name)}')"):
                        span(
                            '+',
                            cls='toggle-icon',
                            style='float: left; color: #666; margin-right: 8px; font-weight: bold;',
                        )
                        strong(f'{dir_name.replace("./", "")}')
                        span(f' ({len(apps_in_dir)}/{total_apps} apps)', style='color: #666;')
                        span(f'Owners: {owners_str}', cls='owner-label')

                    # Table can be collapsed, controlled by table header row
                    with div(cls='table-container'):
                        with div(cls='table-content', id=f'{category_id}-{hash(dir_name)}', style='display: none;'):
                            with table():
                                with thead(cls='table-header'):
                                    with tr():
                                        th('Target')
                                        th('Config')
                                        if show_status:
                                            th('Status')
                                        else:
                                            th('Reason')
                                with tbody():
                                    for app in apps_in_dir:
                                        app_owners = app_to_owners_map.get(app.app_dir, [])
                                        with tr(
                                            cls='app-row',
                                            data_codeowners=','.join(app_owners),
                                        ):
                                            td(app.target)
                                            td(app.config_name or '-')
                                            if show_status:
                                                td('Success')
                                            else:
                                                reason = (
                                                    app.build_comment
                                                    if app.build_status == 'disabled'
                                                    else app.test_comment
                                                )
                                                td(reason or 'Reason not specified')
            else:
                div('No apps in this category', cls='empty-section')

    def get_javascript() -> str:
        """Return JavaScript code"""
        return """
            function toggleTable(id) {
                const content = document.getElementById(id);
                if (!content) {
                    console.error('Content element not found:', id);
                    return;
                }

                const directoryInfo = content.parentElement.previousElementSibling;
                if (!directoryInfo) {
                    console.error('Directory info element not found');
                    return;
                }

                const icon = directoryInfo.querySelector('.toggle-icon');

                if (content.style.display === 'none' || content.style.display === '') {
                    content.style.display = 'block';
                    if (icon) icon.textContent = '-';
                } else {
                    content.style.display = 'none';
                    if (icon) icon.textContent = '+';
                }
            }

            function expandAll() {
                const allContents = document.querySelectorAll('.table-content');
                const allIcons = document.querySelectorAll('.toggle-icon');

                allContents.forEach(content => {
                    content.style.display = 'block';
                });

                allIcons.forEach(icon => {
                    icon.textContent = '-';
                });
            }

            function collapseAll() {
                const allContents = document.querySelectorAll('.table-content');
                const allIcons = document.querySelectorAll('.toggle-icon');

                allContents.forEach(content => {
                    content.style.display = 'none';
                });

                allIcons.forEach(icon => {
                    icon.textContent = '+';
                });
            }

            function applyFilters() {
                // Get selected codeowners
                const selectedCodeowners = Array.from(
                    document.querySelectorAll('.codeowner-checkbox:checked')
                ).map(cb => cb.value);

                // Get selected targets
                const selectedTargets = Array.from(
                    document.querySelectorAll('.target-checkbox:checked')
                ).map(cb => cb.value);

                // Update 'All' checkbox states
                const allCodeownerCheckbox = document.getElementById('codeowner-all');
                const allCodeownerCheckboxes = document.querySelectorAll('.codeowner-checkbox');
                allCodeownerCheckbox.checked = selectedCodeowners.length === allCodeownerCheckboxes.length;

                const allTargetCheckbox = document.getElementById('target-all');
                const allTargetCheckboxes = document.querySelectorAll('.target-checkbox');
                allTargetCheckbox.checked = selectedTargets.length === allTargetCheckboxes.length;

                // Filter rows and hide empty directory sections
                const directorySections = document.querySelectorAll('.directory-info');
                directorySections.forEach(section => {
                    const tableContainer = section.nextElementSibling;
                    if (!tableContainer) return;

                    const rows = tableContainer.querySelectorAll('.app-row');
                    let visibleRows = 0;
                    rows.forEach(row => {
                        const codeowners = row.getAttribute('data-codeowners').split(',');
                        // Assumes target is the first cell
                        const target = row.querySelector('td:first-child').textContent;

                        const codeownerMatch = selectedCodeowners.some(owner => codeowners.includes(owner));
                        const targetMatch = selectedTargets.includes(target);

                        if (codeownerMatch && targetMatch) {
                            row.style.display = 'table-row';
                            visibleRows++;
                        } else {
                            row.style.display = 'none';
                        }
                    });

                    // Hide the entire directory section if no rows are visible
                    if (visibleRows === 0) {
                        section.style.display = 'none';
                        tableContainer.style.display = 'none';
                    } else {
                        section.style.display = 'block';
                        tableContainer.style.display = 'block';
                    }
                });
            }

            function toggleAll(checkbox, filterType) {
                const allCheckboxes = document.querySelectorAll(`.${filterType}-checkbox`);
                allCheckboxes.forEach(cb => {
                    cb.checked = checkbox.checked;
                });
                applyFilters(); // Apply filters immediately
            }

            // Initial filter call to show all rows at the beginning
            document.addEventListener('DOMContentLoaded', function() {
                applyFilters();
            });
        """

    # Define category configuration
    categories = [
        ('cant-build-temp', '1. Build temporarily disabled', cant_build_temp, False),
        ('can-build-cant-test-temp', '2. Test temporarily disabled', can_build_cant_test_temp, False),
        ('cant-build-not-temp', '3. Build disabled permanently', cant_build_not_temp, False),
        ('can-build-cant-test-not_temp', '4. Test disabled permanently', can_build_cant_test_not_temp, False),
        ('can-test', '5. Normal', can_test, True),
    ]

    with doc.head:
        style(get_css_styles())
        script(raw(get_javascript()))

    with doc:
        with div(cls='container'):
            h1('Build and Test Status Report')
            div(f'Generated at: {datetime.now().strftime("%Y-%m-%d %H:%M:%S")}', cls='timestamp')

            # Create summary section
            create_summary_section()

            # Create navigation section
            create_navigation_section()

            # Create filter section and pass owner counts
            with div(cls='filters-wrapper'):
                create_filter_group(
                    'Filter by Codeowner:',
                    'codeowner',
                    sorted(list(all_codeowners)),
                    owner_app_counts,
                    len(apps),
                )
                create_filter_group(
                    'Filter by Target:',
                    'target',
                    sorted(list(all_targets)),
                    target_app_counts,
                    len(apps),
                )

            # Create control buttons
            create_control_buttons()

            # Generate each category using configuration
            for category_id, title, apps_list, show_status in categories:
                create_category_section(category_id, title, apps_list, apps, show_status)

    # Write to file
    with open(report_path, 'w', encoding='utf-8') as f:
        f.write(doc.render())


def main() -> int:
    parser = argparse.ArgumentParser(
        description='Generate a report of disabled and skipped builds/tests',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Basic usage (uses default paths: examples, tools/test_apps, components)
  python gen_disabled_report.py --output report.html

  # Use existing JSON file
  python gen_disabled_report.py --input apps.json --output report.html

  # Enable verbose output
  python gen_disabled_report.py --input apps.json --output report.html --verbose
        """,
    )

    # Input options
    parser.add_argument('--input', type=str, help='Input JSON file containing apps data')

    # Output options
    parser.add_argument(
        '--output',
        type=str,
        default='disabled_report.html',
        help='Output report file path (default: disabled_report.html)',
    )

    # idf-build-apps find options
    parser.add_argument(
        '--temp-json',
        type=str,
        default='apps.json',
        help='Temporary JSON file path for idf-build-apps find output (default: apps.json)',
    )
    parser.add_argument('--verbose', action='store_true', help='Enable verbose output')
    parser.add_argument('--enable-preview-targets', action='store_true', help='Enable preview targets')

    args = parser.parse_args()

    try:
        # Always run idf-build-apps find to get the most up-to-date data
        print('Running idf-build-apps find to generate app list...')
        input_file = run_idf_build_apps_find(
            output_file=args.temp_json,
            verbose=args.verbose,
            enable_preview_targets=args.enable_preview_targets,
        )

        # Load application data
        print(f'Loading apps from {input_file}...')
        apps = load_apps_from_json(input_file)

        # Add codeowners information
        print('Adding codeowners information...')
        idf_path = os.environ.get('IDF_PATH')
        if not idf_path:
            raise ValueError('IDF_PATH environment variable is not set')
        codeowners_path = os.path.join(idf_path, '.gitlab', 'CODEOWNERS')
        codeowners_mapping = parse_codeowners(codeowners_path)
        app_to_owners_map = add_codeowners_to_apps(apps, codeowners_mapping)

        # Generate report
        print(f'Generating report to {args.output}...')
        generate_disabled_report(apps, app_to_owners_map, args.output)

        print(f'Report generated successfully: {args.output}')

        return 0

    except Exception as e:
        print(f'Error: {e}')
        if args.verbose:
            import traceback

            traceback.print_exc()
        return 1


if __name__ == '__main__':
    sys.exit(main())
