# Copyright 2015-2021 Espressif Systems (Shanghai) CO LTD
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

# Control-key characters
CTRL_A = '\x01'
CTRL_B = '\x02'
CTRL_F = '\x06'
CTRL_H = '\x08'
CTRL_R = '\x12'
CTRL_T = '\x14'
CTRL_Y = '\x19'
CTRL_P = '\x10'
CTRL_X = '\x18'
CTRL_L = '\x0c'
CTRL_RBRACKET = '\x1d'  # Ctrl+]

# Command parsed from console inputs
CMD_STOP = 1
CMD_RESET = 2
CMD_MAKE = 3
CMD_APP_FLASH = 4
CMD_OUTPUT_TOGGLE = 5
CMD_TOGGLE_LOGGING = 6
CMD_ENTER_BOOT = 7

# Tags for tuples in queues
TAG_KEY = 0
TAG_SERIAL = 1
TAG_SERIAL_FLUSH = 2
TAG_CMD = 3

__version__ = '1.1'
