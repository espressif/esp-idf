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

""" example of writing test with TinyTestFW """
import re
import os
import sys

# if we want to run test case outside `tiny-test-fw` folder,
# we need to insert tiny-test-fw path into sys path
test_fw_path = os.getenv("TEST_FW_PATH")
if test_fw_path and test_fw_path not in sys.path:
    sys.path.insert(0, test_fw_path)

import TinyFW
import IDF


@IDF.idf_example_test(env_tag="Example_SDIO")
def test_example_sdio_communication(env, extra_data):
    """
    Configurations
    dut1 = host -> dut2 = slave
    should be in the same group of devices, otherwise may meet download issue
    group1: (Wroom-32 Series or PICO-D4 modules: PICO-Kit, DevKitC, WroverKit v2 or earlier)
    group2: (Wrover module: WroverKit v3)

    GPIO14->GPIO14
    GPIO15->GPIO15
    GPIO2->GPIO2
    GPIO4->GPIO4
    GND->GND

    VDD3.3 -> GPIO13 if dut2 uses WroverKit v3

    or use sdio test board, which has two wrover modules connect to a same FT3232
    Assume that first dut is host and second is slave
    """
    dut1 = env.get_dut("sdio_host", "examples/peripherals/sdio/host")
    dut2 = env.get_dut("sdio_slave", "examples/peripherals/sdio/slave")
    dut1.start_app()
    #wait until the master is ready to setup the slave
    dut1.expect("host ready, start initializing slave...")

    dut2.start_app()
    dut1.expect("0a 0d 10 13 16 19 1c 1f  22 25 28 2b 2e 31 34 37")
    dut1.expect("3a 3d 40 43 46 49 4c 4f  52 55 58 5b 00 00 00 00")
    dut1.expect("6a 6d 70 73 76 79 7c 7f  82 85 88 8b 8e 91 94 97")
    dut1.expect("9a 9d a0 a3 a6 a9 ac af  b2 b5 b8 bb be c1 c4 c7")

    dut2.expect("================ JOB_WRITE_REG ================")
    dut2.expect("0a 0d 10 13 16 19 1c 1f  22 25 28 2b 2e 31 34 37")
    dut2.expect("3a 3d 40 43 46 49 4c 4f  52 55 58 5b 00 00 00 00")
    dut2.expect("6a 6d 70 73 76 79 7c 7f  82 85 88 8b 8e 91 94 97")
    dut2.expect("9a 9d a0 a3 a6 a9 ac af  b2 b5 b8 bb be c1 c4 c7")

    dut1.expect("host int: 0")
    dut1.expect("host int: 1")
    dut1.expect("host int: 2")
    dut1.expect("host int: 3")
    dut1.expect("host int: 4")
    dut1.expect("host int: 5")
    dut1.expect("host int: 6")
    dut1.expect("host int: 7")
    dut1.expect("host int: 0")
    dut1.expect("host int: 1")
    dut1.expect("host int: 2")
    dut1.expect("host int: 3")
    dut1.expect("host int: 4")
    dut1.expect("host int: 5")
    dut1.expect("host int: 6")
    dut1.expect("host int: 7")

    dut2.expect("================ JOB_SEND_INT ================")
    dut2.expect("================ JOB_SEND_INT ================")

    dut1.expect("send packet length: 3")
    dut1.expect("send packet length: 6")
    dut1.expect("send packet length: 12")
    dut1.expect("send packet length: 128")
    dut1.expect("send packet length: 511")
    dut1.expect("send packet length: 512")

    dut2.expect("recv len: 3")
    dut2.expect("recv len: 6")
    dut2.expect("recv len: 12")
    dut2.expect("recv len: 128")
    #511
    dut2.expect("recv len: 128")
    dut2.expect("recv len: 128")
    dut2.expect("recv len: 128")
    dut2.expect("recv len: 127")
    #512
    dut2.expect("recv len: 128")
    dut2.expect("recv len: 128")
    dut2.expect("recv len: 128")
    dut2.expect("recv len: 128")

    dut1.expect("receive data, size: 3")
    dut1.expect("receive data, size: 6")
    dut1.expect("receive data, size: 12")
    dut1.expect("receive data, size: 128")

    dut1.expect("receive data, size: 128")
    dut1.expect("receive data, size: 128")
    dut1.expect("receive data, size: 128")
    dut1.expect("receive data, size: 127")

    dut1.expect("receive data, size: 128")
    dut1.expect("receive data, size: 128")
    dut1.expect("receive data, size: 128")
    dut1.expect("receive data, size: 128")

    #the last valid line of one round
    dut1.expect("ce d3 d8 dd e2 e7 ec f1  f6 fb 00 05 0a 0f 14 19")
    #the first 2 lines of the second round
    dut1.expect("46 4b 50")
    dut1.expect("5a 5f 64 69 6e 73")


if __name__ == '__main__':
    TinyFW.set_default_config(env_config_file="EnvConfigTemplate.yml", dut=IDF.IDFDUT)
    test_example_sdio_communication()
