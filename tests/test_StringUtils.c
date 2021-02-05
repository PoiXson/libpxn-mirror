
#include "src/StringUtils.h"
#include "src/Testing.h"

#include <string.h>



TEST_INIT(test_StringUtils);
void test_StringUtils() {
	size_t size = 10;
	char str[size];
	size_t pos;

	// test strlcpy()
	strlcpy(str, "qwerty", size);
		assert(__FILE__, __LINE__, strcmp(str, "qwerty") == 0);
	strlcpy(str, "abc", size);
		assert(__FILE__, __LINE__, strcmp(str, "abc") == 0);
	// test strlcat()
	strlcat(str, "def", size);
		assert(__FILE__, __LINE__, strcmp(str, "abcdef") == 0);
	strlcat(str, "ghijkl", size);
		assert_strcmp(__FILE__, __LINE__, "abcdefghi", str);

	// test chrpos()
	pos = chrpos(str, 'd');
		assert_intcmp(__FILE__, __LINE__, 3, pos);
	pos = chrpos(str, 'z');
		assert_intcmp(__FILE__, __LINE__, -1, pos);

	// test pad_front()
	strlcpy(str, "abc", size);
	pad_front(str, 9);
		assert_strcmp(__FILE__, __LINE__, "      abc", str);
	strlcpy(str, "abc", size);
	pad_front(str, 2);
		assert_strcmp(__FILE__, __LINE__, "abc", str);
	// test pad_end()
	strlcpy(str, "abc", size);
	pad_end(str, 9);
		assert_strcmp(__FILE__, __LINE__, "abc      ", str);
	strlcpy(str, "abc", size);
	pad_end(str, 2);
		assert_strcmp(__FILE__, __LINE__, "abc", str);
	// test pad_center()
	strlcpy(str, "abc", size);
	pad_center(str, 9);
		assert_strcmp(__FILE__, __LINE__, "   abc   ", str);
	strlcpy(str, "abc", size);
	pad_center(str, 2);
		assert_strcmp(__FILE__, __LINE__, "abc", str);

	// test str_starts_with()
	assert(__FILE__, __LINE__,  str_starts_with(str, "ab"));
	assert(__FILE__, __LINE__, !str_starts_with(str, "abcd"));

	// test str_ends_with()
	assert(__FILE__, __LINE__,  str_ends_with(str, "bc"));
	assert(__FILE__, __LINE__, !str_ends_with(str, "ab"));

}
