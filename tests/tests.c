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

#include "test_MemUtils.c"
#include "test_StringUtils.c"
#include "test_NumberUtils.c"
#include "test_FileUtils.c"
#include "test_tpl.c"
#include "test_run.c"



int main(int argc, char *argv[]) {
	testing_init(argc, argv);

	testing_add(test_MemUtils,    "MemUtils",    "test_MemUtils"   );
	testing_add(test_StringUtils, "StringUtils", "test_StringUtils");
	testing_add(test_NumberUtils, "NumberUtils", "test_NumberUtils");
	testing_add(test_FileUtils,   "FileUtils",   "test_FileUtils"  );
	testing_add(test_tpl,         "tpl",         "test_tpl"        );
	testing_add(test_run,         "run",         "test_run"        );

	testing_start();
	int result = testing_done();
	testing_free();
	return result;
}
