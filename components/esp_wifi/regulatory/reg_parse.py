#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2008 Luis R. Rodriguez <mcgrof@gmail.com>
# SPDX-FileCopyrightText: 2008 Johannes Berg <johannes@sipsolutions.net>
# SPDX-FileCopyrightText: 2008 Michael Green <Michael.Green@Atheros.com>
#
# SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# Copyright (c) 2008, Luis R. Rodriguez <mcgrof@gmail.com>
# Copyright (c) 2008, Johannes Berg <johannes@sipsolutions.net>
# Copyright (c) 2008, Michael Green <Michael.Green@Atheros.com>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.

# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

import math
import sys
from collections import OrderedDict
from collections import defaultdict
from functools import total_ordering
from math import ceil
from typing import Callable
from typing import Dict
from typing import List
from typing import Optional
from typing import TextIO
from typing import Tuple

flag_definitions: Dict[str, int] = {
    'NO-OFDM': 1 << 0,
    'NO-CCK': 1 << 1,
    'NO-INDOOR': 1 << 2,
    'NO-OUTDOOR': 1 << 3,
    'DFS': 1 << 4,
    'PTP-ONLY': 1 << 5,
    'PTMP-ONLY': 1 << 6,
    'NO-IR': 1 << 7,
    # hole at bit 8
    # hole at bit 9. FIXME: Where is NO-HT40 defined?
    'NO-HT40': 1 << 10,
    'AUTO-BW': 1 << 11,
}

dfs_regions: Dict[str, int] = {
    'DFS-FCC': 1,
    'DFS-ETSI': 2,
    'DFS-JP': 3,
}

typical_regdomain: Dict[str, List[str]] = {
    'DEFAULT': ['01'],
    'CE': [
        'AT',
        'BE',
        'BG',
        'CH',
        'CY',
        'CZ',
        'DE',
        'DK',
        'EE',
        'ES',
        'FI',
        'FR',
        'GR',
        'HU',
        'IE',
        'IS',
        'IT',
        'LI',
        'LT',
        'LU',
        'LV',
        'MT',
        'NL',
        'NO',
        'PL',
        'PT',
        'RO',
        'SE',
        'SI',
        'SK',
    ],
    'ACMA': ['AU'],
    'ANATEL': ['BR'],
    'ISED': ['CA'],
    'SRRC': ['CN'],
    'OFCA': ['HK'],
    'WPC': ['IN'],
    'MIC': ['JP'],
    'KCC': ['KR'],
    'IFETEL': ['MX'],
    'RCM': ['NZ'],
    'NCC': ['TW'],
    'FCC': ['US'],
}


@total_ordering
class WmmRule(object):
    def __init__(self, vo_c: int, vi_c: int, be_c: int, bk_c: int, vo_ap: int, vi_ap: int, be_ap: int, bk_ap: int):
        self.vo_c = vo_c
        self.vi_c = vi_c
        self.be_c = be_c
        self.bk_c = bk_c
        self.vo_ap = vo_ap
        self.vi_ap = vi_ap
        self.be_ap = be_ap
        self.bk_ap = bk_ap

    def _as_tuple(self) -> Tuple[int, int, int, int, int, int, int, int]:
        return (self.vo_c, self.vi_c, self.be_c, self.bk_c, self.vo_ap, self.vi_ap, self.be_ap, self.bk_ap)

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, WmmRule):
            return False
        return self._as_tuple() == other._as_tuple()

    def __ne__(self, other: object) -> bool:
        return not (self == other)

    def __lt__(self, other: object) -> bool:
        if not isinstance(other, WmmRule):
            return False
        return self._as_tuple() < other._as_tuple()

    def __hash__(self) -> int:
        return hash(self._as_tuple())


