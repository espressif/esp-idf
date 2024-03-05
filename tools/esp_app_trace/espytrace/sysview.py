# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import copy
import json
import re
import struct

import espytrace.apptrace as apptrace

SYSVIEW_EVTID_NOP                 = 0  # Dummy packet.
SYSVIEW_EVTID_OVERFLOW            = 1
SYSVIEW_EVTID_ISR_ENTER           = 2
SYSVIEW_EVTID_ISR_EXIT            = 3
SYSVIEW_EVTID_TASK_START_EXEC     = 4
SYSVIEW_EVTID_TASK_STOP_EXEC      = 5
SYSVIEW_EVTID_TASK_START_READY    = 6
SYSVIEW_EVTID_TASK_STOP_READY     = 7
SYSVIEW_EVTID_TASK_CREATE         = 8
SYSVIEW_EVTID_TASK_INFO           = 9
SYSVIEW_EVTID_TRACE_START         = 10
SYSVIEW_EVTID_TRACE_STOP          = 11
SYSVIEW_EVTID_SYSTIME_CYCLES      = 12
SYSVIEW_EVTID_SYSTIME_US          = 13
SYSVIEW_EVTID_SYSDESC             = 14
SYSVIEW_EVTID_MARK_START          = 15
SYSVIEW_EVTID_MARK_STOP           = 16
SYSVIEW_EVTID_IDLE                = 17
SYSVIEW_EVTID_ISR_TO_SCHEDULER    = 18
SYSVIEW_EVTID_TIMER_ENTER         = 19
SYSVIEW_EVTID_TIMER_EXIT          = 20
SYSVIEW_EVTID_STACK_INFO          = 21
SYSVIEW_EVTID_MODULEDESC          = 22
SYSVIEW_EVTID_INIT                = 24
SYSVIEW_EVENT_ID_PREDEF_LEN_MAX   = SYSVIEW_EVTID_INIT
SYSVIEW_EVTID_NAME_RESOURCE       = 25
SYSVIEW_EVTID_PRINT_FORMATTED     = 26
SYSVIEW_EVTID_NUMMODULES          = 27
SYSVIEW_EVENT_ID_PREDEF_MAX       = SYSVIEW_EVTID_NUMMODULES

SYSVIEW_EVENT_ID_MAX             = 200

SYSVIEW_MODULE_EVENT_OFFSET      = 512

SYSVIEW_SYNC_LEN                  = 10

_sysview_events_map = {
    'SYS_NOP': SYSVIEW_EVTID_NOP,
    'SYS_OVERFLOW': SYSVIEW_EVTID_OVERFLOW,
    'SYS_ISR_ENTER': SYSVIEW_EVTID_ISR_ENTER,
    'SYS_ISR_EXIT': SYSVIEW_EVTID_ISR_EXIT,
    'SYS_TASK_START_EXEC': SYSVIEW_EVTID_TASK_START_EXEC,
    'SYS_TASK_STOP_EXEC': SYSVIEW_EVTID_TASK_STOP_EXEC,
    'SYS_TASK_START_READY': SYSVIEW_EVTID_TASK_START_READY,
    'SYS_TASK_STOP_READY': SYSVIEW_EVTID_TASK_STOP_READY,
    'SYS_TASK_CREATE': SYSVIEW_EVTID_TASK_CREATE,
    'SYS_TASK_INFO': SYSVIEW_EVTID_TASK_INFO,
    'SYS_TRACE_START': SYSVIEW_EVTID_TRACE_START,
    'SYS_TRACE_STOP': SYSVIEW_EVTID_TRACE_STOP,
    'SYS_SYSTIME_CYCLES': SYSVIEW_EVTID_SYSTIME_CYCLES,
    'SYS_SYSTIME_US': SYSVIEW_EVTID_SYSTIME_US,
    'SYS_SYSDESC': SYSVIEW_EVTID_SYSDESC,
    'SYS_USER_START': SYSVIEW_EVTID_MARK_START,
    'SYS_USER_STOP': SYSVIEW_EVTID_MARK_STOP,
    'SYS_IDLE': SYSVIEW_EVTID_IDLE,
    'SYS_ISR_TO_SCHEDULER': SYSVIEW_EVTID_ISR_TO_SCHEDULER,
    'SYS_TIMER_ENTER': SYSVIEW_EVTID_TIMER_ENTER,
    'SYS_TIMER_EXIT': SYSVIEW_EVTID_TIMER_EXIT,
    'SYS_STACK_INFO': SYSVIEW_EVTID_STACK_INFO,
    'SYS_MODULEDESC': SYSVIEW_EVTID_INIT,
    'SYS_INIT': SYSVIEW_EVTID_INIT,
    'SYS_NAME_RESOURCE': SYSVIEW_EVTID_NAME_RESOURCE,
    'SYS_PRINT_FORMATTED': SYSVIEW_EVTID_PRINT_FORMATTED,
    'SYS_NUMMODULES': SYSVIEW_EVTID_NUMMODULES
}

_os_events_map = {}


def parse_trace(reader, parser, os_evt_map_file=''):
    """
    Parses trace.

    Parameters
    ----------
    reader : apptrace.Reader
        Trace reader object.
    parser : SysViewTraceDataParser
        Top level parser object.
    os_evt_map_file : string
        Path to file containg events format description.
    """
    global _os_events_map
    # parse OS events formats file
    _os_events_map = _read_events_map(os_evt_map_file)
    parser.esp_ext = ('; ESP_Extension\n' in _read_file_header(reader))
    _read_init_seq(reader)
    while True:
        event = parser.read_event(reader, _os_events_map)
        parser.on_new_event(event)


def _read_events_map(os_evt_map_file):
    """
    Reads SystemView events format description from file.

    Parameters
    ----------
    os_evt_map_file : string
        Path to file containg events format description.

    Returns
    -------
    dict
        a dict with event IDs as keys and values as tuples containg event name and a list of parameters.
    """
    os_evt_map = {}
    with open(os_evt_map_file) as f:
        for line in f:
            comps = line.split()
            if len(comps) < 2:
                continue
            params = []
            if len(comps) > 2:
                for p in comps[2:]:
                    sp = p.split('=')
                    if sp[1].startswith('%'):
                        sp[1] = sp[1][1:]
                    if sp[1] == 'u':
                        params.append(SysViewEventParamSimple(sp[0], _decode_u32))
                    elif sp[1] == 's':
                        params.append(SysViewEventParamSimple(sp[0], _decode_str))
                    elif sp[1] == 't' or sp[1] == 'T' or sp[1] == 'I' or sp[1] == 'p':
                        # TODO: handle shrinked task/queue ID and addresses
                        params.append(SysViewEventParamSimple(sp[0], _decode_u32))
            os_evt_map[int(comps[0])] = (comps[1], params)
    return os_evt_map


