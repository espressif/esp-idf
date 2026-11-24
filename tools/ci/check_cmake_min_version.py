#!/usr/bin/env python
# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Update CMakeLists.txt files to the minimum CMake version in tools.json.

The minimum is the lowest cmake entry whose status is "supported".
Versions are compared by major.minor, so 3.22 meets a minimum of 3.22.1.

Too-old versions are rewritten in place, which makes pre-commit report the files as
modified. Policy ranges (VERSION <min>...<max>) are reported instead of rewritten,
because raising <min> above <max> makes CMake fail. The report includes the whole
declaration and says the upper bound must move as well.

Only paths the superproject tracks are considered, so submodule contents are left
alone. Bracket comments are blanked, and the command has to start the line, so
comments and strings are not rewritten.
"""

import argparse
import json
import os
import re
import subprocess
import sys
from collections.abc import Iterable
from dataclasses import dataclass

IDF_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
DEFAULT_TOOLS_JSON = os.path.join(IDF_PATH, 'tools', 'tools.json')

# In VERSION 3.16...3.22 the required minimum is the first number, 3.16.
# The version can be quoted. The command has to start the line, so a trailing
# comment or a string on the same line is not a declaration.
CMAKE_MIN_RE = re.compile(
    r'^\s*cmake_minimum_required\s*\(\s*VERSION\s+"?(\d+(?:\.\d+)*)',
    re.IGNORECASE | re.MULTILINE,
)
# #[=*[ ... ]=*] . The equals signs have to match. Blanking keeps the length,
# so match offsets still refer to the original text.
BRACKET_COMMENT_RE = re.compile(r'#\[(=*)\[.*?\]\1\]', re.DOTALL)


@dataclass(frozen=True)
class Declaration:
    """One cmake_minimum_required command, located by line and by offset in the file text."""

    lineno: int
    version: str
    start: int
    end: int
    line_start: int

    def is_policy_range(self, text: str) -> bool:
        """Tell whether this is the VERSION <min>...<max> form, where version is <min>."""
        return text.startswith('...', self.end)

    def shown(self, text: str) -> str:
        """Return the whole command on one line, including a policy range."""
        close = text.find(')', self.end)
        snippet = text[self.line_start :] if close < 0 else text[self.line_start : close + 1]
        return ' '.join(snippet.split())


def version_key(version: str) -> tuple[int, int]:
    """Return (major, minor) so that 3.22 and 3.22.1 compare as equal."""
    parts = version.split('.')
    return int(parts[0]), int(parts[1]) if len(parts) > 1 else 0


def version_text(version: tuple[int, int]) -> str:
    return f'{version[0]}.{version[1]}'


def get_minimum_cmake_version(tools_json: str = DEFAULT_TOOLS_JSON) -> tuple[int, int]:
    """Return the lowest supported CMake version as (major, minor)."""
    with open(tools_json, encoding='utf-8') as tools_file:
        tools_info = json.load(tools_file)

    versions = [
        version_key(entry['name'])
        for tool in tools_info['tools']
        if tool['name'] == 'cmake'
        for entry in tool['versions']
        if entry['status'] == 'supported'
    ]
    if not versions:
        raise ValueError(f'no supported cmake version found in {tools_json}')
    return min(versions)


def without_bracket_comments(text: str) -> str:
    """Replace each #[=*[ ... ]=*] comment with spaces of the same length."""
    return BRACKET_COMMENT_RE.sub(lambda match: ' ' * len(match.group(0)), text)


def find_declarations(text: str) -> list[Declaration]:
    """Return every cmake_minimum_required command in text, ignoring comments and strings."""
    return [
        Declaration(
            text.count('\n', 0, match.start(1)) + 1,
            match.group(1),
            match.start(1),
            match.end(1),
            match.start(),
        )
        for match in CMAKE_MIN_RE.finditer(without_bracket_comments(text))
    ]


def repo_relative(path: str) -> str | None:
    """Return path relative to this repository, or None when it lies outside."""
    absolute = os.path.normpath(path if os.path.isabs(path) else os.path.join(IDF_PATH, path))
    try:
        if os.path.commonpath([absolute, IDF_PATH]) != IDF_PATH:
            return None
    except ValueError:
        return None
    return os.path.relpath(absolute, IDF_PATH).replace('\\', '/')


