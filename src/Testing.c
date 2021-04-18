#include "Testing.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "StringUtils.h"
#include "ProgFlags.h"
#include "Logs.h"
#include "pxnDefines.h"



bool abort_on_fail;

size_t count_tests   = 0;
size_t count_success = 0;
size_t count_failed  = 0;

clock_t time_start;

TestPoint *test_points;
size_t test_points_size = 0;



void test_main(int argc, char *argv[], char *exec_name) {
	size_t flag_abort   = progflags_add('a', 1, "abort",   "Abort on failed asserts [default]");
	progflags_add_spacer();
	size_t flag_verbose = progflags_add('v', 1, "verbose", "Detailed logging");
	size_t flag_help    = progflags_add('h', 1, "help",    "Display this help message and exit");
	progflags_process(argc, argv, exec_name);
	abort_on_fail = progflags_get_bool(flag_abort);
	if (progflags_get_bool(flag_verbose)) {
		log_level_set(LVL_ALL);
	}
	// --help
	if (progflags_get_bool(flag_help)) {
		display_help();
		exit(1);
	}
	time_start = clock();
	// run tests
	printf("\n");
	tests();
	printf("\n");
	// results
	test_results_display( progflags_get_bool(flag_verbose) || count_failed > 0 );
	if (count_success == 0 && count_failed == 0)
		exit(1);
	if (count_failed > 0)
		exit(1);
	exit(0);
}

void test_results_display(bool display_detail) {
	clock_t time_end = clock();
	double elapsed = ( ((double)time_end) - ((double)time_start) ) / CLOCKS_PER_SEC;
	if (display_detail) {
		double last = time_start;
		double duration;
		for (size_t index=0; index<test_points_size; index++) {
			if (test_points[index].used != true)
				continue;
			if (test_points[index].line == -1) {
				last = test_points[index].timestamp;
				continue;
			}
			duration = ( ((double)test_points[index].timestamp) - last ) / CLOCKS_PER_SEC;
			printf(
				" [%c] %.3f %s %s:%i",
				(test_points[index].success ? ' ' : 'X'),
				(duration > 0.001 ? duration : duration * 1000.0),
				(duration > 0.001 ? "s" : "ms"),
				test_points[index].file,
				test_points[index].line
			);
			if (strlen(test_points[index].msg) > 0) {
				printf("%s", test_points[index].msg);
			}
			printf("\n");
			last = test_points[index].timestamp;
		}
	}
	{
		printf(
			"\nRan %lu test set%s in %.3f %s\n",
			count_tests,
			(count_tests == 1 ? "" : "s"),
			(elapsed > 0.001 ? elapsed : elapsed * 1000.0),
			(elapsed > 0.001 ? "seconds" : "milliseconds")
		);
		if (count_success == 0 && count_failed == 0) {
			printf(" "ANSI_COLOR_RED"No test asserts to run!"ANSI_COLOR_RESET"\n", count_failed);
		} else
		if (count_failed == 0) {
			printf(" "ANSI_COLOR_GREEN"%lu passing asserts"ANSI_COLOR_RESET"\n", count_success);
			printf(ANSI_COLOR_GREEN"<PASS>"ANSI_COLOR_RESET"\n");
		} else {
			printf(" %lu passing asserts\n", count_success);
			printf(" "ANSI_COLOR_RED"%lu failed asserts!"ANSI_COLOR_RESET"\n", count_failed);
			printf(ANSI_COLOR_RED"<FAIL>"ANSI_COLOR_RESET"\n");
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
		count_success++;
		printf(".");
	} else {
		count_failed++;
		printf("x");
		if (abort_on_fail) {
			test_results_display(true);
			exit(1);
		}
	}
}

void _assert_strcmp(char *file, const int line, char *expected, char *actual) {
	size_t index = test_point_allocate();
	strlcpy(test_points[index].file, file, PATH_MAX);
	test_points[index].line = line;
	int result = strcmp(expected, actual);
	if (result == 0) {
		test_points[index].success = true;
		count_success++;
		printf(".");
	} else {
		test_points[index].success = false;
		count_failed++;
		printf("x");
		snprintf(
			test_points[index].msg,
			TEST_MSG_SIZE,
			"\n   expected: '%s'\n   actual: '%s'",
			expected,
			actual
		);
		if (abort_on_fail) {
			test_results_display(true);
			exit(1);
		}
	}
}

void _assert_intcmp(char *file, const int line, int expected, int actual) {
	size_t index = test_point_allocate();
	strlcpy(test_points[index].file, file, PATH_MAX);
	test_points[index].line = line;
	if (expected == actual) {
		test_points[index].success = true;
		count_success++;
		printf(".");
	} else {
		test_points[index].success = false;
		count_failed++;
		printf("x");
		snprintf(
			test_points[index].msg,
			TEST_MSG_SIZE,
			"\n   expected: %i actual: %i",
			expected,
			actual
		);
		if (abort_on_fail) {
			test_results_display(true);
			exit(1);
		}
	}
}
