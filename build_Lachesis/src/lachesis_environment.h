/**
 * File              : lachesis_environment.h
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 26.02.2022
 * Last Modified Date: 12.03.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */

#pragma once
#ifndef LACHESIS_ENVIRONMENT_H
#define LACHESIS_ENVIRONMENT_H
#include "../lib/mpc/mpc.h"
#include "lachesis_object.h"
#include "lachesis_type.h"
#include <stdio.h>
#include <string.h>

lenv* lenv_new(void);
lenv* lenv_copy(lenv* e);
void lenv_del(lenv* e);
void lenv_put(lenv* e, LObject* o, LObject* func_ptr);
void lenv_add_builtin_func(lenv* e, char* name, lbuiltin func);
void lenv_builtin_init_list(lenv* e);
void lenv_define(lenv* e, LObject* o, LObject* func);
LObject* lenv_get(lenv* e, LObject* o);

#endif // LACHESIS_ENVIRONMENT_H
