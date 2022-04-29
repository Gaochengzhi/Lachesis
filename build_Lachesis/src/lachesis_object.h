/**
 * File              : lachesis_object.h
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 18.02.2022
 * Last Modified Date: 27.04.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */

#ifndef _LACHESIS_OBJECT_H
#define _LACHESIS_OBJECT_H

#include "../lib/mpc/mpc.h"
#include "lachesis_builtin.h"
#include "lachesis_debug.h"
#include "lachesis_type.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*function interface*/

void lobj_print_expr(LObject *obj, char begin_symbol, char end_symbol);
void lobj_print_string(LObject *string_obj);
void lobj_fprint_string(FILE *fd, LObject *string_obj);
void lobj_print(LObject *obj);
void lobj_fprint(FILE *fd, LObject *obj);
void lobj_print_line(LObject *obj);
void lobj_delete(LObject *obj);

/* __init_function__*/
LObject *lobj_number(long number);
LObject *lobj_double(double double_num);
LObject *lobj_error(char *fmt, ...);
LObject *lobj_symbol(char *symbol);
LObject *lobj_string(const char *string);
LObject *lobj_sexpr(void);
LObject *lobj_qexpr(void);

LObject *lobj_add(LObject *target_obj, LObject *add_obj);
LObject *lobj_eval_sexpr(lenv *e, LObject *obj);
LObject *lobj_eval(lenv *e, LObject *obj);
LObject *lobj_read(mpc_ast_t *t);
LObject *lobj_read_num(mpc_ast_t *t);
LObject *lobj_read_double(mpc_ast_t *t);
LObject *lobj_read_str(mpc_ast_t *t);
LObject *lobj_take_out(LObject *obj, int index);
LObject *lobj_pop(LObject *obj, int i);
LObject *lobj_join(LObject *x, LObject *y);
LObject *lobj_func(lbuiltin func);
LObject *lobj_copy(LObject *obj);
LObject *lobj_lambda(LObject *arguments, LObject *body);
LObject *lobj_call(lenv *e, LObject *func, LObject *obj);
bool lobj_equal(LObject *x, LObject *y);

#endif
