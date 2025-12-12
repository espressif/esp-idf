#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# Sphinx extension to generate ReSTructured Text .inc snippets
# with version-based content for this IDF version

import os
import re
import subprocess
from typing import Any
from typing import cast

from esp_docs.util.util import copy_if_modified

TEMPLATES = {
    'en': {
        'git-clone-bash': """
.. code-block:: bash

    mkdir -p ~/esp
    cd ~/esp
    git clone %(clone_args)s--single-branch --recursive https://github.com/espressif/esp-idf.git
        """,
        'version-note': {
            'master': """
.. note::

     This is documentation for the master branch (latest version) of ESP-IDF.
     This version is under continual development. `Stable version`_ documentation
     is available, as well as other :doc:`/versions`.
""",
            'stable': """
.. note::

     This is documentation for stable version %s of ESP-IDF.
     Other :doc:`/versions` are also available.
""",
            'branch': """
.. note::

     This is documentation for %s ``%s`` of ESP-IDF.
     Other :doc:`/versions` are also available.
""",
        },  # version-note
    },  # en
    'zh_CN': {
        'git-clone-bash': """
.. code-block:: bash

    mkdir -p ~/esp
    cd ~/esp
    git clone %(clone_args)s--single-branch --recursive https://github.com/espressif/esp-idf.git
            """,
        'version-note': {
            'master': """
.. note::

     这是 ESP-IDF master 分支（最新版本）的文档，该版本在持续开发中。还有
     `Stable version`_ 的文档，以及其他版本的文档 :doc:`/versions` 供参考。
""",
            'stable': """
.. note::

     这是 ESP-IDF 稳定版本 %s 的文档，还有其他版本的文档 :doc:`/versions` 供参考。
""",
            'branch': """
.. note::

     这是 ESP-IDF %s ``%s`` 版本的文档，还有其他版本的文档 :doc:`/versions` 供参考。
""",
        },  # version-note
    },  # zh_CN
}


def setup(app: Any) -> dict[str, Any]:
    # doesn't need to be this event specifically, but this is roughly the right time
    app.connect('project-build-info', generate_version_specific_includes)
    return {'parallel_read_safe': True, 'parallel_write_safe': True, 'version': '0.1'}


def generate_version_specific_includes(app: Any, project_description: Any) -> None:
    language = app.config.language
    tmp_out_dir = os.path.join(app.config.build_dir, 'version_inc')
    if not os.path.exists(tmp_out_dir):
        print(f'Creating directory {tmp_out_dir}')
        os.mkdir(tmp_out_dir)

    template = TEMPLATES[language]

    version, ver_type, is_stable = get_version()

    write_git_clone_inc_files(template, tmp_out_dir, version, ver_type, is_stable)
    version_note_template = cast(dict[str, Any], template['version-note'])
    write_version_note(version_note_template, tmp_out_dir, version, ver_type, is_stable)
    copy_if_modified(tmp_out_dir, os.path.join(app.config.build_dir, 'inc'))
    print('Done')


def write_git_clone_inc_files(
    template: dict[str, Any], out_dir: str, version: str, ver_type: str, is_stable: bool
) -> None:
    def out_file(basename: str) -> str:
        p = os.path.join(out_dir, f'{basename}.inc')
        print(f'Writing {p}...')
        return p

    if version == 'master':
        clone_args = ''
    else:
        clone_args = f'-b {version} '

    with open(out_file('git-clone-bash'), 'w', encoding='utf-8') as f:
        f.write(template['git-clone-bash'] % locals())

    print('Wrote git-clone-xxx.inc files')


def write_version_note(template: dict[str, Any], out_dir: str, version: str, ver_type: str, is_stable: bool) -> None:
    if version == 'master':
        content = template['master']
    elif ver_type == 'tag' and is_stable:
        content = template['stable'] % version
    else:
        content = template['branch'] % (ver_type, version)
    out_file = os.path.join(out_dir, 'version-note.inc')
    with open(out_file, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f'{out_file} written')


def get_version() -> tuple[str, str, bool]:
    """
    Returns a tuple of (name of branch/tag/commit-id, type branch/tag/commit, is_stable)
    """
    # Use git to look for a tag
    try:
        tag = subprocess.check_output(['git', 'describe', '--exact-match']).strip().decode('utf-8')
        is_stable = re.match(r'v[0-9\.]+$', tag) is not None
        return (tag, 'tag', is_stable)
    except subprocess.CalledProcessError:
        pass

    # No tag, look at branch name from CI, this will give the correct branch name even if the ref for the branch we
    # merge into has moved forward before the pipeline runs
    branch = os.environ.get('CI_COMMIT_REF_NAME', None)
    if branch is not None:
        return (branch, 'branch', False)

    # Try to find the branch name even if docs are built locally
    branch = subprocess.check_output(['git', 'rev-parse', '--abbrev-ref', 'HEAD']).strip().decode('utf-8')
    if branch != 'HEAD':
        return (branch, 'branch', False)

    # As a last resort we return commit SHA-1, should never happen in CI/docs that should be published
    return (subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).strip().decode('utf-8'), 'commit', False)
