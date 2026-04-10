# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import re

from docutils import nodes  # type: ignore
from docutils.statemachine import StringList  # type: ignore
from sphinx import addnodes
from sphinx.addnodes import pending_xref
from sphinx.application import Sphinx
from sphinx.builders import Builder
from sphinx.directives import ObjDescT
from sphinx.directives import ObjectDescription
from sphinx.domains import Domain
from sphinx.environment import BuildEnvironment
from sphinx.roles import XRefRole
from sphinx.util.docutils import SphinxDirective
from sphinx.util.nodes import make_refnode
from sphinx.util.nodes import nested_parse_with_titles

RST_COMMENT_RE = re.compile(r'(?sm)^#\[\[api\n(.*?)\n#\]\]')


class CMakeV2IncludeDirective(SphinxDirective):
    required_arguments = 1

    def run(self) -> list:
        env = self.env
        rel_filename, filename = env.relfn2path(self.arguments[0])

        # Track the file as a dependency so Sphinx rebuilds if it changes.
        env.note_dependency(rel_filename)

        # Read CMakeLists.txt file content.
        with open(filename, encoding='utf-8') as f:
            raw_content = f.read()

        # Extract all RST comments from the file.
        rst_comments = RST_COMMENT_RE.findall(raw_content)

        parsed_nodes = []

        for rst_comment in rst_comments:
            # Temporary node to hold parsed comment.
            node = nodes.section()
            node.document = self.state.document

            string_list = StringList(rst_comment.splitlines(), source='<embedded cmakev2>')
            nested_parse_with_titles(self.state, string_list, node)

            parsed_nodes.extend(node.children)

        if not hasattr(env, 'cmakev2_comment_nodes'):
            env.cmakev2_comment_nodes = {}

        env.cmakev2_comment_nodes.setdefault(env.docname, []).extend(parsed_nodes)

        return []


class CMakeV2Description(ObjectDescription):
    def handle_signature(self, sig: str, signode: addnodes.desc_signature) -> ObjDescT:
        signode += addnodes.desc_name(text=sig)
        return sig

    def add_target_and_index(self, name: ObjDescT, sig: str, signode: addnodes.desc_signature) -> None:
        # Register object target only (no index)
        labelid = f'cmakev2-{self.cmakev2_type}-{sig}'
        signode['ids'].append(labelid)
        self.env.domaindata['cmakev2']['xrefs'][sig] = (self.env.docname, labelid)

    def run(self) -> list[nodes.Node]:
        index, node = super().run()
        # Add CMakeV2 custom attributes used when function, macro, and variable
        # nodes are added to the doctree and sorted.
        node['cmakev2-type'] = self.cmakev2_type
        node['cmakev2-name'] = self.arguments[0]
        return [node]


class CMakeV2VariableDirective(CMakeV2Description):
    cmakev2_type = 'variable'


class CMakeV2FunctionDirective(CMakeV2Description):
    cmakev2_type = 'function'


class CMakeV2MacroDirective(CMakeV2Description):
    cmakev2_type = 'macro'


class CMakeV2Domain(Domain):
    name = 'cmakev2'
    label = 'ESP-IDF build system v2'

    roles = {
        'ref': XRefRole(),
    }

    directives = {
        'variable': CMakeV2VariableDirective,
        'function': CMakeV2FunctionDirective,
        'macro': CMakeV2MacroDirective,
        'include': CMakeV2IncludeDirective,
    }

    initial_data: dict = {
        'xrefs': {},
    }

    def resolve_xref(
        self,
        env: BuildEnvironment,
        fromdocname: str,
        builder: Builder,
        typ: str,
        target: str,
        node: pending_xref,
        contnode: nodes.Element,
    ) -> nodes.Element | None:
        xref = self.data['xrefs'].get(target)
        if xref:
            todocname, labelid = xref

            # Extract clean text from contnode and wrap in non-literal node
            text = contnode.astext()
            newnode = nodes.emphasis(text, text)
            return make_refnode(builder, fromdocname, todocname, labelid, newnode)
        return None


def insert_cmakev2_comment_nodes(app: Sphinx, doctree: nodes.document) -> None:
    env = app.builder.env
    # Get nodes parsed and created by CMakeV2IncludeDirective.
    pending = getattr(env, 'cmakev2_comment_nodes', {}).get(env.docname, [])

    # Split nodes parsed in CMakeV2IncludeDirective into buckets based on their
    # type.
    buckets: dict = {'function': [], 'macro': [], 'variable': []}
    for node in pending:
        buckets[node['cmakev2-type']].append(node)

    # Sort functions, macros, and variables based on their names.
    buckets_sorted = {}
    for bucket_name, bucket_list in buckets.items():
        buckets_sorted[bucket_name] = sorted(bucket_list, key=lambda x: x['cmakev2-name'])

    # Traverse through the doctree to locate the target section labels (such as
    # _cmakev2_variables) and insert the nodes sorted into the appropriate
    # section.
    for section in doctree.traverse(nodes.section):
        if 'cmakev2-variables' in section['ids']:
            section.extend(buckets_sorted['variable'])
        elif 'cmakev2-functions' in section['ids']:
            section.extend(buckets_sorted['function'])
        elif 'cmakev2-macros' in section['ids']:
            section.extend(buckets_sorted['macro'])


def setup(app: Sphinx) -> dict:
    app.add_domain(CMakeV2Domain)
    # The nodes generated with CMakeV2IncludeDirective need to be placed into the
    # proper sections (functions, macros, variables), but the section labels
    # are not known during the directive run() method. Place the nodes into
    # section in the doctree-read event.
    app.connect('doctree-read', insert_cmakev2_comment_nodes)

    return {
        'version': '0.1',
        'parallel_read_safe': True,
        'parallel_write_safe': True,
    }
