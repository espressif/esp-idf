# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from __future__ import print_function

import os
import sys

try:
    from urlparse import urlparse
except ImportError:
    from urllib.parse import urlparse

import os.path
import socketserver as SocketServer
import subprocess
import tempfile
import threading
import time

import elftools.elf.constants as elfconst
import elftools.elf.elffile as elffile


def clock():
    if sys.version_info >= (3, 3):
        return time.process_time()
    else:
        return time.clock()


def addr2line(toolchain, elf_path, addr):
    """
        Creates trace reader.

        Parameters
        ----------
        toolchain : string
            toolchain prefix to retrieve source line locations using addresses
        elf_path : string
            path to ELF file to use
        addr : int
            address to retrieve source line location

        Returns
        -------
        string
            source line location string
    """
    try:
        return subprocess.check_output(['%saddr2line' % toolchain, '-e', elf_path, '0x%x' % addr]).decode('utf-8')
    except subprocess.CalledProcessError:
        return ''


class ParseError(RuntimeError):
    """
        Parse error exception
    """
    def __init__(self, message):
        RuntimeError.__init__(self, message)


class ReaderError(RuntimeError):
    """
        Trace reader error exception
    """
    def __init__(self, message):
        RuntimeError.__init__(self, message)


class ReaderTimeoutError(ReaderError):
    """
        Trace reader timeout error
    """
    def __init__(self, tmo, sz):
        ReaderError.__init__(self, 'Timeout %f sec while reading %d bytes!' % (tmo, sz))


class ReaderShutdownRequest(ReaderError):
    """
        Trace reader shutdown request error
        Raised when user presses CTRL+C (SIGINT).
    """
    def __init__(self):
        ReaderError.__init__(self, 'Shutdown request!')


class Reader:
    """
        Base abstract reader class
    """
    def __init__(self, tmo):
        """
            Constructor

            Parameters
            ----------
            tmo : int
                read timeout
        """
        self.timeout = tmo
        self.need_stop = False

    def read(self, sz):
        """
            Reads a number of bytes

            Parameters
            ----------
            sz : int
                number of bytes to read

            Returns
            -------
            bytes object
                read bytes

            Returns
            -------
            ReaderTimeoutError
                if timeout expires
            ReaderShutdownRequest
                if SIGINT was received during reading
        """
        pass

    def readline(self):
        """
            Reads line

            Parameters
            ----------
            sz : int
                number of bytes to read

            Returns
            -------
            string
                read line
        """
        pass

    def forward(self, sz):
        """
            Moves read pointer to a number of bytes

            Parameters
            ----------
            sz : int
                number of bytes to read
        """
        pass

    def cleanup(self):
        """
            Cleans up reader
        """
        self.need_stop = True


class FileReader(Reader):
    """
        File reader class
    """
    def __init__(self, path, tmo):
        """
            Constructor

            Parameters
            ----------
            path : string
                path to file to read
            tmo : int
                see Reader.__init__()
        """
        Reader.__init__(self, tmo)
        self.trace_file_path = path
        self.trace_file = open(path, 'rb')

    def read(self, sz):
        """
            see Reader.read()
        """
        data = b''
        start_tm = clock()
        while not self.need_stop:
            data += self.trace_file.read(sz - len(data))
            if len(data) == sz:
                break
            if self.timeout != -1 and clock() >= start_tm + self.timeout:
                raise ReaderTimeoutError(self.timeout, sz)
        if self.need_stop:
            raise ReaderShutdownRequest()
        return data

    def get_pos(self):
        """
            Retrieves current file read position

            Returns
            -------
            int
                read position
        """
        return self.trace_file.tell()

    def readline(self, linesep=os.linesep):
        """
            see Reader.read()
        """
        line = ''
        start_tm = clock()
        while not self.need_stop:
            line += self.trace_file.readline().decode('utf-8')
            if line.endswith(linesep):
                break
            if self.timeout != -1 and clock() >= start_tm + self.timeout:
                raise ReaderTimeoutError(self.timeout, 1)
        if self.need_stop:
            raise ReaderShutdownRequest()
        return line

    def forward(self, sz):
        """
            see Reader.read()
        """
        cur_pos = self.trace_file.tell()
        start_tm = clock()
        while not self.need_stop:
            file_sz = os.path.getsize(self.trace_file_path)
            if file_sz - cur_pos >= sz:
                break
            if self.timeout != -1 and clock() >= start_tm + self.timeout:
                raise ReaderTimeoutError(self.timeout, sz)
        if self.need_stop:
            raise ReaderShutdownRequest()
        self.trace_file.seek(sz, os.SEEK_CUR)


