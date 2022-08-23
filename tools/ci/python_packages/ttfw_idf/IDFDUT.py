# SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

""" DUT for IDF applications """
import collections
import functools
import io
import os
import os.path
import re
import subprocess
import sys
import tempfile
import time

import pexpect
import serial

# python2 and python3 queue package name is different
try:
    import Queue as _queue
except ImportError:
    import queue as _queue  # type: ignore

from serial.tools import list_ports
from tiny_test_fw import DUT, Utility

try:
    import esptool
except ImportError:  # cheat and use IDF's copy of esptool if available
    idf_path = os.getenv('IDF_PATH')
    if not idf_path or not os.path.exists(idf_path):
        raise
    sys.path.insert(0, os.path.join(idf_path, 'components', 'esptool_py', 'esptool'))
    import esptool

try:
    # esptool>=4.0
    detect_chip = esptool.cmds.detect_chip
    FatalError = esptool.util.FatalError
    targets = esptool.targets
except (AttributeError, ModuleNotFoundError):
    # esptool<4.0
    detect_chip = esptool.ESPLoader.detect_chip
    FatalError = esptool.FatalError
    targets = esptool

import espefuse
import espsecure


class IDFToolError(OSError):
    pass


class IDFDUTException(RuntimeError):
    pass


class IDFRecvThread(DUT.RecvThread):

    PERFORMANCE_PATTERN = re.compile(r'\[Performance]\[(\w+)]: ([^\r\n]+)\r?\n')
    EXCEPTION_PATTERNS = [
        re.compile(r"(Guru Meditation Error: Core\s+\d panic'ed \([\w].*?\))"),
        re.compile(r'(abort\(\) was called at PC 0x[a-fA-F\d]{8} on core \d)'),
        re.compile(r'(rst 0x\d+ \(TG\dWDT_SYS_RESET|TGWDT_CPU_RESET\))')
    ]
    BACKTRACE_PATTERN = re.compile(r'Backtrace:((\s(0x[0-9a-f]{8}):0x[0-9a-f]{8})+)')
    BACKTRACE_ADDRESS_PATTERN = re.compile(r'(0x[0-9a-f]{8}):0x[0-9a-f]{8}')

    def __init__(self, read, dut):
        super(IDFRecvThread, self).__init__(read, dut)
        self.exceptions = _queue.Queue()
        self.performance_items = _queue.Queue()

    def collect_performance(self, comp_data):
        matches = self.PERFORMANCE_PATTERN.findall(comp_data)
        for match in matches:
            Utility.console_log('[Performance][{}]: {}'.format(match[0], match[1]), color='orange')
            self.performance_items.put((match[0], match[1]))

    def detect_exception(self, comp_data):
        for pattern in self.EXCEPTION_PATTERNS:
            start = 0
            while True:
                match = pattern.search(comp_data, pos=start)
                if match:
                    start = match.end()
                    self.exceptions.put(match.group(0))
                    Utility.console_log('[Exception]: {}'.format(match.group(0)), color='red')
                else:
                    break

    def detect_backtrace(self, comp_data):
        start = 0
        while True:
            match = self.BACKTRACE_PATTERN.search(comp_data, pos=start)
            if match:
                start = match.end()
                Utility.console_log('[Backtrace]:{}'.format(match.group(1)), color='red')
                # translate backtrace
                addresses = self.BACKTRACE_ADDRESS_PATTERN.findall(match.group(1))
                translated_backtrace = ''
                for addr in addresses:
                    ret = self.dut.lookup_pc_address(addr)
                    if ret:
                        translated_backtrace += ret + '\n'
                if translated_backtrace:
                    Utility.console_log('Translated backtrace\n:' + translated_backtrace, color='yellow')
                else:
                    Utility.console_log('Failed to translate backtrace', color='yellow')
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
            if not self.rom_inst:
                if not self.secure_boot_en:
                    self.rom_inst = detect_chip(self.port_inst)
                else:
                    self.rom_inst = self.get_rom()(self.port_inst)
            self.rom_inst.connect('hard_reset')

            if (self.secure_boot_en):
                esp = self.rom_inst
                esp.flash_spi_attach(0)
            else:
                esp = self.rom_inst.run_stub()

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
    INVALID_PORT_PATTERN = re.compile(r'AMA|Bluetooth')
    # if need to erase NVS partition in start app
    ERASE_NVS = True
    RECV_THREAD_CLS = IDFRecvThread

    def __init__(self, name, port, log_file, app, allow_dut_exception=False, **kwargs):
        super(IDFDUT, self).__init__(name, port, log_file, app, **kwargs)
        self.allow_dut_exception = allow_dut_exception
        self.exceptions = _queue.Queue()
        self.performance_items = _queue.Queue()
        self.rom_inst = None
        self.secure_boot_en = self.app.get_sdkconfig_config_value('CONFIG_SECURE_BOOT') and \
            not self.app.get_sdkconfig_config_value('CONFIG_EFUSE_VIRTUAL')

    @classmethod
    def get_rom(cls):
        raise NotImplementedError('This is an abstraction class, method not defined.')

    @classmethod
    def get_mac(cls, app, port):
        """
        get MAC address via esptool

        :param app: application instance (to get tool)
        :param port: serial port as string
        :return: MAC address or None
        """
        esp = None
        try:
            esp = cls.get_rom()(port)
            esp.connect()
            return esp.read_mac()
        except RuntimeError:
            return None
        finally:
            if esp:
                # do hard reset after use esptool
                esp.hard_reset()
                esp._port.close()

    @classmethod
    def confirm_dut(cls, port, **kwargs):
        inst = None
        try:
            expected_rom_class = cls.get_rom()
        except NotImplementedError:
            expected_rom_class = None

        try:
            # TODO: check whether 8266 works with this logic
            # Otherwise overwrite it in ESP8266DUT
            inst = detect_chip(port)
            if expected_rom_class and type(inst) != expected_rom_class:
                raise RuntimeError('Target not expected')
            return inst.read_mac() is not None, get_target_by_rom_class(type(inst))
        except (FatalError, RuntimeError):
            return False, None
        finally:
            if inst is not None:
                inst._port.close()

    def _try_flash(self, erase_nvs):
        """
        Called by start_app()

        :return: None
        """
        flash_files = []
        encrypt_files = []
        try:
            # Open the files here to prevents us from having to seek back to 0
            # each time. Before opening them, we have to organize the lists the
            # way esptool.write_flash needs:
            # If encrypt is provided, flash_files contains all the files to
            # flash.
            # Else, flash_files contains the files to be flashed as plain text
            # and encrypt_files contains the ones to flash encrypted.
            flash_files = self.app.flash_files
            encrypt_files = self.app.encrypt_files
            encrypt = self.app.flash_settings.get('encrypt', False)
            if encrypt:
                flash_files = encrypt_files
                encrypt_files = []
            else:
                flash_files = [entry
                               for entry in flash_files
                               if entry not in encrypt_files]

            flash_files = [(offs, open(path, 'rb')) for (offs, path) in flash_files]
            encrypt_files = [(offs, open(path, 'rb')) for (offs, path) in encrypt_files]

            if erase_nvs:
                address = self.app.partition_table['nvs']['offset']
                size = self.app.partition_table['nvs']['size']
                nvs_file = tempfile.TemporaryFile()
                nvs_file.write(b'\xff' * size)
                nvs_file.seek(0)
                if not isinstance(address, int):
                    address = int(address, 0)
                # We have to check whether this file needs to be added to
                # flash_files list or encrypt_files.
                # Get the CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT macro
                # value. If it is set to True, then NVS is always encrypted.
                sdkconfig_dict = self.app.get_sdkconfig()
                macro_encryption = 'CONFIG_SECURE_FLASH_ENCRYPTION_MODE_DEVELOPMENT' in sdkconfig_dict
                # If the macro is not enabled (plain text flash) or all files
                # must be encrypted, add NVS to flash_files.
                if not macro_encryption or encrypt:
                    flash_files.append((address, nvs_file))
                else:
                    encrypt_files.append((address, nvs_file))

            self.write_flash_data(flash_files, encrypt_files, False, encrypt)
        finally:
            for (_, f) in flash_files:
                f.close()
            for (_, f) in encrypt_files:
                f.close()

    @_uses_esptool
    def write_flash_data(self, esp, flash_files=None, encrypt_files=None, ignore_flash_encryption_efuse_setting=True, encrypt=False):
        """
        Try flashing at a particular baud rate.

        Structured this way so @_uses_esptool will reconnect each time
        :return: None
        """
        last_error = None
        for baud_rate in [921600, 115200]:
            try:
                # fake flasher args object, this is a hack until
                # esptool Python API is improved
                class FlashArgs(object):
                    def __init__(self, attributes):
                        for key, value in attributes.items():
                            self.__setattr__(key, value)

                # write_flash expects the parameter encrypt_files to be None and not
                # an empty list, so perform the check here
                flash_args = FlashArgs({
                    'flash_size': self.app.flash_settings['flash_size'],
                    'flash_mode': self.app.flash_settings['flash_mode'],
                    'flash_freq': self.app.flash_settings['flash_freq'],
                    'addr_filename': flash_files or None,
                    'encrypt_files': encrypt_files or None,
                    'no_stub': self.secure_boot_en,
                    'compress': not self.secure_boot_en,
                    'verify': False,
                    'encrypt': encrypt,
                    'ignore_flash_encryption_efuse_setting': ignore_flash_encryption_efuse_setting,
                    'erase_all': False,
                    'after': 'no_reset',
                    'force': False,
                    'chip': esp.CHIP_NAME.lower().replace('-', ''),
                })

                esp.change_baud(baud_rate)
                esptool.detect_flash_size(esp, flash_args)
                esptool.write_flash(esp, flash_args)
                break
            except RuntimeError as e:
                last_error = e
        else:
            raise last_error

    def image_info(self, path_to_file):
        """
        get hash256 of app

        :param: path: path to file
        :return: sha256 appended to app
        """

        old_stdout = sys.stdout
        new_stdout = io.StringIO()
        sys.stdout = new_stdout

        class Args(object):
            def __init__(self, attributes):
                for key, value in attributes.items():
                    self.__setattr__(key, value)

        args = Args({
            'chip': self.TARGET,
            'filename': path_to_file,
        })
        esptool.image_info(args)
        output = new_stdout.getvalue()
        sys.stdout = old_stdout
        return output

    def start_app(self, erase_nvs=ERASE_NVS):
        """
        download and start app.

        :param: erase_nvs: whether erase NVS partition during flash
        :return: None
        """
        self._try_flash(erase_nvs)

    def start_app_no_enc(self):
        """
        download and start app.

        :param: erase_nvs: whether erase NVS partition during flash
        :return: None
        """
        flash_files = self.app.flash_files + self.app.encrypt_files
        self.write_flash(flash_files)

    def write_flash(self, flash_files=None, encrypt_files=None, ignore_flash_encryption_efuse_setting=True, encrypt=False):
        """
        Flash files

        :return: None
        """
        flash_offs_files = []
        encrypt_offs_files = []
        try:
            if flash_files:
                flash_offs_files = [(offs, open(path, 'rb')) for (offs, path) in flash_files]

            if encrypt_files:
                encrypt_offs_files = [(offs, open(path, 'rb')) for (offs, path) in encrypt_files]

            self.write_flash_data(flash_offs_files, encrypt_offs_files, ignore_flash_encryption_efuse_setting, encrypt)
        finally:
            for (_, f) in flash_offs_files:
                f.close()
            for (_, f) in encrypt_offs_files:
                f.close()

    def bootloader_flash(self):
        """
        download bootloader.

        :return: None
        """
        bootloader_path = os.path.join(self.app.binary_path, 'bootloader', 'bootloader.bin')
        offs = int(self.app.get_sdkconfig()['CONFIG_BOOTLOADER_OFFSET_IN_FLASH'], 0)
        flash_files = [(offs, bootloader_path)]
        self.write_flash(flash_files)

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
        address = self.app.partition_table[partition]['offset']
        size = self.app.partition_table[partition]['size']
        esp.erase_region(address, size)

    @_uses_esptool
    def erase_flash(self, esp):
        """
        erase the flash completely

        :return: None
        """
        esp.erase_flash()

    @_uses_esptool
    def dump_flash(self, esp, output_file, **kwargs):
        """
        dump flash

        :param output_file: output file name, if relative path, will use sdk path as base path.
        :keyword partition: partition name, dump the partition.
                            ``partition`` is preferred than using ``address`` and ``size``.
        :keyword address: dump from address (need to be used with size)
        :keyword size: dump size (need to be used with address)
        :return: None
        """
        if os.path.isabs(output_file) is False:
            output_file = os.path.relpath(output_file, self.app.get_log_folder())
        if 'partition' in kwargs:
            partition = self.app.partition_table[kwargs['partition']]
            _address = partition['offset']
            _size = partition['size']
        elif 'address' in kwargs and 'size' in kwargs:
            _address = kwargs['address']
            _size = kwargs['size']
        else:
            raise IDFToolError("You must specify 'partition' or ('address' and 'size') to dump flash")

        content = esp.read_flash(_address, _size)
        with open(output_file, 'wb') as f:
            f.write(content)

    @staticmethod
    def _sort_usb_ports(ports):
        """
        Move the usb ports to the very beginning
        :param ports: list of ports
        :return: list of ports with usb ports at beginning
        """
        usb_ports = []
        rest_ports = []
        for port in ports:
            if 'usb' in port.lower():
                usb_ports.append(port)
            else:
                rest_ports.append(port)
        return usb_ports + rest_ports

    @classmethod
    def list_available_ports(cls):
        # It will return other kinds of ports as well, such as ttyS* ports.
        # Give the usb ports higher priority
        ports = cls._sort_usb_ports([x.device for x in list_ports.comports()])
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
        cmd = ['%saddr2line' % self.TOOLCHAIN_PREFIX,
               '-pfiaC', '-e', self.app.elf_file, pc_addr]
        ret = ''
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
            for name in ['performance_items', 'exceptions']:
                source_queue = getattr(self.receive_thread, name)
                dest_queue = getattr(self, name)
                self._queue_copy(source_queue, dest_queue)
        super(IDFDUT, self).stop_receive()

    def get_exceptions(self):
        """ Get exceptions detected by DUT receive thread. """
        return self._get_from_queue('exceptions')

    def get_performance_items(self):
        """
        DUT receive thread will automatic collect performance results with pattern ``[Performance][name]: value\n``.
        This method is used to get all performance results.

        :return: a list of performance items.
        """
        return self._get_from_queue('performance_items')

    def close(self):
        super(IDFDUT, self).close()
        if not self.allow_dut_exception and self.get_exceptions():
            raise IDFDUTException('DUT exception detected on {}'.format(self))


