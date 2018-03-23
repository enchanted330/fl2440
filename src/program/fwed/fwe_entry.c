/*********************************************************************************
 *      Copyright:  (C) Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  fw_entry.c 
 *    Description:  This is the FWE(FL2440 Worker Engine) applets(such as gsmd, 
 *                  gpsd, zigbeed) main entry point function.
 *                 
 *        Version:  1.0.0(01/13/2013~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "01/13/2013 02:40:36 PM"
 *                 
 ********************************************************************************/

#include "fwe_entry.h"
#include "gsmd.h"
#include "gpsd.h"
#include "zigd.h"

FWE_APPLET applets_list[] = 
{
    {"gsmd", gsmd_main},
    {"gpsd", gpsd_main},
    {"zigd", zigd_main},
    {0, 0}
};

static void entry_usage(void);
static void print_applist(void);

int main(int argc, char **argv)
{
    FWE_APPLET      *app = NULL; 
    const char       *applet_name = NULL;
    int              ind = 0;

    applet_name = basename(argv[0]);

    /* Run command like "FWE gsmd"  */
    if( !strncmp(applet_name, FWE_ENTRY_NAME, 4) )
    {
        if(argv[1] && argv[1][0]!='-') 
        { 
            applet_name = basename(argv[1]); 
            ind = 1;
        }
        else
        { 
            entry_usage();
            return 0;
        }
    }

    for(app=(FWE_APPLET *)&applets_list; app->name; app++)
    {
        if(!strcmp (app->name, applet_name) && app->main)
        {
            return app->main(argc-ind, argv+ind);
        }
    }

    printf("%s: applet not found\n\n", applet_name);
    print_applist();

    return 0;
}

static void print_applist(void)
{
    FWE_APPLET       *app;

    printf("Currently defined applets:\n");
    for(app=(FWE_APPLET *)&applets_list; app->name; app++)
    {
        printf("%s ", app->name);
    }
    printf("\n");
}

/* Just print the help information  */
static void entry_usage(void)
{
    banner(FWE_ENTRY_NAME);

    printf("Usage: %s [applet] [arguments]...\n", FWE_ENTRY_NAME);
    printf("   or: applet [arguments]...\n");

    printf("\n\t%s is a multi-call binary that combines many FL2440 Work Engine(FWE) applets\n", FWE_ENTRY_NAME);
    printf("\tinto a single executable. We can create a symbolic link to %s for each applet\n",FWE_ENTRY_NAME);
    printf("\tlike busybox work.\n\n");

    print_applist();
    printf("\n");

    return ;
}
