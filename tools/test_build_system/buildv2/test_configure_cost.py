# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import subprocess
import sys
from pathlib import Path

import pytest
from test_build_system_helpers import EnvDict

# The configure time depends on the CI runner and its load, so the test counts the work of a reconfigure instead:
# - CMake commands, from the CMake JSON trace.
# - Processes: execute_process() calls. A process is one command, but it takes 2-400 ms. A typical command takes
#   about 2 us.
# The counts do not depend on the machine. The project is the test app with COUNT generated components.
#
# If the test fails, measure the reconfigure time with and without the change, for example in
# examples/get-started/hello_world:
#     IDF_BUILD_V2=y idf.py reconfigure          # creates the build directory
#     time IDF_BUILD_V2=y idf.py reconfigure     # run several times
# If the configure is slower, fix the change. If it is not slower, or the extra time is accepted, set
# MEASURED_COMMANDS and MEASURED_PROCESSES to the counts from the failure message.
COUNT = 100
MEASURED_COMMANDS = 2217628
MEASURED_PROCESSES = 16
MAX_COMMANDS = int(MEASURED_COMMANDS * 1.1)
MAX_PROCESSES = MEASURED_PROCESSES


def count_reconfigure(app_path: Path, build_dir: Path, env: EnvDict) -> tuple[int, int]:
    """Configure the project, then count the CMake commands and processes of a reconfigure."""
    cmd = [
        'cmake',
        '-G',
        'Ninja',
        '-S',
        str(app_path),
        '-B',
        str(build_dir),
        f'-DSDKCONFIG={build_dir / "sdkconfig"}',
        '-DPYTHON_DEPS_CHECKED=1',
        # No `git describe` for the project version. In CI, the project is inside the IDF git repository.
        '-DPROJECT_VER=1',
    ]
    result = subprocess.run(cmd, env=env, capture_output=True, text=True)
    assert result.returncode == 0, result.stdout + result.stderr

    commands = 0
    processes = 0
    errors = []
    # The trace has several hundred MB, so it is counted from the pipe and not stored.
    with subprocess.Popen(
        cmd + ['--trace-format=json-v1'], env=env, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE
    ) as proc:
        assert proc.stderr is not None
        for line in proc.stderr:
            if b'"cmd":"' not in line:
                errors.append(line.decode(errors='replace'))
                continue
            commands += 1
            if b'"cmd":"execute_process"' in line:
                processes += 1
    assert proc.returncode == 0, ''.join(errors)
    return commands, processes


@pytest.mark.skipif(sys.platform != 'linux', reason='The limits are measured on Linux')
def test_configure_cost(test_app_copy: Path, default_idf_env: EnvDict) -> None:
    env = default_idf_env
    # CI sets this. Without it, the counts depend on the state of the IDF submodules.
    env['IDF_SKIP_CHECK_SUBMODULES'] = '1'
    # Default target and toolchain.
    env.pop('IDF_TARGET', None)
    env.pop('IDF_TOOLCHAIN', None)

    names = [f'comp_{i}' for i in range(COUNT)]
    for name in names:
        component_path = test_app_copy / 'components' / name
        (component_path / 'include').mkdir(parents=True)
        (component_path / 'CMakeLists.txt').write_text(
            'idf_component_register(SRCS "a.c" "b.c" INCLUDE_DIRS "include")\n'
        )
        (component_path / 'a.c').touch()
        (component_path / 'b.c').touch()
    (test_app_copy / 'main' / 'CMakeLists.txt').write_text(
        f'idf_component_register(SRCS "build_test_app.c" PRIV_REQUIRES {" ".join(names)})\n'
    )

    commands, processes = count_reconfigure(test_app_copy, test_app_copy / 'build', env)
    logging.info(f'Reconfigure: {commands} CMake commands, {processes} processes')
    assert commands <= MAX_COMMANDS and processes <= MAX_PROCESSES, (
        f'The reconfigure of {test_app_copy} executed {commands} CMake commands (limit {MAX_COMMANDS}) and started '
        f'{processes} processes (limit {MAX_PROCESSES}). See the comment at the top of {Path(__file__).name}.'
    )
