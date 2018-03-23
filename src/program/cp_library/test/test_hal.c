/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  test_hal.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(12/05/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "12/05/2012 04:52:59 PM"
 *                 
 ********************************************************************************/

#include <libgen.h>
#include <getopt.h>
#include "cp_hal.h"

#define  HAL_LIBRARY_TEST

void test_led_hal_api(void)
{
    printf("+------------------------------------+\n");
    printf("|          Test LED HAL API          |\n");
    printf("+------------------------------------+\n");

    printf("Turn all LED off\n");
    hal_turn_led_off(LED_ALL);
    sleep(2);

    printf("Turn LED blink one by one\n");
    hal_turn_led_blink(LED_SYS, MODE_FAST);
    hal_turn_led_blink(LED_SIM1, MODE_FAST);
    hal_turn_led_blink(LED_SIM2, MODE_FAST);
    hal_turn_led_blink(LED_WIFI, MODE_FAST);
    hal_turn_led_blink(LED_ETH, MODE_FAST);
    sleep(4);

    printf("Turn LED off one by one\n");
    hal_turn_led_off(LED_SYS);
    hal_turn_led_off(LED_SIM1);
    hal_turn_led_off(LED_SIM2);
    hal_turn_led_off(LED_WIFI);
    hal_turn_led_off(LED_ETH);
    sleep(4);

    printf("Turn LED on one by one\n");
    hal_turn_led_on(LED_SYS);
    hal_turn_led_on(LED_SIM1);
    hal_turn_led_on(LED_SIM2);
    hal_turn_led_on(LED_WIFI);
    hal_turn_led_on(LED_ETH);
    sleep(4);

    printf("Turn all LED off\n");
    hal_turn_led_off(LED_ALL);
    sleep(2);

    printf("Turn all LED blink\n");
    hal_turn_led_blink(LED_ALL, MODE_SLOW);
    sleep(4);

    printf("Turn all LED on\n");
    hal_turn_led_on(LED_ALL);
    sleep(4);
    hal_turn_led_off(LED_ALL);
}

void test_buzzer_hal_api(void)
{
    printf("+------------------------------------+\n");
    printf("|         Test Buzzer HAL API        |\n");
    printf("+------------------------------------+\n");

    printf("Turn buzzer on\n");
    hal_turn_buzzer_on();
    sleep(2);

    printf("Turn buzzer off\n");
    hal_turn_buzzer_off();
    sleep(2);

    printf("Turn buzzer beep for 5 times\n");
    hal_turn_buzzer_beep(3);
    sleep(4);

    printf("Turn buzzer off\n");
    hal_turn_buzzer_off();
    sleep(3);

    printf("Turn buzzer beep infinitely\n");
    hal_turn_buzzer_beep(0);
    sleep(5);

    printf("Turn buzzer off\n");
    hal_turn_buzzer_off();
}

void test_gprs_hal_api(void)
{
    int        rv = 0;

    printf("+------------------------------------+\n");
    printf("|         Test GPRS HAL API          |\n");
    printf("+------------------------------------+\n");

    hal_poweron_gprs(SIM_NONE);

    rv = hal_check_simdoor(SIM_ALL);
    printf("Check all SIM card present status: 0x%02x\n", rv);

    rv = hal_check_simdoor(SIM1);
    printf("Check SIM1 card present status: 0x%02x\n", rv);

    rv = hal_check_simdoor(SIM2);
    printf("Check SIM2 card present status: 0x%02x\n", rv);

    printf("Get current working SIM card: 0x%02x\n", hal_get_worksim());

    hal_set_worksim(SIM2);
    printf("Set current working SIM to SIM2\n");

    printf("Get current working SIM card: 0x%02x\n", hal_get_worksim());

    hal_set_worksim(SIM1);
    printf("Set current working SIM to SIM1\n");

    printf("Get current working SIM card: 0x%02x\n", hal_get_worksim());
    hal_poweroff_gprs();

}

void print_usage(char *progname)
{
    printf("Usage: %s [OPTION]...\n", progname);
    printf("\nMandatory arguments to long options are mandatory for short options too:\n");

    printf(" -l[led     ]  Test LED HAL API\n");
    printf(" -b[buzzer  ]  Test Buzzer HAL API\n");
    printf(" -g[gprs    ]  Test GPRS HAL API\n");
    printf(" -h[help    ]  Display this help information\n");


}


#ifdef HAL_LIBRARY_TEST
/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    int                   opt;
    char                  *progname=NULL;
    int                   test_led = 0;
    int                   test_gprs = 0;
    int                   test_buzzer = 0;

    struct option long_options[] = {
        {"led", no_argument, NULL, 'l'},
        {"buzzer", no_argument, NULL, 'b'},
        {"gprs", no_argument, NULL, 'g'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    progname = basename(argv[0]);

    while ((opt = getopt_long(argc, argv, "bghl", long_options, NULL)) != -1)
    {
        switch(opt)
        {
            case 'b':
                test_buzzer = 1;
                break;

            case 'l':
                test_led = 1;
                break;

            case 'g':
                test_gprs = 1;
                break;

            case 'h':
                print_usage(progname);

            default:
                break;
        }
    }

    if(!test_buzzer && !test_led && !test_gprs)
        print_usage(progname);

    if(test_buzzer)
        test_buzzer_hal_api();

    if(test_led)
        test_led_hal_api();

    if(test_gprs)
        test_gprs_hal_api();

    return 0;
} /* ----- End of main() ----- */

#endif /*  HAL_LIBRARY_TEST */
