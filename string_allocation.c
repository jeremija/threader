#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "string_allocation.h"
#include "js_logger.h"

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

  if (resulting_string == NULL) {
    die("memory error");
  }
  /*
   * print to memory
   */
  vsprintf(resulting_string, format, argptr);

  va_end(argptr);

  return resulting_string;
}

int find_position_of_string(const char *input, const char* search) {
  return find_position_of_string_from_position(input, search, 0);
}

int find_position_of_string_from_position(const char *input, const char* search, int start) {
  if (input == NULL || search == NULL) return -1;
  int input_length = strlen(input);
  int search_length = strlen(search);
  
//  LOG(INFO, "input_length=%d, search_length=%d", input_length, search_length);
  
  int i, j, found = -1;
  for (i = start; i < input_length; i++) {
    for (j = 0; j < search_length; j++) {
      printf("%c", input[i+j]);
      if (!(i + j < input_length)) {
//        printf("\n\n");
        break;
      }
      if (input[i + j] != search[j]) {
//        printf("\n\n'%c' != '%c'\n\n", input[i+j], search[j]);
        break;
      }
      if (j  == search_length - 1) {
        found = i;
      }
    }
    if (found > -1) {
      break;
    }
  }
  
  return found;
}

char* replace_first_occurrence_in_string(const char *input, const char* search, const char* replace) {
  if (input == NULL || search == NULL || replace == NULL) {
    return NULL;
  }

  int pos = find_position_of_string(input, search);
  if (pos < -1) {
    return strdup(input);
  }
  
  int input_length = strlen(input);
  int search_length = strlen(search);
  
  char* first = malloc(sizeof(char));
  *first = '\0';
  if (pos > 0) {
    int first_size = pos + 1;
    free(first);
    first = malloc(sizeof(char)*first_size);
    strncpy(first, input, first_size - 1);
    first[first_size - 1] = '\0';
  }
  
  int second_start = pos + search_length;
  int second_length = input_length - second_start + 1;
  char* second = malloc(sizeof(char) * second_length);
  strncpy(second, input + second_start, second_length);
  
  LOG(INFO, "first='%s', second='%s'", first, second);
  LOG(INFO, "first_size=%d, second_size=%d", strlen(first), strlen(second));
  
  char* replaced = print_to_string("%s%s%s", first, replace, second);
  free(first);
  free(second);
  
  return replaced;
}
