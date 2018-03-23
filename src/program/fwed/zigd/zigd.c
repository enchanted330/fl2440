/*********************************************************************************
 *      Copyright:  (C) Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  zigd.c
 *    Description:  This is the RWME(Radiation Monitor Works Engine) zigd applet entry point
 *                 
 *        Version:  1.0.0(01/13/2013~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "01/13/2013 02:40:36 PM"
 *                 
 ********************************************************************************/
#include "zigd.h"

static void usage_zigd(const char *progname)
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

int init_zigd_context(ZIGD_CONTEX *ctx)
{
    log_info("Initialize zigd context.\n");

    ctx->pwr_status = POWER_OFF;

    /*  Initialize the zig dataport pointer */
    ctx->comport = comport_init(ZIGD_DATAPORT, 9600, "8N1N");
    if(NULL == ctx->comport)
        return -1;
    
    return 0;
}


int zigd_main(int argc, char **argv)
{
    int            opt; 
    int            rv = 0; 
    int            debug = 0;
    char           pid_file[64] = { 0 }; /* The file used to record the PID */
    const char     *progname=NULL;
    char           *log_file = ZIGD_DEF_LOG_FILE;
    int            log_level = ZIGD_DEF_LOG_LEVEL;
    cp_logger      *logger = NULL;
    ZIGD_CONTEX    zigd_ctx;

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
                usage_zigd(progname); 
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
 
    if ( !(logger=cp_log_init(NULL, log_file, log_level, ZIGD_LOG_FILE_SIZE)) )
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

    if( init_zigd_context(&zigd_ctx) < 0 )
    {
        log_fatal("Initialise zigd work context failure, rv=%d\n", rv); 
        goto CleanUp; 
    }

#if 0
    /*  Start the zig thread to locate the device */ 
    if( (rv=start_thread_zigd(&(zigd_ctx))) < 0)
    { 
        log_fatal("Start PPP thread failure, rv=%d\n", rv); 
        goto CleanUp; 
    } 
    
    ctrl_thread_start( &zigd_ctx);
#endif

CleanUp: 
    log_nrml("Terminate program \"%s\".\n", progname);
    unlink(pid_file); 
    cp_log_term(); 
    return rv;
}


