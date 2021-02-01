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

import re


class LineMatcher(object):
    """
    Assembles a dictionary of filtering rules based on the --print_filter
    argument of idf_monitor. Then later it is used to match lines and
    determine whether they should be shown on screen or not.
    """
    LEVEL_N = 0
    LEVEL_E = 1
    LEVEL_W = 2
    LEVEL_I = 3
    LEVEL_D = 4
    LEVEL_V = 5

    level = {'N': LEVEL_N, 'E': LEVEL_E, 'W': LEVEL_W, 'I': LEVEL_I, 'D': LEVEL_D,
             'V': LEVEL_V, '*': LEVEL_V, '': LEVEL_V}

    def __init__(self, print_filter):
        # type: (str) -> None
        self._dict = dict()
        self._re = re.compile(r'^(?:\033\[[01];?[0-9]+m?)?([EWIDV]) \([0-9]+\) ([^:]+): ')
        items = print_filter.split()
        if len(items) == 0:
            self._dict['*'] = self.LEVEL_V  # default is to print everything
        for f in items:
            s = f.split(r':')
            if len(s) == 1:
                # specifying no warning level defaults to verbose level
                lev = self.LEVEL_V
            elif len(s) == 2:
                if len(s[0]) == 0:
                    raise ValueError('No tag specified in filter ' + f)
                try:
                    lev = self.level[s[1].upper()]
                except KeyError:
                    raise ValueError('Unknown warning level in filter ' + f)
            else:
                raise ValueError('Missing ":" in filter ' + f)
            self._dict[s[0]] = lev

    def match(self, line):
        # type: (str) -> bool
        try:
            m = self._re.search(line)
            if m:
                lev = self.level[m.group(1)]
                if m.group(2) in self._dict:
                    return self._dict[m.group(2)] >= lev
                return self._dict.get('*', self.LEVEL_N) >= lev
        except (KeyError, IndexError):
            # Regular line written with something else than ESP_LOG*
            # or an empty line.
            pass
        # We need something more than "*.N" for printing.
        return self._dict.get('*', self.LEVEL_N) > self.LEVEL_N
