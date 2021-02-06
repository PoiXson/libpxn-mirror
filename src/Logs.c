#include "logs.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "StringUtils.h"



void (*log_printer)(LogLevel, char*) = NULL;

LogLevel current_level = LVL_INFO;

int count_warnings = 0;
int count_severe   = 0;
int count_fatal    = 0;



#define LOG_PRINT_PRE \
	va_list args; \
	va_start(args, msg); \
	size_t buf_size = strlen(msg) + LOG_ARGS_LEN_MAX; \
	char buf[buf_size+1]; \
	vsnprintf(buf, buf_size, msg, args);

#define LOG_PRINT_POST \
	va_end(args);



void log_lines(void (*callback)(char *msg, ...), char *text) {
	if (text == NULL) {
		(*callback) ("<null>");
		return;
	}
	size_t len = strlen(text) + 1;
	char *txt = malloc(len * sizeof(char));
	strlcpy(txt, text, len);
	char *ptr = txt;
	size_t pos;
	while (true) {
		pos = chrpos(ptr, '\n');
		if (pos == -1) {
			(*callback) (ptr);
			break;
		}
		ptr[pos] = '\0';
		(*callback) (ptr);
		ptr += pos;
		ptr++;
	}
	free(txt);
}

void log_line(char *msg, ...) {
LOG_PRINT_PRE
	log_printer(LVL_OFF, buf);
LOG_PRINT_POST
}



void log_detail(char *msg, ...) {
LOG_PRINT_PRE
	log_printer(LVL_DETAIL, buf);
LOG_PRINT_POST
}

void log_info(char *msg, ...) {
LOG_PRINT_PRE
	log_printer(LVL_INFO, buf);
LOG_PRINT_POST
}

void log_notice(char *msg, ...) {
LOG_PRINT_PRE
	log_printer(LVL_NOTICE, buf);
LOG_PRINT_POST
}

void log_warning(char *msg, ...) {
LOG_PRINT_PRE
	count_warnings++;
	log_printer(LVL_WARNING, buf);
LOG_PRINT_POST
}

void log_severe(char *msg, ...) {
LOG_PRINT_PRE
	count_severe++;
	log_printer(LVL_SEVERE, buf);
LOG_PRINT_POST
}

void log_fatal(char *msg, ...) {
LOG_PRINT_PRE
	count_fatal++;
	log_printer(LVL_FATAL, buf);
LOG_PRINT_POST
}



void log_print(LogLevel level, char *msg) {
	// title
	if (level == LVL_TITLE) {
		size_t len = strlen(msg) + 6;
		char bufA[len+1];
		char bufB[len+1];
		for (size_t i=0; i<len-1; i++) {
			if (i < 1) {
				bufA[i] = ' ';
			} else {
				bufA[i] = '*';
			}
		}
		bufA[len-1] = ' ';
		bufA[len]   = '\0';
		bufB[0] = '\0';
		strcpy(bufB, " * ");
		strcat(bufB, msg);
		strcat(bufB, " * ");
		log_write(bufA);
		log_write(bufB);
		log_write(bufA);
		return;
	}
	if (level >= current_level) {
		// level name
		char lvlName[LOG_LEVEL_NAME_MAX+1];
		log_level_to_name(level, lvlName);
		// plain line
		if (strlen(lvlName) == 0) {
			log_write(msg);
			return;
		}
		// message with level
		for (char *c=lvlName; (*c=toupper(*c)); ++c);
		pad_center(lvlName, LOG_LEVEL_NAME_MAX);
		size_t msg_len = strlen(msg);
		size_t buf_size = msg_len + LOG_LEVEL_NAME_MAX+5;
		char buf[buf_size+1];
		snprintf(buf, buf_size, " [%s] %s", lvlName, msg);
		if (msg_len-1 == LOG_ARGS_LEN_MAX) {
			buf[buf_size-4] = '.';
			buf[buf_size-3] = '.';
			buf[buf_size-2] = '.';
		}
		buf[buf_size-1] = '\0';
		log_write(buf);
	}
}

void log_write(char *line) {
	printf(line);
	if (line[strlen(line)-1] != '\n') {
		printf("\n");
	}
//TODO: log to file
}



void log_level_set(LogLevel lvl) {
	current_level = lvl;
}

void log_level_to_name(LogLevel level, char *name) {
	switch (level) {
	case LVL_OFF:     break;
	case LVL_ALL:     break;
	case LVL_TITLE:   break;
	case LVL_DETAIL:  strcpy(name, "Detail");  return;
	case LVL_INFO:    strcpy(name, "Info");    return;
	case LVL_NOTICE:  strcpy(name, "Notice");  return;
	case LVL_WARNING: strcpy(name, "Warning"); return;
	case LVL_SEVERE:  strcpy(name, "Severe");  return;
	case LVL_FATAL:   strcpy(name, "Fatal");   return;
	default: break;
	}
	name[0] = '\0';
}



int get_warning_count() {
	int count = count_warnings;
	count_warnings = 0;
	return count;
}

int get_severe_count() {
	int count = count_severe;
	count_severe = 0;
	return count;
}

int get_fatal_count() {
	int count = count_fatal;
	count_fatal = 0;
	return count;
}



bool has_warnings() {
	return (count_warnings > 0);
}

bool has_severe() {
	return (count_severe > 0);
}

bool has_fatal() {
	return (count_fatal > 0);
}