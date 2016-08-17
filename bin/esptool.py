#!/usr/bin/env python
# NB: Before sending a PR to change the above line to '#!/usr/bin/env python2', please read https://github.com/themadinventor/esptool/issues/21
#
# ESP8266 ROM Bootloader Utility
# https://github.com/themadinventor/esptool
#
# Copyright (C) 2014-2016 Fredrik Ahlberg, Angus Gratton, other contributors as noted.
# ESP31/32 support Copyright (C) 2016 Angus Gratton, based in part on work Copyright
# (C) 2015-2016 Espressif Systems.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
# Street, Fifth Floor, Boston, MA 02110-1301 USA.

import argparse
import hashlib
import inspect
import json
import os
import serial
import struct
import subprocess
import sys
import time
import zlib

__version__ = "2.0-dev"


MAX_UINT32 = 0xffffffff
MAX_UINT24 = 0xffffff


class ESPROM(object):
    """ Base class providing access to ESP ROM bootloader. Subclasses provide
    ESP8266, ESP31 & ESP32 specific functionality.

    Don't instantiate this base class directly, either instantiate a subclass or
    call ESPROM.detect_chip() which will interrogate the chip and return the
    appropriate subclass instance.

    """
    CHIP_NAME = "Espressif device"

    DEFAULT_PORT = "/dev/ttyUSB0"

    # These are the currently known commands supported by the ROM
    ESP_FLASH_BEGIN = 0x02
    ESP_FLASH_DATA  = 0x03
    ESP_FLASH_END   = 0x04
    ESP_MEM_BEGIN   = 0x05
    ESP_MEM_END     = 0x06
    ESP_MEM_DATA    = 0x07
    ESP_SYNC        = 0x08
    ESP_WRITE_REG   = 0x09
    ESP_READ_REG    = 0x0a

    # Maximum block sized for RAM and Flash writes, respectively.
    ESP_RAM_BLOCK   = 0x1800
    ESP_FLASH_BLOCK = 0x400

    # Default baudrate. The ROM auto-bauds, so we can use more or less whatever we want.
    ESP_ROM_BAUD    = 115200

    # First byte of the application image
    ESP_IMAGE_MAGIC = 0xe9

    # Initial state for the checksum routine
    ESP_CHECKSUM_MAGIC = 0xef

    # Flash sector size, minimum unit of erase.
    ESP_FLASH_SECTOR = 0x1000

    UART_DATA_REG_ADDR = 0x60000078

    # SPI peripheral "command" bitmasks
    SPI_CMD_READ_ID = 0x10000000

    # Memory addresses
    IROM_MAP_START = 0x40200000
    IROM_MAP_END = 0x40300000

    # The number of bytes in the response that signify command status
    STATUS_BYTES_LENGTH = 2

    def __init__(self, port=DEFAULT_PORT, baud=ESP_ROM_BAUD, do_connect=True):
        """Base constructor for ESPROM objects

        Don't call this constructor, either instantiate ESP8266ROM,
        ESP31ROM, or ESP32ROM, or use ESPROM.detect_chip().

        """
        self._port = serial.Serial(port)
        self._slip_reader = slip_reader(self._port)
        # setting baud rate in a separate step is a workaround for
        # CH341 driver on some Linux versions (this opens at 9600 then
        # sets), shouldn't matter for other platforms/drivers. See
        # https://github.com/themadinventor/esptool/issues/44#issuecomment-107094446
        self._port.baudrate = baud
        if do_connect:
            self.connect()

    @staticmethod
    def detect_chip(port=DEFAULT_PORT, baud=ESP_ROM_BAUD):
        """Use serial access to detect the chip type.

        We use the UART's datecode register for this, it's mapped at
        the same address on ESP8266 & ESP31/32 so we can use one
        memory read and compare to the datecode register for each chip
        type.

        """
        detect_port = ESPROM(port, baud, True)
        sys.stdout.write('Detecting chip type... ')
        date_reg = detect_port.read_reg(ESPROM.UART_DATA_REG_ADDR)
        for cls in [ESP8266ROM, ESP31ROM, ESP32ROM]:
            if date_reg == cls.DATE_REG_VALUE:
                inst = cls(port, baud, False)  # don't connect a second time
                print '%s' % inst.CHIP_NAME
                return inst
        print ''
        raise FatalError("Unexpected UART datecode value 0x%08x. Failed to autodetect chip type." % date_reg)

    """ Read a SLIP packet from the serial port """
    def read(self):
        return self._slip_reader.next()

    """ Write bytes to the serial port while performing SLIP escaping """
    def write(self, packet):
        buf = '\xc0' \
              + (packet.replace('\xdb','\xdb\xdd').replace('\xc0','\xdb\xdc')) \
              + '\xc0'
        self._port.write(buf)

    """ Calculate checksum of a blob, as it is defined by the ROM """
    @staticmethod
    def checksum(data, state=ESP_CHECKSUM_MAGIC):
        for b in data:
            state ^= ord(b)
        return state

    """ Send a request and read the response """
    def command(self, op=None, data=None, chk=0):
        if op is not None:
            pkt = struct.pack('<BBHI', 0x00, op, len(data), chk) + data
            self.write(pkt)

        # tries to get a response until that response has the
        # same operation as the request or a retries limit has
        # exceeded. This is needed for some esp8266s that
        # reply with more sync responses than expected.
        for retry in xrange(100):
            p = self.read()
            if len(p) < 8:
                continue
            (resp, op_ret, len_ret, val) = struct.unpack('<BBHI', p[:8])
            if resp != 1:
                continue
            data = p[8:]
            if op is None or op_ret == op:
                return val, data

        raise FatalError("Response doesn't match request")

    def check_command(self, op_description, op=None, data=None, chk=0):
        """
        Execute a command with 'command', check the result code and throw an appropriate
        FatalError if it fails.

        Returns the "result" of a successful command.
        """
        val, data = self.command(op, data, chk)

        # things are a bit weird here, bear with us

        # the status bytes are the last 2/4 bytes in the data (depending on chip)
        if len(data) < self.STATUS_BYTES_LENGTH:
            raise FatalError("Failed to %s. Only got %d byte status response." % op_description, len(data))
        status_bytes = data[-self.STATUS_BYTES_LENGTH:]
        # we only care if the first one is non-zero. If it is, the second byte is a reason.
        if status_bytes[0] != '\0':
            raise FatalError.WithResult('Failed to %s' % op_description, status_bytes)

        # if we had more data than just the status bytes, return it as the result
        # (this is used by the md5sum command, maybe other commands?)
        if len(data) > self.STATUS_BYTES_LENGTH:
            return data[:-self.STATUS_BYTES_LENGTH]
        else:  # otherwise, just return the 'val' field which comes from the reply header (this is used by read_reg)
            return val

    def sync(self):
        """ Perform a connection test """
        self.command(ESPROM.ESP_SYNC, '\x07\x07\x12\x20' + 32 * '\x55')
        for i in xrange(7):
            self.command()

    def connect(self):
        """ Try connecting repeatedly until successful, or giving up """
        print 'Connecting...'

        for _ in xrange(4):
            # issue reset-to-bootloader:
            # RTS = either CH_PD or nRESET (both active low = chip in reset)
            # DTR = GPIO0 (active low = boot to flasher)
            self._port.setDTR(False)
            self._port.setRTS(True)
            time.sleep(0.05)
            self._port.setDTR(True)
            self._port.setRTS(False)
            time.sleep(0.05)
            self._port.setDTR(False)

            # worst-case latency timer should be 255ms (probably <20ms)
            self._port.timeout = 0.3
            last_exception = None
            for _ in xrange(4):
                try:
                    self._port.flushInput()
                    self._slip_reader = slip_reader(self._port)
                    self._port.flushOutput()
                    self.sync()
                    self._port.timeout = 5
                    return
                except FatalError as e:
                    last_exception = e
                    time.sleep(0.05)
        raise FatalError('Failed to connect to %s: %s' % (self.CHIP_NAME, last_exception))

    """ Read memory address in target """
    def read_reg(self, addr):
        # we don't call check_command here because read_reg() function is called
        # when detecting chip type, and the way we check for success (STATUS_BYTES_LENGTH) is different
        # for different chip types (!)
        val, data = self.command(ESPROM.ESP_READ_REG, struct.pack('<I', addr))
        if data[0] != '\0':
            raise FatalError.WithResult("Failed to read register address %08x" % addr, data)
        return val

    """ Write to memory address in target """
    def write_reg(self, addr, value, mask, delay_us=0):
        return self.check_command("write target memory", ESPROM.ESP_WRITE_REG,
                                  struct.pack('<IIII', addr, value, mask, delay_us))

    """ Start downloading an application image to RAM """
    def mem_begin(self, size, blocks, blocksize, offset):
        return self.check_command("enter RAM download mode", ESPROM.ESP_MEM_BEGIN,
                                  struct.pack('<IIII', size, blocks, blocksize, offset))

    """ Send a block of an image to RAM """
    def mem_block(self, data, seq):
        return self.check_command("write to target RAM", ESPROM.ESP_MEM_DATA,
                                  struct.pack('<IIII', len(data), seq, 0, 0) + data,
                                  ESPROM.checksum(data))

    """ Leave download mode and run the application """
    def mem_finish(self, entrypoint=0):
        return self.check_command("leave RAM download mode", ESPROM.ESP_MEM_END,
                                  struct.pack('<II', int(entrypoint == 0), entrypoint))

    """ Start downloading to Flash (performs an erase) """
    def flash_begin(self, size, offset):
        old_tmo = self._port.timeout
        num_blocks = (size + ESPROM.ESP_FLASH_BLOCK - 1) / ESPROM.ESP_FLASH_BLOCK
        erase_size = self.get_erase_size(size)

        self._port.timeout = 20
        t = time.time()
        self.check_command("enter Flash download mode", ESPROM.ESP_FLASH_BEGIN,
                           struct.pack('<IIII', erase_size, num_blocks, ESPROM.ESP_FLASH_BLOCK, offset))
        if size != 0:
            print "Took %.2fs to erase flash block" % (time.time() - t)
        self._port.timeout = old_tmo

    """ Write block to flash """
    def flash_block(self, data, seq):
        self.check_command("write to target Flash after seq %d" % seq,
                           ESPROM.ESP_FLASH_DATA,
                           struct.pack('<IIII', len(data), seq, 0, 0) + data,
                           ESPROM.checksum(data))

    """ Leave flash mode and run/reboot """
    def flash_finish(self, reboot=False):
        pkt = struct.pack('<I', int(not reboot))
        self.check_command("leave Flash mode", ESPROM.ESP_FLASH_END, pkt)

    """ Run application code in flash """
    def run(self, reboot=False):
        # Fake flash begin immediately followed by flash end
        self.flash_begin(0, 0)
        self.flash_finish(reboot)

    """ Read SPI flash manufacturer and device id """
    def flash_id(self):
        self.flash_begin(0, 0)
        self.write_reg(self.SPI_W0_REG_ADDR, 0x0, MAX_UINT32)
        self.write_reg(self.SPI_CMD_REG_ADDR, self.SPI_CMD_READ_ID, MAX_UINT32)
        flash_id = self.read_reg(self.SPI_W0_REG_ADDR)
        self.flash_finish(False)
        return flash_id

    def parse_flash_size_arg(self, arg):
        try:
            return self.FLASH_SIZES[arg]
        except KeyError:
            raise FatalError("Flash size '%s' is not supported by this chip type. Supported sizes: %s"
                             % (arg, ", ".join(self.FLASH_SIZES.keys())))

    """ Abuse the loader protocol to force flash to be left in write mode """
    def flash_unlock_dio(self):
        # Enable flash write mode
        self.flash_begin(0, 0)
        # Reset the chip rather than call flash_finish(), which would have
        # write protected the chip again (why oh why does it do that?!)
        self.mem_begin(0,0,0,0x40100000)
        self.mem_finish(0x40000080)

    def run_stub(self, stub, params, read_output=False):
        stub = dict(stub)
        stub['code'] = unhexify(stub['code'])
        if 'data' in stub:
            stub['data'] = unhexify(stub['data'])

        if stub['num_params'] != len(params):
            raise FatalError('Stub requires %d params, %d provided'
                             % (stub['num_params'], len(params)))

        params = struct.pack('<' + ('I' * stub['num_params']), *params)
        pc = params + stub['code']

        # Upload
        self.mem_begin(len(pc), 1, len(pc), stub['params_start'])
        self.mem_block(pc, 0)
        if 'data' in stub:
            self.mem_begin(len(stub['data']), 1, len(stub['data']), stub['data_start'])
            self.mem_block(stub['data'], 0)
        self.mem_finish(stub['entry'])

        if read_output:
            print 'Stub executed, reading response:'
            while True:
                p = self.read()
                print hexify(p)
                if p == '':
                    return


