#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>

int main(int argc, char **argv)
{
    int fd;

    //close(0);
    close(1);
    //close(2);
    //以阻塞方式打开设备文件，非阻塞时flags=O_NONBLOCK
    fd = open("/dev/adc", 0);
    if(fd < 0)
    {
        printf("Open ADC Device Faild!\n");
        exit(1);
    }

    while(1)
    {
        int ret;
        int data;
        
        //读设备
        ret = read(fd, &data, sizeof(data));

        if(ret != sizeof(data))
        {
            if(errno != EAGAIN)
            {
                printf("Read ADC Device Faild!\n");
            }

            continue;
        }
        else
        {
            printf("Read ADC value is: %d\n", data);
        }
        sleep(2);
    }

    close(fd);

    return 0;
}
