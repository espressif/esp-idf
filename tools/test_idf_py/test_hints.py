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


class TestHintModuleComponentRequirements(unittest.TestCase):
    def run_idf(self, args: List[str]) -> str:
        # Simple helper to run idf command and return it's stdout.
        cmd = [
            sys.executable,
            os.path.join(os.environ['IDF_PATH'], 'tools', 'idf.py')
        ]
        proc = run(cmd + args, capture_output=True, cwd=str(self.projectdir), text=True)
        return proc.stdout + proc.stderr

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
        output = self.run_idf(['app'])
        self.assertIn('Missing "component1.h" file name found in the following component(s): component1(', output)

        # Based on previous hint the component1.h is added to INCLUDE_DIRS, but main still doesn't
        # have dependency on compoment1. Hints should suggest to add component1 into main component
        # PRIV_REQUIRES, because foo.h is not in main public interface.
        self.run_idf(['fullclean'])
        component1cmake = self.projectdir / 'components' / 'component1' / 'CMakeLists.txt'
        component1cmake.write_text('idf_component_register(INCLUDE_DIRS ".")')
        output = self.run_idf(['app'])
        self.assertIn('To fix this, add component1 to PRIV_REQUIRES list of idf_component_register call', output)

        # Add foo.h into main public interface. Now the hint should suggest to use
        # REQUIRES instead of PRIV_REQUIRES.
        self.run_idf(['fullclean'])
        maincmake = self.projectdir / 'main' / 'CMakeLists.txt'
        maincmake.write_text(('idf_component_register(SRCS "foo.c" '
                              'REQUIRES esp_timer '
                              'INCLUDE_DIRS ".")'))
        output = self.run_idf(['app'])
        self.assertIn('To fix this, add component1 to REQUIRES list of idf_component_register call', output)

        # Add component1 to REQUIRES as suggested by previous hint, but also
        # add esp_psram as private req for component1 and add esp_psram.h
        # to component1.h. New the hint should report that esp_psram should
        # be moved from PRIV_REQUIRES to REQUIRES for component1.
        self.run_idf(['fullclean'])
        maincmake.write_text(('idf_component_register(SRCS "foo.c" '
                              'REQUIRES esp_timer component1 '
                              'INCLUDE_DIRS ".")'))
        (self.projectdir / 'components' / 'component1' / 'component1.h').write_text('#include "esp_psram.h"')
        component1cmake.write_text('idf_component_register(INCLUDE_DIRS "." PRIV_REQUIRES esp_psram)')
        output = self.run_idf(['app'])
        self.assertIn('To fix this, move esp_psram from PRIV_REQUIRES into REQUIRES', output)

    def tearDown(self) -> None:
        self.tmpdir.cleanup()


if __name__ == '__main__':
    unittest.main()
