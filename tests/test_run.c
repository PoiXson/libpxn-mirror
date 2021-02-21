
#include "src/Testing.h"

#include "string.h"

#include "src/Run.h"
#include "src/Logs.h"



TEST_INIT(test_parse_cmd_args)
void test_parse_cmd_args() {
	size_t size = 10;
	char cmd[size];
	strcpy(cmd, "ls /etc");
	char *argv[CMD_ARGS_MAX];
	parse_cmd_args(cmd, argv);
		assert_strcmp("ls", cmd);
		assert_strcmp("ls",   argv[0]);
		assert_strcmp("/etc", argv[1]);
		assert       (argv[2] == NULL);
	strcpy(cmd, "w");
	parse_cmd_args(cmd, argv);
		assert_strcmp("w", cmd);
		assert_strcmp("w", argv[0]);
		assert       (argv[1] == NULL);
	strcpy(cmd, "echo 1 2 3");
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
	strlcpy(cmd, "sh tests/test_run.sh", PATH_MAX);
	save_level = current_level;
	current_level = LVL_WARNING;
	result = run_cmd_live(cmd);
	current_level = save_level;
		assert(result != NULL);
		assert_intcmp(42, result->status);
		assert_strcmp("Test works\nError works too\n", result->out);
	// test args
	strlcpy(cmd, "sh tests/test_run.sh abc 123", PATH_MAX);
	save_level = current_level;
	current_level = LVL_WARNING;
	result = run_cmd_live(cmd);
	current_level = save_level;
		assert(result != NULL);
		assert_intcmp(0, result->status);
		assert_strcmp("Arg: abc\nArg: 123\n", result->out);
}