class ESP8266ROM(ESPROM):
    """ Access class for ESP8266 ROM bootloader
    """
    CHIP_NAME = "ESP8266EX"

    DATE_REG_VALUE = 0x00062000

    # OTP ROM addresses
    ESP_OTP_MAC0    = 0x3ff00050
    ESP_OTP_MAC1    = 0x3ff00054
    ESP_OTP_MAC3    = 0x3ff0005c

    SPI_CMD_REG_ADDR = 0x60000200
    SPI_W0_REG_ADDR = 0x60000240

    FLASH_SIZES = {
        '512KB':0x00,
        '256KB':0x10,
        '1MB':0x20,
        '2MB':0x30,
        '4MB':0x40,
        '2MB-c1': 0x50,
        '4MB-c1':0x60,
        '4MB-c2':0x70}

    def chip_id(self):
        """ Read Chip ID from OTP ROM - see http://esp8266-re.foogod.com/wiki/System_get_chip_id_%28IoT_RTOS_SDK_0.9.9%29 """
        id0 = self.read_reg(self.ESP_OTP_MAC0)
        id1 = self.read_reg(self.ESP_OTP_MAC1)
        return (id0 >> 24) | ((id1 & MAX_UINT24) << 8)

    def read_mac(self):
        """ Read MAC from OTP ROM """
        mac0 = self.read_reg(self.ESP_OTP_MAC0)
        mac1 = self.read_reg(self.ESP_OTP_MAC1)
        mac3 = self.read_reg(self.ESP_OTP_MAC3)
        if (mac3 != 0):
            oui = ((mac3 >> 16) & 0xff, (mac3 >> 8) & 0xff, mac3 & 0xff)
        elif ((mac1 >> 16) & 0xff) == 0:
            oui = (0x18, 0xfe, 0x34)
        elif ((mac1 >> 16) & 0xff) == 1:
            oui = (0xac, 0xd0, 0x74)
        else:
            raise FatalError("Unknown OUI")
        return oui + ((mac1 >> 8) & 0xff, mac1 & 0xff, (mac0 >> 24) & 0xff)

    def get_erase_size(self, size):
        """ Calculate an erase size given a specific size in bytes.

        Provides a workaround for the bootloader erase bug."""

        sectors_per_block = 16
        sector_size = self.ESP_FLASH_SECTOR
        num_sectors = (size + sector_size - 1) / sector_size
        start_sector = offset / sector_size

        head_sectors = sectors_per_block - (start_sector % sectors_per_block)
        if num_sectors < head_sectors:
            head_sectors = num_sectors

        if num_sectors < 2 * head_sectors:
            return (num_sectors + 1) / 2 * sector_size
        else:
            return (num_sectors - head_sectors) * sector_size

class ESP31ROM(ESPROM):
    """ Access class for ESP31 ROM bootloader
    """
    CHIP_NAME = "ESP31"

    DATE_REG_VALUE = 0x15052100

    SPI_CMD_REG_ADDR = 0x60003000
    SPI_W0_REG_ADDR = 0x60003040

    EFUSE_BASE = 0x6001a000

    FLASH_SIZES = {
        '1MB':0x00,
        '2MB':0x10,
        '4MB':0x20,
        '8MB':0x30,
        '16MB':0x40
    }

    def read_efuse(self, n):
        """ Read the nth word of the ESP3x EFUSE region. """
        return self.read_reg(self.EFUSE_BASE + (4 * n))

    def chip_id(self):
        word16 = self.read_efuse(16)
        word17 = self.read_efuse(17)
        return ((word17 & MAX_UINT24) << 24) | (word16 >> 8) & MAX_UINT24

    def read_mac(self):
        """ Read MAC from EFUSE region """
        word16 = self.read_efuse(16)
        word17 = self.read_efuse(17)
        word18 = self.read_efuse(18)
        word19 = self.read_efuse(19)
        wifi_mac = (((word17 >> 16) & 0xff), ((word17 >> 8) & 0xff), ((word17 >> 0) & 0xff),
                    ((word16 >> 24) & 0xff), ((word16 >> 16) & 0xff), ((word16 >> 8) & 0xff))
        bt_mac = (((word19 >> 16) & 0xff), ((word19 >> 8) & 0xff), ((word19 >> 0) & 0xff),
                  ((word18 >> 24) & 0xff), ((word18 >> 16) & 0xff), ((word18 >> 8) & 0xff))
        return (wifi_mac,bt_mac)

    def get_erase_size(self, size):
        return size

