# Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
DUT provides 3 major groups of features:

* DUT port feature, provide basic open/close/read/write features
* DUT tools, provide extra methods to control the device, like download and start app
* DUT expect method, provide features for users to check DUT outputs

The current design of DUT have 3 classes for one DUT: BaseDUT, DUTPort, DUTTool.

* BaseDUT class:
    * defines methods DUT port and DUT tool need to overwrite
    * provide the expect methods and some other methods based on DUTPort
* DUTPort class:
    * inherent from BaseDUT class
    * implements the port features by overwriting port methods defined in BaseDUT
* DUTTool class:
    * inherent from one of the DUTPort class
    * implements the tools features by overwriting tool methods defined in BaseDUT
    * could add some new methods provided by the tool

This module implements the BaseDUT class and one of the port class SerialDUT.
User should implement their DUTTool classes.
If they using different port then need to implement their DUTPort class as well.
"""

from __future__ import print_function
import time
import re
import sys
import threading
import copy
import functools

# python2 and python3 queue package name is different
try:
    import Queue as _queue
except ImportError:
    import queue as _queue

import serial
from serial.tools import list_ports

from . import Utility


class ExpectTimeout(ValueError):
    """ timeout for expect method """
    pass


class UnsupportedExpectItem(ValueError):
    """ expect item not supported by the expect method """
    pass


def _expect_lock(func):
    @functools.wraps(func)
    def handler(self, *args, **kwargs):
        with self.expect_lock:
            ret = func(self, *args, **kwargs)
        return ret
    return handler


def _decode_data(data):
    """ for python3, if the data is bytes, then decode it to string """
    if isinstance(data, bytes):
        # convert bytes to string. This is a bit of a hack, we know that we want to log this
        # later so encode to the stdout encoding with backslash escapes for anything non-encodable
        encoding = sys.stdout.encoding
        if encoding is None:
            encoding = 'ascii'
        try:
            return data.decode(encoding, "backslashreplace")
        except (UnicodeDecodeError, TypeError):  # Python <3.5 doesn't support backslashreplace
            return data.decode(encoding, "replace")
    return data


def _pattern_to_string(pattern):
    try:
        ret = "RegEx: " + pattern.pattern
    except AttributeError:
        ret = pattern
    return ret


class _DataCache(_queue.Queue):
    """
    Data cache based on Queue. Allow users to process data cache based on bytes instead of Queue."
    """

    def __init__(self, maxsize=0):
        _queue.Queue.__init__(self, maxsize=maxsize)
        self.data_cache = str()

    def _move_from_queue_to_cache(self):
        """
        move all of the available data in the queue to cache

        :return: True if moved any item from queue to data cache, else False
        """
        ret = False
        while True:
            try:
                self.data_cache += _decode_data(self.get(0))
                ret = True
            except _queue.Empty:
                break
        return ret

    def get_data(self, timeout=0.0):
        """
        get a copy of data from cache.

        :param timeout: timeout for waiting new queue item
        :return: copy of data cache
        """
        # make sure timeout is non-negative
        if timeout < 0:
            timeout = 0

        ret = self._move_from_queue_to_cache()

        if not ret:
            # we only wait for new data if we can't provide a new data_cache
            try:
                data = self.get(timeout=timeout)
                self.data_cache += _decode_data(data)
            except _queue.Empty:
                # don't do anything when on update for cache
                pass
        return copy.deepcopy(self.data_cache)

    def flush(self, index=0xFFFFFFFF):
        """
        flush data from cache.

        :param index: if < 0 then don't do flush, otherwise flush data before index
        :return: None
        """
        # first add data in queue to cache
        self.get_data()

        if index > 0:
            self.data_cache = self.data_cache[index:]


class _LogThread(threading.Thread, _queue.Queue):
    """
    We found some SD card on Raspberry Pi could have very bad performance.
    It could take seconds to save small amount of data.
    If the DUT receives data and save it as log, then it stops receiving data until log is saved.
    This could lead to expect timeout.
    As an workaround to this issue, ``BaseDUT`` class will create a thread to save logs.
    Then data will be passed to ``expect`` as soon as received.
    """
    def __init__(self):
        threading.Thread.__init__(self, name="LogThread")
        _queue.Queue.__init__(self, maxsize=0)
        self.setDaemon(True)
        self.flush_lock = threading.Lock()

    def save_log(self, filename, data):
        """
        :param filename: log file name
        :param data: log data. Must be ``bytes``.
        """
        self.put({"filename": filename, "data": data})

    def flush_data(self):
        with self.flush_lock:
            data_cache = dict()
            while True:
                # move all data from queue to data cache
                try:
                    log = self.get_nowait()
                    try:
                        data_cache[log["filename"]] += log["data"]
                    except KeyError:
                        data_cache[log["filename"]] = log["data"]
                except _queue.Empty:
                    break
            # flush data
            for filename in data_cache:
                with open(filename, "ab+") as f:
                    f.write(data_cache[filename])

    def run(self):
        while True:
            time.sleep(1)
            self.flush_data()


class RecvThread(threading.Thread):

    CHECK_FUNCTIONS = []
    """ DUT subclass can define a few check functions to process received data. """

    def __init__(self, read, dut):
        super(RecvThread, self).__init__()
        self.exit_event = threading.Event()
        self.setDaemon(True)
        self.read = read
        self.dut = dut
        self.data_cache = dut.data_cache
        self.recorded_data = dut.recorded_data
        self.record_data_lock = dut.record_data_lock
        self._line_cache = str()

    def _line_completion(self, data):
        """
        Usually check functions requires to check for one complete line.
        This method will do line completion for the first line, and strip incomplete last line.
        """
        ret = self._line_cache
        decoded_data = _decode_data(data)

        # cache incomplete line to later process
        lines = decoded_data.splitlines(True)
        last_line = lines[-1]

        if last_line[-1] != "\n":
            if len(lines) == 1:
                # only one line and the line is not finished, then append this to cache
                self._line_cache += lines[-1]
                ret = str()
            else:
                # more than one line and not finished, replace line cache
                self._line_cache = lines[-1]
                ret += "".join(lines[:-1])
        else:
            # line finishes, flush cache
            self._line_cache = str()
            ret += decoded_data
        return ret

    def run(self):
        while not self.exit_event.isSet():
            raw_data = self.read(1000)
            if raw_data:
                with self.record_data_lock:
                    self.data_cache.put(raw_data)
                    for capture_id in self.recorded_data:
                        self.recorded_data[capture_id].put(raw_data)

                # we need to do line completion before call check functions
                comp_data = self._line_completion(raw_data)
                for check_function in self.CHECK_FUNCTIONS:
                    check_function(self, comp_data)

    def exit(self):
        self.exit_event.set()
        self.join()


class BaseDUT(object):
    """
    :param name: application defined name for port
    :param port: comport name, used to create DUT port
    :param log_file: log file name
    :param app: test app instance
    :param kwargs: extra args for DUT to create ports
    """

    DEFAULT_EXPECT_TIMEOUT = 10
    MAX_EXPECT_FAILURES_TO_SAVED = 10
    RECV_THREAD_CLS = RecvThread
    TARGET = None
    """ DUT subclass can specify RECV_THREAD_CLS to do add some extra stuff when receive data.
    For example, DUT can implement exception detect & analysis logic in receive thread subclass. """
    LOG_THREAD = _LogThread()
    LOG_THREAD.start()

    def __init__(self, name, port, log_file, app, **kwargs):

        self.expect_lock = threading.Lock()
        self.name = name
        self.port = port
        self.log_file = log_file
        self.app = app
        self.data_cache = _DataCache()
        # the main process of recorded data are done in receive thread
        # but receive thread could be closed in DUT lifetime (tool methods)
        # so we keep it in BaseDUT, as their life cycle are same
        self.recorded_data = dict()
        self.record_data_lock = threading.RLock()
        self.receive_thread = None
        self.expect_failures = []
        self._port_open()
        self.start_receive()

    def __str__(self):
        return "DUT({}: {})".format(self.name, str(self.port))

    def _save_expect_failure(self, pattern, data, start_time):
        """
        Save expect failure. If the test fails, then it will print the expect failures.
        In some cases, user will handle expect exceptions.
        The expect failures could be false alarm, and test case might generate a lot of such failures.
        Therefore, we don't print the failure immediately and limit the max size of failure list.
        """
        self.expect_failures.insert(0, {"pattern": pattern, "data": data,
                                        "start": start_time, "end": time.time()})
        self.expect_failures = self.expect_failures[:self.MAX_EXPECT_FAILURES_TO_SAVED]

    def _save_dut_log(self, data):
        """
        Save DUT log into file using another thread.
        This is a workaround for some devices takes long time for file system operations.

        See descriptions in ``_LogThread`` for details.
        """
        self.LOG_THREAD.save_log(self.log_file, data)

    # define for methods need to be overwritten by Port
    @classmethod
    def list_available_ports(cls):
        """
        list all available ports.

        subclass (port) must overwrite this method.

        :return: list of available comports
        """
        pass

    def _port_open(self):
        """
        open the port.

        subclass (port) must overwrite this method.

        :return: None
        """
        pass

    def _port_read(self, size=1):
        """
        read form port. This method should not blocking for long time, otherwise receive thread can not exit.

        subclass (port) must overwrite this method.

        :param size: max size to read.
        :return: read data.
        """
        pass

    def _port_write(self, data):
        """
        write to port.

        subclass (port) must overwrite this method.

        :param data: data to write
        :return: None
        """
        pass

    def _port_close(self):
        """
        close port.

        subclass (port) must overwrite this method.

        :return: None
        """
        pass

    # methods that need to be overwritten by Tool
    @classmethod
    def confirm_dut(cls, port, **kwargs):
        """
        confirm if it's a DUT, usually used by auto detecting DUT in by Env config.

        subclass (tool) must overwrite this method.

        :param port: comport
        :return: tuple of result (bool), and target (str)
        """
        pass

    def start_app(self):
        """
        usually after we got DUT, we need to do some extra works to let App start.
        For example, we need to reset->download->reset to let IDF application start on DUT.

        subclass (tool) must overwrite this method.

        :return: None
        """
        pass

    # methods that features raw port methods
    def start_receive(self):
        """
        Start thread to receive data.

        :return: None
        """
        self.receive_thread = self.RECV_THREAD_CLS(self._port_read, self)
        self.receive_thread.start()

    def stop_receive(self):
        """
        stop the receiving thread for the port
        :return: None
        """
        if self.receive_thread:
            self.receive_thread.exit()
        self.LOG_THREAD.flush_data()
        self.receive_thread = None

    def close(self):
        """
        permanently close the port
        """
        self.stop_receive()
        self._port_close()

    @staticmethod
    def u_to_bytearray(data):
        """
        if data is not bytearray then it tries to convert it

        :param data: data which needs to be checked and maybe transformed
        """
        if isinstance(data, type(u'')):
            try:
                data = data.encode('utf-8')
            except Exception as e:
                print(u'Cannot encode {} of type {}'.format(data, type(data)))
                raise e
        return data

    def write(self, data, eol="\r\n", flush=True):
        """
        :param data: data
        :param eol: end of line pattern.
        :param flush: if need to flush received data cache before write data.
                      usually we need to flush data before write,
                      make sure processing outputs generated by wrote.
        :return: None
        """
        # do flush before write
        if flush:
            self.data_cache.flush()
        # do write if cache
        if data is not None:
            self._port_write(self.u_to_bytearray(data) + self.u_to_bytearray(eol) if eol else self.u_to_bytearray(data))

    @_expect_lock
    def read(self, size=0xFFFFFFFF):
        """
        read(size=0xFFFFFFFF)
        read raw data. NOT suggested to use this method.
        Only use it if expect method doesn't meet your requirement.

        :param size: read size. default read all data
        :return: read data
        """
        data = self.data_cache.get_data(0)[:size]
        self.data_cache.flush(size)
        return data

    def start_capture_raw_data(self, capture_id="default"):
        """
        Sometime application want to get DUT raw data and use ``expect`` method at the same time.
        Capture methods provides a way to get raw data without affecting ``expect`` or ``read`` method.

        If you call ``start_capture_raw_data`` with same capture id again, it will restart capture on this ID.

        :param capture_id: ID of capture. You can use different IDs to do different captures at the same time.
        """
        with self.record_data_lock:
            try:
                # if start capture on existed ID, we do flush data and restart capture
                self.recorded_data[capture_id].flush()
            except KeyError:
                # otherwise, create new data cache
                self.recorded_data[capture_id] = _DataCache()

    def stop_capture_raw_data(self, capture_id="default"):
        """
        Stop capture and get raw data.
        This method should be used after ``start_capture_raw_data`` on the same capture ID.

        :param capture_id: ID of capture.
        :return: captured raw data between start capture and stop capture.
        """
        with self.record_data_lock:
            try:
                ret = self.recorded_data[capture_id].get_data()
                self.recorded_data.pop(capture_id)
            except KeyError as e:
                e.message = "capture_id does not exist. " \
                            "You should call start_capture_raw_data with same ID " \
                            "before calling stop_capture_raw_data"
                raise e
        return ret

    # expect related methods

    @staticmethod
    def _expect_str(data, pattern):
        """
        protected method. check if string is matched in data cache.

        :param data: data to process
        :param pattern: string
        :return: pattern if match succeed otherwise None
        """
        index = data.find(pattern)
        if index != -1:
            ret = pattern
            index += len(pattern)
        else:
            ret = None
        return ret, index

    @staticmethod
    def _expect_re(data, pattern):
        """
        protected method. check if re pattern is matched in data cache

        :param data: data to process
        :param pattern: compiled RegEx pattern
        :return: match groups if match succeed otherwise None
        """
        ret = None
        if isinstance(pattern.pattern, bytes):
            pattern = re.compile(_decode_data(pattern.pattern))
        match = pattern.search(data)
        if match:
            ret = tuple(x for x in match.groups())
            index = match.end()
        else:
            index = -1
        return ret, index

    EXPECT_METHOD = [
        [type(re.compile("")), "_expect_re"],
        [type(b''), "_expect_str"],  # Python 2 & 3 hook to work without 'from builtins import str' from future
        [type(u''), "_expect_str"],
    ]

    def _get_expect_method(self, pattern):
        """
        protected method. get expect method according to pattern type.

        :param pattern: expect pattern, string or compiled RegEx
        :return: ``_expect_str`` or ``_expect_re``
        """
        for expect_method in self.EXPECT_METHOD:
            if isinstance(pattern, expect_method[0]):
                method = expect_method[1]
                break
        else:
            raise UnsupportedExpectItem()
        return self.__getattribute__(method)

    @_expect_lock
    def expect(self, pattern, timeout=DEFAULT_EXPECT_TIMEOUT):
        """
        expect(pattern, timeout=DEFAULT_EXPECT_TIMEOUT)
        expect received data on DUT match the pattern. will raise exception when expect timeout.

        :raise ExpectTimeout: failed to find the pattern before timeout
        :raise UnsupportedExpectItem: pattern is not string or compiled RegEx

        :param pattern: string or compiled RegEx(string pattern)
        :param timeout: timeout for expect
        :return: string if pattern is string; matched groups if pattern is RegEx
        """
        method = self._get_expect_method(pattern)

        # non-blocking get data for first time
        data = self.data_cache.get_data(0)
        start_time = time.time()
        while True:
            ret, index = method(data, pattern)
            if ret is not None:
                self.data_cache.flush(index)
                break
            time_remaining = start_time + timeout - time.time()
            if time_remaining < 0:
                break
            # wait for new data from cache
            data = self.data_cache.get_data(time_remaining)

        if ret is None:
            pattern = _pattern_to_string(pattern)
            self._save_expect_failure(pattern, data, start_time)
            raise ExpectTimeout(self.name + ": " + pattern)
        return ret

    def _expect_multi(self, expect_all, expect_item_list, timeout):
        """
        protected method. internal logical for expect multi.

        :param expect_all: True or False, expect all items in the list or any in the list
        :param expect_item_list: expect item list
        :param timeout: timeout
        :return: None
        """
        def process_expected_item(item_raw):
            # convert item raw data to standard dict
            item = {
                "pattern": item_raw[0] if isinstance(item_raw, tuple) else item_raw,
                "method": self._get_expect_method(item_raw[0] if isinstance(item_raw, tuple)
                                                  else item_raw),
                "callback": item_raw[1] if isinstance(item_raw, tuple) else None,
                "index": -1,
                "ret": None,
            }
            return item

        expect_items = [process_expected_item(x) for x in expect_item_list]

        # non-blocking get data for first time
        data = self.data_cache.get_data(0)

        start_time = time.time()
        matched_expect_items = list()
        while True:
            for expect_item in expect_items:
                if expect_item not in matched_expect_items:
                    # exclude those already matched
                    expect_item["ret"], expect_item["index"] = \
                        expect_item["method"](data, expect_item["pattern"])
                    if expect_item["ret"] is not None:
                        # match succeed for one item
                        matched_expect_items.append(expect_item)

            # if expect all, then all items need to be matched,
            # else only one item need to matched
            if expect_all:
                match_succeed = len(matched_expect_items) == len(expect_items)
            else:
                match_succeed = True if matched_expect_items else False

            time_remaining = start_time + timeout - time.time()
            if time_remaining < 0 or match_succeed:
                break
            else:
                data = self.data_cache.get_data(time_remaining)

        if match_succeed:
            # sort matched items according to order of appearance in the input data,
            # so that the callbacks are invoked in correct order
            matched_expect_items = sorted(matched_expect_items, key=lambda it: it["index"])
            # invoke callbacks and flush matched data cache
            slice_index = -1
            for expect_item in matched_expect_items:
                # trigger callback
                if expect_item["callback"]:
                    expect_item["callback"](expect_item["ret"])
                slice_index = max(slice_index, expect_item["index"])
            # flush already matched data
            self.data_cache.flush(slice_index)
        else:
            pattern = str([_pattern_to_string(x["pattern"]) for x in expect_items])
            self._save_expect_failure(pattern, data, start_time)
            raise ExpectTimeout(self.name + ": " + pattern)

    @_expect_lock
    def expect_any(self, *expect_items, **timeout):
        """
        expect_any(*expect_items, timeout=DEFAULT_TIMEOUT)
        expect any of the patterns.
        will call callback (if provided) if pattern match succeed and then return.
        will pass match result to the callback.

        :raise ExpectTimeout: failed to match any one of the expect items before timeout
        :raise UnsupportedExpectItem: pattern in expect_item is not string or compiled RegEx

        :arg expect_items: one or more expect items.
                           string, compiled RegEx pattern or (string or RegEx(string pattern), callback)
        :keyword timeout: timeout for expect
        :return: None
        """
        # to be compatible with python2
        # in python3 we can write f(self, *expect_items, timeout=DEFAULT_TIMEOUT)
        if "timeout" not in timeout:
            timeout["timeout"] = self.DEFAULT_EXPECT_TIMEOUT
        return self._expect_multi(False, expect_items, **timeout)

    @_expect_lock
    def expect_all(self, *expect_items, **timeout):
        """
        expect_all(*expect_items, timeout=DEFAULT_TIMEOUT)
        expect all of the patterns.
        will call callback (if provided) if all pattern match succeed and then return.
        will pass match result to the callback.

        :raise ExpectTimeout: failed to match all of the expect items before timeout
        :raise UnsupportedExpectItem: pattern in expect_item is not string or compiled RegEx

        :arg expect_items: one or more expect items.
                           string, compiled RegEx pattern or (string or RegEx(string pattern), callback)
        :keyword timeout: timeout for expect
        :return: None
        """
        # to be compatible with python2
        # in python3 we can write f(self, *expect_items, timeout=DEFAULT_TIMEOUT)
        if "timeout" not in timeout:
            timeout["timeout"] = self.DEFAULT_EXPECT_TIMEOUT
        return self._expect_multi(True, expect_items, **timeout)

    @staticmethod
    def _format_ts(ts):
        return "{}:{}".format(time.strftime("%m-%d %H:%M:%S", time.localtime(ts)), str(ts % 1)[2:5])

    def print_debug_info(self):
        """
        Print debug info of current DUT. Currently we will print debug info for expect failures.
        """
        Utility.console_log("DUT debug info for DUT: {}:".format(self.name), color="orange")

        for failure in self.expect_failures:
            Utility.console_log(u"\t[pattern]: {}\r\n\t[data]: {}\r\n\t[time]: {} - {}\r\n"
                                .format(failure["pattern"], failure["data"],
                                        self._format_ts(failure["start"]), self._format_ts(failure["end"])),
                                color="orange")


class SerialDUT(BaseDUT):
    """ serial with logging received data feature """

    DEFAULT_UART_CONFIG = {
        "baudrate": 115200,
        "bytesize": serial.EIGHTBITS,
        "parity": serial.PARITY_NONE,
        "stopbits": serial.STOPBITS_ONE,
        "timeout": 0.05,
        "xonxoff": False,
        "rtscts": False,
    }

    def __init__(self, name, port, log_file, app, **kwargs):
        self.port_inst = None
        self.serial_configs = self.DEFAULT_UART_CONFIG.copy()
        for uart_config_name in self.serial_configs.keys():
            if uart_config_name in kwargs:
                self.serial_configs[uart_config_name] = kwargs[uart_config_name]
        super(SerialDUT, self).__init__(name, port, log_file, app, **kwargs)

    def _format_data(self, data):
        """
        format data for logging. do decode and add timestamp.

        :param data: raw data from read
        :return: formatted data (str)
        """
        timestamp = "[{}]".format(self._format_ts(time.time()))
        formatted_data = timestamp.encode() + b"\r\n" + data + b"\r\n"
        return formatted_data

    def _port_open(self):
        self.port_inst = serial.serial_for_url(self.port, **self.serial_configs)

    def _port_close(self):
        self.port_inst.close()

    def _port_read(self, size=1):
        data = self.port_inst.read(size)
        if data:
            self._save_dut_log(self._format_data(data))
        return data

    def _port_write(self, data):
        if isinstance(data, str):
            data = data.encode()
        self.port_inst.write(data)

    @classmethod
    def list_available_ports(cls):
        return [x.device for x in list_ports.comports()]
