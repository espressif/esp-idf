# SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import json
import logging
import os
import shutil
import signal
import subprocess
import sys
import textwrap
import typing
from pathlib import Path

import pytest
import yaml
from test_build_system_helpers import EnvDict
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import find_python
from test_build_system_helpers import replace_in_file
from test_build_system_helpers import run_idf_py

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

[tool.setuptools]
packages = ["test_extension_package_{suffix}"]
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
    expected_info = f'INFO: Loaded component extension from "{os.path.join("components", "test_component")}"'
    assert expected_info in ret.stdout
    ret = idf_py('test-component-action')
    assert 'Test extension action executed - component extension' in ret.stdout
    assert expected_info in ret.stdout


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


@pytest.mark.usefixtures('test_app_copy')
def test_idf_py_help_rich_click_component_extension_panel(
    idf_py: IdfPyFunc,
    default_idf_env: dict[str, str],
) -> None:
    """Default Commands panel first; component extension gets its own panel after."""
    idf_py('create-component', '-C', 'components', 'help_group_comp')
    comp_dir = Path('components') / 'help_group_comp'
    (comp_dir / 'idf_ext.py').write_text(
        textwrap.dedent(
            TEST_EXT_TEMPLATE.format(
                suffix='help panel comp',
                global_options='',
                actions="""'help-group-comp-cmd': {
                'callback': test_extension_action,
                'help': 'Component extension command for help panel test'
            }""",
            )
        )
    )
    replace_in_file(
        Path('main') / 'CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        '\n'.join(['INCLUDE_DIRS "." ', 'REQUIRES "help_group_comp" ']),
    )
    idf_py('reconfigure')

    env = {**default_idf_env, 'COLUMNS': '120', 'NO_COLOR': '1'}
    ret = run_idf_py('--help', env=env, workdir=os.getcwd(), check=True)

    idx_commands = ret.stdout.find('Commands')
    assert idx_commands != -1, 'Root idf.py --help should list the default Commands group.'
    idx_comp_panel = ret.stdout.find('help_group_comp', idx_commands)
    assert idx_comp_panel != -1, 'Expected help group for the help_group_comp component extension.'
    assert idx_comp_panel > idx_commands, 'Component extension group should appear after the default Commands group.'

    # ret.stdout[i:j]: substring from index i (inclusive) to j (exclusive); ':' separates the two bounds.
    default_block = ret.stdout[idx_commands:idx_comp_panel]
    assert 'build' in default_block, 'Built-in `build` should still appear under the default Commands panel.'


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


@pytest.mark.usefixtures('test_app_copy')
def test_idf_py_help_rich_click_entrypoint_extension_panel(
    idf_py: IdfPyFunc,
    default_idf_env: dict[str, str],
    extension_package_manager: ExtensionPackageManager,
) -> None:
    """Default Commands panel first; entry-point extension gets its own panel after."""
    extension_package_manager.create_package('helpgroup')

    env = {**default_idf_env, 'COLUMNS': '120', 'NO_COLOR': '1'}
    ret = run_idf_py('--help', env=env, workdir=os.getcwd(), check=True)

    idx_commands = ret.stdout.find('Commands')
    assert idx_commands != -1, 'Root idf.py --help should list the default Commands group.'
    idx_ep_panel = ret.stdout.find('test_extension_helpgroup', idx_commands)
    assert idx_ep_panel != -1, 'Expected help group for the helpgroup entry-point extension.'
    assert idx_ep_panel > idx_commands, 'Entry-point extension group should appear after the default Commands group.'

    # ret.stdout[i:j]: substring from index i (inclusive) to j (exclusive); ':' separates the two bounds.
    default_block = ret.stdout[idx_commands:idx_ep_panel]
    assert 'build' in default_block, 'Built-in `build` should still appear under the default Commands panel.'


# ----------- General extension tests -----------


@pytest.mark.usefixtures('test_app_copy')
def test_idf_py_subcommand_help_shows_options(
    idf_py: IdfPyFunc,
    default_idf_env: dict[str, str],
) -> None:
    """Subcommand --help must list global/action options (rich-click + default_panels_first)."""
    idf_py('reconfigure')
    env = {**default_idf_env, 'NO_COLOR': '1', 'COLUMNS': '120'}
    ret = run_idf_py('flash', '--help', env=env, workdir=os.getcwd(), check=True)
    assert '--project-dir' in ret.stdout or '-C ' in ret.stdout


# ----------- Regression test: idf.py recursion via idf_version clause -----------


