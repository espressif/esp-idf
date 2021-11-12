#!/usr/bin/env python
import os
import sys
import unittest

import pexpect


class Test(unittest.TestCase):
    def test_fish(self):
        os.environ['TERM'] = 'vt100'
        child = pexpect.spawn('fish -i')
        with open(os.environ['IDF_PATH'] + '/fish' + str(sys.version_info.major) + '.out', 'wb') as output:
            child.logfile = output
            child.sendline('. ./export.fish')
            result = child.expect(
                ['Go to the project directory and run.*idf\\.py build', pexpect.EOF,
                 pexpect.TIMEOUT], timeout=40)
            self.assertEqual(result, 0, 'Export was not successful!')
            child.send('idf.py \t\t')
            result = child.expect(['all.*app.*app-flash.*bootloader.*', pexpect.EOF, pexpect.TIMEOUT], timeout=40)
            self.assertEqual(result, 0, 'Autocompletion for idf.py failed in fish!')

    def test_bash(self):
        os.environ['TERM'] = 'xterm-256color'
        child = pexpect.spawn('bash -i')
        with open(os.environ['IDF_PATH'] + '/bash' + str(sys.version_info.major) + '.out', 'wb') as output:
            child.logfile = output
            child.sendline('. ./export.sh')
            child.send('idf.py \t\t')
            result = child.expect(
                ['Go to the project directory and run.*idf\\.py build', pexpect.EOF,
                 pexpect.TIMEOUT], timeout=40)
            self.assertEqual(result, 0, 'Export was not successful!')
            result = child.expect(
                ['all.*app.*app-flash.*bootloader.*bootloader-flash.*build-system-targets.*clean.*', pexpect.EOF,
                 pexpect.TIMEOUT], timeout=40)
            self.assertEqual(result, 0, 'Autocompletion for idf.py failed in bash!')

    def test_zsh(self):
        child = pexpect.spawn('zsh -i')
        with open(os.environ['IDF_PATH'] + '/zsh' + str(sys.version_info.major) + '.out', 'wb') as output:
            child.logfile = output
            child.sendline('. ./export.sh')
            result = child.expect(
                ['Go to the project directory and run.*idf\\.py build', pexpect.EOF,
                 pexpect.TIMEOUT], timeout=40)
            self.assertEqual(result, 0, 'Export was not successful!')
            child.send('idf.py \t\t')
            result = child.expect(
                ['all.*app.*app-flash.*bootloader.*bootloader-flash.*build-system-targets.*clean.*', pexpect.EOF,
                 pexpect.TIMEOUT], timeout=40)
            self.assertEqual(result, 0, 'Autocompletion for idf.py failed in zsh!')


if __name__ == '__main__':
    unittest.main()
