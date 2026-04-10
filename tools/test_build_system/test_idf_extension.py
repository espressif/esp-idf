# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import logging
import shutil
import subprocess
import sys
import textwrap
import typing
from pathlib import Path

import pytest
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import replace_in_file

from conftest import should_clean_test_dir

# Template constants for extension packages from entrypoints
TEST_EXT_TEMPLATE = """
def action_extensions(base_actions, project_path):
    def test_extension_action(target_name, ctx, args):
        print("Test extension action executed - {suffix}")
        return 0

    return {{
        'version': '1',
        'global_options': [{global_options}],
        'actions': {{
            {actions}
        }}
    }}
"""

PYPROJECT_TOML_TEMPLATE = """
[project]
name = "{package_name}"
version = "0.1.0"

[project.entry-points.idf_extension]
{entry_point_name} = "{declarative_value}"
"""


class ExtensionPackageManager:
    """
    Helper class to manage multiple extension packages within a single test.
    Tracks all created packages and handles cleanup automatically.
    """

    def __init__(self, func_work_dir: Path, request: pytest.FixtureRequest):
        self.func_work_dir = func_work_dir
        self.request = request
        self.packages: list[tuple[Path, str]] = []

    def create_package(
        self,
        suffix: str,
        template_vars: dict | None = None,
    ) -> tuple[str, str]:
        """
        Create and install an extension package with the given suffix.
        - suffix: Package suffix for unique naming
        - template_vars: Dictionary of variables to substitute in templates
        """
        test_name_sanitized = self.request.node.name.replace('[', '_').replace(']', '')

        # Default template variables
        default_vars = {
            'suffix': suffix,
            'package_name': f'test-idf-extension-package-{suffix}',
            'package_dir_name': f'{test_name_sanitized}_pkg_{suffix}',
            'action_name': f'test-extension-action-{suffix}',
            'entry_point_name': f'test_extension_{suffix}',
            'declarative_value': f'test_extension_package_{suffix}.test_ext:action_extensions',
            # Template placeholders - can be overridden via template_vars
            'global_options': '',
            'actions': f"""'{f'test-extension-action-{suffix}'}': {{
                'callback': test_extension_action,
                'help': 'Test action from extension package - {suffix}'
            }}""",
            'extension_file_name': 'test_ext.py',
        }

        # Merge with user-provided variables
        if template_vars:
            default_vars.update(template_vars)

        package_path = self.func_work_dir / default_vars['package_dir_name']
        package_path.mkdir(exist_ok=True)
        logging.debug(f"Creating python package '{default_vars['package_name']}' in directory '{package_path}'")
        test_package_dir = package_path / f'test_extension_package_{suffix}'
        test_package_dir.mkdir(exist_ok=True)
        (test_package_dir / '__init__.py').write_text('')

        # Fill test_ext.py with template
        (test_package_dir / default_vars['extension_file_name']).write_text(
            textwrap.dedent(TEST_EXT_TEMPLATE.format(**default_vars))
        )

        # Fill pyproject.toml with template
        (package_path / 'pyproject.toml').write_text(textwrap.dedent(PYPROJECT_TOML_TEMPLATE.format(**default_vars)))

        # Install the package
        cmd = [sys.executable, '-m', 'pip', 'install', '-e', '.']
        logging.debug(f'Running command: {" ".join(cmd)} in {package_path}')
        try:
            subprocess.run(cmd, check=True, cwd=package_path, capture_output=True, text=True)
        except subprocess.CalledProcessError as e:
            logging.error(f'Failed to install package at {package_path}: {e.stderr}')
            raise

        # Track the package for cleanup
        self.packages.append((package_path, default_vars['package_name']))

        return default_vars['entry_point_name'], default_vars['action_name']

    def cleanup(self) -> None:
        """
        Uninstall all packages and clean up directories.
        """
        for package_path, package_name in self.packages:
            try:
                subprocess.run([sys.executable, '-m', 'pip', 'uninstall', '-y', package_name])
                logging.debug(f'Uninstalled test extension package: {package_name}')
            except Exception as e:
                logging.warning(f'Failed to uninstall test extension package: {e}')

            if should_clean_test_dir(self.request):
                try:
                    shutil.rmtree(package_path, ignore_errors=True)
                except Exception:
                    pass


