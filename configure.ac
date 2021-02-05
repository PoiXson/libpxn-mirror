AC_INIT([libpxn], [0.1], [support@poixson.com])
AC_PACKAGE_URL([https://poixson.com])

# clear default flags
: ${CFLAGS=""}
AC_PREREQ([2.69])
LT_PREREQ([2.4.6])

AC_DISABLE_SHARED
AC_ENABLE_STATIC

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_LANG(C)

#AC_CONFIG_SRCDIR([src/utils/StringUtils.c])
#AC_CONFIG_SRCDIR([src/utils/StringUtils.h])

LT_INIT([static disable-shared])
AM_INIT_AUTOMAKE([subdir-objects foreign serial-tests])
AM_CFLAGS="-Wall -Werror"
AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--enable-debug],
		[enable debug data generation (default=no)])],
	[enable_debug="$enableval"],
	[enable_debug=no])
if test "x$enable_debug" = xyes; then
	AC_MSG_RESULT([Debug Mode])
	AC_DEFINE([DEBUG],[],[Debug Mode])
	AM_CFLAGS="$AM_CFLAGS -g -Wno-uninitialized -O0"
else
	AC_MSG_RESULT([Production Mode])
    AC_DEFINE([NDEBUG],[],[No-debug Mode])
    AM_CFLAGS="$AM_CFLAGS -O3"
fi
echo "AM_CFLAGS=$AM_CFLAGS"
AC_SUBST([AM_CFLAGS])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
