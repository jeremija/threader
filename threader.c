#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* getopt */
#include <pthread.h> /* threading */
#include <string.h>
#include <limits.h>
#include <errno.h>

#include "threader.h"
#include "js_logger.h"
#include "string_allocation.h"
#include "config.h"

void *do_work();

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; /* for synchronized reading */
int last_file = -1;         /* global variable, file conversion counter */
int files_count = 0;        /* global variable, total files count */
int size = 0;
char* output_dir;           /* outp ut directory */
char** input_files;         /* array of input files (whole path to file) */
char** output_filenames;    /* array of output files (whole path to file) */
char* quality = "4";        /* default quality to use in conversion command */
Config config;              /* configuration struct, the config file is read to this struct */

int dry_run = 0;
int use_custom_config_file = 0;
char* custom_config_file_location;
/* config overrides */
int override_config_threads_num = 0;

/* initial size of files array */
const int INITIAL_SIZE = 5;
const char PATH_SEPARATOR = '/';

/* string %INPUT_FILES% in config.command will be replaced with input_files[i] */
const char* INPUT_FILE = "%INPUT_FILE%";      
/* string %OUTPUT_FILES% in config.command will be replaced with output_filenames[i] */
const char* OUTPUT_FILE = "%OUTPUT_FILE%";
/* string %QUALITY%  in config.command will be replaced with quality */
const char* QUALITY = "%QUALITY%";

/*
 * print the help and exit (for -h flag)
 */
void help() {
  printf(
      "usage: %s [OPTIONS] INPUT_DIR OUTPUT_DIR\n\n"
      "Options:\n"
      "    -c [path] use custom config file\n"
      "    -d        dry run (skips the command call from config file, but will call\n"
      "               script if set\n"
      "    -h        help\n"
      "    -n [num]  number of threads to use (default 2)\n"
      "    -i        if this option is used, the user can use the unlimited number of\n"
      "              full paths to the filenames to use (instead of the INPUT_DIR, but\n"
      "              the last parameter must be the OUTPUT_DIR.\n"
      "    -u        disables color output to console (should be first in line)\n"
      "    -v        verbose (should be first in line (after -u), or something may not\n"
      "              be outputted)\n"
      "    -V [num]  quality (for example VBR quality for lame encoder)\n", APP_NAME);
  exit(0);
}

/*
 * read fro mconfig file and set it up
 */
void config_file_actions(const char* home_dir) {
  /*
   * path of config file, default: /home/$USER/.threader/config
   */
  char* config_location;
  if (use_custom_config_file) {
    config_location = print_to_string("%s", custom_config_file_location);
  } else {
    config_location = print_to_string("%s%s%s%s", home_dir, "/.", APP_NAME, "/config");
  }
  LOG(INFO, "config_location=%s", config_location);
  /*
   * read config file
   */
  config = read_conf_file(config_location);
  free(config_location);
  LOG(INFO, "command='%s', threads=%d, old_ext='%s', new_ext='%s', run_script_on_finish=%d, "
      "script_path=%s",
      config.command, config.threads, config.old_ext, config.new_ext, config.run_script_on_finish, config.script_path);
  
  /* CONFIG FILE OVERRIDES */
  
  /*
   * override thread number set in config file if -n option was passed
   */
  if (override_config_threads_num != 0) {
    LOG(INFO, "Overriding thread number from config file from %d to %d (-n option was passed)", 
        config.threads, override_config_threads_num);
    set_config_threads(&config, override_config_threads_num);
  }
}

void reallocate_memory_for_next_filename() {
  if (files_count >= size) {
    size = files_count + 1;
    input_files = realloc(input_files, sizeof(char*) * size);
    output_filenames = realloc(output_filenames, sizeof(char*) * size);
    if (input_files == NULL || output_filenames == NULL) {
      die("Memory error!");
    }
  }
}

