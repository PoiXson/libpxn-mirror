#pragma once

#include <stdbool.h>
#include <time.h>
#include <limits.h>



#define TEST_MAIN \
int main(int argc, char *argv[]) { \
	test_main(argc, argv); \
}

#define TEST_INIT(NAME) \
void NAME##_init() { \
	test_init(__##FILE__); \
}

#define RUN_TEST(NAME) \
	NAME##_init(); \
	printf(" ["); \
	NAME(); \
	printf("]\n");



extern bool abort_on_fail;

extern int count_tests;
extern int count_success;
extern int count_failed;

extern clock_t time_start;



#define TEST_MSG_SIZE 100

typedef struct TestPoint {
	bool used;
	char file[PATH_MAX];
	int line;
	clock_t timestamp;
	bool success;
	char msg[TEST_MSG_SIZE];
} TestPoint;

extern TestPoint *test_points;
extern size_t test_points_size;



void test_main(int argc, char *argv[]);
void test_results_display();
void tests();

size_t test_point_allocate();
void test_point_init(const size_t index);

void test_init(char *file);

void assert(char *file, const int line, const bool test);
void assert_strcmp(char *file, const int line, char *expected, char *actual);
void assert_intcmp(char *file, const int line, int expected, int actual);
