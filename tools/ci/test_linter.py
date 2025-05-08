# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import ast
import itertools
import os
import typing as t
import warnings
from collections import defaultdict

import pytest
from idf_pytest.constants import PREVIEW_TARGETS
from idf_pytest.constants import SUPPORTED_TARGETS
from idf_pytest.constants import TARGET_MARKERS
from pytest import Config
from pytest import Function
from pytest import Mark


def is_target_in_marker(mark: Mark) -> bool:
    return mark.name in TARGET_MARKERS or mark.name in ('supported_targets', 'preview_targets', 'linux')


def remove_keys(data: t.Dict[str, t.Any], keys_to_remove: t.List[str]) -> t.Dict[str, t.Any]:
    """
    Remove specific keys from a dictionary.
    """
    return {key: value for key, value in data.items() if key not in keys_to_remove}


def get_values_by_keys(data: t.Dict[str, t.Any], keys: t.List[str]) -> t.Tuple[t.Any, ...]:
    """
    Retrieve values from a dictionary for specified keys.
    """
    return tuple([data[key] for key in keys if key in data])


def group_by_target(vals: t.List[t.Dict[str, t.Any]]) -> t.List[t.Dict[str, t.Any]]:
    """
    Groups rows by non-target keys and modifies targets to 'supported_targets'
    if all supported targets are present in a group.

    Parameters:
        vals: List of dictionaries to process.

    Returns:
        Processed list of dictionaries with supported targets.
    """
    if not vals or 'target' not in vals[0]:
        return vals

    def _process_group(
        _vals: t.List[t.Dict[str, t.Any]], group: t.List[str], group_name: str
    ) -> t.List[t.Dict[str, t.Any]]:
        # Identify keys excluding 'target'
        non_target_keys = [key for key in sorted(_vals[0].keys()) if key != 'target']

        # Group rows by values of keys excluding 'target'
        grouped_rows = defaultdict(list)
        for index, row in enumerate(_vals):
            key = get_values_by_keys(row, non_target_keys)
            grouped_rows[key].append((index, row['target']))

        # Identify groups that contain all supported targets
        to_skip_lines: t.Set[int] = set()
        to_update_lines: t.Set[int] = set()
        for _, rows in grouped_rows.items():
            lines = []
            remaining_targets = set(group)
            for index, target in rows:
                if target in remaining_targets:
                    lines.append(index)
                    remaining_targets.remove(target)
            if not remaining_targets:
                to_skip_lines.update(lines[1:])  # Skip all but the first matching line
                to_update_lines.add(lines[0])  # Update the first matching line
                break

        # Construct new list of rows with modifications
        new_values = []
        for ind, row in enumerate(_vals):
            if ind in to_update_lines:
                row['target'] = group_name
            if ind not in to_skip_lines:
                new_values.append(row)

        return new_values

    if SUPPORTED_TARGETS:
        vals = _process_group(vals, SUPPORTED_TARGETS, 'supported_targets')
    if PREVIEW_TARGETS:
        vals = _process_group(vals, PREVIEW_TARGETS, 'preview_targets')
    return vals


class CurrentItemContext:
    test_name: str


class PathRestore:
    # If restored is True, then add the import os when the file is being formatted.
    restored: bool = False

    def __init__(self, path: str) -> None:
        PathRestore.restored = True
        self.path = path

    def __repr__(self) -> str:
        return f"f'{self.path}'"


def restore_path(vals: t.List[t.Dict[str, t.Any]], file_path: str) -> t.List[t.Dict[str, t.Any]]:
    if 'app_path' not in vals[0].keys():
        return vals
    file_path = os.path.dirname(os.path.abspath(file_path))
    for row in vals:
        paths = row['app_path'].split('|')
        row['app_path'] = '|'.join([
            f'{{os.path.join(os.path.dirname(__file__), "{os.path.relpath(p, file_path)}")}}' for p in paths
        ])
        row['app_path'] = PathRestore(row['app_path'])
    return vals


def make_hashable(item: t.Any) -> t.Union[t.Tuple[t.Any, ...], t.Any]:
    """Recursively convert object to a hashable form, storing original values."""
    if isinstance(item, (set, list, tuple)):
        converted = tuple(make_hashable(i) for i in item)
    elif isinstance(item, dict):
        converted = tuple(sorted((k, make_hashable(v)) for k, v in item.items()))
    else:
        converted = item  # Primitives are already hashable

    return converted


