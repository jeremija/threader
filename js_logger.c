#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "js_logger.h"

int verbose = 0;

void enable_verbose() {
  verbose = 1;
  LOG(DEBUG, "Debugging enabled!");
}

void die(const char *message) {
  if (errno) {
    perror(message);
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
  
  char line[LINE_SIZE];
  
  sprintf(line, "%s%s %s :: %s%s\n", color, current_time, log_type, message_format, NO_COLOR);
  
  va_list argptr;
  va_start(argptr, message_format);
  if (type != ERROR) {
    vfprintf(stdout, line, argptr);
  }
  else {
    vfprintf(stderr, line, argptr);
  }
  va_end(argptr);
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
