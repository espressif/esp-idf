from __future__ import unicode_literals
import re
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_unit_test(env, extra_data):

    dut = env.get_dut('unit_test', 'examples/system/unit_test')
    dut.start_app()

    def get_reg_nums(number):
        return re.compile(r'\d{1,2}\s+' * number)

    dut.expect_all('In main application. Collecting 32 random numbers from 1 to 100:',
                   get_reg_nums(10),
                   get_reg_nums(10),
                   get_reg_nums(10),
                   get_reg_nums(2),
                   timeout=30)


if __name__ == '__main__':
    test_examples_unit_test()
