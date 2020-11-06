#!/usr/bin/env python
import os
import pexpect
import unittest


class Test(unittest.TestCase):
    def test_fish(self):
        os.environ["TERM"] = "vt100"
        child = pexpect.spawn("fish -i")
        child.logfile = open(os.environ["IDF_PATH"] + "/fish.out", "wb")
        child.sendline(
            '. {$IDF_PATH}/export.fish >> {$IDF_PATH}/debug.out')
        child.send("idf.py \t\t")
        result = child.expect(["all.*app.*app-flash.*bootloader.*", pexpect.EOF, pexpect.TIMEOUT], timeout=5)
        self.assertEqual(result, 0, "Autocompletion for idf.py failed in fish!")
        result = child.expect(["bootloader-flash.*build-system-targets.*clean.*", pexpect.EOF, pexpect.TIMEOUT],
                              timeout=5)
        self.assertEqual(result, 0, "Autocompletion for idf.py failed in fish!")

    def test_bash(self):
        os.environ["TERM"] = "xterm-256color"
        child = pexpect.spawn("bash -i")
        child.logfile = open(os.environ["IDF_PATH"] + "/bash.out", "wb")
        child.sendline(
            '. ${IDF_PATH}/export.sh >> ${IDF_PATH}/debug.out')
        child.send("idf.py \t\t")
        result = child.expect(
            ["all.*app.*app-flash.*bootloader.*bootloader-flash.*build-system-targets.*clean.*", pexpect.EOF,
             pexpect.TIMEOUT], timeout=5)
        self.assertEqual(result, 0, "Autocompletion for idf.py failed in bash!")

    def test_zsh(self):
        child = pexpect.spawn("zsh -i")
        child.logfile = open(os.environ["IDF_PATH"] + "/zsh.out", "wb")
        child.sendline(
            '. ${IDF_PATH}/export.sh >> ${IDF_PATH}/debug.out ')
        child.send("idf.py \t")
        result = child.expect(
            ["all.*app.*app-flash.*bootloader.*bootloader-flash.*build-system-targets.*clean.*", pexpect.EOF,
             pexpect.TIMEOUT], timeout=5)
        self.assertEqual(result, 0, "Autocompletion for idf.py failed in zsh!")


if __name__ == '__main__':
    unittest.main()
