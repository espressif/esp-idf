from __future__ import print_function
import os
import hashlib

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_WIFI')
def test_examples_spiffsgen(env, extra_data):
    # Test with default build configurations
    dut = env.get_dut('spiffsgen', 'examples/storage/spiffsgen')
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