class ESP32ROM(ESP31ROM):
    """Access class for ESP32 ROM bootloader

    """
    CHIP_NAME = "ESP32"

    DATE_REG_VALUE = 0x15122500

    # ESP32-only commands
    ESP_SPI_FLASH_SET = 0xb

    ESP_SPI_ATTACH_REQ = 0xD

    ESP_CHANGE_BAUDRATE = 0x0F
    ESP_FLASH_DEFL_BEGIN = 0x10
    ESP_FLASH_DEFL_DATA  = 0x11
    ESP_FLASH_DEFL_END   = 0x12

    ESP_SPI_FLASH_MD5    = 0x13

    IROM_MAP_START = 0x400d0000
    IROM_MAP_END   = 0x40400000
    DROM_MAP_START = 0x3F400000
    DROM_MAP_END   = 0x3F700000

    # ESP32 uses a 4 byte status reply
    STATUS_BYTES_LENGTH = 4

    def flash_defl_begin(self, size, compsize, offset):
        """ Start downloading compressed data to Flash (performs an erase) """
        old_tmo = self._port.timeout
        num_blocks = (compsize + self.ESP_FLASH_BLOCK - 1) / self.ESP_FLASH_BLOCK
        erase_blocks = (size + self.ESP_FLASH_BLOCK - 1) / self.ESP_FLASH_BLOCK

        erase_size = size
        if erase_size > 0 and (offset + erase_size) >= (16 / 8) * 1024 * 1024:
            self.flash_spi_param_set()

        self._port.timeout = 20
        t = time.time()
        print "Unc size %d comp size %d comp blocks %d" % (size, compsize, num_blocks)
        self.check_command("enter compressed flash mode", self.ESP_FLASH_DEFL_BEGIN,
                           struct.pack('<IIII', erase_blocks * self.ESP_FLASH_BLOCK, num_blocks, self.ESP_FLASH_BLOCK, offset))
        if size != 0:
            print "Took %.2fs to erase flash block" % (time.time() - t)
        self._port.timeout = old_tmo

    """ Write block to flash, send compressed """
    def flash_defl_block(self, data, seq):
        self.check_command("write compressed data to flash after seq %d" % seq,
                           self.ESP_FLASH_DEFL_DATA, struct.pack('<IIII', len(data), seq, 0, 0) + data, ESPROM.checksum(data))

    """ Leave compressed flash mode and run/reboot """
    def flash_defl_finish(self, reboot=False):
        pkt = struct.pack('<I', int(not reboot))
        self.check_command("leave compressed flash mode", self.ESP_FLASH_DEFL_END, pkt)
        self.in_bootloader = False

    def flash_md5sum(self, addr, size):
        # the MD5 command is special (
        return self.check_command('calculate md5sum', self.ESP_SPI_FLASH_MD5, struct.pack('<IIII', addr, size, 0, 0))

    def changebaud(self, baud):
        print "Changing baud rate to %d" % baud
        self.command(self.ESP_CHANGE_BAUDRATE, struct.pack('<II', baud, 0))
        print "Changed."
        time.sleep(0.05)  # get rid of crap sent during baud rate change
        self._port.flushInput()
        self._port.baudrate = baud

    def flash_spi_attach_req(self,ucIsHspi,ucIsLegacy):
        """Send SPI attach command

        Internal Espressif function. Deprecate?
        """
        print "SEND ESP SPI ATTACH CMD"
        # last 3 bytes in ESP_SPI_ATTACH_REQ argument are reserved values
        arg = struct.pack('<IBBBB', ucIsHspi, ucIsLegacy, 0, 0, 0)
        self.check_command("configure SPI Flash attachment", ESP32ROM.ESP_SPI_ATTACH_REQ,
                           arg)

    def flash_spi_param_set(self):
        """Set the flash params for ESP booter

        I think this means writing a "flash_chip" type structure to RAM, so the ESP32 knows we have a larger flash size.
        """
        # FOR ESP32, SET FLASH INFO FOR ROM CODE, DEFAULT IS 16Mbits, WE NEED TO RE-SET IT TO A BIGGER SIZE.
        print("SET FLASH PARAMS")
        fl_id = 0
        total_size = (128 / 8) * 1024 * 1024
        block_size = 64 * 1024
        sector_size = 4 * 1024
        page_size = 256
        status_mask = 0xffff

        err = self.command(ESP32ROM.ESP_SPI_FLASH_SET,
                           struct.pack('<IIIIII', fl_id, total_size, block_size, sector_size, page_size, status_mask))[1]
        if err:  # Should be checking one part of this tuple or the other, I think
            raise FatalError.WithResult('Failed to config flash', err)


class ESPBOOTLOADER(object):
    """ These are constants related to software ESP bootloader, working with 'v2' image files """

    # First byte of the "v2" application image
    IMAGE_V2_MAGIC = 0xea

    # First 'segment' value in a "v2" application image, appears to be a constant version value?
    IMAGE_V2_SEGMENT = 4


def LoadFirmwareImage(chip, filename):
    """ Load a firmware image. Can be for ESP8266 or ESP32. ESP8266 images will be examined to determine if they are
        original ROM firmware images (ESPFirmwareImage) or "v2" OTA bootloader images.

        Returns a BaseFirmwareImage subclass, either ESPFirmwareImage (v1) or OTAFirmwareImage (v2).
    """
    with open(filename, 'rb') as f:
        if chip == 'esp32':
            return ESP32FirmwareImage(f)
        else:  # Otherwise, ESP8266 so look at magic to determine the image type
            magic = ord(f.read(1))
            f.seek(0)
            if magic == ESPROM.ESP_IMAGE_MAGIC:
                return ESPFirmwareImage(f)
            elif magic == ESPBOOTLOADER.IMAGE_V2_MAGIC:
                return OTAFirmwareImage(f)
            else:
                raise FatalError("Invalid image magic number: %d" % magic)


class ImageSegment(object):
    """ Wrapper class for a segment in an ESP image
    (very similar to a section in an ELFImage also) """
    def __init__(self, addr, data, file_offs=None):
        self.addr = addr
        # pad all ImageSegments to at least 4 bytes length
        pad_mod = len(data) % 4
        if pad_mod != 0:
            data += "\x00" * (4 - pad_mod)
        self.data = data
        self.file_offs = file_offs

    def copy_with_new_addr(self, new_addr):
        """ Return a new ImageSegment with same data, but mapped at
        a new address. """
        return ImageSegment(new_addr, self.data, 0)

    def __repr__(self):
        r = "len 0x%05x load 0x%08x" % (len(self.data), self.addr)
        if self.file_offs is not None:
            r += " file_offs 0x%08x" % (self.file_offs)
        return r

class ELFSection(ImageSegment):
    """ Wrapper class for a section in an ELF image, has a section
    name as well as the common properties of an ImageSegment. """
    def __init__(self, name, addr, data):
        super(ELFSection, self).__init__(addr, data)
        self.name = name

    def __repr__(self):
        return "%s %s" % (self.name, super(ELFSection, self).__repr__())


class BaseFirmwareImage(object):
    SEG_HEADER_LEN = 8

    """ Base class with common firmware image functions """
    def __init__(self):
        self.segments = []
        self.entrypoint = 0

    def load_common_header(self, load_file, expected_magic):
            (magic, segments, self.flash_mode, self.flash_size_freq, self.entrypoint) = struct.unpack('<BBBBI', load_file.read(8))

            if magic != expected_magic or segments > 16:
                raise FatalError('Invalid firmware image magic=%d segments=%d' % (magic, segments))
            return segments

    def load_segment(self, f, is_irom_segment=False):
        """ Load the next segment from the image file """
        file_offs = f.tell()
        (offset, size) = struct.unpack('<II', f.read(8))
        if not is_irom_segment:
            if offset > 0x40200000 or offset < 0x3ffe0000 or size > 65536:
                print('WARNING: Suspicious segment 0x%x, length %d' % (offset, size))
        segment_data = f.read(size)
        if len(segment_data) < size:
            raise FatalError('End of file reading segment 0x%x, length %d (actual length %d)' % (offset, size, len(segment_data)))
        segment = ImageSegment(offset, segment_data, file_offs)
        self.segments.append(segment)
        return segment

    def save_segment(self, f, segment, checksum=None):
        """ Save the next segment to the image file, return next checksum value if provided """
        f.write(struct.pack('<II', segment.addr, len(segment.data)))
        f.write(segment.data)
        if checksum is not None:
            return ESPROM.checksum(segment.data, checksum)

    def read_checksum(self, f):
        """ Return ESPROM checksum from end of just-read image """
        # Skip the padding. The checksum is stored in the last byte so that the
        # file is a multiple of 16 bytes.
        align_file_position(f, 16)
        return ord(f.read(1))

    def append_checksum(self, f, checksum):
        """ Append ESPROM checksum to the just-written image """
        align_file_position(f, 16)
        f.write(struct.pack('B', checksum))

    def write_common_header(self, f, segments):
        f.write(struct.pack('<BBBBI', ESPROM.ESP_IMAGE_MAGIC, len(segments),
                            self.flash_mode, self.flash_size_freq, self.entrypoint))

    def is_irom_addr(self, addr):
        """ Returns True if an address starts in the irom region.
        Valid for ESP8266 only.
        """
        return ESPROM.IROM_MAP_START <= addr < ESPROM.IROM_MAP_END

    def get_irom_segment(self):
            irom_segments = [s for s in self.segments if self.is_irom_addr(s.addr)]
            if len(irom_segments) > 0:
                if len(irom_segments) != 1:
                    raise FatalError('Found %d segments that could be irom0. Bad ELF file?' % len(irom_segments))
                return irom_segments[0]
            return None

    def get_non_irom_segments(self):
        irom_segment = self.get_irom_segment()
        return [s for s in self.segments if s != irom_segment]


class ESPFirmwareImage(BaseFirmwareImage):
    """ 'Version 1' firmware image, segments loaded directly by the ROM bootloader. """
    def __init__(self, load_file=None):
        super(ESPFirmwareImage, self).__init__()
        self.flash_mode = 0
        self.flash_size_freq = 0
        self.version = 1

        if load_file is not None:
            segments = self.load_common_header(load_file, ESPROM.ESP_IMAGE_MAGIC)

            for _ in xrange(segments):
                self.load_segment(load_file)
            self.checksum = self.read_checksum(load_file)

    def default_output_name(self, input_file):
        """ Derive a default output name from the ELF name. """
        return input_file + '-'

    def save(self, basename):
        """ Save a set of V1 images for flashing. Parameter is a base filename. """
        # IROM data goes in its own plain binary file
        irom_segment = self.get_irom_segment()
        if irom_segment is not None:
            with open("%s0x%05x.bin" % (basename, irom_segment.addr), "wb") as f:
                f.write(irom_segment.data)

        # everything but IROM goes at 0x00000 in an image file
        normal_segments = self.get_non_irom_segments()
        with open("%s0x00000.bin" % basename, 'wb') as f:
            self.write_common_header(f, normal_segments)
            checksum = ESPROM.ESP_CHECKSUM_MAGIC
            for segment in self.segments:
                checksum = self.save_segment(f, segment, checksum)
            self.append_checksum(f, checksum)


