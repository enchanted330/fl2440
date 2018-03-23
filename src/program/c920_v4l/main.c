/*********************************************************************************
 *      Copyright:  (C) 2015 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  main.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(12/02/2015)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "12/02/2015 05:14:18 PM"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <libgen.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <zbar.h>

#include "version.h"
#include "cp_logger.h"

#define BELL "\a"

#define ON           1
#define OFF          0

int display_video_capability(int fd);
int list_video_format(int fd);
int get_video_format(int fd);
int set_video_format(int fd, int width, int height, int pix_fmt);
void set_video_capture(int fd, int status); /* status: 1<on> 0<off>  */
void my_video_test(char *dev);

static void data_handler (zbar_image_t *img, const void *userdata);

static void program_usage(const char *progname)
{
    banner(progname);

    printf("Usage: %s [OPTION]...\n", progname);
    printf(" %s is a daemon program running on the device, which used to \n", progname);
    printf("receive date from a common/SSL socket and transfer the data to \n");
    printf("remote server by a common/SSL socket.\n");
  
    printf("\nMandatory arguments to long options are mandatory for short options too:\n");
    printf(" -d[debug   ]  Specify the video device\n");
    printf(" -p[prescale]  <W>x<H>, request alternate video image size from driver\n");
    printf(" -D[debug   ]  Running in debug mode\n");
    printf(" -l[level   ]  Set the log level as [0..%d]\n", LOG_LEVEL_MAX-1);
    printf(" -h[help    ]  Display this help information\n");
    printf(" -v[version ]  Display the program version\n");

    return;
}

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    const char     *progname=NULL;
    char           *log_file = "/logs/v4l.log";
    int            log_level = LOG_LEVEL_NRML;
    char           *video_device="/dev/video0";
    int            opt, rv, i=0;
    int            w=640, h=480;
    int            debug = 0;
    cp_logger      logger;

    int              display=0;
    zbar_processor_t *proc;

    struct option long_options[] = { 
        {"device", required_argument, NULL, 'd'}, 
        {"prescale", required_argument, NULL, 'p'}, 
        {"debug", required_argument, NULL, 'D'}, 
        {"level", required_argument, NULL, 'l'}, 
        {"version", no_argument, NULL, 'v'}, 
        {"help", no_argument, NULL, 'h'}, 
        {NULL, 0, NULL, 0}  
    };  

    progname = basename(argv[0]);

    /* setup zbar library standalone processor, threads will be used if available */
    proc = zbar_processor_create(1);
    if(!proc) 
    {
        log_err("Unable to allocate zbar memory\n");
        return -2;
    }   
    zbar_processor_set_data_handler(proc, data_handler, NULL); 


    /* Parser the command line parameters */
    while ((opt = getopt_long(argc, argv, "c:p:d:D:l:vh", long_options, NULL)) != -1)
    {
        switch (opt)
        {
            case 'd': /* Specify video device */
                video_device = optarg;
                break;

            case 'p': /* 640x480  */
                {
                    char *x = NULL;

                    w = strtol(optarg, &x, 10);
                    if(x && *x == 'x')
                    {
                        h = strtol(x + 1, NULL, 10);
                    }
                    if(!w || !h || !x || *x != 'x') 
                    {
                        printf("ERROR: invalid prescale: %s\n\n", optarg);
                        return -1;
                    }
                    zbar_processor_request_size(proc, w, h);
                }
                break;


            case 'D': /* Set debug running */
                debug = 1;
                log_file = DBG_LOG_FILE;
                zbar_set_verbosity(atoi(optarg));
                break;

            case 'l': /* Set the log level */
                i = atoi(optarg);
                log_level = i>LOG_LEVEL_MAX ? LOG_LEVEL_MAX-1 : LOG_LEVEL_MAX-1;
                logger.flag |= CP_LOGGER_LEVEL_OPT;
                break;

            case 'v':  /* Get software version */
                banner(progname); /* Defined in version.h */
                return EXIT_SUCCESS;

            case 'h':  /* Get help information */
                program_usage(progname);
                return 0;

            default:
                break;
        } /*  end of "switch(opt)" */
    }


    if ( !cp_log_init(&logger, log_file, log_level, 10240 ) || cp_log_open() )
    {
        printf("Initialize logger system failure with file: %s\n", logger.file);
        return -1;
    }

    my_video_test(video_device);
    while(1);

    /*  open video device, don't open window */
    if(zbar_processor_init(proc, video_device, display) )
    {
        log_err("zbar init video device '%s' failure\n", video_device);
        return(zbar_processor_error_spew(proc, 0));
    }

    /*  start video */ 
    int active = 1;
    if(zbar_processor_set_active(proc, active))
    {
        log_err("zbar start video device '%s' failure\n", video_device);
        return(zbar_processor_error_spew(proc, 0));
    }

    /*  let the callback handle data */
    while((rv = zbar_processor_user_wait(proc, -1)) >= 0) 
    {
        if(rv == 'q' || rv == 'Q')
            break;

        if(rv == ' ') 
        {
            active = !active;
            if(zbar_processor_set_active(proc, active))
            {
                return(zbar_processor_error_spew(proc, 0));
            }
        }
    }

    /*  report any errors that aren't "window closed" */
    if(rv && rv!='q' && rv!='Q' && zbar_processor_get_error_code(proc) != ZBAR_ERR_CLOSED)
    {
        log_err("zbar window closed\n");
        return(zbar_processor_error_spew(proc, 0));
    }

    /*  free resources (leak check) */
    zbar_processor_destroy(proc);

    return 0;
} /* ----- End of main() ----- */