@pytest.fixture
def extension_package_manager(
    func_work_dir: Path, request: pytest.FixtureRequest
) -> typing.Generator[ExtensionPackageManager, None, None]:
    """
    Fixture that provides an ExtensionPackageManager to create multiple extension packages
    within a single test.
    """
    manager = ExtensionPackageManager(func_work_dir, request)

    try:
        yield manager
    finally:
        manager.cleanup()


# ----------- Test cases for component extension -----------


def test_extension_from_component(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Test loading extensions from component directories')

    # Create a component with a CLI extension
    idf_py('create-component', '-C', 'components', 'test_component')
    component_dir = test_app_copy / 'components' / 'test_component'
    idf_ext_py = component_dir / 'idf_ext.py'
    idf_ext_py.write_text(
        textwrap.dedent(
            TEST_EXT_TEMPLATE.format(
                suffix='component extension',
                global_options='',
                actions="""'test-component-action': {
                'callback': test_extension_action,
                'help': 'Test action from component extension'
            }""",
            )
        )
    )
    replace_in_file(
        test_app_copy / 'main' / 'CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        '\n'.join(['INCLUDE_DIRS "." ', 'REQUIRES "test_component" ']),
    )

    idf_py('reconfigure')
    ret = idf_py('--help')
    assert 'test-component-action' in ret.stdout
    assert 'INFO: Loaded component extension from "components/test_component"' in ret.stdout
    ret = idf_py('test-component-action')
    assert 'Test extension action executed - component extension' in ret.stdout
    assert 'INFO: Loaded component extension from "components/test_component"' in ret.stdout


def test_extension_from_component_invalid_syntax(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('Test handling of invalid component extensions')

    idf_py('create-component', '-C', 'components', 'invalid_component')
    replace_in_file(
        test_app_copy / 'main' / 'CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        '\n'.join(['INCLUDE_DIRS "." ', 'REQUIRES "invalid_component" ']),
    )
    ret = idf_py('reconfigure')
    assert ret.returncode == 0

    component_dir = test_app_copy / 'components' / 'invalid_component'
    idf_ext_py = component_dir / 'idf_ext.py'
    idf_ext_py.write_text('def some_function()  # no ":" at the end - INVALID SYNTAX')
    ret = idf_py('--help')
    assert 'Warning: Failed to import extension' in ret.stderr

    idf_ext_py.write_text(
        textwrap.dedent("""
        def some_function():
            pass
    """)
    )
    ret = idf_py('--help')
    assert "has no attribute 'action_extensions'" in ret.stderr

    idf_ext_py.write_text(
        textwrap.dedent(
            TEST_EXT_TEMPLATE.format(
                suffix='component extension',
                global_options='',
                actions="""'test-component-action': {
                'callback': test_extension_action,
                'help': 'Test action from component extension'
            }""",
            )
        )
    )
    replace_in_file(
        idf_ext_py,
        "'version': '1',",
        '\n',
    )
    ret = idf_py('--help')
    assert 'Attribute "version" is required in custom extension.' in ret.stderr


# ----------- Test cases for entry point extension -----------


@pytest.mark.usefixtures('test_app_copy')
def test_extension_entrypoint(idf_py: IdfPyFunc, extension_package_manager: ExtensionPackageManager) -> None:
    logging.info('Test loading multiple extensions from Python entry points')

    _, action1_name = extension_package_manager.create_package('alpha')
    _, action2_name = extension_package_manager.create_package('beta')

    ret = idf_py('--help')
    assert action1_name in ret.stdout
    assert action2_name in ret.stdout

    ret_alpha = idf_py('test-extension-action-alpha')
    assert 'Test extension action executed - alpha' in ret_alpha.stdout

    ret_beta = idf_py('test-extension-action-beta')
    assert 'Test extension action executed - beta' in ret_beta.stdout


@pytest.mark.usefixtures('test_app_copy')
def test_extension_entrypoint_declarative_value_duplicate(
    idf_py: IdfPyFunc, extension_package_manager: ExtensionPackageManager
) -> None:
    logging.info('Test entry point declarative value duplicate name warning')

    entry_point1_name, action1_name = extension_package_manager.create_package(
        'collision1',
        template_vars={
            'declarative_value': 'duplicate_test_ext:action_extensions'  # Same declarative value
        },
    )

    entry_point2_name, action2_name = extension_package_manager.create_package(
        'collision2',
        template_vars={
            'declarative_value': 'duplicate_test_ext:action_extensions'  # Same declarative value
        },
    )

    ret = idf_py('--help')
    assert action1_name not in ret.stdout
    assert action2_name not in ret.stdout
    assert 'name collision detected for - duplicate_test_ext:action_extensions' in ret.stderr
    assert entry_point1_name in ret.stderr
    assert entry_point2_name in ret.stderr


@pytest.mark.usefixtures('test_app_copy')
def test_extension_entrypoint_default_declarative_value(
    idf_py: IdfPyFunc, extension_package_manager: ExtensionPackageManager
) -> None:
    """
    Test recommendation warning log when entrypoint uses default idf_ext:action_extensions declarative value.
    This declarative value (extension file name) is used for components participating in the build,
    thus is not recommended to use it for external components - entrypoints.
    """
    logging.info('Test entrypoint uses default idf_ext:action_extensions declarative value')

    entry_point_name, _ = extension_package_manager.create_package(
        'default_value',
        template_vars={
            'declarative_value': 'idf_ext:action_extensions',
        },
    )

    ret = idf_py('--help')
    assert f'Entry point "{entry_point_name}" has declarative value "idf_ext:action_extensions"' in ret.stderr
    assert (
        'For external components, it is recommended to use name like <<COMPONENT_NAME>>_ext:action_extensions'
        in ret.stderr
    )


@pytest.mark.usefixtures('test_app_copy')
def test_extension_entrypoint_non_existing_module(
    idf_py: IdfPyFunc, extension_package_manager: ExtensionPackageManager
) -> None:
    logging.info('Test entrypoint uses non-existing module')

    entry_point_name, _ = extension_package_manager.create_package(
        'non_existing_module',
        template_vars={
            'declarative_value': 'non_existing_module:action_extensions',
        },
    )

    ret = idf_py('--help')
    assert f'Failed to load entry point extension "{entry_point_name}"' in ret.stderr
    assert "No module named 'non_existing_module'" in ret.stderr


@pytest.mark.usefixtures('test_app_copy')
def test_extension_entrypoint_conflicting_names(
    idf_py: IdfPyFunc, extension_package_manager: ExtensionPackageManager
) -> None:
    logging.info('Test action name conflict warning')

    extension_package_manager.create_package(
        'conflicting_action',
        template_vars={
            'actions': """
                'bootloader': {
                    'callback': test_extension_action,
                    'help': 'This action conflicts with built-in action',
                },
                'my-custom-action': {
                'callback': test_extension_action,
                'help': 'Custom action with conflicting aliases',
                'aliases': ['clean']
            }
            """,
            'global_options': """{
                'names': ['--project-dir'],
                'help': 'This global option conflicts with existing one'
            }""",
        },
    )

    ret = idf_py('--help')
    assert "Action 'bootloader' already defined. External action will not be added." in ret.stderr
    assert 'This action conflicts with built-in action' not in ret.stdout
    assert (
        "Action 'my-custom-action' has aliases ['clean'] that conflict with existing actions or aliases" in ret.stderr
    )
    assert 'Custom action with conflicting aliases' not in ret.stdout
    assert "Global option ['--project-dir'] already defined. External option will not be added." in ret.stderr
    assert 'This global option conflicts with existing one' not in ret.stdout
