AC_INIT([mib2h5], [0.0.1], [https://github.com/ePSIC-DLS/mib2h5/issues], [mib2h5], [https://github.com/ePSIC-DLS/mib2h5])
AM_INIT_AUTOMAKE([-Wall foreign -Werror tar-pax dist-xz])
AC_PROG_CC
AM_SILENT_RULES([yes])
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

AC_MSG_NOTICE([Check the path of --with-hdf5])
AS_IF([test -n "$HDFDIR"], [
  # User gave --with-hdf5 — use it directly
  CPPFLAGS="$CPPFLAGS -I$HDFDIR/include"
  LDFLAGS="$LDFLAGS -L$HDFDIR/lib -Wl,-rpath=$HDFDIR/lib"

  AC_CHECK_HEADER([hdf5.h], [], [AC_MSG_ERROR([Missing hdf5.h in $HDFDIR/include])])
  AC_CHECK_LIB([hdf5], [H5open], [], [AC_MSG_ERROR([Missing libhdf5 in $HDFDIR/lib])])
  AC_DEFINE([HAVE_HDF5], [1], [Define if HDF5 is available])
],
[
  # No --with-hdf5: try different common path
  AC_MSG_NOTICE([Check the path of $HDF5_ROOT])
  AS_IF([test -n "$HDF5_ROOT"], [
    CPPFLAGS="$CPPFLAGS -I$HDF5_ROOT/include"
    LDFLAGS="$LDFLAGS -L$HDF5_ROOT/lib -Wl,-rpath=$HDF5_ROOT/lib"

    AC_CHECK_HEADER([hdf5.h], [], [AC_MSG_ERROR([Missing hdf5.h in $HDF5_ROOT/include])])
    AC_CHECK_LIB([hdf5], [H5open], [], [AC_MSG_ERROR([Missing libhdf5 in $HDF5_ROOT/lib])])
    AC_DEFINE([HAVE_HDF5], [1], [Define if HDF5 is available])
  ],
  [
    AC_MSG_NOTICE([Check the path of $HDF5_HOME])
    AS_IF([test -n "$HDF5_HOME"], [
      CPPFLAGS="$CPPFLAGS -I$HDF5_HOME/include"
      LDFLAGS="$LDFLAGS -L$HDF5_HOME/lib -Wl,-rpath=$HDF5_HOME/lib"

      AC_CHECK_HEADER([hdf5.h], [], [AC_MSG_ERROR([Missing hdf5.h in $HDF5_HOME/include])])
      AC_CHECK_LIB([hdf5], [H5open], [], [AC_MSG_ERROR([Missing libhdf5 in $HDF5_HOME/lib])])
      AC_DEFINE([HAVE_HDF5], [1], [Define if HDF5 is available])
    ],
    [
      AC_MSG_NOTICE([Check the path of $HDF5_DIR])
      AS_IF([test -n "$HDF5_DIR"], [
        CPPFLAGS="$CPPFLAGS -I$HDF5_DIR/include"
        LDFLAGS="$LDFLAGS -L$HDF5_DIR/lib -Wl,-rpath=$HDF5_DIR/lib"

        AC_CHECK_HEADER([hdf5.h], [], [AC_MSG_ERROR([Missing hdf5.h in $HDF5_DIR/include])])
        AC_CHECK_LIB([hdf5], [H5open], [], [AC_MSG_ERROR([Missing libhdf5 in $HDF5_DIR/lib])])
        AC_DEFINE([HAVE_HDF5], [1], [Define if HDF5 is available])
      ],
      [
        AC_MSG_NOTICE([for system-wide HDF5 installation])
        AC_CHECK_HEADER([hdf5.h], [
          AC_CHECK_LIB([hdf5], [H5open], [
            AC_DEFINE([HAVE_HDF5], [1], [Define if HDF5 is available])
          ], [AC_MSG_ERROR([libhdf5 not found system-wide])])
        ], [AC_MSG_ERROR([hdf5.h not found system-wide])])
      ])
    ])
  ])
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

AS_IF([test "x$with_compression" != xno], [

  AC_MSG_NOTICE([Checking for Blosc compression support...])

  # --- If user gave paths, use them directly ---
  AS_IF([test -n "$BLOSCDIR" && test -n "$HBDIR"], [

    CPPFLAGS="$CPPFLAGS -I$BLOSCDIR/include -I$HBDIR/src"
    LDFLAGS="$LDFLAGS -L$BLOSCDIR/lib64 -L$HBDIR/build"
    LDFLAGS="$LDFLAGS -Wl,-rpath=$BLOSCDIR/lib64 -Wl,-rpath=$HBDIR/build"

    AC_CHECK_HEADER([blosc.h], [],
      [AC_MSG_ERROR([blosc.h not found in $BLOSCDIR/include])])
    AC_CHECK_LIB([blosc], [blosc_init], [],
      [AC_MSG_ERROR([libblosc not found in $BLOSCDIR/lib64])])

    AC_CHECK_HEADER([blosc_filter.h], [],
      [AC_MSG_ERROR([blosc_filter.h not found in $HBDIR/src])])
    AC_CHECK_LIB([blosc_filter], [register_blosc], [],
      [AC_MSG_ERROR([libblosc_filter not found in $HBDIR/build])])

    AC_DEFINE([HAVE_COMPRESSION], [1], [Define if compression is enabled])
    AC_SUBST([LIBCOMPRESSION], ["-lblosc -lblosc_filter"])

  ], [

    # --- No user path: try system-wide ---
    AC_CHECK_HEADER([blosc.h], [
      AC_CHECK_LIB([blosc], [blosc_init], [BLOSC_OK=yes])
    ])
    AC_CHECK_HEADER([blosc_filter.h], [
      AC_CHECK_LIB([blosc_filter], [register_blosc], [HDF5_BLOSC_OK=yes])
    ])

    AS_IF([test "x$BLOSC_OK" = xyes && test "x$HDF5_BLOSC_OK" = xyes], [
      AC_DEFINE([HAVE_COMPRESSION], [1], [Define if compression is enabled])
      AC_SUBST([LIBCOMPRESSION], ["-lblosc -lblosc_filter"])
    ], [
      AS_IF([test "x$with_compression" != xcheck], [
        AC_MSG_FAILURE([Compression requested but blosc and hdf5-blosc not found system-wide, and no --with-blosc or --with-hdf5-blosc given])
      ])
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