class FreqBand(object):
    def __init__(self, start: float, end: float, bw: float, comments: Optional[List[str]] = None):
        self.start = start
        self.end = end
        self.maxbw = bw
        self.comments = comments or []

    def _as_tuple(self) -> Tuple[float, float, float]:
        return (self.start, self.end, self.maxbw)

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, FreqBand):
            return False
        return self._as_tuple() == other._as_tuple()

    def __ne__(self, other: object) -> bool:
        return not (self == other)

    def __lt__(self, other: object) -> bool:
        if not isinstance(other, FreqBand):
            return False
        return self._as_tuple() < other._as_tuple()

    def __hash__(self) -> int:
        return hash(self._as_tuple())

    def __str__(self) -> str:
        return '<FreqBand %.3f - %.3f @ %.3f>' % (self.start, self.end, self.maxbw)


@total_ordering
class PowerRestriction(object):
    def __init__(self, max_ant_gain: float, max_eirp: float, comments: Optional[List[str]] = None):
        self.max_ant_gain = max_ant_gain
        self.max_eirp = max_eirp
        self.comments = comments or []

    def _as_tuple(self) -> Tuple[float, float]:
        return (self.max_ant_gain, self.max_eirp)

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, PowerRestriction):
            return False
        return self._as_tuple() == other._as_tuple()

    def __ne__(self, other: object) -> bool:
        return not (self == other)

    def __lt__(self, other: object) -> bool:
        if not isinstance(other, PowerRestriction):
            return False
        return self._as_tuple() < other._as_tuple()

    def __hash__(self) -> int:
        return hash(self._as_tuple())

    def __str__(self) -> str:
        return '<PowerRestriction ...>'


class DFSRegionError(Exception):
    def __init__(self, dfs_region: str):
        self.dfs_region = dfs_region


class FlagError(Exception):
    def __init__(self, flag: str):
        self.flag = flag


@total_ordering
class Permission(object):
    def __init__(self, freqband: FreqBand, power: PowerRestriction, flags: List[str], wmmrule: Optional[WmmRule]):
        assert isinstance(freqband, FreqBand)
        assert isinstance(power, PowerRestriction) or power is None
        assert isinstance(wmmrule, WmmRule) or wmmrule is None
        self.freqband = freqband
        self.power = power
        self.wmmrule = wmmrule
        self.flags = 0
        self.dfs = 0
        for flag in flags:
            if flag not in flag_definitions:
                raise FlagError(flag)
            self.flags |= flag_definitions[flag]
        self.textflags = flags
        if self.flags & (1 << 4):
            self.dfs = 1

    def _as_tuple(self) -> Tuple[FreqBand, PowerRestriction, int, Optional[WmmRule]]:
        return (self.freqband, self.power, self.flags, self.wmmrule)

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, Permission):
            return False
        return self._as_tuple() == other._as_tuple()

    def __ne__(self, other: object) -> bool:
        return not (self == other)

    def __lt__(self, other: object) -> bool:
        if not isinstance(other, Permission):
            return False
        return self._as_tuple() < other._as_tuple()

    def __hash__(self) -> int:
        return hash(self._as_tuple())

    def __str__(self) -> str:
        return str(self.freqband) + str(self.power) + str(self.wmmrule)


class Country(object):
    def __init__(
        self,
        dfs_region: str,
        permissions: Optional[List[Permission]] = None,
        permissions_2g: Optional[List[Permission]] = None,
        comments: Optional[List[str]] = None,
    ):
        self._permissions = permissions or []
        self._permissions_2g = permissions_2g or []
        self.comments = comments or []
        self.dfs_region = 0

        if dfs_region:
            if dfs_region not in dfs_regions:
                raise DFSRegionError(dfs_region)
            self.dfs_region = dfs_regions[dfs_region]

    def add(self, perm: Permission) -> None:
        assert isinstance(perm, Permission)
        self._permissions.append(perm)
        self._permissions.sort()

    def add_2g(self, perm: Permission) -> None:
        assert isinstance(perm, Permission)
        self._permissions_2g.append(perm)
        self._permissions_2g.sort()

    def __contains__(self, perm: Permission) -> bool:
        assert isinstance(perm, Permission)
        return perm in self._permissions

    def __str__(self) -> str:
        r = ['(%s, %s)' % (str(permission.freqband), str(permission.power)) for permission in self._permissions]
        return '<Country (%s)>' % (', '.join(r))

    def _get_permissions_tuple(self) -> Tuple[Permission, ...]:
        return tuple(self._permissions)

    def _get_permissions_2g_tuple(self) -> Tuple[Permission, ...]:
        return tuple(self._permissions_2g)

    permissions = property(_get_permissions_tuple)
    permissions_2g = property(_get_permissions_2g_tuple)