void folder_input(int argc, char* argv[], int optind) {
  /*
   * must have two lone arguments (input and output folder directory)
   */
  if(optind > argc - 2) {
    die("No input/output folders specified");
  }
  else if(optind < argc - 2) {
    die("Too many arguments specified. Maybe you wanted to use the -i option? ");
  }

  /* first argument */
  char *source_dir = argv[optind++];
  /* second argument */
  output_dir = argv[optind];
  /* current working directory */
  //getcwd(source_dir, PATH_MAX);
  LOG(DEBUG, "current dir: %s", source_dir);
  LOG(DEBUG, "output dir: %s", output_dir);
  
  if (source_dir[strlen(source_dir) - 1] != PATH_SEPARATOR) {
    die("Input dir should end with path separator!"); 
  }
  if (output_dir[strlen(output_dir) - 1] != PATH_SEPARATOR) {
    die("Output dir should end with path separator!"); 
  }
  

  char* list_files_command;
  /*
   * command to list files in source_dir
   */
  list_files_command = print_to_string("ls \"%s\" | grep -E \"(%s$)\"", source_dir, config.old_ext);
  FILE *fp = popen (list_files_command, "r");
  free(list_files_command);
 
  if (fp == NULL) {
    die("Failed to run search command");
  }

  /* Read the output a line at a time */
  char line[PATH_MAX];
  
  files_count = 0;
  /*
   * read the output of list_files_command, one line at a time
   */
  while( fgets(line, sizeof(line) - 1, fp) != NULL ) {
    
    /*
     * reallocate memory for next filename if neccessary
     */ 
    reallocate_memory_for_next_filename();
    
    
    char* path_to_file;
    char* line_without_newline = strndup(line, strlen(line) - 1);
    /*
     * concatenate directory and input filename  
     */
    path_to_file = print_to_string("%s%s", source_dir, line_without_newline);
    
    input_files[files_count] = path_to_file;
    
    /*
     * original filename without extension
     */
    char* itemName = 
        strndup(line_without_newline, strlen(line_without_newline) -  strlen(config.old_ext));
    
    /*
     * concatenate output filename, original filename without extension and append the new extension 
     */
    output_filenames[files_count] = print_to_string("%s%s%s", output_dir, itemName, config.new_ext);
    
    /* free unneccessary pointers */
    free(itemName);
    free(line_without_newline);

    files_count++;
  }
  
  pclose(fp);
  
  if (files_count <= 0) {
    die("No files found");
  }
}

void filenames_input(int argc, char* argv[], int optind) {
  /*
   * must have two or more lone arguments. First n-1 is input filename, the last is the output folder
   */
  if(optind > argc - 2) {
    die("No input/output folders specified");
  }
  
  files_count = 0;
  /*
   * number of lone arguments
   */
  char* output_dir = argv[argc - 1];
  if (output_dir[strlen(output_dir) - 1] != PATH_SEPARATOR) {
    die("Output dir should end with path separator!"); 
  }
  files_count = 0;
  int i;
  for(i = optind; i < argc - 1; i++) {
    /*
     * reallocate memory for next filename if neccessary
     */ 
    reallocate_memory_for_next_filename();
    
    char* input_file = strdup(argv[i]);
    input_files[files_count] = input_file;
    
    char* output_filename = extract_filename_from_path_no_ext(input_file, config.old_ext);
    output_filenames[files_count] = 
        print_to_string("%s%s%s", output_dir, output_filename, config.new_ext);
    free(output_filename);
    
    files_count++;
  }
}


/*
 * The program begins here. 
 *  
 * Arguments: 
 *    argc - count of arguments
 *    argv - array of arguments
 *    envp - array of enviroment variables
 */
