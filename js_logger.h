#ifndef _JS_LOGGER
#define _JS_LOGGER

#define RED_BOLD  "\033[1;31m"        /* 0 -> normal ;  31 -> red */
#define RED       "\033[0;31m"        /* 0 -> normal ;  31 -> red */
#define CYAN      "\033[0;36m"        /* 1 -> bold ;  36 -> cyan */
#define GREEN     "\033[0;32m"        /* 4 -> underline ;  32 -> green */
#define BLUE      "\033[9;34m"        /* 9 -> strike ;  34 -> blue */
#define NO_COLOR  "\033[0m"           /* to flush the previous property */

#define ERROR                 1
#define INFO                  2
#define DEBUG                 3         

#define ERROR_STR     "ERROR"
#define INFO_STR      "INFO "
#define DEBUG_STR     "DEBUG"     
#define UNKNOWN_STR   "(***)"  

#define TIME_SIZE            19

void enable_verbose();

void LOG(int type, const char* message, ...);
void currentTime(char* current_time);
void die(const char* message);

#endif
