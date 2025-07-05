from setuptools import setup, Extension
from Cython.Build import cythonize
from pathlib import Path
import os


def check_header(include_dir, header_file):
  header_path = Path(include_dir) / header_file
  if not header_path.exists():
    raise FileNotFoundError(f"{header_file} not found in {include_dir}")


# hdf5 paths
HDF5_ROOT = os.environ.get("HDF5_ROOT", "/usr")
hdf5_include = Path(HDF5_ROOT) / "include"
hdf5_lib = Path(HDF5_ROOT) / "lib"
check_header(hdf5_include, "hdf5.h")

# blosc paths
BLOSC_ROOT = os.environ.get("BLOSC_ROOT", "/usr")
blosc_include = Path(BLOSC_ROOT) / "include"
blosc_lib = Path(BLOSC_ROOT) / "lib64"
check_header(blosc_include, "blosc.h")

# hdf5-blosc paths
HDF5_BLOSC_ROOT = os.environ.get("HDF5_BLOSC_ROOT", "/usr")
hdf5_blosc_include = Path(HDF5_BLOSC_ROOT) / "src"
hdf5_blosc_lib = Path(HDF5_BLOSC_ROOT) / "build"
check_header(hdf5_blosc_include, "blosc_filter.h")

ext = Extension(
  name="mib2h5",
  sources=[
    "mib2h5.pyx",
    "../src/mib_to_h5.c",
    "../src/append.c",
    "../src/compress.c",
    "../src/framebuffer.c",
    "../src/hdf5_init.c",
    "../src/hdf5_init_meta.c",
    "../src/io_header.c",
    "../src/parser.c",
    "../src/read.c",
    "../src/utils.c"
  ],
  include_dirs=["../src",
                str(hdf5_include),
                str(blosc_include),
                str(hdf5_blosc_include)
                ],
  library_dirs=[str(hdf5_lib),
                str(blosc_lib),
                str(hdf5_blosc_lib)
                ],
  libraries=["hdf5", "blosc", "blosc_filter"],
  extra_compile_args=["-std=c11"],
  extra_link_args=[
    f"-Wl,-rpath,{hdf5_lib}",
    f"-Wl,-rpath,{blosc_lib}",
    f"-Wl,-rpath,{hdf5_blosc_lib}"
  ],
)

setup(
  name="mib2h5",
  ext_modules=cythonize(ext, language_level=3),
)
