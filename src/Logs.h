#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>



#define LOG_LEVEL_NAME_MAX 7
#define LOG_ARGS_LEN_MAX 200



typedef enum LogLevel {
	LVL_OFF     = INT_MAX,
	LVL_ALL     = INT_MIN,
	LVL_TITLE   = 9000,
	LVL_DETAIL  = 100,
	LVL_INFO    = 500,
	LVL_NOTICE  = 700,
	LVL_WARNING = 800,
	LVL_SEVERE  = 900,
	LVL_FATAL   = 1000
} LogLevel;



#define LOG_PRINTER_ARGS LogLevel, char*

extern void (*log_printer)(LOG_PRINTER_ARGS);

extern LogLevel current_level;

extern int count_warnings;
extern int count_severe;
extern int count_fatal;



void set_log_printer( void (*printer)(LOG_PRINTER_ARGS) );

void log_lines(void (*callback)(char *msg, ...), char *text);
void log_line   (char *msg, ...);

void log_detail (char *msg, ...);
void log_info   (char *msg, ...);
void log_notice (char *msg, ...);
void log_warning(char *msg, ...);
void log_severe (char *msg, ...);
void log_fatal  (char *msg, ...);

void log_print(LOG_PRINTER_ARGS);
void log_write(char *line);

void log_level_set(LogLevel lvl);
void log_level_to_name(LogLevel level, char *name);

int get_warning_count();
int get_severe_count();
int get_fatal_count();

bool has_warnings();
bool has_severe();
bool has_fatal();
