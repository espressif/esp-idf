from __future__ import print_function
import os
import sys
import hashlib

try:
    import IDF
    from IDF.IDFDUT import ESP32DUT
except ImportError:
    test_fw_path = os.getenv('TEST_FW_PATH')
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF


@IDF.idf_example_test(env_tag='Example_WIFI')
def test_examples_spiffsgen(env, extra_data):
    # Test with default build configurations
    dut = env.get_dut('spiffsgen', 'examples/storage/spiffsgen', dut_class=ESP32DUT)
    dut.start_app()

    base_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'spiffs_image')

    # Expect hello.txt is read successfully
    with open(os.path.join(base_dir, 'hello.txt'), 'r') as hello_txt:
        dut.expect('Read from hello.txt: ' + hello_txt.read())

    # Expect alice.txt MD5 hash is computed accurately
    with open(os.path.join(base_dir, 'sub', 'alice.txt'), 'rb') as alice_txt:
        alice_md5 = hashlib.md5(alice_txt.read()).hexdigest()
        dut.expect('Computed MD5 hash of alice.txt: ' + alice_md5)


if __name__ == '__main__':
    test_examples_spiffsgen()
