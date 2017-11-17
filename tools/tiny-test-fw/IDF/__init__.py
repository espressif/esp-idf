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

import TinyFW
from IDF.IDFApp import Example, UT
from IDF.IDFDUT import IDFDUT


def idf_example_test(app=Example, dut=IDFDUT, chip="ESP32",
                     module="examples", execution_time=1,
                     **kwargs):
    """
    decorator for testing idf examples (with default values for some keyword args).

    :param app: test application class
    :param dut: dut class
    :param chip: chip supported, string or tuple
    :param module: module, string
    :param execution_time: execution time in minutes, int
    :param kwargs: other keyword args
    :return: test method
    """
    # not use partial function as define as function support auto generating document
    return TinyFW.test_method(app=app, dut=dut, chip=chip, module=module,
                              execution_time=execution_time, **kwargs)
