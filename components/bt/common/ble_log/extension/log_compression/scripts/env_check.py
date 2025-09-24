# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
try:
    import tree_sitter_c
except ModuleNotFoundError:
    tree_sitter_c = None
try:
    import tree_sitter
except ModuleNotFoundError:
    tree_sitter = None
import platform
import sys
from importlib.metadata import version
from pathlib import Path

from ble_log_compress import TsInit
from ble_log_compress import TsQueryByTree

IDF_PATH = Path(Path(__file__).resolve().parent / Path('../../../../../../')).resolve()

TEST_C_STR = b"""
void test_func(void);
int main(void) {
    printf("Hello world\n");
    return 0;
}
"""

TEST_FUNCTION_QUERY = """
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
TEST_LOG_QUERY = """
(expression_statement
    (call_expression
        function: (identifier) @fname
        arguments: (argument_list) @args
        (#match? @fname "^(printf)$")
    ) @log_stmt
)
"""


def check_py_version(min_version: str = '3.8.0') -> None:
    cur_str = platform.python_version()
    cur_tuple = tuple(map(int, cur_str.split('.')))
    min_tuple = tuple(map(int, min_version.split('.')))

    if min_tuple > cur_tuple:
        print(
            ('Please use Python 3.8 or above'),
            file=sys.stderr,
        )
        exit(1)


def validate() -> None:
    if tree_sitter is None:
        print(
            (
                'tree_sitter import failed, please check whether the package is installed correctly,'
                'Please refer to the file:'
                f'{IDF_PATH}/components/bt/common/ble_log/log_compression/scripts/install.en.md'
                ' for installation instructions.'
            ),
            file=sys.stderr,
        )
        exit(1)
    if tree_sitter_c is None:
        print(
            (
                'tree_sitter_c import failed, '
                'please check whether the package is installed correctly,'
                'Please refer to the file:'
                f'{IDF_PATH}/components/bt/common/ble_log/log_compression/scripts/install.en.md'
                ' for installation instructions.'
            ),
            file=sys.stderr,
        )
        exit(1)


def test_parse() -> None:
    try:
        lang, parser = TsInit()
        tree = parser.parse(TEST_C_STR)
        captures = TsQueryByTree(lang, tree, TEST_LOG_QUERY)

        assert len(captures.keys()) == 3
        assert len(captures.values()) == 3
        assert 'log_stmt' in captures.keys()
        assert 'fname' in captures.keys()
        assert 'args' in captures.keys()
        assert captures['log_stmt'][0].type == 'call_expression'
        assert captures['log_stmt'][0].start_point == (3, 4)
        assert captures['log_stmt'][0].end_point == (4, 2)

        captures = TsQueryByTree(lang, tree, TEST_FUNCTION_QUERY)
        print(captures, file=sys.stderr)
        assert len(captures.keys()) == 2
        assert len(captures.values()) == 2
        assert 'func_decls' in captures.keys()
        assert 'func_name' in captures.keys()
        assert len(captures['func_name']) == 2
        assert len(captures['func_decls']) == 1
        assert captures['func_name'][0].type == 'identifier'
        assert captures['func_name'][0].start_point == (1, 5)
        assert captures['func_name'][0].end_point == (1, 14)

    except Exception:
        print(
            (
                f'Code parsing error,'
                f'please provide the following information to Espressif to help you solve the problem'
                f'tree-sitter version: {version(tree_sitter)} '
                f'tree-sitter-c version: {version(tree_sitter_c)} '
                f'python version: {platform.python_version()} ',
                f'os_system:      {platform.system()} ',
                f'os_release: {platform.release()} ',
                f'os_version: {platform.version()} ',
                f'machine: {platform.machine()}',
            ),
            file=sys.stderr,
        )
        exit(1)


check_py_version()
validate()
test_parse()
exit(0)
