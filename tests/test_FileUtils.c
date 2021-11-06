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

#include "src/FileUtils.h"

#include <stdlib.h>
#include <string.h>



TEST_INIT(test_FileUtils)
void test_FileUtils() {

	// test get_file_type()
	assert_intcmp(IS_DIR,  get_file_type("/etc"));
	assert_intcmp(IS_FILE, get_file_type("/etc/redhat-release"));
	assert_intcmp(NOT_FOUND, get_file_type("/abcdefg12345gfedcba"));

	// test basename()
//TODO

	// test build_path()
	{
		char *path = calloc(PATH_MAX, sizeof(char));
		build_path(path, 2, "/etc/", "///redhat-release");
			assert_strcmp("/etc/redhat-release", path);
		build_path(path, 3, "/var/log/", "cups", "error_log");
			assert_strcmp("/var/log/cups/error_log", path);
		build_path(path, 2, "data", "file.txt");
			assert_strcmp("data/file.txt", path);
		free(path);
	}

	// test get_lock() and free_lock()
//TODO: maybe use pwd

	// test load_text_file() and read_text_file()
	{
		char *out;
		int len = load_text_file("/etc/redhat-release", &out);
		assert(len > 0);
		assert(strlen(out) > 0);
	}

	// test save_text_file()
//TODO

	// test copy_file()
//TODO

	// test file_compare()
	{
		char *data;
		size_t data_size = 0;
		for (size_t i=0; i<5; i++) {
			for (size_t j=0; j<10; j++) {
				strlrcat(&data, &data_size, "abcdefghijklmnopqrstuvwxyz\n", 27);
			}
			strlrcat(&data, &data_size, "\n", 2);
		}
		char *file = "tests/test.data";
		// test matching
		assert_intcmp(0, file_compare(file, data, data_size));
		// test differ
		char c = data[1350];
		data[1350] = '2';
		assert_intcmp(1350, file_compare(file, data, data_size));
		// test matching
		data[1350] = c;
		assert_intcmp(0, file_compare(file, data, data_size));
		// test matching extra lines
		strlrcat(&data, &data_size, "\n\n\n", 4);
		assert_intcmp(0, file_compare(file, data, data_size));
		// test matching
		str_trim(data);
		strlrcat(&data, &data_size, "\n", 2);
		assert_intcmp(0, file_compare(file, data, data_size));
		// test differ longer
		strlrcat(&data, &data_size, "abc\n", 5);
		assert_intcmp(1356, file_compare(file, data, data_size));
		// test differ shorter
		data[20] = '\0';
		assert_intcmp(20, file_compare(file, data, data_size));
	}
}
