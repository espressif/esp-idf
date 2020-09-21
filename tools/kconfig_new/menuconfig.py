# Copyright 2020 Espressif Systems (Shanghai) CO LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import print_function
from __future__ import unicode_literals
from distutils.sysconfig import get_python_lib
import os
import subprocess
import sys


def main(args):
    """
    Helper function to invoke menuconfig.py from the kconfiglib package. It invokes menuconfig as a process because:
    1. kconfiblib/menuconfig.py invoked directly has advantage over calling menuconfig() - Kconfiglib object needs to
       be prepared but it still reads the sdkconfig file,
    2. command line invoked from make/cmake can be the same for menuconfig.py and mconf-idf.
    """
    cmd = [sys.executable]

    # path where menuconfig is installed from the kconfiglib package:
    cmd += [os.path.join(get_python_lib(), 'menuconfig.py')]

    # args is expected to be [ '$IDF_PATH/Kconfig' ]:
    cmd += args

    print('Running', ' '.join(cmd))
    subprocess.check_call(cmd)


if __name__ == '__main__':
    main(sys.argv[1:])
