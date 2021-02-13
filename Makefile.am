lib_LTLIBRARIES = libpxn.la
libpxn_la_LDFLAGS = -static
libpxn_la_CFLAGS = -fPIC

check_PROGRAMS = pxntest
pxntest_LDADD = -lpxn
pxntest_SOURCES = tests/tests.c

libpxn_la_SOURCES = \
	src/libpxn.c      \
	src/Debug.c       \
	src/StringUtils.c \
	src/NumberUtils.c \
	src/FileUtils.c   \
	src/Logs.c        \
	src/ProgFlags.c   \
	src/Run.c         \
	src/tpl-engine.c  \
	src/Testing.c
pkginclude_HEADERS = \
	src/pxnDefines.h  \
	src/Debug.h       \
	src/StringUtils.h \
	src/NumberUtils.h \
	src/FileUtils.h   \
	src/Logs.h        \
	src/ProgFlags.h   \
	src/Run.h         \
	src/tpl-engine.h  \
	src/Testing.h