class NetRequestHandler:
    """
        Handler for incoming network requests (connections, datagrams)
    """
    def handle(self):
        while not self.server.need_stop:
            data = self.rfile.read(1024)
            if len(data) == 0:
                break
            self.server.wtrace.write(data)
            self.server.wtrace.flush()


class NetReader(FileReader):
    """
        Base netwoek socket reader class
    """
    def __init__(self, tmo):
        """
            see Reader.__init__()
        """
        fhnd,fname = tempfile.mkstemp()
        FileReader.__init__(self, fname, tmo)
        self.wtrace = os.fdopen(fhnd, 'wb')
        self.server_thread = threading.Thread(target=self.serve_forever)
        self.server_thread.start()

    def cleanup(self):
        """
            see Reader.cleanup()
        """
        FileReader.cleanup(self)
        self.shutdown()
        self.server_close()
        self.server_thread.join()
        time.sleep(0.1)
        self.trace_file.close()
        self.wtrace.close()


class TCPRequestHandler(NetRequestHandler, SocketServer.StreamRequestHandler):
    """
        Handler for incoming TCP connections
    """
    pass


class TCPReader(NetReader, SocketServer.TCPServer):
    """
        TCP socket reader class
    """
    def __init__(self, host, port, tmo, handler=TCPRequestHandler):
        """
            Constructor

            Parameters
            ----------
            host : string
                see SocketServer.BaseServer.__init__()
            port : int
                see SocketServer.BaseServer.__init__()
            tmo : int
                see Reader.__init__()
        """
        SocketServer.TCPServer.__init__(self, (host, port), handler)
        NetReader.__init__(self, tmo)


class UDPRequestHandler(NetRequestHandler, SocketServer.DatagramRequestHandler):
    """
        Handler for incoming UDP datagrams
    """
    pass


class UDPReader(NetReader, SocketServer.UDPServer):
    """
        UDP socket reader class
    """
    def __init__(self, host, port, tmo, handler=UDPRequestHandler):
        """
            Constructor

            Parameters
            ----------
            host : string
                see SocketServer.BaseServer.__init__()
            port : int
                see SocketServer.BaseServer.__init__()
            tmo : int
                see Reader.__init__()
        """
        SocketServer.UDPServer.__init__(self, (host, port), handler)
        NetReader.__init__(self, tmo)


def reader_create(trc_src, tmo, handler=None):
    """
        Creates trace reader.

        Parameters
        ----------
        trc_src : string
            trace source URL. Supports 'file:///path/to/file' or (tcp|udp)://host:port
        tmo : int
            read timeout

        Returns
        -------
        Reader
            reader object or None if URL scheme is not supported
    """
    url = urlparse(trc_src)
    if len(url.scheme) == 0 or url.scheme == 'file':
        if os.name == 'nt':
            # workaround for Windows path
            return FileReader(trc_src[7:], tmo)
        else:
            return FileReader(url.path, tmo)
    if url.scheme == 'tcp':
        if handler is not None:
            return TCPReader(url.hostname, url.port, tmo, handler)
        return TCPReader(url.hostname, url.port, tmo)
    if url.scheme == 'udp':
        if handler is not None:
            return UDPReader(url.hostname, url.port, tmo, handler)
        return UDPReader(url.hostname, url.port, tmo)
    return None


class TraceEvent:
    """
        Base class for all trace events.
    """
    def __init__(self, name, core_id, evt_id):
        self.name = name
        self.ctx_name = 'None'
        self.in_irq = False
        self.core_id = core_id
        self.id = evt_id
        self.ts = 0
        self.params = {}

    @property
    def ctx_desc(self):
        if self.in_irq:
            return 'IRQ "%s"' % self.ctx_name
        return 'task "%s"' % self.ctx_name

    def to_jsonable(self):
        res = self.__dict__
        params = {}
        for p in self.params:
            params.update(self.params[p].to_jsonable())
        res['params'] = params
        return res


