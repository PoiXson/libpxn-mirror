#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <limits.h>



#define LOG_LEVEL_NAME_MAX 7
#define LOG_ARGS_LEN_MAX 200



typedef enum Log_Level {
	LVL_OFF     = INT_MIN,
	LVL_ALL     = INT_MAX,
	LVL_TITLE   = 9000,
	LVL_DETAIL  = 100,
	LVL_INFO    = 500,
	LVL_NOTICE  = 700,
	LVL_WARNING = 800,
	LVL_SEVERE  = 900,
	LVL_FATAL   = 1000
} Log_Level;



extern int count_warnings;
extern int count_severe;
extern int count_fatal;

int get_warning_count();
int get_severe_count();
int get_fatal_count();

bool has_warnings();
bool has_severe();
bool has_fatal();

void log_line   (char *msg, ...);
void log_title  (char *msg, ...);
void log_detail (char *msg, ...);
void log_info   (char *msg, ...);
void log_notice (char *msg, ...);
void log_warning(char *msg, ...);
void log_severe (char *msg, ...);
void log_fatal  (char *msg, ...);
void log_lines(void (*callback)(char *msg, ...), char *text);

void log_print(Log_Level level, char *msg);
void log_level_to_name(Log_Level level, char *name);
void log_write(char *msg);
