/*********************************************************************************
 *      Copyright:  (C) 2013 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  crc321.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(09/18/2013~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "09/18/2013 01:26:29 PM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

typedef unsigned int   uint32_t;
typedef unsigned char  uint8_t;

uint32_t rc_crc32(uint32_t crc, const char *buf, size_t len)
{
    static uint32_t table[256]; 
    static int have_table = 0; uint32_t rem;
    uint8_t octet;
    int i, j;
    const char *p, *q; 
    
    /* This check is not thread safe; there is no mutex. */
    if (have_table == 0) 
    {
        /* Calculate CRC table. */
        for (i = 0; i < 256; i++) 
        {
            rem = i;  /* remainder from polynomial division */
            for (j = 0; j < 8; j++) 
            {
                if (rem & 1) 
                {
                    rem >>= 1;
                    rem ^= 0xedb88320;
                }
                else
                    rem >>= 1;
            }
            
            table[i] = rem;
        }

        have_table = 1;
    }
 
    crc = ~crc;
    q = buf + len;
    for (p = buf; p < q; p++) 
    { 
        octet = *p;  /* Cast to unsigned octet. */
        crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
    }
    
    return ~crc;
}



/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    uint32_t      crc = 0;
    int           fd = -1;
    struct stat   sb;
    char          *addr;
    size_t        file_size;

    if (argc != 2)
    {
        fprintf(stderr, "%s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
        handle_error("open");

    if (fstat(fd, &sb) == -1)           /*  To obtain file size */
        handle_error("fstat");

    file_size = sb.st_size;

    addr = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) 
        handle_error("mmap");

    crc = rc_crc32(crc, addr, file_size);

    munmap(addr, file_size);
    close(fd);

    printf("CRC32: %u\n", crc);
    return 0;
} /* ----- End of main() ----- */

