#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
ESP-IDF Examples Count Metrics Generator

This script scans all ESP-IDF examples and generates a list of records,
where each record contains:
- component: the component name (e.g., 'wifi', 'bluetooth', 'storage')
- target: the supported ESP32 chip variant (e.g., 'ESP32', 'ESP32-C3', 'ESP32-S3')
- count: the number of examples for that component-target combination

The script parses README.md files in example directories to extract supported targets
from the "Supported Targets" table format.
"""

import json
import os
import sys
from collections import defaultdict
from pathlib import Path


def extract_supported_targets_from_readme(readme_path: Path) -> list[str]:
    """
    Extract supported targets from a README.md file.

    Looks for the "Supported Targets" table in the README header
    and parses the list of supported ESP32 variants.

    Expected table format::

        | Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ... |
        | ----------------- | ----- | -------- | -------- | ... |

    :param readme_path: Path to a README.md file
    :return: List of supported targets (e.g. ``["ESP32", "ESP32-C3", "ESP32-S3"]``)
    """
    if not readme_path.exists():
        return []

    try:
        for line in readme_path.read_text(encoding='utf-8').splitlines():
            if not line.strip().startswith('| Supported Targets'):
                continue
            parts = [p.strip() for p in line.split('|')[2:]]
            return [p for p in parts if p and p not in ('-', '--')]
    except Exception as e:
        print(f'Warning: Could not parse {readme_path}: {e}')
    return []


def process_example_dir(component: str, example_dir: Path, results: dict[str, dict[str, list[str]]]) -> None:
    """Process a single example directory (with README)."""
    readme = example_dir / 'README.md'
    if not readme.exists():
        return
    targets = extract_supported_targets_from_readme(readme)
    if targets:
        path = f'{component}/{example_dir.name}'
        results[component][path] = targets
        print(f'  Found example: {path} ({len(targets)} targets)')


def process_nested_example(component: str, parent_dir: Path, results: dict[str, dict[str, list[str]]]) -> None:
    """Process nested sub-examples inside a parent directory."""
    for sub in parent_dir.iterdir():
        if not sub.is_dir() or sub.name.startswith('.'):
            continue
        readme = sub / 'README.md'
        if not readme.exists():
            continue
        targets = extract_supported_targets_from_readme(readme)
        if targets:
            path = f'{component}/{parent_dir.name}/{sub.name}'
            results[component][path] = targets


def find_examples(examples_root: Path) -> dict[str, dict[str, list[str]]]:
    """
    Traverse the ESP-IDF examples directory and collect supported targets.

    :param examples_root: Path to the ``examples`` directory
    :return: Nested dictionary of the form::

        {
            "component": {
                "component/example": ["ESP32", "ESP32-C3"],
                "component/nested/example": ["ESP32-S3"],
            },
            ...
        }
    """
    results: dict[str, dict[str, list[str]]] = defaultdict(dict)
    if not examples_root.exists():
        print(f'Error: Examples directory not found: {examples_root}')
        return {}

    for component_dir in examples_root.iterdir():
        if not component_dir.is_dir() or component_dir.name.startswith('.'):
            continue
        component = component_dir.name
        print(f'Analyzing component: {component}')

        for item in component_dir.iterdir():
            if not item.is_dir() or item.name.startswith('.'):
                continue
            if (item / 'README.md').exists():
                process_example_dir(component, item, results)
            else:
                process_nested_example(component, item, results)

    return dict(results)


def generate_metrics(esp_idf_path: str) -> dict:
    """
    Generate ESP-IDF examples count metrics.

    :param esp_idf_path: Path to the ESP-IDF repository root
    :return: Dictionary with examples count data in a list format
    """
    examples_data = find_examples(Path(esp_idf_path) / 'examples')
    if not examples_data:
        return {'examples_count': {'data': [], 'total_examples_count': 0}}

    counts: dict[tuple[str, str], int] = defaultdict(int)
    total_examples = 0

    for component, examples in examples_data.items():
        for example_path, targets in examples.items():
            for target in targets:
                counts[(component, target)] += 1
        total_examples += len(examples)

    examples_count_list = []
    for (component, target), count in sorted(counts.items()):
        examples_count_list.append(
            {
                'component': component,
                'target': target,
                'count': count,
            }
        )

    return {
        'examples_count': {
            'data': examples_count_list,
            'total_examples_count': total_examples,
        }
    }


def main() -> None:
    """CLI to generate metrics."""
    esp_idf_path = os.environ.get('IDF_PATH', os.getcwd())

    if not Path(esp_idf_path, 'examples').exists():
        print(f'Error: ESP-IDF root not found at {esp_idf_path}')
        sys.exit(1)

    print(f'Using ESP-IDF path: {esp_idf_path}')
    metrics = generate_metrics(esp_idf_path)

    output_file = Path('metrics.json')
    output_file.write_text(json.dumps(metrics, indent=2))
    print(f'\nMetrics generated successfully and saved to {output_file}')


if __name__ == '__main__':
    main()
