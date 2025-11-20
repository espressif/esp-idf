# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# The current project needs to support environments before Python 3.9,
# and UP007 will prohibit the use of Tuple, Union, etc.
# ruff: noqa: UP007

# The current project needs to support environments before Python 3.9,
# Therefore, it is necessary to prohibit UP006 from automatically
# changing the annotation type
# ruff: noqa: UP006
# ruff: noqa: UP035

"""
BLE Log Compression Utility
===========================
This script processes Bluetooth source files to compress logging statements.
"""

import argparse
import enum
import importlib.util
import logging
import os
import re
import shutil
import sys
import textwrap
import traceback
from datetime import datetime
from pathlib import Path
from typing import Any
from typing import Dict
from typing import List
from typing import Tuple
from typing import Union
from typing import cast

import tree_sitter_c as tsc
import yaml
from c_format_parse import parse_format_string
from inttypes_map import TYPES_MACRO_MAP
from LogDBManager import LogDBManager

try:
    import importlib.metadata as meta

    _TS_VER = tuple(map(int, meta.version('tree-sitter').split('.')[:2]))
except Exception:
    _TS_VER = (0, 20)

if _TS_VER >= (0, 21):
    from tree_sitter import Language
    from tree_sitter import Node
    from tree_sitter import Parser
    from tree_sitter import Query
    from tree_sitter import Tree

    if _TS_VER >= (0, 25):
        from tree_sitter import QueryCursor
else:
    from tree_sitter import Language
    from tree_sitter import Parser
    from tree_sitter import Tree

# Initialize logger
LOGGER = logging.getLogger('ble_log_compression')

# Global parser instances
C_LANGUAGE: Union[Language, None] = None
CLANG_PARSER: Union[Parser, None] = None

# Log source enumeration
SOURCE_ENUM_MAP = {
    'BLE_HOST': 0,
    'BLE_MESH': 1,
    'BLE_MESH_LIB': 2,
}

# Functions that require hex formatting
HEX_FUNCTIONS = {
    # func_name: (arg_cnt, buf_idx, buf_len)
    # Negative buf_len indicates constant buffer size
    'bt_hex': (2, 0, 1),  # Used in Mesh and Audio modules
    'MAC2STR': (1, 0, -6),  # Used in Bluedroid Host
}

# C keywords to exclude from function names
C_KEYWORDS = {
    'auto',
    'break',
    'case',
    'char',
    'const',
    'continue',
    'default',
    'do',
    'double',
    'else',
    'enum',
    'extern',
    'float',
    'for',
    'goto',
    'if',
    'int',
    'long',
    'register',
    'return',
    'short',
    'signed',
    'sizeof',
    'static',
    'struct',
    'switch',
    'typedef',
    'union',
    'unsigned',
    'void',
    'volatile',
    'while',
}

FUNC_MACROS = {'__func__', '__FUNCTION__'}

LINE_MACROS = {
    '__LINE__',
}


class ARG_SIZE_TYPE(enum.IntEnum):
    U32 = 0
    STR = 1
    U64 = 2
    LZU32 = 3
    LZU64 = 4
    AZU32 = 5
    AZU64 = 6


def TsInit() -> Tuple[Language, Parser]:
    if _TS_VER > (0, 21):
        lang = Language(tsc.language())
        parser = Parser(lang)
        return lang, parser
    else:
        lang = Language(tsc.language(), 'c')
        parser = Parser()
        parser.set_language(lang)
        return lang, parser


def TsQueryByTree(language: Language, tree: Tree, query_str: str) -> Dict[str, List[Node]]:
    if _TS_VER == (0, 21):
        captures_res = language.query(query_str).captures(tree.root_node)
    elif _TS_VER > (0, 21) and _TS_VER < (0, 25):
        captures_res = Query(language, (query_str)).captures(tree.root_node)
    else:
        captures_res = QueryCursor(Query(language, (query_str))).captures(tree.root_node)

    captures: Dict[str, List[Node]] = {}
    if isinstance(captures_res, list):
        for node, node_type in captures_res:
            if node_type not in captures:
                captures[node_type] = []
            captures[node_type].append(node)
    else:
        captures = captures_res
    return captures


