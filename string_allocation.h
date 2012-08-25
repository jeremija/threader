#ifndef _STRING_ALLOCATION
#define _STRING_ALLOCATION

/*
 * like sprintf, but dynamically allocates memory for resulting string. remember to call free()
 * to free that memory!
 */
char* print_to_string(const char *format, ...);

/*
 * same as find_position_of_string_from_position with start=0
 */
int find_position_of_string(const char *input, const char* search_string);

/*
 * searches the input string from start for substring search and returns the position where 
 * the search starts. If not found, returns -1
 */
int find_position_of_string_from_position(const char *input, const char* search, int start);

/*
 * replaces all substrings of 'search' in 'input' with 'replace'. returns a newly allocated
 * string which should be freed afterwards
 */
char* replace_all_in_string(const char *input, const char* search, const char* replace);


char* extract_filename_from_path(const char *path);

char* extract_filename_from_path_no_ext(const char* path, const char* ext);

#endif
