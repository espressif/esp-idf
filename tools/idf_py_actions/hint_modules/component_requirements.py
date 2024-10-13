# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import re
from typing import Optional

from idf_py_actions.tools import get_build_context

'''
glossary:
orignal_component:   component which compilation failed
source_component:    component containing file which is including the missing header file
candidate_component: component which contain the missing header file
original_filename:   abs path of file(compilation unit) in original_component
source_filename:     abs path of file in source_component which is including the missing header file
missing_header:      filename of the missing header included in source_filename
'''

# Regex to find source_filename in preprocessor's error message
ENOENT_RE = re.compile(r'^(.+):\d+:\d+: fatal error: (.+): No such file or directory$',
                       flags=re.MULTILINE)
# Regex to find full preprocessor's error message to identify the original_filename
# in case the missing_header is reported in indirect include.
ENOENT_FULL_RE = re.compile(r'^(In file included.*No such file or directory)$',
                            flags=re.MULTILINE | re.DOTALL)
# Regex to find original_filename in preprocessor's error message
ORIGINAL_FILE_RE = re.compile(r'.*from (.*):[\d]+:')


def _bug(msg: str) -> str:
    return f'BUG: {os.path.basename(__file__)}: {msg}'


def _get_absolute_path(filename: str, base: str) -> str:
    # If filename path is relative, return absolute path based
    # on base directory. The filename is normalized in any case.
    if not os.path.isabs(filename):
        filename = os.path.join(base, filename)
    filename = os.path.normpath(filename)
    return filename


