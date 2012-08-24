#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "js_logger.h"

const char* KEY_COMMAND = "command";
const char* KEY_THREADS = "threads";

const int LINE_SIZE = 512;

Config config;

void parse_key_value(char* key, char* value) {
  if (key == NULL || value == NULL) {
    return;
  }
  
  if (!strcmp(key, KEY_COMMAND)) {
    config.command = strdup(value);
    return;
  }
  
  if (!strcmp(key, KEY_THREADS)) {
    config.threads = atoi(value);
    return;
  }
}

void extract_key_value_pairs(char* line, char** key, char** value) {
  if (line == NULL) {
    return;
  }
 
  int len = strlen(line);
  if (len == 0) return;
  if (len == 1 && line[0] == '\n') return;
  
  
  if (line[len - 1] == '\n') {
    len--;
  }
  
  /*
   * The real work begins here!
   */
  
  int i;
  int valid = 0;
  /*
   * find the '=' sign
   */ 
  for (i = 0; i < len; i++) {
    if (line[i] == '=') {
      valid = 1;
      break;
    }
  }
  
  if (!valid) return;
  
  int key_size = i + 1;
  *key = malloc(sizeof(char)*(key_size));
  strncpy(*key, line, key_size - 1);
  *(*key + key_size - 1) = '\0';
  
  int value_start = i + 1;
  int value_size = len  - value_start + 1;
  *value = malloc(sizeof(char)*(value_size));
  strncpy(*value, line + value_start, value_size - 1);
  *(*value + value_size - 1) = '\0';
  
//  LOG(INFO, "key_size=%d, value_size=%d", key_size, value_size);
//  LOG(INFO, "key='%s', value='%s'", *key, *value);
}

Config read_conf_file() {
  char line[LINE_SIZE];
  int line_num = 0;
  
  FILE *config_file;
  
  /*
   * TODO make this path better
   */
  config_file = fopen("/home/jere/Desktop/config", "r");
  
  if (config_file == NULL) {
    die("Config file not found");
  }
  
  while ( fgets(line, LINE_SIZE, config_file) != NULL ) {
    char* key = NULL;
    char* value = NULL;
  
    line_num++;
    if (line[0] == '#') continue;
    
    extract_key_value_pairs(line, &key, &value);
    parse_key_value(key, value);
    
    free(key);
    free(value);
  }
  
  fclose(config_file);
  
  return config;
}