class OTAFirmwareImage(BaseFirmwareImage):
    """ 'Version 2' firmware image, segments loaded by software bootloader stub
        (ie Espressif bootloader or rboot)
    """
    def __init__(self, load_file=None):
        super(OTAFirmwareImage, self).__init__()
        self.version = 2
        if load_file is not None:
            segments = self.load_common_header(load_file, ESPBOOTLOADER.IMAGE_V2_MAGIC)
            if segments != ESPBOOTLOADER.IMAGE_V2_SEGMENT:
                # segment count is not really segment count here, but we expect to see '4'
                print 'Warning: V2 header has unexpected "segment" count %d (usually 4)' % segments

            # irom segment comes before the second header
            #
            # the file is saved in the image with a zero load address
            # in the header, so we need to calculate a load address
            irom_offs = load_file.tell()
            irom_segment = self.load_segment(load_file, True)
            irom_segment.addr = irom_offs + ESPROM.IROM_MAP_START

            first_flash_mode = self.flash_mode
            first_flash_size_freq = self.flash_size_freq
            first_entrypoint = self.entrypoint
            # load the second header
            self.load_common_header(load_file, ESPROM.ESP_IMAGE_MAGIC)
            (magic, segments, self.flash_mode, self.flash_size_freq, self.entrypoint) = struct.unpack('<BBBBI', load_file.read(8))

            if first_flash_mode != self.flash_mode:
                print('WARNING: Flash mode value in first header (0x%02x) disagrees with second (0x%02x). Using second value.'
                      % (first_flash_mode, self.flash_mode))
            if first_flash_size_freq != self.flash_size_freq:
                print('WARNING: Flash size/freq value in first header (0x%02x) disagrees with second (0x%02x). Using second value.'
                      % (first_flash_size_freq, self.flash_size_freq))
            if first_entrypoint != self.entrypoint:
                print('WARNING: Entrypoint address in first header (0x%08x) disagrees with second header (0x%08x). Using second value.'
                      % (first_entrypoint, self.entrypoint))

            # load all the usual segments
            for _ in xrange(segments):
                self.load_segment(load_file)
            self.checksum = self.read_checksum(load_file)

    def default_output_name(self, input_file):
        """ Derive a default output name from the ELF name. """
        irom_segment = self.get_irom_segment()
        if irom_segment is not None:
            irom_offs = irom_segment.addr - ESPROM.IROM_MAP_START
        else:
            irom_offs = 0
        return "%s-0x%05x.bin" % (os.path.splitext(input_file)[0],
                                  irom_offs & ~(ESPROM.ESP_FLASH_SECTOR - 1))

    def save(self, filename):
        with open(filename, 'wb') as f:
            # Save first header for irom0 segment
            f.write(struct.pack('<BBBBI', ESPBOOTLOADER.IMAGE_V2_MAGIC, ESPBOOTLOADER.IMAGE_V2_SEGMENT,
                                self.flash_mode, self.flash_size_freq, self.entrypoint))

            irom_segment = self.get_irom_segment()
            if irom_segment is not None:
                # save irom0 segment, make sure it has load addr 0 in the file
                irom_segment = irom_segment.copy_with_new_addr(0)
                self.save_segment(f, irom_segment)

            # second header, matches V1 header and contains loadable segments
            normal_segments = self.get_non_irom_segments()
            self.write_common_header(f, normal_segments)
            checksum = ESPROM.ESP_CHECKSUM_MAGIC
            for segment in normal_segments:
                checksum = self.save_segment(f, segment, checksum)
            self.append_checksum(f, checksum)


class ESP32FirmwareImage(BaseFirmwareImage):
    """ ESP32 firmware image is very similar to V1 ESP8266 image,
    except with an additional 16 byte reserved header at top of image,
    and because of new flash mapping capabilities the flash-mapped regions
    can be placed in the normal image (just @ 64kB padded offsets).
    """
    def __init__(self, load_file=None):
        super(ESP32FirmwareImage, self).__init__()
        self.flash_mode = 0
        self.flash_size_freq = 0
        self.version = 1
        self.additional_header = '\x00' * 16

        if load_file is not None:
            segments = self.load_common_header(load_file, ESPROM.ESP_IMAGE_MAGIC)
            self.additional_header = load_file.read(16)

            for i in xrange(segments):
                self.load_segment(load_file)
            self.checksum = self.read_checksum(load_file)

    def is_flash_addr(self, addr):
        return (ESP32ROM.IROM_MAP_START <= addr < ESP32ROM.IROM_MAP_END) \
            or (ESP32ROM.DROM_MAP_START <= addr < ESP32ROM.DROM_MAP_END)

    def default_output_name(self, input_file):
        """ Derive a default output name from the ELF name. """
        return "%s.bin" % (os.path.splitext(input_file)[0])

    def save(self, filename):
        padding_segments = 0
        with open(filename, 'wb') as f:
            self.write_common_header(f, self.segments)
            f.write(self.additional_header)

            checksum = ESPROM.ESP_CHECKSUM_MAGIC
            last_addr = None
            for segment in sorted(self.segments, key=lambda s:s.addr):
                #print("Writing %s file @ 0x%x" % (segment, f.tell()))

                # IROM/DROM segment flash mappings need to align on
                # 64kB boundaries.
                #
                # TODO: intelligently order segments to reduce wastage
                # by squeezing smaller DRAM/IRAM segments into the
                # 64kB padding space.
                IROM_ALIGN = 65536

                # check for multiple ELF sections that live in the same flash mapping region.
                # this is usually a sign of a broken linker script, but if you have a legitimate
                # use case then let us know (we can merge segments here, but as a rule you probably
                # want to merge them in your linker script.)
                if last_addr is not None and self.is_flash_addr(last_addr) \
                   and self.is_flash_addr(segment.addr) and segment.addr // IROM_ALIGN == last_addr // IROM_ALIGN:
                    raise FatalError(("Segment loaded at 0x%08x lands in same 64KB flash mapping as segment loaded at 0x%08x. "+
                                     "Can't generate binary. Suggest changing linker script or ELF to merge sections.") %
                                     (segment.addr, last_addr))
                last_addr = segment.addr

                if self.is_flash_addr(segment.addr):
                    #print("Padding from offset %08x" % f.tell())
                    # Actual alignment required for the segment header: positioned so that
                    # after we write the next 8 byte header, file_offs % IROM_ALIGN == segment.addr % IROM_ALIGN
                    #
                    # (this is because the segment's vaddr may not be IROM_ALIGNed, more likely is aligned
                    # IROM_ALIGN+0x10 to account for longest possible header.
                    align_past = (segment.addr % IROM_ALIGN) - self.SEG_HEADER_LEN
                    #print "segment starts 0x%x so aligning header at +0x%x" % (segment.addr, align_past)
                    assert (align_past + self.SEG_HEADER_LEN) == (segment.addr % IROM_ALIGN)

                    # subtract SEG_HEADER_LEN a second time, as the padding block has a header as well
                    pad_len = ( IROM_ALIGN - (f.tell() % IROM_ALIGN) ) + align_past - self.SEG_HEADER_LEN
                    if pad_len < 0:
                        pad_len += IROM_ALIGN
                    if pad_len > 0:
                        #print("Calculated pad length %08x to place next header @ %08x" % (pad_len, f.tell()+pad_len))
                        null = ImageSegment(0, '\x00' * pad_len, f.tell())
                        checksum = self.save_segment(f, null, checksum)
                        #print("After padding, at file offset %08x" % f.tell())
                        padding_segments += 1
                    #print "Comparing file offs %x (data @ %x) with segment load addr %x" % (f.tell(), f.tell() + 8, segment.addr)
                    # verify that after the 8 byte header is added, were are at the correct offset relative to the segment's vaddr
                    assert (f.tell() + 8) % IROM_ALIGN == segment.addr % IROM_ALIGN
                checksum = self.save_segment(f, segment, checksum)
            self.append_checksum(f, checksum)
            # kinda hacky: go back to the initial header and write the new segment count
            # that includes padding segments. Luckily(?) this header is not checksummed
            f.seek(1)
            f.write(chr(len(self.segments) + padding_segments))


class ELFFile(object):
    SEC_TYPE_PROGBITS = 0x01
    SEC_TYPE_STRTAB = 0x03

    def __init__(self, name):
        # Load sections from the ELF file
        self.name = name
        with open(self.name, 'rb') as f:
            self._read_elf_file(f)

    def _read_elf_file(self, f):
        # read the ELF file header
        LEN_FILE_HEADER = 0x34
        try:
            (ident,_type,machine,_version,
             self.entrypoint,_phoff,shoff,_flags,
             _ehsize, _phentsize,_phnum,_shentsize,
             _shnum,shstrndx) = struct.unpack("<16sHHLLLLLHHHHHH", f.read(LEN_FILE_HEADER))
        except struct.error as e:
            raise FatalError("Failed to read a valid ELF header from %s: %s" % (self.name, e))

        if ident[0] != '\x7f' or ident[1:4] != 'ELF':
            raise FatalError("%s has invalid ELF magic header" % self.name)
        if machine != 0x5e:
            raise FatalError("%s does not appear to be an Xtensa ELF file. e_machine=%04x" % (self.name, machine))
        self._read_sections(f, shoff, shstrndx)

    def _read_sections(self, f, section_header_offs, shstrndx):
        f.seek(section_header_offs)
        section_header = f.read()
        LEN_SEC_HEADER = 0x28
        if len(section_header) == 0:
            raise FatalError("No section header found at offset %04x in ELF file." % section_header_offs)
        if len(section_header) % LEN_SEC_HEADER != 0:
            print 'WARNING: Unexpected ELF section header length %04x is not mod-%02x' % (len(section_header),LEN_SEC_HEADER)

        # walk through the section header and extract all sections
        section_header_offsets = range(0, len(section_header), LEN_SEC_HEADER)

        def read_section_header(offs):
            name_offs,sec_type,_flags,lma,sec_offs,size = struct.unpack_from("<LLLLLL", section_header[offs:])
            return (name_offs, sec_type, lma, size, sec_offs)
        all_sections = [read_section_header(offs) for offs in section_header_offsets]
        prog_sections = [s for s in all_sections if s[1] == ELFFile.SEC_TYPE_PROGBITS]

        # search for the string table section
        if not shstrndx * LEN_SEC_HEADER in section_header_offsets:
            raise FatalError("ELF file has no STRTAB section at shstrndx %d" % shstrndx)
        _,sec_type,_,sec_size,sec_offs = read_section_header(shstrndx * LEN_SEC_HEADER)
        if sec_type != ELFFile.SEC_TYPE_STRTAB:
            print 'WARNING: ELF file has incorrect STRTAB section type 0x%02x' % sec_type
        f.seek(sec_offs)
        string_table = f.read(sec_size)

        # build the real list of ELFSections by reading the actual section names from the
        # string table section, and actual data for each section from the ELF file itself
        def lookup_string(offs):
            raw = string_table[offs:]
            return raw[:raw.index('\x00')]

        def read_data(offs,size):
            f.seek(offs)
            return f.read(size)

        prog_sections = [ELFSection(lookup_string(n_offs), lma, read_data(offs, size)) for (n_offs, _type, lma, size, offs) in prog_sections
                         if lma != 0]
        self.sections = prog_sections


