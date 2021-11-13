AC_INIT([libpxn], [0.1], [support@poixson.com])
AC_PACKAGE_URL([https://poixson.com])

# clear default flags
: ${CFLAGS=""}

AC_PREREQ([2.69])
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_LANG(C)

# expected files
AC_CONFIG_SRCDIR([src/libpxn_version.h])
AC_CONFIG_SRCDIR([src/StringUtils.c])
AC_CONFIG_SRCDIR([src/StringUtils.h])
AC_CONFIG_SRCDIR([src/cjson/cJSON.c])
AC_CONFIG_SRCDIR([src/cjson/cJSON.h])

AC_DISABLE_SHARED
AC_ENABLE_STATIC
LT_INIT([static disable-shared])

AM_INIT_AUTOMAKE([subdir-objects foreign])
CFLAGS="-Wall -Werror"
AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--enable-debug],
		[enable debug data generation (default=no)])],
	[enable_debug="$enableval"],
	[enable_debug=no])
if test "x$enable_debug" = xyes; then
	AC_MSG_RESULT([Debug Mode])
	AC_DEFINE([DEBUG],[],[Debug Mode])
	CFLAGS="$CFLAGS -Og -ggdb3 -Wno-uninitialized"
else
	AC_MSG_RESULT([Production Mode])
    AC_DEFINE([NDEBUG],[],[No-debug Mode])
    CFLAGS="$CFLAGS -O3 -D_FORTIFY_SOURCE"
fi
echo -ne "\nCFLAGS=$CFLAGS\n\n"
AC_SUBST([CFLAGS])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
