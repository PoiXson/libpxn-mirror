#pragma once

#include <stdlib.h>



// 10kb
#define RUN_CMD_RESULT_MAX 1024 * 10



typedef struct RunResult {
	size_t out_size;
	char *out;
	int exit_code;
} RunResult;



RunResult* run_cmd(char *cmd);