class CesantaFlasher(object):

    # From stub_flasher.h
    CMD_FLASH_WRITE = 1
    CMD_FLASH_READ = 2
    CMD_FLASH_DIGEST = 3
    CMD_FLASH_ERASE_CHIP = 5
    CMD_BOOT_FW = 6

    def __init__(self, esp, baud_rate=0):
        print 'Running Cesanta flasher stub...'
        if baud_rate <= ESPROM.ESP_ROM_BAUD:  # don't change baud rates if we already synced at that rate
            baud_rate = 0
        self._esp = esp
        esp.run_stub(json.loads(_CESANTA_FLASHER_STUB), [baud_rate])
        if baud_rate > 0:
            esp._port.baudrate = baud_rate
        # Read the greeting.
        p = esp.read()
        if p != 'OHAI':
            raise FatalError('Failed to connect to the flasher (got %s)' % hexify(p))

    def flash_write(self, addr, data, show_progress=False):
        assert addr % self._esp.ESP_FLASH_SECTOR == 0, 'Address must be sector-aligned'
        assert len(data) % self._esp.ESP_FLASH_SECTOR == 0, 'Length must be sector-aligned'
        sys.stdout.write('Writing %d @ 0x%x... ' % (len(data), addr))
        sys.stdout.flush()
        self._esp.write(struct.pack('<B', self.CMD_FLASH_WRITE))
        self._esp.write(struct.pack('<III', addr, len(data), 1))
        num_sent, num_written = 0, 0
        while num_written < len(data):
            p = self._esp.read()
            if len(p) == 4:
                num_written = struct.unpack('<I', p)[0]
            elif len(p) == 1:
                status_code = struct.unpack('<B', p)[0]
                raise FatalError('Write failure, status: %x' % status_code)
            else:
                raise FatalError('Unexpected packet while writing: %s' % hexify(p))
            if show_progress:
                progress = '%d (%d %%)' % (num_written, num_written * 100.0 / len(data))
                sys.stdout.write(progress + '\b' * len(progress))
                sys.stdout.flush()
            while num_sent - num_written < 5120:
                self._esp._port.write(data[num_sent:num_sent + 1024])
                num_sent += 1024
        p = self._esp.read()
        if len(p) != 16:
            raise FatalError('Expected digest, got: %s' % hexify(p))
        digest = hexify(p).upper()
        expected_digest = hashlib.md5(data).hexdigest().upper()
        print
        if digest != expected_digest:
            raise FatalError('Digest mismatch: expected %s, got %s' % (expected_digest, digest))
        p = self._esp.read()
        if len(p) != 1:
            raise FatalError('Expected status, got: %s' % hexify(p))
        status_code = struct.unpack('<B', p)[0]
        if status_code != 0:
            raise FatalError('Write failure, status: %x' % status_code)

    def flash_read(self, addr, length, show_progress=False):
        sys.stdout.write('Reading %d @ 0x%x... ' % (length, addr))
        sys.stdout.flush()
        self._esp.write(struct.pack('<B', self.CMD_FLASH_READ))
        # USB may not be able to keep up with the read rate, especially at
        # higher speeds. Since we don't have flow control, this will result in
        # data loss. Hence, we use small packet size and only allow small
        # number of bytes in flight, which we can reasonably expect to fit in
        # the on-chip FIFO. max_in_flight = 64 works for CH340G, other chips may
        # have longer FIFOs and could benefit from increasing max_in_flight.
        self._esp.write(struct.pack('<IIII', addr, length, 32, 64))
        data = ''
        while True:
            p = self._esp.read()
            data += p
            self._esp.write(struct.pack('<I', len(data)))
            if show_progress and (len(data) % 1024 == 0 or len(data) == length):
                progress = '%d (%d %%)' % (len(data), len(data) * 100.0 / length)
                sys.stdout.write(progress + '\b' * len(progress))
                sys.stdout.flush()
            if len(data) == length:
                break
            if len(data) > length:
                raise FatalError('Read more than expected')
        p = self._esp.read()
        if len(p) != 16:
            raise FatalError('Expected digest, got: %s' % hexify(p))
        expected_digest = hexify(p).upper()
        digest = hashlib.md5(data).hexdigest().upper()
        print
        if digest != expected_digest:
            raise FatalError('Digest mismatch: expected %s, got %s' % (expected_digest, digest))
        p = self._esp.read()
        if len(p) != 1:
            raise FatalError('Expected status, got: %s' % hexify(p))
        status_code = struct.unpack('<B', p)[0]
        if status_code != 0:
            raise FatalError('Write failure, status: %x' % status_code)
        return data

    def flash_digest(self, addr, length, digest_block_size=0):
        self._esp.write(struct.pack('<B', self.CMD_FLASH_DIGEST))
        self._esp.write(struct.pack('<III', addr, length, digest_block_size))
        digests = []
        while True:
            p = self._esp.read()
            if len(p) == 16:
                digests.append(p)
            elif len(p) == 1:
                status_code = struct.unpack('<B', p)[0]
                if status_code != 0:
                    raise FatalError('Write failure, status: %x' % status_code)
                break
            else:
                raise FatalError('Unexpected packet: %s' % hexify(p))
        return digests[-1], digests[:-1]

    def boot_fw(self):
        self._esp.write(struct.pack('<B', self.CMD_BOOT_FW))
        p = self._esp.read()
        if len(p) != 1:
            raise FatalError('Expected status, got: %s' % hexify(p))
        status_code = struct.unpack('<B', p)[0]
        if status_code != 0:
            raise FatalError('Boot failure, status: %x' % status_code)

    def flash_erase(self):
        self._esp.write(struct.pack('<B', self.CMD_FLASH_ERASE_CHIP))
        p = self._esp.read()
        if len(p) != 1:
            raise FatalError('Expected status, got: %s' % hexify(p))
        status_code = struct.unpack('<B', p)[0]
        if status_code != 0:
            raise FatalError('Chip erase failure, status: %x' % status_code)


def slip_reader(port):
    """Generator to read SLIP packets from a serial port.
    Yields one full SLIP packet at a time, raises exception on timeout or invalid data.

    Designed to avoid too many calls to serial.read(1), which can bog
    down on slow systems.
    """
    partial_packet = None
    in_escape = False
    while True:
        waiting = port.inWaiting()
        read_bytes = port.read(1 if waiting == 0 else waiting)
        if read_bytes == '':
            raise FatalError("Timed out waiting for packet %s" % ("header" if partial_packet is None else "content"))

        for b in read_bytes:
            if partial_packet is None:  # waiting for packet header
                if b == '\xc0':
                    partial_packet = ""
                else:
                    raise FatalError('Invalid head of packet (%r)' % b)
            elif in_escape:  # part-way through escape sequence
                in_escape = False
                if b == '\xdc':
                    partial_packet += '\xc0'
                elif b == '\xdd':
                    partial_packet += '\xdb'
                else:
                    raise FatalError('Invalid SLIP escape (%r%r)' % ('\xdb', b))
            elif b == '\xdb':  # start of escape sequence
                in_escape = True
            elif b == '\xc0':  # end of packet
                yield partial_packet
                partial_packet = None
            else:  # normal byte in packet
                partial_packet += b


def arg_auto_int(x):
    return int(x, 0)


def div_roundup(a, b):
    """ Return a/b rounded up to nearest integer,
    equivalent result to int(math.ceil(float(int(a)) / float(int(b))), only
    without possible floating point accuracy errors.
    """
    return (int(a) + int(b) - 1) / int(b)


def binutils_safe_path(p):
    """Returns a 'safe' version of path 'p' to pass to binutils

    Only does anything under Cygwin Python, where cygwin paths need to
    be translated to Windows paths if the binutils wasn't compiled
    using Cygwin (should also work with binutils compiled using
    Cygwin, see #73.)
    """
    if sys.platform == "cygwin":
        try:
            return subprocess.check_output(["cygpath", "-w", p]).rstrip('\n')
        except subprocess.CalledProcessError:
            print "WARNING: Failed to call cygpath to sanitise Cygwin path."
    return p


def align_file_position(f, size):
    """ Align the position in the file to the next block of specified size """
    align = (size - 1) - (f.tell() % size)
    f.seek(align, 1)


def hexify(s):
    return ''.join('%02X' % ord(c) for c in s)


def unhexify(hs):
    s = ''
    for i in range(0, len(hs) - 1, 2):
        s += chr(int(hs[i] + hs[i + 1], 16))
    return s


