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

""" DUT for IDF applications """
import os
import os.path
import sys
import re
import functools
import tempfile
import subprocess

# python2 and python3 queue package name is different
try:
    import Queue as _queue
except ImportError:
    import queue as _queue


from serial.tools import list_ports

import DUT
import Utility

try:
    import esptool
except ImportError:  # cheat and use IDF's copy of esptool if available
    idf_path = os.getenv("IDF_PATH")
    if not idf_path or not os.path.exists(idf_path):
        raise
    sys.path.insert(0, os.path.join(idf_path, "components", "esptool_py", "esptool"))
    import esptool


class IDFToolError(OSError):
    pass


class IDFDUTException(RuntimeError):
    pass


class IDFRecvThread(DUT.RecvThread):

    PERFORMANCE_PATTERN = re.compile(r"\[Performance]\[(\w+)]: ([^\r\n]+)\r?\n")
    EXCEPTION_PATTERNS = [
        re.compile(r"(Guru Meditation Error: Core\s+\d panic'ed \([\w].*?\))"),
        re.compile(r"(abort\(\) was called at PC 0x[a-fA-F\d]{8} on core \d)"),
        re.compile(r"(rst 0x\d+ \(TG\dWDT_SYS_RESET|TGWDT_CPU_RESET\))")
    ]
    BACKTRACE_PATTERN = re.compile(r"Backtrace:((\s(0x[0-9a-f]{8}):0x[0-9a-f]{8})+)")
    BACKTRACE_ADDRESS_PATTERN = re.compile(r"(0x[0-9a-f]{8}):0x[0-9a-f]{8}")

    def __init__(self, read, dut):
        super(IDFRecvThread, self).__init__(read, dut)
        self.exceptions = _queue.Queue()
        self.performance_items = _queue.Queue()

    def collect_performance(self, comp_data):
        matches = self.PERFORMANCE_PATTERN.findall(comp_data)
        for match in matches:
            Utility.console_log("[Performance][{}]: {}".format(match[0], match[1]),
                                color="orange")
            self.performance_items.put((match[0], match[1]))

    def detect_exception(self, comp_data):
        for pattern in self.EXCEPTION_PATTERNS:
            start = 0
            while True:
                match = pattern.search(comp_data, pos=start)
                if match:
                    start = match.end()
                    self.exceptions.put(match.group(0))
                    Utility.console_log("[Exception]: {}".format(match.group(0)), color="red")
                else:
                    break

    def detect_backtrace(self, comp_data):
        start = 0
        while True:
            match = self.BACKTRACE_PATTERN.search(comp_data, pos=start)
            if match:
                start = match.end()
                Utility.console_log("[Backtrace]:{}".format(match.group(1)), color="red")
                # translate backtrace
                addresses = self.BACKTRACE_ADDRESS_PATTERN.findall(match.group(1))
                translated_backtrace = ""
                for addr in addresses:
                    ret = self.dut.lookup_pc_address(addr)
                    if ret:
                        translated_backtrace += ret + "\n"
                if translated_backtrace:
                    Utility.console_log("Translated backtrace\n:" + translated_backtrace, color="yellow")
                else:
                    Utility.console_log("Failed to translate backtrace", color="yellow")
            else:
                break

    CHECK_FUNCTIONS = [collect_performance, detect_exception, detect_backtrace]


def _uses_esptool(func):
    """ Suspend listener thread, connect with esptool,
    call target function with esptool instance,
    then resume listening for output
    """
    @functools.wraps(func)
    def handler(self, *args, **kwargs):
        self.stop_receive()

        settings = self.port_inst.get_settings()

        try:
            if not self._rom_inst:
                self._rom_inst = esptool.ESPLoader.detect_chip(self.port_inst)
            self._rom_inst.connect('hard_reset')
            esp = self._rom_inst.run_stub()

            ret = func(self, esp, *args, **kwargs)
            # do hard reset after use esptool
            esp.hard_reset()
        finally:
            # always need to restore port settings
            self.port_inst.apply_settings(settings)

        self.start_receive()

        return ret
    return handler


