#ifndef _STRING_ALLOCATION
#define _STRING_ALLOCATION

/*
 * like sprintf, but dynamically allocates memory for resulting string. remember to call free()
 * to free that memory!
 */
char* print_to_string(const char *format, ...);

int find_position_of_string(const char *input, const char* search_string);
int find_position_of_string_from_position(const char *input, const char* search, int start);

char* replace_first_occurrence_in_string(const char *input, const char* search, const char* replace);


#endif
