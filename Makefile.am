lib_LTLIBRARIES = libpxn.la
libpxn_la_LDFLAGS = -static
libpxn_la_CFLAGS = -fPIC

check_PROGRAMS = test-libpxn
test_libpxn_LDADD = -lpxn
test_libpxn_SOURCES = tests/tests.c

libpxn_la_SOURCES = \
	src/libpxn.c      \
	src/Debug.c       \
	src/Testing.c     \
	src/MemUtils.c    \
	src/StringUtils.c \
	src/NumberUtils.c \
	src/FileUtils.c   \
	src/SystemUtils.c \
	src/Logs.c        \
	src/Params.c      \
	src/Run.c         \
	src/tpl.c         \
	src/tpl_parse.c   \
	src/tpl_render.c  \
	src/cjson/cJSON.c
pkginclude_HEADERS = \
	src/libpxn_version.h \
	src/pxnDefines.h     \
	src/Debug.h          \
	src/Testing.h        \
	src/MemUtils.h       \
	src/StringUtils.h    \
	src/NumberUtils.h    \
	src/FileUtils.h      \
	src/SystemUtils.h    \
	src/Logs.h           \
	src/Params.h         \
	src/Run.h            \
	src/tpl.h            \
	src/cjson/cJSON.h
