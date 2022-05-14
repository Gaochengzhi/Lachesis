/**
 * File              : lachesis_environment.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 26.02.2022
 * Last Modified Date: 30.04.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */

#include "lachesis_environment.h"
#include "lachesis_builtin.h"
#include "lachesis_object.h"
#include "lachesis_type.h"
#include <stdlib.h>
#include <string.h>

lenv* lenv_new(void)
{
    lenv* e = malloc(sizeof(lenv));
    e->parent = NULL;
    e->count = 0;
    e->symbol_list = NULL;
    e->functor_list = NULL;
    return e;
}

void lenv_delete(lenv* e)
{
    for (int i = 0; i < e->count; ++i) {
        free(e->symbol_list[i]);
        lobj_delete(e->functor_list[i]);
    }

    free(e->symbol_list);
    free(e->functor_list);
    free(e);
}

lenv* lenv_copy(lenv* e)
{
    lenv* new_env = malloc(sizeof(lenv));
    new_env->parent = e->parent;
    new_env->count = e->count;
    new_env->symbol_list = malloc(sizeof(char*) * new_env->count);
    new_env->functor_list = malloc(sizeof(LObject*) * new_env->count);
    for (int i = 0; i < new_env->count; ++i) {
        new_env->symbol_list[i] = malloc(strlen(e->symbol_list[i]) + 1);
        strcpy(new_env->symbol_list[i], e->symbol_list[i]);
        new_env->functor_list[i] = lobj_copy(e->functor_list[i]);
    }
    return new_env;
}

/*get an copy of the LObject in lenv*/
LObject* lenv_get_copied_obj_from_env(lenv* mother_env, LObject* son_obj)
{

    for (int i = 0; i < mother_env->count; ++i)
        if (strcmp(mother_env->symbol_list[i], son_obj->symbol) == 0)
            return lobj_copy(mother_env->functor_list[i]);

    return (mother_env->parent)
        ? lenv_get_copied_obj_from_env(mother_env->parent, son_obj)
        : lobj_error("Undefined symbol:%s", son_obj->symbol);
}

void lenv_put_symbol(lenv* e, LObject* symbol_obj, LObject* functor)
{
    /*iterate e, if function's symbol already in e, delete the old one */
    /*and replace with the new one;*/
    for (int i = 0; i < e->count; ++i)
        if (strcmp(e->symbol_list[i], symbol_obj->symbol) == 0) {
            lobj_delete(e->functor_list[i]);
            e->functor_list[i] = lobj_copy(functor);
            return;
        }

    /*if not repeated found, allocate for new obj */
    e->count++;
    e->functor_list = realloc(e->functor_list, sizeof(LObject*) * e->count);
    e->symbol_list = realloc(e->symbol_list, sizeof(char*) * e->count);
    e->functor_list[e->count - 1] = lobj_copy(functor);
    e->symbol_list[e->count - 1] = malloc(strlen(symbol_obj->symbol) + 1);
    strcpy(e->symbol_list[e->count - 1], symbol_obj->symbol);
}

void lenv_add_builtin_func(lenv* e, char* symbol_name, builtin_func_type func)
{
    LObject* symbol_obj = lobj_symbol(symbol_name);
    LObject* functor = lobj_func(func);
    lenv_put_symbol(e, symbol_obj, functor);
    lobj_delete(symbol_obj);
    lobj_delete(functor);
}

void lenv_define(lenv* e, LObject* symbol_obj, LObject* functor)
{
    /*find e->parent's parent... */
    while (e->parent)
        e = e->parent;
    lenv_put_symbol(e, symbol_obj, functor);
}
void lenv_builtin_init_list(lenv* e)
{
    /*__BASIC_TYPES__*/
    lenv_add_builtin_func(e, "list", built_in_list);
    lenv_add_builtin_func(e, "head", built_in_head);
    lenv_add_builtin_func(e, "tail", built_in_tail);
    lenv_add_builtin_func(e, "eval", built_in_eval);
    lenv_add_builtin_func(e, "join", built_in_join);
    lenv_add_builtin_func(e, "loop", built_in_loop);

    lenv_add_builtin_func(e, "+", built_in_add);
    lenv_add_builtin_func(e, "-", built_in_sub);
    lenv_add_builtin_func(e, "*", built_in_mult);
    lenv_add_builtin_func(e, "/", built_in_div);

    /*__FUNCTION__*/
    lenv_add_builtin_func(e, "if", built_in_if);
    lenv_add_builtin_func(e, "\\", built_in_lambda);
    lenv_add_builtin_func(e, "def", built_in_define);
    lenv_add_builtin_func(e, "=", built_in_put);
    lenv_add_builtin_func(e, "==", built_in_euql);
    lenv_add_builtin_func(e, "!=", built_in_not_euql);
    lenv_add_builtin_func(e, ">", built_in_great_than);
    lenv_add_builtin_func(e, "<", built_in_less_than);
    lenv_add_builtin_func(e, ">=", built_in_great_and_equal);
    lenv_add_builtin_func(e, "<=", built_in_less_and_equal);

    /*__STRING_FUNCTION__*/
    lenv_add_builtin_func(e, "import", built_in_import);
    lenv_add_builtin_func(e, "error", built_in_error);
    lenv_add_builtin_func(e, "print", built_in_print);
    lenv_add_builtin_func(e, "fprint", built_in_fprint);

    /*__SYSTEM__*/

    // lenv_add_builtin_func(e, "import", built_in_import);
}
