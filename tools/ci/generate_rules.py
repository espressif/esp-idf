#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import argparse
import inspect
import os
import sys
from collections import defaultdict
from itertools import product

import yaml
from idf_ci_utils import GitlabYmlConfig
from idf_ci_utils import IDF_PATH

try:
    import pygraphviz as pgv
except ImportError:  # used when pre-commit, skip generating image
    pass

try:
    from typing import Union
except ImportError:  # used for type hint
    pass


def _list(str_or_list):  # type: (Union[str, list]) -> list
    if isinstance(str_or_list, str):
        return [str_or_list]
    elif isinstance(str_or_list, list):
        return str_or_list
    else:
        raise ValueError('Wrong type: {}. Only supports str or list.'.format(type(str_or_list)))


def _format_nested_dict(_dict, f_tuple):  # type: (dict[str, dict], tuple[str, ...]) -> dict[str, dict]
    res = {}
    for k, v in _dict.items():
        k = k.split('__')[0]
        if isinstance(v, dict):
            v = _format_nested_dict(v, f_tuple)
        elif isinstance(v, list):
            v = _format_nested_list(v, f_tuple)
        elif isinstance(v, str):
            v = v.format(*f_tuple)
        res[k.format(*f_tuple)] = v
    return res


def _format_nested_list(_list, f_tuple):  # type: (list[str], tuple[str, ...]) -> list[str]
    res = []
    for item in _list:
        if isinstance(item, list):
            item = _format_nested_list(item, f_tuple)
        elif isinstance(item, dict):
            item = _format_nested_dict(item, f_tuple)
        elif isinstance(item, str):
            item = item.format(*f_tuple)
        res.append(item)
    return res


class RulesWriter:
    AUTO_GENERATE_MARKER = inspect.cleandoc(r'''
    ##################
    # Auto Generated #
    ##################
    ''')

    LABEL_TEMPLATE = inspect.cleandoc(r'''
    .if-label-{0}: &if-label-{0}
      if: '$BOT_LABEL_{1} || $CI_MERGE_REQUEST_LABELS =~ /^(?:[^,\n\r]+,)*{0}(?:,[^,\n\r]+)*$/i'
    ''')

    RULE_PROTECTED = '    - <<: *if-protected'
    RULE_PROTECTED_NO_LABEL = '    - <<: *if-protected-no_label'
    RULE_BUILD_ONLY = '    - <<: *if-label-build-only\n' \
                      '      when: never'
    RULE_REVERT_BRANCH = '    - <<: *if-revert-branch\n' \
                         '      when: never'
    RULE_LABEL_TEMPLATE = '    - <<: *if-label-{0}'
    RULE_PATTERN_TEMPLATE = '    - <<: *if-dev-push\n' \
                            '      changes: *patterns-{0}'
    SPECIFIC_RULE_TEMPLATE = '    - <<: *{0}'
    RULES_TEMPLATE = inspect.cleandoc(r"""
    .rules:{0}:
      rules:
    {1}
    """)

    KEYWORDS = ['labels', 'patterns']

    def __init__(self, rules_yml, depend_yml):  # type: (str, str) -> None
        self.rules_yml = rules_yml
        self.rules_cfg = yaml.load(open(rules_yml), Loader=yaml.FullLoader)

        self.full_cfg = yaml.load(open(depend_yml), Loader=yaml.FullLoader)
        self.cfg = {k: v for k, v in self.full_cfg.items() if not k.startswith('.')}
        self.cfg = self.expand_matrices()
        self.rules = self.expand_rules()

        self.yml_config = GitlabYmlConfig()
        self.graph = None

    def expand_matrices(self):  # type: () -> dict
        """
        Expand the matrix into different rules
        """
        res = {}
        for k, v in self.cfg.items():
            res.update(self._expand_matrix(k, v))

        for k, v in self.cfg.items():
            if not v:
                continue
            deploy = v.get('deploy')
            if deploy:
                for item in _list(deploy):
                    res['{}-{}'.format(k, item)] = v
        return res

    @staticmethod
    def _expand_matrix(name, cfg):  # type: (str, dict) -> dict
        """
        Expand matrix into multi keys
        :param cfg: single rule dict
        :return:
        """
        default = {name: cfg}
        if not cfg:
            return default
        matrices = cfg.pop('matrix', None)
        if not matrices:
            return default

        res = {}
        for comb in product(*_list(matrices)):
            res.update(_format_nested_dict(default, comb))
        return res

    def expand_rules(self):  # type: () -> dict[str, dict[str, list]]
        res = defaultdict(lambda: defaultdict(set))  # type: dict[str, dict[str, set]]
        for k, v in self.cfg.items():
            if not v:
                continue
            for vk, vv in v.items():
                if vk in self.KEYWORDS:
                    res[k][vk] = set(_list(vv))
                else:
                    res[k][vk] = vv
            for key in self.KEYWORDS:  # provide empty set for missing field
                if key not in res[k]:
                    res[k][key] = set()

        for k, v in self.cfg.items():
            if not v:
                continue
            if 'included_in' in v:
                for item in _list(v['included_in']):
                    if 'specific_rules' in v:
                        res[item]['specific_rules'].update(_list(v['specific_rules']))
                    if 'labels' in v:
                        res[item]['labels'].update(_list(v['labels']))
                    if 'patterns' in v:
                        for _pat in _list(v['patterns']):
                            # Patterns must be pre-defined
                            if '.patterns-{}'.format(_pat) not in self.rules_cfg:
                                print('WARNING: pattern {} not exists'.format(_pat))
                                continue
                            res[item]['patterns'].add(_pat)

        sorted_res = defaultdict(lambda: defaultdict(list))  # type: dict[str, dict[str, list]]
        for k, v in res.items():
            for vk, vv in v.items():
                sorted_res[k][vk] = sorted(vv)
        return sorted_res

    def new_labels_str(self):  # type: () -> str
        _labels = set([])
        for k, v in self.cfg.items():
            if not v:
                continue  # shouldn't be possible
            labels = v.get('labels')
            if not labels:
                continue
            _labels.update(_list(labels))
        labels = sorted(_labels)

        res = ''
        res += '\n\n'.join([self._format_label(_label) for _label in labels])
        return res

    @classmethod
    def _format_label(cls, label):  # type: (str) -> str
        return cls.LABEL_TEMPLATE.format(label, cls.bot_label_str(label))

    @staticmethod
    def bot_label_str(label):  # type: (str) -> str
        return label.upper().replace('-', '_')

    def new_rules_str(self):  # type: () -> str
        res = []
        for k, v in sorted(self.rules.items()):
            if k.startswith('pattern'):
                continue

            if '.rules:' + k not in self.yml_config.used_templates:
                print(f'WARNING: unused rule: {k}, skipping...')
                continue

            res.append(self.RULES_TEMPLATE.format(k, self._format_rule(k, v)))
        return '\n\n'.join(res)

    def _format_rule(self, name, cfg):  # type: (str, dict) -> str
        _rules = [self.RULE_REVERT_BRANCH]
        if name.endswith('-production'):
            _rules.append(self.RULE_PROTECTED_NO_LABEL)
        else:
            if not (name.endswith('-preview') or name.startswith('labels:')):
                _rules.append(self.RULE_PROTECTED)
            if name.startswith('test:'):
                _rules.append(self.RULE_BUILD_ONLY)

            for specific_rule in cfg['specific_rules']:
                if f'.{specific_rule}' in self.rules_cfg:
                    _rules.append(self.SPECIFIC_RULE_TEMPLATE.format(specific_rule))
                else:
                    print('WARNING: specific_rule {} not exists'.format(specific_rule))
            for label in cfg['labels']:
                _rules.append(self.RULE_LABEL_TEMPLATE.format(label))
            for pattern in cfg['patterns']:
                if '.patterns-{}'.format(pattern) in self.rules_cfg:
                    _rules.append(self.RULE_PATTERN_TEMPLATE.format(pattern))
                else:
                    print('WARNING: pattern {} not exists'.format(pattern))
        return '\n'.join(_rules)

    def update_rules_yml(self):  # type: () -> bool
        with open(self.rules_yml) as fr:
            file_str = fr.read()

        auto_generate_str = '\n{}\n\n{}\n'.format(self.new_labels_str(), self.new_rules_str())
        rest, marker, old = file_str.partition(self.AUTO_GENERATE_MARKER)
        if old == auto_generate_str:
            return False
        else:
            print(self.rules_yml, 'has been modified. Please check')
            with open(self.rules_yml, 'w') as fw:
                fw.write(rest + marker + auto_generate_str)
            return True


