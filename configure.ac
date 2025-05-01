AC_INIT([mib2h5], [1.5.2025], [teo.ching@diamond.ac.uk])
AM_INIT_AUTOMAKE([-Wall foreign -Werror])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])

# --- Build Type Options --
AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug], [Enable debug build (default: yes)]),
  [debug_build=$enableval], [debug_build=yes])

AC_ARG_ENABLE([asan],
  AS_HELP_STRING([--enable-asan], [Enable AddressSanitizer build]),
  [asan_build=$enableval], [asan_build=no])

# --- External paths (can be passed as environment variables) ---
AC_ARG_VAR([HDFDIR], [Path to HDF5 installation])
AC_ARG_VAR([BLOSCDIR], [Path to c-blosc installation])
AC_ARG_VAR([HBDIR], [Path to hdf5-blosc installation])

# --- Set compiler flags based on build type ---
if test "$asan_build" = "yes"; then
  CFLAGS="$CFLAGS -Wall -Wextra -std=c11 -g -O0 -fsanitize=address"
  LDFLAGS="$LDFLAGS -fsanitize=address"
elif test "$debug_build" = "yes"; then
  CFLAGS="$CFLAGS -Wall -Wextra -std=c11 -g -O0 -fanalyzer"
else
  CFLAGS="$CFLAGS -Wall -Wextra -std=c11 -O2"
fi

# --- Set preprocessor and linker paths based on environment variables ---
CPPFLAGS="$CPPFLAGS -I$HDFDIR/include -I$BLOSCDIR/include -I$HBDIR/src"
LDFLAGS="$LDFLAGS -L$HDFDIR/lib -L$BLOSCDIR/lib64 -L$HBDIR/build -Wl,-rpath=${BLOSCDIR}/lib64 -Wl,-rpath=${HBDIR}/build"

# --- Check for HDF5 ---
AC_CHECK_HEADER([hdf5.h], [],
  [AC_MSG_ERROR([Missing hdf5.h. Set HDFDIR correctly to the HDF5 install path.])])
AC_CHECK_LIB([hdf5], [H5open], [],
  [AC_MSG_ERROR([Missing libhdf5. Set HDFDIR correctly to the HDF5 install path.])])

# --- Check for BLOSC ---
AC_CHECK_HEADER([blosc.h], [],
  [AC_MSG_ERROR([Missing blosc.h. Set BLOSCDIR correctly to the BLOSC install path.])])
AC_CHECK_LIB([blosc], [blosc_init], [],
  [AC_MSG_ERROR([Missing libblosc. Set BLOSCDIR correctly to the BLOSC install path.])])

# --- Check for hdf5-blosc ---
AC_CHECK_HEADER([blosc_filter.h], [],
  [AC_MSG_ERROR([Missing blosc_filter.h. Set HBDIR correctly to the hdf5-blosc source path.])])
AC_CHECK_LIB([blosc_filter], [register_blosc], [],
  [AC_MSG_ERROR([Missing libblosc_filter. Set HBDIR correctly to the hdf5-blosc build path.])])

AC_OUTPUT