class IDFDUT(DUT.SerialDUT):
    """ IDF DUT, extends serial with esptool methods

    (Becomes aware of IDFApp instance which holds app-specific data)
    """

    # /dev/ttyAMA0 port is listed in Raspberry Pi
    # /dev/tty.Bluetooth-Incoming-Port port is listed in Mac
    INVALID_PORT_PATTERN = re.compile(r"AMA|Bluetooth")
    # if need to erase NVS partition in start app
    ERASE_NVS = True
    RECV_THREAD_CLS = IDFRecvThread
    TOOLCHAIN_PREFIX = "xtensa-esp32-elf-"

    def __init__(self, name, port, log_file, app, allow_dut_exception=False, **kwargs):
        super(IDFDUT, self).__init__(name, port, log_file, app, **kwargs)
        self.allow_dut_exception = allow_dut_exception
        self.exceptions = _queue.Queue()
        self.performance_items = _queue.Queue()
        self._rom_inst = None

    @classmethod
    def _get_rom(cls):
        raise NotImplementedError("This is an abstraction class, method not defined.")

    @classmethod
    def get_mac(cls, app, port):
        """
        get MAC address via esptool

        :param app: application instance (to get tool)
        :param port: serial port as string
        :return: MAC address or None
        """
        try:
            esp = cls._get_rom()(port)
            esp.connect()
            return esp.read_mac()
        except RuntimeError:
            return None
        finally:
            # do hard reset after use esptool
            esp.hard_reset()
            esp._port.close()

    @classmethod
    def confirm_dut(cls, port, app, **kwargs):
        inst = None
        try:
            expected_rom_class = cls._get_rom()
        except NotImplementedError:
            expected_rom_class = None

        try:
            # TODO: check whether 8266 works with this logic
            # Otherwise overwrite it in ESP8266DUT
            inst = esptool.ESPLoader.detect_chip(port)
            if expected_rom_class and type(inst) != expected_rom_class:
                raise RuntimeError("Target not expected")
            return inst.read_mac() is not None
        except(esptool.FatalError, RuntimeError):
            return False
        finally:
            if inst is not None:
                inst._port.close()

    @_uses_esptool
    def _try_flash(self, esp, erase_nvs, baud_rate):
        """
        Called by start_app() to try flashing at a particular baud rate.

        Structured this way so @_uses_esptool will reconnect each time
        """
        try:
            # note: opening here prevents us from having to seek back to 0 each time
            flash_files = [(offs, open(path, "rb")) for (offs, path) in self.app.flash_files]

            if erase_nvs:
                address = self.app.partition_table["nvs"]["offset"]
                size = self.app.partition_table["nvs"]["size"]
                nvs_file = tempfile.TemporaryFile()
                nvs_file.write(b'\xff' * size)
                nvs_file.seek(0)
                flash_files.append((int(address, 0), nvs_file))

            # fake flasher args object, this is a hack until
            # esptool Python API is improved
            class FlashArgs(object):
                def __init__(self, attributes):
                    for key, value in attributes.items():
                        self.__setattr__(key, value)

            flash_args = FlashArgs({
                'flash_size': self.app.flash_settings["flash_size"],
                'flash_mode': self.app.flash_settings["flash_mode"],
                'flash_freq': self.app.flash_settings["flash_freq"],
                'addr_filename': flash_files,
                'no_stub': False,
                'compress': True,
                'verify': False,
                'encrypt': self.app.flash_settings.get("encrypt", False),
                'erase_all': False,
            })

            esp.change_baud(baud_rate)
            esptool.detect_flash_size(esp, flash_args)
            esptool.write_flash(esp, flash_args)
        finally:
            for (_, f) in flash_files:
                f.close()

    def start_app(self, erase_nvs=ERASE_NVS):
        """
        download and start app.

        :param: erase_nvs: whether erase NVS partition during flash
        :return: None
        """
        for baud_rate in [921600, 115200]:
            try:
                self._try_flash(erase_nvs, baud_rate)
                break
            except RuntimeError:
                continue
        else:
            raise IDFToolError()

    @_uses_esptool
    def reset(self, esp):
        """
        hard reset DUT

        :return: None
        """
        # decorator `_use_esptool` will do reset
        # so we don't need to do anything in this method
        pass

    @_uses_esptool
    def erase_partition(self, esp, partition):
        """
        :param partition: partition name to erase
        :return: None
        """
        raise NotImplementedError()  # TODO: implement this
        # address = self.app.partition_table[partition]["offset"]
        size = self.app.partition_table[partition]["size"]
        # TODO can use esp.erase_region() instead of this, I think
        with open(".erase_partition.tmp", "wb") as f:
            f.write(chr(0xFF) * size)

    @_uses_esptool
    def dump_flush(self, esp, output_file, **kwargs):
        """
        dump flush

        :param output_file: output file name, if relative path, will use sdk path as base path.
        :keyword partition: partition name, dump the partition.
                            ``partition`` is preferred than using ``address`` and ``size``.
        :keyword address: dump from address (need to be used with size)
        :keyword size: dump size (need to be used with address)
        :return: None
        """
        if os.path.isabs(output_file) is False:
            output_file = os.path.relpath(output_file, self.app.get_log_folder())
        if "partition" in kwargs:
            partition = self.app.partition_table[kwargs["partition"]]
            _address = partition["offset"]
            _size = partition["size"]
        elif "address" in kwargs and "size" in kwargs:
            _address = kwargs["address"]
            _size = kwargs["size"]
        else:
            raise IDFToolError("You must specify 'partition' or ('address' and 'size') to dump flash")

        content = esp.read_flash(_address, _size)
        with open(output_file, "wb") as f:
            f.write(content)

    @classmethod
    def list_available_ports(cls):
        ports = [x.device for x in list_ports.comports()]
        espport = os.getenv('ESPPORT')
        if not espport:
            # It's a little hard filter out invalid port with `serial.tools.list_ports.grep()`:
            # The check condition in `grep` is: `if r.search(port) or r.search(desc) or r.search(hwid)`.
            # This means we need to make all 3 conditions fail, to filter out the port.
            # So some part of the filters will not be straight forward to users.
            # And negative regular expression (`^((?!aa|bb|cc).)*$`) is not easy to understand.
            # Filter out invalid port by our own will be much simpler.
            return [x for x in ports if not cls.INVALID_PORT_PATTERN.search(x)]

        # On MacOs with python3.6: type of espport is already utf8
        if isinstance(espport, type(u'')):
            port_hint = espport
        else:
            port_hint = espport.decode('utf8')

        # If $ESPPORT is a valid port, make it appear first in the list
        if port_hint in ports:
            ports.remove(port_hint)
            return [port_hint] + ports

        # On macOS, user may set ESPPORT to /dev/tty.xxx while
        # pySerial lists only the corresponding /dev/cu.xxx port
        if sys.platform == 'darwin' and 'tty.' in port_hint:
            port_hint = port_hint.replace('tty.', 'cu.')
            if port_hint in ports:
                ports.remove(port_hint)
                return [port_hint] + ports

        return ports

    def lookup_pc_address(self, pc_addr):
        cmd = ["%saddr2line" % self.TOOLCHAIN_PREFIX,
               "-pfiaC", "-e", self.app.elf_file, pc_addr]
        ret = ""
        try:
            translation = subprocess.check_output(cmd)
            ret = translation.decode()
        except OSError:
            pass
        return ret

    @staticmethod
    def _queue_read_all(source_queue):
        output = []
        while True:
            try:
                output.append(source_queue.get(timeout=0))
            except _queue.Empty:
                break
        return output

    def _queue_copy(self, source_queue, dest_queue):
        data = self._queue_read_all(source_queue)
        for d in data:
            dest_queue.put(d)

    def _get_from_queue(self, queue_name):
        self_queue = getattr(self, queue_name)
        if self.receive_thread:
            recv_thread_queue = getattr(self.receive_thread, queue_name)
            self._queue_copy(recv_thread_queue, self_queue)
        return self._queue_read_all(self_queue)

    def stop_receive(self):
        if self.receive_thread:
            for name in ["performance_items", "exceptions"]:
                source_queue = getattr(self.receive_thread, name)
                dest_queue = getattr(self, name)
                self._queue_copy(source_queue, dest_queue)
        super(IDFDUT, self).stop_receive()

    def get_exceptions(self):
        """ Get exceptions detected by DUT receive thread. """
        return self._get_from_queue("exceptions")

    def get_performance_items(self):
        """
        DUT receive thread will automatic collect performance results with pattern ``[Performance][name]: value\n``.
        This method is used to get all performance results.

        :return: a list of performance items.
        """
        return self._get_from_queue("performance_items")

    def close(self):
        super(IDFDUT, self).close()
        if not self.allow_dut_exception and self.get_exceptions():
            Utility.console_log("DUT exception detected on {}".format(self), color="red")
            raise IDFDUTException()


class ESP32DUT(IDFDUT):
    @classmethod
    def _get_rom(cls):
        return esptool.ESP32ROM


class ESP32S2DUT(IDFDUT):
    @classmethod
    def _get_rom(cls):
        return esptool.ESP32S2ROM


class ESP8266DUT(IDFDUT):
    @classmethod
    def _get_rom(cls):
        return esptool.ESP8266ROM
