/*********************************************************************************
 *      Copyright:  (C) Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  gpsd.c
 *    Description:  This is the RWME(Radiation Monitor Works Engine) gpsd applet entry point
 *                 
 *        Version:  1.0.0(01/13/2013~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "01/13/2013 02:40:36 PM"
 *                 
 ********************************************************************************/
#include "gpsd.h"
#include "cp_hal.h"
#include "cp_time.h"

static void usage_gpsd(const char *progname)
{
    banner(progname);

    printf("Usage: %s [OPTION]...\n", progname); 
    printf(" %s is a daemon program running on the device to locate the device\n", progname); 
    
    printf("\nMandatory arguments to long options are mandatory for short options too:\n"); 
    printf(" -d[debug   ]  Running in debug mode\n");
    printf(" -l[level   ]  Set the log level as [0..%d]\n", LOG_LEVEL_MAX-1); 
    printf(" -h[help    ]  Display this help information\n");
    printf(" -v[version ]  Display the program version\n");

    return;
}

int init_gpsd_context(GPSD_CONTEX *gps)
{
    log_info("Initialize gpsd context.\n");

    gps->pwr_status = POWER_OFF;

    nmea_zero_INFO(&(gps->info));

    /*  Initialize the GPS dataport pointer */
    gps->comport = comport_init(GPS_DATAPORT, 9600, "8N1N");
    if(NULL == gps->comport)
    {
        log_err("Initialise GPS data port %s on 9600 with 8N1N failure\n", GPS_DATAPORT);
        return -1;
    }
    
    if( comport_open(gps->comport) < 0)
    {
        log_err("Open GPS data port %s on 9600 with 8N1N failure\n", GPS_DATAPORT);
        return -2;
    }

    log_dbg("Open GPS data port %s on 9600 with 8N1N successfully\n", GPS_DATAPORT);
    return 0;
}

void term_gpsd_context(GPSD_CONTEX *gps)
{
    comport_term(gps->comport);

#if 0
    if( ON== hal_get_gps_power()) 
    {
        hal_turn_gps_power(OFF);
    }
#endif

    return ;
}



int start_gpsd_service(GPSD_CONTEX *gps)
{
    int            len;
    char           buf[1024];

    while( !g_cp_signal.stop )
    {
        if( OFF== (gps->pwr_status=hal_get_gps_power()) )
        {
            hal_turn_gps_power(ON);
            continue;
        }

        memset(buf, 0, sizeof(buf));
        if( (len=comport_recv(gps->comport, buf, sizeof(buf), 3000)) > 0)
        {
            /* Data sample: 
             * $GPGGA,081803.000,3029.9100,N,11423.2012,E,1,05,1.6,38.7,M,-13.7,M,,0000*4C
             * $GPGSA,A,3,18,24,21,22,15,,,,,,,,3.1,1.6,2.7*3A
             * $GPRMC,081803.000,A,3029.9100,N,11423.2012,E,1.14,126.38,210113,,,A*62  
             */
            nmea_parse(buf, len, &(gps->info));
            log_nrml("GPS get UTC time: %d-%d-%d %d:%d:%d.%d\n", gps->info.utc.year+1900, gps->info.utc.mon+1, gps->info.utc.day,
                    gps->info.utc.hour+8, gps->info.utc.min, gps->info.utc.sec, gps->info.utc.hsec);
            log_nrml("GPS Sig[%d] Latitude:[%f] Longitude:[%f]\n", gps->info.sig, gps->info.lat, gps->info.lon);
        }

        micro_second_sleep(100);
    }

    return 0;
}

int gpsd_main(int argc, char **argv)
{
    int            opt; 
    int            rv = 0; 
    int            debug = 0;
    char           pid_file[64] = { 0 }; /* The file used to record the PID */
    const char     *progname=NULL;
    char           *log_file = GPSD_DEF_LOG_FILE;
    int            log_level = GPSD_DEF_LOG_LEVEL;
    cp_logger      *logger = NULL;
    GPSD_CONTEX    gpsd_ctx;

    struct option long_options[] = { 
        {"debug", no_argument, NULL, 'd'}, 
        {"level", required_argument, NULL, 'l'}, 
        {"version", no_argument, NULL, 'v'}, 
        {"help", no_argument, NULL, 'h'}, 
        {NULL, 0, NULL, 0} 
    };

    progname = basename(argv[0]);

    /* Parser the command line parameters */
    while ((opt = getopt_long(argc, argv, "dl:vh", long_options, NULL)) != -1) 
    { 
        switch (opt) 
        { 
            case 'd': /* Set debug running */ 
                debug = 1; 
                log_file = DBG_LOG_FILE;
                break; 

            case 'l': /* Set the log level */
                log_level = atoi(optarg); 
                break; 

            case 'v':  /* Get software version */ 
                banner(progname); /* Defined in version.h */ 
                return EXIT_SUCCESS; 
            
            case 'h':  /* Get help information */ 
                usage_gpsd(progname); 
                return 0; 

            default: 
                break; 
        } /*  end of "switch(opt)" */ 
    }

    if( !debug )
    {
        snprintf(pid_file, sizeof(pid_file), "%s/%s.pid", PID_FILE_PATH, progname);
        if( check_daemon_running(pid_file) ) 
        { 
            printf("Programe already running, exit now.\n"); 
            return -1; 
        } 
    }
 
    if ( !(logger=cp_log_init(NULL, log_file, log_level, GPSD_LOG_FILE_SIZE)) )
    {
        printf("Logger initialize failure with file: %s\n", logger->file);
        return -1;
    }

    if ( cp_log_open() )
    {
        printf("Logger initialize failure with file: %s\n", logger->file);
        return -1;
    }

    if( !debug )
    {
        if( set_daemon_running(pid_file) )
        {
            log_fatal("Set program \"%s\" running as daemon failure.\n", progname);
            goto CleanUp;
        } 
    }

    cp_install_proc_signal();

    if( init_gpsd_context(&gpsd_ctx) < 0 )
    {
        log_fatal("Initialise gpsd work context failure, rv=%d\n", rv); 
        goto CleanUp; 
    }

    /*  Start the GPS thread to locate the device */ 
    if( (rv=start_gpsd_service(&gpsd_ctx)) < 0)
    { 
        log_fatal("Start PPP thread failure, rv=%d\n", rv); 
        goto CleanUp; 
    } 
    
CleanUp: 
    log_nrml("Terminate program \"%s\".\n", progname);
    term_gpsd_context(&gpsd_ctx);
    unlink(pid_file); 
    cp_log_term(); 
    return rv;
}


