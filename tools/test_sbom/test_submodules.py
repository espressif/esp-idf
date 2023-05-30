# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
from subprocess import check_output


def run_cmd(cmd):  # type: (list[str]) -> str
    """Simple helper to run command and return it's stdout."""
    return check_output(cmd, universal_newlines=True).strip()


def get_gitwdir():  # type: () -> str
    """Return absolute path to the current git working tree."""
    return run_cmd(['git', 'rev-parse', '--show-toplevel'])


def get_submodules_info():  # type () -> list[dict[str,str]]
    """Return list of submodules, where each submodule is represented
    as dictionary with name, path and hash keys."""
    cmd = ['git', 'submodule', '--quiet', 'foreach','echo "$name,$sm_path,$sha1"']
    out = run_cmd(cmd)
    submodules = []
    for line in out.splitlines():
        name, sm_path, sha1 = line.split(',')
        submodules += [{'name': name, 'path': sm_path, 'hash': sha1}]

    return submodules


def get_submodules_config():  # type () -> dict[str,str]
    """Return dictionary, where key is variable name and value
    is variable value in git's --list(dot) format. Only variables
    starting with "submodule." are returned and this prefix is removed
    to make it simple to match against the submodule info dictionary."""
    gitmodules_fn = os.path.join(get_gitwdir(), '.gitmodules')
    gitmodules_data = run_cmd(['git', 'config', '--list', '--file', gitmodules_fn])
    prefix = 'submodule.'
    config = {}
    for line in gitmodules_data.splitlines():
        var, val = line.split('=', 1)
        if not var.startswith(prefix):
            continue
        # remove "submodule." prefix
        var = var[len(prefix):]
        config[var] = val

    return config


def test_sha():  # type: () -> None
    """ Check that submodule SHA1 in git-tree and .gitmodules match
    if sbom-hash variable is available in the .gitmodules file.
    """
    submodules = get_submodules_info()
    config = get_submodules_config()

    for submodule in submodules:
        sbom_hash = config.get(submodule['name'] + '.sbom-hash')
        if not sbom_hash:
            continue
        msg = ('Submodule \"{sn}\" SHA \"{sh}\" in git '
               'tree does not match SHA \"{h}\" recorded in .gitmodules. '
               'Please update \"sbom-hash\" in .gitmodules for \"{sn}\" '
               'and also please do not forget to update version and other submodule '
               'information if necessary. It is important to keep this information '
               'up-to-date for SBOM generation.').format(sn=submodule['name'], sh=submodule['hash'], h=sbom_hash)
        assert submodule['hash'] == sbom_hash, msg


if __name__ == '__main__':
    test_sha()
