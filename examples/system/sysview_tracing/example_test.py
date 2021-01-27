from __future__ import unicode_literals

import os
import re
import tempfile
import time
from io import open

import debug_backend
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='test_jtag_arm')
def test_examples_sysview_tracing(env, extra_data):

    rel_project_path = os.path.join('examples', 'system', 'sysview_tracing')
    dut = env.get_dut('sysview_tracing', rel_project_path)
    proj_path = os.path.join(dut.app.idf_path, rel_project_path)
    elf_path = os.path.join(dut.app.binary_path, 'sysview_tracing.elf')

    def get_temp_file():
        with tempfile.NamedTemporaryFile(delete=False) as f:
            return f.name

    try:
        tempfiles = [get_temp_file(), get_temp_file()]

        with open(os.path.join(proj_path, 'gdbinit')) as f_in, open(tempfiles[0], 'w') as f_out:
            new_content = f_in.read()
            # localhost connection issue occurs in docker unless:
            new_content = new_content.replace(':3333', '127.0.0.1:3333', 1)
            new_content = new_content.replace('file:///tmp/sysview_example.svdat', 'file://{}'.format(tempfiles[1]), 1)
            f_out.write(new_content)

        with ttfw_idf.OCDBackend(os.path.join(proj_path, 'openocd.log'), dut.app.target) as oocd:
            dut.start_app()

            def dut_expect_task_event():
                dut.expect(re.compile(r'example: Task\[0x3[0-9A-Fa-f]+\]: received event \d+'), timeout=30)

            dut_expect_task_event()

            gdb_log = os.path.join(proj_path, 'gdb.log')
            gdb_workdir = os.path.join(proj_path, 'main')
            with ttfw_idf.GDBBackend(gdb_log, elf_path, dut.app.target, tempfiles[0], gdb_workdir) as p:
                p.gdb.wait_target_state(debug_backend.TARGET_STATE_RUNNING)
                stop_reason = p.gdb.wait_target_state(debug_backend.TARGET_STATE_STOPPED)
                assert stop_reason == debug_backend.TARGET_STOP_REASON_BP, 'STOP reason: {}'.format(stop_reason)

                dut.expect('example: Created task')  # dut has been restarted by gdb since the last dut.expect()
                dut_expect_task_event()

                # Do a sleep while sysview samples are captured.
                time.sleep(3)
                # GDBMI isn't responding now to any commands, therefore, the following command is issued to openocd
                oocd.cmd_exec('esp sysview stop')
    finally:
        for x in tempfiles:
            try:
                os.unlink(x)
            except Exception:
                pass


if __name__ == '__main__':
    test_examples_sysview_tracing()