class TraceDataProcessor:
    """
        Base abstract class for all trace data processors.
    """
    def __init__(self, print_events, keep_all_events=False):
        """
            Constructor.

            Parameters
            ----------
            print_events : bool
                if True every event will be printed as they arrive
            keep_all_events : bool
                if True all events will be kept in self.events in the order they arrive
        """
        self.print_events = print_events
        self.keep_all_events = keep_all_events
        self.total_events = 0
        self.events = []
        # This can be changed by the root procesor that includes several sub-processors.
        # It is used access some method of root processor which can contain methods/data common for all sub-processors.
        # Common info could be current execution context, info about running tasks, available IRQs etc.
        self.root_proc = self

    def _print_event(self, event):
        """
            Base method to print an event.

            Parameters
            ----------
            event : object
                Event object
        """
        print('EVENT[{:d}]: {}'.format(self.total_events, event))

    def print_report(self):
        """
            Base method to print report.
        """
        print('Processed {:d} events'.format(self.total_events))

    def cleanup(self):
        """
            Base method to make cleanups.
        """
        pass

    def on_new_event(self, event):
        """
            Base method to process event.
        """
        if self.print_events:
            self._print_event(event)
        if self.keep_all_events:
            self.events.append(event)
        self.total_events += 1


class LogTraceParseError(ParseError):
    """
        Log trace parse error exception.
    """
    pass


def get_str_from_elf(felf, str_addr):
    """
        Retrieves string from ELF file.

        Parameters
        ----------
        felf : elffile.ELFFile
            open ELF file handle to retrive format string from
        str_addr : int
            address of the string

        Returns
        -------
        string
            string or None if it was not found
    """
    tgt_str = ''
    for sect in felf.iter_sections():
        if sect['sh_addr'] == 0 or (sect['sh_flags'] & elfconst.SH_FLAGS.SHF_ALLOC) == 0:
            continue
        if str_addr < sect['sh_addr'] or str_addr >= sect['sh_addr'] + sect['sh_size']:
            continue
        sec_data = sect.data()
        for i in range(str_addr - sect['sh_addr'], sect['sh_size']):
            if type(sec_data) is str:
                ch = sec_data[i]
            else:
                ch = str(chr(sec_data[i]))
            if ch == '\0':
                break
            tgt_str += ch
        if len(tgt_str) > 0:
            return tgt_str
    return None


class LogTraceEvent:
    """
        Log trace event.
    """
    def __init__(self, fmt_addr, log_args):
        """
            Constructor.

            Parameters
            ----------
            fmt_addr : int
                address of the format string
            log_args : list
                list of log message arguments
        """
        self.fmt_addr = fmt_addr
        self.args = log_args

    def get_message(self, felf):
        """
            Retrieves log message.

            Parameters
            ----------
            felf : elffile.ELFFile
                open ELF file handle to retrive format string from

            Returns
            -------
            string
                formatted log message

            Raises
            ------
            LogTraceParseError
                if format string has not been found in ELF file
        """
        fmt_str = get_str_from_elf(felf, self.fmt_addr)
        if not fmt_str:
            raise LogTraceParseError('Failed to find format string for 0x%x' % self.fmt_addr)
        prcnt_idx = 0
        for i, arg in enumerate(self.args):
            prcnt_idx = fmt_str.find('%', prcnt_idx, -2)  # TODO: check str ending with %
            if prcnt_idx == -1:
                break
            prcnt_idx += 1  # goto next char
            if fmt_str[prcnt_idx] == 's':
                # find string
                arg_str = get_str_from_elf(felf, self.args[i])
                if arg_str:
                    self.args[i] = arg_str
                else:
                    self.args[i] = '<None>'
        fmt_str = fmt_str.replace('%p', '%x')
        return fmt_str % tuple(self.args)


class BaseLogTraceDataProcessorImpl:
    """
        Base implementation for log data processors.
    """
    def __init__(self, print_log_events=False, elf_path=''):
        """
            Constructor.

            Parameters
            ----------
            print_log_events : bool
                if True every log event will be printed as they arrive
            elf_path : string
                path to ELF file to retrieve format strings for log messages
        """
        if len(elf_path):
            self.felf = elffile.ELFFile(open(elf_path, 'rb'))
        else:
            self.felf = None
        self.print_log_events = print_log_events
        self.messages = []

    def cleanup(self):
        """
            Cleanup
        """
        if self.felf:
            self.felf.stream.close()

    def print_report(self):
        """
            Prints log report
        """
        print('=============== LOG TRACE REPORT ===============')
        print('Processed {:d} log messages.'.format(len(self.messages)))

    def on_new_event(self, event):
        """
            Processes log events.

            Parameters
            ----------
            event : LogTraceEvent
                Event object.
        """
        msg = event.get_message(self.felf)
        self.messages.append(msg)
        if self.print_log_events:
            print(msg, end='')


class HeapTraceParseError(ParseError):
    """
        Heap trace parse error exception.
    """
    pass


