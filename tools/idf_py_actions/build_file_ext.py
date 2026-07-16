# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import hashlib
import os
import re
import shutil
import tempfile
from typing import Any

import rich_click as click
import yaml
from esp_pylib.logger import log
from rich.markup import escape
from rich_click import Context

from idf_py_actions.errors import FatalError
from idf_py_actions.tools import PropertyDict

# Container projects are stored in the system temp directory.
# The OS cleans these up automatically, which is desirable since IDF build
# directories can get quite large.
_BUILD_FILE_CACHE_DIR = os.path.join(tempfile.gettempdir(), 'esp-idf-build-file')

# Marker for idf-build-file frontmatter
_FRONTMATTER_MARKER = 'idf-build-file:'


def _parse_frontmatter(source_file: str) -> dict:
    """Parse YAML frontmatter from a C source file.

    Looks for a block comment containing 'idf-build-file:' as a marker,
    then parses the YAML content after that marker.

    Returns a dict with keys like 'config', 'dependencies', etc.
    Returns empty dict if no frontmatter found.
    """
    with open(source_file, encoding='utf-8') as f:
        content = f.read()

    # Find block comments that contain the frontmatter marker
    block_comment_pattern = re.compile(r'/\*(.+?)\*/', re.DOTALL)

    for match in block_comment_pattern.finditer(content):
        comment_body = match.group(1)
        if _FRONTMATTER_MARKER not in comment_body:
            continue

        # Extract everything starting from the marker line
        lines = comment_body.split('\n')
        yaml_lines = []
        found_marker = False
        for line in lines:
            stripped = line.strip()
            if not found_marker:
                if stripped.startswith('idf-build-file:'):
                    yaml_lines.append(line)
                    found_marker = True
            else:
                yaml_lines.append(line)

        if not yaml_lines:
            continue

        yaml_text = '\n'.join(yaml_lines)
        try:
            parsed = yaml.safe_load(yaml_text)
            if isinstance(parsed, dict) and 'idf-build-file' in parsed:
                return parsed['idf-build-file'] or {}
        except yaml.YAMLError as e:
            raise FatalError(f'Failed to parse frontmatter YAML in {source_file}: {e}')

    return {}


def _compute_project_hash(source_file: str) -> str:
    """Compute a stable hash for the container project directory name."""
    abs_path = os.path.abspath(source_file)
    return hashlib.sha256(abs_path.encode()).hexdigest()[:16]


def _write_if_changed(filepath: str, content: str) -> bool:
    """Write content to file only if it differs from current content.

    Returns True if file was written, False if unchanged.
    """
    if os.path.exists(filepath):
        with open(filepath, encoding='utf-8') as f:
            existing = f.read()
        if existing == content:
            return False

    os.makedirs(os.path.dirname(filepath), exist_ok=True)
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    return True


def _is_bool_or_number(value: str) -> bool:
    """Check if a config value is boolean or numeric (doesn't need quoting)."""
    if value in ('y', 'n', ''):
        return True
    try:
        int(value, 0)  # supports decimal, hex (0x...), octal (0o...)
        return True
    except ValueError:
        return False


def _cmake_quote(value: str) -> str:
    """Quote a string as a CMake quoted argument.

    Backslashes and double quotes are escaped so that the option reaches the
    compiler exactly as written in the frontmatter, e.g. -DMSG="hello world".
    """
    escaped = value.replace('\\', '\\\\').replace('"', '\\"')
    return f'"{escaped}"'


def _classify_dependencies(dependencies: list) -> tuple[list[str], dict[str, Any]]:
    """Split dependencies into built-in components and managed components.

    Built-in components (no '/' in name, no path) -> PRIV_REQUIRES in idf_component_register
    Managed components (with '/' like 'espressif/zlib') -> idf_component.yml
    Path-based components (dict with 'path' key) -> idf_component.yml

    Returns (builtin_requires, managed_deps_dict).
    """
    builtin: list[str] = []
    managed: dict[str, Any] = {}

    for dep in dependencies:
        if isinstance(dep, str):
            # Strip version specifier to check the name
            name = dep.split('==')[0].split('>=')[0].strip()
            if '/' in name:
                # Managed component
                if '==' in dep:
                    n, version = dep.split('==', 1)
                    managed[n.strip()] = {'version': version.strip()}
                elif '>=' in dep:
                    n, version = dep.split('>=', 1)
                    managed[n.strip()] = {'version': f'>={version.strip()}'}
                else:
                    managed[name] = '*'
            else:
                # Built-in component
                builtin.append(name)
        elif isinstance(dep, dict):
            # Dict entries: path-based or other complex deps go to idf_component.yml
            managed.update(dep)

    return builtin, managed


