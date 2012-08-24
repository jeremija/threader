#ifndef _CONFIG
#define _CONFIG

typedef struct {
  char* command;
  int threads;
} Config;

Config read_conf_file();

#endif