class MySyntaxError(Exception):
    pass


class DBParser(object):
    def __init__(self, warn: Optional[Callable[[str], None]] = None) -> None:
        self._warn_callout = warn or sys.stderr.write
        self._lineno: int = 0
        self._comments: List[str] = []
        self._banddup: Dict[str, str] = {}
        self._bandrev: Dict[FreqBand, str] = {}
        self._bands: Dict[str, FreqBand] = {}
        self._bandline: Dict[str, int] = {}
        self._powerdup: Dict[str, str] = {}
        self._powerrev: Dict[PowerRestriction, str] = {}
        self._power: Dict[str, PowerRestriction] = {}
        self._powerline: Dict[str, int] = {}
        self._wmm_rules: Dict[str, OrderedDict] = defaultdict(lambda: OrderedDict())
        self._countries: Dict[bytes, Country] = {}
        self._bands_used: Dict[str, bool] = {}
        self._power_used: Dict[str, bool] = {}
        self._current_countries: Optional[Dict[bytes, Country]] = None
        self._current_regions: Optional[Dict[str, int]] = None
        self._channel_list: List[int] = [
            1,
            2,
            3,
            4,
            5,
            6,
            7,
            8,
            9,
            10,
            11,
            12,
            13,
            14,
            36,
            40,
            44,
            48,
            52,
            56,
            60,
            64,
            100,
            104,
            108,
            112,
            116,
            120,
            124,
            128,
            132,
            136,
            140,
            144,
            149,
            153,
            157,
            161,
            165,
            169,
            173,
            177,
        ]

    def _syntax_error(self, txt: Optional[str] = None) -> None:
        txt = txt and ' (%s)' % txt or ''
        raise MySyntaxError('Syntax error in line %d%s' % (self._lineno, txt))

    def _warn(self, txt: str) -> None:
        self._warn_callout('Warning (line %d): %s\n' % (self._lineno, txt))

    def channel_to_freq(self, channel: int) -> int:
        if channel == 14:
            return 2484
        elif channel < 14:
            return 2407 + channel * 5
        elif channel >= 182 and channel <= 196:
            return 4000 + channel * 5
        else:
            return 5000 + channel * 5

    def find_channel(self, start_freq: float, end_freq: float) -> Tuple:
        start_channel = 0
        end_channel = 0
        for channel in self._channel_list:
            channel_center_freq = self.channel_to_freq(channel)
            if (
                (channel_center_freq >= start_freq and channel_center_freq <= end_freq)
                and (channel_center_freq - 10 >= start_freq and channel_center_freq - 10 <= end_freq)
                and (channel_center_freq + 10 >= start_freq and channel_center_freq + 10 <= end_freq)
            ):
                if start_channel == 0:
                    start_channel = channel
                end_channel = channel
        return (start_channel, end_channel)

    def convert_maxbandwidth(self, bw: float) -> int:
        maxbw: int = 0
        if bw == 20:
            maxbw = 1
        elif bw == 40:
            maxbw = 2
        elif bw == 80:
            maxbw = 3
        elif bw == 160:
            maxbw = 4
        else:
            maxbw = -1
        return maxbw

    def _parse_band_def(self, bname: str, banddef: str, dupwarn: bool = True) -> None:
        try:
            freqs, bw_str = banddef.split('@')
            bw = float(bw_str)
        except ValueError:
            bw = 20.0

        try:
            start_str, end_str = freqs.split('-')
            start = float(start_str)
            end = float(end_str)
            if start <= 0:
                self._syntax_error('Invalid start freq (%d)' % start)
            if end <= 0:
                self._syntax_error('Invalid end freq (%d)' % end)
            if start > end:
                self._syntax_error('Inverted freq range (%d - %d)' % (start, end))
            if start == end:
                self._syntax_error('Start and end freqs are equal (%d)' % start)
        except ValueError:
            self._syntax_error('band must have frequency range')

        b = FreqBand(start, end, bw, comments=self._comments)
        self._comments = []
        self._banddup[bname] = bname
        if b in self._bandrev:
            if dupwarn:
                self._warn('Duplicate band definition ("%s" and "%s")' % (bname, self._bandrev[b]))
            self._banddup[bname] = self._bandrev[b]
        self._bands[bname] = b
        self._bandrev[b] = bname
        self._bandline[bname] = self._lineno

    def _parse_band(self, line: str) -> None:
        try:
            bname, line = line.split(':', 1)
            if not bname:
                self._syntax_error("'band' keyword must be followed by name")
        except ValueError:
            self._syntax_error('band name must be followed by colon')

        if bname in flag_definitions:
            self._syntax_error('Invalid band name')

        self._parse_band_def(bname, line)

    def _parse_power(self, line: str) -> None:
        try:
            pname, line = line.split(':', 1)
            if not pname:
                self._syntax_error("'power' keyword must be followed by name")
        except ValueError:
            self._syntax_error('power name must be followed by colon')

        if pname in flag_definitions:
            self._syntax_error('Invalid power name')

        self._parse_power_def(pname, line)

    def _parse_power_def(self, pname: str, line: str, dupwarn: bool = True) -> None:
        try:
            max_eirp = line
            if max_eirp == 'N/A':
                max_eirp = '0'
            max_ant_gain = float(0)

            def conv_pwr(pwr: str) -> float:
                if pwr.endswith('mW'):
                    pwr_float = float(pwr[:-2])
                    return 10.0 * math.log10(pwr_float)
                else:
                    return float(pwr)

            max_eirp_float = conv_pwr(max_eirp)
        except ValueError:
            self._syntax_error('invalid power data')

        p = PowerRestriction(max_ant_gain, max_eirp_float, comments=self._comments)
        self._comments = []
        self._powerdup[pname] = pname
        if p in self._powerrev:
            if dupwarn:
                self._warn('Duplicate power definition ("%s" and "%s")' % (pname, self._powerrev[p]))
            self._powerdup[pname] = self._powerrev[p]
        self._power[pname] = p
        self._powerrev[p] = pname
        self._powerline[pname] = self._lineno

    def _parse_wmmrule(self, line: str) -> None:
        regions = line[:-1].strip()
        if not regions:
            self._syntax_error("'wmmrule' keyword must be followed by region")

        regions_list = regions.split(',')

        self._current_regions = {}
        for region in regions_list:
            if region in self._wmm_rules:
                self._warn('region %s was added already to wmm rules' % region)
            self._current_regions[region] = 1
        self._comments = []

    def _validate_input(self, cw_min: int, cw_max: int, aifsn: int, cot: int) -> None:
        if cw_min < 1:
            self._syntax_error('Invalid cw_min value (%d)' % cw_min)
        if cw_max < 1:
            self._syntax_error('Invalid cw_max value (%d)' % cw_max)
        if cw_min > cw_max:
            self._syntax_error('Inverted contention window (%d - %d)' % (cw_min, cw_max))
        if not (bin(cw_min + 1).count('1') == 1 and cw_min < 2**15):
            self._syntax_error('Invalid cw_min value should be power of 2 - 1 (%d)' % cw_min)
        if not (bin(cw_max + 1).count('1') == 1 and cw_max < 2**15):
            self._syntax_error('Invalid cw_max value should be power of 2 - 1 (%d)' % cw_max)
        if aifsn < 1:
            self._syntax_error('Invalid aifsn value (%d)' % aifsn)
        if cot < 0:
            self._syntax_error('Invalid cot value (%d)' % cot)

    def _validate_size(self, var: int, bytcnt: float) -> bool:
        return bytcnt < ceil(len(bin(var)[2:]) / 8.0)

    def _parse_wmmrule_item(self, line: str) -> None:
        bytcnt = (2.0, 2.0, 1.0, 2.0)
        try:
            ac, cval = line.split(':')
            if not ac:
                self._syntax_error('wmm item must have ac prefix')
        except ValueError:
            self._syntax_error('access category must be followed by colon')
        p = tuple([int(v.split('=', 1)[1]) for v in cval.split(',')])
        self._validate_input(*p)
        for v, b in zip(p, bytcnt):
            if self._validate_size(v, b):
                self._syntax_error('unexpected input size expect %d got %d' % (b, v))

        if self._current_regions is not None:
            for r in self._current_regions:
                self._wmm_rules[r][ac] = p

    def _parse_country(self, line: str) -> None:
        try:
            cname, cvals = line.split(':', 1)
            dfs_region = cvals.strip()
            if not cname:
                self._syntax_error("'country' keyword must be followed by name")
        except ValueError:
            self._syntax_error('country name must be followed by colon')

        cnames = cname.split(',')

        self._current_countries = {}
        for cname in cnames:
            if len(cname) != 2:
                self._warn("country '%s' not alpha2" % cname)
            cname_bytes = cname.encode('ascii')
            if cname_bytes not in self._countries:
                self._countries[cname_bytes] = Country(dfs_region, comments=self._comments)
            self._current_countries[cname_bytes] = self._countries[cname_bytes]
        self._comments = []

    def _parse_country_item(self, line: str) -> None:
        if line[0] == '(':
            try:
                band, line = line[1:].split('),', 1)
                bname = 'UNNAMED %d' % self._lineno
                self._parse_band_def(bname, band, dupwarn=False)
            except Exception:
                self._syntax_error('Badly parenthesised band definition')
        else:
            try:
                bname, line = line.split(',', 1)
                if not bname:
                    self._syntax_error('country definition must have band')
                if not line:
                    self._syntax_error('country definition must have power')
            except ValueError:
                self._syntax_error('country definition must have band and power')

        if line[0] == '(':
            items = line.split('),', 1)
            if len(items) == 1:
                pname = items[0]
                line = ''
                if not pname[-1] == ')':
                    self._syntax_error('Badly parenthesised power definition')
                pname = pname[:-1]
                flags = []
            else:
                pname = items[0]
                flags = items[1].split(',')
            power = pname[1:]
            pname = 'UNNAMED %d' % self._lineno
            self._parse_power_def(pname, power, dupwarn=False)
        else:
            line_list = line.split(',')
            pname = line_list[0]
            flags = line_list[1:]
        w = None
        if flags and 'wmmrule' in flags[-1]:
            try:
                region = flags.pop().split('=', 1)[1]
                if region not in self._wmm_rules.keys():
                    self._syntax_error('No wmm rule for %s' % region)
            except IndexError:
                self._syntax_error('flags is empty list or no region was found')
            w = WmmRule(*self._wmm_rules[region].values())

        if bname not in self._bands:
            self._syntax_error('band does not exist')
        if pname not in self._power:
            self._syntax_error('power does not exist')
        self._bands_used[bname] = True
        self._power_used[pname] = True
        bname = self._banddup[bname]
        pname = self._powerdup[pname]
        b = self._bands[bname]
        p = self._power[pname]
        if (b.start >= 2400 and b.end <= 2494) or (b.start >= 5150 and b.end <= 5895):
            try:
                channel_tuple = self.find_channel(b.start, b.end)
                b.start = channel_tuple[0]
                b.end = channel_tuple[1]
                b.maxbw = self.convert_maxbandwidth(b.maxbw)
                perm = Permission(b, p, flags, w)
            except FlagError as e:
                self._syntax_error("Invalid flag '%s'" % e.flag)
            if self._current_countries is not None:
                for cname, c in self._current_countries.items():
                    if perm in c:
                        self._warn('Rule "%s, %s" added to "%s" twice' % (bname, pname, cname.decode('ascii')))
                    else:
                        c.add(perm)
                        if perm.freqband.end <= 14:
                            c.add_2g(perm)
            else:
                self._warn('No current countries defined')

    def parse(self, f: TextIO) -> Dict[bytes, Country]:
        for line in f:
            self._lineno += 1
            line = line.strip()
            if line[0:1] == '#':
                self._comments.append(line[1:].strip())
            line = line.replace(' ', '').replace('\t', '')
            if not line:
                self._current_regions = None
                self._comments = []
            line = line.split('#')[0]
            if not line:
                continue
            if line[0:4] == 'band':
                self._parse_band(line[4:])
                self._current_countries = None
                self._current_regions = None
                self._comments = []
            elif line[0:5] == 'power':
                self._parse_power(line[5:])
                self._current_countries = None
                self._current_regions = None
                self._comments = []
            elif line[0:7] == 'country':
                self._parse_country(line[7:])
                self._comments = []
                self._current_regions = None
            elif self._current_countries is not None:
                self._current_regions = None
                self._parse_country_item(line)
                self._comments = []
            elif line[0:7] == 'wmmrule':
                self._parse_wmmrule(line[7:])
                self._current_countries = None
                self._comments = []
            elif self._current_regions is not None:
                self._parse_wmmrule_item(line)
                self._current_countries = None
                self._comments = []
            else:
                self._syntax_error('Expected band, power or country definition')

        countries = self._countries
        bands = {}
        for k, v in self._bands.items():
            if k in self._bands_used:
                bands[self._banddup[k]] = v
                continue
            if self._banddup[k] == k:
                self._lineno = self._bandline[k]
                self._warn('Unused band definition "%s"' % k)
        power = {}
        for k, j in self._power.items():
            if k in self._power_used:
                power[self._powerdup[k]] = j
                continue
            if self._powerdup[k] == k:
                self._lineno = self._powerline[k]
                self._warn('Unused power definition "%s"' % k)
        return countries


