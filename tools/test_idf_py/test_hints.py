#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import sys
import tempfile
import unittest
from pathlib import Path
from subprocess import run
from typing import List

import yaml

try:
    EXT_IDF_PATH = os.environ['IDF_PATH']  # type: str
except KeyError:
    print(('This test needs to run within ESP-IDF environmnet. '
           'Please run export script first.'), file=sys.stderr)
    exit(1)

CWD = os.path.join(os.path.dirname(__file__))
ERR_OUT_YML = os.path.join(CWD, 'error_output.yml')

try:
    from idf_py_actions.tools import generate_hints
except ImportError:
    sys.path.append(os.path.join(CWD, '..'))
    from idf_py_actions.tools import generate_hints


class TestHintsMassages(unittest.TestCase):
    def setUp(self) -> None:
        self.tmpdir = tempfile.TemporaryDirectory()

    def test_output(self) -> None:
        with open(ERR_OUT_YML) as f:
            error_output = yaml.safe_load(f)

        error_filename = os.path.join(self.tmpdir.name, 'hint_input')
        for error, hint in error_output.items():
            with open(error_filename, 'w') as f:
                f.write(error)
            for generated_hint in generate_hints(f.name):
                self.assertEqual(generated_hint, hint)

    def tearDown(self) -> None:
        self.tmpdir.cleanup()


def run_idf(args: List[str], cwd: Path) -> str:
    # Simple helper to run idf command and return it's stdout.
    cmd = [
        sys.executable,
        os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py')
    ]
    proc = run(cmd + args, capture_output=True, cwd=cwd, text=True)
    return str(proc.stdout + proc.stderr)


class TestHintModuleComponentRequirements(unittest.TestCase):
    def setUp(self) -> None:
        # Set up a dummy project in tmp directory with main and component1 component.
        # The main component includes component1.h from component1, but the header dir is
        # not added in INCLUDE_DIRS and main doesn't have REQUIRES for component1.
        self.tmpdir = tempfile.TemporaryDirectory()
        self.tmpdirpath = Path(self.tmpdir.name)

        self.projectdir = self.tmpdirpath / 'project'
        self.projectdir.mkdir(parents=True)
        (self.projectdir / 'CMakeLists.txt').write_text((
            'cmake_minimum_required(VERSION 3.16)\n'
            'include($ENV{IDF_PATH}/tools/cmake/project.cmake)\n'
            'project(foo)'))

        maindir = self.projectdir / 'main'
        maindir.mkdir()
        (maindir / 'CMakeLists.txt').write_text('idf_component_register(SRCS "foo.c" REQUIRES esp_timer)')
        (maindir / 'foo.h').write_text('#include "component1.h"')
        (maindir / 'foo.c').write_text('#include "foo.h"\nvoid app_main(){}')

        component1dir = self.projectdir / 'components' / 'component1'
        component1dir.mkdir(parents=True)
        (component1dir / 'CMakeLists.txt').write_text('idf_component_register()')
        (component1dir / 'component1.h').touch()

    def test_component_requirements(self) -> None:
        # The main component uses component1.h, but this header is not in component1 public
        # interface. Hints should suggest that component1.h should be added into INCLUDE_DIRS
        # of component1.
        output = run_idf(['app'], self.projectdir)
        self.assertIn('Missing "component1.h" file name found in the following component(s): component1(', output)

        # Based on previous hint the component1.h is added to INCLUDE_DIRS, but main still doesn't
        # have dependency on compoment1. Hints should suggest to add component1 into main component
        # PRIV_REQUIRES, because foo.h is not in main public interface.
        run_idf(['fullclean'], self.projectdir)
        component1cmake = self.projectdir / 'components' / 'component1' / 'CMakeLists.txt'
        component1cmake.write_text('idf_component_register(INCLUDE_DIRS ".")')
        output = run_idf(['app'], self.projectdir)
        self.assertIn('To fix this, add component1 to PRIV_REQUIRES list of idf_component_register call', output)

        # Add foo.h into main public interface. Now the hint should suggest to use
        # REQUIRES instead of PRIV_REQUIRES.
        run_idf(['fullclean'], self.projectdir)
        maincmake = self.projectdir / 'main' / 'CMakeLists.txt'
        maincmake.write_text(('idf_component_register(SRCS "foo.c" '
                              'REQUIRES esp_timer '
                              'INCLUDE_DIRS ".")'))
        output = run_idf(['app'], self.projectdir)
        self.assertIn('To fix this, add component1 to REQUIRES list of idf_component_register call', output)

        # Add component1 to REQUIRES as suggested by previous hint, but also
        # add esp_psram as private req for component1 and add esp_psram.h
        # to component1.h. Now the hint should report that esp_psram should
        # be moved from PRIV_REQUIRES to REQUIRES for component1.
        run_idf(['fullclean'], self.projectdir)
        maincmake.write_text(('idf_component_register(SRCS "foo.c" '
                              'REQUIRES esp_timer component1 '
                              'INCLUDE_DIRS ".")'))
        (self.projectdir / 'components' / 'component1' / 'component1.h').write_text('#include "esp_psram.h"')
        component1cmake.write_text('idf_component_register(INCLUDE_DIRS "." PRIV_REQUIRES esp_psram)')
        output = run_idf(['app'], self.projectdir)
        self.assertIn('To fix this, move esp_psram from PRIV_REQUIRES into REQUIRES', output)

    def tearDown(self) -> None:
        self.tmpdir.cleanup()