class ESP32DUT(IDFDUT):
    TARGET = 'esp32'
    TOOLCHAIN_PREFIX = 'xtensa-esp32-elf-'

    @classmethod
    def get_rom(cls):
        return targets.ESP32ROM


class ESP32S2DUT(IDFDUT):
    TARGET = 'esp32s2'
    TOOLCHAIN_PREFIX = 'xtensa-esp32s2-elf-'

    @classmethod
    def get_rom(cls):
        return targets.ESP32S2ROM


class ESP32S3DUT(IDFDUT):
    TARGET = 'esp32s3'
    TOOLCHAIN_PREFIX = 'xtensa-esp32s3-elf-'

    @classmethod
    def get_rom(cls):
        return targets.ESP32S3ROM

    def erase_partition(self, esp, partition):
        raise NotImplementedError()


class ESP32C2DUT(IDFDUT):
    TARGET = 'esp32c2'
    TOOLCHAIN_PREFIX = 'riscv32-esp-elf-'

    @classmethod
    def get_rom(cls):
        return targets.ESP32C2ROM


class ESP32C3DUT(IDFDUT):
    TARGET = 'esp32c3'
    TOOLCHAIN_PREFIX = 'riscv32-esp-elf-'

    @classmethod
    def get_rom(cls):
        return targets.ESP32C3ROM


