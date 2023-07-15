#!/usr/bin/env python
# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

from __future__ import print_function

import argparse
import struct
import sys

import elftools.elf.elffile as elffile
import espytrace.apptrace as apptrace


class ESPLogTraceParserError(RuntimeError):
    def __init__(self, message):
        RuntimeError.__init__(self, message)


class ESPLogTraceRecord(object):
    def __init__(self, fmt_addr, log_args):
        super(ESPLogTraceRecord, self).__init__()
        self.fmt_addr = fmt_addr
        self.args = log_args

    def __repr__(self):
        return 'fmt_addr = 0x%x, args = %d/%s' % (self.fmt_addr, len(self.args), self.args)


def logtrace_parse(fname):
    ESP32_LOGTRACE_HDR_FMT = '<BL'
    ESP32_LOGTRACE_HDR_SZ = struct.calcsize(ESP32_LOGTRACE_HDR_FMT)

    recs = []
    try:
        ftrc = open(fname, 'rb')
    except OSError as e:
        raise ESPLogTraceParserError('Failed to open trace file (%s)!' % e)
    # data_ok = True
    while True:
        # read args num and format str addr
        try:
            trc_buf = ftrc.read(ESP32_LOGTRACE_HDR_SZ)
        except IOError as e:
            raise ESPLogTraceParserError('Failed to read log record header (%s)!' % e)
        if len(trc_buf) < ESP32_LOGTRACE_HDR_SZ:
            # print "EOF"
            if len(trc_buf) > 0:
                print('Unprocessed %d bytes of log record header!' % len(trc_buf))
                # data_ok = False
            break
        try:
            nargs,fmt_addr = struct.unpack(ESP32_LOGTRACE_HDR_FMT, trc_buf)
        except struct.error as e:
            raise ESPLogTraceParserError('Failed to unpack log record header (%s)!' % e)
        # read args
        args_sz = struct.calcsize('<%sL' % nargs)
        try:
            trc_buf = ftrc.read(args_sz)
        except IOError as e:
            raise ESPLogTraceParserError('Failed to read log record args (%s)!' % e)
        if len(trc_buf) < args_sz:
            # print("EOF")
            if len(trc_buf) > 0:
                print('Unprocessed %d bytes of log record args!' % len(trc_buf))
                # data_ok = False
            break
        try:
            log_args = struct.unpack('<%sL' % nargs, trc_buf)
        except struct.error as e:
            raise ESPLogTraceParserError('Failed to unpack log record args (%s)!' % e)
        # print(log_args)
        recs.append(ESPLogTraceRecord(fmt_addr, list(log_args)))

    ftrc.close()
    # sorted(recs, key=lambda rec: rec.fmt_addr)
    return recs


def logtrace_formated_print(recs, elfname, no_err):
    try:
        felf = elffile.ELFFile(open(elfname, 'rb'))
    except OSError as e:
        raise ESPLogTraceParserError('Failed to open ELF file (%s)!' % e)

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
        # print("\nFmt = {%s}, args = %d/%s" % lrec)
        fmt_str = fmt_str.replace('%p', '%x')
        # print("=====> " + fmt_str % lrec.args)
        try:
            print(fmt_str % tuple(lrec.args), end='')
            # print(".", end='')
            pass
        except Exception as e:
            if not no_err:
                print('Print error (%s)' % e)
                print('\nFmt = {%s}, args = %d/%s' % (fmt_str, len(lrec.args), lrec.args))
    felf.stream.close()


def main():

    parser = argparse.ArgumentParser(description='ESP32 Log Trace Parsing Tool')

    parser.add_argument('trace_file', help='Path to log trace file', type=str)
    parser.add_argument('elf_file', help='Path to program ELF file', type=str)
    # parser.add_argument('--print-details', '-d', help='Print detailed stats', action='store_true')
    parser.add_argument('--no-errors', '-n', help='Do not print errors', action='store_true')
    args = parser.parse_args()

    # parse trace file
    try:
        print("Parse trace file '%s'..." % args.trace_file)
        lrecs = logtrace_parse(args.trace_file)
        print('Parsing completed.')
    except ESPLogTraceParserError as e:
        print('Failed to parse log trace (%s)!' % e)
        sys.exit(2)
    # print recs
    # get format strings and print info
    print('====================================================================')
    try:
        logtrace_formated_print(lrecs, args.elf_file, args.no_errors)
    except ESPLogTraceParserError as e:
        print('Failed to print log trace (%s)!' % e)
        sys.exit(2)
    print('\n====================================================================\n')

    print('Log records count: %d' % len(lrecs))


if __name__ == '__main__':
    main()