class Regdomain(object):
    def __init__(self) -> None:
        self.regdomain_countries: Dict[bytes, int] = {}
        self.typical_regulatory: Dict[str, List] = {}
        self.regdomain_countries_2g: Dict[bytes, int] = {}
        self.typical_regulatory_2g: Dict[str, List] = {}

    def build_typical_regdomains(self, countries: Dict[bytes, Country]) -> None:
        """Populate typical regulatory domains based on country permissions."""
        for cn, country in countries.items():
            cn_str = cn.decode('utf-8')
            for reg_type in typical_regdomain:
                if cn_str in typical_regdomain[reg_type]:
                    self.typical_regulatory.setdefault(reg_type, country.permissions)

    def build_typical_regdomains_2g(self, countries: Dict[bytes, Country]) -> None:
        """Populate typical regulatory domains based on country permissions."""
        for cn, country in countries.items():
            cn_str = cn.decode('utf-8')
            for reg_type in typical_regdomain:
                if cn_str in typical_regdomain[reg_type]:
                    self.typical_regulatory_2g.setdefault(reg_type, country.permissions_2g)

    def simplify_countries(self, countries: Dict[bytes, Country]) -> None:
        """Simplify country permissions by building typical regdomains."""
        self.build_typical_regdomains(countries)
        perm_list = list(self.typical_regulatory.values())

        for cn, country in countries.items():
            cn_str = cn.decode('utf-8')
            permissions = country.permissions
            if permissions not in perm_list:
                self.typical_regulatory[cn_str] = permissions
                perm_list.append(permissions)
            self.regdomain_countries[cn] = perm_list.index(permissions)

    def simplify_countries_2g(self, countries: Dict[bytes, Country]) -> None:
        """Simplify country permissions by building typical regdomains."""
        self.build_typical_regdomains_2g(countries)

        perm_list = list(self.typical_regulatory_2g.values())

        for cn, country in countries.items():
            cn_str = cn.decode('utf-8')
            permissions = country.permissions_2g

            if permissions not in perm_list:
                self.typical_regulatory_2g[cn_str] = permissions
                perm_list.append(permissions)
            self.regdomain_countries_2g[cn] = perm_list.index(permissions)
