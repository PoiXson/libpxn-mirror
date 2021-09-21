lib_LTLIBRARIES = libpxn.la
libpxn_la_LDFLAGS = -static
libpxn_la_CFLAGS = -fPIC

check_PROGRAMS = libpxn-test
libpxn_test_LDADD = -lpxn
libpxn_test_SOURCES = tests/tests.c

libpxn_la_SOURCES = \
	src/libpxn.c      \
	src/Debug.c       \
	src/StringUtils.c \
	src/NumberUtils.c \
	src/FileUtils.c   \
	src/SystemUtils.c \
	src/Logs.c        \
	src/Params.c      \
	src/Run.c         \
	src/tpl-engine.c  \
	src/Testing.c     \
	src/cjson/cJSON.c
pkginclude_HEADERS = \
	src/version.h     \
	src/pxnDefines.h  \
	src/Debug.h       \
	src/StringUtils.h \
	src/NumberUtils.h \
	src/FileUtils.h   \
	src/SystemUtils.h \
	src/Logs.h        \
	src/Params.h      \
	src/Run.h         \
	src/tpl-engine.h  \
	src/Testing.h     \
	src/cjson/cJSON.h
