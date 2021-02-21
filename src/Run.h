#pragma once

#include <stdlib.h>



#define CMD_ARGS_MAX 64
// 10kb
#define RUN_CMD_RESULT_MAX 1024 * 10



typedef struct RunResult {
	size_t out_size;
	char *out;
	int status;
} RunResult;



RunResult* run_cmd(char *cmd);
RunResult* run_script(char *script);

void parse_cmd_args(char *cmd, char **argv);
RunResult* run_cmd_live(char *cmd);
