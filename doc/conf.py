# Configuration file for the Sphinx documentation builder.
#
# SPDX-License-Identifier: Apache-2.0

import os
import subprocess

# -- Project information -----------------------------------------------------

project = "JetECU-Core"
copyright = "2025-2026, JetECU contributors"
author = "JetECU contributors"
release = "0.3"
version = "0.3"

# -- General configuration ---------------------------------------------------

extensions = [
    "breathe",
    "sphinx_rtd_theme",
    "sphinx_copybutton",
]

templates_path = ["_templates"]
exclude_patterns = ["_build", "_doxygen_output"]

# -- Breathe configuration ---------------------------------------------------

breathe_projects = {
    "JetECU": os.path.join(os.path.dirname(__file__), "_doxygen_output", "xml"),
}
breathe_default_project = "JetECU"
breathe_domain_by_extension = {"h": "c", "hpp": "cpp"}

# -- Options for HTML output -------------------------------------------------

html_theme = "sphinx_rtd_theme"
html_static_path = ["_static"]
html_css_files = ["custom.css"]
html_theme_options = {
    "logo_only": False,
    "display_version": True,
    "prev_next_buttons_location": "both",
    "style_external_links": True,
    "collapse_navigation": False,
    "sticky_navigation": True,
    "navigation_depth": 4,
    "includehidden": True,
    "titles_only": False,
}

html_context = {
    "display_github": True,
    "github_user": "jakub-michalik",
    "github_repo": "JetECU-Core",
    "github_version": "main",
    "conf_py_path": "/doc/",
}

# -- Run Doxygen before Sphinx -----------------------------------------------

def run_doxygen(_):
    """Generate Doxygen XML output before Sphinx build."""
    doxy_dir = os.path.dirname(__file__)
    doxyfile = os.path.join(doxy_dir, "Doxyfile.in")
    if os.path.isfile(doxyfile):
        os.makedirs(os.path.join(doxy_dir, "_doxygen_output", "xml"), exist_ok=True)
        subprocess.check_call(["doxygen", doxyfile], cwd=doxy_dir)

def setup(app):
    app.connect("builder-inited", run_doxygen)
