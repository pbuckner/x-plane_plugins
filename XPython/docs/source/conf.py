# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys
sys.path.insert(0, os.path.abspath('.'))
sys.path.insert(0, os.path.abspath('../../Resources/plugins/XPPython3'))
sys.path.insert(0, os.path.abspath('../../stubs'))


# -- Project information -----------------------------------------------------

project = 'XPPython3'
copyright = '2022, Peter Buckner'
author = 'Peter Buckner'

master_doc = 'index'

# The full version, including alpha/beta/rc tags
release = '3.1.5'

# -- general configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx_comments',
    'sphinx.ext.autodoc',
    'sphinx.ext.githubpages',
    'sphinx.ext.todo',
    'sphinx_search.extension'
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_js_files = [
    'js/versions.js'
]    
html_theme = 'python_docs_theme'
html_theme_path = ['_theme', ]
html_last_updated_fmt = ""
html_title = '{} v{}'.format(project, release)
html_theme_options = {
    'root_name': '',
    'root_icon': 'xppython3.png',
    'root_url': '',
    'issues_url': 'https://github.com/pbuckner/x-plane_plugins/issues',
    'collapsiblesidebar': True,
    'versionlist': [('3.1.5', 'stable'), ]
}
html_js_files = [
    'js/versions.js'
]    

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

add_module_names = False

comments_config = {
    "hypothesis": True
}
