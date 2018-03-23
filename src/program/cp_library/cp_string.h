#ifndef _CP_STRING_H
#define _CP_STRING_H

/** @{ */
/**
 * @file
 * cp_string - 'safe' string allowing binary content 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

/** cp_string definition */
typedef struct _cp_string
{
	int size;    /**< size allocated   */
	int len;     /**< size used        */
	char *data;  /**< internal buffer  */
} cp_string;

/** allocate a new cp_string */
cp_string *cp_string_create(char *data, int len);
/** allocate an empty cp_string with a given buffer size */
cp_string *cp_string_create_empty(int initial_size);
/** deallocate a cp_string */
void cp_string_destroy(cp_string *str);
/** Sets string data to 0 */
void cp_string_clear_data(cp_string *str);
/** copies the content of a null terminated c string */
cp_string *cp_string_cstrcpy(cp_string *str, char *cstr);
/** copies the content of a cp_string */
//cp_string *cp_string_cp(cp_string *dst, cp_string *src);
int cp_string_copy(cp_string *dst, cp_string *src);
int cp_string_move(cp_string *dst, cp_string *src);
/** copy the string into *dst  */
int cp_string_cstrcopy(cp_string *dst, char *string, int len);
/** creates a copy of src string. internal buffer is duplicated. */
cp_string *cp_string_dup(cp_string *src);
/** creates a cp_string with src as its content */
cp_string *cp_string_cstrdup(char *src);
/** concatenate cp_strings */
cp_string *cp_string_cat(cp_string *str, cp_string *appendum);
/** append data from a buffer */
cp_string *cp_string_cat_bin(cp_string *str, void *bin, int len);
/** append data from a null terminated c string */
cp_string *cp_string_cstrcat(cp_string *str, char *cstr);
/** append a character to a string */
cp_string *cp_string_append_char(cp_string *str, char ch);
/** compare cp_strings */
int cp_string_cmp(cp_string *s1, cp_string *s2);
/** return a pointer to the internal buffer */
char *cp_string_tocstr(cp_string *str);
/** return the length of the internal buffer */
int cp_string_len(cp_string *s);
/** return the internal buffer */
char *cp_string_data(cp_string *s);

/** read len bytes from an open file descriptor (blocking) */
cp_string *cp_string_read(int fd, int len);
/** write the content of a cp_string to a file descriptor (blocking) */
int cp_string_write(cp_string *str, int fd);
/** read the contents of a file into a cp_string */
cp_string *cp_string_read_file(char *filename);
/** write the contents of a cp_string to a file */
int cp_string_write_file(cp_string *str, char *filename);

/** flip the contents of a cp_string */
void cp_string_flip(cp_string *str);
/** remove all occurrences of letters from str */
cp_string *cp_string_filter(cp_string *str, char *letters);

/** dump a cp_string to stdout */
const char *cp_hexdump_string(const void *data, size_t len);
void cp_string_dump(cp_string *str);
void cp_cstring_dump(char *data, int len);

char *del_char_from_string(char *str, char delchar);
int split_string_to_value(char *str, char *fmt, ...);

/** @} */

#endif

