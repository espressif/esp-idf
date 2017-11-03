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
import re
import subprocess
import functools

import DUT


class IDFToolError(OSError):
    pass


def _tool_method(func):
    """ close port, execute tool method and then reopen port """
    @functools.wraps(func)
    def handler(self, *args, **kwargs):
        self.close()
        ret = func(self, *args, **kwargs)
        self.open()
        return ret
    return handler


class IDFDUT(DUT.SerialDUT):
    """ IDF DUT, extends serial with ESPTool methods """

    CHIP_TYPE_PATTERN = re.compile(r"Detecting chip type[.:\s]+(.+)")

    def __init__(self, name, port, log_file, app, **kwargs):
        self.download_config, self.partition_table = app.process_app_info()
        super(IDFDUT, self).__init__(name, port, log_file, app, **kwargs)

    @classmethod
    def get_chip(cls, app, port):
        """
        get chip id via esptool

        :param app: application instance (to get tool)
        :param port: comport
        :return: chip ID or None
        """
        try:
            output = subprocess.check_output(["python", app.esptool, "--port", port, "chip_id"])
        except subprocess.CalledProcessError:
            output = bytes()
        if isinstance(output, bytes):
            output = output.decode()
        chip_type = cls.CHIP_TYPE_PATTERN.search(output)
        return chip_type.group(1) if chip_type else None

    @classmethod
    def confirm_dut(cls, port, app, **kwargs):
        return cls.get_chip(app, port) is not None

    @_tool_method
    def start_app(self):
        """
        download and start app.

        :return: None
        """
        retry_baud_rates = ["921600", "115200"]
        error = IDFToolError()
        for baud_rate in retry_baud_rates:
            try:
                subprocess.check_output(["python", self.app.esptool,
                                         "--port", self.port, "--baud", baud_rate]
                                        + self.download_config)
                break
            except subprocess.CalledProcessError as error:
                continue
        else:
            raise error

    @_tool_method
    def reset(self):
        """
        reset DUT with esptool

        :return: None
        """
        subprocess.check_output(["python", self.app.esptool, "--port", self.port, "run"])

    @_tool_method
    def dump_flush(self, output_file, **kwargs):
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
            partition = self.partition_table[kwargs["partition"]]
            _address = partition["offset"]
            _size = partition["size"]
        elif "address" in kwargs and "size" in kwargs:
            _address = kwargs["address"]
            _size = kwargs["size"]
        else:
            raise IDFToolError("You must specify 'partition' or ('address' and 'size') to dump flash")
        subprocess.check_output(
            ["python", self.app.esptool, "--port", self.port, "--baud", "921600",
             "--before", "default_reset", "--after", "hard_reset", "read_flash",
             _address, _size, output_file]
        )