class ESP32C6DUT(IDFDUT):
    TARGET = 'esp32c6'
    TOOLCHAIN_PREFIX = 'riscv32-esp-elf-'

    @classmethod
    def get_rom(cls):
        return targets.ESP32C6BETAROM


class ESP32H2DUT(IDFDUT):
    TARGET = 'esp32h2'
    TOOLCHAIN_PREFIX = 'riscv32-esp-elf-'

    @classmethod
    def get_rom(cls):
        return targets.ESP32H2ROM


class ESP8266DUT(IDFDUT):
    TARGET = 'esp8266'
    TOOLCHAIN_PREFIX = 'xtensa-lx106-elf-'

    @classmethod
    def get_rom(cls):
        return targets.ESP8266ROM


def get_target_by_rom_class(cls):
    for c in [ESP32DUT, ESP32S2DUT, ESP32S3DUT, ESP32C2DUT, ESP32C3DUT, ESP32C6DUT, ESP32H2DUT, ESP8266DUT, IDFQEMUDUT]:
        if c.get_rom() == cls:
            return c.TARGET
    return None


class IDFQEMUDUT(IDFDUT):
    TARGET = None
    TOOLCHAIN_PREFIX = None
    ERASE_NVS = True
    DEFAULT_EXPECT_TIMEOUT = 30  # longer timeout, since app startup takes more time in QEMU (due to slow SHA emulation)
    QEMU_SERIAL_PORT = 3334

    def __init__(self, name, port, log_file, app, allow_dut_exception=False, **kwargs):
        self.flash_image = tempfile.NamedTemporaryFile('rb+', suffix='.bin', prefix='qemu_flash_img')
        self.app = app
        self.flash_size = 4 * 1024 * 1024
        self._write_flash_img()

        args = [
            'qemu-system-xtensa',
            '-nographic',
            '-machine', self.TARGET,
            '-drive', 'file={},if=mtd,format=raw'.format(self.flash_image.name),
            '-nic', 'user,model=open_eth',
            '-serial', 'tcp::{},server,nowait'.format(self.QEMU_SERIAL_PORT),
            '-S',
            '-global driver=timer.esp32.timg,property=wdt_disable,value=true']
        # TODO(IDF-1242): generate a temporary efuse binary, pass it to QEMU

        if 'QEMU_BIOS_PATH' in os.environ:
            args += ['-L', os.environ['QEMU_BIOS_PATH']]

        self.qemu = pexpect.spawn(' '.join(args), timeout=self.DEFAULT_EXPECT_TIMEOUT)
        self.qemu.expect_exact(b'(qemu)')
        super(IDFQEMUDUT, self).__init__(name, port, log_file, app, allow_dut_exception=allow_dut_exception, **kwargs)

    def _write_flash_img(self):
        self.flash_image.seek(0)
        self.flash_image.write(b'\x00' * self.flash_size)
        for offs, path in self.app.flash_files:
            with open(path, 'rb') as flash_file:
                contents = flash_file.read()
                self.flash_image.seek(offs)
                self.flash_image.write(contents)
        self.flash_image.flush()

    @classmethod
    def get_rom(cls):
        return targets.ESP32ROM

    @classmethod
    def get_mac(cls, app, port):
        # TODO(IDF-1242): get this from QEMU/efuse binary
        return '11:22:33:44:55:66'

    @classmethod
    def confirm_dut(cls, port, **kwargs):
        return True, cls.TARGET

    def start_app(self, erase_nvs=ERASE_NVS):
        # TODO: implement erase_nvs
        # since the flash image is generated every time in the constructor, maybe this isn't needed...
        self.qemu.sendline(b'cont\n')
        self.qemu.expect_exact(b'(qemu)')

    def reset(self):
        self.qemu.sendline(b'system_reset\n')
        self.qemu.expect_exact(b'(qemu)')

    def erase_partition(self, partition):
        raise NotImplementedError('method erase_partition not implemented')

    def erase_flash(self):
        raise NotImplementedError('method erase_flash not implemented')

    def dump_flash(self, output_file, **kwargs):
        raise NotImplementedError('method dump_flash not implemented')

    @classmethod
    def list_available_ports(cls):
        return ['socket://localhost:{}'.format(cls.QEMU_SERIAL_PORT)]

    def close(self):
        super(IDFQEMUDUT, self).close()
        self.qemu.sendline(b'q\n')
        self.qemu.expect_exact(b'(qemu)')
        for _ in range(self.DEFAULT_EXPECT_TIMEOUT):
            if not self.qemu.isalive():
                break
            time.sleep(1)
        else:
            self.qemu.terminate(force=True)


