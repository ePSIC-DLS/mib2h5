"""mib2h5 - Python wrapper for MIB to HDF5 conversion.

This package provides Python binding for converting MerlinEM MIB files
to HDF5 files.
"""

from ._wrapper import convert

__all__ = ["convert"]
