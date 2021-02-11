
#include "src/Testing.h"

#include "src/StringUtils.h"

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

	// test strlcmp()
	strlcpy(str, "Abc", size);
		assert_intcmp(__FILE__, __LINE__, -65, strlcmp(str, "", size));
		assert_intcmp(__FILE__, __LINE__,   0, strlcmp(str, "Abc", size));
		assert_intcmp(__FILE__, __LINE__,  -1, strlcmp(str, "Abb", size));
		assert_intcmp(__FILE__, __LINE__,   1, strlcmp(str, "Abd", size));
		assert_intcmp(__FILE__, __LINE__, -99, strlcmp(str, "Ab",  size));
		assert_intcmp(__FILE__, __LINE__, 100, strlcmp(str, "Abcd", size));
		assert_intcmp(__FILE__, __LINE__,   0, strlcmp(str, "Abcxyz", 3));
		assert_intcmp(__FILE__, __LINE__, 120, strlcmp(str, "Abcxyz", 4));
		assert_intcmp(__FILE__, __LINE__, 120, strlcmp(str, "Abcxyz", 6));

	// test str_trim()
	char *result;
	result = str_trim(str);
		assert_strcmp(__FILE__, __LINE__, "Abc", result);
	strlcpy(str, "   \t Abc \t   ", size);
	result = str_trim(str);
		assert_strcmp(__FILE__, __LINE__, "Abc", result);

	// test chrpos()
	strlcpy(str, "abcdefg", size);
	pos = chrpos(str, 'd');
		assert_intcmp(__FILE__, __LINE__, 3, pos);
	pos = chrpos(str, 'z');
		assert_intcmp(__FILE__, __LINE__, -1, pos);
	// test chrposs()
	strlcpy(str, "abcdefg", size);
	pos = chrposs(str, 'd', 3);
		assert_intcmp(__FILE__, __LINE__, 3, pos);
	pos = chrposs(str, 'd', 4);
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
