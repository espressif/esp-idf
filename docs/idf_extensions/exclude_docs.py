# Updates the excluded documents according to the conditional_include_dict {tag:documents}
def update_exclude_patterns(app, config):
    for tag, docs in config.conditional_include_dict.items():
        if not app.tags.has(tag):
            app.config.exclude_patterns.extend(docs)


def setup(app):
    # Tags are generated together with defines
    app.connect('config-inited', update_exclude_patterns)

    return {'parallel_read_safe': True, 'parallel_write_safe': True, 'version': '0.1'}