def generate_hint(output: str) -> Optional[str]:
    # get the project description
    proj_desc = get_build_context().get('proj_desc')
    if not proj_desc:
        # hints cannot be generated because we are not in the build context,
        # meaning ensure_build_directory() was not ran and project description
        # is not available
        return None

    hint_match = ENOENT_RE.search(output)
    if not hint_match:
        return None

    # this is the file where the error has occurred
    source_filename = _get_absolute_path(hint_match.group(1), proj_desc['build_dir'])

    # this is the header file we tried to include
    missing_header = hint_match.group(2)

    # find the source_component that contains the source file
    found_source_component_name = None
    found_source_component_info = None
    for component_name, component_info in proj_desc['all_component_info'].items():
        # look if the source_filename is within a component directory, not only
        # at component_info['sources'], because the missing file may be included
        # from header file, which is not present in component_info['sources']
        component_dir = os.path.normpath(component_info['dir'])
        if source_filename.startswith(component_dir):
            if found_source_component_info and len(found_source_component_info['dir']) >= len(component_dir):
                continue
            found_source_component_info = component_info
            found_source_component_name = component_name
    if not found_source_component_name:
        # The source file is not in any component.
        # It could be in a subproject added via ExternalProject_Add, in which case
        # we can't help much.
        return None

    # find the original_component, which may be different from source_component
    found_original_component_name = found_source_component_name
    found_original_component_info = found_source_component_info
    original_filename = source_filename
    hint_match_full = ENOENT_FULL_RE.search(output)
    if hint_match_full:
        # As a precaution remove empty lines, but there should be none.
        lines = [line for line in hint_match_full.group().splitlines() if line]
        # second line from the end contains filename which is part of the
        # original_component
        original_file_match = ORIGINAL_FILE_RE.match(lines[-2])
        if original_file_match:
            original_filename = _get_absolute_path(original_file_match.group(1), proj_desc['build_dir'])
            for component_name, component_info in proj_desc['all_component_info'].items():
                component_dir = os.path.normpath(component_info['dir'])
                if original_filename.startswith(component_dir):
                    found_original_component_name = component_name
                    found_original_component_info = component_info
                    break
            else:
                # Original component not found. This should never happen, because the
                # original_filename has to part of some component, which was added to
                # the build.
                return _bug((f'cannot find original component for source '
                             f'component {found_source_component_name}'))

        else:
            # We should never reach this path. It would probably mean
            # the preprocessor output was changed or we received malformed
            # output.
            return _bug((f'cannot match original component filename for '
                         f'source component {found_source_component_name}'))

    # look for the header file in the public include directories of all components
    found_dep_component_names = []
    for candidate_component_name, candidate_component_info in proj_desc['all_component_info'].items():
        if candidate_component_name == found_source_component_name:
            # skip the component that contains the source file
            continue
        candidate_component_include_dirs = candidate_component_info['include_dirs']
        component_dir = os.path.normpath(candidate_component_info['dir'])
        for candidate_component_include_dir in candidate_component_include_dirs:
            candidate_header_path = os.path.join(component_dir, candidate_component_include_dir, missing_header)
            if os.path.exists(candidate_header_path):
                found_dep_component_names.append(candidate_component_name)
                break  # no need to look further in this component

    if not found_dep_component_names:
        # Header file not found in any component INCLUDE_DIRS. Try to scan whole component
        # directories if we can find the missing header there and notify user about possible
        # missing entry in INCLUDE_DIRS.
        candidate_component_include_dirs = []
        for component_name, component_info in proj_desc['all_component_info'].items():
            component_dir = os.path.normpath(component_info['dir'])
            for root, _, _ in os.walk(component_dir):
                full_path = os.path.normpath(os.path.join(root, missing_header))
                # sanity check that the full_path is still within component's directory
                if not full_path.startswith(component_dir):
                    continue
                if os.path.isfile(full_path):
                    candidate_component_include_dirs.append(f'{component_name}({full_path})')

        if candidate_component_include_dirs:
            candidates = ', '.join(candidate_component_include_dirs)
            return (f'Missing "{missing_header}" file name found in the following component(s): {candidates}. '
                    f'Maybe one of the components needs to add the missing header directory to INCLUDE_DIRS '
                    f'of idf_component_register call in CMakeLists.txt.')

        # The missing header not found anywhere, nothing much we can do here.
        return None

    assert found_source_component_info is not None  # to help mypy
    assert found_original_component_info is not None  # to help mypy

    # Sanity check: verify we didn't somehow find a component which is already in the requirements list
    all_reqs = (found_source_component_info['reqs']
                + found_source_component_info['managed_reqs'])
    if found_original_component_name == found_source_component_name:
        # Add also private reqs, but only if source_component is same original_component.
        # The missing_header may be part of component which is already added as private
        # req for source_component. Meaning it's not part of source_component public
        # interface.
        all_reqs += (found_source_component_info['priv_reqs']
                     + found_source_component_info['managed_priv_reqs'])

    for dep_component_name in found_dep_component_names:
        if dep_component_name in all_reqs:
            # Oops. This component is already in the requirements list.
            # How did this happen?
            return _bug((f'{missing_header} found in component {dep_component_name} '
                         f'which is already in the requirements list of {found_source_component_name}'))

    # try to figure out the correct require type: REQUIRES or PRIV_REQUIRES
    requires_type = None
    source_component_has_priv_dep = False
    if original_filename == source_filename:
        # The error is reported directly in compilation unit, so
        # missing_header should not be part of public interface.
        requires_type = 'PRIV_REQUIRES'
    elif found_original_component_name == found_source_component_name:
        # The original_component and source_component are the same and original_filename
        # is different from source_filename. Check if the source_file is part of the
        # original_component's public interface. If so, the REQUIRES should be used.
        for include_dir in found_original_component_info['include_dirs']:
            include_dir = _get_absolute_path(found_original_component_info['dir'], include_dir)
            if source_filename.startswith(include_dir):
                # source_filename is part of public interface
                requires_type = 'REQUIRES'
                break
        if not requires_type:
            # source_file not part of public interface, suggest PRIV_REQUIRES
            requires_type = 'PRIV_REQUIRES'
    else:
        # The source_filename is part of different component than the original_component, so
        # the source_component needs to use REQUIRES to make the missing_header available for
        # original_component.
        requires_type = 'REQUIRES'
        if len(found_dep_component_names) == 1:
            # If there is only one component found as missing dependency, look at
            # source_component private requires to see if the missing dependency is
            # already there. If so, we suggest to move it from PRIV_REQUIRES to REQUIRES.
            # This is done only if there is one component in found_dep_component_names, because
            # otherwise we cannot be sure which component should be moved.
            priv_reqs = (found_source_component_info['priv_reqs']
                         + found_source_component_info['managed_priv_reqs'])
            if found_dep_component_names[0] in priv_reqs:
                source_component_has_priv_dep = True

    found_dep_component_names_list = ', '.join(found_dep_component_names)
    source_filename_short = os.path.basename(source_filename)
    cmakelists_file_to_fix = os.path.normpath(os.path.join(found_source_component_info['dir'], 'CMakeLists.txt'))
    problem_description = (
        f'Compilation failed because {source_filename_short} (in "{found_source_component_name}" component) '
        f'includes {missing_header}, provided by {found_dep_component_names_list} component(s).\n')

    if source_component_has_priv_dep:
        problem_solution = (
            f'However, {found_dep_component_names_list} component(s) is in the private requirements list '
            f'of "{found_source_component_name}".\n'
            f'To fix this, move {found_dep_component_names_list} from PRIV_REQUIRES into '
            f'REQUIRES list of idf_component_register call in {cmakelists_file_to_fix}.')
    else:
        problem_solution = (
            f'However, {found_dep_component_names_list} component(s) is not in the requirements list '
            f'of "{found_source_component_name}".\n'
            f'To fix this, add {found_dep_component_names_list} to {requires_type} list '
            f'of idf_component_register call in {cmakelists_file_to_fix}.')

    return problem_description + problem_solution