class FatalError(RuntimeError):
    """
    Wrapper class for runtime errors that aren't caused by internal bugs, but by
    ESP8266 responses or input content.
    """
    def __init__(self, message):
        RuntimeError.__init__(self, message)

    @staticmethod
    def WithResult(message, result):
        """
        Return a fatal error object that appends the hex values of
        'result' as a string formatted argument.
        """
        message += " (result was %s)" % ", ".join(hex(ord(x)) for x in result)
        return FatalError(message)


# "Operation" commands, executable at command line. One function each
#
# Each function takes either two args (<ESPROM instance>, <args>) or a single <args>
# argument.

def load_ram(esp, args):
    image = LoadFirmwareImage(esp, args.filename)

    print 'RAM boot...'
    for (offset, size, data) in image.segments:
        print 'Downloading %d bytes at %08x...' % (size, offset),
        sys.stdout.flush()
        esp.mem_begin(size, div_roundup(size, esp.ESP_RAM_BLOCK), esp.ESP_RAM_BLOCK, offset)

        seq = 0
        while len(data) > 0:
            esp.mem_block(data[0:esp.ESP_RAM_BLOCK], seq)
            data = data[esp.ESP_RAM_BLOCK:]
            seq += 1
        print 'done!'

    print 'All segments done, executing at %08x' % image.entrypoint
    esp.mem_finish(image.entrypoint)


def read_mem(esp, args):
    print '0x%08x = 0x%08x' % (args.address, esp.read_reg(args.address))


def write_mem(esp, args):
    esp.write_reg(args.address, args.value, args.mask, 0)
    print 'Wrote %08x, mask %08x to %08x' % (args.value, args.mask, args.address)


def dump_mem(esp, args):
    f = file(args.filename, 'wb')
    for i in xrange(args.size / 4):
        d = esp.read_reg(args.address + (i * 4))
        f.write(struct.pack('<I', d))
        if f.tell() % 1024 == 0:
            print '\r%d bytes read... (%d %%)' % (f.tell(),
                                                  f.tell() * 100 / args.size),
        sys.stdout.flush()
    print 'Done!'


def write_flash(esp, args):
    # This splitting of functionality will go away eventually,
    # but for now this is the easiest way :|
    if isinstance(esp, ESP32ROM):
        write_flash_no_stub(esp, args)
    else:
        write_flash_via_stub(esp, args)


def write_flash_via_stub(esp, args):
    flash_mode = {'qio':0, 'qout':1, 'dio':2, 'dout': 3}[args.flash_mode]
    flash_size_freq = esp.parse_flash_size_arg(args.flash_size)
    flash_size_freq += {'40m':0, '26m':1, '20m':2, '80m': 0xf}[args.flash_freq]
    flash_params = struct.pack('BB', flash_mode, flash_size_freq)

    flasher = CesantaFlasher(esp, args.baud)

    for address, argfile in args.addr_filename:
        image = argfile.read()
        argfile.seek(0)  # rewind in case we need it again
        # Fix sflash config data.
        if address == 0 and image[0] == '\xe9':
            print 'Flash params set to 0x%02x%02x' % (flash_mode, flash_size_freq)
            image = image[0:2] + flash_params + image[4:]
        # Pad to sector size, which is the minimum unit of writing (erasing really).
        if len(image) % esp.ESP_FLASH_SECTOR != 0:
            image += '\xff' * (esp.ESP_FLASH_SECTOR - (len(image) % esp.ESP_FLASH_SECTOR))
        t = time.time()
        flasher.flash_write(address, image, not args.no_progress)
        t = time.time() - t
        print ('\rWrote %d bytes at 0x%x in %.1f seconds (%.1f kbit/s)...'
               % (len(image), address, t, len(image) / t * 8 / 1000))
    print 'Leaving...'
    if args.verify:
        print 'Verifying just-written flash...'
        _verify_flash(flasher, args, flash_params)
    flasher.boot_fw()


def write_flash_no_stub(esp, args):
    """Write flash directly via the bootloader, no stub

    The existence of this function is a hack. Before this is done,
    the stub needs to speak the ESP bootloader protocol so
    we can roll all this code up into the write_flash().

    This function also includes some support for Espressif internal
    testing functions that probably aren't useful for most people, and
    need to go live somewhere... maybe a generic "bootloader command"
    function or something?

    """
    flash_mode = {'qio':0, 'qout':1, 'dio':2, 'dout': 3}[args.flash_mode]
    flash_size_freq = esp.parse_flash_size_arg(args.flash_size)
    flash_size_freq += {'40m':0, '26m':1, '20m':2, '80m': 0xf}[args.flash_freq]
    flash_info = struct.pack('BB', flash_mode, flash_size_freq)

    print "\n\n"
    print "********************************"
    uc_is_hspi = int(args.ucIsHspi,16)
    uc_is_legacy = int(args.ucIsLegacy,16) & 0xff
    print "IS HSPI: 0x%08x" % (uc_is_hspi),type(uc_is_hspi)
    print "--------------------------"
    print "IS LEGACY: 0x%02x" % uc_is_legacy,type(uc_is_legacy)
    print "*********************************"
    print "SENDING SPI ATTACH COMMAND"
    print "--------------"
    esp.flash_spi_attach_req(uc_is_hspi,uc_is_legacy)
    print "START DOWNLOADING..."

    for address, argfile in args.addr_filename:
        print 'Erasing flash...'
        if args.compress:
            uncimage = argfile.read()
            calcmd5 = hashlib.md5(uncimage).hexdigest()
            uncsize = len(uncimage)
            image = zlib.compress(uncimage, 9)
            blocks = div_roundup(len(image), esp.ESP_FLASH_BLOCK)
            esp.flash_defl_begin(len(uncimage),len(image), address)
        else:
            image = argfile.read()
            calcmd5 = hashlib.md5(image).hexdigest()
            uncsize = len(image)
            blocks = div_roundup(len(image), esp.ESP_FLASH_BLOCK)
            esp.flash_begin(blocks * esp.ESP_FLASH_BLOCK, address)
        argfile.seek(0)  # in case we need it again
        seq = 0
        written = 0
        t = time.time()
        header_block = None
        while len(image) > 0:
            print '\rWriting at 0x%08x... (%d %%)' % (address + seq * esp.ESP_FLASH_BLOCK, 100 * (seq + 1) / blocks),
            sys.stdout.flush()
            block = image[0:esp.ESP_FLASH_BLOCK]
            if args.compress:
                esp.flash_defl_block(block, seq)
            else:
                # Pad the last block
                block = block + '\xff' * (esp.ESP_FLASH_BLOCK - len(block))
                # Fix sflash config data
                if address == 0 and seq == 0 and block[0] == '\xe9':
                    block = block[0:2] + flash_info + block[4:]
                    header_block = block
                esp.flash_block(block, seq)
            image = image[esp.ESP_FLASH_BLOCK:]
            seq += 1
            written += len(block)
        t = time.time() - t
        print '\rWrote %d bytes at 0x%08x in %.1f seconds (%.1f kbit/s)...' % (written, address, t, written / t * 8 / 1000)
        res = esp.flash_md5sum(address, uncsize)
        if res != calcmd5:
            print 'File  md5: %s' % calcmd5
            print 'Flash md5: %s' % res
            raise FatalError("MD5 of file does not match data in flash!")
        else:
            print 'Hash of data verified.'
    print '\nLeaving...'
    if args.flash_mode == 'dio':
        esp.flash_unlock_dio()
    else:
        esp.flash_begin(0, 0)
        if args.compress:
            esp.flash_defl_finish(False)
        else:
            esp.flash_finish(False)
    if args.verify:
        print 'Verifying just-written flash...'
        verify_flash(esp, args, header_block)


def image_info(args):
    image = LoadFirmwareImage(args.chip, args.filename)
    print('Image version: %d' % image.version)
    print('Entry point: %08x' % image.entrypoint) if image.entrypoint != 0 else 'Entry point not set'
    print '%d segments' % len(image.segments)
    print
    checksum = ESPROM.ESP_CHECKSUM_MAGIC
    idx = 0
    for seg in image.segments:
        idx += 1
        print 'Segment %d: %r' % (idx, seg)
        checksum = ESPROM.checksum(seg.data, checksum)
    print
    print 'Checksum: %02x (%s)' % (image.checksum, 'valid' if image.checksum == checksum else 'invalid!')


def make_image(args):
    image = ESPFirmwareImage()
    if len(args.segfile) == 0:
        raise FatalError('No segments specified')
    if len(args.segfile) != len(args.segaddr):
        raise FatalError('Number of specified files does not match number of specified addresses')
    for (seg, addr) in zip(args.segfile, args.segaddr):
        data = file(seg, 'rb').read()
        image.segments.append(ImageSegment(addr, data))
    image.entrypoint = args.entrypoint
    image.save(args.output)


def elf2image(args):
    e = ELFFile(args.input)
    if args.chip == 'auto':  # Default to ESP8266 for backwards compatibility
        print "Creating image for ESP8266..."
        args.chip == 'esp8266'

    if args.chip == 'esp31':
        raise FatalError("No elf2image support for ESP31. Use gen_appimage.py from the ESP31 SDK")
    elif args.chip == 'esp32':
        image = ESP32FirmwareImage()
    elif args.version == '1':  # ESP8266
        image = ESPFirmwareImage()
    else:
        image = OTAFirmwareImage()
    image.entrypoint = e.entrypoint
    image.segments = e.sections  # ELFSection is a subclass of ImageSegment
    image.flash_mode = {'qio':0, 'qout':1, 'dio':2, 'dout': 3}[args.flash_mode]
    image.flash_size_freq = ESP8266ROM.FLASH_SIZES[args.flash_size]
    image.flash_size_freq += {'40m':0, '26m':1, '20m':2, '80m': 0xf}[args.flash_freq]

    if args.output is None:
        args.output = image.default_output_name(args.input)
    image.save(args.output)


