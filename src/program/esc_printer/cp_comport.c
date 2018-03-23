/*  ********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue <guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cp_comport.c
 *    Description:  It's the comport operate library.
 *                  
 *        Version:  1.0.0(10/17/2011~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/17/2011 03:33:25 PM"
 *                  
 ********************************************************************************/

#include    "cp_comport.h"

/**************************************************************************************
 *  Description: Set the comport structure
 *   Input Args: dev_name:  The comport device name path, such as '/dev/ttyS3'
 *               baudrate:  The baudrate, such as 115200
 *               settings:  The databit,parity,stopbit,flowctrl settings, such as '8N1N'
 *  Output Args: NONE
 * Return Value: The COM_PORT structure pointer.
 *************************************************************************************/
COM_PORT *comport_init(const char *dev_name, int baudrate, const char *settings)
{
    COM_PORT *comport = NULL;
    if (NULL == (comport = (COM_PORT *) malloc(sizeof(COM_PORT))))
    {
        return NULL;
    }
    memset(comport, 0, sizeof(COM_PORT));
    comport->is_connted = 0;
    comport->frag_size = 128;

    strncpy(comport->dev_name, dev_name, DEVNAME_LEN);
    comport->baudrate = baudrate;

    set_settings(comport, settings);
#ifdef  COM_DEBUG
    disp_settings(comport);
#endif

    return comport;
}

#ifdef  COM_DEBUG
void disp_settings(COM_PORT * comport)
{
    COM_PRINT("Device:\t\t\t\"%s\"\n", comport->dev_name);
    COM_PRINT("Baudrate:\t\t%ld\n", comport->baudrate);
    COM_PRINT("DataBit:\t\t\'%d\'\n", comport->databit);
    switch (comport->parity)
    {
      case 0:
          COM_PRINT("Parity:\t\t\t\'N\'\n");
          break;
      case 1:
          COM_PRINT("Parity:\t\t\t\'O\'\n");
          break;
      case 2:
          COM_PRINT("Parity:\t\t\t\'E\'\n");
          break;
      case 3:
          COM_PRINT("Parity:\t\t\t\'S\'\n");
          break;
    }
    COM_PRINT("StopBit:\t\t\'%ld\'\n", (long int)comport->stopbit);
    switch (comport->flowctrl)
    {
      case 0:
          COM_PRINT("FlowCtrl:\t\t\'N\'\n");
          break;
      case 1:
          COM_PRINT("FlowCtrl:\t\t\'S\'\n");
          break;
      case 2:
          COM_PRINT("FlowCtrl:\t\t\'H\'\n");
          break;
      case 3:
          COM_PRINT("FlowCtrl:\t\t\'B\'\n");
          break;
    }
    COM_PRINT("\n");
    return;
}
#endif

/**************************************************************************************
 *  Description: Set the comport databit,parity,stopbit,flowctrl
 *   Input Args: comport: the COM_PORT pointer
 *               settings: The databit/parity/stopbit/flowctrl settings as like "8N1N" 
 *  Output Args: NONE
 * Return Value: NONE
 *************************************************************************************/
void set_settings(COM_PORT * comport, const char *settings)
{
    if(NULL==settings || NULL==comport)
        return ;

    switch (settings[0])        /* data bit */
    {
      case '7':
          comport->databit = 7;
          break;
      case '8':
      default:
          comport->databit = 8;
          break;
    }

    switch (settings[1])        /* parity */
    {
      case 'O':
      case 'o':
          comport->parity = 1;
          break;
      case 'E':
      case 'e':
          comport->parity = 2;
          break;
      case 'S':
      case 's':
          comport->parity = 3;
          break;
      case 'N':
      case 'n':
      default:
          comport->parity = 0;
          break;
    }

    switch (settings[2])        /* stop bit */
    {
      case '0':
          comport->stopbit = 0;
          break;
      case '1':
      default:
          comport->stopbit = 1;
          break;
    }

    switch (settings[3])        /* flow control */
    {
      case 'S':
      case 's':
          comport->flowctrl = 1;
          break;
      case 'H':
      case 'h':
          comport->flowctrl = 2;
          break;
      case 'B':
      case 'b':
          comport->flowctrl = 3;
          break;
      case 'N':
      case 'n':
      default:
          comport->flowctrl = 0;
          break;
    }
}

void comport_close(COM_PORT * comport)
{
    if (0 != comport->fd)
    {
        COM_PRINT("Close device \"%s\"\n", comport->dev_name);
        close(comport->fd);
    }
    comport->is_connted = 0x00;
    comport->fd = -1;
}

void comport_term(COM_PORT * comport)
{
    if(NULL == comport)
        return;

    if (0 != comport->fd)
    {
        comport_close(comport);
    }
    memset(comport, 0x00, sizeof(COM_PORT)); 
    free(comport);
    comport = NULL;

    return;
}

