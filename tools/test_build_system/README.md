# Tools & Build System Tests

This directory contains tests for the build system and build-related tools. These tests are meant to be used both by developers and in CI. Please check the sections below for details on:

- Running the tests locally
- Debugging test failures
- Adding new tests
- Fixtures and helper functions

## Running the tests locally

1. Install pytest using `install.{sh,bat,ps1,fish} --enable-pytest`.
1. Activate the IDF shell environment using `export.{sh,bat,ps1,fish}`.
1. To run all the tests, go to `$IDF_PATH/tools/test_build_system` directory, then run:
    ```
    pytest
    ```
1. To run one specific test, use `-k` flag of pytest, for example
    ```
    pytest -k test_compile_commands_json_updated_by_reconfigure
    ```
1. To speed up the builds you can install Ccache and set the following environment variables:
    ```
    export IDF_CCACHE_ENABLE=1
    export CCACHE_NOHASHDIR=1
    ```

## Debugging test failures

If you are working on a bug fix or a feature and one of the tests starts to fail, you should try to reproduce the failure locally.

1. Find the name of the failing test in the CI job log
1. Follow the steps in the section above to run that one test
1. By default, the fixtures which create temporary directories will remove them after the test. To prevent the directories from being removed, run `pytest` with `--work-dir /some/path` flag. The temporary directories will be created under `/some/path`, and you will be able to inspect them once the test fails.
1. You can increase the logging level to see the commands being executed by the test by running `pytest` with `--log-cli-level DEBUG` argument.

## Adding new tests

1. When adding a new test, think of the developer who might have to run this test locally.
   - Avoid adding tests which take a long time to run. Running the entire test suite should be possible!
   - Remember that developers run these tests in their IDF work directories. Be careful with destructive actions, especially removing directories recursively. Developers might have untracked files in the directory you are removing! Prefer using the `idf_copy` fixture to make a copy of the IDF directory, when doing some modifications to IDF source. If this is too expensive, do the modification in place but make sure to clean up the changes you perform using a try/finally block. This especially applies to the newly created files.
1. Read through the test cases and try to find a test which does something similar to what you need to test. This will usually be a good starting point. Also read through the section below, which explains fixtures and utility functions.
1. The tests need to run on Windows, Linux and macOS. Avoid calling OS-specific programs such as `sed` or `awk` in tests. If you need to perform some complex file modification in the test case, consider writing a Python helper function for that.

## Fixtures and helper functions

If you aren't yet familiar with Pytest fixtures, please take a few moments and read a Pytest tutorial or watch the training, before moving on to the next section.

### `test_app_copy` fixture

This fixture selects the app (inside IDF) to be used by the test and copies this app to a temporary directory, recursively. The working directory is set to the root of the copied app. The directory is removed once the test is finished.

```python
def test_something(test_app_copy):
    assert test_app_copy == os.getcwd()
    # the current working directory now contains the copy of the test app
```

If the test case doesn't use the `test_app_copy` argument, pylint will typically warn about an unused argument, even if the fixture is actually used. To avoid the warning, use the following pattern:
```python
@pytest.mark.usefixtures('test_app_copy')
def test_something(idf_py):
    idf_py('build')
```

