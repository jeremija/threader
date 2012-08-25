#ifndef _JS_LOGGER
#define _JS_LOGGER

#define ERROR                 1
#define WARN                  2
#define INFO                  3
#define DEBUG                 4         

#define ERROR_STR     "ERROR"
#define WARN_STR      "WARN "
#define INFO_STR      "INFO "
#define DEBUG_STR     "DEBUG"     
#define UNKNOWN_STR   "(***)"  

#define TIME_SIZE            19

/*
 * If called, will output the DEBUG log entries to console.
 */
void enable_verbose();

/*
 * If called, output to console will be colorless
 */
void disable_color_output();

/*
 * Logs to console in various colors.
 * Enabled types are ERROR, INFO and DEBUG
 */
void LOG(int type, const char* message, ...);

/*
 * Puts the current time in current_time. Length must be TIME_SIZE.
 */
void currentTime(char* current_time);

/*
 * Violently kills the program, but prints a message before killing.
 * If errno != 0, prints the meaning of errno.
 */
void die(const char* message);

#endif
