# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import textwrap


def generate_mesh_log_prefix(tag: str, print_statm: str) -> str:
    level = tag.split('_')[-1]
    mod = tag.split('_')[0]
    if level == 'ERR':
        level = 'ERROR'
        log_level = 'BLE_MESH_LOG_LEVEL_ERROR'
    elif level == 'WARN':
        level = 'WARN'
        log_level = 'BLE_MESH_LOG_LEVEL_WARN'
    elif level == 'INFO':
        level = 'INFO'
        log_level = 'BLE_MESH_LOG_LEVEL_INFO'
    elif level == 'DBG':
        level = 'DEBUG'
        log_level = 'BLE_MESH_LOG_LEVEL_DEBUG'
    else:
        return ''
    if mod == 'NET':
        used_log_levl = 'BLE_MESH_NET_BUF_LOG_LEVEL'
        used_log_mod = 'BLE_MESH_NET_BUF'
    else:
        used_log_levl = 'BLE_MESH_LOG_LEVEL'
        used_log_mod = 'BLE_MESH'
    return (
        f'{{do {{ if (({used_log_levl} >= {log_level}) &&'
        f' BLE_MESH_LOG_LEVEL_CHECK({used_log_mod}, {level})) {print_statm};}} while (0);}}\\\n'
    )


def gen_header_head() -> str:
    head = textwrap.dedent("""
                // Compression function declarations
                extern int ble_log_compressed_hex_print
                        (uint8_t source, uint32_t log_index, size_t args_size_cnt, ...);
                extern int ble_log_compressed_hex_print_buf
                        (uint8_t source, uint32_t log_index, uint8_t buf_idx, const uint8_t *buf, size_t len);
    """)
    return head


def gen_compressed_stmt(
    log_index: int, module_id: int, func_name: str, fmt: str, args: list[dict], buffer_args: list[dict]
) -> str:
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
        return '    ' + generate_mesh_log_prefix(func_name, stmt)
    size_str = ', '.join([arg['size_type'] for arg in args])
    args_str = ', '.join([arg['name'] for arg in args]).replace('\n', '')
    stmt = f'    ble_log_compressed_hex_print({module_id}, {log_index}, {len(args)}, {size_str}, {args_str});'
    for idx, buffer_arg in enumerate(buffer_args):
        stmt += '\\\n'
        stmt += (
            f'    ble_log_compressed_hex_print_buf('
            f'{module_id}, {log_index}, {idx}, '
            f'(const uint8_t *){buffer_arg["buffer"]}, {buffer_arg["length"]});'
        )
    stmt += '\\\n'
    return '    ' + generate_mesh_log_prefix(func_name, stmt)
