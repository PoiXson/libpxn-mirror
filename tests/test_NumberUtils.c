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

	// test len_int()
	{
		assert_int_cmp(__LINE__, 9,  len_int(123456789));
		assert_int_cmp(__LINE__, 10, len_int(-123456789));
		assert_int_cmp(__LINE__, 1,  len_int(0));
		assert_int_cmp(__LINE__, 1,  len_int(1));
		assert_int_cmp(__LINE__, 2,  len_int(-1));
		assert_int_cmp(__LINE__, 3,  len_int(999));
		assert_int_cmp(__LINE__, 4,  len_int(-999));
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
