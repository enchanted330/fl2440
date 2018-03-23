
/**
 * @addtogroup cp_string
 */
/** @{ */
/**
 * @file
 * cp_string - 'safe' string implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "cp_string.h"
#include "cp_common.h"

cp_string *cp_string_create(char *data, int len)
{
	cp_string *str;
	
#ifdef DEBUG
	if (len < 0) 
	{
		return NULL;
	}
#endif
	
	str = t_malloc(sizeof(cp_string));

	if (str)
	{
		str->len = len;
		str->size = str->len + 1;
		str->data = t_malloc(str->size * sizeof(char));
		if (str->data)
			memcpy(str->data, data, str->len);
		else
		{
			t_free(str);
			str = NULL;
		}
	}

	return str;
}

cp_string *cp_string_create_empty(int initial_size)
{
	cp_string *str = t_malloc(sizeof(cp_string));

	if (str)
    { 
		str->len = 0;
		str->size = initial_size;
		str->data = (char *) t_malloc(str->size * sizeof(char));
		if (str->data == NULL)
		{
			t_free(str);
			str = NULL;
		}
	}

	return str;
}

void cp_string_destroy(cp_string *str)
{
	if (str)
	{
		if (str->data)
        {
			t_free(str->data);
        }

		t_free(str);
	}
}


void cp_string_clear_data(cp_string *str)
{
	if (str && str->data)
    {
        memset(str->data, 0, str->size);
        str->len = 0;
    }
}

cp_string *cp_string_cstrcpy(cp_string *str, char *cstr)
{
	if (str)
	{
		str->len = strlen(cstr);
		if (str->size < str->len + 1)
		{
			str->size = str->len + 1;
			str->data = t_malloc(str->size * sizeof(char));
		}
		if (str->data)
			memcpy(str->data, cstr, str->size * sizeof(char));
		else
		{
			t_free(str);
			str = NULL;
		}
	}

	return str;
}

int cp_string_copy(cp_string *dst, cp_string *src)
{
    int        left;
    int        size;

    if(!dst || !dst->data || !src || !src->data)
        return 0;

    left=dst->size - dst->len;
    size = left>src->len ? src->len : left;

    memcpy(&dst->data[dst->len], src->data, size);
    dst->len += size;

    return size;
}

int cp_string_cstrcopy(cp_string *dst, char *string, int len)
{
    int        left;
    int        size;

    if(!dst || !dst->data || !string || len<=0)
        return 0;

    left=dst->size - dst->len;
    size = left>len ? len : left;

    memcpy(&dst->data[dst->len], string, size);
    dst->len += size;

    return size;
}


int cp_string_move(cp_string *dst, cp_string *src)
{
    int        left;
    int        size;

    if(!dst || !dst->data || !src || !src->data)
        return 0;

    /* Check how many left size in $dst and set the size */
    left=dst->size - dst->len;
    size = left>src->len ? src->len : left;

    /* copy the $src data to $dst  */
    memcpy(&dst->data[dst->len], src->data, size);
    dst->len += size;

    /* remove the $src copied data in it*/
    src->len -= size;
    memmove (src->data, src->data+size, src->len);

    return size;
}

cp_string *cp_string_dup(cp_string *src)
{
	cp_string *str = t_malloc(sizeof(cp_string));

	if (str)
	{
		*str = *src; /* bitwise copy */
		str->data = t_malloc((str->len + 1) * sizeof(char));
		if (str->data)
			memcpy(str->data, src->data, (str->len  + 1) * sizeof(char));
		else
		{
			t_free(str);
			str = NULL;
		}
	}

	return str;
}

cp_string *cp_string_cstrdup(char *src)
{
	cp_string *str = t_malloc(sizeof(cp_string));

	if (str)
	{
		str->len = strlen(src);
		str->size = str->len + 1;
		str->data = t_malloc(str->size * sizeof(char));
		if (str->data == NULL)
		{
			t_free(str);
			return NULL;
		}
		memcpy(str->data, src, str->size);
	}

	return str;
}
	
cp_string *cp_string_cat(cp_string *str, cp_string *appendum)
{
	int len = str->len;
	str->len += appendum->len;
	if (str->len + 1 > str->size)
	{
		str->size = str->len + 1;
		str->data = realloc(str->data, str->size * sizeof(char));
	}
	if (str->data)
		memcpy(str->data + len * sizeof(char), appendum->data, 
			appendum->len * sizeof(char));

	return str;
}

cp_string *cp_string_cstrcat(cp_string *str, char *cstr)
{
	int len = str->len;
	int clen = strlen(cstr);

	str->len += clen * sizeof(char);
	if (str->len + 1 > str->size)
	{
//		str->size = str->len + 0x400 - (str->len % 0x400); /* align to 1kb block */
		str->size = str->len + 1;
		str->data = realloc(str->data, str->size * sizeof(char));
	}
	if (str->data)
		memcpy(str->data + len * sizeof(char), cstr, clen);

	return str;
}