@pytest.mark.skipif(os.name == 'nt', reason='start_new_session and os.killpg are POSIX-only')
def test_idf_version_recursion_on_reconfigure(idf_py: IdfPyFunc, test_app_copy: Path, default_idf_env: EnvDict) -> None:
    """`idf.py reconfigure` must terminate even when IDF_VERSION is absent from the
    environment and dependencies.lock contains an `if: idf_version` clause.

    Before the fix, the component-manager's lock parser called `idf.py --version`
    to resolve the clause, which re-entered init_cli() -> infinite recursion /
    fork bomb.  The fix seeds IDF_VERSION early in init_cli() from
    idf_version_from_cmake() (no subprocess).

    To force init_cli() down the path that loads the lock, the project must have
    a component whose source is `project_managed_components` and which ships an
    `idf_ext.py`.  init_cli() then calls `_is_component_trusted()` ->
    `_get_trusted_names_from_lock()` -> `LockManager(...).load()`, which eagerly
    parses every `OptionalDependency` and evaluates the `idf_version` clause.
    """
    logging.info('Test idf.py reconfigure must not recurse via idf_version lock clause')

    # 1. Create a managed component that ships idf_ext.py.  It is referenced from
    #    main/idf_component.yml as a *local path* dependency, which makes the
    #    component manager register it with source == project_managed_components
    #    in build/project_description.json (enforced by the precondition check below).
    ns = 'test_ns'
    comp = 'test_comp'
    comp_dir = test_app_copy / 'managed_components' / f'{ns}__{comp}'
    comp_dir.mkdir(parents=True)

    (comp_dir / 'idf_ext.py').write_text(
        textwrap.dedent(
            TEST_EXT_TEMPLATE.format(
                suffix='managed component extension',
                global_options='',
                actions="""'test-managed-action': {
                'callback': test_extension_action,
                'help': 'Test action from managed component'
            }""",
            )
        )
    )
    (comp_dir / 'CMakeLists.txt').write_text('idf_component_register(INCLUDE_DIRS ".")\n')
    (comp_dir / 'idf_component.yml').write_text('version: "1.0.0"\n')

    # Declare the managed component as a local-path dependency of `main`.
    # The `path` is resolved relative to the manifest file (main/), hence the
    # leading `../`.  The component manager assigns it the
    # project_managed_components source on reconfigure.
    (test_app_copy / 'main' / 'idf_component.yml').write_text(
        textwrap.dedent(
            f"""\
            dependencies:
              {ns}/{comp}:
                version: "*"
                path: ../managed_components/{ns}__{comp}
            """
        )
    )

    # 2. First reconfigure: generates build/project_description.json (with the
    #    managed component listed) and dependencies.lock.
    idf_py('reconfigure')

    # Precondition: verify the managed component is registered with
    # source == 'project_managed_components'.  init_cli() only parses
    # dependencies.lock for components with that source; if the source ever
    # changes this assertion will fail loudly instead of the test passing vacuously.
    project_desc_path = test_app_copy / 'build' / 'project_description.json'
    project_desc = json.loads(project_desc_path.read_text())
    comp_key = f'{ns}__{comp}'
    assert comp_key in project_desc.get('build_component_info', {}), (
        f'Managed component {comp_key!r} not found in build_component_info; the test setup may be broken.'
    )
    assert project_desc.get('all_component_info', {}).get(comp_key, {}).get('source') == 'project_managed_components', (
        f'Component {comp_key!r} source is not project_managed_components; '
        'init_cli() will not parse dependencies.lock for this component and '
        'the idf_version recursion guard is not exercised.'
    )

    # 3. Overwrite dependencies.lock, injecting an `if: idf_version` clause into
    #    the managed component's sub-dependency.  This is written *after* the
    #    reconfigure because reconfigure regenerates the lock and would clobber a
    #    pre-planted one.
    #
    #    Only the `rules:` clause is injected; FORMAT_VERSION, manifest_hash,
    #    source, and target are all preserved from the component manager's own output
    #    so the test does not encode lock-format internals.
    lock_path = test_app_copy / 'dependencies.lock'
    lock = yaml.safe_load(lock_path.read_text())
    lock['dependencies'][f'{ns}/{comp}']['dependencies'] = [{'name': 'idf', 'rules': [{'if': 'idf_version >=6.0'}]}]
    lock_path.write_text(yaml.safe_dump(lock))

    # 4. Run idf.py reconfigure as a subprocess with IDF_VERSION and
    #    CI_TESTING_IDF_VERSION stripped, reproducing the "outside a CMake build"
    #    context that triggered the recursion.
    #    Use start_new_session=True so the entire process tree (including any
    #    runaway children) lands in one process group that can be killed atomically.
    env = dict(default_idf_env)
    env.pop('IDF_VERSION', None)
    env.pop('CI_TESTING_IDF_VERSION', None)

    idf_path = env['IDF_PATH']
    python = find_python(env['PATH'])
    cmd = [python, os.path.join(idf_path, 'tools', 'idf.py'), 'reconfigure']

    logging.debug(f'Running {cmd} without IDF_VERSION in env')
    proc = subprocess.Popen(
        cmd,
        env=env,
        cwd=str(test_app_copy),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        start_new_session=True,
    )

    stdout, stderr = b'', b''
    try:
        stdout, stderr = proc.communicate(timeout=30)
    except subprocess.TimeoutExpired:
        # Kill the entire process group to reap any runaway recursive children.
        try:
            os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
        except ProcessLookupError:
            pass
        proc.communicate()
        pytest.fail(
            'idf.py reconfigure timed out (30 s) — likely hit the init_cli() recursion. '
            'IDF_VERSION was not seeded early enough in init_cli().'
        )

    logging.debug(f'reconfigure stdout: {stdout.decode(errors="replace")}')
    logging.debug(f'reconfigure stderr: {stderr.decode(errors="replace")}')
    assert proc.returncode == 0, (
        f'idf.py reconfigure exited with {proc.returncode}.\n'
        f'stdout: {stdout.decode(errors="replace")}\n'
        f'stderr: {stderr.decode(errors="replace")}'
    )

    # Runtime proof: the warning is emitted by idf.py only when
    # _is_component_trusted() returns False for a project_managed_components
    # component — i.e. after _get_trusted_names_from_lock() -> LockManager.load()
    # parsed the planted lock to completion (instead of forking).  The local-path
    # component is never a trusted WebServiceSource, so this warning is deterministic.
    assert 'Not loading component extension from untrusted source' in stderr.decode(errors='replace'), (
        'Expected the untrusted-source warning in stderr, which proves that '
        '_is_component_trusted() / LockManager.load() ran to completion with '
        'IDF_VERSION seeded (no recursion). stderr was:\n' + stderr.decode(errors='replace')
    )
