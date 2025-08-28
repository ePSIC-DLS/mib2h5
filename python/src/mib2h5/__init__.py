"""mib2h5 - Python wrapper for MIB to HDF5 conversion.

This package provides Python binding for converting MerlinEM MIB files
to HDF5 files.
"""

from importlib.metadata import PackageNotFoundError, version

from ._wrapper import convert

try:
    __version__ = version("mib2h5")
except PackageNotFoundError:
    __version__ = "unknown"

__all__ = ["convert", "__version__"]
