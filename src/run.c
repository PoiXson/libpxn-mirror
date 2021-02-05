#include "run.h"

#include <stdio.h>
#include <string.h>

#include "StringUtils.h"



Run_Result* run_cmd(char *cmd) {
	FILE *handle = popen(cmd, "r");
	if (handle == NULL) {
//		log_severe("Failed to execute: %s", cmd);
		return NULL;
	}
	Run_Result *result = calloc(1, sizeof(Run_Result));
	size_t chunk_len;
	size_t chunk_size = 2;
	char chunk[chunk_size];
	result->out_size = chunk_size;
	result->out = calloc(result->out_size, sizeof(char));
	size_t len = 0;
	while (fgets(chunk, chunk_size, handle) != NULL) {
		chunk_len = strlen(chunk);
		if (len + chunk_len > result->out_size - 2) {
			while (len + chunk_len > result->out_size - 2) {
				result->out_size *= 2;
			}
			if (result->out_size > RUN_CMD_RESULT_MAX)
				result->out_size = RUN_CMD_RESULT_MAX;
			result->out = realloc(result->out, result->out_size);
		}
		strlcat(result->out, chunk, result->out_size);
		len = strlen(result->out);
		if (result->out_size == RUN_CMD_RESULT_MAX)
			break;
	}
	result->exit_code = pclose(handle) / 256;
	return result;
}
