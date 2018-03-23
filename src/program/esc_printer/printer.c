#include "cp_comport.h"
#include "logo.h"

/* Linux Image convert command:
 *
 * convert -depth 1 -resize 160x113! -monochrome logo.png logo.bmp
 */

//#define TEST_FILE_NAME    "a.bmp"
#define TEST_FILE_NAME    "test.txt"
#define COM_PRINTER       "/dev/ttyS0"

#define  PICTURE_TYPE_NORMAL          0 
#define  PICTURE_TYPE_DOUBLE_WIDTH    1 
#define  PICTURE_TYPE_DOUBLE_HEIGHT   2 
#define  PICTURE_TYPE_QUADRUPLE       3

#define uart_send_print(buf, size)  comport_send(print_comport, buf, size)

COM_PORT   *print_comport = NULL;

void print_lfn(unsigned char  lines)
{
    char       buf[3]={0x1B, 0x64};

    buf[2] = lines;

    uart_send_print(buf, 3);
}


/* Print picture in ESC/POS printer 
 * $hsize is the horizontal size(image width pix/8),
 * $vsize is the vertical size(image heigth)
 * for example: image is 160x113, then hsize=160/8=20 vsize=113
 *
 * $image_buf is the C array output  by Image2Lcd tools on mode 
 * horizontal scan, monochrome, without image head data  
 */
void image_printh(unsigned char type, unsigned short hsize, unsigned short vsize, unsigned char *image_buf)
{
    unsigned long   size;
    char            val[3] = {0x1D, 0x76, 0x30};

    uart_send_print(val, 3);
    uart_send_print((char *)&type, 1);

    uart_send_print((char *)&hsize, 2);
    size = hsize;

    uart_send_print((char *)&vsize, 2);
    size *= vsize;

    uart_send_print((char *)image_buf, size);
}

int main (int argc, char **argv)
{
    char       init_printer[2]={0x1B, 0X40}; /* ESC @ */
    char       set_format[3]={0x1B, 0x21, 0x08}; /* ESC ! 08 */
    char       print_str1[]={0x61, 0x62, 0x63, 0x64, 0x0d, 0x0a};
    char       print_str2[]={0xB4, 0xF2, 0xd3, 0xa1, 0xca, 0xbe, 0xc0, 0xfd, 0xa3, 0xba, 0xd7, 0xd6, 0xcc, 0xe5, 0x0d, 0x0a};

    if( !(print_comport=comport_init(COM_PRINTER, 9600, "8N1N") ) )
    {
        printf("Init printer comport '%s' failure\n", COM_PRINTER);
        return -1;
    }

    if( !comport_open(print_comport) )
    {
        printf("Open comport '%s' failure\n", print_comport->dev_name);
        return -2;
    }

    uart_send_print(init_printer, 2);
    uart_send_print(set_format, 3);
    uart_send_print(print_str1, sizeof(print_str1));
    uart_send_print(print_str2, sizeof(print_str2));

#ifdef TEST_FILE_NAME
    {
        int        fd = -1;
        int        len = 0;
        char       buf[1024];

        fd = open(TEST_FILE_NAME,O_RDONLY);
        if(fd < 0)
        {
            printf("open file failed\n");
            return -3;
        }

        len = read(fd, buf, sizeof(buf));
        uart_send_print(buf, len);
    }
#endif

    print_lfn(2);

    /* Image is 160(20*8bit)x113  */
    image_printh(PICTURE_TYPE_NORMAL, 20, 113, (unsigned char *)gImage_logo);

    print_lfn(4);

    comport_term(print_comport);

    return 0;
} /* ----- End of main() ----- */

