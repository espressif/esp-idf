# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
from subprocess import PIPE, run
from typing import Dict, List


def run_cmd(cmd: List[str]) -> str:
    """Simple helper to run command and return it's stdout."""
    proc = run(cmd, stdout=PIPE, stderr=PIPE, check=True, universal_newlines=True)
    return proc.stdout.strip()


def get_gitwdir() -> str:
    """Return absolute path to the current git working tree."""
    return run_cmd(['git', 'rev-parse', '--show-toplevel'])


def get_submodules_info() -> List[Dict[str,str]]:
    """Return list of submodules, where each submodule is represented
    as dictionary with name, path and hash keys."""
    cmd = ['git', 'submodule', '--quiet', 'foreach','echo "$name,$sm_path,$sha1"']
    out = run_cmd(cmd)
    submodules = []
    for line in out.splitlines():
        name, sm_path, sha1 = line.split(',')
        submodules += [{'name': name, 'path': sm_path, 'hash': sha1}]

    return submodules


def get_submodules_config() -> Dict[str,str]:
    """Return dictionary, where key is variable name and value
    is variable value in git's --list(dot) format. Only variables
    starting with "submodule." are returned and this prefix is removed
    to make it simple to match against the submodule info dictionary."""
    gitmodules_fn = os.path.join(get_gitwdir(), '.gitmodules')
    gitmodules_data = run_cmd(['git', 'config', '--list', '--file', gitmodules_fn])
    prefix = 'submodule.'
    config = {}
    for line in gitmodules_data.splitlines():
        var, val = line.split('=', maxsplit=1)
        if not var.startswith(prefix):
            continue
        # remove "submodule." prefix
        var = var[len(prefix):]
        config[var] = val

    return config


def test_sha() -> None:
    """ Check that submodule SHA1 in git-tree and .gitmodules match
    if sbom-hash variable is available in the .gitmodules file.
    """
    submodules = get_submodules_info()
    config = get_submodules_config()

    for submodule in submodules:
        sbom_hash = config.get(submodule['name'] + '.sbom-hash')
        if not sbom_hash:
            continue
        msg = (f'Submodule \"{submodule["name"]}\" SHA \"{submodule["hash"]}\" in git '
               f'tree does not match SHA \"{sbom_hash}\" recorded in .gitmodules. '
               f'Please update \"sbom-hash\" in .gitmodules for \"{submodule["name"]}\" '
               f'and also please do not forget to update version and other submodule '
               f'information if necessary. It is important to keep this information '
               f'up-to-date for SBOM generation.')
        assert submodule['hash'] == sbom_hash, msg


if __name__ == '__main__':
    test_sha()
