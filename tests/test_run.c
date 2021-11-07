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

#include "src/Run.h"



void test_run() {
//TODO
}
/*
TEST_INIT(test_parse_cmd_args)
void test_parse_cmd_args() {
	size_t size = 11;
	char cmd[size];
	strlcpy(cmd, "ls /etc", size);
	char *argv[CMD_ARGS_MAX];
	parse_cmd_args(cmd, argv);
		assert_strcmp("ls",   cmd);
		assert_strcmp("ls",   argv[0]);
		assert_strcmp("/etc", argv[1]);
		assert       (argv[2] == NULL);
	strlcpy(cmd, "w", size);
	parse_cmd_args(cmd, argv);
		assert_strcmp("w", cmd);
		assert_strcmp("w", argv[0]);
		assert       (argv[1] == NULL);
	strlcpy(cmd, "echo 1 2 3", size);
	parse_cmd_args(cmd, argv);
		assert_strcmp("echo", cmd);
		assert_strcmp("echo", argv[0]);
		assert_strcmp("1",    argv[1]);
		assert_strcmp("2",    argv[2]);
		assert_strcmp("3",    argv[3]);
		assert       (argv[4] == NULL);
}



TEST_INIT(test_run)
void test_run() {
	// check script exists
	{
		File_Type file_type = get_file_type("tests/test_run.sh");
		assert(file_type == IS_FILE);
		if (file_type != IS_FILE) {
			printf("\nScript not found: test_run.sh\n");
			return;
		}
	}
	char cmd[PATH_MAX];
	RunResult *result;
	LogLevel save_level;
	// test stdout and stderr
	strlcpy(cmd, "tests/test_run.sh", PATH_MAX);
	save_level = current_level;
	current_level = LVL_WARNING;
	result = run_cmd_live(cmd);
	current_level = save_level;
		assert(result != NULL);
		assert_intcmp(42, result->status);
		assert_strcmp("Test works\nError works too\n", result->out);
	// test args
	strlcpy(cmd, "tests/test_run.sh abc 123", PATH_MAX);
	save_level = current_level;
	current_level = LVL_WARNING;
	result = run_cmd_live(cmd);
	current_level = save_level;
		assert(result != NULL);
		assert_intcmp(0, result->status);
		assert_strcmp("Arg: abc\nArg: 123\n", result->out);
}
*/
