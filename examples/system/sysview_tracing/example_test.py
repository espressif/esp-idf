from __future__ import unicode_literals
from io import open
import os
import re
import tempfile
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag="test_jtag_arm")
def test_examples_sysview_tracing(env, extra_data):

    rel_project_path = os.path.join('examples', 'system', 'sysview_tracing')
    dut = env.get_dut('sysview_tracing', rel_project_path)
    idf_path = dut.app.get_sdk_path()
    proj_path = os.path.join(idf_path, rel_project_path)
    elf_path = os.path.join(dut.app.get_binary_path(rel_project_path), 'sysview_tracing.elf')

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

        with ttfw_idf.OCDProcess(os.path.join(proj_path, 'openocd.log')):
            dut.start_app()

            def dut_expect_task_event():
                dut.expect(re.compile(r'example: Task\[0x3[0-9A-Fa-f]+\]: received event \d+'), timeout=30)

            dut_expect_task_event()

            gdb_args = '-x {} --directory={}'.format(tempfiles[0], os.path.join(proj_path, 'main'))
            with ttfw_idf.GDBProcess(os.path.join(proj_path, 'gdb.log'), elf_path, dut.app.target, gdb_args) as gdb:
                gdb.pexpect_proc.expect_exact('Thread 1 "main" hit Breakpoint 1, app_main ()')
                gdb.pexpect_proc.expect_exact('Targets connected.')
                gdb.pexpect_proc.expect(re.compile(r'\d+'))

                dut.expect('example: Created task')  # dut has been restarted by gdb since the last dut.expect()
                dut_expect_task_event()

                gdb.pexpect_proc.sendcontrol('c')
                gdb.pexpect_proc.expect_exact('(gdb)')
    finally:
        for x in tempfiles:
            try:
                os.unlink(x)
            except Exception:
                pass


if __name__ == '__main__':
    test_examples_sysview_tracing()
