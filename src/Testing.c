//===============================================================================
// Copyright (c) 2020-2021 PoiXson, Mattsoft
// <https://poixson.com> <https://mattsoft.net>
// Released under the GPL 3.0
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//===============================================================================
#include "Testing.h"

#include "StringUtils.h"
#include "Params.h"
#include "Logs.h"
#include "pxnDefines.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



bool abort_on_fail;

size_t count_tests   = 0;
size_t count_success = 0;
size_t count_failed  = 0;

clock_t time_start;

TestPoint *test_points;
size_t test_points_size = 0;



void test_main(int argc, char *argv[], char *exec_name) {
	#ifdef DEBUG
		printf("<Built with Debug Flags>");
	#else
		printf("<Production Build>");
	#endif
	size_t flag_abort   = param_add('a', 1, "abort",   "Abort on failed asserts [default]");
	param_add_spacer();
	size_t flag_color   = param_add('C', 1, "color",   "Enable console colors");
	size_t flag_ncolor  = param_add('\0',1, "no-color","Disable console colors");
	size_t flag_verbose = param_add('v', 1, "verbose", "Detailed logging");
	size_t flag_help    = param_add('h', 1, "help",    "Display this help message and exit");
	params_process(argc, argv, exec_name);
	abort_on_fail = params_get_bool(flag_abort);
	if (params_get_bool(flag_ncolor)) {
		set_color_enabled(false);
	} else
	if (params_get_bool(flag_color)) {
		set_color_enabled(true);
	}
	if (params_get_bool(flag_verbose)) {
		log_level_set(LVL_ALL);
	}
	// --help
	if (params_get_bool(flag_help)) {
		display_help();
		exit(1);
	}
	time_start = clock();
	// run tests
	printf("\n");
	tests();
	printf("\n");
	// results
	test_results_display( params_get_bool(flag_verbose) );
	if (count_success == 0 && count_failed == 0)
		exit(1);
	if (count_failed > 0)
		exit(1);
	exit(0);
}

void test_results_display(bool display_detail) {
	clock_t time_end = clock();
	double elapsed = ( ((double)time_end) - ((double)time_start) ) / CLOCKS_PER_SEC;
	if (display_detail || count_failed > 0) {
		double last = time_start;
		double duration;
		for (size_t index=0; index<test_points_size; index++) {
			if (test_points[index].used != true)
				continue;
			if (test_points[index].line == -1) {
				last = test_points[index].timestamp;
				continue;
			}
			if (display_detail || ! test_points[index].success) {
				duration = ( ((double)test_points[index].timestamp) - last ) / CLOCKS_PER_SEC;
				printf(
					" [%c] %.3f %s %s:%i",
					(test_points[index].success ? ' ' : 'X'),
					(duration > 0.01 ? duration : duration * 1000.0),
					(duration > 0.01 ? "s " : "ms"),
					test_points[index].file,
					test_points[index].line
				);
				if (strlen(test_points[index].msg) > 0) {
					printf("%s", test_points[index].msg);
				}
				printf("\n");
			}
			last = test_points[index].timestamp;
		}
	}
	bool color = has_color_enabled();
	{
		printf(
			"\nRan %lu test set%s in %.3f %s\n",
			count_tests,
			(count_tests == 1 ? "" : "s"),
			(elapsed > 0.01 ? elapsed : elapsed * 1000.0),
			(elapsed > 0.01 ? "seconds" : "milliseconds")
		);
		if (count_success == 0 && count_failed == 0) {
			printf(" %sNo test asserts to run!%s\n",
				(color ? COLOR_RED   : ""),
				(color ? COLOR_RESET : "")
			);
		} else
		if (count_failed == 0) {
			printf(
				" %s<PASS> %lu passing asserts%s\n",
				(color ? COLOR_GREEN : ""),
				count_success,
				(color ? COLOR_RESET : "")
			);
		} else {
			printf(" %lu passing asserts\n", count_success);
			printf(" %s<FAIL> %lu failed asserts!%s\n",
				(color ? COLOR_RED : ""),
				count_failed,
				(color ? COLOR_RESET : "")
			);
		}
	}
	printf("\n");
}



