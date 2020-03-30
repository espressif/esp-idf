import os
import threading
import time

import pexpect
import serial

from tiny_test_fw import Utility
import ttfw_idf


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
    elf_path = os.path.join(example.binary_path, 'hello-world.elf')
    esp_log_path = os.path.join(proj_path, 'esp.log')

    with SerialThread(esp_log_path):
        openocd_log = os.path.join(proj_path, 'openocd.log')
        gdb_log = os.path.join(proj_path, 'gdb.log')
        gdb_args = '-x {} --directory={}'.format(os.path.join(proj_path, '.gdbinit.ci'),
                                                 os.path.join(proj_path, 'main'))

        with ttfw_idf.OCDProcess(openocd_log), ttfw_idf.GDBProcess(gdb_log, elf_path, gdb_args) as gdb:
            gdb.pexpect_proc.sendline('')  # it is for "---Type <return> to continue, or q <return> to quit---"
            i = gdb.pexpect_proc.expect_exact(['Thread 1 hit Temporary breakpoint 2, app_main ()',
                                               'Load failed'])
            if i == 0:
                Utility.console_log('gdb is at breakpoint')
            elif i == 1:
                raise RuntimeError('Load has failed. Please examine the logs.')
            else:
                Utility.console_log('i = {}'.format(i))
                Utility.console_log(str(gdb.pexpect_proc))
                # This really should not happen. TIMEOUT and EOF failures are exceptions.
                raise RuntimeError('An unknown error has occurred. Please examine the logs.')

            gdb.pexpect_proc.expect_exact('(gdb)')
            gdb.pexpect_proc.sendline('b esp_restart')
            gdb.pexpect_proc.sendline('c')
            gdb.pexpect_proc.expect_exact('Thread 1 hit Breakpoint 3, esp_restart ()')

    if pexpect.run('grep "Restarting now." {}'.format(esp_log_path), withexitstatus=True)[1]:
        raise RuntimeError('Expected output from ESP was not received')


if __name__ == '__main__':
    test_examples_loadable_elf()
