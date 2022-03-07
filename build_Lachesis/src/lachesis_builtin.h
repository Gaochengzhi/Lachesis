/**
 * File              : lachesis_builtin.h
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 07.03.2022
 * Last Modified Date: 07.03.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */
#pragma once
#if !defined(_LACHESIS_BUILTIN_H)
#define _LACHESIS_BUILTIN_H

#include "lachesis_debug.h"
#include "lachesis_environment.h"
#include "lachesis_object.h"
#include "lachesis_type.h"

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
LObject* built_in_var(lenv* e, LObject* a, char* func);
LObject* built_in_define(lenv* e, LObject* a);
LObject* built_in_put(lenv* e, LObject* a);
LObject* built_in_lambda(lenv* e, LObject* a);

#endif // _LACHESIS_BUILTIN_