class ESP32QEMUDUT(IDFQEMUDUT):
    TARGET = 'esp32'  # type: ignore
    TOOLCHAIN_PREFIX = 'xtensa-esp32-elf-'  # type: ignore


class IDFFPGADUT(IDFDUT):
    TARGET = None                           # type: str
    TOOLCHAIN_PREFIX = None                 # type: str
    ERASE_NVS = True
    FLASH_ENCRYPT_SCHEME = None             # type: str
    FLASH_ENCRYPT_CNT_KEY = None            # type: str
    FLASH_ENCRYPT_CNT_VAL = 0
    FLASH_ENCRYPT_PURPOSE = None            # type: str
    SECURE_BOOT_EN_KEY = None               # type: str
    SECURE_BOOT_EN_VAL = 0
    FLASH_SECTOR_SIZE = 4096

    def __init__(self, name, port, log_file, app, allow_dut_exception=False, efuse_reset_port=None, **kwargs):
        super(IDFFPGADUT, self).__init__(name, port, log_file, app, allow_dut_exception=allow_dut_exception, **kwargs)
        self.esp = self.get_rom()(port)
        self.efuses = None
        self.efuse_operations = None
        self.efuse_reset_port = efuse_reset_port

    @classmethod
    def get_rom(cls):
        raise NotImplementedError('This is an abstraction class, method not defined.')

    def erase_partition(self, esp, partition):
        raise NotImplementedError()

    def enable_efuses(self):
        # We use an extra COM port to reset the efuses on FPGA.
        # Connect DTR pin of the COM port to the efuse reset pin on daughter board
        # Set EFUSEPORT env variable to the extra COM port
        if not self.efuse_reset_port:
            raise RuntimeError('EFUSEPORT not specified')

        # Stop any previous serial port operation
        self.stop_receive()
        if self.secure_boot_en:
            self.esp.connect()
        self.efuses, self.efuse_operations = espefuse.get_efuses(self.esp, False, False, True)

    def burn_efuse(self, field, val):
        if not self.efuse_operations:
            self.enable_efuses()
        BurnEfuseArgs = collections.namedtuple('burn_efuse_args', ['name_value_pairs'])
        args = BurnEfuseArgs({field: val})
        self.efuse_operations.burn_efuse(self.esp, self.efuses, args)

    def burn_efuse_key(self, key, purpose, block):
        if not self.efuse_operations:
            self.enable_efuses()
        BurnKeyArgs = collections.namedtuple('burn_key_args',
                                             ['keyfile', 'keypurpose', 'block',
                                              'force_write_always', 'no_write_protect', 'no_read_protect'])
        args = BurnKeyArgs([key],
                           [purpose],
                           [block],
                           False, False, False)
        self.efuse_operations.burn_key(self.esp, self.efuses, args)

    def burn_efuse_key_digest(self, key, purpose, block):
        if not self.efuse_operations:
            self.enable_efuses()
        BurnDigestArgs = collections.namedtuple('burn_key_digest_args',
                                                ['keyfile', 'keypurpose', 'block',
                                                 'force_write_always', 'no_write_protect', 'no_read_protect'])
        args = BurnDigestArgs([open(key, 'rb')],
                              [purpose],
                              [block],
                              False, False, True)
        self.efuse_operations.burn_key_digest(self.esp, self.efuses, args)

    def reset_efuses(self):
        if not self.efuse_reset_port:
            raise RuntimeError('EFUSEPORT not specified')
        with serial.Serial(self.efuse_reset_port) as efuseport:
            print('Resetting efuses')
            efuseport.dtr = 0
            self.port_inst.setRTS(1)
            self.port_inst.setRTS(0)
            time.sleep(1)
            efuseport.dtr = 1
            self.efuse_operations = None
            self.efuses = None

    def sign_data(self, data_file, key_files, version, append_signature=0):
        SignDataArgs = collections.namedtuple('sign_data_args',
                                              ['datafile','keyfile','output', 'version', 'append_signatures'])
        outfile = tempfile.NamedTemporaryFile()
        args = SignDataArgs(data_file, key_files, outfile.name, str(version), append_signature)
        espsecure.sign_data(args)
        outfile.seek(0)
        return outfile.read()


