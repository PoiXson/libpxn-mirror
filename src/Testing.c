#include "Testing.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "StringUtils.h"
#include "defines.h"



int count_tests   = 0;
int count_success = 0;
int count_failed  = 0;

clock_t time_start;

TestPoint *test_points;
size_t test_points_size = 0;



int test_main(int argc, char *argv[]) {
	time_start = clock();
	// run tests
	printf("\n");
	tests();
	printf("\n");
	// results
	clock_t time_end = clock();
	double elapsed = ( ((double)time_end) - ((double)time_start) ) / CLOCKS_PER_SEC;
	{
		double last = time_start;
		double duration;
		for (size_t index=0; index<test_points_size; index++) {
			if (!test_points[index].used)
				continue;
			if (test_points[index].line == -1) {
				last = test_points[index].timestamp;
				continue;
			}
			duration = ( ((double)test_points[index].timestamp) - last ) / CLOCKS_PER_SEC;
			printf(
				" [%c] %.3f %s %s:%i\n",
				(test_points[index].success ? ' ' : 'X'),
				(duration > 0.001 ? duration : duration * 1000.0),
				(duration > 0.001 ? "s" : "ms"),
				test_points[index].file,
				test_points[index].line
			);
			if (strlen(test_points[index].msg) > 0) {
				printf("   %s", test_points[index].msg);
			}
			last = test_points[index].timestamp;
		}
	}
	{
		printf(
			"\nRan %i test%s in %.3f %s\n",
			count_tests,
			(count_tests == 1 ? "" : "s"),
			(elapsed > 0.001 ? elapsed : elapsed * 1000.0),
			(elapsed > 0.001 ? "seconds" : "milliseconds")
		);
		printf(" "ANSI_COLOR_GREEN"%i passing asserts"ANSI_COLOR_RESET"\n", count_success);
		if (count_failed > 0) {
			printf(" "ANSI_COLOR_RED"%i failed asserts!"ANSI_COLOR_RESET"\n", count_failed);
		}
	}
	printf("\n");
	if (count_failed > 0)
		return 1;
	return 0;
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

void assert(char *file, const int line, const bool test) {
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
	}
}

void assert_strcmp(char *file, const int line, char *expected, char *actual) {
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
		   "expected: %s actual: %s",
			expected,
		   actual
		);
	}
}

void assert_intcmp(char *file, const int line, int expected, int actual) {
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
		   "expected: %i actual: %i",
			expected,
		   actual
		);
	}
}
