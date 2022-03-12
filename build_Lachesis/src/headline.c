/**
 * File              : headline.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 16.02.2022
 * Last Modified Date: 12.03.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */
#include "headline.h"
#include <stdio.h>
#include <stdlib.h>
#define _COSTOMIZE "default"
void lec_print_headline()
{
    struct utsname uname_pointer;

    uname(&uname_pointer);

    puts(
        " █████                          █████                        ███      "
        "      \n░░███                          ░░███                        "
        "░░░             \n ░███         ██████    ██████  ░███████    ██████  "
        " █████  ████   █████    \n ░███        ░░░░░███  ███░░███ ░███░░███  "
        "███░░███ ███░░  ░░███  ███░░     \n ░███         ███████ ░███ ░░░  "
        "░███ ░███ ░███████ ░░█████  ░███ ░░█████    \n ░███      █ ███░░███ "
        "░███  ███ ░███ ░███ ░███░░░   ░░░░███ ░███  ░░░░███   \n "
        "███████████░░████████░░██████  ███ █████░░██████  ██████  █████ "
        "██████    \n░░░░░░░░░░░  ░░░░░░░░  ░░░░░░  ░░░░ ░░░░░  ░░░░░░  ░░░░░░ "
        " ░░░░░ ░░░░░░     \n                                                  "
        "                          \n                                          "
        "                                  \n                                  "
        "                                          \n");
    printf("Lachesis 0.0.1(%s) \n", _COSTOMIZE);
    puts("if you want to quit, type 'q'\n");
    printf("System name: %s, ", uname_pointer.sysname);
    printf("Version: %s, ", uname_pointer.release);
    printf("Machine: %s. ", uname_pointer.machine);
    puts("\n");
    puts("      Type -h or --h to help");
    puts("      Type -g or --g to debug\n\n");
}
void print_help()
{
    printf("Function: \
            \n def {x} 100 \
            \n +x 1 \
            \n \
            \n def {a b} 5 6 \
            \n + ab \
            ");
}