def _read_file_header(reader):
    """
    Reads SystemView trace file header.

    Parameters
    ----------
    reader : apptrace.Reader
        Trace reader object.

    Returns
    -------
    list
        a list of header lines.
    """
    empty_count = 0
    lines = []
    while empty_count < 2:
        lines.append(reader.readline(linesep='\n'))
        if lines[-1] == ';\n':
            empty_count += 1
    return lines


def _read_init_seq(reader):
    """
    Reads SystemView trace initial synchronisation sequence of bytes.

    Parameters
    ----------
    reader : apptrace.Reader
        Trace reader object.

    Raises
    -------
    SysViewTraceParseError
        If sync sequence is broken.
    """
    SYNC_SEQ_FMT = '<%dB' % SYSVIEW_SYNC_LEN
    sync_bytes = struct.unpack(SYNC_SEQ_FMT, reader.read(struct.calcsize(SYNC_SEQ_FMT)))
    for b in sync_bytes:
        if b != 0:
            raise SysViewTraceParseError('Invalid sync sequense!')


def _decode_u32(reader):
    """
    Reads and decodes unsigned 32-bit integer.

    Parameters
    ----------
    reader : apptrace.Reader
        Trace reader object.

    Returns
    -------
    tuple
        a tuple containg number of read bytes and decoded value.
    """
    sz = 0
    val = 0
    while True:
        b, = struct.unpack('<B', reader.read(1))
        if b & 0x80:
            val |= (b & 0x7F) << (7 * sz)
        else:
            val |= b << (7 * sz)
            break
        sz += 1
    return (sz + 1,val)


def _decode_id(reader):
    """
    Reads and decodes ID (task ID, queue handle, semaphore handle etc.).

    Parameters
    ----------
    reader : apptrace.Reader
        Trace reader object.

    Returns
    -------
    tuple
        a tuple containg number of read bytes and decoded value.
    """
    return _decode_u32(reader)


def _decode_u64(reader):
    """
    Reads and decodes unsigned 64-bit integer.

    Parameters
    ----------
    reader : apptrace.Reader
        Trace reader object.

    Returns
    -------
    tuple
        a tuple containg number of read bytes and decoded value.
    """
    sz,val = _decode_u32(reader)
    sz2,high = _decode_u32(reader)
    sz += sz2
    return sz,(val | (high << 32))


def _decode_str(reader):
    """
    Reads and decodes string.

    Parameters
    ----------
    reader : apptrace.Reader
        Trace reader object.

    Returns
    -------
    tuple
        a tuple containg number of read bytes and decoded value.
    """
    sz = 0
    val = ''
    sz, = struct.unpack('<B', reader.read(1))
    if sz == 0xFF:
        buf = struct.unpack('<2B', reader.read(2))
        sz = (buf[1] << 8) | buf[0]
    val, = struct.unpack('<%ds' % sz, reader.read(sz))
    val = val.decode('utf-8')
    if sz < 0xFF:
        return (sz + 1,val)  # one extra byte for length
    return (sz + 3,val)  # 3 extra bytes for length


def _decode_plen(reader):
    """
    Reads and decodes event's payload length.

    Parameters
    ----------
    reader : apptrace.Reader
        Trace reader object.

    Returns
    -------
    int
        decoded value.
    """
    plen = 0
    b0, = struct.unpack('<B', reader.read(1))
    if b0 & 0x80:
        b1, = struct.unpack('<B', reader.read(1))
        plen = b1  # higher part
        plen = (plen << 7) | (b0 & ~0x80)  # lower 7 bits
    else:
        plen = b0
    return plen


class SysViewTraceParseError(apptrace.ParseError):
    """
    SystemView parse error exception.
    """
    pass


class SysViewEvent(apptrace.TraceEvent):
    """
        Generic SystemView event class. This is a base class for all events.
    """
    def __init__(self, evt_id, core_id, reader, events_fmt_map=None):
        """
            Constructor. Reads and optionally decodes event.

            Parameters
            ----------
            evt_id : int
                Event ID.
            reader : apptrace.Reader
                Trace reader object.
            core_id : int
                Core ID event has been generated on.
            events_fmt_map : dict
                see return value of _read_events_map()
        """
        apptrace.TraceEvent.__init__(self, 'SysViewEvent', core_id, evt_id)
        self.plen = 0
        if self.id >= SYSVIEW_EVENT_ID_PREDEF_LEN_MAX:
            self.plen = _decode_plen(reader)
        if events_fmt_map:
            self._read_payload(reader, events_fmt_map)
        else:
            reader.forward(self.plen)
        _,self.ts = _decode_u32(reader)

    def _read_payload(self, reader, events_fmt_map):
        """
            Reads event's payload and populates its parameters dictionary.

            Parameters
            ----------
            reader : apptrace.Reader
                Trace reader object.
            events_fmt_map : dict
                see return value of _read_events_map()

            Raises
            -------
            SysViewTraceParseError
                if event has unknown or invalid format.
        """
        if self.id not in events_fmt_map:
            raise SysViewTraceParseError('Unknown event ID %d!' % self.id)
        self.name = events_fmt_map[self.id][0]
        evt_params_templates = events_fmt_map[self.id][1]
        params_len = 0
        for i in range(len(evt_params_templates)):
            event_param = copy.deepcopy(evt_params_templates[i])
            try:
                cur_pos = reader.get_pos()
                sz,param_val = event_param.decode(reader, self.plen - params_len)
            except Exception as e:
                raise SysViewTraceParseError('Failed to decode event {}({:d}) {:d} param @ 0x{:x}! {}'.format(self.name, self.id, self.plen, cur_pos, e))
            event_param.idx = i
            event_param.value = param_val
            self.params[event_param.name] = event_param
            params_len += sz
        if self.id >= SYSVIEW_EVENT_ID_PREDEF_LEN_MAX and self.plen != params_len:
            raise SysViewTraceParseError('Invalid event {}({:d}) payload len {:d}! Must be {:d}.'.format(self.name, self.id, self.plen, params_len))

    def __str__(self):
        params = ''
        for param in sorted(self.params.values(), key=lambda x: x.idx):
            params += '{}, '.format(param)
        if len(params):
            params = params[:-2]  # remove trailing ', '
        return '{:.9f} - core[{:d}].{}({:d}), plen {:d}: [{}]'.format(self.ts, self.core_id, self.name, self.id, self.plen, params)