def _get_config_target(config_items: list[str]) -> str | None:
    """Extract the CONFIG_IDF_TARGET value from a list of sdkconfig lines, if present."""
    for item in config_items:
        if item.startswith('CONFIG_IDF_TARGET='):
            return item.split('=', 1)[1].strip().strip('"')
    return None


def _get_cached_target(build_dir: str) -> str | None:
    """Extract the IDF_TARGET value pinned in the CMake cache, if present."""
    cmake_cache = os.path.join(build_dir, 'CMakeCache.txt')
    if not os.path.exists(cmake_cache):
        return None
    with open(cmake_cache, encoding='utf-8') as f:
        for line in f:
            match = re.match(r'IDF_TARGET:\w+=(\w+)', line)
            if match:
                return match.group(1)
    return None


def _invalidate_stale_config(container_dir: str, processed_config: list[str]) -> None:
    """Invalidate stale build state after the generated sdkconfig.defaults changed.

    Values in sdkconfig.defaults are only applied when sdkconfig is generated,
    so the existing sdkconfig must be removed for the new defaults to take
    effect. A target change additionally requires removing the build directory,
    since IDF_TARGET is pinned in the CMake cache and the toolchain file.
    """
    new_target = _get_config_target(processed_config) or os.environ.get('IDF_TARGET') or 'esp32'

    build_dir = os.path.join(container_dir, 'build')
    cached_target = _get_cached_target(build_dir)
    if cached_target and cached_target != new_target:
        shutil.rmtree(build_dir)
        log.note(f'build-file: Target changed from {cached_target} to {new_target}, cleaned the build directory')

    sdkconfig_path = os.path.join(container_dir, 'sdkconfig')
    if os.path.exists(sdkconfig_path):
        os.remove(sdkconfig_path)
        log.note('build-file: Configuration changed, sdkconfig will be regenerated')


def _generate_container_project(source_file: str, frontmatter: dict, container_dir: str) -> None:
    """Generate or update the container project for a source file."""
    abs_source = os.path.abspath(source_file)
    source_basename = os.path.basename(source_file)
    source_name = os.path.splitext(source_basename)[0]

    main_dir = os.path.join(container_dir, 'main')
    os.makedirs(main_dir, exist_ok=True)

    # Classify dependencies
    dependencies = frontmatter.get('dependencies', [])
    builtin_requires, managed_deps = _classify_dependencies(dependencies)

    # Top-level CMakeLists.txt (cmakev2)
    top_cmake = (
        '# Auto-generated by idf.py build-file. Do not edit.\n'
        'cmake_minimum_required(VERSION 3.22)\n'
        '\n'
        'include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)\n'
        f'project({source_name} C CXX ASM)\n'
        'idf_project_default()\n'
    )
    _write_if_changed(os.path.join(container_dir, 'CMakeLists.txt'), top_cmake)

    # Copy the source file into main/ (using _write_if_changed for caching)
    with open(abs_source, encoding='utf-8') as f:
        source_content = f.read()
    _write_if_changed(os.path.join(main_dir, source_basename), source_content)

    # main/CMakeLists.txt with REQUIRES for built-in components
    requires_clause = ''
    if builtin_requires:
        requires_clause = f'\n                    PRIV_REQUIRES {" ".join(builtin_requires)}'

    main_cmake = (
        '# Auto-generated by idf.py build-file. Do not edit.\n'
        f'idf_component_register(SRCS "{source_basename}"\n'
        f'                    INCLUDE_DIRS "."{requires_clause})\n'
    )

    # Extra compiler options from the frontmatter
    compile_options = frontmatter.get('compile_options', [])
    if not isinstance(compile_options, list) or not all(isinstance(opt, str) for opt in compile_options):
        raise FatalError(f'compile_options in {source_file} frontmatter must be a list of strings')
    if compile_options:
        quoted_options = ' '.join(_cmake_quote(opt) for opt in compile_options)
        main_cmake += f'target_compile_options(${{COMPONENT_LIB}} PRIVATE {quoted_options})\n'

    _write_if_changed(os.path.join(main_dir, 'CMakeLists.txt'), main_cmake)

    # sdkconfig.defaults
    config_list = frontmatter.get('config', [])
    # Auto-quote string values that aren't already quoted, boolean, or numeric.
    # Kconfig string options require quoted values in sdkconfig.defaults.
    processed_config = []
    for item in config_list:
        if '=' in item:
            key, value = item.split('=', 1)
            if not (value.startswith('"') and value.endswith('"')) and not _is_bool_or_number(value):
                item = f'{key}="{value}"'
        processed_config.append(item)
    sdkconfig_content = '\n'.join(processed_config) + '\n' if processed_config else ''
    if _write_if_changed(os.path.join(container_dir, 'sdkconfig.defaults'), sdkconfig_content):
        _invalidate_stale_config(container_dir, processed_config)

    # idf_component.yml for managed dependencies only
    if managed_deps:
        idf_component_yml = yaml.dump({'dependencies': managed_deps}, default_flow_style=False)
        _write_if_changed(os.path.join(main_dir, 'idf_component.yml'), idf_component_yml)
    else:
        yml_path = os.path.join(main_dir, 'idf_component.yml')
        if os.path.exists(yml_path):
            os.remove(yml_path)


