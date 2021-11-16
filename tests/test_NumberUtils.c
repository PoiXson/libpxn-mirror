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

#include "src/NumberUtils.h"



void test_NumberUtils() {

	// test min/max
	assert_int_cmp(__LINE__, 0, MIN( 0, 0));
	assert_int_cmp(__LINE__, 0, MIN( 0, 9));
	assert_int_cmp(__LINE__, 0, MIN( 9, 0));
	assert_int_cmp(__LINE__, 1, MIN( 1, 2));
	assert_int_cmp(__LINE__,-9, MIN(-9, 9));
	assert_int_cmp(__LINE__,-9, MIN(-9,-5));
	assert_int_cmp(__LINE__, 0, MAX( 0, 0));
	assert_int_cmp(__LINE__, 9, MAX( 0, 9));
	assert_int_cmp(__LINE__, 9, MAX( 9, 0));
	assert_int_cmp(__LINE__, 2, MAX( 1, 2));
	assert_int_cmp(__LINE__, 9, MAX(-9, 9));
	assert_int_cmp(__LINE__,-5, MAX(-9,-5));

	// pow()
	{
		// int power
		assert_int_cmp(__LINE__,    0, pow_int( 2, 0));
		assert_int_cmp(__LINE__,    2, pow_int( 2, 1));
		assert_int_cmp(__LINE__,    4, pow_int( 2, 2));
		assert_int_cmp(__LINE__,   16, pow_int( 2, 4));
		assert_int_cmp(__LINE__,   10, pow_int(10, 1));
		assert_int_cmp(__LINE__,  100, pow_int(10, 2));
		assert_int_cmp(__LINE__, 1000, pow_int(10, 3));
		// size_t power
		assert_sizet_cmp(__LINE__,    0, pow_sizet( 2, 0));
		assert_sizet_cmp(__LINE__,    2, pow_sizet( 2, 1));
		assert_sizet_cmp(__LINE__,    4, pow_sizet( 2, 2));
		assert_sizet_cmp(__LINE__,   16, pow_sizet( 2, 4));
		assert_sizet_cmp(__LINE__,   10, pow_sizet(10, 1));
		assert_sizet_cmp(__LINE__,  100, pow_sizet(10, 2));
		assert_sizet_cmp(__LINE__, 1000, pow_sizet(10, 3));
	}

	// test len_int()
	{
		assert_sizet_cmp(__LINE__,  9, len_int( 123456789));
		assert_sizet_cmp(__LINE__, 10, len_int(-123456789));
		assert_sizet_cmp(__LINE__,  1, len_int(   0));
		assert_sizet_cmp(__LINE__,  1, len_int(   1));
		assert_sizet_cmp(__LINE__,  2, len_int(  -1));
		assert_sizet_cmp(__LINE__,  3, len_int( 999));
		assert_sizet_cmp(__LINE__,  4, len_int(-999));
	}

	// test len_sizet()
	{
		assert_sizet_cmp(__LINE__, 19, len_sizet(LONG_MAX       ));
		assert_sizet_cmp(__LINE__, 18, len_sizet(LONG_MAX / 10LU));
		assert_sizet_cmp(__LINE__, 19, len_sizet( -1));
		assert_sizet_cmp(__LINE__,  1, len_sizet(  0));
		assert_sizet_cmp(__LINE__,  1, len_sizet(  1));
		assert_sizet_cmp(__LINE__,  3, len_sizet(999));
	}

	// test i_to_str()
	{
		char *str;
		str = i_to_str(123456789);
			assert_str_cmp(__LINE__,  "123456789", str); free(str);
		str = i_to_str(-123456789);
			assert_str_cmp(__LINE__, "-123456789", str); free(str);
		str = i_to_str(0);
			assert_str_cmp(__LINE__,          "0", str); free(str);
		str = i_to_str(1);
			assert_str_cmp(__LINE__,          "1", str); free(str);
		str = i_to_str(-1);
			assert_str_cmp(__LINE__,         "-1", str); free(str);
		str = i_to_str(999);
			assert_str_cmp(__LINE__,        "999", str); free(str);
		str = i_to_str(-999);
			assert_str_cmp(__LINE__,       "-999", str); free(str);
	}

}
