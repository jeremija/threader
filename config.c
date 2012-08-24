#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "js_logger.h"

const char* KEY_COMMAND = "command";
const char* KEY_THREADS = "threads";
const char* KEY_OLD_EXT = "old_ext";
const char* KEY_NEW_EXT = "new_ext";
const char* KEY_RUN_SCRIPT_ON_FINISH = "run_script_on_finish";

const int LINE_SIZE = 512;
const int THREADS_MAX = 4;
const int THREADS_DEFAULT = 2;

Config config;

void set_config_threads(Config* conf, int threads) {
  if (threads <= 0) {
    LOG(INFO, "Illegal number of threads %d. Setting number of threads to %d.", 
        threads, THREADS_DEFAULT);
    conf->threads = THREADS_DEFAULT;
  }
  else if (threads > THREADS_MAX) {
    LOG(INFO, "Number of threads (%d) exceeds THREADS_MAX (%d). Setting number of threads to %d.", 
         threads, THREADS_MAX, THREADS_MAX);
    conf->threads = THREADS_MAX;
  }
  else {
    conf->threads = threads;
  }
}

void parse_key_value(char* key, char* value) {
  if (key == NULL || value == NULL) {
    return;
  }
  
  if (!strcmp(key, KEY_COMMAND)) {
    config.command = strdup(value);
    return;
  }
  
  if (!strcmp(key, KEY_THREADS)) {
    set_config_threads(&config, atoi(value));
    return;
  }
  
  if (!strcmp(key, KEY_OLD_EXT)) {
    config.old_ext = strdup(value);
    return;
  }
  
  if (!strcmp(key, KEY_NEW_EXT)) {
    config.new_ext = strdup(value);
    return;
  }
  
  if (!strcmp(key, KEY_RUN_SCRIPT_ON_FINISH)) {
    config.run_script_on_finish = 1;
    config.script_path = strdup(value);
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
  
  /*
   * +1 is for '\0' char which is added later
   */
  int key_size = i + 1;
  *key = malloc(sizeof(char)*(key_size));
  if (key == NULL) die("Memory error!");
  /*
   * key is everything before the first '=' character
   */
  strncpy(*key, line, key_size - 1);
  /*
   * add the '\0' to the end of the string
   */
  *(*key + key_size - 1) = '\0';
  
  int value_start = i + 1;
  int value_size = len  - value_start + 1;
  *value = malloc(sizeof(char)*(value_size));
  if (value == NULL) die("Memory error!");
  /*
   * value is everything after the first '=' character
   */
  strncpy(*value, line + value_start, value_size - 1);
  /*
   * add the '\0' to the end of the string.
   */
  *(*value + value_size - 1) = '\0';
  
//  LOG(INFO, "key_size=%d, value_size=%d", key_size, value_size);
//  LOG(INFO, "key='%s', value='%s'", *key, *value);
}

Config read_conf_file(char* location) {
  char line[LINE_SIZE];
  
  FILE *config_file;
  
  config_file = fopen(location, "r");
  if (config_file == NULL) die("Config file not found");
  
  /*
   * read one line at a time.
   */
  int line_num = 0;
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
  
  LOG(INFO, "configuration file read: command=%s, threads=%d, old_ext=%s, new_ext=%s",
      config.command, config.threads, config.old_ext, config.new_ext);
  
  return config;
}

void free_config(Config config) {
  free(config.command);
  free(config.old_ext);
  free(config.new_ext);
  free(config.script_path);
}
