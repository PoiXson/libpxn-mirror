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



TEST_INIT(test_NumberUtils)
void test_NumberUtils() {
	int len;
	// test len_int()
	len = len_int(123456789);
		assert_intcmp(9, len);
	len = len_int(-123456789);
		assert_intcmp(10, len);
	len = len_int(0);
		assert_intcmp(1, len);
	len = len_int(1);
		assert_intcmp(1, len);
	len = len_int(-1);
		assert_intcmp(2, len);
	len = len_int(999);
		assert_intcmp(3, len);
	len = len_int(-999);
		assert_intcmp(4, len);
	// test itostr()
	char *str;
	str = itostr(123456789);
		assert_strcmp("123456789", str); free(str);
	str = itostr(-123456789);
		assert_strcmp("-123456789", str); free(str);
	str = itostr(0);
		assert_strcmp("0", str); free(str);
	str = itostr(1);
		assert_strcmp("1", str); free(str);
	str = itostr(-1);
		assert_strcmp("-1", str); free(str);
	str = itostr(999);
		assert_strcmp("999", str); free(str);
	str = itostr(-999);
		assert_strcmp("-999", str); free(str);
}
