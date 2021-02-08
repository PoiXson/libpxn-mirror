
#include "src/Testing.h"

#include "test_StringUtils.c"
#include "test_NumberUtils.c"
#include "test_FileUtils.c"

#include <stdio.h>



TEST_MAIN;



void tests() {
	RUN_TEST(test_StringUtils);
	RUN_TEST(test_NumberUtils);
	RUN_TEST(test_FileUtils);
}
