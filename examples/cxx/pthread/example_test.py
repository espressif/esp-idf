from __future__ import unicode_literals
import re
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_GENERIC')
def test_examples_cpp_pthread(env, extra_data):

    dut = env.get_dut('cpp_pthread', 'examples/cxx/pthread')
    dut.start_app()

    dut.expect_all(re.compile(r'pthread: This thread \(with the default name\) may run on any core.'
                              r'Core id: [01], prio: 5, minimum free stack: \d+ bytes.'),
                   re.compile(r'Thread [12]: Core id: [01], prio: 5, minimum free stack: \d+ bytes.'),
                   re.compile(r'Thread [12]: This is the INHERITING thread with the same parameters as our parent, '
                              r'including name. Core id: [01], prio: 5, minimum free stack: \d+ bytes.'),
                   re.compile(r'Thread [12]: Core id: [01], prio: 5, minimum free stack: \d+ bytes'))


if __name__ == '__main__':
    test_examples_cpp_pthread()
