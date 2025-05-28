# setup.py

from setuptools import setup, Extension
from Cython.Build import cythonize
import os

HDF5_ROOT = os.environ.get("HDF5_ROOT", "/usr")

hdf5_include = os.path.join(HDF5_ROOT, "include")
hdf5_lib = os.path.join(HDF5_ROOT, "lib")

BLOSC_ROOT = os.environ.get("BLOSC_ROOT", "/usr")

blosc_include = os.path.join(BLOSC_ROOT, "include")
blosc_lib = os.path.join(BLOSC_ROOT, "lib64")

HDF5_BLOSC_ROOT = os.environ.get("HDF5_BLOSC_ROOT", "/usr")

hdf5_blosc_include = os.path.join(HDF5_BLOSC_ROOT, "src")
hdf5_blosc_lib = os.path.join(HDF5_BLOSC_ROOT, "build")

ext = Extension(
  name="mib2h5_wrapper",
  sources=[
    "mib2h5_wrapper.pyx",
    "src/mib_to_h5.c",
    "src/append.c",
    "src/compress.c",
    "src/framebuffer.c",
    "src/hdf5_init.c",
    "src/hdf5_init_meta.c",
    "src/io_header.c",
    "src/parser.c",
    "src/read.c",
    "src/utils.c"
  ],
  include_dirs=["src", hdf5_include, ".", blosc_include, hdf5_blosc_include],
  library_dirs=[hdf5_lib, blosc_lib, hdf5_blosc_lib],
  libraries=["hdf5", "blosc", "blosc_filter"],
  extra_compile_args=["-std=c11"],
  extra_link_args=[
    f"-Wl,-rpath,{hdf5_lib}",
    f"-Wl,-rpath,{blosc_lib}",
    f"-Wl,-rpath,{hdf5_blosc_lib}"
  ],
)

setup(
  name="mib2h5_wrapper",
  ext_modules=cythonize(ext, language_level=3),
)