class HeapTraceDuplicateAllocError(HeapTraceParseError):
    """
        Heap trace duplicate allocation error exception.
    """
    def __init__(self, addr, new_size, prev_size):
        """
            Constructor.

            Parameters
            ----------
            addr : int
                memory block address
            new_size : int
                size of the new allocation
            prev_size : int
                size of the previous allocation
        """
        HeapTraceParseError.__init__(self, """Duplicate alloc @ 0x{:x}!
                                              New alloc is {:d} bytes,
                                              previous is {:d} bytes.""".format(addr, new_size, prev_size))


class HeapTraceEvent:
    """
        Heap trace event.
    """
    def __init__(self, trace_event, alloc, toolchain='', elf_path=''):
        """
            Constructor.

            Parameters
            ----------
            sys_view_event : TraceEvent
                trace event object related to this heap event
            alloc : bool
                True for allocation event, otherwise False
            toolchain_pref : string
                toolchain prefix to retrieve source line locations using addresses
            elf_path : string
                path to ELF file to retrieve format strings for log messages
        """
        self.trace_event = trace_event
        self.alloc = alloc
        self.toolchain = toolchain
        self.elf_path = elf_path
        if self.alloc:
            self.size = self.trace_event.params['size'].value
        else:
            self.size = None

    @property
    def addr(self):
        return self.trace_event.params['addr'].value

    @property
    def callers(self):
        return self.trace_event.params['callers'].value

    def __repr__(self):
        if len(self.toolchain) and len(self.elf_path):
            callers = os.linesep
            for addr in self.trace_event.params['callers'].value:
                if addr == 0:
                    break
                callers += '{}'.format(addr2line(self.toolchain, self.elf_path, addr))
        else:
            callers = ''
            for addr in self.trace_event.params['callers'].value:
                if addr == 0:
                    break
                if len(callers):
                    callers += ':'
                callers += '0x{:x}'.format(addr)
        if self.alloc:
            return '[{:.9f}] HEAP: Allocated {:d} bytes @ 0x{:x} from {} on core {:d} by: {}'.format(self.trace_event.ts,
                                                                                                     self.size, self.addr,
                                                                                                     self.trace_event.ctx_desc,
                                                                                                     self.trace_event.core_id,
                                                                                                     callers)
        else:
            return '[{:.9f}] HEAP: Freed bytes @ 0x{:x} from {} on core {:d} by: {}'.format(self.trace_event.ts,
                                                                                            self.addr, self.trace_event.ctx_desc,
                                                                                            self.trace_event.core_id, callers)


class BaseHeapTraceDataProcessorImpl:
    """
        Base implementation for heap data processors.
    """
    def __init__(self, print_heap_events=False):
        """
            Constructor.

            Parameters
            ----------
            print_heap_events : bool
                if True every heap event will be printed as they arrive
        """
        self._alloc_addrs = {}
        self.allocs = []
        self.frees = []
        self.heap_events_count = 0
        self.print_heap_events = print_heap_events

    def on_new_event(self, event):
        """
            Processes heap events. Keeps track of active allocations list.

            Parameters
            ----------
            event : HeapTraceEvent
                Event object.
        """
        self.heap_events_count += 1
        if self.print_heap_events:
            print(event)
        if event.alloc:
            if event.addr in self._alloc_addrs:
                raise HeapTraceDuplicateAllocError(event.addr, event.size, self._alloc_addrs[event.addr].size)
            self.allocs.append(event)
            self._alloc_addrs[event.addr] = event
        else:
            # do not treat free on unknown addresses as errors, because these blocks coould be allocated when tracing was disabled
            if event.addr in self._alloc_addrs:
                event.size = self._alloc_addrs[event.addr].size
                self.allocs.remove(self._alloc_addrs[event.addr])
                del self._alloc_addrs[event.addr]
            else:
                self.frees.append(event)

    def print_report(self):
        """
            Prints heap report
        """
        print('=============== HEAP TRACE REPORT ===============')
        print('Processed {:d} heap events.'.format(self.heap_events_count))
        if len(self.allocs) == 0:
            print('OK - Heap errors was not found.')
            return
        leaked_bytes = 0
        for alloc in self.allocs:
            leaked_bytes += alloc.size
            print(alloc)
            for free in self.frees:
                if free.addr > alloc.addr and free.addr <= alloc.addr + alloc.size:
                    print('Possible wrong free operation found')
                    print(free)
        print('Found {:d} leaked bytes in {:d} blocks.'.format(leaked_bytes, len(self.allocs)))
