from sphinx.util import get_matching_files
from sphinx.util.matching import compile_matchers


# Updates the excluded documents according to the conditional_include_dict {tag:documents}
def update_exclude_patterns(app, config):

    # Default to building all if option not set
    if config.docs_to_build:
        build_subset(app, config)

    for tag, docs in config.conditional_include_dict.items():
        if not app.tags.has(tag):
            app.config.exclude_patterns.extend(docs)


def build_subset(app, config):
    # Convert to list of docs to build
    docs_to_build = config.docs_to_build.split(',')

    # Exclude all documents which were not set as docs_to_build when build_docs were called
    exclude_docs = [filename for filename in get_matching_files(app.srcdir, compile_matchers(docs_to_build))]
    docs = [filename for filename in get_matching_files(app.srcdir, compile_matchers(exclude_docs))]

    app.config.exclude_patterns.extend(exclude_docs)

    # Get all docs that will be built
    docs = [filename for filename in get_matching_files(app.srcdir, compile_matchers(exclude_docs))]
    if not docs:
        raise ValueError("No documents to build")
    print("Building a subset of the documents: {}".format(docs))

    # Sphinx requires a master document, if there is a document name 'index' then we pick that
    index_docs = [doc for doc in docs if 'index' in doc]
    if index_docs:
        config.master_doc = index_docs[0].replace('.rst', '')
    else:
        config.master_doc = docs[0].replace('.rst', '')


def setup(app):
    # Tags are generated together with defines
    app.connect('config-inited', update_exclude_patterns)

    return {'parallel_read_safe': True, 'parallel_write_safe': True, 'version': '0.1'}
