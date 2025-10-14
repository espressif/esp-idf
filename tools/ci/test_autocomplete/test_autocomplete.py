#!/usr/bin/env python
# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import collections
import os
import unittest

import pexpect

Test = collections.namedtuple('Test', ['name', 'shell', 'term', 'prefix', 'pattern', 'ext'])

IDF_PATH = os.environ['IDF_PATH']

TESTS = (
    Test('base',
         'fish',
         'vt100',
         '',
         'all.*app.*app-flash.*bootloader.*',
         'fish'),
    Test('base',
         'bash',
         'xterm-256color',
         '',
         'all.*app.*app-flash.*bootloader.*bootloader-flash.*build-system-targets.*clean.*',
         'sh'),
    Test('base',
         'zsh',
         '',
         '',
         'all.*app.*app-flash.*bootloader.*bootloader-flash.*build-system-targets.*clean.*',
         'sh'),
    # Tests for file autocompletion
    Test('file',
         'fish',
         'vt100',
         f'@{IDF_PATH}/README',
         '.*README.*md.*',
         'fish'),
    Test('file',
         'bash',
         'xterm-256color',
         f'@{IDF_PATH}/README',
         '.*README.*md.*',
         'sh'),
    Test('file',
         'zsh',
         '',
         f'@{IDF_PATH}/README',
         '.*README.*md.*',
         'sh')
)

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
        env = os.environ.copy()
        for test in TESTS:
            with self.subTest():
                with open(os.path.join(IDF_PATH, f'{test.shell}.{test.name}.out'), 'wb') as o:
                    env['TERM'] = test.term
                    with pexpect.spawn(f'{test.shell} -i', env=env, logfile=o, timeout=200) as pproc:
                        if test.shell == 'bash':
                            # The @-argument autocomplete works reliably in bash only
                            # if @ is not included in COMP_WORDBREAKS.
                            pproc.sendline('COMP_WORDBREAKS=${COMP_WORDBREAKS//@/}')
                        pproc.sendline(f'. {IDF_PATH}/export.{test.ext}')
                        i = pproc.expect(['Go to the project directory and run.*idf\\.py build', *pargs])
                        self.assertEqual(i, 0, get_fail_msg(pproc, 'Export was not successful!', i))
                        pproc.send(f'idf.py {test.prefix}\t\t')
                        i = pproc.expect([test.pattern, *pargs], timeout=100)
                        self.assertEqual(i, 0, get_fail_msg(pproc, f'Autocompletion for idf.py failed in {test.shell}!', i))


if __name__ == '__main__':
    unittest.main()
