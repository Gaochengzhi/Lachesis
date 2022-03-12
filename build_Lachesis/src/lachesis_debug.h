/**
 * File              : lachesis_debug.h
 * License           : The MIT License (MIT);
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 21.02.2022
 * Last Modified Date: 12.03.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */

#pragma once
#ifndef _LACHESIS_DEBUG_H
#define _LACHESIS_DEBUG_H

#include <stdarg.h>
#include <stdio.h>
/*function interface*/

extern int _debug_mode;
void lbug_print_s(char* s);
void lbug_print_sl(char* s, long l);
void lbug_print_slsl(char* s1, long l1, char* s2, long l2);
void lbug_print_sssl(char* s1, char* l1, char* s2, long l2);
void lbug_print_sdsl(char* s1, int l1, char* s2, long l2);

#define ERROW_CHECK(args, conditon, fmt, ...)                                  \
    if (!(conditon)) {                                                         \
        LObject* err = lobj_error(fmt, ##__VA_ARGS__);                         \
        lobj_del(args);                                                        \
        return err;                                                            \
    }
#endif

#define ERROW_CHECK_TYPE(func, args, index, expect)                            \
    ERROW_CHECK(args, args->cell[index]->type == expect,                       \
        "function '%s' passed incorrect type for argument %i.\nGot %s, "       \
        "Expect %s.",                                                          \
        func, index, lobj_type_name(args->cell[index]->type),                  \
        lobj_type_name(expect))

#define ERROW_CHECK_NUM(func, args, num)                                       \
    ERROW_CHECK(args, args->count == num,                                      \
        "Function '%s' passed incorrect number of argument.\nGot %i, Expect "  \
        "%i",                                                                  \
        func, args->count, num)

#define ERROW_CHECK_NOT_EMPTY(func, args, index)                               \
    ERROW_CHECK(args, args->cell[index]->count != 0,                           \
        "Function '%s' passed {} for argument %i.", func, index)