class SysViewEventParam:
    """
        Abstract base SystemView event's parameter class. This is a base class for all event's parameters.
    """
    def __init__(self, name, decode_func):
        """
            Constructor.

            Parameters
            ----------
            name : string
                Event parameter name.
            decode_func : callable object
                Parameter decoding function.
        """
        self.name = name
        self.decode_func = decode_func
        self.value = None
        # positional index, used for sorting parameters before printing to make them looking as they appear in the event
        self.idx = 0

    def decode(self, reader, max_sz):
        """
            Reads and decodes events parameter.

            Parameters
            ----------
            reader : apptrace.Reader
                Trace reader object.
            max_sz : int
                Maximum number of bytes to read.

            Returns
            -------
            tuple
                a tuple containg number of read bytes and decoded value.
        """
        pass

    def __str__(self):
        return '{}: {}'.format(self.name, self.value)

    def to_jsonable(self):
        return {self.name: self.value}


class SysViewEventParamSimple(SysViewEventParam):
    """
        Simple SystemView event's parameter class.
    """
    def decode(self, reader, max_sz):
        """
            see SysViewEventParam.decode()
        """
        return self.decode_func(reader)


class SysViewEventParamArray(SysViewEventParamSimple):
    """
        Array SystemView event's parameter class.
    """
    def __init__(self, name, decode_func, size=-1):
        """
            Constructor.

            Parameters
            ----------
            name : string
                see SysViewEventParam.__init__()
            decode_func : callable object
                see SysViewEventParam.__init__()
            size : int
                Array's size. If -1 decode() will try to read all bytes from reader.
        """
        SysViewEventParamSimple.__init__(self, name, decode_func)
        self.arr_size = size

    def decode(self, reader, max_sz):
        """
            see SysViewEventParam.decode()
        """
        tottal_sz = 0
        vals = []
        i = 0
        while tottal_sz < max_sz:
            sz,val = self.decode_func(reader)
            vals.append(val)
            tottal_sz += sz
            i += 1
            if self.arr_size != -1 and i == self.arr_size:
                break
        return tottal_sz,vals


class SysViewPredefinedEvent(SysViewEvent):
    """
        Pre-defined SystemView events class.
    """
    _predef_events_fmt = {
        SYSVIEW_EVTID_NOP:              ('svNop', []),
        SYSVIEW_EVTID_OVERFLOW:         ('svOverflow', [SysViewEventParamSimple('drop_cnt', _decode_u32)]),
        SYSVIEW_EVTID_ISR_ENTER:        ('svIsrEnter', [SysViewEventParamSimple('irq_num', _decode_u32)]),
        SYSVIEW_EVTID_ISR_EXIT:         ('svIsrExit', []),
        SYSVIEW_EVTID_TASK_START_EXEC:  ('svTaskStartExec', [SysViewEventParamSimple('tid', _decode_id)]),
        SYSVIEW_EVTID_TASK_STOP_EXEC:   ('svTaskStopExec', []),
        SYSVIEW_EVTID_TASK_START_READY: ('svTaskStartReady', [SysViewEventParamSimple('tid', _decode_id)]),
        SYSVIEW_EVTID_TASK_STOP_READY:  ('svTaskStopReady', [SysViewEventParamSimple('tid', _decode_id),
                                                             SysViewEventParamSimple('cause', _decode_u32)]),
        SYSVIEW_EVTID_TASK_CREATE:      ('svTaskCreate', [SysViewEventParamSimple('tid', _decode_id)]),
        SYSVIEW_EVTID_TASK_INFO:        ('svTaskInfo', [SysViewEventParamSimple('tid', _decode_id),
                                                        SysViewEventParamSimple('prio', _decode_u32),
                                                        SysViewEventParamSimple('name', _decode_str)]),
        SYSVIEW_EVTID_TRACE_START:      ('svTraceStart', []),
        SYSVIEW_EVTID_TRACE_STOP:       ('svTraceStop', []),
        SYSVIEW_EVTID_SYSTIME_CYCLES:   ('svSysTimeCycles', [SysViewEventParamSimple('cycles', _decode_u32)]),
        SYSVIEW_EVTID_SYSTIME_US:       ('svSysTimeUs', [SysViewEventParamSimple('time', _decode_u64)]),
        SYSVIEW_EVTID_SYSDESC:          ('svSysDesc', [SysViewEventParamSimple('desc', _decode_str)]),
        SYSVIEW_EVTID_MARK_START:       ('svUserStart', [SysViewEventParamSimple('user_id', _decode_u32)]),
        SYSVIEW_EVTID_MARK_STOP:        ('svUserStop', [SysViewEventParamSimple('user_id', _decode_u32)]),
        SYSVIEW_EVTID_IDLE:             ('svIdle', []),
        SYSVIEW_EVTID_ISR_TO_SCHEDULER: ('svExitIsrToScheduler', []),
        SYSVIEW_EVTID_TIMER_ENTER:      ('svTimerEnter', [SysViewEventParamSimple('tim_id', _decode_u32)]),
        SYSVIEW_EVTID_TIMER_EXIT:       ('svTimerExit', []),
        SYSVIEW_EVTID_STACK_INFO:       ('svStackInfo', [SysViewEventParamSimple('tid', _decode_id),
                                                         SysViewEventParamSimple('base', _decode_u32),
                                                         SysViewEventParamSimple('sz', _decode_u32),
                                                         SysViewEventParamSimple('unused', _decode_u32)]),
        SYSVIEW_EVTID_MODULEDESC:       ('svModuleDesc', [SysViewEventParamSimple('mod_id', _decode_u32),
                                                          SysViewEventParamSimple('evt_off', _decode_u32),
                                                          SysViewEventParamSimple('desc', _decode_str)]),
        SYSVIEW_EVTID_INIT:             ('svInit', [SysViewEventParamSimple('sys_freq', _decode_u32),
                                                    SysViewEventParamSimple('cpu_freq', _decode_u32),
                                                    SysViewEventParamSimple('ram_base', _decode_u32),
                                                    SysViewEventParamSimple('id_shift', _decode_u32)]),
        SYSVIEW_EVTID_NAME_RESOURCE:    ('svNameResource', [SysViewEventParamSimple('res_id', _decode_u32),
                                                            SysViewEventParamSimple('name', _decode_str)]),
        SYSVIEW_EVTID_PRINT_FORMATTED:  ('svPrint', [SysViewEventParamSimple('msg', _decode_str),
                                                     SysViewEventParamSimple('lvl', _decode_u32),
                                                     SysViewEventParamSimple('unused', _decode_u32)]),
        SYSVIEW_EVTID_NUMMODULES:       ('svNumModules', [SysViewEventParamSimple('mod_cnt', _decode_u32)]),
    }

    def __init__(self, evt_id, core_id, reader):
        """
            see SysViewEvent.__init__()
        """
        SysViewEvent.__init__(self, evt_id, core_id, reader, self._predef_events_fmt)
        # self.name = 'SysViewPredefinedEvent'