void my_video_test(char *dev)
{
    int           fd;

    if( (fd=open(dev, O_RDWR)) < 0)
    {
        log_err("Open device '%s' failure: %s\n", dev, strerror(errno));
        return ;
    }

    display_video_capability(fd);
    list_video_format(fd);

    get_video_format(fd);
    set_video_format(fd, 640, 480, V4L2_PIX_FMT_MJPEG);
    get_video_format(fd);

    set_video_capture(fd, ON);
    sleep(5);
    set_video_capture(fd, OFF);

    close(fd);
    return ;
}

int display_video_capability(int fd)
{
    struct v4l2_capability cap; 
    
    if( ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0 )
    {
        log_err("ioctl VIDIOC_QUERYCAP failed: %s\n", strerror(errno));
        return -1;
    }

    log_nrml("Video device capability:\n");
    log_nrml("   Card       Name: %s\n", cap.card);
    log_nrml("   Driver  version: %s v%u.%u.%u\n", cap.driver, (cap.version>>16)&0XFF, (cap.version>>8)&0XFF,cap.version&0XFF);

    if (V4L2_CAP_VIDEO_CAPTURE & cap.capabilities )
    {
        log_nrml("   Capture video device\n");
    }

    if (V4L2_CAP_STREAMING & cap.capabilities )
    {
        log_nrml("   Streaming video device\n");
    }

    return 0;
}

int list_video_format(int fd)
{
    struct v4l2_fmtdesc fmtdesc; 
    
    fmtdesc.index=0; 
    fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; 
    
    log_nrml("Video device support format:\n");

    while( ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1 )
    { 
        log_nrml("   %d: %s\n", fmtdesc.index+1, fmtdesc.description); 
        fmtdesc.index++;
    }

    return 0;
}

/* pix_fmt:  V4L2_PIX_FMT_MJPEG or V4L2_PIX_FMT_YUYV
 */
int set_video_format(int fd, int width, int height, int pix_fmt)
{
    struct v4l2_format  fmt; 
    char                *pix_fmt_str="Unknow";
    
    fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; 

    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    fmt.fmt.pix.pixelformat = pix_fmt;

    switch(fmt.fmt.pix.pixelformat)
    {
        case V4L2_PIX_FMT_YUYV:
            pix_fmt_str = "YUV 4:2:2 (YUYV)";
            break;

        case V4L2_PIX_FMT_MJPEG:
            pix_fmt_str = "MJPEG";
            break;

        default:
            break;
    }
    log_nrml("Set Video format: %s %dx%d\n", pix_fmt_str, fmt.fmt.pix.width,fmt.fmt.pix.height);

    if( ioctl(fd, VIDIOC_S_FMT, &fmt) < 0 )
    {
        log_err("ioctl VIDIOC_S_FMT failed: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int get_video_format(int fd)
{
    struct v4l2_format  fmt; 
    char                *pix_fmt_str="Unknow";
    
    fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE; 

    if( ioctl(fd, VIDIOC_G_FMT, &fmt) < 0 )
    {
        log_err("ioctl VIDIOC_G_FMT failed: %s\n", strerror(errno));
        return -1;
    }

    switch(fmt.fmt.pix.pixelformat)
    {
        case V4L2_PIX_FMT_YUYV:
            pix_fmt_str = "YUV 4:2:2 (YUYV)";
            break;

        case V4L2_PIX_FMT_MJPEG:
            pix_fmt_str = "MJPEG";
            break;

        default:
            break;
    }

    log_nrml("Current Video format: %s %dx%d\n", pix_fmt_str, fmt.fmt.pix.width,fmt.fmt.pix.height);

    return 0;
}

int query_video_standard(int fd)
{
    int           rv;
    v4l2_std_id   std;
    
    do {
        rv = ioctl(fd, VIDIOC_QUERYSTD, &std);
    } while (rv == -1 && errno == EAGAIN);

    switch (std) 
    {
        case V4L2_STD_NTSC:
            log_nrml("Video support standard: NTSC\n");
            break;

        case V4L2_STD_PAL:
            log_nrml("Video support standard: PAL\n");
            break;

        default:
            log_nrml("Video support standard: Unkow\n");
            break;
    }

    return 0;
}


void set_video_capture(int fd, int status)
{
    enum v4l2_buf_type       type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if( !status )
        ioctl(fd, VIDIOC_STREAMOFF, &type);
    else 
        ioctl(fd, VIDIOC_STREAMON, &type);
}

static void data_handler (zbar_image_t *img, const void *userdata)
{
    int                  n = 0;
    const zbar_symbol_t  *sym = zbar_image_first_symbol(img);

    if(!sym)
    {
        log_err("zbar symbole is NULL\n");
        return ;
    }

    for(; sym; sym = zbar_symbol_next(sym)) 
    {
        if(zbar_symbol_get_count(sym))
            continue;

        zbar_symbol_type_t type = zbar_symbol_get_type(sym);
        if(type == ZBAR_PARTIAL)
            continue;

        log_nrml("%s%s:%s\n",zbar_get_symbol_name(type), zbar_get_addon_name(type), zbar_symbol_get_data(sym));
        n++;
    }
}


