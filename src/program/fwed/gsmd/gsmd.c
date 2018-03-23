/*********************************************************************************
 *      Copyright:  (C) Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  gsmd.c
 *    Description:  This is the RWME(Radiation Monitor Works Engine) gsmd applet entry point
 *                 
 *        Version:  1.0.0(01/13/2013~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "01/13/2013 02:40:36 PM"
 *                 
 ********************************************************************************/
#include "gsmd.h"

static void usage_gsmd(const char *progname)
{
    banner(progname);

    printf("Usage: %s [OPTION]...\n", progname); 
    printf(" %s is a daemon program running on the device, which used to\n", progname); 
    printf("auto start ppp connection and receive/send SMS by GPRS module.\n"); 
    
    printf("\nMandatory arguments to long options are mandatory for short options too:\n"); 
    printf(" -d[debug   ]  Running in debug mode\n");
    printf(" -l[level   ]  Set the log level as [0..%d]\n", LOG_LEVEL_MAX-1); 
    printf(" -h[help    ]  Display this help information\n");
    printf(" -v[version ]  Display the program version\n");

    return;
}


int gsmd_main(int argc, char **argv)
{
    int            opt; 
    int            rv = 0; 
    int            debug = 0;
    char           pid_file[64] = { 0 }; /* The file used to record the PID */
    char           *conf_file = GSMD_DEF_CONF_FILE;
    const char     *progname=NULL;
    char           *log_file = GSMD_DEF_LOG_FILE;
    int            log_level = GSMD_DEF_LOG_LEVEL;
    cp_logger      *logger = NULL;
    MODULE_INFO    module;

    struct option long_options[] = { 
        {"conf", required_argument, NULL, 'c'}, 
        {"debug", no_argument, NULL, 'd'}, 
        {"level", required_argument, NULL, 'l'}, 
        {"version", no_argument, NULL, 'v'}, 
        {"help", no_argument, NULL, 'h'}, 
        {NULL, 0, NULL, 0} 
    };

    progname = basename(argv[0]);

    /* Parser the command line parameters */
    while ((opt = getopt_long(argc, argv, "c:dl:vh", long_options, NULL)) != -1) 
    { 
        switch (opt) 
        { 
            case 'c':  /* Set configure file */ 
                conf_file = optarg; 
                break; 
            
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
                usage_gsmd(progname); 
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
 
    if ( !(logger=cp_log_init(NULL, log_file, log_level, GSMD_LOG_FILE_SIZE)) )
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

    if( init_gsm_module(&module) < 0 )
    {
        log_fatal("Initialise gsmd work context failure, rv=%d\n", rv); 
        goto CleanUp; 
    }

    /*  Start the PPP thread to do PPP dial up */ 
    if( ( rv=start_thread_ppp(&module)) < 0)
    { 
        log_fatal("Start PPP thread failure, rv=%d\n", rv); 
        goto CleanUp; 
    } 
    
#ifdef CONFIG_SMS
    /*  Create the SMS thread to receive the SMS */
    if( (rv=start_thread_sms(&module)) < 0)
    {
        log_fatal("Start SMS thread failure, rv=%d\n", rv); 
        goto CleanUp;
    }
#endif

    ctrl_thread_start(&module);

CleanUp: 

    log_nrml("Terminate program \"%s\".\n", progname);
    unlink(pid_file); 
    cp_log_term(); 
    return rv;
}


