//===============================================================================
// Copyright (c) 2020-2021 PoiXson, Mattsoft
// <https://poixson.com> <https://mattsoft.net>
// Released under the GPL 3.0
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//===============================================================================
#include "src/Testing.h"

#include "src/StringUtils.h"

#include <string.h>



TEST_INIT(test_StringUtils)
void test_StringUtils() {
	size_t size = 11;
	char str[size];

	// test strlcpy()
	strlcpy(str, "qwerty", size);
		assert(strcmp(str, "qwerty") == 0);
	strlcpy(str, "abc", size);
		assert(strcmp(str, "abc") == 0);
	// test strlcat()
	strlcat(str, "def", size);
		assert(strcmp(str, "abcdef") == 0);
	strlcat(str, "ghijklmnopqrstuvwxyz", size);
		assert_strcmp("abcdefghij", str);
	// test strlcatfront
	strlcpy(str, "def", size);
	strlcatfront(str, "abc", size);
		assert_strcmp("abcdef", str);
	strlcatfront(str, "123456", size);
		assert_strcmp("123456abcd", str);
	strlcatfront(str, "ABCD", 5);
		assert_strcmp("ABCD", str);

	// test str reallocate functions
	{
		size_t sizeB;
		char *strB;
		// test strrcpy() strrcat()
		sizeB = 2;
		strB = calloc(sizeB, sizeof(char));
		strrcpy(&strB, &sizeB, "abcd");
			assert_intcmp(5, sizeB);
			assert_strcmp("abcd", strB);
		free(strB);
		sizeB = 2;
		strB = calloc(sizeB, sizeof(char));
		strrcat(&strB, &sizeB, "abcd");
			assert_intcmp(8, sizeB);
			assert_strcmp("abcd", strB);
		strrcat(&strB, &sizeB, "efg");
			assert_intcmp(8, sizeB);
			assert_strcmp("abcdefg", strB);
		strrcat(&strB, &sizeB, "hijklmnopqrstuvwxyz");
			assert_intcmp(32, sizeB);
			assert_strcmp("abcdefghijklmnopqrstuvwxyz", strB);
		free(strB);
		// test strlrcpy() strlrcat()
		sizeB = 2;
		strB = calloc(sizeB, sizeof(char));
		strlrcpy(&strB, &sizeB, "abc", 3);
			assert_intcmp(4, sizeB);
			assert_strcmp("abc", strB);
		free(strB);
		sizeB = 2;
		strB = calloc(sizeB, sizeof(char));
		strlrcat(&strB, &sizeB, "abc", 3);
			assert_intcmp(4, sizeB);
			assert_strcmp("abc", strB);
		strlrcat(&strB, &sizeB, "def", 1);
			assert_intcmp(8, sizeB);
			assert_strcmp("abcd", strB);
		strlrcat(&strB, &sizeB, "ef", 20);
			assert_intcmp(8, sizeB);
			assert_strcmp("abcdef", strB);
		free(strB);
	}

	// test strlcmp()
	strlcpy(str, "Abc", size);
		assert_intcmp(-65, strlcmp(str, "", size));
		assert_intcmp(  0, strlcmp(str, "Abc", size));
		assert_intcmp( -1, strlcmp(str, "Abb", size));
		assert_intcmp(  1, strlcmp(str, "Abd", size));
		assert_intcmp(-99, strlcmp(str, "Ab",  size));
		assert_intcmp(100, strlcmp(str, "Abcd", size));
		assert_intcmp(  0, strlcmp(str, "Abcxyz", 3));
		assert_intcmp(120, strlcmp(str, "Abcxyz", 4));
		assert_intcmp(120, strlcmp(str, "Abcxyz", 6));

	// test str_trim()
	char *result;
	result = str_trim(str);
		assert_strcmp("Abc", result);
	strlcpy(str, "   \t Abc \t   \n", size);
	result = str_trim(str);
		assert_strcmp("Abc", result);

	// test str_sum()
	result = str_sum("Abc");
		assert_strcmp("Abc", result);
		free(result);
	result = str_sum(" Abc\n Efg");
		assert_strcmp("Abc", result);
		free(result);
	result = str_sum("\n\tAbc\n\tEfg");
		assert_strcmp("Abc", result);
		free(result);
	result = str_sum("\n\n\n");
		assert_strcmp("", result);
		free(result);
	// test str_unescape
	result = str_unescape("Abc");
		assert_strcmp("Abc", result);
		free(result);
	result = strl_unescape("A\tb\rc\nd\0e", 9);
		assert_strcmp("A\\tb\\rc\\nd\\0e", result);
		free(result);
	result = strl_unescape("\t\r\n\0", 4);
		assert_strcmp("\\t\\r\\n\\0", result);
		free(result);

	// test chrpos()
	strlcpy(str, "abcdefg", size);
		assert_intcmp( 0, chrpos(str, 'a'));
		assert_intcmp( 3, chrpos(str, 'd'));
		assert_intcmp( 6, chrpos(str, 'g'));
		assert_intcmp(-1, chrpos(str, 'z'));
	// test chrposs()
	strlcpy(str, "abcdefg", size);
		assert_intcmp( 0, chrposs(str, 'a', 0));
		assert_intcmp(-1, chrposs(str, 'a', 1));
		assert_intcmp( 3, chrposs(str, 'd', 3));
		assert_intcmp(-1, chrposs(str, 'd', 4));
	strlcpy(str, "abc\n\n\n", size);
	size_t pos;
		assert_intcmp( 3, pos = chrposs(str, '\n', 0));
		assert_intcmp( 4, pos = chrposs(str, '\n', pos+1));
		assert_intcmp( 5, pos = chrposs(str, '\n', pos+1));
		assert_intcmp(-1, pos = chrposs(str, '\n', pos+1));

	// test pad_front()
	strlcpy(str, "abc", size);
	str_pad_front(str, 9);
		assert_strcmp("      abc", str);
	strlcpy(str, "abc", size);
	str_pad_front(str, 2);
		assert_strcmp("abc", str);
	// test pad_end()
	strlcpy(str, "abc", size);
	str_pad_end(str, 9);
		assert_strcmp("abc      ", str);
	strlcpy(str, "abc", size);
	str_pad_end(str, 2);
		assert_strcmp("abc", str);
	// test pad_center()
	strlcpy(str, "abc", size);
	str_pad_center(str, 9);
		assert_strcmp("   abc   ", str);
	strlcpy(str, "abc", size);
	str_pad_center(str, 2);
		assert_strcmp("abc", str);

	// test str_starts_with()
	strlcpy(str, "abc", size);
		assert( str_starts_with(str, "ab"));
		assert(!str_starts_with(str, "abcd"));
	// test str_ends_with()
	assert( str_ends_with(str, "bc"));
	assert(!str_ends_with(str, "ab"));

	// test str_make_safe()
	strlcpy(str, "a b\tc!d@efg", size);
	str_make_safe(str, size);
		assert_strcmp("a_b_c_d_ef", str);
	strlcpy(str, "abc", size);
	str[5]      = 'A';
	str[size-1] = 'B';
	str_make_safe(str, size);
		assert_strcmp("abc", str);
		assert(str[5]      == '\0');
		assert(str[size-1] == '\0');

}
