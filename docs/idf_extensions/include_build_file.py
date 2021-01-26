import os.path

from docutils.parsers.rst import directives
from docutils.parsers.rst.directives.misc import Include as BaseInclude
from sphinx.util.docutils import SphinxDirective


class IncludeBuildFile(BaseInclude, SphinxDirective):
    """
    Like the standard "Include" directive, but relative to the app
    build directory
    """
    def run(self):
        abspath = os.path.join(self.env.config.build_dir, self.arguments[0])
        self.arguments[0] = abspath
        self.env.note_included(abspath)
        return super(IncludeBuildFile, self).run()


def setup(app):
    directives.register_directive('include-build-file', IncludeBuildFile)

    return {'parallel_read_safe': True, 'parallel_write_safe': True, 'version': '0.1'}