class ESP32C3FPGADUT(IDFFPGADUT):
    TARGET = 'esp32c3'
    TOOLCHAIN_PREFIX = 'riscv32-esp-elf-'
    FLASH_ENCRYPT_SCHEME = 'AES-XTS'
    FLASH_ENCRYPT_CNT_KEY = 'SPI_BOOT_CRYPT_CNT'
    FLASH_ENCRYPT_CNT_VAL = 1
    FLASH_ENCRYPT_PURPOSE = 'XTS_AES_128_KEY'
    SECURE_BOOT_EN_KEY = 'SECURE_BOOT_EN'
    SECURE_BOOT_EN_VAL = 1

    @classmethod
    def get_rom(cls):
        return targets.ESP32C3ROM

    def erase_partition(self, esp, partition):
        raise NotImplementedError()

    def flash_encrypt_burn_cnt(self):
        self.burn_efuse(self.FLASH_ENCRYPT_CNT_KEY, self.FLASH_ENCRYPT_CNT_VAL)

    def flash_encrypt_burn_key(self, key, block=0):
        self.burn_efuse_key(key, self.FLASH_ENCRYPT_PURPOSE, 'BLOCK_KEY%d' % block)

    def flash_encrypt_get_scheme(self):
        return self.FLASH_ENCRYPT_SCHEME

    def secure_boot_burn_en_bit(self):
        self.burn_efuse(self.SECURE_BOOT_EN_KEY, self.SECURE_BOOT_EN_VAL)

    def secure_boot_burn_digest(self, digest, key_index=0, block=0):
        self.burn_efuse_key_digest(digest, 'SECURE_BOOT_DIGEST%d' % key_index, 'BLOCK_KEY%d' % block)

    @classmethod
    def confirm_dut(cls, port, **kwargs):
        return True, cls.TARGET


