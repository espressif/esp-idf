# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import glob
import json
import os
import re
import shutil
import subprocess
import sys
from collections import namedtuple
from typing import IO
from typing import Any
from typing import cast

script_dir = os.path.dirname(os.path.abspath(__file__))
current_dir = os.path.abspath(os.getcwd())
if script_dir != current_dir:
    print(f'Error: This script must be run from its own directory: {script_dir}')
    print(f'Current working directory is: {current_dir}')
    sys.exit(1)

idf_path = os.getenv('IDF_PATH')
if idf_path is None:
    idf_path = os.path.realpath(os.path.join(script_dir, '..', '..', '..', '..'))

Param = namedtuple('Param', ['ptr', 'array', 'qual', 'type', 'name'])

component_path = os.path.normpath(os.path.join(os.path.realpath(__file__), '..', '..'))
AUTO_GENERATED = 'This file is auto-generated'
COPYRIGHT_HEADER = open(component_path + '/scripts/copyright_header.h').read()
NAMESPACE = re.compile(r'^esp_wifi')
DEPRECATED_API = ['esp_wifi_set_ant_gpio', 'esp_wifi_get_ant', 'esp_wifi_get_ant_gpio', 'esp_wifi_set_ant']
KCONFIG_MULTIPLE_DEF = '# ignore: multiple-definition'
wifi_configs = []
FunctionPrototypes = dict[str, tuple[str, list[Any]]]


# Parse the header file and extract function prototypes
def extract_function_prototypes(
    header_code: str, header: str, prefixes: str | list[str] | tuple[str, ...]
) -> FunctionPrototypes:
    from pycparser import c_ast
    from pycparser import c_parser

    class FunctionVisitor(c_ast.NodeVisitor):
        def __init__(self, header: str, prefixes: str | list[str] | tuple[str, ...]) -> None:
            self.function_prototypes: FunctionPrototypes = {}
            self.ptr = 0
            self.array = 0
            self.content = open(header).read()
            self.prefixes = prefixes if isinstance(prefixes, list | tuple) else [prefixes]

        def get_type(self, node: Any, suffix: str = 'param') -> tuple[str, str, Any]:
            if suffix == 'param':
                self.ptr = 0
                self.array = 0

            if isinstance(node.type, c_ast.TypeDecl):
                typename = node.type.declname
                quals = ''
                if node.type.quals:
                    quals = ' '.join(node.type.quals)
                if node.type.type.names:
                    c_type = ' '.join(node.type.type.names)
                    return quals, c_type, typename
            if isinstance(node.type, c_ast.PtrDecl):
                quals, c_type, name = self.get_type(node.type, 'ptr')
                self.ptr += 1
                return quals, c_type, name

            if isinstance(node.type, c_ast.ArrayDecl):
                quals, c_type, name = self.get_type(node.type, 'array')
                self.array = int(node.type.dim.value)
                return quals, c_type, name
            return '', '', None

        def visit_FuncDecl(self, node: c_ast.FuncDecl) -> None:
            if isinstance(node.type, c_ast.TypeDecl):
                func_name = node.type.declname
                if (
                    any(func_name.startswith(prefix) for prefix in self.prefixes)
                    and not func_name.endswith('_t')
                    and func_name in self.content
                ):
                    if func_name in DEPRECATED_API:
                        return
                    ret = node.type.type.names[0]
                    args = []
                    for param in node.args.params:
                        quals, c_type, name = self.get_type(param)
                        param = Param(ptr=self.ptr, array=self.array, qual=quals, type=c_type, name=name)
                        args.append(param)
                    self.function_prototypes[func_name] = (ret, args)

    parser = c_parser.CParser()  # Set debug parameter to False
    ast = parser.parse(header_code)
    visitor = FunctionVisitor(header, prefixes)
    visitor.visit(ast)
    return visitor.function_prototypes