class LogCompressor:
    """Main class for BLE log compression."""

    def __init__(self) -> None:
        self.code_base_path = Path()
        self.build_dir = Path()
        self.bt_compressed_srcs_path = Path()
        self.config: dict[str, Any] = {}
        self.module_info: dict[str, Any] = {}
        self.module_mod: dict[str, Any] = {}

    def init_parser(self) -> Parser:
        """Initialize tree-sitter parser for C."""
        global C_LANGUAGE, CLANG_PARSER

        C_LANGUAGE, CLANG_PARSER = TsInit()

        return CLANG_PARSER

    def extract_log_calls(self, code_content: bytes, log_tags: list[str]) -> list[dict]:
        """
        Extract log statements from C source code.

        Args:
            code_content: Source code as bytes
            log_tags: List of log tags to search for

        Returns:
            List of dictionaries containing log information
        """
        parser = self.init_parser()
        tree = parser.parse(code_content)
        function_map = self._get_function_boundaries(tree)
        return self._find_log_statements(tree, log_tags, function_map)

    def _get_function_boundaries(self, tree: Tree) -> list[tuple[str, int, int, int]]:
        """
        Identify function boundaries in the AST.

        Returns:
            List of tuples (function_name, start_byte, end_byte, line_number)
        """
        function_query = """
        [
            (function_declarator
                (identifier) @func_name
            )
            (declaration
                (function_declarator
                (identifier) @func_decls
                )
            )
        ]
        """
        captures: dict[str, list[Node]] = TsQueryByTree(C_LANGUAGE, tree, function_query)

        # Filter valid function names
        func_names = [
            node
            for node in captures.get('func_name', [])
            if node not in captures.get('func_decls', []) and node.text.decode('utf-8') not in C_KEYWORDS
        ]

        # Sort by start byte
        func_names.sort(key=lambda node: node.start_byte)
        boundaries = []

        for i, node in enumerate(func_names):
            func_name = node.text.decode('utf-8')
            start_byte = node.start_byte
            end_byte = func_names[i + 1].start_byte if i < len(func_names) - 1 else len(tree.root_node.text)
            line_number = node.start_point[0] + 1
            boundaries.append((func_name, start_byte, end_byte, line_number))

        return boundaries

    def _find_log_statements(
        self, tree: Tree, log_tags: list[str], function_boundaries: list[tuple[str, int, int, int]]
    ) -> list[dict]:
        """
        Find log statements in the AST.

        Args:
            tree: Parsed AST tree
            log_tags: List of log tags to search for
            function_boundaries: Function boundaries list

        Returns:
            List of log information dictionaries
        """
        # Build pattern for matching log tags
        tag_pattern = '|'.join(log_tags)
        log_query = f"""
        (expression_statement
            (call_expression
                function: (identifier) @fname
                arguments: (argument_list) @args
                (#match? @fname "^({tag_pattern})$")
            ) @log_stmt
        )
        """
        captures: dict[str, list[Node]] = TsQueryByTree(C_LANGUAGE, tree, log_query)
        log_nodes = captures.get('log_stmt', [])

        # It is necessary to ensure that the nodes
        # are sorted according to the starting bytes,
        # because only the nodes are ordered can ensure
        # that the subsequent calculation offset is correct.
        log_nodes.sort(key=lambda node: node.start_byte)
        logs = []

        for node in log_nodes:
            try:
                log_info = self._process_log_node(node, function_boundaries)
                if log_info:
                    logs.append(log_info)
            except Exception as e:
                LOGGER.error(f'Error processing log node: {e}\n{traceback.format_exc()}')
                raise

        return logs

    def _process_log_node(self, node: Node, function_boundaries: list[tuple[str, int, int, int]]) -> Union[dict, None]:
        """
        Process a log AST node and extract information.

        Args:
            node: Log statement AST node
            function_boundaries: Function boundaries list

        Returns:
            Log information dictionary or None if invalid
        """
        # Extract basic information
        tag_node = node.child_by_field_name('function')
        if not tag_node:
            return None

        tag = tag_node.text.decode('utf-8')

        args_node = node.child_by_field_name('arguments')
        if not args_node or args_node.type != 'argument_list':
            return None

        # Initialize log info
        log_info = {
            'tag': (tag, tag_node.start_byte, tag_node.end_byte),
            'line_number': node.start_point[0] + 1,
            'arguments': [],
            'hexify': True,
        }

        # Process format string (first argument)
        valid_arg_childrn: list[Node] = [n for n in args_node.named_children if n.type != 'comment']
        fmt_node = valid_arg_childrn[0] if len(valid_arg_childrn) > 0 else None
        if not fmt_node:
            return None

        if fmt_node.type == 'concatenated_string':
            log_fmt = self._process_concatenated_string(fmt_node)
        elif fmt_node.type == 'string_literal':
            log_fmt = fmt_node.text.decode('utf-8')[1:-1]  # Remove quotes
        else:
            return None

        log_info['arguments'].append((f'"{log_fmt}"', fmt_node.start_byte, fmt_node.end_byte))

        # Parse format tokens
        tokens = parse_format_string(f'"{log_fmt}"')
        tokens_tuple_map: list[int] = []
        for idx, tk in enumerate(tokens):
            if isinstance(tk, tuple):
                tokens_tuple_map.append(idx)

        arguments: list[Node] = valid_arg_childrn[1:]

        if len(arguments) != len(tokens_tuple_map):
            raise SyntaxError(f'LogSyntaxError:{node.text.decode("utf-8")}')

        # Process each argument
        for i, (token, arg_node) in enumerate(zip([t for t in tokens if isinstance(t, tuple)], arguments)):
            arg_text = arg_node.text.decode('utf-8')
            log_info['arguments'].append((arg_text, arg_node.start_byte, arg_node.end_byte))

            # Check if argument can be hexified
            # if not self._can_be_hexified(token, arg_node):
            #     log_info['hexify'] = False

            # Handle special identifiers
            if arg_text in FUNC_MACROS:
                token_list = list(token)
                token_list[6] = '@func'  # Modify conversion char to special marker
                tokens[tokens_tuple_map[i]] = tuple(token_list)
            elif arg_text in LINE_MACROS:
                token_list = list(token)
                token_list[6] = '@line'
                tokens[tokens_tuple_map[i]] = tuple(token_list)

            # Handle hex functions
            if (
                arg_node.type == 'call_expression'
                and arg_node.child_by_field_name('function')
                and arg_node.child_by_field_name('function').text.decode('utf-8') in HEX_FUNCTIONS.keys()
            ):
                # Extract arguments of the hex function
                hex_func_name = arg_node.child_by_field_name('function').text.decode('utf-8')
                hex_args = arg_node.child_by_field_name('arguments')
                hex_func_info = HEX_FUNCTIONS[hex_func_name]
                if hex_args and hex_args.named_child_count == hex_func_info[0]:
                    buf_node = hex_args.named_children[hex_func_info[1]].text.decode('utf-8')
                    if hex_func_info[2] < 0:
                        len_node = abs(hex_func_info[2])
                    else:
                        len_node = hex_args.named_children[hex_func_info[2]].text.decode('utf-8')
                    token_list = list(token)
                    token_list[6] = f'@hex_func@{buf_node}@{len_node}'
                    tokens[tokens_tuple_map[i]] = tuple(token_list)

        log_info['argu_tokens'] = tokens

        # Find calling function
        caller_info = self._find_calling_function(node.start_byte, function_boundaries)
        if not caller_info:
            return None

        log_info.update(caller_info)
        return log_info

    def _process_concatenated_string(self, node: Node) -> str:
        """Process a concatenated string node into a single string."""
        parts = []
        for child in node.named_children:
            if child.type == 'identifier':
                identifier = child.text.decode('utf-8')
                if identifier in TYPES_MACRO_MAP:
                    parts.append(TYPES_MACRO_MAP[identifier])
                else:
                    raise ValueError(f'Unknown format macro: {identifier}')
            elif child.type == 'string_literal':
                parts.append(child.text.decode('utf-8')[1:-1])  # Remove quotes
            else:
                raise ValueError(f'Unsupported node in concatenated string: {child.type}')
        return ''.join(parts)

    def _can_be_hexified(self, token: tuple[int, int, str, str, str, str, str], node: Node) -> bool:
        """Determine if a node can be represented in hex format."""
        if token[-1] != 's':
            return True

        if node.type == 'identifier' and node.text.decode('utf-8') in FUNC_MACROS:
            return True

        if (
            node.type == 'call_expression'
            and node.child_by_field_name('function')
            and node.child_by_field_name('function').text.decode('utf-8') in HEX_FUNCTIONS.keys()
        ):
            return True

        return False

    def _find_calling_function(
        self, log_start: int, function_boundaries: list[tuple[str, int, int, int]]
    ) -> Union[dict, None]:
        """
        Find the function containing the log statement.

        Args:
            log_start: Start byte of the log statement
            function_boundaries: List of function boundaries

        Returns:
            Dictionary with caller information or None if not found
        """
        for name, start, end, line in function_boundaries:
            if start <= log_start < end:
                return {'caller_name': name, 'caller_line_number': line}
        return None

    def generate_compressed_macro(
        self, source: str, log_idx: int, tag: str, print_fmt: Union[str, None], log_info: dict
    ) -> str:
        """
        Generate a compressed log macro definition.

        Args:
            source: Log source module
            log_idx: Unique log index
            tag: Original log tag
            print_fmt: Simplified format string
            log_info: Log information dictionary

        Returns:
            Macro definition string
        """
        if not log_idx:
            return ''

        source_value = SOURCE_ENUM_MAP.get(source.upper())
        if source_value is None:
            raise ValueError(f'Invalid source: {source}')

        macro = f'#define {tag}_{log_idx}(fmt, ...) {{\\\n'

        if log_info['hexify']:
            # Count of arguments that are not special (__func__, __LINE__, etc.)
            arg_tokens = [t for t in log_info['argu_tokens'] if isinstance(t, tuple)]
            arg_count = len(arg_tokens)
            arguments = []
            sizes = []
            hex_func: list[str] = []

            LOGGER.info(f'{arg_tokens}:{[a[0] for a in log_info["arguments"]]}')

            for token, argument in zip(
                arg_tokens,
                [a[0] for a in log_info['arguments'][1:]],
            ):
                # Skip special tokens
                if token[6] in ('@func', '@line'):
                    arg_count -= 1
                    continue

                # Handle hex function
                if token[6].startswith('@hex_func'):
                    if not hex_func:
                        hex_func = []
                    hex_func.append(token[6])
                    arg_count -= 1
                    continue

                arguments.append(argument)

                if token[6] == 'f' or token[5] == 'll':  # float or long long
                    sizes.append(f'{int(ARG_SIZE_TYPE.U64)}')
                elif token[6] == 's':
                    sizes.append(f'{int(ARG_SIZE_TYPE.STR)}')
                else:
                    sizes.append(f'{int(ARG_SIZE_TYPE.U32)}')

            stmt = self.module_mod[source].gen_compressed_stmt(
                log_idx,
                source_value,
                tag,
                log_info['arguments'][0],
                [{'name': arg, 'size_type': size_type} for arg, size_type in zip(arguments, sizes)],
                [
                    {
                        'buffer': hex_str.split('@')[2],
                        'length': hex_str.split('@')[3],
                    }
                    for hex_str in hex_func
                ],
            )
            macro += cast(str, stmt)

            if (
                'tags_with_preserve' in self.module_info[source]
                and tag in self.module_info[source]['tags_with_preserve']
            ):
                macro += f'    {tag}(fmt, ##__VA_ARGS__);\\\n'
        else:
            # Non-hexified log
            raise ValueError('Hexify convert failed')

        macro += '}\n'
        return macro

    def compress_file(self, file_info: tuple[str, str]) -> list[tuple[str, int, str]]:
        """
        Process a single file for log compression.

        Args:
            file_info: Tuple of (module_name, file_path)

        Returns:
            List of generated macros (module, log_id, macro)
        """
        module, file_path = file_info
        generated_macros = []

        try:
            with open(file_path, 'rb') as f:
                content = f.read()

            new_content = bytearray(content)
            logs = self.extract_log_calls(content, self.module_info[module]['tags'])
            LOGGER.info(f'Processing {file_path} - found {len(logs)} logs')
            offset = 0  # Track cumulative changes due to tag replacements

            for log in logs:
                tag = log['tag'][0]

                # Extract existing index if present
                if match := re.fullmatch(r'(.+)_([0-9A-F]{8})', tag):
                    tag_base = match.group(1)
                    file_index = match.group(2)
                else:
                    tag_base = tag
                    file_index = None

                # Generate simplified format string
                no_buf_fmt: str = ''
                simple_fmt_list: list[str] = []
                hex_buffer_cnt = 0
                for token in log['argu_tokens']:
                    if isinstance(token, tuple):
                        if '@func' in token[6] or '@line' in token[6]:
                            continue
                        if '@hex_func' in token[6]:
                            simple_fmt_list.append(f'@hex_buffer{hex_buffer_cnt}')
                            no_buf_fmt += f'@hex_buffer{hex_buffer_cnt}'
                            hex_buffer_cnt += 1
                            continue
                        simple_fmt_list.append(token[2])
                        no_buf_fmt += token[2]
                    else:
                        no_buf_fmt += token
                simple_fmt_str = ' '.join(simple_fmt_list) if simple_fmt_list else None

                # Add to database
                result, db_index = self.db_manager.add_log(
                    source=module,
                    log_tag=tag_base,
                    log_format=no_buf_fmt if log['hexify'] else log['arguments'][0][0],
                    log_line_number=log['line_number'],
                    hexify=log['hexify'],
                    caller_func=log['caller_name'],
                    caller_line=log['caller_line_number'],
                    file_name=os.path.basename(file_path),
                )

                LOGGER.info(f'Got log tag {tag}, generated or quired idx {db_index}')

                if result == LogDBManager.SUCCESS:
                    LOGGER.info(f'Added new log: {db_index} - {tag_base}')
                elif result == LogDBManager.LOG_EXISTS:
                    if file_index:
                        if int(file_index, 16) != db_index:
                            LOGGER.info(f'Updating index: {file_index} -> {db_index}')
                        else:
                            LOGGER.info(f'duplicate index: {file_index} == {db_index}')
                            continue
                    else:
                        LOGGER.info(f'Recovery log index {db_index}')
                else:
                    LOGGER.error(f'Database error for log: {tag_base}')
                    continue

                # Update tag in content
                new_tag_bytes = f'{tag_base}_{db_index}'.encode()
                tag_start, tag_end = log['tag'][1], log['tag'][2]
                new_content[offset + tag_start : offset + tag_end] = new_tag_bytes
                offset += len(new_tag_bytes) - (tag_end - tag_start)

                # Generate macro if this is a new log or index changed
                if result == LogDBManager.SUCCESS or file_index or self.db_manager.is_config_updated(module):
                    macro = self.generate_compressed_macro(module, db_index, tag_base, simple_fmt_str, log)
                    generated_macros.append((module, db_index, macro))

            # Write updated content
            with open(file_path, 'wb') as f_out:
                f_out.write(new_content)

        except Exception as e:
            LOGGER.error(f'Error processing {file_path}: {e}\n{traceback.format_exc()}')
            raise

        return generated_macros

    def prepare_source_files(self, srcs: list[str]) -> None:
        """
        Prepare source files for processing.

        Args:
            srcs: List of source file paths
        """
        for module, info in self.module_info.items():
            code_dirs = '|'.join(info['code_path'])
            pattern = re.compile(f'({code_dirs}).*\\.c$')
            info['files_to_process'] = []
            compressed_file_cnt = 0
            total_cnt = 0
            for src in srcs:
                if pattern.match(src):
                    src_path = self.code_base_path / src
                    dest_path = self.bt_compressed_srcs_path / src
                    temp_path = f'{dest_path}.tmp'
                    total_cnt += 1
                    # Skip if already processed
                    if self.db_manager.is_file_processed(
                        module, src_path, temp_path
                    ) and not self.db_manager.is_config_updated(module):
                        compressed_file_cnt += 1
                        continue

                    # Ensure directory exists
                    os.makedirs(os.path.dirname(temp_path), exist_ok=True)
                    shutil.copy2(src_path, temp_path)
                    info['files_to_process'].append(temp_path)
                    LOGGER.info(f'Prepared: {src}')
            LOGGER.info(f'Compressed cnt {compressed_file_cnt} {total_cnt}')
            if compressed_file_cnt == total_cnt:
                print(
                    f'All source files in module {module} have been compressed\n', flush=True, end='', file=sys.stdout
                )
            else:
                print(
                    f'Found {len(info["files_to_process"])} source files in module {module} requiring compression\n',
                    flush=True,
                    end='',
                    file=sys.stdout,
                )
        LOGGER.info('Source file preparation complete')

    def generate_log_index_header(self, module: str, macros: list[tuple[int, str]]) -> None:
        """
        Generate or update the log index header file.

        Args:
            module: Module name
            macros: List of (log_id, macro_definition)
        """
        # header_path = self.code_base_path / self.module_info[module]['log_index_path']
        header_path = self.build_dir / Path('ble_log') / Path('include') / self.module_info[module]['log_index_file']
        # Create directory if needed
        header_path.parent.mkdir(parents=True, exist_ok=True)

        update_state = self.db_manager.source_update_state(source=module)
        if update_state == self.db_manager.SOURCE_LOG_UPDATE_NONE:
            return
        elif update_state == self.db_manager.SOURCE_LOG_UPDATE_FULL:
            # Header template
            header_content = textwrap.dedent(f"""
               /*
                * SPDX-FileCopyrightText: {datetime.now().year} Espressif Systems (Shanghai) CO LTD
                *
                * SPDX-License-Identifier: Apache-2.0
                */

                #ifndef __{module.upper()}_INTERNAL_LOG_INDEX_H
                #define __{module.upper()}_INTERNAL_LOG_INDEX_H

                #include <stddef.h>
                #include <stdlib.h>

                """).strip()

            header_content += self.module_mod[module].gen_header_head()
            header_content += '\n\n'

            # Add sorted macros
            for log_id, macro_def in sorted(macros, key=lambda x: x[0]):
                header_content += macro_def + '\n'

            header_content += f'#endif // __{module.upper()}_INTERNAL_LOG_INDEX_H\n'

            with open(header_path, 'w') as f:
                f.write(header_content)
        else:
            append_content = ''
            log_idx_set: dict[int, int] = dict()
            for log_id, macro_def in sorted(macros, key=lambda x: x[0]):
                append_content += macro_def + '\n'
                log_idx_set[log_id] = 1
            with open(header_path, encoding='utf-8') as f:
                lines = f.readlines()
            log_idx_pattern = re.compile(r'#define .+(\d+)\(fmt,')
            for idx, line in enumerate(lines):
                if line.strip().startswith('#define'):
                    res = log_idx_pattern.match(line)
                    if res:
                        li = int(res.group(1))
                        if li in log_idx_set:
                            raise ValueError(
                                f'The generated log index{li} andlog_index in the header file have duplicates'
                            )

                if line.strip().startswith('#endif'):
                    break
            else:
                raise RuntimeError('#endif not found')
            lines.insert(idx, append_content)
            with open(header_path, 'w', encoding='utf-8') as f:
                f.writelines(lines)
        LOGGER.info(f'Generated log index header: {header_path}')

    def load_config(self, config_path: str, module_names: list[str]) -> None:
        """
        Load and validate log configuration.

        Args:
            config_path: Path to configuration file
            module_names: List of module names to load
        """
        with open(config_path, encoding='utf-8') as f:
            config = yaml.safe_load(f)

        # Extract global config
        log_config = config.get('log_config', {})
        for key, value in log_config.items():
            if key != 'modules':
                self.config[key] = value

        # Extract module configs
        modules = log_config.get('modules', {})
        for module in module_names:
            if module in modules:
                self.module_info[module] = modules[module]
                module_script_path = self.module_info[module]['script']
                spec = self.module_mod[module] = importlib.util.spec_from_file_location(module, module_script_path)
                if spec and spec.loader:
                    self.module_mod[module] = importlib.util.module_from_spec(spec)
                    spec.loader.exec_module(self.module_mod[module])
                else:
                    LOGGER.error(f"Failed to load module '{module}' script")
                    raise ImportError(' Failed to load module script')
                print(f'Found module {module} for compression\n', flush=True, end='', file=sys.stdout)
            else:
                LOGGER.warning(f"Skipping module '{module}' - config not found")

    def main(self) -> int:
        """Main entry point for the compression utility."""
        parser = argparse.ArgumentParser(description='BLE Log Compression Utility')
        subparsers = parser.add_subparsers(dest='command', required=True)

        compress_parser = subparsers.add_parser('compress')
        compress_parser.add_argument('--srcs', required=True, help='Semicolon-separated source file paths')
        compress_parser.add_argument('--code_base_path', required=True, help='Component base path')
        compress_parser.add_argument('--module', required=True, help='Semicolon-separated module names')
        compress_parser.add_argument('--build_path', required=True, help='Build output directory')
        compress_parser.add_argument('--compressed_srcs_path', required=True, help='Directory for processed sources')

        args = parser.parse_args()

        # Setup paths
        self.code_base_path = Path(args.code_base_path)
        self.build_dir = Path(args.build_path)
        self.bt_compressed_srcs_path = Path(args.compressed_srcs_path)

        # Create directories
        (self.build_dir / 'ble_log').mkdir(parents=True, exist_ok=True)
        self.bt_compressed_srcs_path.mkdir(parents=True, exist_ok=True)

        # Configure logging
        log_file = self.build_dir / 'ble_log' / f'ble_script_log_{datetime.now().strftime("%y%m%d_%H%M%S")}.log'
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            handlers=[logging.FileHandler(log_file, mode='w')],
        )

        # Load configuration
        modules = args.module.split(';')
        config_path = self.build_dir / 'ble_log/module_info.yml'
        self.load_config(str(config_path), modules)

        # Initialize database
        db_path = self.build_dir / self.config.get('db_path', 'log_db')
        db_manager = LogDBManager(
            data_dir=str(db_path),
            sources={source: str(config) for source, config in self.module_info.items()},
            logger=LOGGER,
        )

        self.db_manager = db_manager

        # Prepare source files
        src_list = args.srcs.split(';')
        self.prepare_source_files(src_list)

        # Collect files to process
        files_to_process = []
        for module, info in self.module_info.items():
            files_to_process.extend([(module, path) for path in info['files_to_process']])

        if not files_to_process:
            LOGGER.info('No files to process')
            print('No source files require compression; exiting log compression\n', flush=True, end='', file=sys.stdout)
            for root, _, files in os.walk(self.bt_compressed_srcs_path):
                for name in files:
                    if name.endswith('.tmp'):
                        file_src = os.path.join(root, name)
                        dst_path = os.path.join(root, name[: -len('.tmp')])
                        shutil.copy2(file_src, dst_path)
                        LOGGER.info(f'Recovery src {file_src} dst {dst_path}')
            db_manager.close()
            return 0

        all_macros: dict[str, list[tuple[int, str]]] = {}
        files_to_process.sort(key=lambda x: x[1])
        try:
            compressed_log_count = 0
            for needs_compressed_file in files_to_process:
                file_macros = self.compress_file(needs_compressed_file)
                compressed_log_count += len(file_macros)
                for module, log_id, macro in file_macros:
                    all_macros.setdefault(module, []).append((log_id, macro))
            print(f'{compressed_log_count} ble log(s) compressed\n', flush=True, end='', file=sys.stdout)
        except Exception as e:
            LOGGER.error(f'Processing failed: {e}')
            db_manager.close()
            return 1

        # Generate headers
        for module, macros in all_macros.items():
            self.generate_log_index_header(module, macros)
        print('Header file for compressed logs generated\n', flush=True, end='', file=sys.stdout)

        # Mark files as processed
        for module, info in self.module_info.items():
            for temp_path in info['files_to_process']:
                src_path = self.code_base_path / os.path.relpath(temp_path[:-4], self.bt_compressed_srcs_path)
                db_manager.mark_file_processed(module, src_path, temp_path)
        for root, _, files in os.walk(self.bt_compressed_srcs_path):
            for name in files:
                if name.endswith('.tmp'):
                    file_src = os.path.join(root, name)
                    dst_path = os.path.join(root, name[: -len('.tmp')])
                    shutil.copy2(file_src, dst_path)
                    LOGGER.info(f'Recovery src {file_src} dst {dst_path}')
        db_manager.close()
        LOGGER.info('Compression completed successfully')
        return 0


if __name__ == '__main__':
    print('Log compression underway, please wait...\n', flush=True, end='', file=sys.stdout)
    compressor = LogCompressor()
    exit(compressor.main())
