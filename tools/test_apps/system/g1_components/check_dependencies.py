# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import argparse
import glob
import logging
import os

# Base G1/G0 components (static list)
g1_g0_components_base = [
    'hal',
    'cxx',
    'esp_libc',
    'freertos',
    'esp_hw_support',
    'heap',
    'log',
    'soc',
    'esp_rom',
    'esp_common',
    'esp_system',
    'xtensa',
    'riscv',
    'spi_flash',
    'esp_mm',
]


def get_all_esp_hal_components() -> list[str]:
    """Dynamically discover all esp_hal_* components"""
    esp_hal_components = []

    # Try to get IDF_PATH from environment
    idf_path = os.environ.get('IDF_PATH')
    if idf_path is None:
        # Fallback: assume script is in IDF_PATH/tools/test_apps/system/g1_components/
        script_dir = os.path.dirname(os.path.abspath(__file__))
        idf_path = os.path.join(script_dir, '../../../../..')

    components_dir = os.path.join(idf_path, 'components')
    if os.path.exists(components_dir):
        # Find all esp_hal_* directories
        esp_hal_dirs = glob.glob(os.path.join(components_dir, 'esp_hal_*'))
        for hal_dir in esp_hal_dirs:
            if os.path.isdir(hal_dir):
                component_name = os.path.basename(hal_dir)
                esp_hal_components.append(component_name)

    return sorted(esp_hal_components)


# Build complete G1/G0 components list (base + dynamic esp_hal_* components)
g1_g0_components = g1_g0_components_base + get_all_esp_hal_components()

# Global expected dependency violations that apply to all targets
expected_dep_violations = {
    'esp_system': ['esp_timer', 'bootloader_support', 'esp_pm', 'esp_usb_cdc_rom_console'],
    'spi_flash': ['bootloader_support', 'esp_blockdev', 'esp_driver_gpio'],
    'esp_hw_support': ['efuse', 'bootloader_support', 'esp_driver_gpio', 'esp_timer', 'esp_pm'],
    'cxx': ['pthread'],
}

# Target-specific expected dependency violations
target_specific_expected_dep_violations = {
    # 'target': {
    # Add target-specific violations for target here
    # 'component_name': ['dependency1', 'dependency2'],
    # },
    'esp32s2': {
        # ESP32-S2 uses the crypto DMA lock for encrypted writes, thus, spi_flash needs to depend on esp_security
        'spi_flash': ['esp_security'],
    },
}


def merge_expected_violations(target: str | None = None) -> dict[str, list[str]]:
    """
    Merge global and target-specific expected dependency violations.

    Args:
        target: The target target name (e.g., 'esp32', 'esp32s3', etc.)

    Returns:
        Merged dictionary of expected dependency violations
    """
    # Start with a deep copy of global violations
    merged_violations = {}
    for component, deps in expected_dep_violations.items():
        merged_violations[component] = deps.copy()

    # Add target-specific violations if target is specified
    if target and target in target_specific_expected_dep_violations:
        target_violations = target_specific_expected_dep_violations[target]
        for component, deps in target_violations.items():
            if component in merged_violations:
                # Extend existing list with target-specific dependencies
                merged_violations[component].extend(deps)
            else:
                # Add new component with its dependencies
                merged_violations[component] = deps.copy()

    return merged_violations


def parse_dependencies(file_path: str, target: str | None = None) -> tuple[dict[str, list[str]], list[str]]:
    new_dependency_errors = []

    # Get merged expected violations for the specified target
    merged_expected_violations = merge_expected_violations(target)

    with open(file_path) as file:
        for line in file:
            line = line.strip(' ;')

            if line:
                parts = line.split(' -> ')

                if len(parts) >= 2:
                    source_component = parts[0].strip('"')
                    target_component = parts[1].split()[0].strip('"')  # Extracting the target component
                    print(f'{source_component} -> {target_component}')
                    logging.debug(f'Parsed dependency: {source_component} -> {target_component}')

                    # Check that g1/g0 dependencies are either on the list of expected violations
                    # or dependencies to other g1/g0 components
                    if source_component in g1_g0_components and target_component not in g1_g0_components:
                        if (
                            source_component in merged_expected_violations
                            and target_component in merged_expected_violations[source_component]
                        ):
                            logging.debug(
                                f'Removing dependency {target_component} from {source_component} '
                                f'in list of expected violations'
                            )
                            merged_expected_violations[source_component].remove(target_component)
                        else:
                            new_dependency_errors.append(f'{source_component} -> {target_component}')

    # Any leftover dependencies in the merged_expected_violations are no longer true dependencies and
    # can be removed from the list
    false_dependencies = {k: v for k, v in merged_expected_violations.items() if len(v) > 0}

    return (false_dependencies, new_dependency_errors)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Check G1 dependencies')
    parser.add_argument(
        '--component_deps_file', required=True, type=str, help='The path to the component_deps.dot file'
    )
    parser.add_argument('--target', type=str, help='The target name (e.g., esp32, esp32s3, esp32c6, etc.)')

    args = parser.parse_args()

    (false_dependencies, new_dependency_errors) = parse_dependencies(args.component_deps_file, args.target)

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