def restore_params(data: t.List[t.Dict[str, t.Any]]) -> t.List[t.Tuple[t.List[str], t.List[t.Any]]]:
    """
    Restore parameters from pytest --collect-only data structure.
    """
    # Ensure all dictionaries have the same number of keys
    if len({len(d) for d in data}) != 1:
        raise ValueError(
            f'Inconsistent parameter {CurrentItemContext.test_name} structure: all rows must have the same number of keys.'
        )

    all_markers_is_empty = []
    for d in data:
        if 'markers' in d:
            all_markers_is_empty.append(not (d['markers']))
            d['markers'] = list(set(d['markers']))
    if all(all_markers_is_empty):
        for d in data:
            del d['markers']

    hashable_to_original: t.Dict[t.Tuple[str, t.Any], t.Any] = {}

    def save_to_hash(key: str, hashable_value: t.Any, original_value: t.Any) -> t.Any:
        """Stores the mapping of hashable values to their original."""
        if isinstance(original_value, list):
            original_value = tuple(original_value)
        hashable_to_original[(key, hashable_value)] = original_value
        return hashable_value

    def restore_from_hash(key: str, hashable_value: t.Any) -> t.Any:
        """Restores the original value from its hashable equivalent."""
        return hashable_to_original.get((key, hashable_value), hashable_value)

    # Convert data to a hashable format
    data = [{k: save_to_hash(k, make_hashable(v), v) for k, v in row.items()} for row in data]
    unique_data = []
    for d in data:
        if d not in unique_data:
            unique_data.append(d)
    data = unique_data
    data = group_by_target(data)

    params_multiplier: t.List[t.Tuple[t.List[str], t.List[t.Any]]] = []
    current_keys: t.List[str] = sorted(data[0].keys(), key=lambda x: (x == 'markers', x))
    i = 1

    while len(current_keys) > i:
        # It should be combinations because we are only concerned with the elements, not their order.
        for _ in itertools.combinations(current_keys, i):
            perm: t.List[str] = list(_)
            if perm == ['markers'] or [k for k in current_keys if k not in perm] == ['markers']:
                # The mark_runner must be used together with another parameter.
                continue

            grouped_buckets = defaultdict(list)
            for row in data:
                grouped_buckets[get_values_by_keys(row, perm)].append(remove_keys(row, perm))

            grouped_values = list(grouped_buckets.values())
            if all(v == grouped_values[0] for v in grouped_values):
                current_keys = [k for k in current_keys if k not in perm]
                params_multiplier.append((perm, list(grouped_buckets.keys())))
                data = grouped_values[0]
                break
        else:
            i += 1

    if data:
        remaining_values = [get_values_by_keys(row, current_keys) for row in data]
        params_multiplier.append((current_keys, remaining_values))

    for key, values in params_multiplier:
        values[:] = [tuple(restore_from_hash(key[i], v) for i, v in enumerate(row)) for row in values]
        output: t.List[t.Any] = []
        if len(key) == 1:
            for row in values:
                output.extend(row)
            values[:] = output

    for p in params_multiplier:
        if 'markers' in p[0]:
            for i, el in enumerate(p[1]):
                if el[-1] == ():
                    p[1][i] = el[:-1]

    return params_multiplier


def format_mark(name: str, args: t.Tuple[t.Any, ...], kwargs: t.Dict[str, t.Any]) -> str:
    """Format pytest mark with given arguments and keyword arguments."""
    args_str = ', '.join(repr(arg) if isinstance(arg, str) else str(arg) for arg in args)
    kwargs_str = ', '.join(f'{key}={repr(value) if isinstance(value, str) else value}' for key, value in kwargs.items())
    combined = ', '.join(filter(None, [args_str, kwargs_str]))
    return f'@pytest.mark.{name}({combined})\n' if combined else f'@pytest.mark.{name}\n'


def format_parametrize(keys: t.Union[str, t.List[str]], values: t.List[t.Any], indirect: t.Sequence[str]) -> str:
    """Format pytest parametrize for given keys and values."""
    # Ensure keys is always a list
    if isinstance(keys, str):
        keys = [keys]

    # Markers will always be at the end, so just remove markers from the keys if it is present
    # keys = [k for k in keys if k not in ('__markers',)]

    key_str = repr(keys[0]) if len(keys) == 1 else repr(','.join(keys))

    # If there any value which need to be represented in some spec way, best way is wrap it with class like PathRestore
    formatted_values = ['    ' + repr(value) for value in values]
    values_str = ',\n'.join(formatted_values)

    if indirect:
        return f'@idf_parametrize({key_str}, [\n{values_str}\n], indirect={indirect})\n'

    return f'@idf_parametrize({key_str}, [\n{values_str}\n])\n'


