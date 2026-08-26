# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import textwrap


def generate_nimble_log_prefix(print_statm: str) -> str:
    return f'{{if (MYNEWT_VAL(BLE_HS_LOG_LVL) <= LOG_LEVEL_ ##fmt) {print_statm};}}\\\n'


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
    log_index: int,
    module_id: int,
    func_name: str,
    fmt: str,
    args: list[dict],
    buffer_args: list[dict],
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
        return '    ' + generate_nimble_log_prefix(stmt)
    size_str = ', '.join([arg['size_type'] for arg in args])
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
    return '    ' + generate_nimble_log_prefix(stmt)
