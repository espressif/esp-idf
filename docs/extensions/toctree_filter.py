# Based on https://stackoverflow.com/a/46600038 with some modifications
import re

from sphinx.directives.other import TocTree


def setup(app):
    app.add_directive('toctree', TocTreeFilt, override=True)

    return {'parallel_read_safe': True, 'parallel_write_safe': True, 'version': '0.1'}


class TocTreeFilt(TocTree):
    """
    Override normal toctree directive to support clauses of the kind

    :filter: Name <link>

    Where the :filter: part becomes selective to only include the document if
    one of the provided tags is set, same as the logic used by the "only" directive.

    If no :filter: is supplied, works the same as default Sphinx :toctree:

    Note that excluding via filter doesn't prevent Sphinx from finding these .rst files
    when it scan the src/ directory, so it's also necessary to make sure that the files
    are covered by the exclude_patterns list in conf.py
    """
    RE_PATTERN = re.compile(r'^\s*:(.+?):\s*(.+)$')

    def run(self):
        # Remove all TOC entries that should not be on display
        env = self.state.document.settings.env
        self.content = [self.filter_entry(env, e) for e in self.content if e is not None]

        return super(TocTreeFilt, self).run()

    def filter_entry(self, env, entry):
        m = self.RE_PATTERN.match(entry)
        if m is not None:
            tag_filter, entry = m.groups()
            if not env.app.builder.tags.eval_condition(tag_filter):
                return None
        return entry