LABEL_COLOR = 'green'
PATTERN_COLOR = 'cyan'
RULE_COLOR = 'blue'


def build_graph(rules_dict):  # type: (dict[str, dict[str, list]]) -> pgv.AGraph
    graph = pgv.AGraph(directed=True, rankdir='LR', concentrate=True)

    for k, v in rules_dict.items():
        if not v:
            continue
        included_in = v.get('included_in')
        if included_in:
            for item in _list(included_in):
                graph.add_node(k, color=RULE_COLOR)
                graph.add_node(item, color=RULE_COLOR)
                graph.add_edge(k, item, color=RULE_COLOR)
        labels = v.get('labels')
        if labels:
            for _label in labels:
                graph.add_node('label:{}'.format(_label), color=LABEL_COLOR)
                graph.add_edge('label:{}'.format(_label), k, color=LABEL_COLOR)
        patterns = v.get('patterns')
        if patterns:
            for _pat in patterns:
                graph.add_node('pattern:{}'.format(_pat), color=PATTERN_COLOR)
                graph.add_edge('pattern:{}'.format(_pat), k, color=PATTERN_COLOR)

    return graph


def output_graph(graph, output_path='output.png'):  # type: (pgv.AGraph, str) -> None
    graph.layout('dot')
    if output_path.endswith('.png'):
        img_path = output_path
    else:
        img_path = os.path.join(output_path, 'output.png')
    graph.draw(img_path)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('rules_yml', nargs='?', default=os.path.join(IDF_PATH, '.gitlab', 'ci', 'rules.yml'),
                        help='rules.yml file path')
    parser.add_argument('dependencies_yml', nargs='?', default=os.path.join(IDF_PATH, '.gitlab', 'ci', 'dependencies',
                                                                            'dependencies.yml'),
                        help='dependencies.yml file path')
    parser.add_argument('--graph',
                        help='Specify PNG image output path. Use this argument to generate dependency graph')
    args = parser.parse_args()

    writer = RulesWriter(args.rules_yml, args.dependencies_yml)
    file_modified = writer.update_rules_yml()

    if args.graph:
        dep_tree_graph = build_graph(writer.rules)
        output_graph(dep_tree_graph)

    sys.exit(file_modified)
