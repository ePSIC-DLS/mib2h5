AC_INIT([mib2h5], [0.0.1], [https://github.com/ePSIC-DLS/mib2h5/issues], [mib2h5], [https://github.com/ePSIC-DLS/mib2h5])
AM_INIT_AUTOMAKE([-Wall foreign -Werror tar-pax dist-xz])
AC_PROG_CC
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_HEADERS([config.h])

# --- Build Type Options --
AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug], [Enable debug build]),
  [debug_build=$enableval], [debug_build=no])

AC_ARG_ENABLE([asan],
  AS_HELP_STRING([--enable-asan], [Enable AddressSanitizer build]),
  [asan_build=$enableval], [asan_build=no])

# --- Check for HDF5 ---

AC_ARG_WITH([hdf5],
  [AS_HELP_STRING([--with-hdf5=PATH], [Path to HDF5 installation])],
  [HDFDIR="$withval"],
  [])

HDF5_OK=no

AS_IF([test -n "$HDFDIR"], [
  AC_MSG_NOTICE([Test HDFDIR provided])
  CPPFLAGS="$CPPFLAGS -I$HDFDIR/include"
  LDFLAGS="$LDFLAGS -L$HDFDIR/lib"

  AC_CHECK_HEADER([hdf5.h], [
    AC_CHECK_LIB([hdf5], [H5open], [
      AC_DEFINE([HAVE_HDF5], [1], [Define if HDF5 is available])
      HDF5_OK=yes
    ], [
      AC_MSG_NOTICE([libhdf5 not found on HDFDIR])
      HDF5_OK=no
    ])
  ], [
    AC_MSG_NOTICE([hdf5.h not found on HDFDIR])
    HDF5_OK=no
  ])
])

AS_IF([test -z "$HDFDIR" || test "x$HDF5_OK" != xyes], [
  AC_MSG_NOTICE([--with-hdf5 not provided, checking common HDF5 environment variables...])
  AS_IF([test -n "$HDF5_ROOT"], [HDFDIR="$HDF5_ROOT"])
  AS_IF([test -z "$HDFDIR" && test -n "$HDF5_HOME"], [HDFDIR="$HDF5_HOME"])
  AS_IF([test -z "$HDFDIR" && test -n "$HDF5_DIR"], [HDFDIR="$HDF5_DIR"])

  AS_IF([test -n "$HDFDIR"], [
    AC_MSG_NOTICE([Trying HDF5 from $HDFDIR])
    CPPFLAGS="$CPPFLAGS -I$HDFDIR/include"
    LDFLAGS="$LDFLAGS -L$HDFDIR/lib -Wl,-rpath=$HDFDIR/lib"
    AC_CHECK_HEADER([hdf5.h], [
      AC_CHECK_LIB([hdf5], [H5open], [
        AC_DEFINE([HAVE_HDF5], [1], [Define if HDF5 is available])
        HDF5_OK=yes
      ])
    ])
  ], [
    AC_MSG_NOTICE([Trying system-wide HDF5 installation])
    AC_CHECK_HEADER([hdf5.h], [
      AC_CHECK_LIB([hdf5], [H5open], [
        AC_DEFINE([HAVE_HDF5], [1], [Define if HDF5 is available])
        HDF5_OK=yes
      ], [
        AC_MSG_NOTICE([libhdf5 not found system-wide])
        HDF5_OK=no
      ])
    ], [
      AC_MSG_ERROR([hdf5.h not found system-wide])
      HDF5_OK=no
    ])
  ])
])

AS_IF([test "x$HDF5_OK" != xyes], [
  AC_MSG_ERROR([HDF5 not found])
], [
  AC_MSG_NOTICE([HDF5 found in $HDFDIR])
])

# ---------------- Compression Option ----------------
AC_ARG_ENABLE([compression],
  [AS_HELP_STRING([--enable-compression], [Enable compression with blosc and hdf5-blosc])],
  [],
  [enable_compression=check])

AC_ARG_WITH([blosc],
  [AS_HELP_STRING([--with-blosc=PATH], [Path to c-blosc installation])],
  [BLOSCDIR="$withval"],
  [])

AC_ARG_WITH([hdf5-blosc],
  [AS_HELP_STRING([--with-hdf5-blosc=PATH], [Path to hdf5-blosc installation])],
  [HBDIR="$withval"],
  [])

AS_IF([test "x$enable_compression" != xno], [
  AC_MSG_NOTICE([Checking for Blosc compression support...])
  # --- If user gave paths, use them directly ---
  AS_IF([test -n "$BLOSCDIR"], [
    CPPFLAGS="$CPPFLAGS -I$BLOSCDIR/include"
    LDFLAGS="$LDFLAGS -L$BLOSCDIR/lib64 -Wl,-rpath=$BLOSCDIR/lib64"

    AC_CHECK_HEADER([blosc.h], [
      AC_CHECK_LIB([blosc], [blosc_init], [BLOSC_OK=yes],
        [AC_MSG_ERROR([libblosc not found in $BLOSCDIR/lib64])])
    ],
    [AC_MSG_ERROR([blosc.h not found in $BLOSCDIR/include])])
  ], [
    AC_CHECK_HEADER([blosc.h], [
      AC_CHECK_LIB([blosc], [blosc_init], [BLOSC_OK=yes])
    ])
  ])

  AS_IF([test -n "$HBDIR"], [
    CPPFLAGS="$CPPFLAGS -I$HBDIR/src"
    LDFLAGS="$LDFLAGS -L$HBDIR/build -Wl,-rpath=$HBDIR/build"

    AC_CHECK_HEADER([blosc_filter.h], [
      AC_CHECK_LIB([blosc_filter], [register_blosc], [HDF5_BLOSC_OK=yes],
        [AC_MSG_ERROR([libblosc_filter not found in $HBDIR/build])])
    ],
    [AC_MSG_ERROR([blosc_filter.h not found in $HBDIR/src])])
  ], [
    AC_CHECK_HEADER([blosc_filter.h], [
      AC_CHECK_LIB([blosc_filter], [register_blosc], [HDF5_BLOSC_OK=yes])
    ])
  ])

  AS_IF([test "x$BLOSC_OK" = xyes && test "x$HDF5_BLOSC_OK" = xyes], [
    AC_DEFINE([HAVE_COMPRESSION], [1], [Define if compression is enabled])
    AC_SUBST([LIBCOMPRESSION], ["-lblosc -lblosc_filter"])
  ], [
    AS_IF([test "x$enable_compression" != xcheck], [
      AC_MSG_FAILURE([Compression requested but blosc and hdf5-blosc not found system-wide, and no --with-blosc and --with-hdf5-blosc given])
    ])
  ])
])

# --- Set compiler flags based on build type ---

AS_IF([test "x$asan_build" = "xyes"], [
  CFLAGS="$CFLAGS -Wall -Wextra -std=c11 -g -O0 -fsanitize=address"
  LDFLAGS="$LDFLAGS -fsanitize=address"
], [test "x$debug_build" = "xyes"], [
  CFLAGS="$CFLAGS -Wall -Wextra -std=c11 -g -O0 -fanalyzer"
], [
  CFLAGS="$CFLAGS -Wall -Wextra -std=c11 -O2"
])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
