# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Macro generator shared by the BLE_ISO and BLE_AUDIO compression modules.

Both components consume the LOG_ERR / LOG_WRN / LOG_INF / LOG_DBG macros
defined in components/bt/esp_ble_iso/include/zephyr/logging/log.h and gate
output through CONFIG_BT_ISO_LOG_LEVEL with the BT_ISO_LOG_* numeric levels.
"""

import textwrap


def generate_iso_log_prefix(tag: str, print_statm: str) -> str:
    """Wrap a compressed-print statement with the matching level check."""
    level = tag.split('_')[-1]  # LOG_ERR -> ERR

    if level == 'ERR':
        log_level = 'BT_ISO_LOG_ERROR'
    elif level == 'WRN':
        log_level = 'BT_ISO_LOG_WARN'
    elif level == 'INF':
        log_level = 'BT_ISO_LOG_INFO'
    elif level == 'DBG':
        log_level = 'BT_ISO_LOG_DEBUG'
    else:
        return ''

    used_log_levl = 'CONFIG_BT_ISO_LOG_LEVEL'

    return f'{{do {{ if (({used_log_levl} >= {log_level})) {{ {print_statm}; }} }} while (0);}}\\\n'


def gen_header_head() -> str:
    head = textwrap.dedent("""
        // Compression function declarations
        extern int ble_log_compressed_hex_print
                (uint8_t source, uint32_t log_index, size_t args_size_cnt, ...);
        extern int ble_log_compressed_hex_print_buf
                (uint8_t source, uint32_t log_index, uint8_t buf_idx, const uint8_t *buf, size_t len);
    """)
    return head


def gen_compressed_stmt(log_index: int, module_id: int, func_name: str, fmt: str, args: list, buffer_args: list) -> str:
    if len(args) == 0:
        stmt = f'    ble_log_compressed_hex_print({module_id}, {log_index}, 0);'
        for idx, buffer_arg in enumerate(buffer_args):
            stmt += '\\\n'
            stmt += (
                f'    ble_log_compressed_hex_print_buf('
                f'{module_id}, {log_index}, {idx}, '
                f'(const uint8_t *){buffer_arg["buffer"]}, {buffer_arg["length"]});'
            )
        stmt += '\\\n'
        return '    ' + generate_iso_log_prefix(func_name, stmt)

    size_str = ', '.join([arg['size_type'] for arg in args])
    # Strip both backslash-newline continuations and bare newlines from arg
    # names so they don't leak into the generated C macro body — the same
    # cleanup the NimBLE / Bluedroid generators do.
    args_str = ', '.join([arg['name'] for arg in args]).replace('\\\n', '').replace('\n', '')
    stmt = f'    ble_log_compressed_hex_print({module_id}, {log_index}, {len(args)}, {size_str}, {args_str});'

    for idx, buffer_arg in enumerate(buffer_args):
        stmt += '\\\n'
        stmt += (
            f'    ble_log_compressed_hex_print_buf('
            f'{module_id}, {log_index}, {idx}, '
            f'(const uint8_t *){buffer_arg["buffer"]}, {buffer_arg["length"]});'
        )

    stmt += '\\\n'
    return '    ' + generate_iso_log_prefix(func_name, stmt)
