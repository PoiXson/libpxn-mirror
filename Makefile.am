lib_LTLIBRARIES = libpxn.la
libpxn_la_SOURCES = \
	src/libpxn.c \
	src/StringUtils.c \
	src/FileUtils.c \
	src/logs.c \
	src/ProgFlags.c \
	src/run.c \
	src/Testing.c
pkginclude_HEADERS = \
	src/StringUtils.h \
	src/FileUtils.h \
	src/logs.h \
	src/ProgFlags.h \
	src/run.h \
	src/defines.h \
	src/Testing.h
libpxn_la_LDFLAGS = -static

check_PROGRAMS = pxntest
pxntest_LDADD = -lpxn
pxntest_SOURCES = \
	tests/tests.c