def read_mac(esp, args):
    mac = esp.read_mac()
    print 'MAC: %s' % ':'.join(map(lambda x: '%02x' % x, mac))


def chip_id(esp, args):
    chipid = esp.chip_id()
    print 'Chip ID: 0x%08x' % chipid


def erase_flash(esp, args):
    print 'Erasing flash (this may take a while)...'
    flasher = CesantaFlasher(esp, args.baud)
    flasher.flash_erase()
    print 'Erase completed successfully.'


def run(esp, args):
    esp.run()


def flash_id(esp, args):
    flash_id = esp.flash_id()
    print 'Manufacturer: %02x' % (flash_id & 0xff)
    print 'Device: %02x%02x' % ((flash_id >> 8) & 0xff, (flash_id >> 16) & 0xff)


def read_flash(esp, args):
    flasher = CesantaFlasher(esp, args.baud)
    t = time.time()
    data = flasher.flash_read(args.address, args.size, not args.no_progress)
    t = time.time() - t
    print ('\rRead %d bytes at 0x%x in %.1f seconds (%.1f kbit/s)...'
           % (len(data), args.address, t, len(data) / t * 8 / 1000))
    file(args.filename, 'wb').write(data)


def _verify_flash(flasher, args, flash_params=None):
    differences = False
    for address, argfile in args.addr_filename:
        image = argfile.read()
        argfile.seek(0)  # rewind in case we need it again
        if address == 0 and image[0] == '\xe9' and flash_params is not None:
            image = image[0:2] + flash_params + image[4:]
        image_size = len(image)
        print 'Verifying 0x%x (%d) bytes @ 0x%08x in flash against %s...' % (image_size, image_size, address, argfile.name)
        # Try digest first, only read if there are differences.
        digest, _ = flasher.flash_digest(address, image_size)
        digest = hexify(digest).upper()
        expected_digest = hashlib.md5(image).hexdigest().upper()
        if digest == expected_digest:
            print '-- verify OK (digest matched)'
            continue
        else:
            differences = True
            if getattr(args, 'diff', 'no') != 'yes':
                print '-- verify FAILED (digest mismatch)'
                continue

        flash = flasher.flash_read(address, image_size)
        assert flash != image
        diff = [i for i in xrange(image_size) if flash[i] != image[i]]
        print '-- verify FAILED: %d differences, first @ 0x%08x' % (len(diff), address + diff[0])
        for d in diff:
            print '   %08x %02x %02x' % (address + d, ord(flash[d]), ord(image[d]))
    if differences:
        raise FatalError("Verify failed.")


def verify_flash(esp, args, flash_params=None):
    flasher = CesantaFlasher(esp)
    _verify_flash(flasher, args, flash_params)


def version(args):
    print __version__

#
# End of operations functions
#


def main():
    parser = argparse.ArgumentParser(description='esptool.py v%s - ESP8266 ROM Bootloader Utility' % __version__, prog='esptool')

    parser.add_argument('--chip', '-c',
                        help='Target chip type',
                        choices=['auto', 'esp8266', 'esp31', 'esp32'],
                        default=os.environ.get('ESPTOOL_CHIP', 'auto'))

    parser.add_argument(
        '--port', '-p',
        help='Serial port device',
        default=os.environ.get('ESPTOOL_PORT', ESPROM.DEFAULT_PORT))

    parser.add_argument(
        '--baud', '-b',
        help='Serial port baud rate used when flashing/reading',
        type=arg_auto_int,
        default=os.environ.get('ESPTOOL_BAUD', ESPROM.ESP_ROM_BAUD))

    subparsers = parser.add_subparsers(
        dest='operation',
        help='Run esptool {command} -h for additional help')

    parser_load_ram = subparsers.add_parser(
        'load_ram',
        help='Download an image to RAM and execute')
    parser_load_ram.add_argument('filename', help='Firmware image')

    parser_dump_mem = subparsers.add_parser(
        'dump_mem',
        help='Dump arbitrary memory to disk')
    parser_dump_mem.add_argument('address', help='Base address', type=arg_auto_int)
    parser_dump_mem.add_argument('size', help='Size of region to dump', type=arg_auto_int)
    parser_dump_mem.add_argument('filename', help='Name of binary dump')

    parser_read_mem = subparsers.add_parser(
        'read_mem',
        help='Read arbitrary memory location')
    parser_read_mem.add_argument('address', help='Address to read', type=arg_auto_int)

    parser_write_mem = subparsers.add_parser(
        'write_mem',
        help='Read-modify-write to arbitrary memory location')
    parser_write_mem.add_argument('address', help='Address to write', type=arg_auto_int)
    parser_write_mem.add_argument('value', help='Value', type=arg_auto_int)
    parser_write_mem.add_argument('mask', help='Mask of bits to write', type=arg_auto_int)

    def add_spi_flash_subparsers(parent):
        """ Add common parser arguments for SPI flash properties """
        parent.add_argument('--flash_freq', '-ff', help='SPI Flash frequency',
                            choices=['40m', '26m', '20m', '80m'],
                            default=os.environ.get('ESPTOOL_FF', '40m'))
        parent.add_argument('--flash_mode', '-fm', help='SPI Flash mode',
                            choices=['qio', 'qout', 'dio', 'dout'],
                            default=os.environ.get('ESPTOOL_FM', 'qio'))
        parent.add_argument('--flash_size', '-fs', help='SPI Flash size in MegaBytes (1MB, 2MB, 4MB, 8MB, 16M)'
                            ' plus ESP8266-only (256KB, 512KB, 2MB-c1, 4MB-c1, 4MB-2)',
                            action=FlashSizeAction,
                            default=os.environ.get('ESPTOOL_FS', '1MB'))

    parser_write_flash = subparsers.add_parser(
        'write_flash',
        help='Write a binary blob to flash')
    parser_write_flash.add_argument('addr_filename', metavar='<address> <filename>', help='Address followed by binary filename, separated by space',
                                    action=AddrFilenamePairAction)
    add_spi_flash_subparsers(parser_write_flash)
    parser_write_flash.add_argument('--no-progress', '-p', help='Suppress progress output', action="store_true")
    parser_write_flash.add_argument('--verify', help='Verify just-written data (only necessary if very cautious, data is already CRCed', action='store_true')
    parser_write_flash.add_argument('--ucIsHspi', '-ih', help='Config SPI PORT/PINS (Espressif internal feature)',default='0')
    parser_write_flash.add_argument('--ucIsLegacy', '-il', help='Config SPI LEGACY (Espressif internal feature)',default='0')
    parser_write_flash.add_argument('--compress', '-z', help='Compress data in transfer',action="store_true")

    subparsers.add_parser(
        'run',
        help='Run application code in flash')

    parser_image_info = subparsers.add_parser(
        'image_info',
        help='Dump headers from an application image')
    parser_image_info.add_argument('filename', help='Image file to parse')

    parser_make_image = subparsers.add_parser(
        'make_image',
        help='Create an application image from binary files')
    parser_make_image.add_argument('output', help='Output image file')
    parser_make_image.add_argument('--segfile', '-f', action='append', help='Segment input file')
    parser_make_image.add_argument('--segaddr', '-a', action='append', help='Segment base address', type=arg_auto_int)
    parser_make_image.add_argument('--entrypoint', '-e', help='Address of entry point', type=arg_auto_int, default=0)

    parser_elf2image = subparsers.add_parser(
        'elf2image',
        help='Create an application image from ELF file')
    parser_elf2image.add_argument('input', help='Input ELF file')
    parser_elf2image.add_argument('--output', '-o', help='Output filename prefix (for version 1 image), or filename (for version 2 single image)', type=str)
    parser_elf2image.add_argument('--version', '-e', help='Output image version', choices=['1','2'], default='1')
    add_spi_flash_subparsers(parser_elf2image)

    subparsers.add_parser(
        'read_mac',
        help='Read MAC address from OTP ROM')

    subparsers.add_parser(
        'chip_id',
        help='Read Chip ID from OTP ROM')

    subparsers.add_parser(
        'flash_id',
        help='Read SPI flash manufacturer and device ID')

    parser_read_flash = subparsers.add_parser(
        'read_flash',
        help='Read SPI flash content')
    parser_read_flash.add_argument('address', help='Start address', type=arg_auto_int)
    parser_read_flash.add_argument('size', help='Size of region to dump', type=arg_auto_int)
    parser_read_flash.add_argument('filename', help='Name of binary dump')
    parser_read_flash.add_argument('--no-progress', '-p', help='Suppress progress output', action="store_true")

    parser_verify_flash = subparsers.add_parser(
        'verify_flash',
        help='Verify a binary blob against flash')
    parser_verify_flash.add_argument('addr_filename', help='Address and binary file to verify there, separated by space',
                                     action=AddrFilenamePairAction)
    parser_verify_flash.add_argument('--diff', '-d', help='Show differences',
                                     choices=['no', 'yes'], default='no')

    subparsers.add_parser(
        'erase_flash',
        help='Perform Chip Erase on SPI flash')

    subparsers.add_parser(
        'version', help='Print esptool version')

    # internal sanity check - every operation matches a module function of the same name
    for operation in subparsers.choices.keys():
        assert operation in globals(), "%s should be a module function" % operation

    args = parser.parse_args()

    print 'esptool.py v%s' % __version__

    # operation function can take 1 arg (args), 2 args (esp, arg)
    # or be a member function of the ESPROM class.

    operation_func = globals()[args.operation]
    operation_args,_,_,_ = inspect.getargspec(operation_func)
    if operation_args[0] == 'esp':  # operation function takes an ESPROM connection object
        initial_baud = min(ESPROM.ESP_ROM_BAUD, args.baud)  # don't sync faster than the default baud rate
        chip_constructor_fun = {
            'auto': ESPROM.detect_chip,
            'esp8266': ESP8266ROM,
            'esp31': ESP31ROM,
            'esp32': ESP32ROM,
        }[args.chip]
        esp = chip_constructor_fun(args.port, initial_baud)
        operation_func(esp, args)
    else:
        operation_func(args)


