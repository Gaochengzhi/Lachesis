/**
 * File              : headline.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 16.02.2022
 * Last Modified Date: 18.02.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */
#include "./headline.h"
#include <stdio.h>
#define _COSTOMIZE "default"
void lec_print_headline()
{
    struct utsname uname_pointer;

    uname(&uname_pointer);

    printf("Lachesis 0.0.1(%s) \n", _COSTOMIZE);
    puts("if you want to quit, type 'q'\n");
    printf("System name: %s, ", uname_pointer.sysname);
    printf("Version: %s, ", uname_pointer.release);
    printf("Machine: %s. ", uname_pointer.machine);
    puts("\n");
    puts("NOTE: this version is made to implement the lval type");
}