class SysViewOSEvent(SysViewEvent):
    """
        OS related SystemView events class.
    """
    def __init__(self, evt_id, core_id, reader, events_fmt_map):
        """
            see SysViewEvent.__init__()
        """
        SysViewEvent.__init__(self, evt_id, core_id, reader, events_fmt_map)
        # self.name = 'SysViewOSEvent'


class SysViewHeapEvent(SysViewEvent):
    """
        Heap related SystemView events class.

        Attributes
        ----------
        events_fmt : dict
            see return value of _read_events_map()
    """
    events_fmt = {
        0:       ('esp_sysview_heap_trace_alloc', [SysViewEventParamSimple('addr', _decode_u32),
                                                   SysViewEventParamSimple('size', _decode_u32),
                                                   SysViewEventParamArray('callers', _decode_u32)]),
        1:       ('esp_sysview_heap_trace_free', [SysViewEventParamSimple('addr', _decode_u32),
                                                  SysViewEventParamArray('callers', _decode_u32)]),
    }

    def __init__(self, evt_id, core_id, events_off, reader):
        """
            Constructor. Reads and optionally decodes event.

            Parameters
            ----------
            evt_id : int
                see SysViewEvent.__init__()
            events_off : int
                Offset for heap events IDs. Greater or equal to SYSVIEW_MODULE_EVENT_OFFSET.
            reader : apptrace.Reader
                see SysViewEvent.__init__()
            core_id : int
                see SysViewEvent.__init__()
        """
        cur_events_map = {}
        for id in self.events_fmt:
            cur_events_map[events_off + id] = self.events_fmt[id]
        SysViewEvent.__init__(self, evt_id, core_id, reader, cur_events_map)
        # self.name = 'SysViewHeapEvent'


class SysViewTraceDataParser(apptrace.TraceDataProcessor):
    """
        Base SystemView trace data parser class.

        Attributes
        ----------
        STREAMID_SYS : int
            system events stream ID. Reserved for internal uses.
        STREAMID_LOG : int
            log events stream ID.
        STREAMID_HEAP : int
            heap events stream ID.
    """
    STREAMID_SYS = -1
    STREAMID_LOG = 0
    STREAMID_HEAP = 1

    def __init__(self, print_events=False, core_id=0):
        """
            Constructor.

            Parameters
            ----------
            print_events : bool
                see apptrace.TraceDataProcessor.__init__()
            core_id : int
                id of the core this parser object relates to.
        """
        apptrace.TraceDataProcessor.__init__(self, print_events=print_events, keep_all_events=True)
        self.sys_info = None
        self._last_ts = 0
        self.irqs_info = {}
        self.tasks_info = {}
        self.core_id = core_id
        self.esp_ext = False

    def _parse_irq_desc(self, desc):
        """
            Parses IRQ description.

            Parameters
            ----------
            desc : string
                IRQ description string.

            Returns
            -------
            tuple
                a tuple with IRQ number and name or None on error.
        """
        m = re.match('I#([0-9]+)=(.+)', desc)
        if m:
            return m.group(2),m.group(1)
        return None

    def _update_ts(self, ts):
        """
            Calculates real event timestamp.

            Parameters
            ----------
            ts : int
                Event timestamp offset.

            Returns
            -------
            float
                real event timestamp.
        """
        self._last_ts += ts
        return float(self._last_ts) / self.sys_info.params['sys_freq'].value

    def read_extension_event(self, evt_id, core_id, reader):
        """
            Reads extension event.
            Default implementation which just reads out event.

            Parameters
            ----------
            evt_id : int
                Event ID.
            reader : apptrace.Reader
                Trace reader object.

            Returns
            -------
            SysViewEvent
                if this is top level parser returns object for generic event,
                otherwise returns None indicating to the calling top level parser
                that extension event are not supported.
        """
        if self.root_proc == self:
            # by default just read out and skip unknown event
            return SysViewEvent(evt_id, core_id, reader)
        return None  # let decide to root parser

    @staticmethod
    def _decode_core_id(high_b):
        if high_b & (1 << 6):
            core_id = 1
            high_b &= ~(1 << 6)
        else:
            core_id = 0
        return high_b,core_id

    def read_event(self, reader, os_evt_map):
        """
            Reads pre-defined or OS-related event.

            Parameters
            ----------
            reader : apptrace.Reader
                Trace reader object.
            os_evt_map : dict
                see return value of _read_events_map()

            Returns
            -------
            SysViewEvent
                pre-defined, OS-related or extension event object.
        """
        evt_hdr, = struct.unpack('<B', reader.read(1))
        # read ID and core num
        evt_id = 0
        if evt_hdr & 0x80:
            # evt_id (2 bytes)
            b, = struct.unpack('<B', reader.read(1))
            # higher part
            if self.esp_ext:
                evt_id,core_id = self._decode_core_id(b)
            else:
                evt_id = b
                core_id = self.core_id
            evt_id = (evt_id << 7) | (evt_hdr & ~0x80)  # lower 7 bits
        else:
            # evt_id (1 byte)
            if self.esp_ext:
                evt_id,core_id = self._decode_core_id(evt_hdr)
            else:
                evt_id = evt_hdr
                core_id = self.core_id
        if evt_id <= SYSVIEW_EVENT_ID_PREDEF_MAX:
            return SysViewPredefinedEvent(evt_id, core_id, reader)
        elif evt_id < SYSVIEW_MODULE_EVENT_OFFSET:
            return SysViewOSEvent(evt_id, core_id, reader, os_evt_map)
        else:
            return self.read_extension_event(evt_id, core_id, reader)

    def event_supported(self, event):
        return False

    def on_new_event(self, event):
        """
            Does essential processing of event. Must be called for every read event.
            Assigns real timestamp to event. Updates global system info: system description,
            mapping of tasks IDs to names and IRQ names to numbers.

            Parameters
            ----------
            event : SysViewEvent
                Event object.
        """
        if event.id == SYSVIEW_EVTID_TRACE_START:
            event.ts = 0
            self._last_ts = 0
        elif self.sys_info:
            event.ts = self._update_ts(event.ts)

        if event.id == SYSVIEW_EVTID_INIT:
            self.sys_info = event
            event.ts = self._update_ts(event.ts)
        elif event.id == SYSVIEW_EVTID_TASK_INFO:
            self.tasks_info[event.params['tid'].value] = event.params['name'].value
        elif event.id == SYSVIEW_EVTID_SYSDESC:
            irq = self._parse_irq_desc(event.params['desc'].value)
            if irq:
                irq_num = int(irq[1])
                self.irqs_info[irq_num] = irq[0]
        # count event and save it to the list
        apptrace.TraceDataProcessor.on_new_event(self, event)


