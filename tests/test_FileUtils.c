
#include "src/FileUtils.h"
#include "src/Testing.h"

#include <string.h>



TEST_INIT(test_FileUtils);
void test_FileUtils() {

	// test get_file_type()
	assert_intcmp(__FILE__, __LINE__, is_dir,  get_file_type("/etc"));
	assert_intcmp(__FILE__, __LINE__, is_file, get_file_type("/etc/redhat-release"));
	assert_intcmp(__FILE__, __LINE__, not_found, get_file_type("/abcdefg12345gfedcba"));
	
	// test get_lock() and free_lock()
//TODO: maybe use pwd

	// test load_text_file() and read_text_file()
	{
		char *out;
		int len = load_text_file("/etc/redhat-release", &out);
		assert(__FILE__, __LINE__, len > 0);
		assert(__FILE__, __LINE__, strlen(out) > 0);
	}

	// test save_text_file()
//TODO

	// test copy_file()
//TODO

}
