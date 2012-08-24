#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "js_logger.h"
#include "string_allocation.h"

#define RED_BOLD  "\033[1;31m"        /* 1 -> bold ;  31 -> red */
#define RED       "\033[0;31m"        /* 0 -> normal ;  31 -> red */
#define CYAN      "\033[0;36m"        /* 0 -> normal ;  36 -> cyan */
#define GREEN     "\033[0;32m"        /* 4 -> underline ;  32 -> green */
#define BLUE      "\033[9;34m"        /* 9 -> strike ;  34 -> blue */
#define NO_COLOR  "\033[0m"           /* to flush the previous property */

int verbose = 0;

void enable_verbose() {
  verbose = 1;
  LOG(DEBUG, "Verbose enabled!");
}


void die(const char *message) {
  if (errno) {
//    perror(message);
    char *error_message = strerror(errno);
    char* whole_message = print_to_string("%s: %s", message, error_message);
    LOG(ERROR, whole_message);
  }
  else {
    LOG(ERROR, message);
  }
  
  exit(1);
}

void LOG(int type, const char* message_format, ...) {
  char* log_type;
  char* color;
  switch(type) {
    case ERROR:
      log_type = ERROR_STR;
      color = RED_BOLD;
      break;
    case INFO:
      log_type = INFO_STR;
      color = CYAN;
      break;
    case DEBUG:
      if (verbose == 0) {
        /* ignore debug logging of not verbose */
        return;
      }
      log_type = DEBUG_STR;
      color = GREEN;
      break;
    default:
      log_type = UNKNOWN_STR;
      color = BLUE;
      break;
  }

  char current_time[TIME_SIZE];
  currentTime(current_time);
  
  char* line = print_to_string("%s%s %s :: %s%s\n", color, current_time, log_type, message_format, NO_COLOR);
  
  va_list argptr;
  va_start(argptr, message_format);
  if (type != ERROR) {
    vfprintf(stdout, line, argptr);
  }
  else {
    vfprintf(stderr, line, argptr);
  }
  va_end(argptr);
  
  free(line);
}

void currentTime(char* current_time) {
  time_t now;
  struct tm *tm;

  now = time(0);
  if ((tm = localtime (&now)) == NULL) {
    current_time=" (time_error) ";
    return;
  }

  sprintf(current_time, "%04d-%02d-%02d %02d:%02d:%02d",
      tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
      tm->tm_hour, tm->tm_min, tm->tm_sec);
}