class FlashSizeAction(argparse.Action):
    """ Custom flash size parser class to support backwards compatibility with megabit size arguments.

    (At next major relase, remove deprecated sizes and this can become a 'normal' choices= argument again.)
    """
    def __init__(self, option_strings, dest, nargs=1, **kwargs):
        super(FlashSizeAction, self).__init__(option_strings, dest, nargs, **kwargs)

    def __call__(self, parser, namespace, values, option_string=None):
        try:
            value = {
                '2m': '256KB',
                '4m': '512KB',
                '8m': '1MB',
                '16m': '2MB',
                '32m': '4MB',
                '16m-c1': '2MB-c1',
                '32m-c1': '4MB-c1',
                '32m-c2': '4MB-c2'
            }[values[0]]
            print("WARNING: Flash size arguments in megabits like '%s' are deprecated." % (values[0]))
            print("Please use the equivalent size '%s'." % (value))
            print("Megabit arguments may be removed in a future release.")
        except KeyError:
            values = values[0]

        known_sizes = dict(ESP8266ROM.FLASH_SIZES)
        known_sizes.update(ESP32ROM.FLASH_SIZES)
        if value not in known_sizes:
            raise argparse.ArgumentError(self, '%s is not a known flash size. Known sizes: %s' % (value, ", ".join(known_sizes.keys())))
        setattr(namespace, self.dest, value)


class AddrFilenamePairAction(argparse.Action):
    """ Custom parser class for the address/filename pairs passed as arguments """
    def __init__(self, option_strings, dest, nargs='+', **kwargs):
        super(AddrFilenamePairAction, self).__init__(option_strings, dest, nargs, **kwargs)

    def __call__(self, parser, namespace, values, option_string=None):
        # validate pair arguments
        pairs = []
        for i in range(0,len(values),2):
            try:
                address = int(values[i],0)
            except ValueError as e:
                raise argparse.ArgumentError(self,'Address "%s" must be a number' % values[i])
            try:
                argfile = open(values[i + 1], 'rb')
            except IOError as e:
                raise argparse.ArgumentError(self, e)
            except IndexError:
                raise argparse.ArgumentError(self,'Must be pairs of an address and the binary filename to write there')
            pairs.append((address, argfile))
        setattr(namespace, self.dest, pairs)

# This is "wrapped" stub_flasher.c, to  be loaded using run_stub.
_CESANTA_FLASHER_STUB = """\
{"code_start": 1074790404, "code": "080000601C000060000000601000006031FCFF71FCFF\
81FCFFC02000680332D218C020004807404074DCC48608005823C0200098081BA5A9239245005803\
1B555903582337350129230B446604DFC6F3FF21EEFFC0200069020DF0000000010078480040004A\
0040B449004012C1F0C921D911E901DD0209312020B4ED033C2C56C2073020B43C3C56420701F5FF\
C000003C4C569206CD0EEADD860300202C4101F1FFC0000056A204C2DCF0C02DC0CC6CCAE2D1EAFF\
0606002030F456D3FD86FBFF00002020F501E8FFC00000EC82D0CCC0C02EC0C73DEB2ADC46030020\
2C4101E1FFC00000DC42C2DCF0C02DC056BCFEC602003C5C8601003C6C4600003C7C08312D0CD811\
C821E80112C1100DF0000C180000140010400C0000607418000064180000801800008C1800008418\
0000881800009018000018980040880F0040A80F0040349800404C4A0040740F0040800F0040980F\
00400099004012C1E091F5FFC961CD0221EFFFE941F9310971D9519011C01A223902E2D1180C0222\
6E1D21E4FF31E9FF2AF11A332D0F42630001EAFFC00000C030B43C2256A31621E1FF1A2228022030\
B43C3256B31501ADFFC00000DD023C4256ED1431D6FF4D010C52D90E192E126E0101DDFFC0000021\
D2FF32A101C020004802303420C0200039022C0201D7FFC00000463300000031CDFF1A333803D023\
C03199FF27B31ADC7F31CBFF1A3328030198FFC0000056C20E2193FF2ADD060E000031C6FF1A3328\
030191FFC0000056820DD2DD10460800000021BEFF1A2228029CE231BCFFC020F51A33290331BBFF\
C02C411A332903C0F0F4222E1D22D204273D9332A3FFC02000280E27B3F721ABFF381E1A2242A400\
01B5FFC00000381E2D0C42A40001B3FFC0000056120801B2FFC00000C02000280EC2DC0422D2FCC0\
2000290E01ADFFC00000222E1D22D204226E1D281E22D204E7B204291E860000126E012198FF32A0\
042A21C54C003198FF222E1D1A33380337B202C6D6FF2C02019FFFC000002191FF318CFF1A223A31\
019CFFC00000218DFF1C031A22C549000C02060300003C528601003C624600003C72918BFF9A1108\
71C861D851E841F83112C1200DF00010000068100000581000007010000074100000781000007C10\
0000801000001C4B0040803C004091FDFF12C1E061F7FFC961E941F9310971D9519011C01A662906\
21F3FFC2D1101A22390231F2FF0C0F1A33590331EAFFF26C1AED045C2247B3028636002D0C016DFF\
C0000021E5FF41EAFF2A611A4469040622000021E4FF1A222802F0D2C0D7BE01DD0E31E0FF4D0D1A\
3328033D0101E2FFC00000561209D03D2010212001DFFFC000004D0D2D0C3D01015DFFC0000041D5\
FFDAFF1A444804D0648041D2FF1A4462640061D1FF106680622600673F1331D0FF10338028030C43\
853A002642164613000041CAFF222C1A1A444804202FC047328006F6FF222C1A273F3861C2FF222C\
1A1A6668066732B921BDFF3D0C1022800148FFC0000021BAFF1C031A2201BFFFC000000C02460300\
5C3206020000005C424600005C5291B7FF9A110871C861D851E841F83112C1200DF0B0100000C010\
0000D010000012C1E091FEFFC961D951E9410971F931CD039011C0ED02DD0431A1FF9C1422A06247\
B302062D0021F4FF1A22490286010021F1FF1A223902219CFF2AF12D0F011FFFC00000461C0022D1\
10011CFFC0000021E9FFFD0C1A222802C7B20621E6FF1A22F8022D0E3D014D0F0195FFC000008C52\
22A063C6180000218BFF3D01102280F04F200111FFC00000AC7D22D1103D014D0F010DFFC0000021\
D6FF32D110102280010EFFC0000021D3FF1C031A220185FFC00000FAEEF0CCC056ACF821CDFF317A\
FF1A223A310105FFC0000021C9FF1C031A22017CFFC000002D0C91C8FF9A110871C861D851E841F8\
3112C1200DF0000200600000001040020060FFFFFF0012C1E00C02290131FAFF21FAFF026107C961\
C02000226300C02000C80320CC10564CFF21F5FFC02000380221F4FF20231029010C432D010163FF\
C0000008712D0CC86112C1200DF00080FE3F8449004012C1D0C9A109B17CFC22C1110C13C51C0026\
1202463000220111C24110B68202462B0031F5FF3022A02802A002002D011C03851A0066820A2801\
32210105A6FF0607003C12C60500000010212032A01085180066A20F2221003811482105B3FF2241\
10861A004C1206FDFF2D011C03C5160066B20E280138114821583185CFFF06F7FF005C1286F5FF00\
10212032A01085140066A20D2221003811482105E1FF06EFFF0022A06146EDFF45F0FFC6EBFF0000\
01D2FFC0000006E9FF000C022241100C1322C110C50F00220111060600000022C1100C13C50E0022\
011132C2FA303074B6230206C8FF08B1C8A112C1300DF0000000000010404F484149007519031027\
000000110040A8100040BC0F0040583F0040CC2E00401CE20040D83900408000004021F4FF12C1E0\
C961C80221F2FF097129010C02D951C91101F4FFC0000001F3FFC00000AC2C22A3E801F2FFC00000\
21EAFFC031412A233D0C01EFFFC000003D0222A00001EDFFC00000C1E4FF2D0C01E8FFC000002D01\
32A004450400C5E7FFDD022D0C01E3FFC00000666D1F4B2131DCFF4600004B22C0200048023794F5\
31D9FFC0200039023DF08601000001DCFFC000000871C861D85112C1200DF000000012C1F0026103\
01EAFEC00000083112C1100DF000643B004012C1D0E98109B1C9A1D991F97129013911E2A0C001FA\
FFC00000CD02E792F40C0DE2A0C0F2A0DB860D00000001F4FFC00000204220E71240F7921C226102\
01EFFFC0000052A0DC482157120952A0DD571205460500004D0C3801DA234242001BDD3811379DC5\
C6000000000C0DC2A0C001E3FFC00000C792F608B12D0DC8A1D891E881F87112C1300DF00000", "\
entry": 1074792180, "num_params": 1, "params_start": 1074790400, "data": "FE0510\
401A0610403B0610405A0610407A061040820610408C0610408C061040", "data_start": 10736\
43520}
"""

if __name__ == '__main__':
    try:
        main()
    except FatalError as e:
        print '\nA fatal error occurred: %s' % e
        sys.exit(2)
