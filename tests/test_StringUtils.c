
#include "src/Testing.h"

#include "src/StringUtils.h"

#include <string.h>



TEST_INIT(test_StringUtils);
void test_StringUtils() {
	size_t size = 10;
	char str[size];

	// test strlcpy()
	strlcpy(str, "qwerty", size);
		assert(__FILE__, __LINE__, strcmp(str, "qwerty") == 0);
	strlcpy(str, "abc", size);
		assert(__FILE__, __LINE__, strcmp(str, "abc") == 0);
	// test strlcat()
	strlcat(str, "def", size);
		assert(__FILE__, __LINE__, strcmp(str, "abcdef") == 0);
	strlcat(str, "ghijklmnopqrstuvwxyz", size);
		assert_strcmp(__FILE__, __LINE__, "abcdefghi", str);

	// test strlrcpy() strlrcat()
	{
		size_t sizeB = 3;
		char *strB = calloc(sizeB, sizeof(char));
		strlrcpy(&strB, "Ab", &sizeB);
			assert_intcmp(__FILE__, __LINE__, 3, sizeB);
			assert_strcmp(__FILE__, __LINE__, "Ab", strB);
		strlrcat(&strB, "cd", &sizeB);
			assert_intcmp(__FILE__, __LINE__, 5, sizeB);
			assert_strcmp(__FILE__, __LINE__, "Abcd", strB);
		free(strB);
		sizeB = 4;
		strB = calloc(sizeB, sizeof(char));
		strlrcpy(&strB, "Abcd", &sizeB);
			assert_intcmp(__FILE__, __LINE__, 5, sizeB);
			assert_strcmp(__FILE__, __LINE__, "Abcd", strB);
		strlrcat(&strB, "efghijklmnopqrstuvwxyz", &sizeB);
			assert_intcmp(__FILE__, __LINE__, 27, sizeB);
			assert_strcmp(__FILE__, __LINE__, "Abcdefghijklmnopqrstuvwxyz", strB);
		free(strB);
		sizeB = 1;
		strB = calloc(sizeB, sizeof(char));
		strlrcpy(
			&strB,
			"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
			"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
			"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
			"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
			"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",
		   &sizeB
		);
			assert_intcmp(__FILE__, __LINE__, 261, sizeB);
		strlrcat(&strB, "1234", &sizeB);
			assert_intcmp(__FILE__, __LINE__, 522, sizeB);
			assert_intcmp(__FILE__, __LINE__, 264, strlen(strB));
		free(strB);
	}

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
		assert_intcmp(__FILE__, __LINE__,  0, chrpos(str, 'a'));
		assert_intcmp(__FILE__, __LINE__,  3, chrpos(str, 'd'));
		assert_intcmp(__FILE__, __LINE__,  6, chrpos(str, 'g'));
		assert_intcmp(__FILE__, __LINE__, -1, chrpos(str, 'z'));
	// test chrposs()
	strlcpy(str, "abcdefg", size);
		assert_intcmp(__FILE__, __LINE__,  0, chrposs(str, 'a', 0));
		assert_intcmp(__FILE__, __LINE__, -1, chrposs(str, 'a', 1));
		assert_intcmp(__FILE__, __LINE__,  3, chrposs(str, 'd', 3));
		assert_intcmp(__FILE__, __LINE__, -1, chrposs(str, 'd', 4));

	// test pad_front()
	strlcpy(str, "abc", size);
	str_pad_front(str, 9);
		assert_strcmp(__FILE__, __LINE__, "      abc", str);
	strlcpy(str, "abc", size);
	str_pad_front(str, 2);
		assert_strcmp(__FILE__, __LINE__, "abc", str);
	// test pad_end()
	strlcpy(str, "abc", size);
	str_pad_end(str, 9);
		assert_strcmp(__FILE__, __LINE__, "abc      ", str);
	strlcpy(str, "abc", size);
	str_pad_end(str, 2);
		assert_strcmp(__FILE__, __LINE__, "abc", str);
	// test pad_center()
	strlcpy(str, "abc", size);
	str_pad_center(str, 9);
		assert_strcmp(__FILE__, __LINE__, "   abc   ", str);
	strlcpy(str, "abc", size);
	str_pad_center(str, 2);
		assert_strcmp(__FILE__, __LINE__, "abc", str);

	// test str_starts_with()
	assert(__FILE__, __LINE__,  str_starts_with(str, "ab"));
	assert(__FILE__, __LINE__, !str_starts_with(str, "abcd"));

	// test str_ends_with()
	assert(__FILE__, __LINE__,  str_ends_with(str, "bc"));
	assert(__FILE__, __LINE__, !str_ends_with(str, "ab"));

	// test str_make_safe()
	strlcpy(str, "a b\tc!d@efg", size);
	str_make_safe(str, size);
		assert_strcmp(__FILE__, __LINE__, "a_b_c_d_e", str);
	strlcpy(str, "abc", size);
	str[5]      = 'A';
	str[size-1] = 'B';
	str_make_safe(str, size);
		assert_strcmp(__FILE__, __LINE__, "abc", str);
		assert(__FILE__, __LINE__, str[5]      == '\0');
		assert(__FILE__, __LINE__, str[size-1] == '\0');

}
