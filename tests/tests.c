
#include "src/Testing.h"

#include "test_StringUtils.c"
#include "test_NumberUtils.c"
#include "test_FileUtils.c"
#include "test_tpl.c"
#include "test_run.c"

#include <stdio.h>



TEST_MAIN("libpxn-test")



void tests() {
	RUN_TEST(test_StringUtils,    "StringUtils");
	RUN_TEST(test_NumberUtils,    "NumberUtils");
	RUN_TEST(test_FileUtils,      "FileUtils");
	RUN_TEST(test_tpl,            "tpl");
	RUN_TEST(test_parse_cmd_args, "test_parse_cmd_args");
	RUN_TEST(test_run,            "run");
}
