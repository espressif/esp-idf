from __future__ import unicode_literals

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC', target=['esp32', 'esp32c3'])
def test_examples_asio_ssl(env, extra_data):

    dut = env.get_dut('asio_ssl_client_server', 'examples/protocols/asio/ssl_client_server')
    dut.start_app()

    dut.expect('Reply: GET / HTTP/1.1')


if __name__ == '__main__':
    test_examples_asio_ssl()