def exec_cmd(what: list[str], out_file: IO[str] | None = None) -> tuple[int, str, str, str]:
    p = subprocess.Popen(
        what,
        stdin=subprocess.PIPE,
        stdout=out_file if out_file is not None else subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    output_b, err_b = p.communicate()
    rc = p.returncode
    output: str = output_b.decode('utf-8') if output_b is not None else ''
    err: str = err_b.decode('utf-8') if err_b is not None else ''
    return rc, output, err, ' '.join(what)


_cached_include_dir_flags: list[str] | None = None


def _handle_missing_tool(msg: str) -> None:
    YELLOW = '\033[33m'
    RESET = '\033[0m'
    full_msg = (
        f'{msg}. WiFi-remote API generation check could not be performed.\n'
        'If you have modified WiFi or Supplicant headers, the corresponding '
        'wifi-remote files may need to be updated.\n'
        'Please set up your ESP-IDF environment (run export.sh) and run this script manually:\n'
        f'cd {os.path.relpath(script_dir, idf_path)} && python3 {os.path.basename(__file__)}\n'
        'Then commit the resulting changes.'
    )
    print(f'{YELLOW}SKIPPED: {full_msg}{RESET}')
    sys.exit(0)


try:
    import idf_build_apps  # noqa: F401
    import pycparser  # noqa: F401
except ImportError:
    _handle_missing_tool('ESP-IDF environment not found (missing python dependencies)')


def preprocess(idf_path: str, header: str) -> str:
    global _cached_include_dir_flags
    project_dir = os.path.join(idf_path, 'examples', 'wifi', 'getting_started', 'station')
    build_dir = os.path.join(project_dir, 'build')

    if _cached_include_dir_flags is None:
        # Clean up build artifacts ONLY on the first run to ensure a fresh state if needed,
        # but idf.py reconfigure is usually good at incremental updates.
        # To be safe and fast, we only clean if we don't have a build dir yet.
        if not os.path.exists(build_dir):
            sdkconfig = os.path.join(project_dir, 'sdkconfig')
            if os.path.exists(sdkconfig):
                os.remove(sdkconfig)

        idf_py = shutil.which('idf.py')
        if idf_py is None:
            idf_py_path = os.path.join(idf_path, 'tools', 'idf.py')
            if os.path.exists(idf_py_path):
                idf_py = f'{sys.executable} {idf_py_path}'
            else:
                _handle_missing_tool('ESP-IDF environment not found')

        assert idf_py is not None
        try:
            if isinstance(idf_py, str) and idf_py.startswith(sys.executable):
                subprocess.run(
                    idf_py.split() + ['-B', build_dir, 'reconfigure'],
                    cwd=project_dir,
                    check=True,
                    stdout=subprocess.DEVNULL,
                    stderr=subprocess.DEVNULL,
                )
            else:
                subprocess.run(
                    [idf_py, '-B', build_dir, 'reconfigure'],
                    cwd=project_dir,
                    check=True,
                    stdout=subprocess.DEVNULL,
                    stderr=subprocess.DEVNULL,
                )
        except (subprocess.CalledProcessError, FileNotFoundError):
            _handle_missing_tool('ESP-IDF environment not found')

        build_commands_json = os.path.join(build_dir, 'compile_commands.json')
        if not os.path.exists(build_commands_json):
            _handle_missing_tool('ESP-IDF environment not found')

        with open(build_commands_json, encoding='utf-8') as f:
            build_command = json.load(f)[0]['command'].split()
        _cached_include_dir_flags = []
        # process compilation flags (includes and defines)
        for item in build_command:
            if item.startswith('-I'):
                _cached_include_dir_flags.append(item)
            if item.startswith('-D'):
                _cached_include_dir_flags.append(
                    item.replace('\\', '')
                )  # removes escaped quotes, eg: -DMBEDTLS_CONFIG_FILE=\\\"mbedtls/esp_config.h\\\"
        _cached_include_dir_flags.append('-I' + os.path.join(build_dir, 'config'))

    temp_file = 'esp_wifi_preprocessed.h'
    with open(temp_file, 'w') as f:
        f.write('#define asm\n')
        f.write('#define volatile\n')
        f.write('#define __asm__\n')
        f.write('#define __volatile__\n')
    with open(temp_file, 'a') as f:
        gcc_cmd = 'xtensa-esp32-elf-gcc'
        if shutil.which(gcc_cmd) is None:
            _handle_missing_tool(f'{gcc_cmd} not found')

        rc, out, err, cmd = exec_cmd(
            [gcc_cmd, '-w', '-P', '-include', 'ignore_extensions.h', '-E', header] + _cached_include_dir_flags, f
        )
        if rc != 0:
            print(f'command {cmd} failed!')
            print(err)
    from pycparser import preprocess_file

    preprocessed_code = preprocess_file(temp_file)
    return cast(str, preprocessed_code)


def get_args(parameters: list[Any]) -> tuple[str, str]:
    params = []
    names = []
    for param in parameters:
        typename = param.type
        if typename == 'void' and param.ptr == 0 and param.name is None:
            params.append(f'{typename}')
            continue
        if param.qual != '':
            typename = f'{param.qual} ' + typename
        declname = param.name
        names.append(f'{declname}')
        if param.ptr > 0:
            declname = '*' * param.ptr + declname
        if param.array > 0:
            declname += f'[{param.array}]'
        params.append(f'{typename} {declname}')
    comma_separated_params = ', '.join(params)
    comma_separated_names = ', '.join(names)
    return comma_separated_params, comma_separated_names


def get_vars(parameters: list[Any]) -> tuple[str, str]:
    definitions = ''
    names = []
    for param in parameters:
        typename = param.type
        if typename == 'void' and param.ptr == 0 and param.name is None:
            continue
        default_value = '0'
        declname = param.name
        names.append(f'{declname}')
        if param.qual != '':
            typename = f'{param.qual} ' + typename
        if param.ptr > 0:
            declname = '*' * param.ptr + declname
            default_value = 'NULL'
        if param.array > 0:
            declname += f'[{param.array}]'
            default_value = '{}'
        definitions += f'        {typename} {declname} = {default_value};\n'
    comma_separated_names = ', '.join(names)
    return definitions, comma_separated_names


def generate_kconfig_wifi_caps(idf_path: str, component_path: str) -> list[str]:
    from idf_build_apps.constants import PREVIEW_TARGETS
    from idf_build_apps.constants import SUPPORTED_TARGETS

    kconfig = os.path.join(component_path, 'Kconfig.soc_wifi_caps.in')
    slave_select = os.path.join(component_path, 'Kconfig.slave_select.in')

    # Read and parse the global Kconfig file for target selections
    target_selections: dict[str, list[str]] = {}
    current_target = None
    with open(os.path.join(idf_path, 'Kconfig')) as f:
        for line in f:
            line = line.strip()
            if line.startswith('config IDF_TARGET_'):
                current_target = line.split()[1]
                target_selections[current_target] = []
            elif current_target and line.strip().startswith('select'):
                selection = line.split()[1]
                target_selections[current_target].append(selection)

    with open(kconfig, 'w') as slave_caps, open(slave_select, 'w') as slave:
        slave_caps.write(f'# {AUTO_GENERATED}\n')
        slave.write(f'# {AUTO_GENERATED}\n')
        slave.write('choice SLAVE_IDF_TARGET\n')
        slave.write('    prompt "choose slave target"\n')
        slave.write('    default SLAVE_IDF_TARGET_ESP32C6 if IDF_TARGET_ESP32P4  # To support common use-cases\n')
        slave.write('    default SLAVE_IDF_TARGET_ESP32\n\n')

        for slave_target in SUPPORTED_TARGETS + PREVIEW_TARGETS:
            add_slave = False
            kconfig_content = []
            soc_caps = os.path.join(
                idf_path, 'components', 'soc', slave_target, 'include', 'soc', 'Kconfig.soc_caps.in'
            )
            try:
                with open(soc_caps) as f:
                    for line in f:
                        if line.strip().startswith('config SOC_WIFI_'):
                            if 'config SOC_WIFI_SUPPORTED' in line:
                                # if WiFi supported for this target, add it to Kconfig slave options and test this slave
                                add_slave = True
                            replaced = re.sub(r'SOC_WIFI_', 'SLAVE_SOC_WIFI_', line.strip())
                            kconfig_content.append(f'    {replaced} {KCONFIG_MULTIPLE_DEF}\n')
                            kconfig_content.append(f'    {f.readline()}')  # type
                            kconfig_content.append(f'    {f.readline()}\n')  # default
            except FileNotFoundError:
                print(f'Warning: File {soc_caps} not found. Skipping target {slave_target}.')
                continue
            except OSError as e:
                print(f'Error reading file {soc_caps}: {e}')
                continue

            if add_slave:
                slave_caps.write(f'\nif SLAVE_IDF_TARGET_{slave_target.upper()}\n\n')
                slave_caps.writelines(kconfig_content)

                # Add any target-specific selections from global Kconfig
                target_key = f'IDF_TARGET_{slave_target.upper()}'
                if target_key in target_selections:
                    for selection in target_selections[target_key]:
                        slave_caps.write(f'    config SLAVE_{selection} {KCONFIG_MULTIPLE_DEF}\n')
                        slave_caps.write('        bool\n')
                        slave_caps.write('        default y\n\n')

                slave_caps.write(f'endif # {slave_target.upper()}\n')

                slave_config_name = 'SLAVE_IDF_TARGET_' + slave_target.upper()
                slave.write(f'    config {slave_config_name}\n')
                slave.write(f'        bool "{slave_target}"\n')

        slave.write('endchoice\n')
    return [kconfig, slave_select]


def generate_remote_wifi_api(function_prototypes: FunctionPrototypes, component_path: str) -> list[str]:
    header = os.path.join(component_path, 'include', 'esp_wifi_remote_api.h')
    wifi_source = os.path.join(component_path, 'esp_wifi_with_remote.c')
    remote_source = os.path.join(component_path, 'esp_wifi_remote_weak.c')
    with open(header, 'w') as f:
        f.write(COPYRIGHT_HEADER)
        f.write('#pragma once\n#include "esp_err.h"\n#include "esp_wifi.h"\n')
        f.write('\n#ifdef __cplusplus\nextern "C" {\n#endif\n\n')
        for func_name, args in function_prototypes.items():
            params, _ = get_args(args[1])
            remote_func_name = NAMESPACE.sub('esp_wifi_remote', func_name)
            f.write(f'{args[0]} {remote_func_name}({params});\n')
        f.write('\n#ifdef __cplusplus\n}\n#endif\n')
    with open(wifi_source, 'w') as wifi, open(remote_source, 'w') as remote:
        wifi.write(COPYRIGHT_HEADER)
        wifi.write('#include "esp_wifi.h"\n')
        wifi.write('#include "esp_wifi_remote.h"\n')
        remote.write(COPYRIGHT_HEADER)
        remote.write('#include "esp_wifi_remote.h"\n')
        remote.write('#include "esp_log.h"\n\n')
        remote.write('#define WEAK __attribute__((weak))\n')
        remote.write(
            '#define LOG_UNSUPPORTED_AND_RETURN(ret) '
            'ESP_LOGW("esp_wifi_remote_weak", "%s unsupported", __func__); \\\n'
            '         return ret;\n'
        )
        for func_name, args in function_prototypes.items():
            remote_func_name = NAMESPACE.sub('esp_wifi_remote', func_name)
            params, names = get_args(args[1])
            ret_type = args[0]
            ret_value = '-1'  # default return value indicating error
            if ret_type == 'esp_err_t':
                ret_value = 'ESP_ERR_NOT_SUPPORTED'
            wifi.write(f'\n{args[0]} {func_name}({params})\n')
            wifi.write('{\n')
            wifi.write(f'    return {remote_func_name}({names});\n')
            wifi.write('}\n')
            remote.write(f'\nWEAK {args[0]} {remote_func_name}({params})\n')
            remote.write('{\n')
            remote.write(f'    LOG_UNSUPPORTED_AND_RETURN({ret_value});\n')
            remote.write('}\n')
    return [header, wifi_source, remote_source]


def generate_remote_eap_api(function_prototypes: FunctionPrototypes, component_path: str) -> list[str]:
    header = os.path.join(component_path, 'include', 'esp_eap_client_remote_api.h')
    eap_source = os.path.join(component_path, 'esp_eap_client_with_remote.c')
    remote_source = os.path.join(component_path, 'esp_eap_client_remote_weak.c')
    with open(header, 'w') as f:
        f.write(COPYRIGHT_HEADER)
        f.write('#pragma once\n')
        f.write('#include "esp_eap_client.h"\n')
        f.write('\n#ifdef __cplusplus\nextern "C" {\n#endif\n\n')
        for func_name, args in function_prototypes.items():
            params, _ = get_args(args[1])
            # Handle esp_wifi functions differently - map them to esp_wifi_remote
            if func_name.startswith('esp_wifi_'):
                remote_func_name = NAMESPACE.sub('esp_wifi_remote', func_name)
            else:
                remote_func_name = func_name.replace('esp_eap_client', 'esp_eap_client_remote')
            f.write(f'{args[0]} {remote_func_name}({params});\n')
        f.write('\n#ifdef __cplusplus\n}\n#endif\n')
    with open(eap_source, 'w') as eap, open(remote_source, 'w') as remote:
        eap.write(COPYRIGHT_HEADER)
        eap.write('#include "esp_eap_client.h"\n')
        eap.write('#include "esp_eap_client_remote_api.h"\n')
        remote.write(COPYRIGHT_HEADER)
        remote.write('#include "esp_eap_client_remote_api.h"\n')
        remote.write('#include "esp_log.h"\n\n')
        remote.write('#define WEAK __attribute__((weak))\n')
        remote.write(
            '#define LOG_UNSUPPORTED_AND_RETURN(ret) '
            'ESP_LOGW("esp_eap_client_remote_weak", "%s unsupported", __func__); \\\n'
            '         return ret;\n'
        )
        remote.write(
            '#define LOG_UNSUPPORTED_VOID() ESP_LOGW("esp_eap_client_remote_weak", "%s unsupported", __func__);\n'
        )
        for func_name, args in function_prototypes.items():
            # Handle esp_wifi functions differently - map them to esp_wifi_remote
            if func_name.startswith('esp_wifi_'):
                remote_func_name = NAMESPACE.sub('esp_wifi_remote', func_name)
            else:
                remote_func_name = func_name.replace('esp_eap_client', 'esp_eap_client_remote')
            params, names = get_args(args[1])
            ret_type = args[0]
            ret_value = '-1'  # default return value indicating error
            if ret_type == 'esp_err_t':
                ret_value = 'ESP_ERR_NOT_SUPPORTED'
            eap.write(f'\n{args[0]} {func_name}({params})\n')
            eap.write('{\n')
            if ret_type == 'void':
                eap.write(f'    {remote_func_name}({names});\n')
            else:
                eap.write(f'    return {remote_func_name}({names});\n')
            eap.write('}\n')
            remote.write(f'\nWEAK {args[0]} {remote_func_name}({params})\n')
            remote.write('{\n')
            if ret_type == 'void':
                remote.write('    LOG_UNSUPPORTED_VOID();\n')
            else:
                remote.write(f'    LOG_UNSUPPORTED_AND_RETURN({ret_value});\n')
            remote.write('}\n')
    return [header, eap_source, remote_source]


def generate_hosted_mocks(function_prototypes: FunctionPrototypes, component_path: str) -> list[str]:
    source = os.path.join(component_path, 'test', 'smoke_test', 'components', 'esp_hosted', 'esp_hosted_mock.c')
    header = os.path.join(
        component_path, 'test', 'smoke_test', 'components', 'esp_hosted', 'include', 'esp_hosted_mock.h'
    )
    os.makedirs(os.path.dirname(source), exist_ok=True)
    os.makedirs(os.path.dirname(header), exist_ok=True)
    with open(source, 'w') as f, open(header, 'w') as h:
        f.write(COPYRIGHT_HEADER)
        h.write(COPYRIGHT_HEADER)
        h.write('#pragma once\n')
        f.write('#include "esp_wifi.h"\n')
        f.write('#include "esp_wifi_remote.h"\n')
        for func_name, args in function_prototypes.items():
            hosted_func_name = NAMESPACE.sub('esp_wifi_remote', func_name)
            params, names = get_args(args[1])
            ret_type = args[0]
            ret_value = '0'  # default return value
            if ret_type == 'esp_err_t':
                ret_value = 'ESP_OK'
            f.write(f'\n{ret_type} {hosted_func_name}({params})\n')
            f.write('{\n')
            f.write(f'    return {ret_value};\n')
            f.write('}\n')
            h.write(f'{ret_type} {hosted_func_name}({params});\n')
        return [source, header]


def generate_wifi_native(idf_path: str, component_path: str) -> list[str]:
    native_headers = []

    def replace_configs(original: str, replaced: str) -> str:
        with open(original) as f:
            content = f.read()
        content = content.replace(r'CONFIG_ESP_WIFI_', 'CONFIG_WIFI_RMT_')
        content = content.replace(r'CONFIG_SOC_WIFI_', 'CONFIG_SLAVE_SOC_WIFI_')
        content = content.replace(r'CONFIG_FREERTOS_', 'CONFIG_SLAVE_FREERTOS_')
        content = content.replace(r'CONFIG_IDF_TARGET_', 'CONFIG_SLAVE_IDF_TARGET_')
        with open(replaced, 'w') as f:
            f.write(content)
        return replaced

    include_dir = os.path.join(idf_path, 'components', 'esp_wifi', 'include')
    remote_dir = os.path.join(component_path, 'include', 'injected')
    os.makedirs(remote_dir, exist_ok=True)

    header_mappings = [
        ('local/esp_wifi_types_native.h', 'esp_wifi_types_native.h'),
        ('esp_wifi.h', 'esp_wifi.h'),
        ('esp_wifi_types_generic.h', 'esp_wifi_types_generic.h'),
        ('esp_wifi_he_types.h', 'esp_wifi_he_types.h'),
        ('esp_wifi_types.h', 'esp_wifi_types.h'),
    ]

    for src_rel, dest_name in header_mappings:
        original = os.path.join(include_dir, src_rel)
        replaced = os.path.join(remote_dir, dest_name)
        native_headers.append(replace_configs(original, replaced))

    # Copy remaining headers
    for pattern in ['esp_wifi*.h', 'esp_mesh*.h', 'esp_now.h']:
        for header in glob.glob(os.path.join(include_dir, pattern)):
            dest = os.path.join(remote_dir, os.path.basename(header))
            if dest not in native_headers:
                shutil.copy(header, dest)
                native_headers.append(dest)

    return native_headers


def get_global_configs(idf_path: str) -> set[str]:
    """Extract global configs from Kconfig that are selected by IDF_TARGET configs"""
    global_configs = set()
    current_target = None
    with open(os.path.join(idf_path, 'Kconfig')) as f:
        for line in f:
            line = line.strip()
            if line.startswith('config IDF_TARGET_'):
                current_target = True
            elif current_target and line.strip().startswith('select'):
                selection = line.split()[1]
                global_configs.add(selection)
            elif line.startswith('config') or line.startswith('choice'):
                current_target = False
    return global_configs


def generate_kconfig(idf_path: str, component_path: str) -> list[str]:
    # Define file paths
    remote_kconfig = os.path.join(component_path, 'Kconfig.wifi.in')
    remote_kconfig_is_remote = os.path.join(component_path, 'Kconfig.wifi_is_remote.in')
    esp_wifi_kconfig = os.path.join(idf_path, 'components', 'esp_wifi', 'Kconfig')
    # Variables for accumulating configuration details
    remote_configs = ''
    current_config = None
    current_config_indent = 0
    depends_on = None

    # Config prefixes to be replaced
    base_configs = ['SOC_WIFI_', 'IDF_TARGET_']
    # Get global configs from the original Kconfig (assumed to be defined elsewhere)
    global_configs = get_global_configs(idf_path)

    # Read the source Kconfig lines
    with open(esp_wifi_kconfig) as f:
        lines = f.readlines()

    # Setup counters and flags for nested ifs and choice blocks
    nesting_threshold = 100  # Initial high threshold; will be adjusted later
    nested_if = 0
    initial_indent = -1  # Will be set once the first config/choice line is encountered
    inside_choice = False

    with open(remote_kconfig, 'w') as outfile:

        def _indent_len(line: str) -> int:
            return len(line) - len(line.lstrip())

        # Write header comments
        outfile.write('# Wi-Fi configuration\n')
        outfile.write(f'# {AUTO_GENERATED}\n')

        for original_line in lines:
            stripped_line = original_line.strip()

            # Update nesting level counters for if/endif blocks
            if re.match(r'^if\s+[A-Z_0-9]+\s*$', stripped_line):
                nested_if += 1
            elif stripped_line.startswith('endif'):
                nested_if -= 1

            # Track whether we are inside a choice block
            if re.match(r'^choice', stripped_line):
                inside_choice = True
            elif stripped_line.startswith('endchoice'):
                inside_choice = False

            # Process lines after reaching the nesting threshold
            if nested_if >= nesting_threshold:
                # Capture the initial indentation level when encountering the first config/choice
                if initial_indent == -1 and re.match(r'^(config|choice)\s+ESP_WIFI_', stripped_line):
                    initial_indent = _indent_len(original_line)

                # Make a copy of the original line for modifications
                modified_line = original_line
                # Replace base configuration prefixes
                for config in base_configs:
                    modified_line = re.compile(config).sub('SLAVE_' + config, modified_line)
                # Replace global config names (ensuring whole word match)
                for config in global_configs:
                    modified_line = re.compile(r'\b' + config + r'\b').sub('SLAVE_' + config, modified_line)
                # Replace the primary prefix ESP_WIFI_ with WIFI_RMT_
                modified_line = re.compile(r'\bESP_WIFI_').sub('WIFI_RMT_', modified_line)
                # TODO: Capture all host dependencies and replace them with WIFI_RMT_ prefix
                # keep track and check integrity with slave settings
                modified_line = re.compile(r'\bESP_PHY_IRAM_OPT').sub('WIFI_RMT_PHY_IRAM_OPT', modified_line)
                # Kconfig.wifi.in lives under components/esp_wifi/remote, so inherited
                # rsource paths from components/esp_wifi/Kconfig need one level up.
                # source_match = re.match(r'^(\s*rsource\s+")([^"]+)(".*)$', modified_line)
                # if source_match and not source_match.group(2).startswith('../'):
                #     modified_line = f'{source_match.group(1)}../{source_match.group(2)}{source_match.group(3)}\n'

                # If a current config block is active, check for changes in indentation
                if current_config is not None:
                    # If the current line is still within the current config block (by indent)
                    if _indent_len(original_line) > current_config_indent:
                        # Check for config properties: type (int or bool) or dependency
                        for keyword in ['int', 'bool', 'depends']:
                            if re.match(rf'^{keyword}', stripped_line):
                                if keyword == 'depends':
                                    depends_on = modified_line.strip()
                                else:
                                    config_type = keyword
                                    # Prepare the default remote value by replacing the prefix
                                    remote_value = current_config.replace('ESP_WIFI_', 'WIFI_RMT_')
                    else:
                        # End of the current config block; append the accumulated remote config block
                        config_indent = ''
                        if config_type == 'bool':
                            remote_configs += f'if {remote_value}\n'
                            config_indent = '    '
                        remote_configs += f'{config_indent}config {current_config} {KCONFIG_MULTIPLE_DEF}\n'
                        remote_configs += f'{config_indent}    {config_type}\n'
                        if depends_on is not None:
                            remote_configs += f'{config_indent}    {depends_on}\n'
                            depends_on = None
                        remote_configs += f'{config_indent}    default {remote_value}\n'
                        if config_type == 'bool':
                            remote_configs += 'endif\n'
                        remote_configs += '\n'
                        current_config = None

                # Check if the line declares a config or choice for ESP_WIFI_
                match = re.match(r'^(config|choice)\s+(ESP_WIFI_[A-Z0-9_]+)', stripped_line)
                if match:
                    wifi_configs.append(match.group(2))
                    # Only treat non-choice config declarations as starting a config block
                    if match.group(1) == 'config' and not inside_choice:
                        current_config = match.group(2)
                        current_config_indent = _indent_len(modified_line)

                # Adjust indentation relative to the captured initial indent
                if len(modified_line) > initial_indent and modified_line[:initial_indent].isspace():
                    modified_line = modified_line[initial_indent:]
                outfile.write(modified_line)

            # When an ESP_WIFI_ENABLED condition is encountered, update the nesting threshold
            if re.match(r'^if\s+\(?ESP_WIFI_ENABLED', stripped_line):
                nesting_threshold = nested_if

        # Append the accumulated remote configurations under the disabled ESP_WIFI condition
        outfile.write('if !ESP_WIFI_ENABLED\n')
        outfile.write('    rsource "Kconfig.wifi_is_remote.in"\n')
        outfile.write('endif # ESP_WIFI_ENABLED\n')
        outfile.write('# Wi-Fi configuration end\n')
    with open(remote_kconfig_is_remote, 'w') as outfile:
        outfile.write(f'# {AUTO_GENERATED}\n')
        outfile.write(remote_configs.rstrip('\n') + '\n')

    return [remote_kconfig, remote_kconfig_is_remote]


def compare_files(base_dir: str, component_path: str, files_to_check: list[str]) -> list[tuple[str, str]]:
    failures = []
    for file_path in files_to_check:
        relative_path = os.path.relpath(file_path, component_path)
        base_file = os.path.join(base_dir, relative_path)

        if not os.path.exists(base_file):
            failures.append((relative_path, 'File does not exist in base directory'))
            continue

        diff_cmd = ['diff', '-I', 'Copyright', file_path, base_file]
        result = subprocess.run(diff_cmd, capture_output=True)

        if result.returncode != 0:  # diff returns 0 if files are identical
            failures.append((relative_path, result.stdout.decode('utf-8')))

    return failures


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Build all projects',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        epilog="""\
TEST FAILED
-----------
Some of the component files are different from the pregenerated output.
Please check the files that marked as "FAILED" in this step,
typically you'd just need to commit the changes and create a new version.
Please be aware that the pregenerated files use the same copyright header, so after
making changes you might need to modify 'copyright_header.h' in the script directory.
        """,
    )
    parser.add_argument(
        '-s', '--skip-check', help='Skip checking the versioned files against the re-generated', action='store_true'
    )
    parser.add_argument('--base-dir', help='Base directory to compare generated files against')
    args = parser.parse_args()

    header = os.path.join(idf_path, 'components', 'esp_wifi', 'include', 'esp_wifi.h')
    eap_header = os.path.join(idf_path, 'components', 'wpa_supplicant', 'esp_supplicant', 'include', 'esp_eap_client.h')
    function_prototypes = extract_function_prototypes(preprocess(idf_path, header), header, ['esp_wifi_'])
    eap_function_prototypes = extract_function_prototypes(
        preprocess(idf_path, eap_header), eap_header, ['esp_eap_client_', 'esp_wifi_']
    )

    files_to_check = []

    files_to_check += generate_kconfig_wifi_caps(idf_path, component_path)
    files_to_check += generate_remote_wifi_api(function_prototypes, component_path)
    files_to_check += generate_remote_eap_api(eap_function_prototypes, component_path)
    files_to_check += generate_hosted_mocks(function_prototypes, component_path)
    files_to_check += generate_wifi_native(idf_path, component_path)
    files_to_check += generate_kconfig(idf_path, component_path)

    modified_files = []
    for f in files_to_check:
        if os.path.exists(f):
            # Check if file is modified relative to index
            rc, _, _, _ = exec_cmd(['git', 'diff', '--exit-code', f])
            if rc != 0:
                modified_files.append(f)

    if modified_files:
        print('WiFi-remote API files were updated:')
        for f in modified_files:
            print(f'  modified: {os.path.relpath(f, idf_path)}')
        print('\nPlease stage these changes and try committing again.')

    if args.skip_check or args.base_dir is None:
        if modified_files:
            sys.exit(1)
        sys.exit(0)

    failures = compare_files(args.base_dir, component_path, files_to_check)

    if failures:
        print(parser.epilog)
        print('\nDifferent files:\n')
        for file, diff in failures:
            print(f'{file}\nChanges:\n{diff}')
        exit(1)
    else:
        print('All files are identical to the base directory.')
        exit(0)
