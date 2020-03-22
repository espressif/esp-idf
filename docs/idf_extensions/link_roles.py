# based on http://protips.readthedocs.io/link-roles.html

from __future__ import print_function
from __future__ import unicode_literals
import re
import os
import subprocess
from docutils import nodes


def get_github_rev():
    path = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).strip().decode('utf-8')
    try:
        tag = subprocess.check_output(['git', 'describe', '--exact-match']).strip().decode('utf-8')
    except subprocess.CalledProcessError:
        tag = None
    print('Git commit ID: ', path)
    if tag:
        print('Git tag: ', tag)
        return tag
    return path


def url_join(*url_parts):
    """ Make a URL out of multiple components, assume first part is the https:// part and
    anything else is a path component """
    result = "/".join(url_parts)
    result = re.sub(r"([^:])//+", r"\1/", result)  # remove any // that isn't in the https:// part
    return result


def setup(app):
    rev = get_github_rev()

    # links to files or folders on the GitHub
    app.add_role('idf', github_link('tree', rev, '/', app.config))
    app.add_role('idf_file', github_link('blob', rev, '/', app.config))
    app.add_role('idf_raw', github_link('raw', rev, '/', app.config))
    app.add_role('component', github_link('tree', rev, '/components/', app.config))
    app.add_role('component_file', github_link('blob', rev, '/components/', app.config))
    app.add_role('component_raw', github_link('raw', rev, '/components/', app.config))
    app.add_role('example', github_link('tree', rev, '/examples/', app.config))
    app.add_role('example_file', github_link('blob', rev, '/examples/', app.config))
    app.add_role('example_raw', github_link('raw', rev, '/examples/', app.config))

    # link to the current documentation file in specific language version
    app.add_role('link_to_translation', link_to_translation(app.config))

    return {'parallel_read_safe': True, 'parallel_write_safe': True, 'version': '0.3'}


def github_link(link_type, rev, root_path, app_config):
    def role(name, rawtext, text, lineno, inliner, options={}, content=[]):
        msgs = []

        def warning(msg):
            system_msg = inliner.reporter.warning(msg)
            system_msg.line = lineno
            msgs.append(system_msg)

        BASE_URL = 'https://github.com/espressif/esp-idf'

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
        line_no = None
        url = url_join(BASE_URL, link_type, rev, rel_path)

        if '#L' in abs_path:
            # drop any URL line number from the file, line numbers take the form #Lnnn or #Lnnn-Lnnn for a range
            abs_path, line_no = abs_path.split('#L')
            line_no = re.search(r'^(\d+)(?:-L(\d+))?', line_no)
            if line_no is None:
                warning("Line number anchor in URL %s doesn't seem to be valid" % link)
            else:
                line_no = tuple(int(l) for l in line_no.groups() if l)  # tuple of (nnn,) or (nnn, NNN) for ranges
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
                if any(True for l in line_no if l > lines):
                    warning("URL %s specifies a range larger than file (file has %d lines)" % (rel_path, lines))

            if tuple(sorted(line_no)) != line_no:  # second line number comes before first one!
                warning("URL %s specifies a backwards line number range" % rel_path)

        node = nodes.reference(rawtext, link_text, refuri=url, **options)
        return [node], msgs
    return role


def link_to_translation(config):
    def role(name, rawtext, text, lineno, inliner, options={}, content=[]):
        (language, link_text) = text.split(':')
        docname = inliner.document.settings.env.docname
        doc_path = inliner.document.settings.env.doc2path(docname, None, None)
        return_path = '../' * doc_path.count('/')  # path back to the root from 'docname'
        # then take off 3 more paths for language/release/targetname and build the new URL
        url = "{}.html".format(os.path.join(return_path, '../../..', language, config.release,
                                            config.idf_target, docname))
        node = nodes.reference(rawtext, link_text, refuri=url, **options)
        return [node], []
    return role
