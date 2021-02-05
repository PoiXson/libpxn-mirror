#pragma once

#include <stdlib.h>



// 10kb
#define RUN_CMD_RESULT_MAX 1024 * 10



typedef struct Run_Result {
	size_t out_size;
	char *out;
	int exit_code;
} Run_Result;



Run_Result* run_cmd(char *cmd);
