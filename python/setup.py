import os
import subprocess
from pathlib import Path
from subprocess import CalledProcessError

from Cython.Build import cythonize
from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext


class AutotoolsError(Exception):
    """Exception raised when autotools configuration fails."""
    pass


class ConfigureBuildExt(build_ext):
    """Custom build_ext that runs configure if needed."""

    def run(self):
        """Run build extension with autotools configuration if needed."""
        config_h_path = self.parent_dir / "config.h"

        if not config_h_path.exists():
            print("config.h not found. Running autotools configuration...")
            print("This is only needed once for the initial setup.")
            self._run_autotools()

        super().run()

    def _run_autotools(self):
        """Run autotools to generate config.h."""
        configure_path = self.parent_dir / "configure"

        if not configure_path.exists():
            self._run_autoreconf()

        self._run_configure()
        print("Configuration completed.")

    def _run_autoreconf(self):
        """Run autoreconf to generate configure script."""
        print("Running autoreconf -i...")
        try:
            subprocess.run(
                ["autoreconf", "-i"],
                cwd=self.parent_dir,
                capture_output=True,
                text=True,
                check=True,
            )
        except CalledProcessError as err:
            raise AutotoolsError(
                f"Failed to run autoreconf: {err.stderr}"
            ) from err

    def _run_configure(self):
        """Run configure script."""
        print("Running ./configure...")
        configure_args = self._get_configure_args()

        try:
            subprocess.run(
                configure_args,
                cwd=self.parent_dir,
                capture_output=True,
                text=True,
                env=os.environ.copy(),
                check=True,
            )
        except CalledProcessError as err:
            raise AutotoolsError(
                "Configure failed. This is usually because of missing HDF5 "
                f"libraries: {err.stderr}"
            ) from err

    def _get_configure_args(self):
        """Get configure command arguments."""
        # check hdf5 environment variables in order of priority
        for env_var in ("HDF5_ROOT", "HDF5_HOME", "HDF5_DIR"):
            hdf5_path_str = os.environ.get(env_var)
            if hdf5_path_str:
                hdf5_path = Path(hdf5_path_str).resolve()
                if not hdf5_path.exists():
                    msg = f"{env_var} path does not exist: {hdf5_path}"
                    raise AutotoolsError(msg)
                print(f"Using {env_var}={hdf5_path} for configure")
                return ["./configure", f"--with-hdf5={hdf5_path}"]
        return ["./configure"]

    @property
    def parent_dir(self):
        return Path(__file__).parent.parent


def _check_header_exists(include_dir, header_file):
    """Check if a header file exists in the given directory."""
    header_path = Path(include_dir) / header_file
    return header_path.exists() and header_path.is_file()


def _get_library_path(root_path, lib_name="lib"):
    """Get library path, checking lib64 first, then lib."""
    lib64_path = root_path / "lib64"
    lib_path = root_path / lib_name

    if lib64_path.exists():
        return lib64_path
    elif lib_path.exists():
        return lib_path
    else:
        raise FileNotFoundError(f"No library directory found in {root_path}")


def _find_hdf5_paths():
    """Find HDF5 include and library paths."""
    # check hdf5 environment variables in order of priority
    hdf5_root = None
    for env_var in ("HDF5_ROOT", "HDF5_HOME", "HDF5_DIR"):
        hdf5_path_str = os.environ.get(env_var)
        if hdf5_path_str:
            hdf5_root = Path(hdf5_path_str).resolve()
            if hdf5_root.exists():
                print(f"Using {env_var}={hdf5_root} for HDF5")
                break
            else:
                print(f"Warning: {env_var} is set but path doesn't exist: "
                      f"{hdf5_root}")
                hdf5_root = None

    if not hdf5_root:
        # try default paths
        for default_path in ("/usr", "/usr/local"):
            candidate = Path(default_path)
            if _check_header_exists(candidate / "include", "hdf5.h"):
                hdf5_root = candidate
                break
        else:
            msg = ("HDF5 not found in default locations. Please set one of "
                   "HDF5_ROOT, HDF5_HOME, or HDF5_DIR environment variables "
                   "to your HDF5 installation.")
            raise FileNotFoundError(msg)

    hdf5_include = hdf5_root / "include"
    hdf5_lib = _get_library_path(hdf5_root)

    # verify hdf5 header exists
    if not _check_header_exists(hdf5_include, "hdf5.h"):
        msg = (f"HDF5 header not found at '{hdf5_include}'. Please verify "
                "your HDF5 installation.")
        raise FileNotFoundError(msg)

    return hdf5_include, hdf5_lib


