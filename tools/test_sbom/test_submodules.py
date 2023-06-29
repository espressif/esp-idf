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


def get_submodules_config():  # type () -> dict[str,dict[str,str]]
    """Return dictionary, where key is submodule name and value
    is a dictionary with variable:value pairs."""
    gitmodules_fn = os.path.join(get_gitwdir(), '.gitmodules')
    gitmodules_data = run_cmd(['git', 'config', '--list', '--file', gitmodules_fn])
    prefix = 'submodule.'
    config = {}  # type: dict[str, dict[str,str]]
    for line in gitmodules_data.splitlines():
        if not line.startswith(prefix):
            continue
        splitted = line.split('=', 1)
        if len(splitted) != 2:
            continue
        section, val = splitted
        # remove "submodule." prefix
        section = section[len(prefix):]
        # split section into module name and variable
        splitted = section.rsplit('.', 1)
        if len(splitted) != 2:
            continue
        module_name, var = splitted
        if module_name not in config:
            config[module_name] = {}
        config[module_name][var] = val

    return config


def test_sha():  # type: () -> None
    """ Check that submodule SHA in git-tree and .gitmodules match
    if sbom-hash variable is available in the .gitmodules file.
    """
    submodules = get_submodules_config()

    for name, variables in submodules.items():
        sbom_hash = variables.get('sbom-hash')
        if not sbom_hash:
            continue
        module_path = variables.get('path')
        if not module_path:
            continue
        output = run_cmd(['git', 'ls-tree', 'HEAD', module_path])
        if not output:
            continue
        module_hash = output.split()[2]
        msg = ('Submodule \"{name}\" SHA \"{module_hash}\" in git '
               'tree does not match SHA \"{sbom_hash}\" recorded in .gitmodules. '
               'Please update \"sbom-hash\" in .gitmodules for \"{name}\" '
               'and also please do not forget to update version and other submodule '
               'information if necessary. It is important to keep this information '
               'up-to-date for SBOM generation.').format(name=name, module_hash=module_hash, sbom_hash=sbom_hash)
        assert module_hash == sbom_hash, msg


if __name__ == '__main__':
    test_sha()