def key_for_item(item: Function) -> t.Tuple[str, str]:
    return item.originalname, str(item.fspath)


def collect_markers(item: Function) -> t.Tuple[t.List[Mark], t.List[Mark]]:
    """Separate local and global markers for a pytest item."""
    local_markers, global_markers = [], []

    for mark in item.iter_markers():
        if mark.name == 'parametrize':
            continue
        if 'callspec' in dir(item) and mark in item.callspec.marks:
            local_markers.append(mark)
        else:
            global_markers.append(mark)

    return local_markers, global_markers


class MarkerRepr(str):
    def __new__(cls, mark_name: str, kwargs_str: str, args_str: str, all_args: str) -> 'MarkerRepr':
        if not all_args:
            instance = super().__new__(cls, f'pytest.mark.{mark_name}')
        else:
            instance = super().__new__(cls, f'pytest.mark.{mark_name}({all_args})')
        return instance  # type: ignore

    def __init__(self, mark_name: str, kwargs_str: str, args_str: str, all_args: str) -> None:
        super().__init__()
        self.kwargs_str = kwargs_str
        self.args_str = args_str
        self.all_args = all_args
        self.mark_name = mark_name

    def __hash__(self) -> int:
        return hash(repr(self))

    def __repr__(self) -> str:
        if not self.all_args:
            return f'pytest.mark.{self.mark_name}'
        return f'pytest.mark.{self.mark_name}({self.all_args})'


def mark_to_source(mark: Mark) -> MarkerRepr:
    """Convert a Mark instance to its pytest.mark source code representation."""
    kwargs_str = ', '.join(f'{k}={repr(v)}' for k, v in mark.kwargs.items())
    args_str = ', '.join(repr(arg) for arg in mark.args)
    all_args = ', '.join(filter(None, [args_str, kwargs_str]))

    return MarkerRepr(mark.name, kwargs_str, args_str, all_args)


def process_local_markers(local_markers: t.List[Mark]) -> t.Tuple[t.List[str], t.List[MarkerRepr]]:
    """Process local markers to extract targets and runners."""
    local_targets, other_markers = [], []

    for mark in local_markers:
        if is_target_in_marker(mark):
            local_targets.append(mark.name)
        else:
            other_markers.append(mark_to_source(mark))
    return sorted(local_targets), sorted(other_markers)


def validate_global_markers(
    global_markers: t.List[Mark], local_targets: t.List[str], function_name: str
) -> t.List[Mark]:
    """Validate and normalize global markers."""
    normalized_markers = []

    for mark in global_markers:
        if is_target_in_marker(mark):
            if local_targets:
                warnings.warn(f'IN {function_name} IGNORING GLOBAL TARGET {mark.name} DUE TO LOCAL TARGETS')
                continue
        normalized_markers.append(mark)

    return normalized_markers


def filter_target(_targets: t.List[str]) -> t.List[str]:
    """
    Filters the input targets based on certain conditions.
    """
    if len(_targets) == 1:
        return _targets

    def remove_duplicates(target_list: t.List[str], group: t.List[str], group_name: str) -> t.List[str]:
        updated_target = []
        for _t in target_list:
            if _t in group:
                warnings.warn(f'{_t} is already included in {group_name}, no need to specify it separately.')
                continue
            updated_target.append(_t)
        return updated_target

    if 'supported_targets' in _targets:
        _targets = remove_duplicates(_targets, SUPPORTED_TARGETS, 'supported_targets')

    if 'preview_targets' in _targets:
        _targets = remove_duplicates(_targets, PREVIEW_TARGETS, 'preview_targets')

    return _targets


