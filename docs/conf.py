from datetime import datetime

import sphinx_rtd_theme


project = "mib2h5"
author = "Timothy Poon (timothy.poon@diamond.ac.uk)"
release = "0.0.1"
copyright = f"{datetime.today().year}"

extensions = [
    "sphinx_rtd_theme",
]

templates_path = ["_templates"]
exclude_patterns = []

html_theme = "pyramid"
html_static_path = ["_static"]


master_doc = "index"
