# based on http://protips.readthedocs.io/link-roles.html

from __future__ import print_function
from __future__ import unicode_literals
import re
import os
from docutils import nodes
from local_util import run_cmd_get_output
from sphinx.transforms.post_transforms import SphinxPostTransform


def get_github_rev():
    path = run_cmd_get_output('git rev-parse --short HEAD')
    tag = run_cmd_get_output('git describe --exact-match')
    print('Git commit ID: ', path)
    if len(tag):
        print('Git tag: ', tag)
        path = tag
    return path


def setup(app):
    rev = get_github_rev()

    # links to files or folders on the GitHub
    baseurl = 'https://github.com/espressif/esp-idf'
    app.add_role('idf', autolink('{}/tree/{}/%s'.format(baseurl, rev)))
    app.add_role('idf_file', autolink('{}/blob/{}/%s'.format(baseurl, rev)))
    app.add_role('idf_raw', autolink('{}/raw/{}/%s'.format(baseurl, rev)))
    app.add_role('component', autolink('{}/tree/{}/components/%s'.format(baseurl, rev)))
    app.add_role('component_file', autolink('{}/blob/{}/components/%s'.format(baseurl, rev)))
    app.add_role('component_raw', autolink('{}/raw/{}/components/%s'.format(baseurl, rev)))
    app.add_role('example', autolink('{}/tree/{}/examples/%s'.format(baseurl, rev)))
    app.add_role('example_file', autolink('{}/blob/{}/examples/%s'.format(baseurl, rev)))
    app.add_role('example_raw', autolink('{}/raw/{}/examples/%s'.format(baseurl, rev)))

    # link to the current documentation file in specific language version
    app.add_role('link_to_translation', link_to_translation)
    app.add_node(translation_link)
    app.add_post_transform(TranslationLinkNodeTransform)


class translation_link(nodes.Element):
    """Node for "link_to_translation" role."""


# Linking to translation is done at the "writing" stage to avoid issues with the info being cached between builders
def link_to_translation(name, rawtext, text, lineno, inliner, options={}, content=[]):
    node = translation_link()
    node['expr'] = (rawtext, text, options)
    return [node], []


class TranslationLinkNodeTransform(SphinxPostTransform):
    # Transform needs to happen early to ensure the new reference node is also transformed
    default_priority = 0

    def run(self, **kwargs):

        # Only output relative links if building HTML
        for node in self.document.traverse(translation_link):
            if 'html' in self.app.builder.name:
                rawtext, text, options = node['expr']
                (language, link_text) = text.split(':')
                env = self.document.settings.env
                docname = env.docname
                doc_path = env.doc2path(docname, None, None)
                return_path = '../' * doc_path.count('/')  # path back to the root from 'docname'
                # then take off 2 more paths for language/release/ and build the new URL
                url = '{}.html'.format(os.path.join(return_path, '../..', language, env.config.release, docname))
                node.replace_self(nodes.reference(rawtext, link_text, refuri=url, **options))
            else:
                node.replace_self([])


def autolink(pattern):
    def role(name, rawtext, text, lineno, inliner, options={}, content=[]):
        m = re.search('(.*)\s*<(.*)>', text)  # noqa: W605 - regular expression
        if m:
            link_text = m.group(1)
            link = m.group(2)
        else:
            link_text = text
            link = text
        url = pattern % (link,)
        node = nodes.reference(rawtext, link_text, refuri=url, **options)
        return [node], []
    return role


def crosslink(pattern):
    def role(name, rawtext, text, lineno, inliner, options={}, content=[]):
        (language, link_text) = text.split(':')
        docname = inliner.document.settings.env.docname
        doc_path = inliner.document.settings.env.doc2path(docname, None, None)
        return_path = '../' * doc_path.count('/')
        url = pattern % (return_path, language, docname)
        node = nodes.reference(rawtext, link_text, refuri=url, **options)
        return [node], []
    return role
