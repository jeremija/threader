#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include "flac2mp3.h"
#include "js_logger.h"
#include "string_allocation.h"
//#include "config.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int lastFile = -1;
int filesCount = 10;
char** files;
char** filenames;
char* output_dir;
int vbr_quality = 4;

const int INITIAL_SIZE = 5;

void help(char* filename) {
  printf(
   "usage: %s [OPTION] INPUT_DIR OUTPUT_DIR\n\nOptions:\n    -h       help\n    -V [num] lame variable bitrate quality\n    -n [num] number of threads to use (default 2)\n    -v       verbose", filename);
  exit(0);
}

int main(int argc, char* argv[], char* envp[]) {
//  const char* home_dir = getenv("HOME");
//  const char* test_env = getenv("TEST");
//  LOG(INFO, "HOME=%s, TEST=%s", home_dir, test_env);

//  Config config = read_conf_file();
//  LOG(INFO, "command=%s, threads=%d", config.command, config.threads);
  
//  free(config.command);

//  int pos = find_position_of_string("testiranje pronalaska", "aska");
//  LOG(INFO, "pos=%d", pos);
  char *string = replace_first_occurrence_in_string("testiranjereplacea", "a", "XXX");
  LOG(INFO, "string=%s", string);
  free(string);

  int num_threads = 2;
  //int max_results = 30;
  
  extern char *optarg;
  extern int optind, opterr;
  
  int argument;
  while( (argument = getopt(argc, argv, "hn:V:v")) != -1  ) {
    LOG(DEBUG, "argument=%c optarg='%s'", argument, optarg);
    switch(argument) {
      case 'h':
        help(argv[0]);
        break;
      case 'n':
        num_threads = atoi(optarg);
        break;
      case 'V':
        vbr_quality = atoi(optarg);
        break;
      case 'v':
        enable_verbose();
        break;
      default: /* '?' */
        //printf("argument %c\n", argument);
        die("Wrong arguments or argument requires an option which was not supplied");
    }
  }
  
  if(optind > argc - 2) {
    die("No input/output folders specified");
  }
  
  char *current_dir = argv[optind++];
  output_dir = argv[optind];
  /* current working directory */
  //char current_dir[PATH_MAX];
  //getcwd(current_dir, PATH_MAX);
  LOG(DEBUG, "current dir: %s", current_dir);
  LOG(DEBUG, "output dir: %s", output_dir);
  
  pthread_t threads[num_threads];
  
  LOG(INFO, "Starting %d threads", num_threads);
  
  char* list_files_command;
  list_files_command = print_to_string("ls \"%s\" | grep -E \"(.flac$)|(.fla$)\"", current_dir);
  FILE *fp = popen (list_files_command, "r");
  free(list_files_command);
 
  if (fp == NULL) {
    die("Failed to run search command");
  }

  /* Read the output a line at a time */
  int i;
  char line[PATH_MAX];

  int size = INITIAL_SIZE;
  char** itemList = malloc(sizeof(char*) * size); 
  char** itemNames = malloc(sizeof(char*) * size);
  if (itemList == NULL || itemNames == NULL) {
        die("Memory error!");
  }
  
  /*
   * not sure if this is really neccessary
   */ 
  for (i = 0; i < size; i++) {
    itemList[i] = NULL;
    itemNames[i] = NULL;
  }
  
  for(i = 0; 1; i++) {
    if (  fgets(line, sizeof(line) - 1, fp) == NULL  ) {
      break;
    }
    
    if (i >= size) {
      size = i + 1;
      itemList = realloc(itemList, sizeof(char*) * size);
      itemNames = realloc(itemNames, sizeof(char*) * size);
      if (itemList == NULL || itemNames == NULL) {
        die("Memory error!");
      }
    }
    
    
    char* path_to_file;
    char* line_without_newline = strndup(line, strlen(line) - 1);
    path_to_file = print_to_string("%s%s", current_dir, line_without_newline);
    
    itemList[i] = path_to_file;
    
    LOG(DEBUG, "input_filename_%d='%s'", i, line_without_newline);
    if (line_without_newline[strlen(line_without_newline) - 1] == 'c') {
      /* if filename ends with .flac */
      itemNames[i] = strndup(line_without_newline, strlen(line_without_newline) - 6 - 1);
    } 
    else {
      /* else filename most probably ends with .fla */
      itemNames[i] = strndup(line_without_newline, strlen(line_without_newline) - 5 - 1);
    }
    
    free(line_without_newline);

    if (itemList[i] == NULL) {
      die("Memory error");
    }
    
    //printf("%s", line);
  }
  
  pclose(fp);
  
  filesCount = i;
  files = itemList;
  filenames = itemNames;

  if (filesCount <= 0) {
    die("No files found");
  }
  
  /* print the numbered results */
//  for(i = 0; i < filesCount; i++) {
//    printf("%d. %s\n", i, files[i]);
//  }
   
  int* thread_indexes[num_threads];
  for (i = 0; i < num_threads; i++) {
    thread_indexes[i] = malloc(sizeof(int));
    *thread_indexes[i] = i;
    pthread_create(&threads[i], NULL, &decodeNext, (void *) thread_indexes[i]);
  }
  
  for (i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
    free(thread_indexes[i]);
  }
  
  for (i = 0; i < size; i++) {
    if (itemList[i] != NULL) {
      free(itemList[i]);
    }
    if (itemNames[i] != NULL) {
      free(itemNames[i]);
    }
  }
  
  free(itemList);
  free(itemNames);
  
  LOG(DEBUG, "Program ended");

  return 0;
}

/**
 * This method will be called from new thread
 */
void *decodeNext(void * thread_num) {
  int thread = *((int *) thread_num);
//  printf("TREAD %d", thread);
//  LOG(DEBUG, "decodeNext() called");

  int currentFile;
  
  /* this is supposed to be something like synchronized in java */
  pthread_mutex_lock(&mutex);
  currentFile = ++lastFile;
  pthread_mutex_unlock(&mutex);
  
  
  /* check if index is out of bounds */
  if (currentFile > filesCount - 1) {
    /* if it is, do nothing */
    LOG(DEBUG, "THREAD %d about to finish!", thread);
    return NULL;
  }
  
  char* command;
  command = print_to_string("flac -scd \"%s\" | lame --quiet -h -V%d - \"%s%s.mp3\"", 
      files[currentFile], vbr_quality, output_dir, filenames[currentFile]);
  
  
  LOG(INFO, "THREAD %d command: %s", thread, command);
  /* start conversion */
//  FILE *file;
//  file = popen(command, "r");
  
  /* wait for the command to complete */
//  pclose(file);
  free(command);
  
  /* convert the file which is next in line if there is any */
//  LOG(DEBUG, "decodeNext() ended");
  decodeNext(thread_num);
  return NULL; 
}
