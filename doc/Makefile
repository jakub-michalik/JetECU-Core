# Makefile for JetECU-Core documentation
# Usage:
#   make html       — build HTML docs (runs Doxygen automatically)
#   make clean      — remove build artifacts

SPHINXOPTS    ?=
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     = .
BUILDDIR      = _build

.PHONY: help html clean

help:
	@echo "  html   — build HTML documentation"
	@echo "  clean  — remove build artifacts"

html:
	@$(SPHINXBUILD) -b html "$(SOURCEDIR)" "$(BUILDDIR)/html" $(SPHINXOPTS)
	@echo "Build finished. Output: $(BUILDDIR)/html/index.html"

clean:
	rm -rf $(BUILDDIR) _doxygen_output