@pytest.hookimpl(tryfirst=True)
def pytest_collection_modifyitems(config: Config, items: t.List[Function]) -> None:
    """
    Local and Global marks in my diff are as follows:
    - Local: Used with a parameter inside a parameterized function, like:
      parameterized(param(marks=[....]))
    - Global: A regular mark.
    """

    test_name_to_params: t.Dict[t.Tuple[str, str], t.List] = defaultdict(list)
    test_name_to_global_mark: t.Dict[t.Tuple[str, str], t.List] = defaultdict(list)
    test_name_has_local_target_marks = defaultdict(bool)

    # Collect all fixtures to determine if a parameter is regular or a fixture
    fm = config.pluginmanager.get_plugin('funcmanage')
    known_fixtures = set(fm._arg2fixturedefs.keys())

    # Collecting data
    for item in items:
        collected = []
        item_key = key_for_item(item)

        local_markers, global_markers = collect_markers(item)
        # global_markers.sort(key=lambda x: x.name)
        global_markers.reverse()  # markers of item need to be reverted to save origin order

        local_targets, other_markers = process_local_markers(local_markers)
        if local_targets:
            test_name_has_local_target_marks[item_key] = True
        local_targets = filter_target(local_targets)
        other_markers_dict = {'markers': other_markers} if other_markers else {'markers': []}

        if local_targets:
            for target in local_targets:
                params = item.callspec.params if 'callspec' in dir(item) else {}
                collected.append({**params, **other_markers_dict, 'target': target})
        else:
            if 'callspec' in dir(item):
                collected.append({**other_markers_dict, **item.callspec.params})

        global_markers = validate_global_markers(global_markers, local_targets, item.name)

        # Just warning if global markers was changed
        if item_key in test_name_to_global_mark:
            if test_name_to_global_mark[item_key] != global_markers:
                warnings.warn(
                    f'{item.originalname} HAS DIFFERENT GLOBAL MARKERS! {test_name_to_global_mark[item_key]} {global_markers}'
                )

        test_name_to_global_mark[item_key] = global_markers
        test_name_to_params[item_key].extend(collected)

    # Post-processing: Modify files based on collected data
    for (function_name, file_path), function_params in test_name_to_params.items():
        CurrentItemContext.test_name = function_name
        to_add_lines = []
        global_targets = []
        for mark in test_name_to_global_mark[(function_name, file_path)]:
            if is_target_in_marker(mark):
                global_targets.append(mark.name)
                continue
            to_add_lines.append(format_mark(mark.name, mark.args, mark.kwargs))

        function_params_will_not_update = True
        if test_name_has_local_target_marks[(function_name, file_path)]:
            function_params_will_not_update = False

        # After filter_target, it will lose part of them, but we need them when removing decorators in the file.
        original_global_targets = global_targets
        global_targets = filter_target(global_targets)

        is_target_already_in_params = any({'target' in param for param in function_params})
        extra = []
        if global_targets:
            # If any of param have target then skip add global marker.
            if is_target_already_in_params:
                warnings.warn(f'Function {function_name} already have target params! Skip adding global target')
            else:
                extra = [{'target': _t} for _t in global_targets]

        def _update_file(file_path: str, to_add_lines: t.List[str], lines: t.List[str]) -> None:
            output = []
            start_with_comment = True

            imports = ['from pytest_embedded_idf.utils import idf_parametrize']
            if PathRestore.restored:
                imports += ['import os']

            for i, line in enumerate(lines):
                if line.strip() in imports:
                    continue
                if start_with_comment:
                    if not line == '\n' and not line.startswith(('from', 'import', '#')):
                        output.extend([f'{_imp}\n' for _imp in imports])
                        start_with_comment = False

                if i in skip_lines:
                    continue

                if line.startswith(f'def {function_name}('):
                    output.extend(to_add_lines)
                output.append(line)

            with open(file_path, 'w+') as file:
                file.writelines(output)

        if not function_params_will_not_update:
            buffered_params: t.List[str] = []
            if function_params:
                function_params = restore_path(function_params, file_path)

                for parameter_names, parameter_values in restore_params(function_params):
                    buffered_params.append(
                        format_parametrize(
                            parameter_names,
                            parameter_values,
                            indirect=[p for p in parameter_names if p in known_fixtures],
                        )
                    )

            to_add_lines.extend(buffered_params)

            with open(file_path) as file:
                lines = file.readlines()
            tree = ast.parse(''.join(lines))

            skip_lines: t.Set[int] = set()
            for node in ast.walk(tree):
                if isinstance(node, ast.FunctionDef) and node.name == function_name:
                    for dec in node.decorator_list:
                        assert dec.end_lineno is not None
                        skip_lines.update(list(range(dec.lineno - 1, dec.end_lineno)))  # ast count lines from 1 not 0
                    break

            _update_file(file_path, to_add_lines, lines)

        if global_targets:
            with open(file_path) as file:
                lines = file.readlines()
            tree = ast.parse(''.join(lines))

            skip_lines = set()
            for node in ast.walk(tree):
                if isinstance(node, ast.FunctionDef) and node.name == function_name:
                    for dec in node.decorator_list:
                        if isinstance(dec, ast.Attribute):
                            if dec.attr in original_global_targets:
                                assert dec.end_lineno is not None
                                skip_lines.update(list(range(dec.lineno - 1, dec.end_lineno)))
                    break

            if extra:
                to_add_lines = [format_parametrize('target', [_t['target'] for _t in extra], ['target'])] if extra else []
            else:
                to_add_lines = []
            _update_file(file_path, to_add_lines, lines)