int comport_open(COM_PORT * comport)
{
    int retval = -1;
    struct termios old_cfg, new_cfg;
    int old_flags;
    long tmp;

    if(NULL==comport)
        return -1;

    comport_close(comport);


    /* Not a TTY device */
    if( !strstr(comport->dev_name, "tty"))
    {
        COM_PRINT("Open Not tty device \"%s\"\n", comport->dev_name);
        comport->fd = open(comport->dev_name, O_RDWR);
        retval = comport->fd<0 ? -2 : comport->fd;
        goto CleanUp;
    }

    comport->fd = open(comport->dev_name, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (comport->fd < 0)
    {
        retval = -3;
        goto CleanUp;
    }
    COM_PRINT("Open device \"%s\"\n", comport->dev_name);

    if ((-1 != (old_flags = fcntl(comport->fd, F_GETFL, 0)))
        && (-1 != fcntl(comport->fd, F_SETFL, old_flags & ~O_NONBLOCK)))
    {
        // Flush input and output
        if (-1 == tcflush(comport->fd, TCIOFLUSH))
        {
            retval = -4;
            goto CleanUp;
        }
    }
    else                        // Failure
    {
        retval = -5;
        goto CleanUp;
    }

    if (0 != tcgetattr(comport->fd, &old_cfg))
    {
        retval = -6;          // Failed to get Com settings  
        goto CleanUp;
    }

    memset(&new_cfg, 0, sizeof(new_cfg));

    /*=====================================*/
    /*       Configure comport         */
    /*=====================================*/

    new_cfg.c_cflag &= ~CSIZE;
    new_cfg.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    new_cfg.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    new_cfg.c_oflag &= ~(OPOST);

    /* Set the data bit */
    switch (comport->databit)
    {
      case 0x07:
          new_cfg.c_cflag |= CS7;
          break;
      case 0x06:
          new_cfg.c_cflag |= CS6;
          break;
      case 0x05:
          new_cfg.c_cflag |= CS5;
          break;
      default:
          new_cfg.c_cflag |= CS8;
          break;
    }

    /* Set the parity */
    switch (comport->parity)
    {
      case 0x01:               // Odd  
          new_cfg.c_cflag |= (PARENB | PARODD);
          new_cfg.c_cflag |= (INPCK | ISTRIP);
          break;
      case 0x02:               // Even 
          new_cfg.c_cflag |= PARENB;
          new_cfg.c_cflag &= ~PARODD;;
          new_cfg.c_cflag |= (INPCK | ISTRIP);
          break;
      case 0x03:
          new_cfg.c_cflag &= ~PARENB;
          new_cfg.c_cflag &= ~CSTOPB;
          break;
      default:
          new_cfg.c_cflag &= ~PARENB;
    }

    /* Set Stop bit */
    if (0x01 != comport->stopbit)
    {
        new_cfg.c_cflag |= CSTOPB;
    }
    else
    {
        new_cfg.c_cflag &= ~CSTOPB;
    }

    /* Set flow control */
    switch (comport->flowctrl)
    {
      case 1:                  // Software control 
      case 3:
          new_cfg.c_cflag &= ~(CRTSCTS);
          new_cfg.c_iflag |= (IXON | IXOFF);
          break;
      case 2:                  // Hardware control
          new_cfg.c_cflag |= CRTSCTS;   // Also called CRTSCTS
          new_cfg.c_iflag &= ~(IXON | IXOFF);
          break;
      default:                 // NONE
          new_cfg.c_cflag &= ~(CRTSCTS);
          new_cfg.c_iflag &= ~(IXON | IXOFF);
          break;
    }

    /* Set baudrate */
    switch (comport->baudrate)
    {
      case 115200:
          tmp = B115200;
          break;
      case 57600:
          tmp = B57600;
          break;
      case 38400:
          tmp = B38400;
          break;
      case 19200:
          tmp = B19200;
          break;
      case 9600:
          tmp = B9600;
          break;
      case 4800:
          tmp = B4800;
          break;
      case 2400:
          tmp = B2400;
          break;
      case 1800:
          tmp = B1800;
          break;
      case 1200:
          tmp = B1200;
          break;
      case 600:
          tmp = B600;
          break;
      case 300:
          tmp = B300;
          break;
      case 200:
          tmp = B200;
          break;
      case 150:
          tmp = B150;
          break;
      case 134:
          tmp = B134;
          break;
      case 110:
          tmp = B110;
          break;
      case 75:
          tmp = B75;
          break;
      case 50:
          tmp = B50;
          break;
      default:
          tmp = B115200;
    }
    cfsetispeed(&new_cfg, tmp);
    cfsetispeed(&new_cfg, tmp);

    /* Set the Com port timeout settings */
    new_cfg.c_cc[VMIN] = 0;
    new_cfg.c_cc[VTIME] = 0;

    tcflush(comport->fd, TCIFLUSH);
    if (0 != tcsetattr(comport->fd, TCSANOW, &new_cfg))
    {
        retval = -7;          // Failed to set device com port settings   
        goto CleanUp;
    }

    COM_PRINT("Connected device \"%s\".\n", comport->dev_name);
    comport->is_connted = 0x01;
    retval = comport->fd;

CleanUp:
    COM_PRINT("Open device \"%s\" %s.\n", comport->dev_name, retval>0 ? "successfully" : "failure");
    return retval;
}

void nonblock()
{
    struct termios ttystate;

    //get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);

    //turn off canonical mode
    ttystate.c_lflag &= ~ICANON;
    //minimum of number input read.
    ttystate.c_cc[VMIN] = 1;

    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

int kbhit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

int comport_recv(COM_PORT * comport, char *buf, int buf_size, unsigned long timeout)
{
    int retval = 0;             // Function return value
    int iRet;
    fd_set stReadFds, stExcpFds;
    struct timeval stTime;

    if (NULL == buf || 0 >= buf_size)
    {
        COM_PRINT("%s() usage error.\n", __FUNCTION__);
        retval = -1;
        goto CleanUp;
    }

    if (0x01 != comport->is_connted)
    {
        COM_PRINT("%s() comport not connected.\n", __FUNCTION__);
        retval = -2;
        goto CleanUp;
    }

    //printf("bufsize=%d timeout=%lu\n", buf_size, timeout);

    FD_ZERO(&stReadFds);
    FD_ZERO(&stExcpFds);
    FD_SET(comport->fd, &stReadFds);
    FD_SET(comport->fd, &stExcpFds);

    if (0xFFFFFFFF != timeout)
    {
        stTime.tv_sec = (time_t) (timeout / 1000);
        stTime.tv_usec = (long)(1000 * (timeout % 1000));

        iRet = select(comport->fd + 1, &stReadFds, 0, &stExcpFds, &stTime);
        if (0 == iRet)
        {
            retval = 0;         // No data in Com port buffer
            goto CleanUp;
        }
        else if (0 < iRet)
        {
            if (0 != FD_ISSET(comport->fd, &stExcpFds))
            {
                retval = -6;  // Error during checking recv status    
                COM_PRINT("Error checking recv status.\n");
                goto CleanUp;
            }

            if (0 == FD_ISSET(comport->fd, &stReadFds))
            {
                retval = 0;  // No incoming data 
                COM_PRINT("No incoming data.\n");
                goto CleanUp;
            }
        }
        else
        {
            if (EINTR == errno)
            {
                COM_PRINT("catch interrupt signal.\n");
                retval = 0;  // Interrupted signal catched
            }
            else
            {
                COM_PRINT("Check recv status failure.\n");
                retval = -7;  // Error during checking recv status
            }

            goto CleanUp;
        }
    }

    usleep(10000); /* sleep for 10ms for data incoming */

    // Get data from Com port
    iRet = read(comport->fd, buf, buf_size);
    if (0 > iRet)
    {
        if (EINTR == errno)
            retval = 0;      // Interrupted signal catched
        else
            retval = -3;      // Failed to read Com port

        goto CleanUp;
    }

#if 0
    {
        int   i=0;
        printf("Receive %d bytes data: \n", iRet);
        for(i=0; i<iRet; i++)
        {
            printf("0x%02x ", buf[i]);
        }
        printf("\n");
    }
#endif

    retval = iRet;

  CleanUp:
    return retval;

}

int comport_send(COM_PORT * comport, char *buf, int send_bytes)
{
    char *ptr, *end;
    int retval = 0;
    int send = 0;

    if (NULL == buf || 0 >= send_bytes)
    {
        COM_PRINT("%s() Usage error.\n", __FUNCTION__);
        retval = -1;
        goto CleanUp;
    }

    if (0x01 != comport->is_connted)    // Comport not opened ?
    {
        retval = -3;
        COM_PRINT("Serail not connected.\n");
        goto CleanUp;
    }

    //printf("Send %s with %d bytes.\n", buf, send_bytes);

    // Large data, then slice them and send
    if (comport->frag_size < send_bytes)
    {
        ptr = buf;
        end = buf + send_bytes;

        do
        {
            // Large than frag_size
            if (comport->frag_size < (end - ptr))
            {
                send = write(comport->fd, ptr, comport->frag_size);
                if (0 >= send || comport->frag_size != send)
                {
                    retval = -4;
                    goto CleanUp;
                }
                ptr += comport->frag_size;
            }
            else                // Less than frag_size, maybe last fragmention.
            {
                send = write(comport->fd, ptr, (end - ptr));
                if (0 >= send || (end - ptr) != send)
                {
                    retval = -4;
                    goto CleanUp;
                }
                ptr += (end - ptr);
            }
        }
        while (ptr < end);
    }
    else                        // The send data is not large than a fragmention.
    {
        send = write(comport->fd, buf, send_bytes);
        if (0 >= send || send_bytes != send)
        {
            retval = -5;
            goto CleanUp;
        }
    }

  CleanUp:
    return retval;
}

