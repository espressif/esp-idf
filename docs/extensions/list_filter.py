import re

from docutils import nodes
from docutils.parsers.rst import Directive


def setup(app):
    app.add_directive('list', ListFilter)

    return {'parallel_read_safe': True, 'parallel_write_safe': True, 'version': '0.1'}


class ListFilter(Directive):
    """
    Provides a list implementation directive that support clauses of the kind

    .. list::

        - Content
        :filter: - Content
        - Content

    Where the :filter: part becomes selective to only include the content if
    one of the provided tags is set, same as the logic used by the "only" directive.

    The directive also works for numbered list.

    """
    RE_PATTERN = re.compile(r'^\s*:(.+?):\s*(.+)$')
    has_content = True
    required_arguments = 0
    optional_arguments = 0
    final_argument_whitespace = True
    option_spec = {}

    def run(self):
        # Raise an error if the directive does not have contents.
        self.assert_has_content()

        # Remove all list entries that should not be on display
        env = self.state.document.settings.env
        filt_data = [self.filter_entry(env, e) for e in self.content.data if e is not None]

        # Clean up deleted values from content
        self.content.data = [data for data in filt_data if data is not None]
        self.content.items = [items for data, items in zip(filt_data, self.content.items) if data is not None]

        # Parse the filtered content and return the new node
        node = nodes.paragraph()
        self.state.nested_parse(self.content, self.content_offset, node)

        return [node]

    def filter_entry(self, env, entry):
        m = self.RE_PATTERN.match(entry)
        if m is not None:
            tag_filter, entry = m.groups()
            if not env.app.builder.tags.eval_condition(tag_filter):
                return None
        return entry
