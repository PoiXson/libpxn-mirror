#include "Run.h"

#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "StringUtils.h"
#include "Logs.h"



RunResult* run_cmd(char *cmd) {
	log_detail("Running cmd: %s", cmd);
	FILE *handle = popen(cmd, "r");
	if (handle == NULL) {
		log_severe("Failed to execute: %s", cmd);
		return NULL;
	}
	RunResult *result = calloc(1, sizeof(RunResult));
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
	if (result->exit_code == 1
	|| is_level_loggable(LVL_DETAIL)) {
		log_line("out:");
		log_lines(log_line, result->out);
	}
	return result;
}

RunResult* run_script(char *script) {
	char cmd[PATH_MAX];
	snprintf(cmd, PATH_MAX, "sh %s", script);
	return run_cmd(cmd);
}



void parse_cmd_args(char *cmd, char **argv) {
	while (*cmd != '\0') {
		while (*cmd == ' ' || *cmd == '\t' || *cmd == '\n')
			*cmd++ = '\0';
		*argv++ = cmd;
		while (true) {
			if (*cmd == '\0' || *cmd == ' '
			||  *cmd == '\t' || *cmd == '\n')
				break;
			cmd++;
		}
	}
	*argv = '\0';
}



RunResult* run_cmd_live(char *cmd) {
	log_detail("Running cmd: %s", cmd);
	char *argv[64];
	parse_cmd_args(cmd, argv);
	// capture stdout
	int handle_out[2];
	pipe(handle_out);
	const pid_t pid = fork();
	if (pid < 0) {
		log_fatal("Failed to fork a new thread");
		return NULL;
	}
	// child thread
	if (pid == 0) {
		dup2(handle_out[1], 1); // replace stdout
		dup2(handle_out[1], 2); // replace stderr
		int handle_in = open("/dev/null", O_WRONLY);
		dup2(handle_in,     0); // replace stdin
		close(handle_in);
		close(handle_out[0]);
		close(handle_out[1]);
		// execute
		if (execvp(*argv, argv) < 0) {
			printf("ERROR: exec failed: %i\n", errno);
		}
		exit(1);
	}
	// parent thread
	close(handle_out[1]);
	RunResult *result = calloc(1, sizeof(RunResult));
	bool log_detail = is_level_loggable(LVL_DETAIL);
	size_t buf_size = 64;
	char buffer[buf_size];
	size_t last_pos = 0;
	while (true) {
		ssize_t len = read(handle_out[0], buffer, buf_size);
		if (len <= 0) {
			if (errno == EAGAIN) continue;
			if (errno == EINTR)  continue;
			break;
		}
		strlrcat(&(result->out), &(result->out_size), buffer, len);
		// live log
		if (log_detail) {
			while (true) {
				size_t pos = chrposs(result->out, '\n', last_pos);
				if (pos == -1)
					break;
				size_t line_len = (pos - last_pos) + 1;
				char line[line_len];
				strlcpy(line, result->out + last_pos, line_len);
				log_line(" >%s", line);
				last_pos = pos + 1;
			}
		} // end live log
	}
	close(handle_out[0]);
	// finish live log
	if (log_detail) {
		if (result->out_size - 1 > last_pos) {
			size_t line_len = result->out_size - last_pos;
			char line[line_len];
			strlcpy(line, result->out + last_pos, line_len);
			log_line(" >%s", line);
		}
	} // end live log
	// wait for death of child
	int status;
	while (true) {
		int r = waitpid(pid, &status, 0);
		if (r != -1)        break;
		if (errno != EINTR) break;
	}
	status /= 256;
	if (log_detail) {
		log_line("cmd exit: %i", status);
	}
	result->status = status;
	return result;
}
