import tiny_test_fw  # noqa: F401 # pylint: disable=unused-import
import ttfw_idf

try:
    import typing  # noqa: F401 # pylint: disable=unused-import
except ImportError:
    pass


@ttfw_idf.idf_component_unit_test(env_tag='COMPONENT_UT_GENERIC', target=['esp32', 'esp32s2', 'esp32s3', 'esp32c3'])
def test_component_ut_newlib(env, _):  # type: (tiny_test_fw.Env, typing.Any) -> None
    dut = env.get_dut('newlib', 'components/newlib/test_apps')
    dut.start_app()
    stdout = dut.expect('Tests finished, rc=0', full_stdout=True)
    ttfw_idf.ComponentUTResult.parse_result(stdout)


if __name__ == '__main__':
    test_component_ut_newlib()