def action_extensions(base_actions: dict, project_path: str) -> dict:
    def _find_build_file_task(tasks: list) -> Any | None:
        """Find the build-file task in the task list, if present."""
        for task in tasks:
            if task.name == 'build-file':
                return task
        return None

    def build_file_global_callback(ctx: Context, global_args: PropertyDict, tasks: list) -> None:
        """Global callback that runs before validate_root_options.

        If build-file is in the task list, this sets up the container project
        and redirects project_dir/build_dir to it, so that subsequent commands
        (build, flash, monitor) operate on the container project.
        """
        build_file_task = _find_build_file_task(tasks)
        if build_file_task is None:
            return

        source_file = build_file_task.action_args.get('source_file')
        if not source_file:
            return

        if not os.path.isfile(source_file):
            raise FatalError(f'Source file not found: {source_file}')

        clean_cache = build_file_task.action_args.get('clean_cache', False)

        # Parse frontmatter
        frontmatter = _parse_frontmatter(source_file)

        if not frontmatter:
            log.warn(
                escape(
                    f'build-file: No idf-build-file frontmatter found in {source_file}. '
                    'The file will be built with default settings.'
                )
            )

        # Determine container project directory
        file_hash = _compute_project_hash(source_file)
        source_name = os.path.splitext(os.path.basename(source_file))[0]
        container_dir = os.path.join(_BUILD_FILE_CACHE_DIR, f'{source_name}_{file_hash}')

        # Clean cache if requested
        if clean_cache and os.path.isdir(container_dir):
            shutil.rmtree(container_dir)
            log.note(escape(f'build-file: Cleaned cache at {container_dir}'))

        # Generate/update the container project
        _generate_container_project(source_file, frontmatter, container_dir)

        # Redirect project_dir and build_dir to the container project.
        # This runs before validate_root_options (from core_ext.py) which will
        # then use these values instead of the defaults.
        global_args['project_dir'] = os.path.realpath(container_dir)
        global_args['build_dir'] = os.path.join(os.path.realpath(container_dir), 'build')

        log.note(escape(f'build-file: Container project: {container_dir}'))
        log.note(escape(f'build-file: Source file: {os.path.abspath(source_file)}'))

    def build_file_callback(action: str, ctx: Context, args: PropertyDict, **action_args: Any) -> None:
        """The build-file action callback.

        The heavy lifting is done in the global callback. This callback exists
        so that the action is registered and composable with other commands.
        By depending on 'all', the build is triggered automatically.
        """
        pass

    return {
        'global_action_callbacks': [build_file_global_callback],
        'actions': {
            'build-file': {
                'callback': build_file_callback,
                'short_help': 'Build a standalone C file without project boilerplate.',
                'help': (
                    'Build a standalone C source file without requiring a full ESP-IDF project structure. '
                    'The source file may contain an optional YAML frontmatter in a block comment '
                    'starting with "idf-build-file:" to specify sdkconfig options, component dependencies, '
                    'compiler options, and other settings.\n\n'
                    'A container project is automatically created and cached. Subsequent builds of the '
                    'same file reuse the container project for fast incremental builds.\n\n'
                    'This command is composable with other idf.py commands. For example:\n'
                    '  idf.py build-file example.c build\n'
                    '  idf.py build-file example.c flash monitor\n'
                ),
                'arguments': [
                    {
                        'names': ['source_file'],
                        'nargs': 1,
                        'type': click.Path(exists=False),
                    },
                ],
                'options': [
                    {
                        'names': ['--clean-cache'],
                        'is_flag': True,
                        'default': False,
                        'help': 'Remove the cached container project before building.',
                    },
                ],
                'dependencies': ['all'],
            },
        },
    }
