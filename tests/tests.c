
#include "src/Testing.h"

#include <stdio.h>

#include "test_StringUtils.c"
#include "test_NumberUtils.c"
#include "test_FileUtils.c"



TEST_MAIN;



void tests() {
	RUN_TEST(test_StringUtils, "StringUtils");
	RUN_TEST(test_NumberUtils, "NumberUtils");
	RUN_TEST(test_FileUtils,   "FileUtils");
}
