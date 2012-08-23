#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include "flac2mp3.h"
#include "js_logger.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int lastFile = -1;
int filesCount = 10;
char** files;
char** filenames;
char* output_dir;
int vbr_quality = 4;


void help(char* filename) {
  printf(
   "usage: %s [OPTION] INPUT_DIR OUTPUT_DIR\n\nOptions:\n    -h       help\n    -V [num] lame variable bitrate quality\n    -n [num] number of threads to use (default 2)\n    -m [num] number of maximum files to convert (default 30)\n    -v       verbose", filename);
  exit(0);
}

int main(int argc, char* argv[]) {
  int num_threads = 2;
  int max_results = 30;
  
  extern char *optarg;
  extern int optind, opterr;
  
  int argument;
  while( (argument = getopt(argc, argv, "hm:n:V:v")) != -1  ) {
    LOG(DEBUG, "argument=%c optarg='%s'", argument, optarg);
    switch(argument) {
      case 'h':
        help(argv[0]);
        break;
      case 'm':
        max_results = atoi(optarg);
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
  LOG(DEBUG, "current dir: %s\n", current_dir);
  LOG(DEBUG, "output dir: %s\n", output_dir);
  
  pthread_t threads[num_threads];
  
  LOG(INFO, "Starting %d threads", num_threads);
  
  char list_files_command[MAX_COMMAND_SIZE] = "\0";
  strcat(list_files_command, "ls \"");
  strcat(list_files_command, current_dir);
  strcat(list_files_command, "\" | grep -E \"(.flac$)|(.fla$)\"");
  
  FILE *fp = popen (list_files_command, "r");
 
  if (fp == NULL) {
    die("Failed to run search command");
  }

  /* Read the output a line at a time */
  int i;
  char line[PATH_MAX];
  char* itemList[max_results];
  char* itemNames[max_results];
  for(i = 0; i < max_results; i++) {
    if (  fgets(line, sizeof(line) - 1, fp) == NULL  ) {
      break;
    }
    
    char path_to_file[PATH_MAX] = "\0";
    strcat(path_to_file, current_dir);
    strcat(path_to_file, "/");
    strcat(path_to_file, line);
    
    /* allocate memory for string */
    itemList[i] = strndup(path_to_file, strlen(path_to_file) - 1);
    
    if (line[strlen(line-1)] == 'c') {
      /* if filename ends with .flac */
      itemNames[i] = strndup(line, strlen(line) - 6 - 1);
    } 
    else {
      /* else filename most probably ends with .fla */
      itemNames[i] = strndup(line, strlen(line) - 5 - 1);
    }
    

    if (itemList[i] == NULL) {
      die("Memory error");
    }
    
    //printf("%s", line);
  }
  filesCount = i;
  files = itemList;
  filenames = itemNames;

  if (filesCount <= 0) {
    printf("No results found\n");
    exit(0);
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
  char command[512];
  
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
  
  sprintf(command, "flac -scd \"%s\" | lame --quiet -h -V%d - \"%s/%s.mp3\"", 
      files[currentFile], vbr_quality, output_dir, filenames[currentFile]);
  
  
  LOG(INFO, "THREAD %d command: %s", thread, command);
  /* start conversion */
  FILE *file;
  file = popen(command, "r");
  
  /* wait for the command to complete */
  pclose(file);
  
  /* convert the file which is next in line if there is any */
//  LOG(DEBUG, "decodeNext() ended");
  decodeNext(thread_num);
  return NULL; 
}
