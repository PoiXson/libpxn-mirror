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



void test_MemUtils() {

	// test mem_set()
	{
		size_t tmp_size = 11;
		char *tmp = calloc(tmp_size, sizeof(char));
		mem_set(tmp, '-', tmp_size);
			assert(__LINE__, (tmp[tmp_size-1] == '-'));
		tmp[tmp_size-1] = '\0';
			assert_str_cmp(__LINE__, "----------", tmp);
		mem_set(tmp+2, 'x', 3);
			assert_str_cmp(__LINE__, "--xxx-----", tmp);
		free(tmp);
	}

	// test realloc_zero()
	{
		char *buf = calloc(4, sizeof(char));
		buf[0] = 'A';
		buf[1] = 'b';
		buf[2] = 'c';
		buf[3] = '\0';
		buf = realloc_zero(buf, 4, 6);
		assert_str_cmp(__LINE__, "Abc", buf);
		assert_int_cmp(__LINE__, 0, buf[3]);
		assert_int_cmp(__LINE__, 0, buf[4]);
		assert_int_cmp(__LINE__, 0, buf[5]);
		free(buf);
	}

}