int main(int argc, char* argv[], char* envp[]) {
  /*
   * initialize configuration (if config file isn't set)
   */
  config.run_script_on_finish = 0;
  
  /*
   * path to home dir "/home/$USER", read from $HOME enviroment variable
   */
  const char* home_dir = getenv("HOME");
  
  /* variables used with getopt() */
  extern char *optarg;
  extern int optind, opterr;
  
  int argument;
  int i_option_enabled = 0;
  /*
   * read arguments in while loop and set the neccessary variables
   */
  while( (argument = getopt(argc, argv, "c:dhin:uV:v")) != -1  ) {
    LOG(DEBUG, "argument='%c', optarg='%s'", argument, optarg);
    switch(argument) {
      case 'c':
        LOG(INFO, "Will use custom config file");
        use_custom_config_file = 1;
        custom_config_file_location = optarg;
        break;
      case 'd':
        LOG(WARN, "DRY RUN ENABLED!!!");
        dry_run = 1;
        break;
      case 'h':
        help();
        break;
      case 'i':
        LOG(INFO, "-i option enabled");
        i_option_enabled = 1;
        break;
      case 'n':
        override_config_threads_num = atoi(optarg);
        if (override_config_threads_num <= 0) {
          die("Invalid number of threads (-n parameter)!");
        }
        break;
      case 'u':
        disable_color_output();
        LOG(INFO, "Disabling colored output");
        break;
      case 'V':
        quality = optarg;
        break;
      case 'v':
        enable_verbose();
        break;
      default: /* '?' */
        die("Wrong arguments or argument requires an option which was not supplied");
    }
  }
  
  config_file_actions(home_dir);
  
  
  size = INITIAL_SIZE;
  /*
   * allocate initial memory for array of input and output filenames (paths)
   */ 
  input_files = malloc(sizeof(char*) * size); 
  output_filenames = malloc(sizeof(char*) * size);
  if ( input_files == NULL || output_filenames == NULL) {
        die("Memory error!");
  }
  
  /*
   * initialize input and output filenames to NULL. not sure if this is really neccessary
   */ 
  int i;
  for (i = 0; i < size; i++) {
    input_files[i] = NULL;
    output_filenames[i] = NULL;
  }
  
  if (i_option_enabled) {
    filenames_input(argc, argv, optind);
  } else {
    folder_input(argc, argv, optind);
  }
  
  

  /*
   * array which stores the threads which will run.
   */  
  pthread_t threads[config.threads];
  
  LOG(INFO, "Starting %d threads", config.threads);
  
  /*
   * this is only used for logging, so that each call to decode() knows which number it is.
   * a single variable for this cannot be used because it is passed by reference (pointer),
   * that's how the pthread_create works.
   */ 
  int* thread_indexes[config.threads];
  for (i = 0; i < config.threads; i++) {
    thread_indexes[i] = malloc(sizeof(int));
    if (thread_indexes[i] == NULL) die("Memory error!");
    *thread_indexes[i] = i;
    
    /*
     * create a new thread (call do_work() in a new thread)
     */
    pthread_create(&threads[i], NULL, &do_work, (void *) thread_indexes[i]);
  }
  
  /*
   * join the just created threads (wait for them to finish!
   */
  for (i = 0; i < config.threads; i++) {
    pthread_join(threads[i], NULL);
    free(thread_indexes[i]);
  }
  
  /*
   * if the config file has the run_script_on_finish set to a path to script, execute it. 
   * TODO explain how this script works
   */
  if (config.run_script_on_finish) {
    LOG(INFO, "Starting script: %s", config.script_path);
    for (i = 0; i < files_count; i++) {
      char* script_command = print_to_string("%s \"%s\" \"%s\"", config.script_path, 
          input_files[i], output_filenames[i]);
      LOG(INFO, "Running post script on file: %s", output_filenames[i]);
      LOG(DEBUG, "Running post script: %s", script_command);
      FILE* fp = popen(script_command, "r");
      if (fp == NULL) die("Unable to open script");
      free(script_command);
      pclose(fp);
    }
  }
  
  /** FREE MEMORY **/
  
  for (i = 0; i < size; i++) {
    if (input_files[i] != NULL) {
      free(input_files[i]);
    }
    if (output_filenames[i] != NULL) {
      free(output_filenames[i]);
    }
  }
  
  free(input_files);
  free(output_filenames);
  free_config(config);
  
  LOG(INFO, "Program finished successfully!");

  return 0;
}

/*
 * replace the variables in command from the config file (%INPUT_FILE%, %OUTPUT_FILE% and %QUALITY%)
 */ 
char* format_command(char *config_command, int currentFile) {
  char* command = strdup(config_command);
  char* replaced = NULL;
  
  replaced = replace_all_in_string(command, INPUT_FILE, input_files[currentFile]);
  free(command);
  command = strdup(replaced);
  free(replaced);
  
  replaced = replace_all_in_string(command, OUTPUT_FILE, output_filenames[currentFile]);
  free(command);
  command = strdup(replaced);
  free(replaced);
  
  replaced = replace_all_in_string(command, QUALITY, quality);
  free(command);
  command = strdup(replaced);
  free(replaced);
  
  return command;
}

/**
 * This method will be called by each thread.
 * It will call itself recursively later on.
 */
void *do_work(void * thread_num) {
  int thread = *((int *) thread_num);
  
  /*
   * the index of the current file
   */
  int currentFile;
  
  /* 
   * this is supposed to be something like synchronized in java,
   * prevents the multiple threads from reading/modifying the same
   * variable at the same time.
   */
  pthread_mutex_lock(&mutex);
  currentFile = ++last_file;
  pthread_mutex_unlock(&mutex);
  
  
  /* check if index is out of bounds */
  if (currentFile > files_count - 1) {
    /* if it is, do nothing */
    LOG(DEBUG, "THREAD %d: about to finish!", thread);
    return NULL;
  }
  
  /*
   * command to call
   */ 
  char* command = format_command(config.command, currentFile);
  
//  char* filename = extract_filename_from_path(input_files[currentFile]);
  LOG(INFO, "THREAD %d: input_filename='%s'", thread, input_files[currentFile]);
//  free(filename);
  
  LOG(DEBUG, "THREAD %d: command: %s", thread, command);
  
  /***********************
   *  START CONVERSION!  *
   ***********************/
  if (!dry_run) {
    FILE *file;
    file = popen(command, "r");
    
    /*
     * wait for the command execution to complete
     */
    pclose(file);
  }
  else {
    LOG(WARN, "THREAD %d: Dry run, skipping the command call!", thread);
  }
  free(command);
  
  /* 
   * convert the file which is next in line if there is any (recursive call)
   */
  do_work(thread_num);
  
  return NULL; 
}
