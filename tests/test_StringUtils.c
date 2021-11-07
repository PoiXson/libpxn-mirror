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

#include "src/MemUtils.h"



void test_StringUtils() {

	// test str_empty()
	assert    (__LINE__, str_empty(NULL ));
	assert    (__LINE__, str_empty(""   ));
	assert_not(__LINE__, str_empty(" "  ));
	assert_not(__LINE__, str_empty("Abc"));
	// test str_not_empty()
	assert_not(__LINE__, str_not_empty(NULL ));
	assert_not(__LINE__, str_not_empty(""   ));
	assert    (__LINE__, str_not_empty(" "  ));
	assert    (__LINE__, str_not_empty("Abc"));

	// test str_len()
	assert_int_cmp(__LINE__, 0, str_len(NULL   ));
	assert_int_cmp(__LINE__, 0, str_len(""     ));
	assert_int_cmp(__LINE__, 3, str_len("Abc"  ));
	assert_int_cmp(__LINE__, 3, str_len("Abc\0"));
	// test str_l_len()
	assert_int_cmp(__LINE__, 0, str_l_len(NULL,    0));
	assert_int_cmp(__LINE__, 0, str_l_len(NULL,   11));
	assert_int_cmp(__LINE__, 3, str_l_len("Abc",  11));
	assert_int_cmp(__LINE__, 0, str_l_len("",      3));
	assert_int_cmp(__LINE__, 0, str_l_len("Abc",   0));
	assert_int_cmp(__LINE__, 1, str_l_len("Abc",   1));
	assert_int_cmp(__LINE__, 3, str_l_len("Abc",   3));
	assert_int_cmp(__LINE__, 3, str_l_len("Abc\0", 3));
	assert_int_cmp(__LINE__, 3, str_l_len("Abcd",  3));

	// test str_dup();
	{
		char *line = str_dup("\r\n\t Abc \t\r\n");
			assert_str_cmp(__LINE__, "\r\n\t Abc \t\r\n", line);
	}

	// test str_l_cpy()
	{
		size_t tmp_size = 11;
		assert_sizet_cmp(__LINE__, 0, str_l_cpy(NULL, "Abc", tmp_size));
		char *tmp = calloc(tmp_size, sizeof(char));
		assert_sizet_cmp(__LINE__, 6, str_l_cpy(tmp, "qwerty", tmp_size));
			assert_str_cmp(__LINE__, "qwerty", tmp);
		assert_sizet_cmp(__LINE__, 3, str_l_cpy(tmp, "Abc", tmp_size));
			assert_str_cmp(__LINE__, "Abc", tmp);
		assert_sizet_cmp(__LINE__, 2, str_l_cpy(tmp, "Abc", 3));
			assert_str_cmp(__LINE__, "Ab", tmp);
		free(tmp);
	}

	// test str_l_cat()
	{
		size_t tmp_size = 11;
		char *tmp = calloc(tmp_size, sizeof(char));
		assert_sizet_cmp(__LINE__, 3, str_l_cat(tmp, "Abc", tmp_size));
			assert_str_cmp(__LINE__, "Abc", tmp);
		assert_sizet_cmp(__LINE__, 6, str_l_cat(tmp, "Def", tmp_size));
			assert_str_cmp(__LINE__, "AbcDef", tmp);
		free(tmp);
	}

	// test str_la_cat()
	{
		size_t tmp_size = 0;
		char *tmp = NULL;
		assert_sizet_cmp(__LINE__, 3, str_la_cat(&tmp, &tmp_size, "Abc", 3));
			assert_sizet_cmp(__LINE__, 4, tmp_size);
			assert_str_cmp  (__LINE__, "Abc", tmp);
		assert_sizet_cmp(__LINE__, 6, str_la_cat(&tmp, &tmp_size, "Def", 3));
			assert_sizet_cmp(__LINE__, 7, tmp_size);
			assert_str_cmp  (__LINE__, "AbcDef", tmp);
		free(tmp);
	}

	// test snprintf_alloc()
	{
		size_t tmp_size = 0;
		char *tmp = NULL;
		tmp = snprintf_alloc(&tmp_size, "Abc %s %i", "def", 123);
			assert_sizet_cmp(__LINE__, 11, tmp_size);
			assert_str_cmp  (__LINE__, "Abc def 123", tmp);
		free(tmp);
	}

	// test str_cmp()
	assert_int_cmp(__LINE__,   0, str_cmp(NULL,  NULL  ));
	assert_int_cmp(__LINE__, -65, str_cmp("Abc", NULL  ));
	assert_int_cmp(__LINE__,  65, str_cmp(NULL,  "Abc" ));
	assert_int_cmp(__LINE__,   0, str_cmp("Abc", "Abc" ));
	assert_int_cmp(__LINE__, -65, str_cmp("Abc", ""    ));
	assert_int_cmp(__LINE__,  65, str_cmp("",    "Abc" ));
	assert_int_cmp(__LINE__,   1, str_cmp("Abb", "Abc" ));
	assert_int_cmp(__LINE__,  -1, str_cmp("Abc", "Abb" ));
	assert_int_cmp(__LINE__,   1, str_cmp("Abc", "Abd" ));
	assert_int_cmp(__LINE__,  -1, str_cmp("Abd", "Abc" ));
	assert_int_cmp(__LINE__, 100, str_cmp("Abc", "Abcd"));
	// test str_l_cmp()
	assert_int_cmp(__LINE__,   0, str_l_cmp(NULL,  NULL,     3));
	assert_int_cmp(__LINE__, -65, str_l_cmp("Abc", NULL,     3));
	assert_int_cmp(__LINE__,  65, str_l_cmp(NULL,  "Abc",    3));
	assert_int_cmp(__LINE__,   0, str_l_cmp("Abc", "Abc",    3));
	assert_int_cmp(__LINE__, -65, str_l_cmp("Abc", ""   ,    3));
	assert_int_cmp(__LINE__,  65, str_l_cmp("",    "Abc",    3));
	assert_int_cmp(__LINE__,   1, str_l_cmp("Abb", "Abc",    3));
	assert_int_cmp(__LINE__,  -1, str_l_cmp("Abc", "Abb",    3));
	assert_int_cmp(__LINE__,   1, str_l_cmp("Abc", "Abd",    3));
	assert_int_cmp(__LINE__,  -1, str_l_cmp("Abd", "Abc",    3));
	assert_int_cmp(__LINE__,   0, str_l_cmp("Abc", "Abcd",   3));
	assert_int_cmp(__LINE__, 100, str_l_cmp("Abc", "Abcd",   4));
	assert_int_cmp(__LINE__,   0, str_l_cmp("Abc", "Abcxyz", 3));
	assert_int_cmp(__LINE__, 120, str_l_cmp("Abc", "Abcxyz", 4));
	assert_int_cmp(__LINE__, 120, str_l_cmp("Abc", "Abcxyz", 6));

	{
		size_t str_size = 11;
		char *str = calloc(str_size, sizeof(char));
		// test chr_pos()
		str_l_cpy(str, "abcdefg", str_size);
			assert_int_cmp(__LINE__,  0, chr_pos(str, 'a'));
			assert_int_cmp(__LINE__,  3, chr_pos(str, 'd'));
			assert_int_cmp(__LINE__,  6, chr_pos(str, 'g'));
			assert_int_cmp(__LINE__, -1, chr_pos(str, 'z'));
		// test chr_s_pos()
		str_l_cpy(str, "abcdefg", str_size);
			assert_int_cmp(__LINE__,  0, chr_s_pos(str, 'a', 0));
			assert_int_cmp(__LINE__, -1, chr_s_pos(str, 'a', 1));
			assert_int_cmp(__LINE__,  3, chr_s_pos(str, 'd', 3));
			assert_int_cmp(__LINE__, -1, chr_s_pos(str, 'd', 4));
		str_l_cpy(str, "abc\n\n\n", str_size);
		size_t pos;
			assert_int_cmp(__LINE__,  3, pos = chr_s_pos(str, '\n', 0));
			assert_int_cmp(__LINE__,  4, pos = chr_s_pos(str, '\n', pos+1));
			assert_int_cmp(__LINE__,  5, pos = chr_s_pos(str, '\n', pos+1));
			assert_int_cmp(__LINE__, -1, pos = chr_s_pos(str, '\n', pos+1));
		free(str);
	}

	// str_trim_left()
	{
		char *line = str_dup("\r\n\t Abc \t\r\n");
		char *result = str_trim_left(line);
			assert_str_cmp(__LINE__, "Abc \t\r\n", result);
		line[0] = '\0';
		result = str_trim_left(line);
			assert_str_cmp(__LINE__, "", result);
		free(line);
	}
	// str_trim_right()
	{
		char *line = str_dup("\r\n\t Abc \t\r\n");
		str_trim_right(line);
			assert_str_cmp(__LINE__, "\r\n\t Abc", line);
		line[0] = '\0';
		str_trim_right(line);
			assert_str_cmp(__LINE__, "", line);
		free(line);
	}
	// str_trim()
	{
		char *line = str_dup("\r\n\t Abc \t\r\n");
		char *result = str_trim(line);
			assert_str_cmp(__LINE__, "Abc", result);
		line[0] = '\0';
		result = str_trim(line);
			assert_str_cmp(__LINE__, "", result);
		free(line);
	}

	// test to upper/lower
	{
		size_t tmp_size = 11;
		char *tmp = NULL;
		str_to_upper(tmp); assert_str_cmp(__LINE__, NULL, tmp);
		str_to_lower(tmp); assert_str_cmp(__LINE__, NULL, tmp);
		tmp = calloc(tmp_size, sizeof(char));
		str_to_upper(tmp); assert_str_cmp(__LINE__, "", tmp);
		str_to_lower(tmp); assert_str_cmp(__LINE__, "", tmp);
		str_l_cpy(tmp, "Abc", 4);
		str_to_upper(tmp); assert_str_cmp(__LINE__, "ABC", tmp);
		str_to_lower(tmp); assert_str_cmp(__LINE__, "abc", tmp);
		free(tmp);
	}

	// test str_pad_front()
	{
		size_t tmp_size = 11;
		char *tmp = calloc(tmp_size, sizeof(char));
		str_l_cpy(tmp, "Abc", 4);
		str_pad_front(tmp, 2); assert_str_cmp(__LINE__, "Abc",   tmp);
		str_pad_front(tmp, 4); assert_str_cmp(__LINE__, "Abc",   tmp);
		str_pad_front(tmp, 6); assert_str_cmp(__LINE__, "  Abc", tmp);
		free(tmp);
	}
	// test str_pad_end()
	{
		size_t tmp_size = 11;
		char *tmp = calloc(tmp_size, sizeof(char));
		str_l_cpy(tmp, "Abc", 4);
		str_pad_end(tmp, 2); assert_str_cmp(__LINE__, "Abc",   tmp);
		str_pad_end(tmp, 4); assert_str_cmp(__LINE__, "Abc",   tmp);
		str_pad_end(tmp, 6); assert_str_cmp(__LINE__, "Abc  ", tmp);
		free(tmp);
	}
	// test str_pad_center()
	{
		size_t tmp_size = 11;
		char *tmp = calloc(tmp_size, sizeof(char));
		str_l_cpy(tmp, "Abc", 4);
		str_pad_center(tmp, 2); assert_str_cmp(__LINE__, "Abc",   tmp);
		str_pad_center(tmp, 4); assert_str_cmp(__LINE__, "Abc",   tmp);
		str_pad_center(tmp, 6); assert_str_cmp(__LINE__, " Abc ", tmp);
		free(tmp);
	}

	// test str_make_safe()
	{
		size_t tmp_size = 11;
		char *tmp = calloc(tmp_size, sizeof(char));
		str_l_cpy(tmp, "a b\tc!d@efg", tmp_size);
		str_l_make_safe(tmp, tmp_size);
			assert_str_cmp(__LINE__, "a_b_c_d_ef", tmp);
		str_l_cpy(tmp, "abc", tmp_size);
		tmp[5]          = 'A';
		tmp[tmp_size-1] = 'B';
		str_l_make_safe(tmp, tmp_size);
			assert_str_cmp(__LINE__, "abc", tmp);
			assert(__LINE__, tmp[5]          == '\0');
			assert(__LINE__, tmp[tmp_size-1] == '\0');
		free(tmp);
	}

	// test str_unescape
	{
		char *result;
		result = str_la_unescape("Abc", 3);
			assert_str_cmp(__LINE__, "Abc", result);
			free(result);
		result = str_la_unescape("Abc", 4);
			assert_str_cmp(__LINE__, "Abc\\0", result);
			free(result);
		result = str_la_unescape("A\tb\rc\nd\0e", 9);
			assert_str_cmp(__LINE__, "A\\tb\\rc\\nd\\0e", result);
			free(result);
		result = str_la_unescape("\t\r\n\0", 4);
			assert_str_cmp(__LINE__, "\\t\\r\\n\\0", result);
			free(result);
	}

	// test str_sum()
	{
		char *result;
		result = str_a_sum("Abc");
			assert_str_cmp(__LINE__, "Abc", result);
			free(result);
		result = str_a_sum(" Abc\n Efg");
			assert_str_cmp(__LINE__, "Abc", result);
			free(result);
		result = str_a_sum("\n\tAbc\n\tEfg");
			assert_str_cmp(__LINE__, "Abc", result);
			free(result);
		result = str_a_sum("\n\n\n");
			assert_str_cmp(__LINE__, "", result);
			free(result);
	}

}