class SysViewTraceDataExtEventParser(SysViewTraceDataParser):
    def __init__(self, events_num, print_events=False, core_id=0):
        """
            Constructor.

            Parameters
            ----------
            print_events : bool
                see apptrace.TraceDataProcessor.__init__()
            core_id : int
                id of the core this parser object relates to.
        """
        SysViewTraceDataParser.__init__(self, core_id=core_id, print_events=print_events)
        self.events_off = 0
        self.events_num = events_num

    def event_supported(self, event):
        return False if (self.events_off < SYSVIEW_MODULE_EVENT_OFFSET or event.id < self.events_off or
                        event.id >= (self.events_off + self.events_num)) else True


class SysViewMultiTraceDataParser(SysViewTraceDataParser):
    """
        SystemView trace data parser supporting multiple event streams.
    """
    def __init__(self, print_events=False, core_id=0):
        """
            see SysViewTraceDataParser.__init__()
        """
        SysViewTraceDataParser.__init__(self, print_events, core_id)
        self.stream_parsers = {}

    def add_stream_parser(self, stream_id, parser):
        """
            Assigns parser for events stream.

            Parameters
            ----------
            stream_id : int
                stream ID. See SysViewTraceDataParser.STREAMID_xxx.
                Parsers for SysViewTraceDataParser.STREAMID_SYS are ignored.
                Top level parser is the default for SysViewTraceDataParser.STREAMID_SYS.
            parser : SysViewTraceDataParser
                parser object.
        """
        if stream_id == SysViewTraceDataParser.STREAMID_SYS:
            return
        parser.root_proc = self
        self.stream_parsers[stream_id] = parser

    def read_extension_event(self, evt_id, core_id, reader):
        """
            Reads extension event.
            Iterates over registered stream parsers trying to find one which supports that type of event.

            Parameters
            ----------
            evt_id : int
                see SysViewTraceDataParser.read_extension_event()
            reader : apptrace.Reader
                see SysViewTraceDataParser.read_extension_event()

            Returns
            -------
            SysViewEvent
                object for extension event, if extension event is not supported return SysViewEvent instance.
        """
        for stream_id in self.stream_parsers:
            evt = self.stream_parsers[stream_id].read_extension_event(evt_id, core_id, reader)
            if evt:
                return evt
        return SysViewTraceDataParser.read_extension_event(self, evt_id, core_id, reader)

    def on_new_event(self, event):
        """
            Iterates over registered stream parsers allowing them to do
            essential processing of event. Must be called for every read event.

            Parameters
            ----------
            event : SysViewEvent
                Event object.
        """
        SysViewTraceDataParser.on_new_event(self, event)
        for stream_id in self.stream_parsers:
            self.stream_parsers[stream_id].on_new_event(event)


class SysViewEventContext():
    """
        SystemView event context.
    """
    def __init__(self, handle, irq, name=''):
        """
            Constructor.

            Parameters
            ----------
            handle : int
                handle of the context: task ID or IRQ number.
            irq : bool
                flag indicating whether this is IRQ or task context.
            name : string
                name of the context: task or IRQ name. Empty if context is unknown.
       """
        self.handle = handle
        self.irq = irq
        self.name = name