cp_string *cp_string_append_char(cp_string *str, char ch)
{
	if (str->len + 1 > str->size)
	{
		str->size = str->len + 0x100;
		str->data = realloc(str->data, str->size * sizeof(char));
		if (str->data == NULL) return NULL;
	}
	str->data[str->len++] = ch;

	return str;
}

cp_string *cp_string_cat_bin(cp_string *str, void *bin, int len)
{
	int olen = str->len;
	str->len += len;

	if (str->len > str->size)
	{
		str->size = str->len + 0x400 - (str->len % 0x400); /* align to 1kb block */
		str->data = realloc(str->data, str->size * sizeof(char));
	}
	memcpy(&str->data[olen], bin, len);

	return str;
}

int cp_string_cmp(cp_string *s1, cp_string *s2)
{
	if (s1 == s2) return 0; //~~ implies cp_string_cmp(NULL, NULL) == 0

	if (s1 == NULL) return -1;
	if (s2 == NULL) return 1;

	if (s1->len == s2->len)
		return memcmp(s1->data, s2->data, s1->len);
	else
	{
		int p = (s1->len > s2->len) ? s2->len : s1->len;
		int rc = memcmp(s1->data, s2->data, p);
		if (rc == 0) 
			return s1->len - s2->len;
		return rc;
	}
}

char *cp_string_tocstr(cp_string *str)
{
	char *cstr = NULL;
	
	if (str)
	{
		str->data[str->len * sizeof(char)] = '\0';
//		str->data[str->len * sizeof(char) + 1] = '\0';
		cstr = str->data;
	}

	return cstr;
}

int cp_string_len(cp_string *s)
{
	return s->len;
}

char *cp_string_data(cp_string *s)
{
	return s->data;
}

#define CHUNK 0x1000
cp_string *cp_string_read(int fd, int len)
{
	char buf[CHUNK];
	int read_len;
	cp_string *res = NULL;
	
	if (len == 0)
		read_len = CHUNK;
	else
		read_len = len < CHUNK ? len : CHUNK;

	while (len == 0 || res == NULL || res->len < len)
	{
		int rc = 
			read(fd, buf, read_len);
		if (rc <= 0) break;
		if (res == NULL)
		{
			res = cp_string_create(buf, rc);
			if (res == NULL) return NULL;
		}
		else
			cp_string_cat_bin(res, buf, rc);
	}

	return res;
}

int cp_string_write(cp_string *str, int fd)
{
	int rc;
	int total = 0;

	while (total < str->len)
	{
		rc = write(fd, &str->data[total], str->len - total);

		/* write sets EAGAIN when a socket is marked non-blocking and the
		 * write would block. trying to write again could result in spinning
		 * on the write call.
		 */
		if (rc == -1) 
		{
			if (errno == EINTR /* || errno == EAGAIN */) /* try again */ 
				continue;
			else 
				break; 
		}
		total += rc;
	}

	return total;
}

cp_string *cp_string_read_file(char *filename)
{
	cp_string *res;
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL) return NULL;

	res = cp_string_read(fileno(fp), 0);
	fclose(fp);

	return res;
}

int cp_string_write_file(cp_string *str, char *filename)
{
	int rc;
	FILE *fp = fopen(filename, "wb");
	if (fp == NULL) return 0;

	rc = cp_string_write(str, fileno(fp));
	fclose(fp);

	return rc;
}

#define LINELEN 81
#define CHARS_PER_LINE 16

static char *print_char = 
	"                "
	"                "
	" !\"#$%&'()*+,-./"
	"0123456789:;<=>?"
	"@ABCDEFGHIJKLMNO"
	"PQRSTUVWXYZ[\\]^_"
	"`abcdefghijklmno"
	"pqrstuvwxyz{|}~ "
	"                "
	"                "
	" ???????????????"
	"????????????????"
	"????????????????"
	"????????????????"
	"????????????????"
	"????????????????";

void cp_cstring_dump(char *data, int len)
{
	int rc;
	int idx;
	char prn[LINELEN];
	char lit[CHARS_PER_LINE + 1];
	char hc[4];
	short line_done = 1;

	rc = len;
	idx = 0;
	lit[CHARS_PER_LINE] = '\0';
	while (rc > 0)
	{
		if (line_done) 
			snprintf(prn, LINELEN, "%08X: ", idx);
		do
		{
			unsigned char c = data[idx];
			snprintf(hc, 4, "%02X ", c);
			strncat(prn, hc, 4);
			lit[idx % CHARS_PER_LINE] = print_char[c];
			++idx;
		} while (--rc > 0 && (idx % CHARS_PER_LINE != 0));
		line_done = (idx % CHARS_PER_LINE) == 0;
		if (line_done) 
			printf("%s  %s\n", prn, lit);
		else if (rc == 0)
			strncat(prn, "   ", LINELEN);
	}
	if (!line_done)
	{
		lit[(idx % CHARS_PER_LINE)] = '\0';
		while ((++idx % CHARS_PER_LINE) != 0) 
			strncat(prn, "   ", LINELEN);

		printf("%s  %s\n", prn, lit);

	}
}