By default, the source app is `tools/test_build_system/build_test_app` and the destination directory name is derived from the test case name. (See more about this test app [here](#application-under-test).) This can be overridden using a `@pytest.mark.test_app_copy` decorator, as shown below. The first argument is the path of the source app. The second argument is the name of the temporary directory to create. The second argument is optional, it is mostly useful to test handling of special characters (such as spaces) in the path.

```python
@pytest.mark.test_app_copy('examples/get-started/blink', 'custom dir name')
@pytest.mark.usefixtures('test_app_copy')
def test_something():
    pass
```

### `idf_py` fixture

This fixture runs `idf.py` with IDF environment set up.

```python
@pytest.mark.usefixtures('test_app_copy')
def test_something(idf_py)
    # 1. 'test_app_copy' fixture has copied the test app into a temporary directory and
    #    set the working directory there.
    # 2. 'idf_py' fixture is a function which calls idf.py:
    idf_py('fullclean')

    # 3. It accepts multiple arguments and returns a subprocess.CompletedProcess
    #    instance. It can be used to check the process output.
    output = idf_py('-DIDF_TARGET=esp32c3', 'reconfigure')
    assert 'CONFIG_IDF_TARGET="esp32c3"' in Path('sdkconfig').read_text()
    assert 'Building ESP-IDF components for target esp32c3' in output.stdout

    # 4. Raises subprocess.CalledProcessError on failure
    with(pytest.raises(subprocess.CalledProcessError)) as exc_info:
        idf_py('unknown_command')
    assert 'command "unknown_command" is not known to idf.py' in exc_info.value.stderr
```

### `default_idf_env` fixture

Returns a dictionary of environment variables required for the IDF build environment. It is similar to the output of `env` command after running the `export` script.

```python
@pytest.mark.usefixtures('test_app_copy')
def test_cmake(default_idf_env):
    with tempfile.TemporaryDirectory() as build_dir:
        # default_idf_env can be passed to subprocess APIs to run other tools
        subprocess.run(['cmake', '-B', build_dir, '.'], env=default_idf_env, check=True)
```

Note, `default_idf_env` sets up the environment based on the `IDF_PATH` environment variable set before launching `pytest`.

### `idf_copy` fixture

Copies IDF from `IDF_PATH` into a new temporary directory. `@pytest.mark.idf_copy('name prefix')` can be used to specify the name prefix of the temporary directory.

For the duration of the test, `IDF_PATH` environment variable is set to the newly created copy.

```python
@pytest.mark.usefixtures('test_app_copy')
def test_idf_copy(idf_copy):
    # idf_copy is the temporary IDF copy.
    # For example, we can check if idf.py build can work without the .git directory:
    shutil.rmtree(os.path.join(idf_copy, '.git'), ignore_errors=True)
    # Note that we can't use idf_py fixture, since it uses the default IDF path.
    # We can use 'get_idf_build_env' with 'run_idf_py', instead:
    env = get_idf_build_env(idf_copy)
    run_idf_py('build', env=env)
```

### Build snapshots

`get_snapshot(list_of_globs)` function takes a list of glob expressions, finds the files matching these expressions, and returns a `Snapshot` instance. `Snapshot` instances record file names and their modification timestamps. Two `Snapshot` instances can be compared using `assert_same` and `assert_different` methods:

```python
@pytest.mark.usefixtures('test_app_copy')
def test_build_jsons_updated_by_reconfigure(idf_py):
    globs = ['build/*.json']
    
    idf_py('reconfigure')
    snapshot_1 = get_snapshot(globs)
    snapshot_2 = get_snapshot(globs)
    snapshot_2.assert_same(snapshot_1)

    idf_py('reconfigure')
    snapshot_3 = get_snapshot(globs)
    snapshot_3.assert_different(snapshot_2)
```

### Helper functions for file modifications

A few extra functions are provided to make simple file modifications:

* `append_to_file(filename: typing.Union[str, Path], what: str) -> None` — appends the given string to a file.
* `replace_in_file(filename: typing.Union[str, Path], search: str, replace: str) -> None` — searches the file for occurrences of the string `search` and replaces all of them with `replace`, then writes the result back to the file.

### Application under test

Most build system tests should use the included [`build_test_app`](build_test_app/), if possible.

Using other test apps and examples for the purpose of testing the build system is okay as long as you keep the following in mind:

* Don't use the build system tests to compile examples or test apps under a particular combination of sdkconfig options. Use the `sdkconfig.ci.*` files for that, instead.
* Examples or test apps may be changed, renamed or removed. If you add a dependency on another example or a test app, your test case might need to be rewritten if someone has to modify or remove the example or a test app your test case depends on.

For convenience, the `build_test_app` app included here provides several placeholders which can be modified using the `replace_in_file` function. You can find the placeholders by running `grep -r placeholder_ build_test_app`.