size_t test_point_allocate() {
	clock_t timestamp = clock();
	// allocate first block
	if (test_points_size == 0) {
		test_points_size = 32;
		test_points = malloc(test_points_size * sizeof(TestPoint));
		if (test_points == NULL) {
			printf("Failed to allocate test_points array");
			exit(1);
		}
		for (size_t index=0; index<test_points_size; index++) {
			test_point_init(index);
		}
		test_points[0].used = true;
		test_points[0].timestamp = timestamp;
		return 0;
	}
	// find space in existing array
	for (size_t index=0; index<test_points_size; index++) {
		if (test_points[index].used == false) {
			test_point_init(index);
			test_points[index].used = true;
			test_points[index].timestamp = timestamp;
			return index;
		}
	}
	// expand array
	test_points = realloc(test_points, sizeof(TestPoint) * test_points_size * 2);
	if (test_points == NULL) {
		printf("Failed to reallocate test_points array");
		exit(1);
	}
	for (size_t index=test_points_size; index<test_points_size*2; index++) {
		test_point_init(index);
	}
	test_points_size *= 2;
	{
		size_t index = test_point_allocate();
		test_points[index].timestamp = timestamp;
		// store a marker to reset the clock
		size_t i = test_point_allocate();
		strlcpy(test_points[i].file, "reallocate", PATH_MAX);
		test_points[i].line = -1;
		return index;
	}
}

void test_point_init(const size_t index) {
	test_points[index].used = false;
	memset(test_points[index].file, '\0', PATH_MAX);
	test_points[index].line      = 0;
	test_points[index].timestamp = 0;
	test_points[index].success   = false;
	memset(test_points[index].msg, '\0', TEST_MSG_SIZE);
}



void test_init(char *file) {
	count_tests++;
	size_t index = test_point_allocate();
	strlcpy(test_points[index].file, file, PATH_MAX);
	test_points[index].line = -1;
	test_points[index].timestamp = clock();
}

void _assert(char *file, const int line, const bool test) {
	size_t index = test_point_allocate();
	strlcpy(test_points[index].file, file, PATH_MAX);
	test_points[index].line = line;
	test_points[index].success = test;
	if (test) {
		TEST_PRINT_DOT;
	} else {
		TEST_PRINT_X;
		TEST_ABORT_FAIL;
	}
}

void _assert_null(char *file, const int line, void *value, bool invert) {
	size_t index = test_point_allocate();
	strlcpy(test_points[index].file, file, PATH_MAX);
	test_points[index].line = line;
	bool test = (value == NULL);
	// not null
	if (invert)
		test = !test;
	if (test) {
		test_points[index].success = true;
		TEST_PRINT_DOT;
	} else {
		test_points[index].success = false;
		TEST_PRINT_X;
		snprintf(
			test_points[index].msg,
			TEST_MSG_SIZE,
			"\n   expected: %sNULL\n   actual: %sNULL",
			(invert ? "NOT " : ""),
			(invert ? "" : "NOT ")
		);
		TEST_ABORT_FAIL;
	}
}

void _assert_strcmp(char *file, const int line, char *expected, char *actual) {
	size_t index = test_point_allocate();
	strlcpy(test_points[index].file, file, PATH_MAX);
	test_points[index].line = line;
	int result = strcmp(expected, actual);
	if (result == 0) {
		test_points[index].success = true;
		TEST_PRINT_DOT;
	} else {
		test_points[index].success = false;
		TEST_PRINT_X;
		char *exp = str_unescape(expected);
		char *act = str_unescape(actual);
		snprintf(
			test_points[index].msg,
			TEST_MSG_SIZE,
			"\n   expected: '%s'\n   actual: '%s'",
			exp,
			act
		);
		free(exp);
		free(act);
		TEST_ABORT_FAIL;
	}
}

void _assert_intcmp(char *file, const int line, int expected, int actual) {
	size_t index = test_point_allocate();
	strlcpy(test_points[index].file, file, PATH_MAX);
	test_points[index].line = line;
	if (expected == actual) {
		test_points[index].success = true;
		TEST_PRINT_DOT;
	} else {
		test_points[index].success = false;
		TEST_PRINT_X;
		snprintf(
			test_points[index].msg,
			TEST_MSG_SIZE,
			"\n   expected: %i actual: %i",
			expected,
			actual
		);
		TEST_ABORT_FAIL;
	}
}

void _assert_sztcmp(char *file, const int line, size_t expected, size_t actual) {
	size_t index = test_point_allocate();
	strlcpy(test_points[index].file, file, PATH_MAX);
	test_points[index].line = line;
	if (expected == actual) {
		test_points[index].success = true;
		TEST_PRINT_DOT;
	} else {
		test_points[index].success = false;
		TEST_PRINT_X;
		snprintf(
			test_points[index].msg,
			TEST_MSG_SIZE,
			"\n   expected: %lu actual: %lu",
			expected,
			actual
		);
		TEST_ABORT_FAIL;
	}
}
