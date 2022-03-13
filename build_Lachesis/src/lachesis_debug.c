/**
 * File              : lachesis_debug.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 23.02.2022
 * Last Modified Date: 13.03.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */

#include "lachesis_debug.h"
#include <stdio.h>
#include <string.h>

int _debug_mode = 0;

void lbug_print_s(char* s)
{
    if (_debug_mode == 1) {
        printf("%-35s\n", s);
    }
}
void lbug_print_sl(char* s, long l)
{
    if (_debug_mode == 1) {
        printf("%-35s   [%-15li]\n", s, l);
    }
}
void lbug_print_slsl(char* s1, long l1, char* s2, long l2)
{
    if (_debug_mode == 1) {
        printf("%-35s   [%-15li]   %-12s   [%-15li]\n", s1, l1, s2, l2);
    }
}

void lbug_print_sssl(char* s1, const char* l1, char* s2, long l2)
{
    if (_debug_mode == 1) {
        printf("%-35s   [%-15s]   %-12s   [%-15li]\n", s1, l1, s2, l2);
    }
}

void lbug_print_sdsl(char* s1, int l1, char* s2, long l2)
{
    if (_debug_mode == 1) {
        printf("%-35s   [%-15d]   %-12s   [%-15li]\n", s1, l1, s2, l2);
    }
}
