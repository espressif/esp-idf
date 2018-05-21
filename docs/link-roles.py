# based on http://protips.readthedocs.io/link-roles.html

import re
import os
from docutils import nodes
from local_util import run_cmd_get_output

def get_github_rev():
    path = run_cmd_get_output('git rev-parse --short HEAD')
    tag = run_cmd_get_output('git describe --exact-match')
    print ('Git commit ID: ', path)
    if len(tag):
        print ('Git tag: ', tag)
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
    on_rtd = os.environ.get('READTHEDOCS', None) == 'True'
    if on_rtd:
        # provide RTD specific commit identification to be included in the link
        tag_rev = 'latest'
        if (run_cmd_get_output('git rev-parse --short HEAD') != rev):
            tag_rev = rev
    else:
        # if not on the RTD then provide generic identification 
        tag_rev = run_cmd_get_output('git describe --always')

    app.add_role('link_to_translation', crosslink('%s../../%s/{}/%s.html'.format(tag_rev)))

def autolink(pattern):
    def role(name, rawtext, text, lineno, inliner, options={}, content=[]):
        m = re.search('(.*)\s*<(.*)>', text)
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
