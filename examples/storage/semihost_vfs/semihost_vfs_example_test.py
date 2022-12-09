from io import open
import os
import shutil
import tempfile
import ttfw_idf

try:
    from itertools import izip_longest as zip_longest
except ImportError:
    # Python 3
    from itertools import zip_longest


@ttfw_idf.idf_example_test(env_tag="test_jtag_arm")
def test_examples_semihost_vfs(env, extra_data):

    rel_project_path = os.path.join('examples', 'storage', 'semihost_vfs')
    dut = env.get_dut('semihost_vfs', rel_project_path)
    idf_path = dut.app.get_sdk_path()
    proj_path = os.path.join(idf_path, rel_project_path)
    host_file_name = 'host_file.txt'

    try:
        temp_dir = tempfile.mkdtemp()
        host_file_path = os.path.join(proj_path, 'data', host_file_name)
        shutil.copyfile(host_file_path, os.path.join(temp_dir, host_file_name))
        openocd_extra_args = '-c \'set ESP_SEMIHOST_BASEDIR {}\''.format(temp_dir)

        with ttfw_idf.OCDProcess(os.path.join(proj_path, 'openocd.log'), openocd_extra_args):
            dut.start_app()
            dut.expect_all('example: Switch to semihosted stdout',
                           'example: Switched back to UART stdout',
                           'example: Wrote 2798 bytes',
                           '====================== HOST DATA START =========================',
                           timeout=20)
            with open(host_file_path) as f:
                file_content = [line.strip() for line in f]
            dut.expect_all(*file_content, timeout=20)
            dut.expect_all('====================== HOST DATA END =========================',
                           'example: Read 6121 bytes',
                           timeout=5)

        with open(os.path.join(temp_dir, 'esp32_stdout.txt')) as f:
            def expected_content():
                yield 'example: Switched to semihosted stdout'
                for i in range(100):
                    yield 'Semihosted stdout write {}'.format(i)
                yield 'example: Switch to UART stdout'

            for actual, expected in zip_longest(f, expected_content(), fillvalue='-'):
                if expected not in actual:  # "in" used because of the printed ASCII color codes
                    raise RuntimeError('"{}" != "{}"'.format(expected, actual.strip()))
    finally:
        shutil.rmtree(temp_dir, ignore_errors=True)


if __name__ == '__main__':
    test_examples_semihost_vfs()
