#pragma once

#include <stdbool.h>
#include <time.h>
#include <limits.h>



#define TEST_NAME_MAX 16



#define TEST_MAIN \
int main(int argc, char *argv[]) { \
	test_main(argc, argv); \
}

#define TEST_INIT(NAME) \
void NAME##_init() { \
	test_init(__##FILE__); \
}

#define RUN_TEST(NAME_FUNC,NAME_STR) \
	{ \
		NAME_FUNC##_init(); \
		char name_padded[TEST_NAME_MAX]; \
		strlcpy(name_padded, NAME_STR, TEST_NAME_MAX); \
		str_pad_front(name_padded, TEST_NAME_MAX); \
		printf("%s [", name_padded); \
		NAME_FUNC(); \
		printf("]\n"); \
	}



extern bool abort_on_fail;

extern size_t count_tests;
extern size_t count_success;
extern size_t count_failed;

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



#define assert(test)                   _assert(       __FILE__, __LINE__, test)
#define assert_strcmp(expected,actual) _assert_strcmp(__FILE__, __LINE__, expected, actual)
#define assert_intcmp(expected,actual) _assert_intcmp(__FILE__, __LINE__, expected, actual)

void _assert(char *file, const int line, const bool test);
void _assert_strcmp(char *file, const int line, char *expected, char *actual);
void _assert_intcmp(char *file, const int line, int expected, int actual);
