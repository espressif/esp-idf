# based on http://protips.readthedocs.io/link-roles.html

from __future__ import print_function
from __future__ import unicode_literals
import re
import os
import subprocess
from docutils import nodes
from collections import namedtuple
from sphinx.transforms.post_transforms import SphinxPostTransform
from get_github_rev import get_github_rev


# Creates a dict of all submodules with the format {submodule_path : (url relative to git root), commit)}
def get_submodules():
    git_root = subprocess.check_output(['git', 'rev-parse', '--show-toplevel']).strip().decode('utf-8')
    gitmodules_file = os.path.join(git_root, '.gitmodules')

    submodules = subprocess.check_output(['git', 'submodule', 'status']).strip().decode('utf-8').split('\n')

    submodule_dict = {}
    Submodule = namedtuple('Submodule', 'url rev')
    for sub in submodules:
        sub_info = sub.lstrip().split(' ')

        # Get short hash, 7 digits
        rev = sub_info[0].lstrip('-')[0:7]
        path = sub_info[1].lstrip('./')

        config_key_arg = "submodule.{}.url".format(path)
        rel_url = subprocess.check_output(['git', 'config', '--file', gitmodules_file, '--get', config_key_arg]).decode('utf-8').lstrip('./').rstrip('\n')

        submodule_dict[path] = Submodule(rel_url, rev)

    return submodule_dict


def url_join(*url_parts):
    """ Make a URL out of multiple components, assume first part is the https:// part and
    anything else is a path component """
    result = "/".join(url_parts)
    result = re.sub(r"([^:])//+", r"\1/", result)  # remove any // that isn't in the https:// part
    return result


def github_link(link_type, idf_rev, submods, root_path, app_config):
    def role(name, rawtext, text, lineno, inliner, options={}, content=[]):
        msgs = []
        BASE_URL = 'https://github.com/'
        IDF_REPO = "espressif/esp-idf"

        def warning(msg):
            system_msg = inliner.reporter.warning(msg)
            system_msg.line = lineno
            msgs.append(system_msg)

        # Redirects to submodule repo if path is a submodule, else default to IDF repo
        def redirect_submodule(path, submods, rev):
            for key, value in submods.items():
                # Add path separator to end of submodule path to ensure we are matching a directory
                if path.lstrip('/').startswith(os.path.join(key, '')):
                    return value.url.replace('.git', ''), value.rev, re.sub('^/{}/'.format(key), '', path)

            return IDF_REPO, rev, path

        # search for a named link (:label<path>) with descriptive label vs a plain URL
        m = re.search(r'(.*)\s*<(.*)>', text)
        if m:
            link_text = m.group(1)
            link = m.group(2)
        else:
            link_text = text
            link = text

        rel_path = root_path + link
        abs_path = os.path.join(app_config.idf_path, rel_path.lstrip('/'))

        repo, repo_rev, rel_path = redirect_submodule(rel_path, submods, idf_rev)

        line_no = None
        url = url_join(BASE_URL, repo, link_type, repo_rev, rel_path)

        if '#L' in abs_path:
            # drop any URL line number from the file, line numbers take the form #Lnnn or #Lnnn-Lnnn for a range
            abs_path, line_no = abs_path.split('#L')
            line_no = re.search(r'^(\d+)(?:-L(\d+))?', line_no)
            if line_no is None:
                warning("Line number anchor in URL %s doesn't seem to be valid" % link)
            else:
                line_no = tuple(int(ln_group) for ln_group in line_no.groups() if ln_group)  # tuple of (nnn,) or (nnn, NNN) for ranges
        elif '#' in abs_path:  # drop any other anchor from the line
            abs_path = abs_path.split('#')[0]
            warning("URL %s seems to contain an unusable anchor after the #, only line numbers are supported" % link)

        is_dir = (link_type == 'tree')

        if not os.path.exists(abs_path):
            warning("IDF path %s does not appear to exist (absolute path %s)" % (rel_path, abs_path))
        elif is_dir and not os.path.isdir(abs_path):
            # note these "wrong type" warnings are not strictly needed  as GitHub will apply a redirect,
            # but the may become important in the future (plus make for cleaner links)
            warning("IDF path %s is not a directory but role :%s: is for linking to a directory, try :%s_file:" % (rel_path, name, name))
        elif not is_dir and os.path.isdir(abs_path):
            warning("IDF path %s is a directory but role :%s: is for linking to a file" % (rel_path, name))

        # check the line number is valid
        if line_no:
            if is_dir:
                warning("URL %s contains a line number anchor but role :%s: is for linking to a directory" % (rel_path, name, name))
            elif os.path.exists(abs_path) and not os.path.isdir(abs_path):
                with open(abs_path, "r") as f:
                    lines = len(f.readlines())
                if any(True for ln in line_no if ln > lines):
                    warning("URL %s specifies a range larger than file (file has %d lines)" % (rel_path, lines))

            if tuple(sorted(line_no)) != line_no:  # second line number comes before first one!
                warning("URL %s specifies a backwards line number range" % rel_path)

        node = nodes.reference(rawtext, link_text, refuri=url, **options)
        return [node], msgs
    return role


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
                # then take off 3 more paths for language/release/targetname and build the new URL
                url = "{}.html".format(os.path.join(return_path, '../../..', language, env.config.release,
                                                    env.config.idf_target, docname))
                node.replace_self(nodes.reference(rawtext, link_text, refuri=url, **options))
            else:
                node.replace_self([])


def setup(app):
    rev = get_github_rev()
    submods = get_submodules()

    # links to files or folders on the GitHub
    app.add_role('idf', github_link('tree', rev, submods, '/', app.config))
    app.add_role('idf_file', github_link('blob', rev, submods, '/', app.config))
    app.add_role('idf_raw', github_link('raw', rev, submods, '/', app.config))
    app.add_role('component', github_link('tree', rev, submods, '/components/', app.config))
    app.add_role('component_file', github_link('blob', rev, submods, '/components/', app.config))
    app.add_role('component_raw', github_link('raw', rev, submods, '/components/', app.config))
    app.add_role('example', github_link('tree', rev, submods, '/examples/', app.config))
    app.add_role('example_file', github_link('blob', rev, submods, '/examples/', app.config))
    app.add_role('example_raw', github_link('raw', rev, submods, '/examples/', app.config))

    # link to the current documentation file in specific language version
    app.add_role('link_to_translation', link_to_translation)
    app.add_node(translation_link)
    app.add_post_transform(TranslationLinkNodeTransform)

    return {'parallel_read_safe': True, 'parallel_write_safe': True, 'version': '0.5'}
