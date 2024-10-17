# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import argparse
import logging
from typing import Dict
from typing import List
from typing import Tuple

g1_g0_components = ['hal', 'cxx', 'newlib', 'freertos', 'esp_hw_support', 'heap', 'log', 'soc', 'esp_rom',
                    'esp_common', 'esp_system', 'xtensa', 'riscv', 'spi_flash', 'esp_mm']

expected_dep_violations = {'esp_system': ['esp_timer', 'bootloader_support', 'esp_pm'],
                           'spi_flash': ['bootloader_support', 'app_update', 'esp_driver_gpio'],
                           'esp_hw_support': ['efuse', 'bootloader_support', 'esp_driver_gpio', 'esp_timer', 'esp_pm', 'esp_security'],
                           'cxx': ['pthread']}


def parse_dependencies(file_path: str) -> Tuple[Dict[str, List[str]], List[str]]:
    new_dependency_errors = []

    with open(file_path, 'r') as file:
        for line in file:
            line = line.strip(' ;')

            if line:
                parts = line.split(' -> ')

                if (len(parts) >= 2):
                    source = parts[0]
                    target = parts[1].split()[0]  # Extracting the target component
                    logging.debug(f'Parsed dependency: {source} -> {target}')

                    # Check that g1/g0 dependencies are either on the list of expected violations
                    # or dependencies to other g1/g0 components
                    if source in g1_g0_components and target not in g1_g0_components:
                        if target in expected_dep_violations[source]:
                            logging.debug(f'Removing dependency {target} from {source} in list of expected violations')
                            expected_dep_violations[source].remove(target)
                        else:
                            new_dependency_errors.append(f'{source} -> {target}')

    # Any leftover dependencies in the expected_dep_violations are no longer true dependencies and
    # can be removed from the list
    false_dependencies = {k: v for k, v in expected_dep_violations.items() if len(v) > 0}

    return (false_dependencies, new_dependency_errors)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Check G1 dependencies')
    parser.add_argument('--component_deps_file', required=True, type=str, help='The path to the component_deps.dot file')

    args = parser.parse_args()

    (false_dependencies, new_dependency_errors) = parse_dependencies(args.component_deps_file)

    if new_dependency_errors:
        print('Found the following new dependency violations:')
        print(new_dependency_errors)
        exit(1)

    if false_dependencies:
        print('The following dependencies are list as violations, but were not found in the component_deps.dot file:')
        print(false_dependencies)
        print('Please remove them from the violation list')
        exit(1)

    print('No new dependency violations found')
