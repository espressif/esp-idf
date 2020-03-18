from sphinx.builders.latex import LaTeXBuilder
import os


# Overrides the default Sphinx latex build
class IdfLatexBuilder(LaTeXBuilder):

    def __init__(self, app):

        # Sets up the latex_documents config value, done here instead of conf.py since it depends on the runtime value 'idf_target'
        self.init_latex_documents(app)

        super().__init__(app)

    def init_latex_documents(self, app):

        file_name = app.config.pdf_file + '.tex'

        if app.config.language == 'zh_CN':
            latex_documents = [('index', file_name, u'ESP-IDF 编程指南', u'乐鑫信息科技', 'manual')]
        else:
            # Default to english naming
            latex_documents = [('index', file_name, u'ESP-IDF Programming Guide', u'Espressif Systems', 'manual')]

        app.config.latex_documents = latex_documents

    def prepare_latex_macros(self, package_path, config):

        PACKAGE_NAME = "espidf.sty"
        latex_package = ''
        with open(package_path, 'r') as template:

            latex_package = template.read()

        idf_target_title = config.idf_target_title_dict[config.idf_target]
        latex_package = latex_package.replace('<idf_target_title>', idf_target_title)

        # Release name for the PDF front page, remove '_' as this is used for subscript in Latex
        idf_release_name = "Release {}".format(config.version.replace('_', '-'))
        latex_package = latex_package.replace('<idf_release_name>', idf_release_name)

        with open(os.path.join(self.outdir, PACKAGE_NAME), 'w') as package_file:
            package_file.write(latex_package)

    def finish(self):
        super().finish()

        TEMPLATE_PATH = "../latex_templates/espidf.sty"
        self.prepare_latex_macros(os.path.join(self.confdir,TEMPLATE_PATH), self.config)


def setup(app):
    app.add_builder(IdfLatexBuilder, override=True)

    return {'parallel_read_safe': True, 'parallel_write_safe': True, 'version': '0.1'}
