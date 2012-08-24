#ifndef _CONFIG
#define _CONFIG

typedef struct {
  char* command;
  char* old_ext;
  char* new_ext;
  int threads;

  int run_script_on_finish;
  char* script_path;
} Config;

/*
 * reads from the config file at location. The key/value pairs are 
 * parsed and the values in Config struct are changed. The struct is 
 * then returned.
 */
Config read_conf_file(char* location);

/*
 * sets the Config.threads to threads. This is used so that it 
 * lowers/increases the threads number if it was too low.
 */
void set_config_threads(Config* conf, int threads);

/*
 * free the memory if char* in Config
 */ 
void free_config(Config config);

#endif