const char *cp_hexdump_string(const void *data, size_t len)
{ 
    static char string[1024]; 
    unsigned char *d = (unsigned char *)data; 
    unsigned int i, left;
                      
    string[0] = '\0';
    left = sizeof(string);
    for (i = 0; len--; i += 3) { 
        if (i >= sizeof(string) - 4)
            break;
        snprintf(string + i, 4, " %02x", *d++);
    }

    return string;
}


void cp_string_dump(cp_string *str)
{
	int rc;
	int idx;
	char prn[LINELEN];
	char lit[CHARS_PER_LINE + 1];
	char hc[4];
	short line_done = 1;

	rc = str->len;
	idx = 0;
	lit[CHARS_PER_LINE] = '\0';
	while (rc > 0)
	{
		if (line_done) 
			snprintf(prn, LINELEN, "%08X: ", idx);
		do
		{
			unsigned char c = str->data[idx];
			snprintf(hc, 4, "%02X ", c);
			strncat(prn, hc, 4);
			lit[idx % CHARS_PER_LINE] = print_char[c];
			++idx;
		} while (--rc > 0 && (idx % CHARS_PER_LINE != 0));
		line_done = (idx % CHARS_PER_LINE) == 0;
		if (line_done) 
			printf("%s  %s\n", prn, lit);
		else if (rc == 0)
			strncat(prn, "   ", LINELEN);
	}
	if (!line_done)
	{
		lit[(idx % CHARS_PER_LINE)] = '\0';
		while ((++idx % CHARS_PER_LINE) != 0) 
			strncat(prn, "   ", LINELEN);

		printf("%s  %s\n", prn, lit);
	}
}

/** flip the contents of a cp_string */
void cp_string_flip(cp_string *str)
{
	if (str->len)
	{
		char *i, *f, ch;
		f = &str->data[str->len - 1];
		i = str->data;
		while (i < f)
		{
			ch = *i;
			*i = *f;
			*f = ch;
			i++;
			f--;
		}
	}
}

/* remove all occurrences of letters from str */
cp_string *cp_string_filter(cp_string *str, char *letters)
{
	char *i;
	char *f;

	str->data[str->len] = '\0';
	i = str->data;
	while ((f = strpbrk(i, letters)))
	{
		i = f;
		while (*f && strchr(letters, *f)) f++;
		if (*f)
		{
			memmove(i, f, str->len - (f - str->data));
			str->len -= f - i;
			str->data[str->len] = '\0';
		}
		else
		{
			*i = '\0';
			str->len -= str->len - (i - str->data);
			break;
		}
	}

	return str;
}

/** @} */


char *del_char_from_string(char *str, char delchar)
{
    char *idx = str;
    char *end = str;
    while (*idx)
    {
        if (*idx == delchar)
        {
            ++idx;
        }
        else
        {
            *end = *idx;
            ++end;
            ++idx;
        }
    }
    *end = '\0';
    return str;
}

int split_string_to_value(char *str, char *fmt, ...)
{
    va_list     ap;

    int         *iPtr;
    long        *lPtr;
    char        *pcPtr;

    char        delim[2]={*(fmt+2), '\0'};
    char        *result;

    va_start(ap, fmt);

    result = strtok( str, delim );

    while(*fmt)
    {
        switch (*fmt++)
        {
            case 's':  /* string */
                pcPtr = va_arg(ap, char *);
                if(NULL!= result)
                {
                    if(NULL!=pcPtr)
                    {
                        strcpy(pcPtr, result);
                    }
                    result = strtok( NULL, delim );
                }
                else
                    goto OUT;

                break;

            case 'd':  /*  int */
                iPtr = va_arg(ap, int *);
                if(NULL!= result)
                {
                    if(NULL!=iPtr)
                    {
                        *iPtr = atoi(result);
                    }
                    result = strtok( NULL, delim );
                }
                else
                    goto OUT;

                break;

            case 'l':  /*  long */
                lPtr = va_arg(ap, long *);
                if(NULL!= result)
                {
                    if(NULL!=lPtr)
                    {
                        *lPtr = strtol(result, NULL, 10);
                    }
                    result = strtok( NULL, delim );
                }
                else
                    goto OUT;

                break;

            case 'x':  /*  long hex*/
                lPtr = va_arg(ap, long *);
                if(NULL!= result)
                {
                    if(NULL!=lPtr)
                    {
                        *lPtr = strtol(result, NULL, 16);
                    }
                    result = strtok( NULL, delim );
                }
                else
                    goto OUT;

                break;
        }
    }

OUT:
    va_end(ap);
    return 0;
}