class SysViewTraceDataProcessor(apptrace.TraceDataProcessor):
    """
        Base SystemView trace data processor class.
    """
    def __init__(self, traces, root_proc=None, print_events=False, keep_all_events=False):
        """
            Constructor.

            Parameters
            ----------
            traces : list
                list of parsers to process data from.
            print_events : bool
                see apptrace.TraceDataProcessor.__init__()
            keep_all_events : bool
                see apptrace.TraceDataProcessor.__init__()
        """
        apptrace.TraceDataProcessor.__init__(self, print_events, keep_all_events)
        self.event_ids = {}
        self.name = ''
        self.root_proc = root_proc if root_proc else self
        self.traces = {}
        self.ctx_stack = {}
        self.prev_ctx = {}
        self.no_ctx_events = []
        for t in traces:
            self.traces[t.core_id] = t
            # current context item is a tuple of task ID or IRQ num and 'in_irq' flag
            # empty list means IDLE context or self.start_ctx
            self.ctx_stack[t.core_id] = []
            # context is undefined, we do not know have we started the tracing in task/IDLE or IRQ context
            # in general there are three scenarious when we can start tracing: when core is in task, IDLE task or IRQ context
            self.prev_ctx[t.core_id] = None

    def _get_curr_context(self, core_id):
        """
            Returns current context.

            Parameters
            ----------
            core_id : int
                core ID for requested context.

            Returns
            -------
            SysViewEventContext
                context object
            None
                if there current is undefined
        """
        if len(self.root_proc.ctx_stack[core_id]):
            return self.root_proc.ctx_stack[core_id][-1]
        if self._get_prev_context(core_id):
            return SysViewEventContext(None, False, 'IDLE%d' % core_id)
        return None

    def _get_prev_context(self, core_id):
        """
            Returns current context.

            Parameters
            ----------
            core_id : int
                core ID for requested context.

            Returns
            -------
            SysViewEventContext
                context object
        """
        return self.root_proc.prev_ctx[core_id]

    def get_trace_stream(self, core_id, stream_id):
        """
            Retrieves parser for specified stream and core.

            Parameters
            ----------
            core_id : int
                Parser's core ID.
            stream_id : int
                Parser's stream ID.

            Returns
            -------
            SysViewTraceDataParser
                parser object for specified stream and core
        """
        if self.root_proc == self:
            return self.traces[core_id]
        return self.root_proc.get_trace_stream(core_id, stream_id)

    def event_supported(self, e):
        """
            Should be overriden in child class.
        """
        return False

    def handle_event(self, e):
        """
            Should be overriden in child class.
        """
        pass

    def print_report(self):
        """
            see apptrace.TraceDataProcessor.print_report()
        """
        apptrace.TraceDataProcessor.print_report(self)

    def _process_event(self, event):
        """
            Processes event.
            Keeps track of execution context on every core.

            Parameters
            ----------
            event : SysViewEvent
                Event object.

            Raises
            ----------
            SysViewTraceParseError
                if there is no parser for event's core or
                if SYSVIEW_EVTID_ISR_ENTER is received for unknown IRQ or
                if SYSVIEW_EVTID_TASK_START_EXEC or SYSVIEW_EVTID_TASK_STOP_READY is received for unknown task.
        """
        if event.core_id not in self.traces:
            if 0 in self.traces and self.traces[0].esp_ext:
                # for Espressif extension there is one trace for all cores
                trace = self.traces[0]
                if event.core_id not in self.ctx_stack:
                    self.ctx_stack[event.core_id] = []
                if event.core_id not in self.prev_ctx:
                    self.prev_ctx[event.core_id] = None
            else:
                raise SysViewTraceParseError('Event for unknown core %d' % event.core_id)
        else:
            trace = self.traces[event.core_id]
        if event.id == SYSVIEW_EVTID_ISR_ENTER:
            if event.params['irq_num'].value not in trace.irqs_info:
                raise SysViewTraceParseError('Enter unknown ISR %d' % event.params['irq_num'].value)
            if len(self.ctx_stack[event.core_id]):
                self.prev_ctx[event.core_id] = self.ctx_stack[event.core_id][-1]
            else:
                # the 1st context switching event after trace start is SYSVIEW_EVTID_ISR_ENTER, so we have been in IDLE context
                self.prev_ctx[event.core_id] = SysViewEventContext(None, False, 'IDLE%d' % event.core_id)
            # put new ISR context on top of the stack (the last in the list)
            self.ctx_stack[event.core_id].append(SysViewEventContext(event.params['irq_num'].value, True, trace.irqs_info[event.params['irq_num'].value]))
        elif event.id == SYSVIEW_EVTID_ISR_EXIT or event.id == SYSVIEW_EVTID_ISR_TO_SCHEDULER:
            if len(self.ctx_stack[event.core_id]):
                # return to the previous context (the last in the list)
                self.prev_ctx[event.core_id] = self.ctx_stack[event.core_id].pop()
            else:
                # the 1st context switching event after trace start is SYSVIEW_EVTID_ISR_EXIT, so we have been in ISR context,
                # but we do not know which one because SYSVIEW_EVTID_ISR_EXIT do not include the IRQ number
                self.prev_ctx[event.core_id] = SysViewEventContext(None, True, 'IRQ_oncore%d' % event.core_id)
        elif event.id == SYSVIEW_EVTID_TASK_START_EXEC:
            if event.params['tid'].value not in trace.tasks_info:
                raise SysViewTraceParseError('Start exec unknown task 0x%x' % event.params['tid'].value)
            if len(self.ctx_stack[event.core_id]):
                # return to the previous context (the last in the list)
                self.prev_ctx[event.core_id] = self.ctx_stack[event.core_id][-1]
            else:
                # the 1st context switching event after trace start is SYSVIEW_EVTID_TASK_START_EXEC, so we have been in IDLE context
                self.prev_ctx[event.core_id] = SysViewEventContext(None, False, 'IDLE%d' % event.core_id)
            # only one task at a time in context stack (can be interrupted by a bunch of ISRs)
            self.ctx_stack[event.core_id] = [SysViewEventContext(event.params['tid'].value, False, trace.tasks_info[event.params['tid'].value])]
        elif event.id == SYSVIEW_EVTID_TASK_STOP_EXEC:
            # delete task from context stack
            for ctx in self.ctx_stack[event.core_id]:
                if not ctx.irq:
                    if len(self.ctx_stack[event.core_id]) == 1:
                        # if this is the only ctx in context stack
                        self.prev_ctx[event.core_id] = ctx
                    del ctx
                    break
        elif event.id == SYSVIEW_EVTID_TASK_STOP_READY:
            if event.params['tid'].value not in trace.tasks_info:
                raise SysViewTraceParseError('Stop ready unknown task 0x%x' % event.params['tid'].value)
            if len(self.ctx_stack[event.core_id]):
                if (not self.ctx_stack[event.core_id][-1].irq and event.params['tid'].value == self.ctx_stack[event.core_id][-1].handle):
                    # return to the previous context (the last in the list)
                    self.prev_ctx[event.core_id] = self.ctx_stack[event.core_id].pop()
            else:
                # the 1st context switching event after trace start is SYSVIEW_EVTID_TASK_STOP_READY, so we have been in task context
                self.prev_ctx[event.core_id] = SysViewEventContext(event.params['tid'].value, False, trace.tasks_info[event.params['tid'].value])

    def on_new_event(self, event):
        """
            Processes heap events.
        """
        if self.root_proc == self:
            SysViewTraceDataProcessor._process_event(self, event)
        curr_ctx = self._get_curr_context(event.core_id)
        if not curr_ctx:
            # postpone events handling till their context is known
            self.no_ctx_events.append(event)
            return
        event.in_irq = curr_ctx.irq
        event.ctx_name = curr_ctx.name
        # here we know the previous context: we switched from it or implied upon the 1st context switching event
        prev_ctx = self._get_prev_context(event.core_id)
        if len(self.no_ctx_events):
            for cached_evt in self.no_ctx_events:
                cached_evt.ctx_name = prev_ctx.name
                cached_evt.in_irq = prev_ctx.irq
                # count and save the event
                apptrace.TraceDataProcessor.on_new_event(self, cached_evt)
                if self.event_supported(event):
                    self.handle_event(event)
            del self.no_ctx_events[:]
        # count and save the event
        apptrace.TraceDataProcessor.on_new_event(self, event)
        if self.event_supported(event):
            self.handle_event(event)

    def merge_and_process(self):
        """
            Merges events from all registered parsers, sorts them by timestamp and processes them.
        """
        all_events = []
        for t in self.traces.values():
            all_events.extend(t.events)
        all_events.sort(key=lambda x: x.ts)
        for event in all_events:
            self.on_new_event(event)


