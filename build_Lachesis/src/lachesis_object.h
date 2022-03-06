/**
 * File              : lachesis_object.h
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 18.02.2022
 * Last Modified Date: 04.03.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */

#pragma once

#ifndef _LACHESIS_OBJECT_H
#define _LACHESIS_OBJECT_H

#include "../lib/mpc/mpc.h"
#include "lachesis_debug.h"
#include "lachesis_type.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*function interface*/
void lobj_print_expr(LObject* v, char open, char close);
void lobj_print();
void lobj_print_line(LObject* v);
void lobj_del(LObject* v);
LObject* lobj_number(long x);
LObject* lobj_error(char* fmt, ...);
LObject* lobj_symbol(char* s);
LObject* lobj_sexpr(void);
LObject* lobj_qexpr(void);
LObject* lobj_add(LObject* v, LObject* x);
LObject* lobj_eval_sexpr(lenv* e, LObject* v);
LObject* lobj_eval(lenv* e, LObject* v);
LObject* lobj_read(mpc_ast_t* t);
LObject* lobj_read_num(mpc_ast_t* t);
LObject* lobj_take_out(LObject* v, int i);
LObject* lobj_pop(LObject* v, int i);
LObject* lobj_join(LObject* x, LObject* y);
LObject* lobj_func(lbuiltin func);
LObject* lobj_copy(LObject* v);
LObject* built_in(LObject* a, char* func);
LObject* built_in_op(lenv* e, LObject* a, char* op);

LObject* built_in_add(lenv* e, LObject* a);
LObject* built_in_sub(lenv* e, LObject* a);
LObject* built_in_mult(lenv* e, LObject* a);
LObject* built_in_div(lenv* e, LObject* a);
LObject* built_in_head(lenv* e, LObject* a);
LObject* built_in_tail(lenv* e, LObject* a);
LObject* built_in_list(lenv* e, LObject* a);
LObject* built_in_eval(lenv* e, LObject* a);
LObject* built_in_join(lenv* e, LObject* a);
LObject* built_in_define(lenv* e, LObject* a);
#endif
