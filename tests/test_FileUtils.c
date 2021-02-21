
#include "src/Testing.h"

#include "src/FileUtils.h"

#include <stdlib.h>
#include <string.h>



TEST_INIT(test_FileUtils)
void test_FileUtils() {

	// test get_file_type()
	assert_intcmp(IS_DIR,  get_file_type("/etc"));
	assert_intcmp(IS_FILE, get_file_type("/etc/redhat-release"));
	assert_intcmp(NOT_FOUND, get_file_type("/abcdefg12345gfedcba"));

	// test basename()
//TODO

	// test build_path()
	{
		char *path = calloc(PATH_MAX, sizeof(char));
		build_path(path, 2, "/etc/", "///redhat-release");
			assert_strcmp("/etc/redhat-release", path);
		build_path(path, 3, "/var/log/", "cups", "error_log");
			assert_strcmp("/var/log/cups/error_log", path);
		build_path(path, 2, "data", "file.txt");
			assert_strcmp("data/file.txt", path);
		free(path);
	}

	// test get_lock() and free_lock()
//TODO: maybe use pwd

	// test load_text_file() and read_text_file()
	{
		char *out;
		int len = load_text_file("/etc/redhat-release", &out);
		assert(len > 0);
		assert(strlen(out) > 0);
	}

	// test save_text_file()
//TODO

	// test copy_file()
//TODO

}