class SysViewMultiStreamTraceDataProcessor(SysViewTraceDataProcessor):
    """
        SystemView trace data processor supporting multiple event streams.
    """
    def __init__(self, traces, print_events=False, keep_all_events=False):
        """
            see SysViewTraceDataProcessor.__init__()
        """
        SysViewTraceDataProcessor.__init__(self, traces, print_events=print_events, keep_all_events=keep_all_events)
        self.stream_procs = {}

    def add_stream_processor(self, stream_id, proc):
        """
            Assigns processor for events stream.

            Parameters
            ----------
            stream_id : int
                stream ID. See SysViewTraceDataParser.STREAMID_xxx.
                Parsers for SysViewTraceDataParser.STREAMID_SYS are ignored.
                Top level parser is the default for SysViewTraceDataParser.STREAMID_SYS.
            proc : SysViewTraceDataProcessor
                processor object.
        """
        if stream_id == SysViewTraceDataParser.STREAMID_SYS:
            return
        proc.root_proc = self  # common processing data is kept by this processor
        self.stream_procs[stream_id] = proc

    def get_trace_stream(self, core_id, stream_id):
        """
            Retrieves parser for specified stream and core.

            Parameters
            ----------
            core_id : int
                Parser's core ID.
            stream_id : int
                Parser's stream ID.

            Returns
            -------
            SysViewTraceDataParser
                parser object for specified stream and core
        """
        if core_id not in self.traces and 0 in self.traces and self.traces[0].esp_ext:
            # for Espressif extension there is one trace for all cores
            trace = self.traces[0]
        else:
            trace = self.traces[core_id]
        if stream_id == SysViewTraceDataParser.STREAMID_SYS:
            return trace
        if isinstance(trace, SysViewMultiTraceDataParser):
            return trace.stream_parsers[stream_id]
        return trace

    def print_report(self):
        """
            Iterates over registered stream processors and prints their reports.
        """
        SysViewTraceDataProcessor.print_report(self)
        # need to sort stream procs by keys to print reports in the same order regardless of Python version
        stream_ids = list(self.stream_procs.keys())
        stream_ids.sort()
        for stream_id in stream_ids:
            self.stream_procs[stream_id].print_report()

    def cleanup(self):
        """
            Iterates over registered stream processors and cleans them up.
        """
        for stream_id in self.stream_procs:
            self.stream_procs[stream_id].cleanup()
        SysViewTraceDataProcessor.cleanup(self)

    def on_new_event(self, event):
        """
            Iterates over registered stream processors allowing them to do
            the processing of event.

            Parameters
            ----------
            event : SysViewEvent
                Event object.
        """
        SysViewTraceDataProcessor.on_new_event(self, event)
        for stream_id in self.stream_procs:
            self.stream_procs[stream_id].on_new_event(event)


class SysViewTraceDataJsonEncoder(json.JSONEncoder):
    JSON_TRACE_VER = '1.0'

    def default(self, obj):
        global _sysview_events_map
        global _os_events_map
        if isinstance(obj, SysViewMultiStreamTraceDataProcessor):
            json_event_ids = {'system': _sysview_events_map, 'os': {}}
            for eid in _os_events_map:
                ename = _os_events_map[eid][0]
                json_event_ids['os'][ename] = eid
            for stream in obj.stream_procs.values():
                json_event_ids[stream.name] = stream.event_ids
            json_events = []
            for e in obj.events:
                for stream in obj.stream_procs.values():
                    if stream.event_supported(e):
                        json_events.append(e)
                        break
                # include also OS and pre-defined events
                if isinstance(e, SysViewPredefinedEvent) or isinstance(e, SysViewOSEvent):
                    json_events.append(e)
            return {'version': self.JSON_TRACE_VER, 'streams': json_event_ids, 'events': json_events}
        if isinstance(obj, SysViewHeapEvent):
            blk_size = 0
            if 'size' in obj.params:
                blk_size = obj.params['size'].value
            blk_addr = '0x{:x}'.format(obj.params['addr'].value)
            callers = []
            for addr in obj.params['callers'].value:
                callers.append('0x{:x}'.format(addr))
            return {'ctx_name': obj.ctx_name, 'in_irq': obj.in_irq, 'id': obj.id, 'core_id': obj.core_id,
                    'ts': obj.ts, 'addr': blk_addr, 'size': blk_size, 'callers': callers}
        if isinstance(obj, SysViewPredefinedEvent) and obj.id == SYSVIEW_EVTID_PRINT_FORMATTED:
            return {'ctx_name': obj.ctx_name, 'in_irq': obj.in_irq, 'id': obj.id, 'core_id': obj.core_id,
                    'ts': obj.ts, 'msg': obj.params['msg'].value, 'lvl': obj.params['lvl'].value}
        if isinstance(obj, SysViewEvent):
            jobj = obj.to_jsonable()
            # remove unused fields
            if 'name' in jobj:
                del jobj['name']
            if 'plen' in jobj:
                del jobj['plen']
            return jobj
        # Let the base class default method raise the TypeError
        return json.JSONEncoder.default(self, obj)


