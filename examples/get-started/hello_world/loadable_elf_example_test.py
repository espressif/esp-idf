import os
import threading
import time

import pexpect
import serial

from tiny_test_fw import Utility
import ttfw_idf


class CustomProcess(object):
    def __init__(self, cmd, logfile):
        self.f = open(logfile, 'wb')
        self.p = pexpect.spawn(cmd, timeout=60, logfile=self.f)

    def __enter__(self):
        return self

    def close(self):
        self.p.terminate(force=True)

    def __exit__(self, type, value, traceback):
        self.close()
        self.f.close()


class OCDProcess(CustomProcess):
    def __init__(self, proj_path):
        cmd = 'openocd -f board/esp32-wrover-kit-3.3v.cfg'
        log_file = os.path.join(proj_path, 'openocd.log')
        super(OCDProcess, self).__init__(cmd, log_file)
        patterns = ['Info : Listening on port 3333 for gdb connections',
                    'Error: type \'esp32\' is missing virt2phys']

        try:
            while True:
                i = self.p.expect_exact(patterns, timeout=30)
                # TIMEOUT or EOF exceptions will be thrown upon other errors
                if i == 0:
                    Utility.console_log('openocd is listening for gdb connections')
                    break  # success
                elif i == 1:
                    Utility.console_log('Ignoring error: "{}"'.format(patterns[i]))
                    # this error message is ignored because it is not a fatal error
        except Exception:
            Utility.console_log('openocd initialization has failed', 'R')
            raise

    def close(self):
        try:
            self.p.sendcontrol('c')
            self.p.expect_exact('shutdown command invoked')
        except Exception:
            Utility.console_log('openocd needs to be killed', 'O')
            super(OCDProcess, self).close()


class GDBProcess(CustomProcess):
    def __init__(self, proj_path, elf_path):
        cmd = 'xtensa-esp32-elf-gdb -x {} --directory={} {}'.format(os.path.join(proj_path, '.gdbinit.ci'),
                                                                    os.path.join(proj_path, 'main'),
                                                                    elf_path)
        log_file = os.path.join(proj_path, 'gdb.log')
        super(GDBProcess, self).__init__(cmd, log_file)
        self.p.sendline('')  # it is for "---Type <return> to continue, or q <return> to quit---"
        i = self.p.expect_exact(['Thread 1 hit Temporary breakpoint 2, app_main ()',
                                 'Load failed'])
        if i == 0:
            Utility.console_log('gdb is at breakpoint')
        elif i == 1:
            raise RuntimeError('Load has failed. Please examine the logs.')
        else:
            Utility.console_log('i = {}'.format(i))
            Utility.console_log(str(self.p))
            # This really should not happen. TIMEOUT and EOF failures are exceptions.
            raise RuntimeError('An unknown error has occurred. Please examine the logs.')

        self.p.expect_exact('(gdb)')

    def close(self):
        try:
            self.p.sendline('q')
            self.p.expect_exact('Quit anyway? (y or n)')
            self.p.sendline('y')
            self.p.expect_exact('Ending remote debugging.')
        except Exception:
            Utility.console_log('gdb needs to be killed', 'O')
            super(GDBProcess, self).close()

    def break_till_end(self):
        self.p.sendline('b esp_restart')
        self.p.sendline('c')
        self.p.expect_exact('Thread 1 hit Breakpoint 3, esp_restart ()')


class SerialThread(object):
    def run(self, log_path, exit_event):
        with serial.Serial('/dev/ttyUSB1', 115200) as ser, open(log_path, 'wb') as f:
            while True:
                f.write(ser.read(ser.in_waiting))
                if exit_event.is_set():
                    break
                time.sleep(1)

    def __init__(self, log_path):
        self.exit_event = threading.Event()
        self.t = threading.Thread(target=self.run, args=(log_path, self.exit_event,))
        self.t.start()

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.exit_event.set()
        self.t.join(60)
        if self.t.is_alive():
            Utility.console_log('The pyserial thread is still alive', 'O')


@ttfw_idf.idf_example_test(env_tag="test_jtag_arm")
def test_examples_loadable_elf(env, extra_data):

    rel_project_path = os.path.join('examples', 'get-started', 'hello_world')
    app_files = ['hello-world.elf', 'partition_table/partition-table.bin']
    example = ttfw_idf.LoadableElfExample(rel_project_path, app_files, target="esp32")
    idf_path = example.get_sdk_path()
    proj_path = os.path.join(idf_path, rel_project_path)
    sdkconfig = example.get_sdkconfig()
    elf_path = os.path.join(example.binary_path, 'hello-world.elf')
    esp_log_path = os.path.join(proj_path, 'esp.log')

    assert(sdkconfig['CONFIG_IDF_TARGET_ESP32'] == 'y'), "Only ESP32 target is supported"
    assert(sdkconfig['CONFIG_APP_BUILD_TYPE_ELF_RAM'] == 'y'), "ELF should be built with CONFIG_APP_BUILD_TYPE_ELF_RAM"

    with SerialThread(esp_log_path):
        with OCDProcess(proj_path), GDBProcess(proj_path, elf_path) as gdb:
            gdb.break_till_end()

    if pexpect.run('grep "Restarting now." {}'.format(esp_log_path), withexitstatus=True)[1]:
        raise RuntimeError('Expected output from ESP was not received')


if __name__ == '__main__':
    test_examples_loadable_elf()
