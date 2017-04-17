#!/usr/bin/env python
#

import argparse
import struct
import sys
import pylibelf as elf
import pylibelf.util as elfutil
import pylibelf.iterators as elfiter
import pylibelf.constants as elfconst
from ctypes import *

class ESPLogTraceParserError(RuntimeError):
    def __init__(self, message):
        RuntimeError.__init__(self, message)


class ESPLogTraceRecord(object):
    def __init__(self, fmt_addr, log_args):
        super(ESPLogTraceRecord, self).__init__()
        self.fmt_addr = fmt_addr
        self.args = log_args

    def __repr__(self):
        return "fmt_addr = 0x%x, args = %d/%s" % (self.fmt_addr, len(self.args), self.args)


def logtrace_parse(fname):
    ESP32_LOGTRACE_HDR_FMT = '<BL'
    ESP32_LOGTRACE_HDR_SZ = struct.calcsize(ESP32_LOGTRACE_HDR_FMT)

    recs = []
    try:
        ftrc = open(fname, 'rb')
    except OSError as e:
        raise ESPLogTraceParserError("Failed to open trace file (%s)!" % e)
    # data_ok = True
    while True:
        # read args num and format str addr
        try:
            trc_buf = ftrc.read(ESP32_LOGTRACE_HDR_SZ)
        except IOError as e:
            raise ESPLogTraceParserError("Failed to read log record header (%s)!" % e)
        if len(trc_buf) < ESP32_LOGTRACE_HDR_SZ:
            # print "EOF"
            if len(trc_buf) > 0:
                print "Unprocessed %d bytes of log record header!" % len(trc_buf)
                # data_ok = False
            break
        try:
            nargs,fmt_addr = struct.unpack(ESP32_LOGTRACE_HDR_FMT, trc_buf)
        except struct.error as e:
            raise ESPLogTraceParserError("Failed to unpack log record header (%s)!" % e)
        # read args
        args_sz = struct.calcsize('<%sL' % nargs)
        try:
            trc_buf = ftrc.read(args_sz)
        except IOError as e:
            raise ESPLogTraceParserError("Failed to read log record args (%s)!" % e)
        if len(trc_buf) < args_sz:
            # print "EOF"
            if len(trc_buf) > 0:
                print "Unprocessed %d bytes of log record args!" % len(trc_buf)
                # data_ok = False
            break
        try:
            log_args = struct.unpack('<%sL' % nargs, trc_buf)
        except struct.error as e:
            raise ESPLogTraceParserError("Failed to unpack log record args (%s)!" % e)
        # print log_args
        recs.append(ESPLogTraceRecord(fmt_addr, list(log_args)))    

    ftrc.close()
    # sorted(recs, key=lambda rec: rec.fmt_addr)
    return recs


def logtrace_get_str_from_elf(felf, str_addr):
    tgt_str = ""
    for sect in elfiter.sections(felf):
        hdr = elfutil.section_hdr(felf, sect)
        if hdr.sh_addr == 0 or hdr.sh_type != elfconst.SHT_PROGBITS:
            continue
        if str_addr < hdr.sh_addr or str_addr >= hdr.sh_addr + hdr.sh_size:
            continue
        # print "Found SECT: %x..%x @ %x" % (hdr.sh_addr, hdr.sh_addr + hdr.sh_size, str_addr - hdr.sh_addr)
        sec_data = elfiter.getOnlyData(sect).contents
        buf = cast(sec_data.d_buf, POINTER(c_char))
        for i in range(str_addr - hdr.sh_addr, hdr.sh_size):
            if buf[i] == "\0":
                break
            tgt_str += buf[i]
        if len(tgt_str) > 0:
            return tgt_str
    return None

def logtrace_formated_print(recs, elfname, no_err):
    try:
        felf = elfutil.open_elf(elfname)
    except OSError as e:
        raise ESPLogTraceParserError("Failed to open ELF file (%s)!" % e)

    for lrec in recs:
        fmt_str = logtrace_get_str_from_elf(felf, lrec.fmt_addr)
        i = 0
        prcnt_idx = 0
        while i < len(lrec.args):
            prcnt_idx = fmt_str.find('%', prcnt_idx, -2) # TODO: check str ending with %
            if prcnt_idx == -1:
                break
            prcnt_idx += 1 #  goto next char
            if fmt_str[prcnt_idx] == 's':
                # find string
                arg_str = logtrace_get_str_from_elf(felf, lrec.args[i])
                if arg_str:
                    lrec.args[i] = arg_str
            i += 1
        # print "\nFmt = {%s}, args = %d/%s" % lrec
        fmt_str = fmt_str.replace('%p', '%x')
        # print "=====> " + fmt_str % lrec.args
        try:
            print fmt_str % tuple(lrec.args),
            # print ".",
            pass
        except Exception as e:
            if not no_err:
                print "Print error (%s)" % e
                print "\nFmt = {%s}, args = %d/%s" % (fmt_str, len(lrec.args), lrec.args)

    elf.elf_end(felf)

def main():

    parser = argparse.ArgumentParser(description='ESP32 Log Trace Parsing Tool')

    parser.add_argument('trace_file', help='Path to log trace file', type=str)
    parser.add_argument('elf_file', help='Path to program ELF file', type=str)
    # parser.add_argument('--print-details', '-d', help='Print detailed stats', action='store_true')
    parser.add_argument('--no-errors', '-n', help='Do not print errors', action='store_true')
    args = parser.parse_args()

    # parse trace file
    try:
        print "Parse trace file '%s'..." % args.trace_file
        lrecs = logtrace_parse(args.trace_file);
        print "Parsing completed."
    except ESPLogTraceParserError as e:
        print "Failed to parse log trace (%s)!" % e
        sys.exit(2)
    # print recs
    # get format strings and print info
    print "===================================================================="
    try:
        logtrace_formated_print(lrecs, args.elf_file, args.no_errors);
    except ESPLogTraceParserError as e:
        print "Failed to print log trace (%s)!" % e
        sys.exit(2)
    print "\n====================================================================\n"

    print "Log records count: %d" % len(lrecs)

if __name__ == '__main__':
    main()