class ESP32S3FPGADUT(IDFFPGADUT):
    TARGET = 'esp32s3'
    TOOLCHAIN_PREFIX = 'xtensa-esp32s3-elf-'
    FLASH_ENCRYPT_SCHEME = 'AES-XTS'
    FLASH_ENCRYPT_CNT_KEY = 'SPI_BOOT_CRYPT_CNT'
    FLASH_ENCRYPT_CNT_VAL = 1
    FLASH_ENCRYPT_PURPOSE = 'XTS_AES_128_KEY'
    SECURE_BOOT_EN_KEY = 'SECURE_BOOT_EN'
    SECURE_BOOT_EN_VAL = 1

    @classmethod
    def get_rom(cls):
        return targets.ESP32S3ROM

    def erase_partition(self, esp, partition):
        raise NotImplementedError()

    def flash_encrypt_burn_cnt(self):
        self.burn_efuse(self.FLASH_ENCRYPT_CNT_KEY, self.FLASH_ENCRYPT_CNT_VAL)

    def flash_encrypt_burn_key(self, key, block=0):
        self.burn_efuse_key(key, self.FLASH_ENCRYPT_PURPOSE, 'BLOCK_KEY%d' % block)

    def flash_encrypt_get_scheme(self):
        return self.FLASH_ENCRYPT_SCHEME

    def secure_boot_burn_en_bit(self):
        self.burn_efuse(self.SECURE_BOOT_EN_KEY, self.SECURE_BOOT_EN_VAL)

    def secure_boot_burn_digest(self, digest, key_index=0, block=0):
        self.burn_efuse_key_digest(digest, 'SECURE_BOOT_DIGEST%d' % key_index, 'BLOCK_KEY%d' % block)

    @classmethod
    def confirm_dut(cls, port, **kwargs):
        return True, cls.TARGET
