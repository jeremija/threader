#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "string_allocation.h"

char* print_to_string(const char *format, ...) {
  va_list argptr;
  
  va_start(argptr, format);
  /*
   * check the number of characters 
   */
  int num_characters = vsnprintf((char *) NULL, 0, format, argptr);
  va_end(argptr);

  va_start(argptr,format);

  char* resulting_string;
  /*
   * allocate memory for string
   */
  resulting_string = malloc(sizeof(char) * (num_characters + 1));
  /*
   * print to memory
   */
  vsprintf(resulting_string, format, argptr);

  va_end(argptr);

  return resulting_string;
}
