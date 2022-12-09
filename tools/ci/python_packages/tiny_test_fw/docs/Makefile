# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line.
SPHINXOPTS    =
SPHINXAPI     = sphinx-apidoc
SPHINXAPISRC  = ..
SPHINXBUILD   = python -msphinx
SPHINXPROJ    = TinyTestFW
SOURCEDIR     = .
BUILDDIR      = _build

# define the files to be excluded here
EXCLUEDLIST   = "$(SPHINXAPISRC)/example.py"

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile
	@$(SPHINXAPI) -o $(SOURCEDIR) $(SPHINXAPISRC) $(EXCLUEDLIST)
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)