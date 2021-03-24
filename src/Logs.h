#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>



#define LOG_NAME_MAX 12
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



#define LOG_PRINTER_ARGS LogLevel, char*, char*

extern void (*log_printer)(LOG_PRINTER_ARGS);

extern LogLevel current_level;
extern char log_module[LOG_NAME_MAX];

extern size_t count_warnings;
extern size_t count_severe;
extern size_t count_fatal;

#define DEFAULT_CONSOLE_COLOR false
extern bool console_color_enabled;


void set_log_printer( void (*printer)(LOG_PRINTER_ARGS) );
void set_log_module(char *name);

void log_lines(void (*callback)(char *msg, ...), char *text);
void log_dash();

void log_line   (char *msg, ...);
void log_title  (char *msg, ...);
void log_detail (char *msg, ...);
void log_info   (char *msg, ...);
void log_notice (char *msg, ...);
void log_warning(char *msg, ...);
void log_severe (char *msg, ...);
void log_fatal  (char *msg, ...);

void log_print(LOG_PRINTER_ARGS);
void log_write(char *line);

bool is_level_loggable(const LogLevel level);
void log_level_set(const LogLevel level);
void log_level_to_name(const LogLevel level, char *name);

size_t get_warning_count();
size_t get_severe_count();
size_t get_fatal_count();

bool has_warnings();
bool has_severe();
bool has_fatal();

bool set_color_enabled(bool enabled);
bool has_color_enabled();

#define COLOR_BLACK       "\e[0;30m"
#define COLOR_BLUE        "\e[0;34m"
#define COLOR_GREEN       "\e[0;32m"
#define COLOR_CYAN        "\e[0;36m"
#define COLOR_RED         "\e[0;31m"
#define COLOR_PURPLE      "\e[0;35m"
#define COLOR_BROWN       "\e[0;33m"
#define COLOR_LIGHTGRAY   "\e[0;37m"
#define COLOR_DARKGRAY    "\e[1;30m"
#define COLOR_LIGHTBLUE   "\e[1;34m"
#define COLOR_LIGHTGREEN  "\e[1;32m"
#define COLOR_LIGHTCYAN   "\e[1;36m"
#define COLOR_LIGHTRED    "\e[1;31m"
#define COLOR_LIGHTPURPLE "\e[1;35m"
#define COLOR_YELLOW      "\e[1;33m"
#define COLOR_WHITE       "\e[1;37m"
#define COLOR_RESET       "\e[0m"
