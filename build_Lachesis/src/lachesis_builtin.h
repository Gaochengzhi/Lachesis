/**
 * File              : lachesis_builtin.h
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 07.03.2022
 * Last Modified Date: 12.03.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */

#pragma once
#if !defined(_LACHESIS_BUILTIN_H)
#define _LACHESIS_BUILTIN_H

#include "../lib/mpc/mpc.h"
#include "lachesis_debug.h"
#include "lachesis_environment.h"
#include "lachesis_object.h"
#include "lachesis_type.h"

LObject* built_in(LObject* o, char* func);
LObject* built_in_op(lenv* e, LObject* o, char* op);
LObject* built_in_add(lenv* e, LObject* o);
LObject* built_in_sub(lenv* e, LObject* o);
LObject* built_in_mult(lenv* e, LObject* o);
LObject* built_in_div(lenv* e, LObject* o);

LObject* built_in_great_than(lenv* e, LObject* o);
LObject* built_in_great_and_equal(lenv* e, LObject* o);
LObject* built_in_less_than(lenv* e, LObject* o);
LObject* built_in_less_and_equal(lenv* e, LObject* o);
LObject* built_in_ord(lenv* e, LObject* o, char* op);
LObject* built_in_cmp(lenv* e, LObject* o, char* op);
LObject* built_in_euql(lenv* e, LObject* o);
LObject* built_in_not_euql(lenv* e, LObject* o);

LObject* built_in_head(lenv* e, LObject* o);
LObject* built_in_tail(lenv* e, LObject* o);
LObject* built_in_list(lenv* e, LObject* o);
LObject* built_in_eval(lenv* e, LObject* o);
LObject* built_in_join(lenv* e, LObject* o);
LObject* built_in_var(lenv* e, LObject* o, char* func);
LObject* built_in_define(lenv* e, LObject* o);
LObject* built_in_if(lenv* e, LObject* o);
LObject* built_in_put(lenv* e, LObject* o);
LObject* built_in_lambda(lenv* e, LObject* o);
LObject* built_in_load(lenv* e, LObject* o);

#endif // _LACHESIS_BUILTIN_
