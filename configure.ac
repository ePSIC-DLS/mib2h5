# 2.64 is needed for AS_VAR_SET
# check https://lists.gnu.org/archive/html/autotools-announce/2009-07/msg00000.html for detail
AC_PREREQ([2.64])
AC_INIT([mib2h5], [0.0.1], [https://github.com/ePSIC-DLS/mib2h5/issues], [mib2h5], [https://github.com/ePSIC-DLS/mib2h5])
AM_INIT_AUTOMAKE([-Wall foreign -Werror tar-pax dist-xz])
AC_USE_SYSTEM_EXTENSIONS
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AC_PROG_CC

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
  AC_MSG_NOTICE([Testing HDF5 at $HDFDIR])
  CPPFLAGS="$CPPFLAGS -I$HDFDIR/include"
  LDFLAGS="$LDFLAGS -L$HDFDIR/lib"

  AC_CHECK_HEADER([hdf5.h], [
    AC_CHECK_LIB([hdf5], [H5open], [
      AC_DEFINE([HAVE_HDF5], [1], [Define if HDF5 is available])
      HDF5_OK=yes
    ], [
      AC_MSG_ERROR([libhdf5 not found at $HDFDIR/lib])
    ])
  ], [
    AC_MSG_ERROR([hdf5.h not found at $HDFDIR/include])
  ])
])

AS_IF([test "x$HDF5_OK" != xyes], [
  AS_UNSET([ac_cv_header_hdf5_h])
  AC_MSG_NOTICE([Trying system-wide HDF5 installation])
  AC_CHECK_HEADER([hdf5.h], [
    AC_CHECK_LIB([hdf5], [H5open], [
      AC_DEFINE([HAVE_HDF5], [1], [Define if HDF5 is available])
      HDF5_OK=yes
      AC_MSG_NOTICE([HDF5 found in system path])
    ], [
      AC_MSG_NOTICE([libhdf5 not found system-wide])
      HDF5_OK=no
    ])
  ], [
    AC_MSG_NOTICE([hdf5.h not found system-wide])
    HDF5_OK=no
  ])
])

if test "x$HDF5_OK" != xyes; then
  for var in HDF5_ROOT HDF5_HOME HDF5_DIR; do
    eval dir=\$$var
    if test -n "$dir"; then
      AS_UNSET([ac_cv_header_hdf5_h])
      AC_MSG_NOTICE([Trying $var at $dir])
      CPPFLAGS="$CPPFLAGS -I$dir/include"
      LDFLAGS="$LDFLAGS -L$dir/lib -Wl,-rpath,$dir/lib"
      AC_CHECK_HEADER([hdf5.h], [
        AC_CHECK_LIB([hdf5], [H5open], [
          AC_DEFINE([HAVE_HDF5], [1], [Define if HDF5 is available])
          HDF5_OK=yes
          HDFDIR="$dir"
          AC_MSG_NOTICE([HDF5 found in $HDFDIR])
        ], [
          AC_MSG_NOTICE([libhdf5 not found at $dir/lib])
          AS_VAR_SET([CPPFLAGS], [`echo "$CPPFLAGS" | sed "s|-I$dir/include||g"`])
          AS_VAR_SET([LDFLAGS], [`echo "$LDFLAGS" | sed "s|-L$dir/lib||g; s|-Wl,-rpath,$dir/lib||g"`])
          HDF5_OK=no
        ])
      ], [
        AC_MSG_NOTICE([hdf5.h not found at $dir/include])
        AS_VAR_SET([CPPFLAGS], [`echo "$CPPFLAGS" | sed "s|-I$dir/include||g"`])
        AS_VAR_SET([LDFLAGS], [`echo "$LDFLAGS" | sed "s|-L$dir/lib||g; s|-Wl,-rpath,$dir/lib||g"`])
        HDF5_OK=no
      ])
      test "x$HDF5_OK" = xyes && break
    fi
  done
fi

AS_IF([test "x$HDF5_OK" != xyes], [
  AC_MSG_ERROR([HDF5 not found])
])