def _check_compression_support():
    """Check for optional compression libraries."""
    blosc_root_env = os.environ.get("BLOSC_ROOT")
    hdf5_blosc_root_env = os.environ.get("HDF5_BLOSC_ROOT")

    if not (blosc_root_env and hdf5_blosc_root_env):
        return None

    try:
        blosc_root = Path(blosc_root_env).resolve()
        hdf5_blosc_root = Path(hdf5_blosc_root_env).resolve()

        blosc_include = blosc_root / "include"
        blosc_lib = _get_library_path(blosc_root)

        hdf5_blosc_include = hdf5_blosc_root / "src"
        hdf5_blosc_lib = hdf5_blosc_root / "build"

        # verify headers exist
        if (_check_header_exists(blosc_include, "blosc.h") and
            _check_header_exists(hdf5_blosc_include, "blosc_filter.h")):
            return {
                "include": blosc_include,
                "lib": blosc_lib,
                "hdf5_blosc_include": hdf5_blosc_include,
                "hdf5_blosc_lib": hdf5_blosc_lib,
            }
    except FileNotFoundError:
        pass

    return None


def _build_extension_config():
    """Build extension configuration."""
    # find hdf5 paths
    hdf5_include, hdf5_lib = _find_hdf5_paths()

    # base configuration
    include_dirs = ["..", "../src", hdf5_include]
    library_dirs = [hdf5_lib]
    libraries = ["hdf5"]
    extra_link_args = [f"-Wl,-rpath,{hdf5_lib}"]
    extra_compile_args = ["-std=c11"]

    # check for compression support
    compression_config = _check_compression_support()
    if compression_config:
        include_dirs.extend([
            compression_config["include"],
            compression_config["hdf5_blosc_include"],
        ])
        library_dirs.extend([
            compression_config["lib"],
            compression_config["hdf5_blosc_lib"],
        ])
        libraries.extend(["blosc", "blosc_filter"])
        extra_link_args.extend([
            f"-Wl,-rpath,{compression_config['lib']}",
            f"-Wl,-rpath,{compression_config['hdf5_blosc_lib']}",
        ])
        extra_compile_args.append("-DENABLE_COMPRESSION")
        print("Compression support enabled.")
    else:
        print("Compression support disabled (blosc libraries not found).")

    return {
        "include_dirs": [str(path) for path in include_dirs],
        "library_dirs": [str(path) for path in library_dirs],
        "libraries": libraries,
        "extra_compile_args": extra_compile_args,
        "extra_link_args": extra_link_args,
    }

# source files for the extension
SOURCE_FILES = [
    "src/mib2h5/_wrapper.pyx",
    "../src/mib_to_h5.c",
    "../src/append.c",
    "../src/compress.c",
    "../src/framebuffer.c",
    "../src/hdf5_init.c",
    "../src/hdf5_init_meta.c",
    "../src/io_header.c",
    "../src/parser.c",
    "../src/read.c",
    "../src/utils.c",
]

config = _build_extension_config()

ext = Extension(
    name="mib2h5._wrapper",
    sources=SOURCE_FILES,
    **config,
)

setup(
    name="mib2h5",
    packages=["mib2h5"],
    package_dir={"": "src"},
    ext_modules=cythonize(ext, language_level=3),
    python_requires=">=3.10",
    cmdclass={"build_ext": ConfigureBuildExt},
)