class SysViewHeapTraceDataParser(SysViewTraceDataExtEventParser):
    """
        SystemView trace data parser supporting heap events.
    """
    def __init__(self, print_events=False, core_id=0):
        """
            SystemView trace data parser supporting multiple event streams.
            see SysViewTraceDataExtEventParser.__init__()
        """
        SysViewTraceDataExtEventParser.__init__(self, events_num=len(SysViewHeapEvent.events_fmt.keys()), core_id=core_id, print_events=print_events)

    def read_extension_event(self, evt_id, core_id, reader):
        """
            Reads heap event.
            see SysViewTraceDataParser.read_extension_event()
        """
        if (self.events_off >= SYSVIEW_MODULE_EVENT_OFFSET and evt_id >= self.events_off and
                evt_id < self.events_off + self.events_num):
            return SysViewHeapEvent(evt_id, core_id, self.events_off, reader)
        return SysViewTraceDataParser.read_extension_event(self, evt_id, core_id, reader)

    def on_new_event(self, event):
        """
            Keeps track of heap module descriptions.
        """
        if self.root_proc == self:
            SysViewTraceDataParser.on_new_event(self, event)
        if event.id == SYSVIEW_EVTID_MODULEDESC and event.params['desc'].value == 'ESP32 SystemView Heap Tracing Module':
            self.events_off = event.params['evt_off'].value


class SysViewHeapTraceDataProcessor(SysViewTraceDataProcessor, apptrace.BaseHeapTraceDataProcessorImpl):
    """
        SystemView trace data processor supporting heap events.
    """
    def __init__(self, toolchain_pref, elf_path, root_proc=None, traces=[], print_events=False, print_heap_events=False):
        """
            Constructor.
            see SysViewTraceDataProcessor.__init__()
            see apptrace.BaseHeapTraceDataProcessorImpl.__init__()
        """
        SysViewTraceDataProcessor.__init__(self, traces, root_proc=root_proc, print_events=print_events)
        apptrace.BaseHeapTraceDataProcessorImpl.__init__(self, print_heap_events)
        self.toolchain = toolchain_pref
        self.elf_path = elf_path
        # self.no_ctx_events = []
        self.name = 'heap'
        stream = self.root_proc.get_trace_stream(0, SysViewTraceDataParser.STREAMID_HEAP)
        self.event_ids = {'alloc': stream.events_off, 'free': stream.events_off + 1}

    def event_supported(self, event):
        heap_stream = self.root_proc.get_trace_stream(event.core_id, SysViewTraceDataParser.STREAMID_HEAP)
        return heap_stream.event_supported(event)

    def handle_event(self, event):
        heap_stream = self.root_proc.get_trace_stream(event.core_id, SysViewTraceDataParser.STREAMID_HEAP)
        if (event.id - heap_stream.events_off) == 0:
            heap_event = apptrace.HeapTraceEvent(event, True, toolchain=self.toolchain,
                                                 elf_path=self.elf_path)
        else:
            heap_event = apptrace.HeapTraceEvent(event, False, toolchain=self.toolchain,
                                                 elf_path=self.elf_path)
        apptrace.BaseHeapTraceDataProcessorImpl.on_new_event(self, heap_event)

    def print_report(self):
        """
            see apptrace.TraceDataProcessor.print_report()
        """
        if self.root_proc == self:
            SysViewTraceDataProcessor.print_report(self)
        apptrace.BaseHeapTraceDataProcessorImpl.print_report(self)


class SysViewLogTraceEvent(apptrace.LogTraceEvent):
    """
        SystemView log event.
    """
    def __init__(self, ts, msg):
        """
            Constructor.

            Parameters
            ----------
            msg : string
                Log message string.
        """
        self.msg = msg
        self.ts = ts

    def get_message(self, unused):
        """
            Retrieves log message.

            Returns
            -------
            string
                formatted log message
        """
        return '[{:.9f}] LOG: {}'.format(self.ts, self.msg)


class SysViewLogTraceDataParser(SysViewTraceDataParser):
    """
        SystemView trace data parser supporting log events.
    """
    def event_supported(self, event):
        return event.id == SYSVIEW_EVTID_PRINT_FORMATTED

    def on_new_event(self, event):
        """
            see SysViewTraceDataParser.on_new_event()
        """
        if self.root_proc == self:
            SysViewTraceDataParser.on_new_event(self, event)


class SysViewLogTraceDataProcessor(SysViewTraceDataProcessor, apptrace.BaseLogTraceDataProcessorImpl):
    """
        SystemView trace data processor supporting heap events.
    """
    def __init__(self, traces=[], root_proc=None, print_events=False, print_log_events=False):
        """
            Constructor.
            see SysViewTraceDataProcessor.__init__()
            see apptrace.BaseLogTraceDataProcessorImpl.__init__()
        """
        SysViewTraceDataProcessor.__init__(self, traces, root_proc=root_proc, print_events=print_events)
        apptrace.BaseLogTraceDataProcessorImpl.__init__(self, print_log_events)
        self.name = 'log'
        self.event_ids = {'print': SYSVIEW_EVTID_PRINT_FORMATTED}

    def event_supported(self, event):
        return event.id == SYSVIEW_EVTID_PRINT_FORMATTED

    def on_new_event(self, event):
        """
            Processes log events.
        """
        if self.root_proc == self:
            SysViewTraceDataProcessor.on_new_event(self, event)
        if event.id == SYSVIEW_EVTID_PRINT_FORMATTED:
            log_evt = SysViewLogTraceEvent(event.ts, event.params['msg'].value)
            apptrace.BaseLogTraceDataProcessorImpl.on_new_event(self, log_evt)

    def print_report(self):
        """
            see apptrace.TraceDataProcessor.print_report()
        """
        if self.root_proc == self:
            SysViewTraceDataProcessor.print_report(self)
        apptrace.BaseLogTraceDataProcessorImpl.print_report(self)
