#!/usr/bin/env python
# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import struct

import elftools.elf.elffile as elffile
import espytrace.apptrace as apptrace
import rich_click as click
from esp_pylib.excepthook import install_exception_reporting
from esp_pylib.logger import log
from rich.markup import escape


class ESPLogTraceParserError(RuntimeError):
    def __init__(self, message):
        RuntimeError.__init__(self, message)


class ESPLogTraceRecord:
    def __init__(self, fmt_addr, log_args):
        super().__init__()
        self.fmt_addr = fmt_addr
        self.args = log_args

    def __repr__(self):
        return f'fmt_addr = 0x{self.fmt_addr:x}, args = {len(self.args)}/{self.args}'


def logtrace_parse(fname):
    ESP32_LOGTRACE_HDR_FMT = '<BL'
    ESP32_LOGTRACE_HDR_SZ = struct.calcsize(ESP32_LOGTRACE_HDR_FMT)

    recs = []
    try:
        ftrc = open(fname, 'rb')
    except OSError as e:
        raise ESPLogTraceParserError(f'Failed to open trace file ({e})!')
    # data_ok = True
    while True:
        # read args num and format str addr
        try:
            trc_buf = ftrc.read(ESP32_LOGTRACE_HDR_SZ)
        except OSError as e:
            raise ESPLogTraceParserError(f'Failed to read log record header ({e})!')
        if len(trc_buf) < ESP32_LOGTRACE_HDR_SZ:
            if len(trc_buf) > 0:
                log.warn(f'Unprocessed {len(trc_buf)} bytes of log record header!')
            break
        try:
            nargs, fmt_addr = struct.unpack(ESP32_LOGTRACE_HDR_FMT, trc_buf)
        except struct.error as e:
            raise ESPLogTraceParserError(f'Failed to unpack log record header ({e})!')
        # read args
        args_sz = struct.calcsize(f'<{nargs}L')
        try:
            trc_buf = ftrc.read(args_sz)
        except OSError as e:
            raise ESPLogTraceParserError(f'Failed to read log record args ({e})!')
        if len(trc_buf) < args_sz:
            if len(trc_buf) > 0:
                log.warn(f'Unprocessed {len(trc_buf)} bytes of log record args!')
            break
        try:
            log_args = struct.unpack(f'<{nargs}L', trc_buf)
        except struct.error as e:
            raise ESPLogTraceParserError(f'Failed to unpack log record args ({e})!')
        recs.append(ESPLogTraceRecord(fmt_addr, list(log_args)))

    ftrc.close()
    return recs


def logtrace_formated_print(recs, elfname, no_err):
    try:
        felf = elffile.ELFFile(open(elfname, 'rb'))
    except OSError as e:
        raise ESPLogTraceParserError(f'Failed to open ELF file ({e})!')

    for lrec in recs:
        fmt_str = apptrace.get_str_from_elf(felf, lrec.fmt_addr)
        i = 0
        prcnt_idx = 0
        while i < len(lrec.args):
            prcnt_idx = fmt_str.find('%', prcnt_idx, -2)  # TODO: check str ending with %
            if prcnt_idx == -1:
                break
            prcnt_idx += 1  # goto next char
            if fmt_str[prcnt_idx] == 's':
                # find string
                arg_str = apptrace.get_str_from_elf(felf, lrec.args[i])
                if arg_str:
                    lrec.args[i] = arg_str
            i += 1
        fmt_str = fmt_str.replace('%p', '%x')
        try:
            # Decoded target logs are payload: use plain print (faster; preserves tabs).
            print(fmt_str % tuple(lrec.args), end='')
        except Exception as e:
            if not no_err:
                log.err(escape(f'Print error ({e})'))
                print(f'\nFmt = {{{fmt_str}}}, args = {len(lrec.args)}/{lrec.args}')
    felf.stream.close()


@click.command(
    context_settings={'help_option_names': ['-h', '--help']},
    help=('ESP32 Log Trace Parsing Tool\n\nTRACE_FILE: Path to log trace file\n\nELF_FILE: Path to program ELF file'),
)
@click.argument('trace_file', type=click.Path(dir_okay=False))
@click.argument('elf_file', type=click.Path(dir_okay=False))
@click.option('--no-errors', '-n', is_flag=True, help='Do not print errors')
def main(trace_file, elf_file, no_errors):
    try:
        log.note(escape(f"Parse trace file '{trace_file}'..."))
        lrecs = logtrace_parse(trace_file)
        log.note('Parsing completed.')
    except ESPLogTraceParserError as e:
        log.die(escape(f'Failed to parse log trace ({e})!'), exit_code=2)
    log.print('====================================================================')
    try:
        logtrace_formated_print(lrecs, elf_file, no_errors)
    except ESPLogTraceParserError as e:
        log.die(escape(f'Failed to print log trace ({e})!'), exit_code=2)
    log.print('\n====================================================================\n')

    log.print(f'Log records count: {len(lrecs)}')


if __name__ == '__main__':
    install_exception_reporting()
    main()
