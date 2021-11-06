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

#include "test_StringUtils.c"
#include "test_NumberUtils.c"
#include "test_FileUtils.c"
#include "test_tpl.c"
#include "test_run.c"

#include <stdio.h>



TEST_MAIN("libpxn-test")



void tests() {
	RUN_TEST(test_StringUtils,    "StringUtils");
	RUN_TEST(test_NumberUtils,    "NumberUtils");
	RUN_TEST(test_FileUtils,      "FileUtils");
	RUN_TEST(test_tpl,            "tpl");
	RUN_TEST(test_parse_cmd_args, "test_parse_cmd_args");
	RUN_TEST(test_run,            "run");
}
