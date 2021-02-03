from __future__ import unicode_literals

import os
import threading
import time

import debug_backend
import pexpect
import serial
import ttfw_idf
from tiny_test_fw import Utility


class SerialThread(object):
    def run(self, log_path, exit_event):
        with serial.Serial(os.getenv('ESPPORT', '/dev/ttyUSB1'), 115200) as ser, open(log_path, 'wb') as f:
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


@ttfw_idf.idf_custom_test(env_tag='test_jtag_arm', group='test-apps')
def test_app_loadable_elf(env, extra_data):
    rel_project_path = os.path.join('tools', 'test_apps', 'system', 'gdb_loadable_elf')
    app_files = ['gdb_loadable_elf.elf']
    target = 'esp32'
    app = ttfw_idf.LoadableElfTestApp(rel_project_path, app_files, target=target)
    idf_path = app.get_sdk_path()
    proj_path = os.path.join(idf_path, rel_project_path)
    elf_path = os.path.join(app.binary_path, 'gdb_loadable_elf.elf')
    esp_log_path = os.path.join(proj_path, 'esp.log')

    with SerialThread(esp_log_path):
        openocd_log = os.path.join(proj_path, 'openocd.log')
        gdb_log = os.path.join(proj_path, 'gdb.log')
        gdb_init = os.path.join(proj_path, 'gdbinit_' + target)
        gdb_dir = os.path.join(proj_path, 'main')

        with ttfw_idf.OCDBackend(openocd_log, app.target):
            with ttfw_idf.GDBBackend(gdb_log, elf_path, app.target, gdb_init, gdb_dir) as p:
                def wait_for_breakpoint():
                    p.gdb.wait_target_state(debug_backend.TARGET_STATE_RUNNING)
                    stop_reason = p.gdb.wait_target_state(debug_backend.TARGET_STATE_STOPPED)
                    assert stop_reason == debug_backend.TARGET_STOP_REASON_BP, 'STOP reason: {}'.format(stop_reason)

                wait_for_breakpoint()

                p.gdb.add_bp('esp_restart')
                p.gdb.exec_continue()

                wait_for_breakpoint()

    if pexpect.run('grep "Restarting now." {}'.format(esp_log_path), withexitstatus=True)[1]:
        raise RuntimeError('Expected output from ESP was not received')


if __name__ == '__main__':
    test_app_loadable_elf()
