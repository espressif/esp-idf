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

import time
import re
import threading
import copy
import sys
import functools

import serial
from serial.tools import list_ports

if sys.version_info[0] == 2:
    import Queue as _queue
else:
    import queue as _queue


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


class _DataCache(_queue.Queue):
    """
    Data cache based on Queue. Allow users to process data cache based on bytes instead of Queue."
    """

    def __init__(self, maxsize=0):
        _queue.Queue.__init__(self, maxsize=maxsize)
        self.data_cache = str()

    def get_data(self, timeout=0):
        """
        get a copy of data from cache.

        :param timeout: timeout for waiting new queue item
        :return: copy of data cache
        """
        # make sure timeout is non-negative
        if timeout < 0:
            timeout = 0

        try:
            data = self.get(timeout=timeout)
            if isinstance(data, bytes):
                # convert bytes to string
                try:
                    data = data.decode("utf-8", "ignore")
                except UnicodeDecodeError:
                    data = data.decode("iso8859-1",)
            self.data_cache += data
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


class _RecvThread(threading.Thread):

    def __init__(self, read, data_cache):
        super(_RecvThread, self).__init__()
        self.exit_event = threading.Event()
        self.setDaemon(True)
        self.read = read
        self.data_cache = data_cache

    def run(self):
        while not self.exit_event.isSet():
            data = self.read(1000)
            if data:
                self.data_cache.put(data)

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

    DEFAULT_EXPECT_TIMEOUT = 5

    def __init__(self, name, port, log_file, app, **kwargs):

        self.expect_lock = threading.Lock()
        self.name = name
        self.port = port
        self.log_file = log_file
        self.app = app
        self.data_cache = _DataCache()
        self.receive_thread = None
        # open and start during init
        self.open()

    def __str__(self):
        return "DUT({}: {})".format(self.name, str(self.port))

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
    def confirm_dut(cls, port, app, **kwargs):
        """
        confirm if it's a DUT, usually used by auto detecting DUT in by Env config.

        subclass (tool) must overwrite this method.

        :param port: comport
        :param app: app instance
        :return: True or False
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
    def open(self):
        """
        open port and create thread to receive data.

        :return: None
        """
        self._port_open()
        self.receive_thread = _RecvThread(self._port_read, self.data_cache)
        self.receive_thread.start()

    def close(self):
        """
        close receive thread and then close port.

        :return: None
        """
        if self.receive_thread:
            self.receive_thread.exit()
        self._port_close()

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
        if data:
            self._port_write(data + eol if eol else data)

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
        match = pattern.search(data)
        if match:
            ret = match.groups()
            index = match.end()
        else:
            index = -1
        return ret, index

    EXPECT_METHOD = [
        [type(re.compile("")), "_expect_re"],
        [str, "_expect_str"],
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
            if ret is not None or time.time() - start_time > timeout:
                self.data_cache.flush(index)
                break
            # wait for new data from cache
            data = self.data_cache.get_data(time.time() + timeout - start_time)

        if ret is None:
            raise ExpectTimeout(self.name + ": " + str(pattern))
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
                        break

            # if expect all, then all items need to be matched,
            # else only one item need to matched
            if expect_all:
                match_succeed = (matched_expect_items == expect_items)
            else:
                match_succeed = True if matched_expect_items else False

            if time.time() - start_time > timeout or match_succeed:
                break
            else:
                data = self.data_cache.get_data(time.time() + timeout - start_time)

        if match_succeed:
            # do callback and flush matched data cache
            slice_index = -1
            for expect_item in matched_expect_items:
                # trigger callback
                if expect_item["callback"]:
                    expect_item["callback"](expect_item["ret"])
                slice_index = max(slice_index, expect_item["index"])
            # flush already matched data
            self.data_cache.flush(slice_index)
        else:
            raise ExpectTimeout(self.name + ": " + str(expect_items))

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
        self.serial_configs.update(kwargs)
        super(SerialDUT, self).__init__(name, port, log_file, app, **kwargs)

    @staticmethod
    def _format_data(data):
        """
        format data for logging. do decode and add timestamp.

        :param data: raw data from read
        :return: formatted data (str)
        """
        timestamp = time.time()
        timestamp = "{}:{}".format(time.strftime("%m-%d %H:%M:%S", time.localtime(timestamp)),
                                   str(timestamp % 1)[2:5])
        try:
            formatted_data = "[{}]:\r\n{}\r\n".format(timestamp, data.decode("utf-8", "ignore"))
        except UnicodeDecodeError:
            # if utf-8 fail, use iso-8859-1 (single char codec with range 0-255)
            formatted_data = "[{}]:\r\n{}\r\n".format(timestamp, data.decode("iso8859-1",))
        return formatted_data

    def _port_open(self):
        self.port_inst = serial.Serial(self.port, **self.serial_configs)

    def _port_close(self):
        self.port_inst.close()

    def _port_read(self, size=1):
        data = self.port_inst.read(size)
        if data:
            with open(self.log_file, "a+") as _log_file:
                _log_file.write(self._format_data(data))
        return data

    def _port_write(self, data):
        self.port_inst.write(data)

    @classmethod
    def list_available_ports(cls):
        return [x.device for x in list_ports.comports()]
