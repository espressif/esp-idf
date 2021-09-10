# Embeds a google analytics tracking tag in every HTML page
def setup(app):
    app.add_config_value('google_analytics_id', None, 'html')
    app.connect('html-page-context', google_analytics_embed)

    return {'parallel_read_safe': True, 'parallel_write_safe': True, 'version': '0.1'}


def google_analytics_embed(app, pagename, templatename, context, doctree):

    ga_id = app.config.google_analytics_id
    if not ga_id:
        return

    metatags = context.get('metatags', '')

    google_analytics_snippet = """
        <!-- Global site tag (gtag.js) - Google Analytics -->
        <script async src="https://www.googletagmanager.com/gtag/js?id={}"></script>
        <script>
            window.dataLayer = window.dataLayer || [];
            function gtag(){{dataLayer.push(arguments);}}
            gtag('js', new Date());

            gtag('config', '{}');
        </script>""".format(ga_id, ga_id)

    # Prepend the google analytics to the HTML metatags (which will be passed to the sphinx templating engine)
    metatags = google_analytics_snippet + metatags
    context['metatags'] = metatags
