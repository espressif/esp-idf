#!/usr/bin/env python
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import contextlib
import io
import json
import os
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, os.path.dirname(__file__))

import check_cmake_min_version as check  # noqa: E402


def _tools(*versions: tuple[str, str]) -> dict:
    return {
        'tools': [
            {
                'name': 'cmake',
                'versions': [{'name': name, 'status': status} for name, status in versions],
            }
        ]
    }


def _write_tools(directory: str, *versions: tuple[str, str]) -> str:
    path = os.path.join(directory, 'tools.json')
    with open(path, 'w', encoding='utf-8') as tools_file:
        json.dump(_tools(*versions), tools_file)
    return path


class MinimumVersionTests(unittest.TestCase):
    def test_lowest_supported_version(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            tools_json = _write_tools(tmp, ('4.0.3', 'recommended'), ('3.22.1', 'supported'), ('3.16.0', 'deprecated'))
            self.assertEqual(check.get_minimum_cmake_version(tools_json), (3, 22))

    def test_lowest_of_several_supported_versions(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            tools_json = _write_tools(tmp, ('4.0.3', 'recommended'), ('3.25.0', 'supported'), ('3.22.1', 'supported'))
            self.assertEqual(check.get_minimum_cmake_version(tools_json), (3, 22))

    def test_recommended_only_is_rejected(self) -> None:
        # Every other tool in tools.json has only a recommended version. If cmake ever
        # takes that shape, this check needs to start honouring it.
        with tempfile.TemporaryDirectory() as tmp:
            tools_json = _write_tools(tmp, ('4.0.3', 'recommended'))
            with self.assertRaises(ValueError):
                check.get_minimum_cmake_version(tools_json)

    def test_missing_cmake_tool(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            path = os.path.join(tmp, 'tools.json')
            with open(path, 'w', encoding='utf-8') as tools_file:
                json.dump({'tools': [{'name': 'ninja', 'versions': []}]}, tools_file)
            with self.assertRaises(ValueError):
                check.get_minimum_cmake_version(path)

    def test_no_supported_version(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            tools_json = _write_tools(tmp, ('3.16.0', 'deprecated'))
            with self.assertRaises(ValueError):
                check.get_minimum_cmake_version(tools_json)


class DeclaredVersionTests(unittest.TestCase):
    def test_major_minor_meets_patch_minimum(self) -> None:
        minimum = check.version_key('3.22.1')
        self.assertEqual(minimum, (3, 22))
        self.assertGreaterEqual(check.version_key('3.22'), minimum)
        self.assertGreaterEqual(check.version_key('3.22.1'), minimum)
        self.assertGreaterEqual(check.version_key('4.0'), minimum)
        self.assertLess(check.version_key('3.16'), minimum)
        self.assertLess(check.version_key('3.5'), minimum)
        self.assertEqual(check.version_text(minimum), '3.22')

    def test_finds_version_and_skips_comments(self) -> None:
        text = '\n'.join(
            [
                '# cmake_minimum_required(VERSION 3.5)',
                'cmake_minimum_required(VERSION 3.16...3.22)',
                'cmake_minimum_required( VERSION 3.22 FATAL_ERROR )',
            ]
        )
        found = check.find_declarations(text)
        self.assertEqual([(d.lineno, d.version) for d in found], [(2, '3.16'), (3, '3.22')])
        self.assertEqual([d.is_policy_range(text) for d in found], [True, False])

    def test_finds_quoted_version(self) -> None:
        text = 'cmake_minimum_required(VERSION "3.16")\ncmake_minimum_required(VERSION "3.10...3.22")\n'
        found = check.find_declarations(text)
        self.assertEqual([(d.lineno, d.version) for d in found], [(1, '3.16'), (2, '3.10')])
        self.assertEqual([d.is_policy_range(text) for d in found], [False, True])

    def test_version_on_following_line(self) -> None:
        text = 'cmake_minimum_required(\n    VERSION 3.22\n)\n'
        found = check.find_declarations(text)
        self.assertEqual([(d.lineno, d.version) for d in found], [(2, '3.22')])
        self.assertEqual(text[found[0].start : found[0].end], '3.22')


class FixTextTests(unittest.TestCase):
    def test_raises_only_too_old_versions(self) -> None:
        text = 'cmake_minimum_required(VERSION 3.16)\nproject(x)\ncmake_minimum_required( VERSION 4.0 )\n'
        fixed_text, fixed = check.fix_text(text, (3, 22))
        self.assertEqual([d.lineno for d in fixed], [1])
        self.assertEqual(
            fixed_text,
            'cmake_minimum_required(VERSION 3.22)\nproject(x)\ncmake_minimum_required( VERSION 4.0 )\n',
        )

    def test_keeps_policy_ranges_and_comments(self) -> None:
        text = '# cmake_minimum_required(VERSION 3.5)\ncmake_minimum_required(VERSION 3.10...3.16)\n'
        fixed_text, fixed = check.fix_text(text, (3, 22))
        self.assertEqual(fixed, [])
        self.assertEqual(fixed_text, text)

    def test_raises_several_declarations_in_one_file(self) -> None:
        text = 'cmake_minimum_required(VERSION 3.5)\nproject(x)\ncmake_minimum_required(VERSION 3.16)\n'
        fixed_text, fixed = check.fix_text(text, (3, 22))
        self.assertEqual([d.lineno for d in fixed], [1, 3])
        self.assertEqual(
            fixed_text,
            'cmake_minimum_required(VERSION 3.22)\nproject(x)\ncmake_minimum_required(VERSION 3.22)\n',
        )

    def test_keeps_quotes_around_the_raised_version(self) -> None:
        fixed_text, fixed = check.fix_text('cmake_minimum_required(VERSION "3.16")\n', (3, 22))
        self.assertEqual([d.lineno for d in fixed], [1])
        self.assertEqual(fixed_text, 'cmake_minimum_required(VERSION "3.22")\n')

    def test_is_idempotent(self) -> None:
        once, _ = check.fix_text('cmake_minimum_required(VERSION 3.5)\n', (3, 22))
        twice, fixed = check.fix_text(once, (3, 22))
        self.assertEqual(twice, once)
        self.assertEqual(fixed, [])

    def test_raises_uppercase_command(self) -> None:
        fixed_text, fixed = check.fix_text('CMAKE_MINIMUM_REQUIRED(VERSION 3.5)\n', (3, 22))
        self.assertEqual([d.lineno for d in fixed], [1])
        self.assertEqual(fixed_text, 'CMAKE_MINIMUM_REQUIRED(VERSION 3.22)\n')

    def test_leaves_bracket_comment_and_rewrites_the_command_after_it(self) -> None:
        text = '#[[\ncmake_minimum_required(VERSION 3.5)\n]]\ncmake_minimum_required(VERSION 3.5)\n'
        fixed_text, fixed = check.fix_text(text, (3, 22))
        self.assertEqual([d.lineno for d in fixed], [4])
        self.assertEqual(
            fixed_text,
            '#[[\ncmake_minimum_required(VERSION 3.5)\n]]\ncmake_minimum_required(VERSION 3.22)\n',
        )

    def test_leaves_text_that_needs_no_edit(self) -> None:
        unchanged = [
            'cmake_minimum_required(VERSION 3.22)\n',
            'set(FOO 1) # cmake_minimum_required(VERSION 3.16)\n',
            'set(EXAMPLE "cmake_minimum_required(VERSION 3.16)")\n',
            '#[=[\ncmake_minimum_required(VERSION 3.5)\n]=]\n',
        ]
        for text in unchanged:
            with self.subTest(text=text):
                self.assertEqual(check.fix_text(text, (3, 22)), (text, []))


class FixFilesTests(unittest.TestCase):
    def test_updates_old_version_and_leaves_current_minimum_alone(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            old = os.path.join(tmp, 'old', 'CMakeLists.txt')
            current = os.path.join(tmp, 'current', 'CMakeLists.txt')
            os.makedirs(os.path.dirname(old))
            os.makedirs(os.path.dirname(current))
            with open(old, 'w', encoding='utf-8') as cmake_file:
                cmake_file.write('cmake_minimum_required(VERSION 3.16)\n')
            with open(current, 'w', encoding='utf-8') as cmake_file:
                cmake_file.write('project(no_minimum)\n')

            minimum = (3, 22)
            rewritten, needs_human = check.fix_files([old, current, os.path.join(tmp, 'missing.txt')], minimum)
            self.assertEqual(rewritten, [f'- {old}:1: raised cmake_minimum_required to VERSION 3.22'])
            self.assertEqual(needs_human, [])
            with open(old, encoding='utf-8') as cmake_file:
                self.assertEqual(cmake_file.read(), 'cmake_minimum_required(VERSION 3.22)\n')
            with open(current, encoding='utf-8') as cmake_file:
                self.assertEqual(cmake_file.read(), 'project(no_minimum)\n')

            stdout = io.StringIO()
            with contextlib.redirect_stdout(stdout):
                self.assertEqual(check.main([old]), 0)
            self.assertEqual(stdout.getvalue(), '')

    def test_reports_policy_range_instead_of_rewriting_it(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            path = os.path.join(tmp, 'CMakeLists.txt')
            with open(path, 'w', encoding='utf-8') as cmake_file:
                cmake_file.write('cmake_minimum_required(VERSION 3.10...3.16)\n')

            stdout = io.StringIO()
            with (
                patch.object(check, 'tracked_paths', side_effect=lambda paths: list(paths)),
                contextlib.redirect_stdout(stdout),
            ):
                self.assertEqual(check.main([path]), 1)
            output = stdout.getvalue()
            self.assertIn(
                f'- {path}:1: cmake_minimum_required(VERSION 3.10...3.16) '
                'is older than the minimum supported CMake 3.22 from tools/tools.json. '
                'The upper bound must also be at least 3.22.',
                output,
            )
            self.assertNotIn('Please update the minimum required CMake version to at least 3.22.', output)
            with open(path, encoding='utf-8') as cmake_file:
                self.assertEqual(cmake_file.read(), 'cmake_minimum_required(VERSION 3.10...3.16)\n')

    def test_main_skips_files_the_superproject_does_not_track(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            path = os.path.join(tmp, 'CMakeLists.txt')
            original = 'cmake_minimum_required(VERSION 3.5)\n'
            with open(path, 'w', encoding='utf-8') as cmake_file:
                cmake_file.write(original)

            self.assertEqual(check.main([path]), 0)
            with open(path, encoding='utf-8') as cmake_file:
                self.assertEqual(cmake_file.read(), original)

    def test_tracked_paths_keeps_only_index_entries(self) -> None:
        completed = subprocess.CompletedProcess(
            args=[], returncode=0, stdout=b'components/a/CMakeLists.txt\0', stderr=b''
        )
        with patch.object(check.subprocess, 'run', return_value=completed) as run:
            kept = check.tracked_paths(['components/a/CMakeLists.txt', 'components/mbedtls/mbedtls/CMakeLists.txt'])
        self.assertEqual(kept, ['components/a/CMakeLists.txt'])
        self.assertEqual(run.call_args.args[0][:4], ['git', 'ls-files', '-z', '--'])

    def test_preserves_crlf_line_endings(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            path = os.path.join(tmp, 'CMakeLists.txt')
            with open(path, 'wb') as cmake_file:
                cmake_file.write(b'cmake_minimum_required(VERSION 3.16)\r\nproject(x)\r\n')

            check.fix_files([path], (3, 22))
            with open(path, 'rb') as cmake_file:
                self.assertEqual(cmake_file.read(), b'cmake_minimum_required(VERSION 3.22)\r\nproject(x)\r\n')

    def test_repository_minimum_accepts_project_cmake(self) -> None:
        minimum = check.get_minimum_cmake_version()
        project_cmake = os.path.join(check.IDF_PATH, 'tools', 'cmake', 'project.cmake')
        with open(project_cmake, encoding='utf-8') as cmake_file:
            text = cmake_file.read()
        self.assertEqual(check.fix_text(text, minimum), (text, []))


if __name__ == '__main__':
    unittest.main()
