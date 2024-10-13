# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os
import sys

tools_dir = os.path.realpath(os.path.join(os.path.dirname(__file__), '..', '..'))
if tools_dir not in sys.path:
    sys.path.append(tools_dir)