class TestNestedModuleComponentRequirements(unittest.TestCase):
    def setUp(self) -> None:
        # Set up a nested component structure. The components directory contains
        # component1, which also contains foo project with main component.
        # components/component1/project/main
        #            ^^^^^^^^^^         ^^^^
        #            component          nested component
        # Both components include esp_timer.h, but only component1 has esp_timer
        # in requirements.
        self.tmpdir = tempfile.TemporaryDirectory()
        self.tmpdirpath = Path(self.tmpdir.name)

        components = self.tmpdirpath / 'components'
        maindir = components / 'component1'
        maindir.mkdir(parents=True)
        (maindir / 'CMakeLists.txt').write_text('idf_component_register(SRCS "component1.c" PRIV_REQUIRES esp_timer)')
        (maindir / 'component1.c').write_text('#include "esp_timer.h"')

        self.projectdir = maindir / 'project'
        self.projectdir.mkdir(parents=True)
        (self.projectdir / 'CMakeLists.txt').write_text((
            'cmake_minimum_required(VERSION 3.16)\n'
            f'set(EXTRA_COMPONENT_DIRS "{components.as_posix()}")\n'
            'set(COMPONENTS main)\n'
            'include($ENV{IDF_PATH}/tools/cmake/project.cmake)\n'
            'project(foo)'))

        maindir = self.projectdir / 'main'
        maindir.mkdir()
        (maindir / 'CMakeLists.txt').write_text('idf_component_register(SRCS "foo.c" REQUIRES component1)')
        (maindir / 'foo.c').write_text('#include "esp_timer.h"\nvoid app_main(){}')

    def test_nested_component_requirements(self) -> None:
        # Verify that source component for a failed include is properly identified
        # when components are nested. The main component should be identified as the
        # real source, not the component1 component.
        output = run_idf(['app'], self.projectdir)
        self.assertNotIn('esp_timer.h found in component esp_timer which is already in the requirements list of component1', output)
        self.assertIn('To fix this, add esp_timer to PRIV_REQUIRES list of idf_component_register call', output)

    def tearDown(self) -> None:
        self.tmpdir.cleanup()


class TestTrimmedModuleComponentRequirements(unittest.TestCase):
    def setUp(self) -> None:
        # Set up a dummy project with a trimmed down list of components and main component.
        # The main component includes "esp_http_client.h", but the esp_http_client
        # component is not added to main's requirements.
        self.tmpdir = tempfile.TemporaryDirectory()
        self.tmpdirpath = Path(self.tmpdir.name)

        self.projectdir = self.tmpdirpath / 'project'
        self.projectdir.mkdir(parents=True)
        (self.projectdir / 'CMakeLists.txt').write_text((
            'cmake_minimum_required(VERSION 3.16)\n'
            'set(COMPONENTS main)\n'
            'include($ENV{IDF_PATH}/tools/cmake/project.cmake)\n'
            'project(foo)'))

        maindir = self.projectdir / 'main'
        maindir.mkdir()
        (maindir / 'CMakeLists.txt').write_text('idf_component_register(SRCS "foo.c")')
        (maindir / 'foo.c').write_text('#include "esp_http_client.h"\nvoid app_main(){}')

    def test_trimmed_component_requirements(self) -> None:
        output = run_idf(['app'], self.projectdir)
        self.assertIn('To fix this, add esp_http_client to PRIV_REQUIRES list of idf_component_register call in', output)

    def tearDown(self) -> None:
        self.tmpdir.cleanup()


if __name__ == '__main__':
    unittest.main()