# ---------------- Compression Option ----------------
AC_ARG_ENABLE([compression],
  [AS_HELP_STRING([--enable-compression], [Enable compression with blosc and hdf5-blosc])],
  [],
  [enable_compression=no])

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

  BLOSC_OK=no

  AS_IF([test -n "$BLOSCDIR"], [
    AS_IF([test -d "$BLOSCDIR/lib64"], [
      BLOSC_LIBDIR="$BLOSCDIR/lib64"
    ], [
      AS_IF([test -d "$BLOSCDIR/lib"], [
        BLOSC_LIBDIR="$BLOSCDIR/lib"
      ], [
        AC_MSG_NOTICE([Neither lib64 nor lib directory exists in $BLOSCDIR])
        BLOSC_OK=no
      ])
    ])
  ])

  CPPFLAGS="$CPPFLAGS -I$BLOSCDIR/include"
  LDFLAGS="$LDFLAGS -L$BLOSC_LIBDIR -Wl,-rpath,$BLOSC_LIBDIR"

  AC_CHECK_HEADER([blosc.h], [
    AC_CHECK_LIB([blosc], [blosc_init], [BLOSC_OK=yes], [
      AS_VAR_SET([CPPFLAGS], [`echo "$CPPFLAGS" | sed "s|-I$BLOSCDIR/include||g"`])
      AS_VAR_SET([LDFLAGS], [`echo "$LDFLAGS" | sed "s|-L$BLOSC_LIBDIR||g; s|-Wl,-rpath,$BLOSC_LIBDIR||g"`])
      AC_MSG_NOTICE([libblosc not found in $BLOSC_LIBDIR])
      BLOSC_OK=no
    ])
  ], [
    AS_VAR_SET([CPPFLAGS], [`echo "$CPPFLAGS" | sed "s|-I$BLOSCDIR/include||g"`])
    AS_VAR_SET([LDFLAGS], [`echo "$LDFLAGS" | sed "s|-L$BLOSC_LIBDIR||g; s|-Wl,-rpath,$BLOSC_LIBDIR||g"`])
    AC_MSG_NOTICE([blosc.h not found in $BLOSCDIR/include])
    BLOSC_OK=no
  ])


  AS_IF([test "x$BLOSC_OK" != xyes], [
    AS_UNSET([ac_cv_lib_blosc_blosc_init])
    AS_UNSET([ac_cv_header_blosc_h])
    AC_MSG_NOTICE([Falling back on system path for blosc])
    AC_CHECK_HEADER([blosc.h], [
      AC_CHECK_LIB([blosc], [blosc_init], [BLOSC_OK=yes], [BLOSC_OK=no])
    ], [BLOSC_OK=no])
  ])

  AS_IF([test -n "$HBDIR"], [
    CPPFLAGS="$CPPFLAGS -I$HBDIR/src"
    LDFLAGS="$LDFLAGS -L$HBDIR/build -Wl,-rpath,$HBDIR/build"

    AC_CHECK_HEADER([blosc_filter.h], [
      AC_CHECK_LIB([blosc_filter], [register_blosc], [HDF5_BLOSC_OK=yes], [
        AS_VAR_SET([CPPFLAGS], [`echo "$CPPFLAGS" | sed "s|-I$HBDIR/src||g"`])
        AS_VAR_SET([LDFLAGS], [`echo "$LDFLAGS" | sed "s|-$HBDIR/build||g; s|-Wl,-rpath,$HBDIR/build||g"`])
        AC_MSG_NOTICE([libblosc_filter not found in $HBDIR/build])
        HDF5_BLOSC_OK=no
      ])
    ], [
      AS_VAR_SET([CPPFLAGS], [`echo "$CPPFLAGS" | sed "s|-I$HBDIR/src||g"`])
      AS_VAR_SET([LDFLAGS], [`echo "$LDFLAGS" | sed "s|-$HBDIR/build||g; s|-Wl,-rpath,$HBDIR/build||g"`])
      AC_MSG_NOTICE([blosc_filter.h not found in $HBDIR/src])
      HDF5_BLOSC_OK=no
    ])
  ])

  AS_IF([test "x$HDF5_BLOSC_OK" != xyes], [
    AS_UNSET([ac_cv_lib_blosc_filter_register_blosc])
    AS_UNSET([ac_cv_header_blosc_filter_h])
    AC_MSG_NOTICE([Falling back on system path for hdf5-blosc])
    AC_CHECK_HEADER([blosc_filter.h], [
      AC_CHECK_LIB([blosc_filter], [register_blosc], [HDF5_BLOSC_OK=yes], [HDF5_BLOSC_OK=no])
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