def tracked_paths(paths: Iterable[str]) -> list[str]:
    """Return the paths the superproject tracks, in the order they were given.

    One ``git ls-files -z`` call. Submodule contents, including nested submodules,
    are not in the superproject index, so they are left out. Paths outside this
    repository are left out too.
    """
    inside = [(path, relative) for path in paths if (relative := repo_relative(path)) is not None]
    if not inside:
        return []

    completed = subprocess.run(
        ['git', 'ls-files', '-z', '--', *[path for path, _relative in inside]],
        cwd=IDF_PATH,
        check=False,
        capture_output=True,
    )
    if completed.returncode != 0:
        message = completed.stderr.decode(errors='replace').strip() or 'git ls-files failed'
        raise OSError(message)

    tracked = {entry.decode().replace('\\', '/') for entry in completed.stdout.split(b'\0') if entry}
    return [path for path, relative in inside if relative in tracked]


def fix_text(text: str, minimum: tuple[int, int]) -> tuple[str, list[Declaration]]:
    """
    Returns the new text and the declarations that were rewritten. A declaration using a
    policy range is left alone, because raising its minimum above its own maximum would
    make CMake fail; find_declarations() on the returned text reports what is still too old.
    """
    pieces: list[str] = []
    rewritten: list[Declaration] = []
    end_of_previous = 0
    for declaration in find_declarations(text):
        if version_key(declaration.version) >= minimum or declaration.is_policy_range(text):
            continue
        pieces.append(text[end_of_previous : declaration.start])
        pieces.append(version_text(minimum))
        end_of_previous = declaration.end
        rewritten.append(declaration)
    pieces.append(text[end_of_previous:])
    return ''.join(pieces), rewritten


def fix_files(paths: Iterable[str], minimum: tuple[int, int]) -> tuple[list[str], list[str]]:
    """
    Returns the messages for what was rewritten and what needs manual action to resolve.
    """
    minimum_text = version_text(minimum)
    rewritten: list[str] = []
    needs_manual_action: list[str] = []
    for path in paths:
        if not os.path.isfile(path):
            continue
        # newline='' keeps the original line endings out of the round trip.
        with open(path, encoding='utf-8', newline='') as cmake_file:
            text = cmake_file.read()
        fixed_text, fixed = fix_text(text, minimum)
        if fixed_text != text:
            with open(path, 'w', encoding='utf-8', newline='') as cmake_file:
                cmake_file.write(fixed_text)
        rewritten.extend(
            f'- {path}:{declaration.lineno}: raised cmake_minimum_required to VERSION {minimum_text}'
            for declaration in fixed
        )
        for declaration in find_declarations(fixed_text):
            if version_key(declaration.version) >= minimum:
                continue
            message = (
                f'- {path}:{declaration.lineno}: {declaration.shown(fixed_text)} '
                f'is older than the minimum supported CMake {minimum_text} from tools/tools.json'
            )
            if declaration.is_policy_range(fixed_text):
                message += f'. The upper bound must also be at least {minimum_text}.'
            needs_manual_action.append(message)
    return rewritten, needs_manual_action


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description='Check cmake_minimum_required against tools/tools.json')
    parser.add_argument('filenames', nargs='*', help='CMakeLists.txt files to check')
    args = parser.parse_args(argv)

    try:
        minimum = get_minimum_cmake_version()
        rewritten, needs_manual_action = fix_files(tracked_paths(args.filenames), minimum)
    except (OSError, ValueError, KeyError, TypeError) as exc:
        print(f'check_cmake_min_version: {exc}')
        return 1

    if rewritten:
        for message in rewritten:
            print(message)
        print('The files above were updated, review the changes and stage them.')
    if needs_manual_action:
        for message in needs_manual_action:
            print(message)
        if any('The upper bound must also' not in message for message in needs_manual_action):
            print(f'Please update the minimum required CMake version to at least {version_text(minimum)}.')
    return 1 if rewritten or needs_manual_action else 0


if __name__ == '__main__':
    sys.exit(main())
