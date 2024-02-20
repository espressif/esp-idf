#!/usr/bin/env python
# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import collections
import os
import unittest

import pexpect

Test = collections.namedtuple('Test', ['name', 'term', 'pattern', 'ext'])


TESTS = (Test('fish', 'vt100', 'all.*app.*app-flash.*bootloader.*', 'fish'),
         Test('bash', 'xterm-256color', 'all.*app.*app-flash.*bootloader.*bootloader-flash.*build-system-targets.*clean.*', 'sh'),
         Test('zsh', '', 'all.*app.*app-flash.*bootloader.*bootloader-flash.*build-system-targets.*clean.*', 'sh'))


# Additional positional arguments for all child.expect() calls are constant so we can rely on the order and print message
# about which pattern was matched
pargs = (pexpect.EOF, pexpect.TIMEOUT)


def get_fail_msg(pproc: pexpect.spawn, msg: str, index: int) -> str:
    try:
        buf = pproc._buffer.getvalue()
    except AttributeError:
        # _buffer is an internal of pexpect.spawn and is not part of the API.
        # Either there is no _buffer or it is not io.BytesIO() anymore.
        buf = '<UNKNOWN>'

    return '{} ({}) buffer: "{}"'.format(msg, 'EOF - child has exited' if index == 1 else 'TIMEOUT', buf)


class UTTest(unittest.TestCase):

    def test_shell(self) -> None:
        idf_path = os.environ['IDF_PATH']
        env = os.environ.copy()
        for test in TESTS:
            with self.subTest():
                with open(os.path.join(idf_path, f'{test.name}.out'), 'wb') as o:
                    env['TERM'] = test.term
                    with pexpect.spawn(f'{test.name} -i', env=env, logfile=o, timeout=200) as pproc:
                        pproc.sendline(f'. {idf_path}/export.{test.ext}')
                        i = pproc.expect(['Go to the project directory and run.*idf\\.py build', *pargs])
                        self.assertEqual(i, 0, get_fail_msg(pproc, 'Export was not successful!', i))
                        pproc.send('idf.py \t\t')
                        i = pproc.expect([test.pattern, *pargs], timeout=100)
                        self.assertEqual(i, 0, get_fail_msg(pproc, f'Autocompletion for idf.py failed in {test.name}!', i))


if __name__ == '__main__':
    unittest.main()
